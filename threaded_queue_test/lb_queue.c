#include "lb_queue.h"
#include <stdio.h>

void display(lb_queue_t* queue);

void enqueue(lb_queue_t* queue, void** item){

	pthread_mutex_lock(&queue->lock);

	node_t* n = (node_t*) malloc(sizeof(node_t));
	n->item = item;
	n->link = NULL;

	if(queue->head == NULL){
		queue->head = n;
	} else {
		queue->tail->link = n;
	}
	queue->tail = n;
	queue->size = queue->size + 1;

	pthread_mutex_unlock(&queue->lock);
}

void** dequeue(lb_queue_t* queue){

	pthread_mutex_lock(&queue->lock);

	while (queue->size == 0) {
		pthread_mutex_unlock(&queue->lock);
		sched_yield();
		pthread_mutex_lock(&queue->lock);
	}

	node_t* old_head = queue->head;
	void** item = old_head->item;
	queue->head = old_head->link;
	queue->size--;
	free(old_head);

	pthread_mutex_unlock(&queue->lock);
	return item;
}

void try_dequeue(lb_queue_t* queue, try_dequeue_ret_t* ret){

	pthread_mutex_lock(&queue->lock);

	if (queue->size == 0) {
		pthread_mutex_unlock(&queue->lock);
		ret->was_empty = true;
		ret->value = NULL;
		return;
	}

	node_t* old_head = queue->head;
	void** item = old_head->item;
	queue->head = old_head->link;
	queue->size--;
	free(old_head);

	pthread_mutex_unlock(&queue->lock);
	ret->was_empty = false;
	ret->value = item;
	return;
}

void display(lb_queue_t* queue){
	node_t* tmp = queue->head;


	if (queue->head != NULL){
		printf("( ");
		while (tmp != NULL){ // why nonsense w/o queue->head != NULL?
			printf("%p ", tmp->item);
			tmp = tmp->link;
		}
		printf("), h: %p, t: %p, s: %d\n", queue->head->item, queue->tail->item, queue->size);
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
