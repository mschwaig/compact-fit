
#ifndef LB_QUEUE_H_
#define LB_QUEUE_H_

#define LB_QUEUE_POINTER_CHUNK_SIZE 4000

#include <pthread.h>
#include <stdlib.h>

typedef struct address_chunk_node_t {
	void **address[LB_QUEUE_POINTER_CHUNK_SIZE];
	struct address_chunk_node_t* link;
} address_chunk_node_t;

typedef struct lb_queue_t {
	struct address_chunk_node_t* head;
	struct address_chunk_node_t* tail;
	int size;
	pthread_mutex_t lock;

	void (*enqueue) (struct lb_queue_t*, address_chunk_node_t* chunk);
	address_chunk_node_t*  (*dequeue) (struct lb_queue_t*);
	address_chunk_node_t* (*try_dequeue) (struct lb_queue_t*);
} lb_queue_t;


void enqueue(lb_queue_t* queue, address_chunk_node_t* chunk);

/*
 * Blocks on empty queue.
 */
address_chunk_node_t* dequeue(lb_queue_t* queue);

/**
 * Returns NULL on empty queue.
 */
address_chunk_node_t* try_dequeue(lb_queue_t* queue);
void init_lb_queue(lb_queue_t *q);

#endif /* LB_QUEUE_H_ */
