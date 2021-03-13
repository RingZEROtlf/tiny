#include <gtest/gtest.h>

#include <chrono>
#include <system_error>

#include <thread>

TEST(ThisThreadTest, 1) {
  try {
    tiny::thread::id id = tiny::this_thread::get_id();
    EXPECT_NE(id, tiny::thread::id());
  } catch (const std::system_error&) {
    EXPECT_TRUE(false);
  } catch (...) {
    EXPECT_TRUE(false);
  }
}

TEST(ThisThreadTest, 2) {
  try {
    tiny::this_thread::yield();
  } catch (const std::system_error&) {
    EXPECT_TRUE(false);
  } catch (...) {
    EXPECT_TRUE(false);
  }
}

void foo3() {
  auto begin = std::chrono::system_clock::now();
  std::chrono::microseconds ms(500);

  tiny::this_thread::sleep_for(ms);

  EXPECT_GE(std::chrono::system_clock::now() - begin, ms);
}

TEST(ThisThreadTest, 3) {
  try {
    tiny::thread t(foo3);
    t.join();
  } catch (const std::system_error&) {
    EXPECT_TRUE(false);
  } catch (...) {
    EXPECT_TRUE(false);
  }
}

void foo4() {
  auto begin = std::chrono::system_clock::now();
  std::chrono::microseconds ms(500);

  tiny::this_thread::sleep_until(std::chrono::system_clock::now() + ms);

  EXPECT_GE(std::chrono::system_clock::now() - begin, ms);
}

TEST(ThisThreadTest, 4) {
  try {
    tiny::thread t(foo4);
    t.join();
  } catch (const std::system_error&) {
    EXPECT_TRUE(false);
  } catch (...) {
    EXPECT_TRUE(false);
  }
}

TEST(ThisThreadTest, InfiniteSleep) {
  auto now = std::chrono::system_clock::now();
  tiny::this_thread::sleep_until(now - 1ul * std::chrono::seconds(1));
}