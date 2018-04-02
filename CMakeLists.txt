cmake_minimum_required (VERSION 2.8.11)
project (Miles)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -std=c++14")
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#-------------------
# set common include folder for module
#-------------------

include_directories(include)
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/include)
include_directories(${COMMON_INCLUDES})

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cxx)
list(REMOVE_ITEM SRC_FILES ${PROJECT_SOURCE_DIR}/src/miles.cxx)

find_package(Tcmalloc)
find_package(Threads)
find_package(OpenSSL)

find_package( Boost 1.40 COMPONENTS program_options system filesystem regex iostreams REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

if(OPENSSL_FOUND)
  include_directories(${OPENSSL_INCLUDE_DIR})
endif()

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
  message("Found ccache ${CCACHE_FOUND}")
  message("Using ccache to speed up compilation")
  set(ENV{CCACHE_CPP2} "yes")
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

add_executable(miles src/miles.cxx ${SRC_FILES})
target_link_libraries(miles LINK_PUBLIC ${Boost_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY} ${Boost_IOSTREAMS_LIBRARY})

#
# Configure google test
#

add_subdirectory(${CMAKE_SOURCE_DIR}/modules/googletest
                 ${CMAKE_SOURCE_DIR}/modules/googletest
                 EXCLUDE_FROM_ALL)


#-------------------
# Test
#-------------------

enable_testing()

file(GLOB TEST_SRC_FILES ${CMAKE_SOURCE_DIR}/test/test_*.cxx)

add_executable(testall test/test_all.cxx ${TEST_SRC_FILES} ${SRC_FILES})
add_dependencies(testall "googletest.git")
target_link_libraries(testall gtest gtest_main ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY} ${Boost_IOSTREAMS_LIBRARY})
