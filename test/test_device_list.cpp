#include <gtest.h>

#include "../structs/device_list.h"
#include "../structs/omemo_device.h"

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


TEST(device_list, add_device)
{
	int retval;
	struct device_list *list;

	struct omemo_device *device = omemo_device_create("test", 1337);

	retval = omemo_device_list_add(&list, device);

	ASSERT_EQ(retval, 1);

	ASSERT_EQ(list->next, nullptr);
	ASSERT_EQ(list->device, device);

	omemo_device_list_free(&list);

	ASSERT_EQ(list, nullptr);
}
	
