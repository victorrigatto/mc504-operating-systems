/*
 * Implement a queue data structure for managing FIFO thread queue
 * in the mutex and conditional variables synchronization handlers
 * It works as a queue of endpoint_t.
 * The queue is implemented as a linked list
 */
#include "svar.h"

#ifndef _SYNCVAR_QUEUE_H
#define _SYNCVAR_QUEUE_H

typedef struct _node {
	endpoint_t value;
	struct _node *next;
} node_t;

typedef struct {
	node_t *head;
	node_t *tail;
} thread_queue_t;

/* initializes a new queue with given endpoint */
thread_queue_t init_queue(void);

/* enqueue a new endpoint into the queue */
int enqueue(thread_queue_t *queue, endpoint_t pt);

/*
 * Peek the next element on the queue.
 * Return 1 if succeeded, 0 if queue is empty or invalid.
 */
int peek(thread_queue_t *queue, endpoint_t *pt);

/*
 * dequeue the first endpoint inserted in the queue
 * that was not yet dequeued, it is stored in pt
 * Return 1 if succeeded, 0 if queue is empty or invalid.
 */
int dequeue(thread_queue_t *queue, endpoint_t *pt);
#endif
