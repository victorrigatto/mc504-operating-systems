/* This file contains the main program of the queue for the SYNCVAR service. */

#include <stdlib.h>
#include "queue.h"

thread_queue_t init_queue() {
	thread_queue_t queue;
	queue.head = NULL;
	queue.tail = NULL;
	return queue;
}

/* inserir na fila */
int enqueue(thread_queue_t *queue, endpoint_t pt){
	node_t *node;
	int r;

	if (queue != NULL){
		node = malloc(sizeof(node_t));
		node->value = pt;
		node->next = NULL;

		if (queue->head == NULL){
			queue->head = node;
		}
		if (queue->tail == NULL){
			queue->tail = node;
		} else{
			queue->tail->next = node;
		}
		r = 1;
	} else{
		r = 0;
	}
	return r;
}

int peek(thread_queue_t *queue, endpoint_t *pt){
	int r = 0;
	if (queue->head != NULL) {
		*pt = queue->head->value;
		r = 1;
	}
	return r;
}

/* tira da fila */
int dequeue(thread_queue_t *queue, endpoint_t *pt){
	int r = 0;
	node_t *aux; /* auxiliar pointer to be able to free node memory */

	if (queue != NULL && pt != NULL){
		if (queue->head != NULL){
			*pt = queue->head->value;

			if (queue->head == queue->tail){
				queue->tail = NULL;
			}
			aux = queue->head;
			queue->head = queue->head->next;
			free(aux);
			r = 1;
		}
	}
	return r;
}
