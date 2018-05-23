#include "test_main.h"

TEST(pubsub, generate_device_list_stanza)
{
	const std::string expected_stanza {
		"<iq from=\"test@test.test\" type=\"set\" id=\"announce1\">\n"
		"  <pubsub>\n"
		"    <publish node=\"urn:xmpp:omemo:0:devicelist\">\n"
		"      <item>\n"
		"        <list xmlns=\"urn:xmpp:omemo:0\">\n"
		"          <device id=\"1000\"/>\n"
		"          <device id=\"1337\"/>\n"
		"        </list>\n"
		"      </item>\n"
		"    </publish>\n"
		"  </pubsub>\n"
		"</iq>"
	};

	char *device_list_stanza;
	struct device_list *list = NULL;
	struct omemo_device *device = NULL;
	struct omemo_device *inactive = NULL;

	device = omemo_device_create("test@test.test", 1337);
	inactive = omemo_device_create("test@test.test", 1000);
	inactive->status = INACTIVE;

	omemo_device_list_add(&list, device);
	omemo_device_list_add(&list, inactive);

	device_list_stanza = omemo_generate_device_list_stanza("test@test.test", list);

	ASSERT_STREQ(expected_stanza.c_str(), device_list_stanza);

	omemo_device_list_free(&list);
	free(device_list_stanza);
}

TEST(pubsub, process_device_list_stanza)
{
	const std::string incoming_stanza {
		"<iq from=\"test@test.test\" type=\"set\" id=\"announce1\">"
			"<pubsub>"
				"<publish node=\"urn:xmpp:omemo:0:devicelist\">"
					"<item>"
						"<list xmlns=\"urn:xmpp:omemo:0\">"
							"<device id=\"1000\"/>"
							"<device id=\"1337\"/>"
						"</list>"
					"</item>"
				"</publish>"
			"</pubsub>"
		"</iq>"
	};

	struct device_list *list;

	list = omemo_process_device_list_stanza(incoming_stanza.c_str());

	ASSERT_TRUE(list != NULL);
	ASSERT_EQ(list->device->address.device_id, 1337);
	ASSERT_STREQ(list->device->address.name, "test@test.test");
	ASSERT_EQ(list->next->device->address.device_id, 1000);
	ASSERT_STREQ(list->next->device->address.name, "test@test.test");
	
	omemo_device_list_free(&list);
}
