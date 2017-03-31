include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/ext/src/gtest/googletest/include/gtest")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/ext/src/gtest/googletest/include/gtest/internal")

add_executable (test_device_list test_device_list.cpp test_main.h)
target_link_libraries (
	test_device_list
	profanity-omemo
	libgtest
)
			
