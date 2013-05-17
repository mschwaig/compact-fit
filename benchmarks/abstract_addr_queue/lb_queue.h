
#ifndef LB_QUEUE_H_
#define LB_QUEUE_H_

#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct try_dequeue_ret_t {
	void **value;
	bool was_empty;
} try_dequeue_ret_t;


typedef struct node_t{
	void** item;
	struct node_t* link;
} node_t;

typedef struct lb_queue_t {
	node_t* head;
	node_t* tail;
	int size;
	pthread_mutex_t lock;

	void (*enqueue) (struct lb_queue_t*, void** item);
	void**  (*dequeue) (struct lb_queue_t*);
	void (*try_dequeue) (struct lb_queue_t*, struct try_dequeue_ret_t*);
} lb_queue_t;


void enqueue(lb_queue_t* queue, void** item);

/*
 * Blocks on empty queue.
 */
void** dequeue(lb_queue_t* queue);

/**
 * Retruns null on empty queue.
 */
void try_dequeue(lb_queue_t* queue, try_dequeue_ret_t* ret);
void init_lb_queue(lb_queue_t *q);

#endif /* LB_QUEUE_H_ */
