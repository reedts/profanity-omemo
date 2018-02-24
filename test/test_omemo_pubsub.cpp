#include "test_main.h"

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

	omemo_publish_device_list_stanza("test@test.test", list);

	omemo_device_list_free(&list);
}
