#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 20 // Total bytes available in the buffer
                       //
typedef struct {
  unsigned char data[BUFFER_SIZE]; // Array to store byte values
  size_t write_idx;                // Index of the next byte to write
  size_t read_idx;                 // Index of the next byte to read
  size_t count;     // Number of bytes currently used in the buffer
  size_t elem_size; // Size of each element in bytes
} ring_buffer;

void ring_buffer_init(ring_buffer *rb, size_t elem_size) {
  rb->write_idx = 0;
  rb->read_idx = 0;
  rb->count = 0;
  rb->elem_size = elem_size;
}

void ring_buffer_push(ring_buffer *rb, void *value) {
  if (rb->count + rb->elem_size <= BUFFER_SIZE) {
    rb->count += rb->elem_size;
  } else {
    rb->read_idx = (rb->read_idx + rb->elem_size) % BUFFER_SIZE;
  }
  memcpy(&rb->data[rb->write_idx], value, rb->elem_size);
  rb->write_idx = (rb->write_idx + rb->elem_size) % BUFFER_SIZE;
}

int ring_buffer_pop(ring_buffer *rb, void *value) {
  if (rb->count < rb->elem_size) {
    // Not enough data to pop
    return 0;
  }
  memcpy(value, &rb->data[rb->read_idx], rb->elem_size);
  rb->read_idx = (rb->read_idx + rb->elem_size) % BUFFER_SIZE;
  rb->count -= rb->elem_size;
  return 1; // Success
}

int main() {
  ring_buffer rb;
  ring_buffer_init(&rb, sizeof(int)); // Initialize buffer for int values
  for (int i = 0; i < 10; i++) {
    int val = i;
    ring_buffer_push(&rb, &val);
  }
  int pop_value;
  while (ring_buffer_pop(&rb, &pop_value)) {
    printf("%d\n", pop_value);
  }
  return 0;
}
