
#include "bench.h"
#include "cf.h"
#include "arch_dep.h"
#include "abstract_addr_queue/lb_queue.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

#define random rdtsc
#define STEPS 512

#define BENCH_MEMORY_CACHE_SIZE 1000

static int us_to_sleep = 0;
static int share = 0;


typedef struct memory_cache_t {
	int index;
	address_chunk_node_t *cache[BENCH_MEMORY_CACHE_SIZE];
} memory_cache_t;

struct memory_cache_t **mem_cache;

#define NUM_BENCH 4494
static int alloc_sizes[NUM_BENCH];

#define NUM_CLASSES 10
static int class_map[] = {
	16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 16272,
};

static lb_queue_t queue;

static int get_alloc_size(int class)
{
	int retval;
	int rand_num;

	rand_num = random();

	if(class == 0) {
		retval = rand_num % class_map[class];
	} else {
		retval = class_map[class - 1] + rand_num % (class_map[class] - class_map[class - 1]);
	}
	return retval;
}

static int get_alloc_class()
{
#if 1
	int retval;
	retval = random() % NUM_BENCH;
	return alloc_sizes[retval];
#else
	return random() % NUM_CLASSES;
#endif
}

static int next_size()
{
	//return rdtsc() % 16272;

	return get_alloc_size(get_alloc_class());
}

inline address_chunk_node_t *cached_malloc(memory_cache_t *pmem_c){
	address_chunk_node_t *ret;
	if (pmem_c->index > 0) {
		pmem_c->index--;
		ret = pmem_c->cache[pmem_c->index];
		return ret;
	} else {
		return malloc(sizeof(address_chunk_node_t));
	}

}

inline void cached_free(address_chunk_node_t *chunk, memory_cache_t *pmem_c){
	if (pmem_c->index < BENCH_MEMORY_CACHE_SIZE){
		pmem_c->cache[pmem_c->index] = chunk;
		pmem_c->index = pmem_c->index + 1;
	} else {
		free(chunk);
	}
	
	return;
}

void fill_cache(memory_cache_t *pmem_c){
	address_chunk_node_t *chunk;
	while (pmem_c->index < BENCH_MEMORY_CACHE_SIZE){
		chunk = malloc(sizeof(address_chunk_node_t));
		cached_free(chunk, pmem_c);
	}
}

void empty_cache(memory_cache_t *pmem_c){
	address_chunk_node_t *chunk;
	while (pmem_c->index > 0){
		chunk = cached_malloc(pmem_c);
		free(chunk);
	}
}

int bench_func(struct bench_stats *stats)
{
	int enq_index = 0;
	int deq_index = LB_QUEUE_POINTER_CHUNK_SIZE;
	memory_cache_t *mem_c;
	address_chunk_node_t *enq_chunk;
	address_chunk_node_t *deq_chunk;
	address_chunk_node_t *tmp_chunk;
	mem_c = mem_cache[stats->id];

	enq_chunk = cached_malloc(mem_c);
	deq_chunk = cached_malloc(mem_c);

	while (stats->run) {

		if (random() % 2){
allocate:
			if (enq_index >= LB_QUEUE_POINTER_CHUNK_SIZE){
				queue.enqueue(&queue, enq_chunk);
				enq_chunk = cached_malloc(mem_c);
				enq_index = 0;
			}

			enq_chunk->address[enq_index] = cf_malloc(next_size());
			enq_index++;
		} else {
			if (deq_index >= LB_QUEUE_POINTER_CHUNK_SIZE){
				tmp_chunk = queue.try_dequeue(&queue);
				if (tmp_chunk == NULL){
					goto allocate;
				} else {
					cached_free(deq_chunk, mem_c);
					deq_chunk = tmp_chunk;
					deq_index = 0;
				}
			}

			cf_free(deq_chunk->address[deq_index]);
			deq_index++;
		}
	}	

	return 0;
}


/* called prior thread creation*/
void set_block_size(int size)
{
	int i;
	int num_threads = get_num_threads();

	share = size;
	printf("use share value %d\n", share);
	for(i=0; i<NUM_BENCH; ++i)
	{
		if (i<NUM_CLASSES) {
			alloc_sizes[i] = 0;
		} else if (i < 108) {
			alloc_sizes[i] = 1;
		} else if (i < 4433) {
			alloc_sizes[i] = 2;
		} else if (i < 4451) {
			alloc_sizes[i] = 3;
		} else if (i < 4462) {
			alloc_sizes[i] = 4;
		} else if (i < 4475) {
			alloc_sizes[i] = 5;
		} else if (i < 4480) {
			alloc_sizes[i] = 6;
		} else if (i < 4488) {
			alloc_sizes[i] = 7;
		} else {
			alloc_sizes[i] = 8;
		}
	}

	printf("filling cache\n");

	mem_cache = malloc(num_threads*sizeof(memory_cache_t));	

	for (i = 0; i < num_threads; i++){
		mem_cache[i] = malloc(sizeof(memory_cache_t));
		mem_cache[i]->index = 0;
		fill_cache(mem_cache[i]);
	}

	init_lb_queue(&queue);
}

void set_sleep_time(int us)
{
	us_to_sleep = us;
}

