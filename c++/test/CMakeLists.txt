# Taken from http://google.github.io/googletest/quickstart-cmake.html

# General configuration
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

## Tests

#add_executable(
#  hello_test
#  hello_test.cpp
#)
#target_link_libraries(
#  hello_test
#  GTest::gtest_main
#)
#
#gtest_discover_tests(hello_test)

include_directories(../src/definitions)

#add_executable(
#  parallel_test
#  parallel_test.cpp
#)
#
#target_link_libraries(
#  parallel_test
#  GTest::gtest_main
#  libzstd_static
#)

#target_compile_options(parallel_test PRIVATE -g -mavx2)
#target_link_options(parallel_test PRIVATE -g)
#
#add_executable(
#  valgrind_tests
#  valgrind_tests.cpp
#)

#target_compile_options(valgrind_tests PRIVATE -g -mavx2)
#target_link_options(valgrind_tests PRIVATE -g)
#
#target_link_libraries(
#  valgrind_tests
#  libzstd_static
#)
#
#gtest_discover_tests(parallel_test)
