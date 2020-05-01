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
#define MAX_QUEUE_SIZE (1000000 + 7)
#define TIME_QUANTUM 500
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
int running;
int time_cnt = 0, isbock = 0;
int queue[MAX_QUEUE_SIZE], ptrF = 0, ptrB = 0;
void push_queue(int x) {
    queue[ptrB++] = x;
}
int front_queue() {
    return queue[ptrF];
}
int size_queue() {
    return ptrB - ptrF;
}
int empty_queue() {
    return ptrF < ptrB ? 0 : 1;
}
void pop_queue() {
    if (ptrF < ptrB)ptrF++;
}
int cmp(const void* a, const void *b){
    Input c = *(Input *)a;
    Input d = *(Input *)b;
    if (c.R < d.R) return -1;
    else if (c.R == d.R) return 0;
    else return 1;
}
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
void sig_child(int signo){
    static int num_finish = 0;
    wait(NULL);
    num_finish+=1;
    pop_queue();
    running = 0;
    isbock = 0;
    time_cnt = 0;
    if (num_finish == num_process) exit(0);
}
void new_priority() {
    if (!running && size_queue() >= 1){
        time_cnt = 0;
        int nextRun = queue[ptrF];
        if (p[nextRun].T <= TIME_QUANTUM)
            isbock = 1;
        SET_PRIORITY(pid[nextRun], SCHED_FIFO, PRIORITY_HIGH);	
        running = 1;						
    }
    if (running && size_queue() >= 2){
        int nextRun = queue[ptrF + 1];
        SET_PRIORITY(pid[nextRun], SCHED_FIFO, PRIORITY_INIT);
    }
}

int main (){    
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
            push_queue(next);
            next = p[++now].process_id;
            new_priority();
        }
        volatile unsigned long i;
        for (i = 0; i < TIME_UNIT; i++);
        time_cnt++;
        if (!isbock && running && time_cnt == TIME_QUANTUM) { 
            int id = front_queue();
            pop_queue();
            p[id].T -= TIME_QUANTUM;
            SET_PRIORITY(pid[id], SCHED_FIFO, PRIORITY_LOW);	
            push_queue(id);
            running = 0;
            new_priority();
        }
    }
}
