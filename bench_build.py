#! /usr/bin/env python

from __future__ import print_function
from subprocess import call
import shutil

thread_count = 10

def parse_result(f):
	allocs = 0
	frees = 0
	compacts = 0
	t_number = 0
	f.seek(0)
	found = False
	for line in f:
		if line.startswith("TRDDTA"):
			splitline = line.strip().split("\t")
			allocs += int(splitline[2])
			compacts += int(splitline[3])
			frees += int(splitline[4])
	print(str(threads)+ "\t" + str(allocs) + "\t" + str(frees) + "\t" + str(compacts) + "\n", file=ncf_results)
shutil.rmtree("benchmarks/results/")
shutil.os.mkdir("benchmarks/results/")

ncf_results = open("benchmarks/results/ncf_t_results.log","w")
print("threads allocs frees compacts",file=ncf_results);
	

for threads in range(1,thread_count+1):
	call(["make","clean"])
	call(["make","CC=gcc-4.4","CPPFLAGS=-DNUM_BENCH_THREADS="+str(threads)+" -DSEC_BENCH_RUNTIME=1 -DREMOTE_FREE_T_LOCK"])
	f = open("benchmarks/results/page_"+str(threads)+"t.log","w+")
	call(["benchmarks/page"],stderr=f)
	parse_result(f)
	f.close()
	f = open("benchmarks/results/class_"+str(threads)+"t.log","w+")
	call(["benchmarks/class"],stderr=f)
	f.close()
ncf_results.close()