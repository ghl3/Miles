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
list(REMOVE_ITEM SRC_FILES ${PROJECT_SOURCE_DIR}/src/miles.cxx) #"IlmImf/b44ExpLogTable.cpp")

#include_directories("include")
#file(GLOB SOURCES "src/*.cxx")
#link_directories("${DEPS_ROOT}/lib")

find_package(Tcmalloc)
find_package(Threads)
find_package(OpenSSL)

find_package( Boost 1.40 COMPONENTS program_options system filesystem regex iostreams REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )
#include_directories( ${Boost_INCLUDE_DIR} )

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

add_executable(miles src/miles.cxx ${SRC_FILES}) # include/sstable.h include/storable.h src/sstable.cxx include/key_map.h src/key_map.cxx include/table.h src/table.cxx include/wal.h src/wal.cxx include/zip.h include/fetchable.h)
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

file(GLOB TEST_SRC_FILES ${CMAKE_SOURCE_DIR}/test/*_test.cxx)

add_executable(testall test/all.cxx ${TEST_SRC_FILES} ${SRC_FILES}) #src/database.cxx include/utils.h include/sstable.h src/sstable.cxx include/table.h src/table.cxx src/key_map.cxx src/wal.cxx test/wal_test.cxx include/fetchable.h)
add_dependencies(testall "googletest.git")
target_link_libraries(testall gtest gtest_main ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY} ${Boost_IOSTREAMS_LIBRARY})
