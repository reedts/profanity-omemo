#include <libxml/tree.h>
#include "test_main.h"

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

TEST(device_list, deserialize)
{
	int retval;

	const std::string xml_input {
		"<list xmlns=\"urn:xmpp:omemo:0\">"
		"<device id=\"1000\"/>"
		"<device id=\"1337\"/>"
		"</list>"
	};

	struct device_list *list = NULL;
	
	xmlDocPtr doc;
	xmlNodePtr node;

	doc = xmlParseDoc(BAD_CAST xml_input.c_str());

	node = xmlDocGetRootElement(doc);

	retval = omemo_device_list_deserialize_xml(&list, node, "test@test.test");

	ASSERT_EQ(retval, 0);

	ASSERT_EQ(list->device->address.device_id, 1337);
	ASSERT_STREQ(list->device->address.name, "test@test.test");
	ASSERT_EQ(list->next->device->address.device_id, 1000);
	ASSERT_STREQ(list->next->device->address.name, "test@test.test");

	omemo_device_list_free(&list);
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
