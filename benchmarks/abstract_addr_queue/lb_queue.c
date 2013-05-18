#include "lb_queue.h"
#include <stdio.h>

void display(lb_queue_t* queue);

void enqueue(lb_queue_t* queue, address_chunk_node_t *chunk){

	pthread_mutex_lock(&queue->lock);

	chunk->link = NULL;

	if(queue->head == NULL){
		queue->head = chunk;
	} else {
		queue->tail->link = chunk;
	}
	queue->tail = chunk;
	queue->size = queue->size + 1;

	pthread_mutex_unlock(&queue->lock);
}

address_chunk_node_t* dequeue(lb_queue_t* queue){

	pthread_mutex_lock(&queue->lock);

	while (queue->size == 0) {
		pthread_mutex_unlock(&queue->lock);
		sched_yield();
		pthread_mutex_lock(&queue->lock);
	}

	address_chunk_node_t* old_head = queue->head;
	queue->head = old_head->link;
	queue->size--;

	pthread_mutex_unlock(&queue->lock);
	return old_head;
}

address_chunk_node_t* try_dequeue(lb_queue_t* queue){

	pthread_mutex_lock(&queue->lock);

	if (queue->size == 0) {
		pthread_mutex_unlock(&queue->lock);
		return NULL;
	}

	address_chunk_node_t* old_head = queue->head;
	queue->head = old_head->link;
	queue->size--;

	pthread_mutex_unlock(&queue->lock);
	return old_head;
}

void display(lb_queue_t* queue){
	address_chunk_node_t* tmp = queue->head;


	if (queue->head != NULL){
		printf("( ");
		while (tmp != NULL){
			printf("%p ", tmp->address);
			tmp = tmp->link;
		}
		printf("), h: %p, t: %p, s: %d\n", queue->head->address, queue->tail->address, queue->size);
	} else {
		printf("(), h: NULL, t: NULL, s: 0\n");
	}
}

void init_lb_queue(lb_queue_t *q){
	q->size = 0;
	q->head = NULL;
	q->tail = NULL;
	q->enqueue = &enqueue;
	q->dequeue = &dequeue;
	q->try_dequeue = &try_dequeue;
	pthread_mutex_init(&q->lock, NULL);
}
