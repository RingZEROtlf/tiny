#ifndef _LIBTINYCXX_TESTSUITE_THREAD_H
#define _LIBTINYCXX_TESTSUITE_THREAD_H

#include <thread>

#include <type_traits>

namespace tiny_test {

  template <class T>
    bool compare_type_to_native_type() {
      typedef T test_type;

      typedef typename T::native_handle_type native_handle;
      typedef typename std::conditional<
        std::is_same<test_type, tiny::thread>::value,
        native_handle,
        typename std::remove_pointer<native_handle>::type
      >::type native_type;

      int st = sizeof(test_type), snt = sizeof(native_type);
      int at = alignof(test_type), ant = alignof(native_type);

      return st == snt && at == ant;
    }

} // namespace tiny_test

#endif // _LIBTINYCXX_TESTSUITE_THREAD_H