cmake_minimum_required (VERSION 2.7)

project (profanity-omemo)

###############################################################################
# for testing
find_package (Threads REQUIRED)
include (ExternalProject)

ExternalProject_Add (
	gtest
	URL https://github.com/google/googletest/archive/master.zip
	PREFIX ${PROJECT_SOURCE_DIR}/test/ext
	INSTALL_COMMAND ""
)

# Get GTest informations
ExternalProject_Get_Property (gtest source_dir binary_dir)

add_library (libgtest IMPORTED STATIC GLOBAL)
add_dependencies (libgtest gtest)

# Set libgtest properties
set_target_properties(libgtest PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/googlemock/gtest/libgtest.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)
include_directories("${source_dir}/googletest/include"
                    "${source_dir}/googlemock/include")

###############################################################################

find_library (LIBSIGNAL libsignal-protocol-c.a)
find_package (LibXml2 REQUIRED)

include_directories (${LIBXML2_INCLUDE_DIR})

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/DEBUG)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/DEBUG)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -std=c99")

include_directories (${PROJECT_SOURCE_DIR})

include ("${PROJECT_SOURCE_DIR}/structs/CMakeLists.txt")
include ("${PROJECT_SOURCE_DIR}/xmpp/CMakeLists.txt")

add_library (
	profanity-omemo SHARED
	${STRUCTS_SRC}
	${XMPP_SRC}
)

target_link_libraries (profanity-omemo "${LIBSIGNAL}" "${LIBXML2_LIBRARIES}")

add_subdirectory ("${PROJECT_SOURCE_DIR}/test")
