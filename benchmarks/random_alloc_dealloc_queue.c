
#include "bench.h"
#include "cf.h"
#include "arch_dep.h"
#include "threaded_queue_test/lb_queue.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

#define random rdtsc
#define STEPS 512

static int us_to_sleep = 0;
static int share = 0;

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

int bench_func(struct bench_stats *stats)
{
	void **mem_object;
	try_dequeue_ret_t ret;

	while (stats->run) {

		if (random() % 2){
			mem_object = cf_malloc(next_size());
			queue.enqueue(&queue, mem_object);
		} else {
			queue.try_dequeue(&queue, &ret);
			if (ret.was_empty){
				continue;
			}
			cf_free(ret.value);
		}
	}	

	return 0;
}

/* called prior thread creation*/
void set_block_size(int size)
{
	int i;

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

	init_lb_queue(&queue);
	printf("init done");
}

void set_sleep_time(int us)
{
	us_to_sleep = us;
}

