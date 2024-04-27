#include <array>
#include <cstdio>

template <class T, std::size_t N> struct ring_buffer {

  std::array<T, N> arr;
  std::size_t write_idx = 0; // Index of the next element to write (push)
  std::size_t read_idx = 0;  // Index of the next element to read (pop)
  std::size_t count = 0;     // Number of elements in the buffer
                             //
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

int main() {
  ring_buffer<int, 5> rb;

  for (int i = 0; i < 10; ++i) {
    rb.push(i);
  }

  while (!rb.is_empty()) {
    printf("%d\n", rb.pop());
  }

  return 0;
}
