#! /usr/bin/env python

from __future__ import print_function
from subprocess import call
import os

thread_count = 10

def parse_run_result(f_run_results, f_results, t_number):
	allocs = 0
	frees = 0
	compacts = 0
	f_run_results.seek(0)
	found = False
	for line in f_run_results:
		if line.startswith("TRDDTA"):
			splitline = line.strip().split("\t")
			allocs += int(splitline[2])
			compacts += int(splitline[3])
			frees += int(splitline[4])
	print(str(t_number)+ "\t" + str(allocs) + "\t" + str(frees) + "\t" + str(compacts) + "\n", file=f_results)

def bench_run(locking_scheme):
	f_results = open("benchmarks/results/" + str(locking_scheme) + "_results.log","w")
	print("threads allocs frees compacts",file=f_results);
	for threads in range(1,thread_count+1):
		f_run_results = open("benchmarks/results/" + str(locking_scheme) + "_" + str(threads) + "t.log","w+")
		call(["benchmarks/" + str(locking_scheme),"-n",str(threads)],stderr=f_run_results)
		parse_run_result(f_run_results, f_results, threads)
		f_run_results.close()
	f_results.close()

res_folder = "benchmarks/results/"

for tmp in os.listdir(res_folder):	 # clean up result folder
	tmp_path = os.path.join(res_folder, tmp)
	try:
		os.unlink(tmp_path)
	except Exxception, e:
		print(e)

call(["make","clean"])
call(["make","CC=gcc-4.4"])


bench_run("global")
bench_run("thread")
bench_run("class")
bench_run("page")
