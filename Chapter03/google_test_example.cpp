#include <array>

#include <cstdio>

#include "gtest/gtest.h"

template <class T, std::size_t N> struct ring_buffer {

  std::array<T, N> arr;

  std::size_t write_idx = 0;

  std::size_t read_idx = 0;

  std::size_t count = 0;

  void push(T t) {

    arr.at(write_idx) = t;

    write_idx = (write_idx + 1) % N;

    if (count < N) {

      count++;

    } else {

      read_idx = (read_idx + 1) % N;
    }
  }

  T pop() {

    if (count == 0) {

      return T{};
    }

    T value = arr.at(read_idx);

    read_idx = (read_idx + 1) % N;

    --count;

    return value;
  }

  bool is_empty() const { return count == 0; }

  std::size_t get_count() const { return count; }
};

TEST(RingBufferInt, PushPop) {

  ring_buffer<int, 2> rb;

  rb.push(1);

  rb.push(2);

  EXPECT_EQ(rb.pop(), 1);

  EXPECT_EQ(rb.pop(), 2);
}

TEST(RingBufferInt, GetCount) {

  ring_buffer<int, 20> rb;

  for (int i = 0; i < 50; i++) {

    rb.push(i);
  }

  EXPECT_EQ(rb.get_count(), 20);

  for (int i = 0; i < 10; i++) {

    rb.pop();
  }

  EXPECT_EQ(rb.get_count(), 10);
}

int main() {

  testing::InitGoogleTest();

  return RUN_ALL_TESTS();
}
