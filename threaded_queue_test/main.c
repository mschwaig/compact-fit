
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lb_queue.h"

#define PRODUCER_THREADS 2
#define CONSUMER_THREADS 10 // only works with one for now
#define PRODUCE_PER_P_THREAD 10
#define CONSUME_PER_C_THREAD 2

void *produce(void *q){
  int i;
  lb_queue_t* queue = (lb_queue_t*) q;
  for(i=0; i < PRODUCE_PER_P_THREAD; i++){
    (*queue).enqueue(queue, i);
    printf("produced %d\n", i);
  }

  return NULL;
}

void *consume(void *q){
  int i, val;
  lb_queue_t* queue = (lb_queue_t*) q;
  for(i = 0; i < CONSUME_PER_C_THREAD; i++){
    val = (*queue).dequeue(queue);
    printf("consumed %d\n", val);
  }

  return NULL;
}

int main(int argc, char *argv[]){
  pthread_t producers[PRODUCER_THREADS];
  pthread_t consumers[CONSUMER_THREADS];

  void* p_status;
  void* c_status;

  long p_id, c_id;
  int rc;

  lb_queue_t init = create_queue();
  lb_queue_t* queue = &init; // = malloc(sizeof(lb_queue_t));
  // memcpy(queue, &init, sizeof(init));

  for(p_id = 0; p_id < PRODUCER_THREADS; p_id++){ // create producers
    rc =  pthread_create(&producers[p_id], NULL, produce, (void *)queue);
    if (rc){
      printf("failed to create producer thread %ld", p_id);
      exit(-1);
    }
  }

  for(c_id = 0; c_id < CONSUMER_THREADS; c_id++){ // create consumers
    rc =  pthread_create(&consumers[c_id], NULL, consume, (void *)queue);
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
