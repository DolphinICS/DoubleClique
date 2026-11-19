#include <malloc.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>
#include "phase_2_queue.h"

// Single producer (ack thread), single consumer (phase 2 thread), both spin instead of sleeping
static queue_item_t *buffer;
static _Atomic size_t head;
static _Atomic size_t tail;

void enqueue(queue_item_t item) {
    size_t current_head = atomic_load_explicit(&head, memory_order_relaxed);
    size_t next_head = (current_head + 1) % QUEUE_SPACE;
    while (next_head == atomic_load_explicit(&tail, memory_order_acquire)) {
        _mm_pause();
    }
    buffer[current_head] = item;
    atomic_store_explicit(&head, next_head, memory_order_release);
}

queue_item_t dequeue(void) {
    size_t current_tail = atomic_load_explicit(&tail, memory_order_relaxed);
    while (current_tail == atomic_load_explicit(&head, memory_order_acquire)) {
        _mm_pause();
    }
    queue_item_t item = buffer[current_tail];
    atomic_store_explicit(&tail, (current_tail + 1) % QUEUE_SPACE, memory_order_release);
    return item;
}

void queue_init(void) {
    buffer = malloc(QUEUE_SPACE * sizeof(queue_item_t));
    if (buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    atomic_store(&head, 0);
    atomic_store(&tail, 0);
}
