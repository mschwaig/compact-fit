SURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(patsubst %.c,%.o,$(SOURCES))
BINARIES = bench\
	   alloc_count\
	   memcpy_count\
	   none\
	   class\
	   global\
	   page\
	   thread\
	   single_malloc_mult_free\
	   mdrv_test\
	   mdrv_tlsf\
	   mdrv_share

BENCH_LIB = bench.o ../nb_stack.o ../page_stack.o ../aa_stack.o ../aa_bucket_stack.o abstract_addr_queue/lb_queue.o

CFLAGS = -pthread -m32 -O3 -DUSE_PRIVATE_ADDRESS -DUSE_STATS -Wall -I..
LDFLAGS = -pthread -m32 -L/usr/lib32/

all: $(BINARIES)

class:  rndm_alloc_queue_dequeue_dealloc.o $(BENCH_LIB) ../cf_class.o
	${CC} $(LDFLAGS)  -o $@ $< $(BENCH_LIB) ../cf_class.o

global: rndm_alloc_queue_dequeue_dealloc.o $(BENCH_LIB) ../cf_global.o
	${CC} $(LDFLAGS)  -o $@ $< $(BENCH_LIB) ../cf_global.o

page:   rndm_alloc_queue_dequeue_dealloc.o $(BENCH_LIB) ../cf_page.o
	${CC} $(LDFLAGS)  -o $@ $< $(BENCH_LIB) ../cf_page.o

thread: rndm_alloc_queue_dequeue_dealloc.o $(BENCH_LIB) ../cf_thread.o
	${CC} $(LDFLAGS)  -o $@ $< $(BENCH_LIB) ../cf_thread.o

none:   rndm_alloc_queue_dequeue_dealloc.o $(BENCH_LIB) ../cf_none.o
	${CC} $(LDFLAGS)  -o $@ $< $(BENCH_LIB) ../cf_none.o

single_malloc_mult_free: single_malloc_mult_free.o $(BENCH_LIB) ../cf_class.o
	${CC} $(LDFLAGS)  -o $@ $< $(BENCH_LIB) ../cf_class.o

bench: $(BENCH_LIB) $(HEADERS)
	${CC} $(LDFLAGS) -o $@ $(BENCH_LIB) ../cf_none.o

alloc_count: alloc_count.o $(BENCH_LIB) $(HEADERS) ../cf_none.o
	${CC} $(LDFLAGS)  -o $@ $< $(BENCH_LIB) ../cf_none.o

memcpy_count: memcpy_count.o $(BENCH_LIB) $(HEADERS)
	${CC} $(LDFLAGS) $(BENCH_LIB) -o $@ $< ../cf_none.o

mdrv_test: mdrv_test.o $(BENCH_LIB) $(HEADERS) ../cf_class.o
	${CC} $(LDFLAGS)  -o $@ $< $(BENCH_LIB) ../cf_class.o

mdrv_share: mdrv_share.o $(BENCH_LIB) $(HEADERS) ../cf_class.o
	${CC} $(LDFLAGS)  -o $@ $< $(BENCH_LIB) ../cf_class.o

tlsf.o: tlsf.c
	${CC} $(CFLAGS) -c -o $@ $< -DUSE_MMAP=1 -DUSE_SBRK=1

mdrv_tlsf: mdrv_tlsf.o $(BENCH_LIB) $(HEADERS) ../cf_none.o tlsf.o
	${CC} $(LDFLAGS)  -o $@ $< $(BENCH_LIB) ../cf_none.o tlsf.o

clean:
	@rm -f $(OBJECTS) $(BINARIES)

.PHONY: clean
