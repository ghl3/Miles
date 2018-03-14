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
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/include) # include/utils.h src/storage.cxx include/key_val_file.h include/results.h src/key_val_file.cxx test/key_val_file_test.cxx)

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

#link_directories(${Boost_LIBRARY_DIR})
#link_directories("${DEPS_ROOT}/lib64")

#add_executable(miles src/miles.cxx src/storage.cxx include/key_val_file.h src/key_val_file.cxx)
#target_link_libraries(miles LINK_PUBLIC ${Boost_LIBRARIES} )

add_executable(miles src/miles.cxx ${SRC_FILES}) # include/storage.h src/storage.cxx include/key_val_file.h src/key_val_file.cxx)
target_link_libraries(miles LINK_PUBLIC ${Boost_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} )

#-------------------
# Test
#-------------------
enable_testing()

file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/test/*_test.cxx)

add_executable(testall test/all.cxx ${TEST_SRC_FILES} src/storage.cxx src/key_val_file.cxx src/utils.cxx ) # ${COMMON_INCLUDES} src/storage.cxx include/key_val_file.h src/key_val_file.cxx)
add_dependencies(testall "googletest.git")
target_link_libraries(testall gtest gtest_main ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})

# from list of files we'll create tests test_name.cpp -> test_name
# foreach(_test_file ${TEST_SRC_FILES})
#     get_filename_component(_test_name ${_test_file} NAME_WE)
#     add_executable(${_test_name} ${_test_file} include/storage.h)
#     add_dependencies(${_test_name} "googletest.git")
#     target_link_libraries(${_test_name} gtest gtest_main ${CMAKE_THREAD_LIBS_INIT})
#     add_test(${_test_name} ${_test_name})
#     set_tests_properties(${_test_name} PROPERTIES TIMEOUT 5)
# endforeach()
