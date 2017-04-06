include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/ext/src/gtest/googletest/include/gtest")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/ext/src/gtest/googletest/include/gtest/internal")

add_executable (test_omemo test_omemo.cpp test_main.h)
target_link_libraries (
	test_omemo
	profanity-omemo
	libgtest
)
			
