#include "lb_queue.h"
#include <stdio.h>

void display(lb_queue_t* queue);

void enqueue(lb_queue_t* queue, int item){

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
	queue->size++;

	pthread_mutex_unlock(&queue->lock);
}

int dequeue(lb_queue_t* queue){

	pthread_mutex_lock(&queue->lock);

	while (queue->size == 0) {
		pthread_mutex_unlock(&queue->lock);
		sched_yield();
		pthread_mutex_lock(&queue->lock);
	}

	node_t* old_head = queue->head;
	int item = old_head->item;
	queue->head = old_head->link;
	queue->size--;
	free(old_head);

	pthread_mutex_unlock(&queue->lock);
	return item;
}

void display(lb_queue_t* queue){
	node_t* tmp = queue->head;


	if (queue->head != NULL){
		printf("( ");
		while (tmp != NULL){ // why nonsense w/o queue->head != NULL?
			printf("%d ", tmp->item);
			tmp = tmp->link;
		}
		printf("), h: %d, t: %d, s: %d\n", queue->head->item, queue->tail->item, queue->size);
	} else {
		printf("(), h: NULL, t: NULL, s: 0\n");
	}
}

lb_queue_t create_queue(){
	lb_queue_t queue;
	queue.size = 0;
	queue.head = NULL;
	queue.tail = NULL;
	queue.enqueue = &enqueue;
	queue.dequeue = &dequeue;
	pthread_mutex_init(&queue.lock, NULL);
	return queue;
}
