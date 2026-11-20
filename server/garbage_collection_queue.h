#ifndef DOUBLECLIQUE_GARBAGE_COLLECTION_QUEUE_H
#define DOUBLECLIQUE_GARBAGE_COLLECTION_QUEUE_H

#include <stddef.h>
#include <stdbool.h>
#include <time.h>

#define QUEUE_SPACE 100000
#define NS_TO_COLLECTION 1000000

typedef struct {
    struct timespec t;
    void *buddy_allocated_addr;
} queue_item_t;

typedef struct {
    queue_item_t *buffer;
    size_t head;
    size_t tail;
} queue_t;

// Only the request region thread uses the queue: the buddy allocator is not thread safe

bool queue_empty(const queue_t *queue);
bool queue_full(const queue_t *queue);
void enqueue(queue_t *queue, queue_item_t item);
queue_item_t dequeue(queue_t *queue);
void queue_init(queue_t *queue);

#endif //DOUBLECLIQUE_GARBAGE_COLLECTION_QUEUE_H
