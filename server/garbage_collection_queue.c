#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "garbage_collection_queue.h"

bool queue_empty(const queue_t *queue) {
    return queue->head == queue->tail;
}

bool queue_full(const queue_t *queue) {
    return ((queue->head + 1) % QUEUE_SPACE) == queue->tail;
}

void enqueue(queue_t *queue, queue_item_t item) {
    queue->buffer[queue->head] = item;
    queue->head = (queue->head + 1) % QUEUE_SPACE;
}

queue_item_t dequeue(queue_t *queue) {
    queue_item_t item = queue->buffer[queue->tail];
    queue->tail = (queue->tail + 1) % QUEUE_SPACE;
    return item;
}

void queue_init(queue_t *queue) {
    queue->buffer = malloc(QUEUE_SPACE * sizeof(queue_item_t));
    if (queue->buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    queue->head = queue->tail = 0;
}
