
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lb_queue.h"

#define PRODUCER_THREADS 2
#define CONSUMER_THREADS 10 // only works with one for now
#define PRODUCE_PER_P_THREAD 10
#define CONSUME_PER_C_THREAD 2

typedef struct thread_args_t{
	struct lb_queue_t* queue;
	int thread_id;
} thread_args_t;

void *produce(void *args){
	thread_args_t* thread_args = args;
	int item_id;
	lb_queue_t* queue = thread_args->queue;
	int first_item_id = thread_args->thread_id*PRODUCE_PER_P_THREAD;
	for(item_id = first_item_id; item_id-first_item_id < PRODUCE_PER_P_THREAD; item_id++){
		(*queue).enqueue(queue, item_id);
		printf("produced %d\n", item_id);
	}

	return NULL;
}

void *consume(void *args){
	thread_args_t* thread_args = args;
	int i, item;
	lb_queue_t* queue = thread_args->queue;
	for(i = 0; i < CONSUME_PER_C_THREAD; i++){
		item = (*queue).dequeue(queue);
		printf("consumed %d\n", item);
	}

	return NULL;
}

int main(int argc, char *argv[]){
	pthread_t producers[PRODUCER_THREADS];
	pthread_t consumers[CONSUMER_THREADS];
	thread_args_t producer_args[PRODUCER_THREADS];
	thread_args_t consumer_args[CONSUMER_THREADS];

	void* p_status;
	void* c_status;

	long p_id, c_id;
	int rc;

	lb_queue_t init = create_queue();
	lb_queue_t* queue = &init; // = malloc(sizeof(lb_queue_t));
	// memcpy(queue, &init, sizeof(init));

	for(p_id = 0; p_id < PRODUCER_THREADS; p_id++){ // create producers
		producer_args[p_id].thread_id = p_id;
		producer_args[p_id].queue = queue;
		rc =  pthread_create(&producers[p_id], NULL, produce, (void *)&producer_args[p_id]);
		if (rc){
			printf("failed to create producer thread %ld", p_id);
			exit(-1);
		}
	}

	for(c_id = 0; c_id < CONSUMER_THREADS; c_id++){ // create consumers
		consumer_args[c_id].thread_id = c_id;
		consumer_args[c_id].queue = queue;
		rc =  pthread_create(&consumers[c_id], NULL, consume, (void *)&consumer_args[c_id]);
		if (rc){
			printf("failed to create consumer thread %ld", c_id);
			exit(-1);
		}
	}

	for(p_id = 0; p_id < PRODUCER_THREADS; p_id++){ // wait for producers
		pthread_join(producers[p_id], &p_status);
	}

	for(c_id = 0; c_id < CONSUMER_THREADS; c_id++){ // wait for consumers
		pthread_join(consumers[c_id], &c_status);
	}

	pthread_mutex_destroy(&queue->lock);
	//free(queue);
	printf("happy ending");

	return EXIT_SUCCESS;
}
