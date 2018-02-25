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
	dir += std::string("/") + std::string(OMEMO_WORKING_DIR);
	dir += std::string("/") + std::string(nick.name) + std::string("/");
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
	dir += std::string("/") + std::string(OMEMO_WORKING_DIR);
	dir += std::string("/") + std::string(nick.name) + std::string("/");
	dir += "/contacts/notTest@test.test"; dir += "/devices";
	
	
	ASSERT_TRUE(!omemo_store_device_list(&nick, list));
	std::fstream f(dir.c_str());
	ASSERT_TRUE(f.good());

	std::string content((std::istreambuf_iterator<char>(f)),
			    std::istreambuf_iterator<char>());
	ASSERT_TRUE(content == result);

	omemo_device_list_free(&list);
}

TEST(store, is_local_user_existant)
{
	signal_protocol_address nick = {"test", 4, 1337};
	signal_protocol_address false_nick = {"notTest", 7, 1000};
	struct device_list *list = NULL;
	struct omemo_device *device = NULL;

	device = omemo_device_create("test@test.test", 1337);
	
	omemo_device_list_add(&list, device);

	ASSERT_TRUE(!omemo_store_device_list(&nick, list));
	ASSERT_TRUE(omemo_is_local_user_existent(&nick) == 1);
	ASSERT_TRUE(omemo_is_local_user_existent(&false_nick) == 0);

	omemo_device_list_free(&list);
}
