#include <stdio.h>
#include <string.h>
// Macro to declare ring buffer type and functions for a specific type and size
#define DECLARE_RING_BUFFER(TYPE, SIZE)                                        \
  typedef struct {                                                             \
    TYPE data[SIZE];                                                           \
    size_t write_idx;                                                          \
    size_t read_idx;                                                           \
    size_t count;                                                              \
  } ring_buffer_##TYPE##_##SIZE;                                               \
  void ring_buffer_init_##TYPE##_##SIZE(ring_buffer_##TYPE##_##SIZE *rb) {     \
    rb->write_idx = 0;                                                         \
    rb->read_idx = 0;                                                          \
    rb->count = 0;                                                             \
  }                                                                            \
  void ring_buffer_push_##TYPE##_##SIZE(ring_buffer_##TYPE##_##SIZE *rb,       \
                                        TYPE value) {                          \
    rb->data[rb->write_idx] = value;                                           \
    rb->write_idx = (rb->write_idx + 1) % SIZE;                                \
    if (rb->count < SIZE) {                                                    \
      rb->count++;                                                             \
    } else {                                                                   \
      rb->read_idx = (rb->read_idx + 1) % SIZE;                                \
    }                                                                          \
  }                                                                            \
  int ring_buffer_pop_##TYPE##_##SIZE(ring_buffer_##TYPE##_##SIZE *rb,         \
                                      TYPE *value) {                           \
    if (rb->count == 0) {                                                      \
      return 0; /* Buffer is empty */                                          \
    }                                                                          \
    *value = rb->data[rb->read_idx];                                           \
    rb->read_idx = (rb->read_idx + 1) % SIZE;                                  \
    rb->count--;                                                               \
    return 1; /* Success */                                                    \
  }                                                                            \

// Example usage with int type and size 5
DECLARE_RING_BUFFER(int, 5) // Declare the ring buffer type and functions for ints
                            //
int main() {
  ring_buffer_int_5 rb;
  ring_buffer_init_int_5(&rb); // Initialize the ring buffer
  // Push values into the ring buffer
  for (int i = 0; i < 10; ++i) {
    ring_buffer_push_int_5(&rb, i);
  }
  // Pop values from the ring buffer and print them
  int value;
  while (ring_buffer_pop_int_5(&rb, &value)) {
    printf("%d\n", value);
  }
  return 0;
}
