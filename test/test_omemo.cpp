#include <gtest.h>
#include <gcrypt.h>
#include <signal/signal_protocol.h>

#include <crypto/crypto_provider.h>
#include <store/store_io.h>
#include <structs/device_list.h>
#include <structs/omemo_device.h>
#include <xmpp/pubsub.h>

#include "test_main.h"

TEST(omemo_device, creation)
{
	const char *jid = "test@testserver.test";
	const uint32_t id = 1337;

	struct omemo_device *test_device;

	test_device = omemo_device_create(jid, id);

	ASSERT_STREQ(test_device->jid, jid);
	ASSERT_EQ(test_device->id, id);

	omemo_device_free(test_device);
}


TEST(device_list, add)
{
	int retval;
	struct device_list *list = NULL;

	struct omemo_device *device = omemo_device_create("test", 1337);

	retval = omemo_device_list_add(&list, device);

	ASSERT_EQ(retval, 1);

	ASSERT_EQ(list->next, nullptr);
	ASSERT_EQ(list->device, device);

	omemo_device_list_free(&list);

	ASSERT_EQ(list, nullptr);
}

TEST(device_list, size)
{
	int retval;
	struct device_list *list = NULL;
	struct omemo_device *st_device = omemo_device_create("test", 1337);
	struct omemo_device *nd_device = omemo_device_create("test", 1338);

	retval = omemo_device_list_size(&list);

	ASSERT_EQ(retval, 0);

	omemo_device_list_add(&list, st_device);
	retval = omemo_device_list_size(&list);
	ASSERT_EQ(retval, 1);

	omemo_device_list_add(&list, nd_device);
	retval = omemo_device_list_size(&list);
	ASSERT_EQ(retval, 2);

	omemo_device_list_free(&list);
	ASSERT_EQ(list, nullptr);
}

TEST(device_list, contains)
{
	int retval;
	struct device_list *list = NULL;
	struct omemo_device *in_list = omemo_device_create("test", 1337);
	struct omemo_device *nin_list = omemo_device_create("test2", 1338);

	omemo_device_list_add(&list, in_list);

	retval = omemo_device_list_contains(&list, in_list);

	ASSERT_EQ(retval, 1);

	retval = omemo_device_list_contains(&list, nin_list);

	ASSERT_EQ(retval, 0);

	omemo_device_list_free(&list);
	omemo_device_free(nin_list);
}

TEST(device_list, contains_id)
{

	int retval;
	struct device_list *list = NULL;
	struct omemo_device *in_list = omemo_device_create("test", 1337);
	struct omemo_device *nin_list = omemo_device_create("test2", 1338);

	omemo_device_list_add(&list, in_list);

	retval = omemo_device_list_contains_id(&list, 1337);

	ASSERT_EQ(retval, 1);

	retval = omemo_device_list_contains_id(&list, 1338);

	ASSERT_EQ(retval, 0);

	omemo_device_list_free(&list);
	omemo_device_free(nin_list);
}

TEST(device_list, remove)
{
	int retval;
	struct device_list *list = NULL;

	struct omemo_device *in_list = omemo_device_create("test", 1337);

	omemo_device_list_add(&list, in_list);
	
	ASSERT_EQ(omemo_device_list_contains(&list, in_list), 1);
	ASSERT_EQ(omemo_device_list_size(&list), 1);

	omemo_device_list_remove(&list, in_list);
	ASSERT_EQ(omemo_device_list_contains(&list, in_list), 0);
	ASSERT_EQ(omemo_device_list_size(&list), 0);

	omemo_device_free(in_list);
	omemo_device_list_free(&list);
}

TEST(device_list, remove_free_device)
{
	int retval;
	struct device_list *list = NULL;

	struct omemo_device *in_list = omemo_device_create("test", 1337);

	omemo_device_list_add(&list, in_list);
	
	ASSERT_EQ(omemo_device_list_contains(&list, in_list), 1);
	ASSERT_EQ(omemo_device_list_size(&list), 1);

	omemo_device_list_free_device(&list, in_list);
	ASSERT_EQ(omemo_device_list_contains(&list, in_list), 0);
	ASSERT_EQ(omemo_device_list_size(&list), 0);

	omemo_device_list_free(&list);
}

TEST(device_list, free)
{
	struct device_list *list = NULL;
	struct omemo_device *in_list = omemo_device_create("test", 1337);

	omemo_device_list_add(&list, in_list);

	ASSERT_EQ(omemo_device_list_contains(&list, in_list), 1);

	omemo_device_list_free(&list);
	ASSERT_EQ(omemo_device_list_contains(&list, in_list), 0);
	ASSERT_EQ(list, nullptr);
}

TEST(device_list, inval_input)
{
	struct device_list *list = NULL;
	struct omemo_device *in_list = omemo_device_create("test", 1337);

	ASSERT_EQ(omemo_device_list_add(NULL, in_list), -1);
	ASSERT_EQ(omemo_device_list_add(&list, NULL), -1);

	ASSERT_EQ(omemo_device_list_contains(NULL, in_list), -1);
	ASSERT_EQ(omemo_device_list_contains(&list, NULL), -1);

	ASSERT_EQ(omemo_device_list_contains_id(NULL, 1), -1);
	ASSERT_EQ(omemo_device_list_contains_id(&list, 0), -1);

	ASSERT_EQ(omemo_device_list_remove(NULL, in_list), -1);
	ASSERT_EQ(omemo_device_list_remove(&list, NULL), -1);

	ASSERT_EQ(omemo_device_list_size(NULL), 0);
	
	ASSERT_EQ(omemo_device_list_free_device(NULL, in_list), -1);
	ASSERT_EQ(omemo_device_list_free_device(&list, NULL), -1);

	omemo_device_list_free(&list);
	omemo_device_free(in_list);
}

TEST(pubsub, xml_output)
{
	struct device_list *list = NULL;
	struct omemo_device *device = NULL;
	struct omemo_device *inactive = NULL;

	device = omemo_device_create("test@test.test", 1337);
	inactive = omemo_device_create("test@test.test", 1000);
	inactive->status = INACTIVE;

	omemo_device_list_add(&list, device);
	omemo_device_list_add(&list, inactive);

	omemo_publish_device_list("test@test.test", &list);

	if (omemo_store_device_list(list) < 0) {
		perror("omemo_store_device_list");
	}

	omemo_device_list_free(&list);
}

TEST(crypto, sha512)
{
	const char *test = "Hello, OMEMO!";
	const char *hash = "2725b70070ea46affe1520598ef78cb5ec97a1ac6c4e75c9f2ab9ad7d4dddfc83ddea9dd802ef68a10bf746e23fdcb8145d6a9a6fa8cf1c8c335b97612821ee2";

	const uint8_t testdata[] = {
		0x02, 0x3f, 0x93, 0xfe, 0x1e
	};
	uint8_t *output = NULL;
	uint8_t result[64];
	signal_buffer *buffer = NULL;;	
	size_t buffer_len;
	void *context = NULL;

	omemo_init_provider();

	omemo_sha512_digest_init(&context, NULL);
	omemo_sha512_digest_update(context, (uint8_t *)test, strlen(test), NULL);
	omemo_sha512_digest_final(context, &buffer, NULL);
	omemo_sha512_digest_cleanup(context, NULL);

	memset(result, 0x0, 64);
	
	output = signal_buffer_data(buffer);

	gcry_md_hash_buffer(GCRY_MD_SHA512, result, test, strlen(test));
	
	ASSERT_EQ(memcmp(result, output, strlen(test)), 0);

	signal_buffer_free(buffer);
}

