#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <wait.h>
#include <fcntl.h>
#include <errno.h>
#include <sched.h>
#include <assert.h>
#include "control.h"

int _fd = 0, *_T = 0;

void make(TSK *tsk){
	// printf("make %s\n", tsk -> name);
	// fflush(stdout);
	syscall(334, &(tsk -> st.tv_sec), &(tsk -> st.tv_nsec));
	tsk -> pid = fork();
	int PID = tsk -> pid;
	if(PID < 0) perror("[Parent] fork error");
	else if(PID == 0){
		fflush(stdout);
		char tim[105];
		sprintf(tim, "%d", tsk -> rem);
		execlp("./task", "./task", tsk -> name, tim, (char *)0);
	}
}

void run(TSK *tsk, int run_time){
	if(run_time > tsk -> rem){
		perror("run time > task time");
		return;
	}
	tsk -> rem -= run_time;
	*_T = run_time;
	struct sched_param param;
	param.sched_priority = 3;
	if(sched_setparam(tsk -> pid, &param) == -1) perror("[Parent] child set param error");
	if(tsk -> rem == 0){
		syscall(334, &(tsk -> ed.tv_sec), &(tsk -> ed.tv_nsec));
		char s[205];
		sprintf(s, "[Project1] %d %lu.%09lu, %lu.%09lu", tsk -> pid, tsk -> st.tv_sec, tsk -> st.tv_nsec, tsk -> ed.tv_sec, tsk -> ed.tv_nsec);
		syscall(333, s);
	}
}

void clear(TSK *tsk){
	waitpid(tsk -> pid, NULL, 0);
}
