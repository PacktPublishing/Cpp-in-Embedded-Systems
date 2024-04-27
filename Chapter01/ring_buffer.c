#include <stdio.h>

#define BUFFER_SIZE 5

typedef struct {
  int arr[BUFFER_SIZE]; // Array to store int values directly
  size_t write_idx;     // Index of the next element to write (push)
  size_t read_idx;      // Index of the next element to read (pop)
  size_t count;         // Number of elements in the buffer
} int_ring_buffer;

void int_ring_buffer_init(int_ring_buffer *rb) {
  rb->write_idx = 0;
  rb->read_idx = 0;
  rb->count = 0;
}

void int_ring_buffer_push(int_ring_buffer *rb, int value) {
  rb->arr[rb->write_idx] = value;
  rb->write_idx = (rb->write_idx + 1) % BUFFER_SIZE;
  if (rb->count < BUFFER_SIZE) {
    rb->count++;
  } else {
    // Buffer is full, move read_idx forward
    rb->read_idx = (rb->read_idx + 1) % BUFFER_SIZE;
  }
}

int int_ring_buffer_pop(int_ring_buffer *rb) {
  if (rb->count == 0) {
    return 0;
  }
  int value = rb->arr[rb->read_idx];
  rb->read_idx = (rb->read_idx + 1) % BUFFER_SIZE;
  rb->count--;
  return value;
}

int main() {
  int_ring_buffer rb;
  int_ring_buffer_init(&rb);
  for (int i = 0; i < 10; i++) {
    int_ring_buffer_push(&rb, i);
  }
  while (rb.count > 0) {
    int value = int_ring_buffer_pop(&rb);
    printf("%d\n", value);
  }
  return 0;
}
