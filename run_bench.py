#! /usr/bin/env python

from __future__ import print_function
from subprocess import call
import os

thread_count = 2
runs = 3

def record_mem_usage_results(f_run_results, fname, threads, r):
	f_run_results.seek(0)
	mem_file = None
	for line in f_run_results:
		if line.startswith("== run"):
			splitline = line.strip().split(" ")
			run = int(splitline[2])
			if mem_file:
				mem_file.close() 
			mem_file = open("benchmarks/results/" + fname + "r" + str(run) + "_mem.log","w")
		elif line.startswith("MEMDTA"):
			mem_file.write(line.replace("MEMDTA","").strip() + "\n")
	if not mem_file.closed:
		mem_file.close()

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
	print(str(t_number)+ "\t" + str(allocs/runs) + "\t" + str(frees/runs) + "\t" + str(compacts/runs) + "\n", file=f_results)

def bench_run(locking_scheme, private):
	default_cmd = ["benchmarks/" + str(locking_scheme),"--partial-comp","-1"] 
	if (private):
		default_cmd.append("-p")
		fname = str(locking_scheme) + "_p_"
	else:
		fname = str(locking_scheme) + "_g_"
	f_results = open("benchmarks/results/" + fname + "results.log","w")
	print("# threads allocs frees compacts",file=f_results);
	for threads in range(1,thread_count+1):
		command = default_cmd[:]
		command.extend(["-n",str(threads)])
		f_run_results = open("benchmarks/results/" + fname + str(threads) + "t.log","w+")
		for r in range(runs):
			f_run_results.write("== run " + str(r + 1) + " ==\n")
			f_run_results.flush()
			call(command, stderr=f_run_results)
			f_run_results.flush()
		if threads == thread_count:
			record_mem_usage_results(f_run_results, fname, threads, r)
		parse_run_result(f_run_results, f_results, threads)
		f_run_results.close()
	f_results.close()

res_folder = "benchmarks/results/"

if not os.path.exists(res_folder):
	os.makedirs(res_folder)

for tmp in os.listdir(res_folder):	 # clean up result folder
	tmp_path = os.path.join(res_folder, tmp)
	try:
		os.unlink(tmp_path)
	except Exxception, e:
		print(e)

if(call(["make","clean"]) != 0):
	raise error("clean failed")
if(call(["make","CC=gcc-4.4"]) != 0):
	raise error("make failed")


bench_run("global", False)
bench_run("class", False)
bench_run("page", False)
bench_run("thread", True)
bench_run("class", True)
bench_run("page", True)
