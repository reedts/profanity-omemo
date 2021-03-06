#include "test_main.h"

TEST(omemo_device, creation)
{
	const char *jid = "test@testserver.test";
	const uint32_t id = 1337;

	struct omemo_device *test_device;

	test_device = omemo_device_create(jid, id);

	ASSERT_STREQ(test_device->address.name, jid);
	ASSERT_EQ(test_device->address.device_id, id);

	omemo_device_free(test_device);
}
