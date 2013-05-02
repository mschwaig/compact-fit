
#ifndef LB_QUEUE_H_
#define LB_QUEUE_H_

#include <pthread.h>
#include <stdlib.h>

typedef struct node_t{
	int item;
	struct node_t* link;
} node_t;

typedef struct lb_queue_t {
	node_t* head;
	node_t* tail;
	int size;
	pthread_mutex_t lock;

	void (*enqueue) (struct lb_queue_t*, int item);
	int  (*dequeue) (struct lb_queue_t*);
} lb_queue_t;

void enqueue(lb_queue_t* queue, int item);
int dequeue(lb_queue_t* queue);
lb_queue_t create_queue();

#endif /* LB_QUEUE_H_ */
