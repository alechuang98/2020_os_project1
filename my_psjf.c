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
#include "my_psjf.h"
#define intmax 2147483647;

int cmp(const void *a, const void *b){
	if(((TSK *)a)->arr - ((TSK *)b)->arr > 0) return 1;
	else if(((TSK *)a)->arr - ((TSK *)b)->arr < 0) return -1;
	else{
		return ((TSK *)a)->rem - ((TSK *)b)->rem;
	}
}

void my_psjf(TSK *tsk,int n){
	TSK id[1005];
	int appear[1005] = {0};
	for(int i = 0; i < n; i ++){
		id[i].arr = tsk[i].arr;
		id[i].rem = tsk[i].rem;
		id[i].pid = i;
	}
	int cnt = 0;
	int lst_cnt = 0;
	int nxt = n;
	qsort(id,n,sizeof(TSK),cmp);
	int now = id[0].arr;
	int lst[4000][2];
	for(int i = 1; i < n; i ++){
		if(id[i].arr != id[0].arr){
			nxt = i;
			break;
		}
	}
	id[n].arr = intmax;
	if(now != 0){
		lst[lst_cnt][0] = -1;
		lst[lst_cnt ++][1] = now;
	}
	while(1){
		int min_index = -1, min_time = intmax;
		for(int i = 0; i < n; i ++){
			if(id[i].rem > 0){
				if(id[i].arr < id[nxt].arr){
					if(min_time > id[i].rem){
						min_time = id[i].rem;
						min_index = i;
					}
				}
			}
			if(now == id[i].arr && !appear[i]){
				lst[lst_cnt][0] = id[i].pid;
				lst[lst_cnt++][1] = -1;
				appear[i] = 1;
			}
		}
		if(min_index == -1){
			lst[lst_cnt][0] = -1;
			lst[lst_cnt++][1] = id[nxt].arr - now;
			for(int i = nxt + 1; i <= n; i ++){
				if(id[i].arr != id[nxt].arr){
					nxt = i;
					break;
				}
			}
			continue;
		}
		if(now + min_time >= id[nxt].arr){
			min_time = id[nxt].arr - now;
			for(int i = nxt + 1; i <= n; i ++){
				if(id[i].arr != id[nxt].arr){
					nxt = i;
					break;
				}
			}
		}
		lst[lst_cnt][0] = id[min_index].pid;
		lst[lst_cnt ++][1] = min_time;
		id[min_index].rem -= min_time;
		now += min_time;
		if(id[min_index].rem == 0) cnt ++;
		if(cnt == n) break;
	}
	for(int i = 0; i < lst_cnt; i ++){
		if(lst[i][0] == -1){
			for(int t = 0 ; t < lst[i][1]; t ++){
			volatile unsigned long k;
				for (k = 0; k < 1000000UL; k ++);
			}
		} else if(lst[i][1] == -1){
			make(&tsk[lst[i][0]]);
		} else {
			run(&tsk[lst[i][0]], lst[i][1]);
		}
	}
	return;
}
