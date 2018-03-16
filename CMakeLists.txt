cmake_minimum_required (VERSION 2.8.9)
project (Miles)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -std=c++14")
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

get_filename_component(DEPS_ROOT "${PROJECT_BINARY_DIR}/deps" ABSOLUTE)

include(ExtProjectUtils)

# Will download external CMakeable project from git repo, branch "master" and install it in $DEPS_ROOT
# This also will create "googletest.git" target, which we'll use as dependency for our test project
ExtProjectGit("https://github.com/google/googletest.git" "master" ${DEPS_ROOT} CMAKE_ARGS "${_OPT_CMAKE_ARGS}")

#-------------------
# set common include folder for module
#-------------------
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/include) # include/utils.h src/storage.cxx include/key_val_file.h include/results.h  test/key_val_file_test.cxx)

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cxx)

include_directories("include")
include_directories("${DEPS_ROOT}/include")
include_directories(${COMMON_INCLUDES})

file(GLOB SOURCES "src/*.cxx")
link_directories("${DEPS_ROOT}/lib")

find_package(Tcmalloc)
find_package(Threads)
find_package(OpenSSL)

find_package( Boost 1.40 COMPONENTS program_options system filesystem REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )
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

add_executable(miles src/miles.cxx ${SRC_FILES} include/sstable.h include/key_storage.h src/sstable.cxx include/key_map.h src/key_map.cxx include/hybrid_key_storage.h src/hybrid_key_storage.cxx)

target_link_libraries(miles LINK_PUBLIC ${Boost_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} )

#-------------------
# Test
#-------------------
enable_testing()

file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/test/*_test.cxx)

add_executable(testall test/all.cxx ${TEST_SRC_FILES} src/storage.cxx src/utils.cxx include/sstable.h src/sstable.cxx include/hybrid_key_storage.h src/hybrid_key_storage.cxx src/key_map.cxx) # ${COMMON_INCLUDES} src/storage.cxx include/key_val_file.h)
add_dependencies(testall "googletest.git")
target_link_libraries(testall gtest gtest_main ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})
