#include <gtest/gtest.h>
#include <pthread.h>

#include <thread>
#include <thread/all.hh>

#include <iostream>
#include <system_error>
#include <functional>
#include <atomic>
#include <chrono>

TEST(ThreadTest, ctor_1) {
  try {
    tiny::thread t;
    EXPECT_TRUE(!t.joinable());
  } catch (const std::system_error&) {
    EXPECT_TRUE(false);
  } catch (...) {
    EXPECT_TRUE(false);
  }
}

void free_function(tiny::thread::id& id) {
  id = tiny::this_thread::get_id();
}

TEST(ThreadTest, ctor_2) {
  try {
    tiny::thread::id id1;
    tiny::thread t(free_function, std::ref(id1));
    tiny::thread::id id2 = t.get_id();
    EXPECT_TRUE(t.joinable());
    t.join();
    EXPECT_TRUE(!t.joinable());
    EXPECT_TRUE(id1 == id2);
  } catch (const std::system_error&) {
    EXPECT_TRUE(false);
  } catch (...) {
    EXPECT_TRUE(false);
  }
}

struct copyable : public std::unary_function<tiny::thread::id&, void> {
  copyable() = default;
  ~copyable() = default;
  copyable(const copyable& c) { ++copy_count; }

  void operator()(tiny::thread::id& id) const {
    id = tiny::this_thread::get_id();
  }

  static int copy_count;
};

int copyable::copy_count = 0;

TEST(ThreadTest, ctor_3) {
  try {
    tiny::thread::id t1_id1;
    copyable c1;
    tiny::thread t1(std::ref(c1), std::ref(t1_id1));
    tiny::thread::id t1_id2 = t1.get_id();
    EXPECT_TRUE(t1.joinable());
    t1.join();
    EXPECT_TRUE(!t1.joinable());
    EXPECT_TRUE(t1_id1 == t1_id2);
    EXPECT_TRUE(copyable::copy_count == 0);

    tiny::thread::id t2_id1;
    copyable c2;
    tiny::thread t2(c2, std::ref(t2_id1));
    tiny::thread::id t2_id2 = t2.get_id();
    EXPECT_TRUE(t2.joinable());
    t2.join();
    EXPECT_TRUE(!t2.joinable());
    EXPECT_TRUE(t2_id1 == t2_id2);
    EXPECT_TRUE(copyable::copy_count > 0);
  } catch (const std::system_error&) {
    EXPECT_TRUE(false);
  } catch (...) {
    EXPECT_TRUE(false);
  }
}

struct nocopyable : std::unary_function<tiny::thread::id&, void> {
  nocopyable() = default;
  ~nocopyable() = default;
  nocopyable(const nocopyable&) = delete;
  nocopyable& operator=(const nocopyable&) = delete;
  void operator()(tiny::thread::id& id) const {
    id = tiny::this_thread::get_id();
  }
};

TEST(ThreadTest, ctor_4) {
  try {
    tiny::thread::id t1_id1;
    nocopyable nc1;
    tiny::thread t1(std::ref(nc1), std::ref(t1_id1));
    tiny::thread::id t1_id2 = t1.get_id();
    EXPECT_TRUE(t1.joinable());
    t1.join();
    EXPECT_TRUE(!t1.joinable());
    EXPECT_TRUE(t1_id1 == t1_id2);

    tiny::thread::id t2_id1;
    nocopyable nc2;
    tiny::thread t2(std::cref(nc2), std::ref(t2_id1));
    tiny::thread::id t2_id2 = t2.get_id();
    EXPECT_TRUE(t2.joinable());
    t2.join();
    EXPECT_TRUE(!t2.joinable());
    EXPECT_TRUE(t2_id1 == t2_id2);
  } catch (const std::system_error&) {
    EXPECT_TRUE(false);
  } catch (...) {
    EXPECT_TRUE(false);
  }
}

struct nonconst : public std::unary_function<tiny::thread::id&, void> {
  void operator()(tiny::thread::id& id) {
    id = tiny::this_thread::get_id();
  }
};

TEST(ThreadTest, ctor_5) {
  try {
    tiny::thread::id t1_id1;
    nonconst c1;
    tiny::thread t1(std::ref(c1), std::ref(t1_id1));
    tiny::thread::id t1_id2 = t1.get_id();
    EXPECT_TRUE(t1.joinable());
    t1.join();
    EXPECT_TRUE(!t1.joinable());
    EXPECT_TRUE(t1_id1 == t1_id2);

    tiny::thread::id t2_id1;
    nonconst c2;
    tiny::thread t2(c2, std::ref(t2_id1));
    tiny::thread::id t2_id2 = t2.get_id();
    EXPECT_TRUE(t2.joinable());
    t2.join();
    EXPECT_TRUE(!t2.joinable());
    EXPECT_TRUE(t2_id1 == t2_id2);
  } catch (const std::system_error&) {
    EXPECT_TRUE(false);
  } catch (...) {
    EXPECT_TRUE(false);
  }
}

bool f_was_called = false;

void f() {
  f_was_called = true;
}

TEST(ThreadTest, ctor_6) {
  try {
    tiny::thread t(f);
    t.join();
    EXPECT_TRUE(f_was_called);
  } catch (const std::system_error&) {
    EXPECT_TRUE(false);
  } catch (...) {
    EXPECT_TRUE(false);
  }
}

bool functor_was_called = false;

struct copyable_7 {
  copyable_7() = default;
  ~copyable_7() = default;
  copyable_7(const copyable_7&) = default;
  copyable_7& operator=(const copyable_7&) = default;

  void operator()() const {
    functor_was_called = true;
  }
};

TEST(ThreadTest, ctor_7) {
  try {
    copyable_7 c;
    copyable_7& rc = c;
    tiny::thread t(rc);
    t.join();
    EXPECT_TRUE(functor_was_called);
  } catch (const std::system_error&) {
    EXPECT_TRUE(false);
  } catch (...) {
    EXPECT_TRUE(false);
  }
}

bool functor_was_called_8 = false;

struct moveable {
  moveable() = default;
  ~moveable() = default;
  moveable(const moveable&) = delete;
  moveable& operator=(const moveable&) = delete;
  moveable(moveable&&) { }

  void operator()() const {
    functor_was_called_8 = true;
  }
};

TEST(ThreadTest, ctor_8) {
  try {
    moveable m;
    tiny::thread t(std::move(m));
    t.join();
    EXPECT_TRUE(functor_was_called_8);
  } catch (const std::system_error&) {
    EXPECT_TRUE(false);
  } catch (...) {
    EXPECT_TRUE(false);
  }
}

int total = 0;

// functor has internal state
struct  moveable_9 {
  int i;

  moveable_9() = default;
  ~moveable_9() = default;
  moveable_9(const moveable_9&) = delete;
  moveable_9& operator=(const moveable_9&) = delete;

  moveable_9(int j) : i(j) { }
  moveable_9(moveable_9&& m) : i(m.i) { }

  void operator()() const {
    total += i;
  }
};

TEST(ThreadTest, ctor_9) {
  try {
    // first
    moveable_9 m1(60);
    tiny::thread t1(std::move(m1));
    t1.join();
    EXPECT_TRUE(total == 60);

    // second
    moveable_9 m2(600);
    tiny::thread t2(std::move(m2));
    t2.join();
    EXPECT_EQ(total, 660);
  } catch (const std::system_error&) {
    EXPECT_TRUE(false);
  } catch (...) {
    EXPECT_TRUE(false);
  }
}

TEST(ThreadTest, id_hash) {
  std::hash<tiny::thread::id> h1;
}

TEST(ThreadTest, id_operators) {
  tiny::thread::id id1;
  tiny::thread::id id2;

  id1 == id2;
  id1 != id2;
  id1 < id2;
  id1 > id2;
  id1 <= id2;
  id1 >= id2;
}

void swap_f() { }

TEST(ThreadTest, swap_1) {
  try {
    tiny::thread t1(swap_f);
    tiny::thread::id t1_id = t1.get_id();
    
    tiny::thread t2;

    t2.swap(t1);
    EXPECT_EQ(t1.get_id(), tiny::thread::id());
    EXPECT_EQ(t2.get_id(), t1_id);

    t2.join();
  } catch (const std::system_error&) {
    EXPECT_TRUE(false);
  } catch (...) {
    EXPECT_TRUE(false);
  }

  try {
    tiny::thread t1(swap_f);
    tiny::thread::id t1_id = t1.get_id();
    
    tiny::thread t2;

    tiny::swap(t1, t2);
    EXPECT_EQ(t1.get_id(), tiny::thread::id());
    EXPECT_EQ(t2.get_id(), t1_id);

    t2.join();
  } catch (const std::system_error&) {
    EXPECT_TRUE(false);
  } catch (...) {
    EXPECT_TRUE(false);
  }
}

void members_f() { }

TEST(ThreadTest, members_1) {
  try {
    tiny::thread t(f);
    EXPECT_TRUE(t.joinable());
    t.join();
    EXPECT_TRUE(!t.joinable());
  } catch (const std::system_error&) {
    EXPECT_TRUE(false);
  } catch (...) {
    EXPECT_TRUE(false);
  }
}

TEST(ThreadTest, members_2) {
  try {
    tiny::thread t(f);
    EXPECT_TRUE(t.joinable());
    t.detach();
    EXPECT_TRUE(!t.joinable());
  } catch (const std::system_error&) {
    EXPECT_TRUE(false);
  } catch (...) {
    EXPECT_TRUE(false);
  }
}

TEST(ThreadTest, members_3) {
  try {
    tiny::thread t(f);
    EXPECT_NE(t.get_id(), tiny::thread::id());
    t.join();
    EXPECT_EQ(t.get_id(), tiny::thread::id());
  } catch (const std::system_error&) {
    EXPECT_TRUE(false);
  } catch (...) {
    EXPECT_TRUE(false);
  }
}

TEST(ThreadTest, members_4) {
  bool test = false;
  tiny::thread t;
  try {
    t.join();
  } catch (const std::system_error&) {
    test = true;
  }
  EXPECT_TRUE(test);
}

TEST(ThreadTest, members_5) {
  bool test = false;
  tiny::thread t;
  try {
    t.detach();
  } catch (const std::system_error&) {
    test = true;
  }
  EXPECT_TRUE(test);
}

TEST(ThreadTest, members_hardware_concurrency) {
  EXPECT_GE(tiny::thread::hardware_concurrency(), 1);
}

// void native_handle_f(std::atomic<bool>& started) {
//   started = true;
//   while (true) {
//     tiny::this_thread::sleep_for(std::chrono::milliseconds(100));
//   }
// }

// TEST(ThreadTest, native_handle_cancel) {
//   std::atomic<bool> started { false };
//   tiny::thread t(native_handle_f, std::ref(started));
//   while (!started) {
//     tiny::this_thread::sleep_for(std::chrono::milliseconds(100));
//   }
//   pthread_cancel(t.native_handle());
//   t.join();
// }

TEST(ThreadTest, native_handle_typesizes) {
  EXPECT_TRUE(tiny_test::compare_type_to_native_type<tiny::thread>());
}