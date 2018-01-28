include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/ext/src/gtest/googletest/include/gtest")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/ext/src/gtest/googletest/include/gtest/internal")


add_executable (test_omemo_crypto test_omemo_crypto.cpp test_main.h)
target_link_libraries (
	test_omemo_crypto
	profanity-omemo
	libgtest
)

add_executable (test_omemo_device test_omemo_device.cpp test_main.h)
target_link_libraries (
	test_omemo_device
	profanity-omemo
	libgtest
)

add_executable (test_omemo_device_list test_omemo_device_list.cpp test_main.h)
target_link_libraries (
	test_omemo_device_list
	profanity-omemo
	libgtest
)

add_executable (test_omemo_pubsub test_omemo_pubsub.cpp test_main.h)
target_link_libraries (
	test_omemo_pubsub
	profanity-omemo
	libgtest
)

add_executable (test_omemo_signal test_omemo_signal.cpp test_main.h)
target_link_libraries (
	test_omemo_signal
	profanity-omemo
	libgtest
)

add_executable (test_omemo_store test_omemo_store.cpp test_main.h)
target_link_libraries (
	test_omemo_store
	profanity-omemo
	libgtest
)

add_test(NAME test_omemo_crypto COMMAND test_omemo_crypto)
add_test(NAME test_omemo_device COMMAND test_omemo_device)
add_test(NAME test_omemo_device_list COMMAND test_omemo_device_list)
add_test(NAME test_omemo_pubsub COMMAND test_omemo_pubsub)
add_test(NAME test_omemo_signal COMMAND test_omemo_signal)
add_test(NAME test_omemo_store COMMAND test_omemo_store)
