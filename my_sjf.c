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
#include "my_sjf.h"

typedef struct _id{
	int tsk_id, arr, rem;
} tid;
typedef struct _seq{
	int tsk_id, rem;
} seq;

int cmp_sjf(const void *a ,const void *b){
	if((*(tid *)a).arr == (*(tid *)b).arr) return (*(tid *)a).rem > (*(tid *)b).rem ? 1 : -1;
	else return (*(tid *)a).arr > (*(tid *)b).arr ? 1 : -1;
}

void my_sjf(TSK *tsk, int n){
	tid id[1005];
	for(int i = 0 ; i < n ; i++){
		id[i].tsk_id = i;
		id[i].arr = tsk[i].arr;
		id[i].rem = tsk[i].rem;
	}
	qsort(id, n ,sizeof(id[0]), cmp_sjf);

	int t = 0, len = 0, m = 0, q_len = 0, inq[1005] = {0};
	seq ord[1005], lst[1005];

	for(int i = 0; i < n; i ++){
		if(q_len == 0 && t < id[i].arr){
			ord[m ++] = (seq){-1, id[i].arr - t};
			t = id[i].arr;
		}
		for(int j = 0; j < n; j ++){
			if(inq[j] == 0 && id[j].arr <= t){
				inq[j] = 1;
				q_len ++;
			}
		}
		int tar_id = -1;
		for(int j = 0; j < n; j ++){
			if(inq[j] == 1 && (tar_id == -1 || id[j].rem < id[tar_id].rem ||
			(id[j].rem == id[tar_id].rem && id[j].arr < id[tar_id].arr))){
				tar_id = j;
			}
		}
		ord[m ++] = (seq){id[tar_id].tsk_id, id[tar_id].rem};
		t += id[tar_id].rem;
		inq[tar_id] = -1;
		q_len --;
	}

	t = 0;
	for(int i = 0; i < m; i ++){
		lst[len ++] = ord[i];
		for(int j = 0, pre = t; j < n; j ++){
			if(t <= id[j].arr && id[j].arr < t + ord[i].rem){
				lst[len - 1].rem = id[j].arr - pre;
				lst[len ++] = (seq){id[j].tsk_id, -1};
				lst[len ++] = (seq){ord[i].tsk_id, t + tsk[ord[i].tsk_id].rem - id[j].arr};
				pre = id[j].arr;
			}
		}
		t += ord[i].rem;
	}

	for(int i = 0; i < len; i ++){
		if(lst[i].tsk_id == -1){
			for(int t = 0 ; t < lst[i].rem; t++){
				volatile unsigned long k;
				for(k = 0; k < 1000000UL; k++);
			}
#ifdef DEBUG
			printf("[$] %d\n", lst[i].rem);
			fflush(stdout);
#endif
		} else if(lst[i].rem == -1){
			make(&tsk[lst[i].tsk_id]);
		} else if(lst[i].rem > 0){
			run(&tsk[lst[i].tsk_id], lst[i].rem);
		}		
	}
}
