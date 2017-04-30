#include "test_main.h"

TEST(store, store_user_device_list)
{
	signal_protocol_address nick = {"test", 4, 1337};
	struct device_list *list = NULL;
	struct omemo_device *device = NULL;
	struct omemo_device *inactive = NULL;

	device = omemo_device_create("test@test.test", 1337);
	inactive = omemo_device_create("test@test.test", 1000);
	inactive->status = INACTIVE;

	omemo_device_list_add(&list, device);
	omemo_device_list_add(&list, inactive);
	
	std::string result("A:1337\nI:1000");
	std::string dir(std::getenv("HOME"));
	dir += OMEMO_WORKING_DIR;
	dir += std::string(nick.name) + std::string("/");
	dir += std::to_string(nick.device_id);
	dir += "/devices";
	
	ASSERT_TRUE(!omemo_store_device_list(&nick, list));
	std::fstream f(dir.c_str());
	ASSERT_TRUE(f.good());

	std::string content((std::istreambuf_iterator<char>(f)),
			    std::istreambuf_iterator<char>());
	ASSERT_TRUE(content == result);

	omemo_device_list_free(&list);
}

TEST(store, store_contacts_device_list)
{
	signal_protocol_address nick = {"test", 4, 1337};
	struct device_list *list = NULL;
	struct omemo_device *device = NULL;
	struct omemo_device *inactive = NULL;

	device = omemo_device_create("notTest@test.test", 1337);
	inactive = omemo_device_create("notTest@test.test", 1000);
	inactive->status = INACTIVE;

	omemo_device_list_add(&list, device);
	omemo_device_list_add(&list, inactive);
	
	std::string result("A:1337\nI:1000");
	std::string dir(std::getenv("HOME"));
	dir += OMEMO_WORKING_DIR;
	dir += std::string(nick.name) + std::string("/");
	dir += std::to_string(nick.device_id);
	dir += "/contacts/notTest@test.test"; dir += "/devices";

	ASSERT_TRUE(!omemo_store_device_list(&nick, list));
	std::fstream f(dir.c_str());
	ASSERT_TRUE(f.good());

	std::string content((std::istreambuf_iterator<char>(f)),
			    std::istreambuf_iterator<char>());
	ASSERT_TRUE(content == result);

	omemo_device_list_free(&list);

}
