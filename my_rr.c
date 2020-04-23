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
#include <fcntl.h>
#include <errno.h>
#include <sched.h>
#include <assert.h>
#include "control.h"
#include "my_rr.h"

int rr_cmp(const void *p1, const void *p2) {
	if(((TSK_rr*)p1) -> tsk.arr < ((TSK_rr*)p2) -> tsk.arr) return -1;
	else if(((TSK_rr*)p1) -> tsk.arr == ((TSK_rr*)p2) -> tsk.arr){
		if(((TSK_rr*)p1) -> id < ((TSK_rr*)p2) -> id) return -1;
		else if(((TSK_rr*)p1) -> id == ((TSK_rr*)p2) -> id) return 0;
		else return 1;
	}
	else return 1;
}
void my_rr(TSK *tsk, int n){
	TSK_rr lst[1005];
	for (int i = 0; i < n; i ++){
		lst[i].tsk = tsk[i];
		lst[i].id = i;
	}
	int id[1005];
	qsort(lst, n, sizeof(TSK_rr), rr_cmp);
	int t = 0;
	int ptr = 0, ll = 0, rr = n-1, num = 0;
	while(ptr != n || num){
		if(!num){
			int tt = lst[ptr].tsk.arr - t;
			t = lst[ptr].tsk.arr;
#ifdef DEBUG
			if(tt){
				printf("[$] %d\n", tt);
				fflush(stdout);
			}
#endif
			for(int i = 0; i < tt; i ++){
				volatile unsigned long k;
				for(k = 0; k < 1000000UL; k ++);
			}
			make(&tsk[lst[ptr].id]);
			rr ++;
			if(rr == n) rr = 0;
			id[rr] = ptr;
			ptr ++;
			num ++;
		}
		int p = id[ll];
		int la = tsk[lst[p].id].rem;
		if (la > 500) la = 500;
		while(ptr != n && t + la >= tsk[lst[ptr].id].arr){
			int used = tsk[lst[ptr].id].arr - t;
			if(used){
				run(&tsk[lst[p].id], used);
				la -= used;
				t += used;
			}
			make(&tsk[lst[ptr].id]);
			rr ++;
			if(rr == n) rr = 0;
			id[rr] = ptr;
			ptr ++;
			num ++;
		}
		if(la){ 
			run(&tsk[lst[p].id], la);
			t += la;
		}
		ll ++;
		if(ll == n) ll = 0;
		num --;
		if(tsk[lst[p].id].rem){
			rr ++;
			if(rr == n) rr = 0;
			id[rr] = p;
			num ++;
		}
	}
}
