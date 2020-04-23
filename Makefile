all: main.c control.c my_fifo.c my_sjf.c my_psjf.c my_rr.c task.c
	gcc -o main main.c control.c my_fifo.c my_sjf.c my_psjf.c my_rr.c -lrt -std=c11
	gcc -o task task.c -lrt -std=c11

debug: main.c control.c my_fifo.c my_sjf.c my_psjf.c my_rr.c task.c
	gcc -o main main.c control.c my_fifo.c my_sjf.c my_psjf.c my_rr.c -lrt -std=c11 -DDEBUG
	gcc -o task task.c -lrt -std=c11 -DDEBUG


clear:
	rm -f main task
