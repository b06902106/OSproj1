all: process scheduler

process:
	gcc process.c -o process

scheduler:
	gcc FIFO.c -o FIFO
	gcc RR.c -o RR
	gcc SJF.c heap.c -o SJF
	gcc PSJF.c heap.c -o PSJF
	gcc scheduler.c -o scheduler

clean:
	@rm process scheduler FIFO RR SJF PSJF
