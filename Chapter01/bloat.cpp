#include <array>
#include <cstdio>

#if USE_TEMPLATES
template <class T, std::size_t N> struct ring_buffer {
  std::array<T, N> arr;
  std::size_t write_idx = 0; // Index of the next element to write (push)
  std::size_t read_idx = 0;  // Index of the next element to read (pop)
  std::size_t count = 0;     // Number of elements in the buffer

  void push(T t) {
    arr.at(write_idx) = t;
    write_idx = (write_idx + 1) % N;
    if (count < N) {
      count++;
    } else {
      // buffer is full, move forward read_idx
      read_idx = (read_idx + 1) % N;
    }
  }

  T pop() {
    if (count == 0) {
      // Buffer is empty, return a default-constructed T.
      return T{};
    }
    T value = arr.at(read_idx);
    read_idx = (read_idx + 1) % N;
    --count;
    return value;
  }

  bool is_empty() const { return count == 0; }
};

#else

#define N 10

struct ring_buffer_int {
  std::array<int, N> arr;
  std::size_t write_idx = 0; // Index of the next element to write (push)
  std::size_t read_idx = 0;  // Index of the next element to read (pop)
  std::size_t count = 0;     // Number of elements in the buffer

  void push(int t) {
    arr.at(write_idx) = t;
    write_idx = (write_idx + 1) % N;
    ++count;
  }

  int pop() {
    if (count == 0) {
      // Buffer is empty, return a default-constructed T.
      return int{};
    }
    int value = arr.at(read_idx);
    read_idx = (read_idx + 1) % N;
    --count;
    return value;
  }
};

struct ring_buffer_float {
  std::array<float, N> arr;
  std::size_t write_idx = 0; // Index of the next element to write (push)
  std::size_t read_idx = 0;  // Index of the next element to read (pop)
  std::size_t count = 0;     // Number of elements in the buffer

  void push(float t) {
    arr.at(write_idx) = t;
    write_idx = (write_idx + 1) % N;
    ++count;
  }

  float pop() {
    if (count == 0) {
      // Buffer is empty, return a default-constructed T.
      return float{};
    }
    float value = arr.at(read_idx);
    read_idx = (read_idx + 1) % N;
    --count;
    return value;
  }
};
#endif

int main() {
#ifdef USE_TEMPLATES
  ring_buffer<int, 10> buffer1;
  ring_buffer<float, 10> buffer2;
#else
  ring_buffer_int buffer1;
  ring_buffer_float buffer2;
#endif
  for (int i = 0; i < 20; i++) {
    buffer1.push(i);
    buffer2.push(i + 0.2f);
  }

  for (int i = 0; i < 10; i++) {
    printf("%d, %.2f\r\n", buffer1.pop(), buffer2.pop());
  }

  return 0;
}
