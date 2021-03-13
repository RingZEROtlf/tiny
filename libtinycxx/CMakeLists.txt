option(build_tests "Build all of libtinyc++'s own tests." ON)

cmake_minimum_required(VERSION 3.9)
cmake_policy(SET CMP0048 NEW)
project(libtinycxx VERSION 0.1.0 LANGUAGES CXX)

set(CXX_FLAGS
  -Wall -Wextra -Werror
  -std=c++11)

if (build_tests)
  enable_testing()

  find_package(GTest REQUIRED)
  # include_directories(${GTEST_INCLUDE_DIR})
  include_directories(./include)
  include_directories(./testsuite/utils)

  find_package(Threads REQUIRED)

  add_executable(libtinycxx_test
    testsuite/libtinycxx_test_main.cc

    # 30_threads
    testsuite/30_threads/this_thread.cc
    testsuite/30_threads/thread.cc)

  target_link_libraries(libtinycxx_test
    gtest Threads::Threads)
endif()