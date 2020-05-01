#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "include/priority.h"
#define MAX_PROCESS_NUM 100
#define MAX_PROCESS_NAME 5
#define TIME_UNIT 1000000UL
typedef struct input{
    int process_id;
    char N[MAX_PROCESS_NAME];
    unsigned int R;
    unsigned int T;
}Input;
Input p[MAX_PROCESS_NUM];
int num_process;
pid_t pid[MAX_PROCESS_NUM];
int now;
int ptrRun;
int running;
void create_process(pid_t *pid, char *Name, int ID, int time){
    if ((*pid = fork()) < 0) fprintf(stderr, "fork() error");
    if (*pid == 0) {
        char exec_time[20], id[10];
        sprintf(exec_time, "%u", time);
        sprintf(id, "%u", ID);
        execlp("./process", "./process", Name, id, exec_time, NULL);
    }
    if (*pid > 0) {
        fflush(stderr);
        SET_PRIORITY(*pid, SCHED_FIFO, PRIORITY_LOW);
        USE_CPU(*pid, 1);
    }
}
int cmp(const void* a, const void *b){
    Input c = *(Input *)a;
    Input d = *(Input *)b;
    if (c.R < d.R) return -1;
    else if (c.R == d.R) return 0;
    else return 1;
}
void sig_child(int signo){
    static int num_finish = 0;
    wait(NULL);
    num_finish+=1;
    ptrRun+=1;
    running = 0;
    if (num_finish == num_process) exit(0);
}
void new_priority() {
    if (!running && ptrRun < now){		
        int nextRun = p[ptrRun].process_id;			
        SET_PRIORITY(pid[nextRun], SCHED_FIFO, PRIORITY_HIGH);	
        running = 1;
    }			
    if (running && ptrRun + 1 < now){
        int nextRun = p[ptrRun + 1].process_id;
        SET_PRIORITY(pid[nextRun], SCHED_FIFO, PRIORITY_INIT);
    }
}
int main(){
    scanf("%d", &num_process);
    for (int i = 0; i < num_process; ++i){
        p[i].process_id = i;
	    scanf("%s%u%u", p[i].N, &p[i].R, &p[i].T);
    }
    qsort(p, num_process, sizeof(Input), cmp);

	struct sigaction act;
	act.sa_flags = 0;
	act.sa_handler = sig_child;
	sigfillset(&act.sa_mask);
	sigaction(SIGCHLD, &act, NULL);
    
    int next = p[now].process_id;
    for (int t = 0, i = num_process; i > 0; ++t) {
        new_priority();
        while (now < num_process && t == p[next].R) {
            create_process(&pid[next], p[next].N, next, p[next].T);
            next = p[++now].process_id;
            new_priority();
        }
        volatile unsigned long i;
        for (i = 0; i < TIME_UNIT; i++);
    }
}
