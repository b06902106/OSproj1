#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "include/heap.h"
#include "include/priority.h"
#define MAX_PROCESS_NUM 100
#define MAX_NAME_LENGTH 5
#define MAX_PROCESS_NAME 40
#define TIME_UNIT 1000000UL
typedef struct input{
    int process_id;
    char N[MAX_PROCESS_NAME];
    unsigned int R;
    unsigned int T;
}Input;
Input p[MAX_PROCESS_NUM];
char N[MAX_PROCESS_NUM][MAX_NAME_LENGTH];
unsigned int R[MAX_PROCESS_NUM], T[MAX_PROCESS_NUM];
int num_process;
pid_t pid[MAX_PROCESS_NUM];
int now;
int running;
struct Node *heap_root = NULL;
int cmpr(const void* a, const void *b){
    Input c = *(Input *)a;
    Input d = *(Input *)b;
    if (c.R < d.R) return -1;
    else if (c.R == d.R) return 0;
    else return 1;
}
int cmpt(const void* a, const void *b){
    Input c = *(Input *)a;
    Input d = *(Input *)b;
    if (c.T < d.T) return -1;
    else if (c.T == d.T) return 0;
    else return 1;
}
void sig_child(int signum){
    static int nFinish = 0;
    wait(NULL);
    nFinish+=1;
    running = 0;
    if (nFinish == num_process) exit(0);
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
void increase_next_process_priority() {
    int nextRun = heap_root->index;
    SET_PRIORITY(pid[nextRun], SCHED_FIFO, PRIORITY_INIT);
}
void decrease_next_process_priority() {
    if (!running || heap_root == NULL)return;
    int nextRun = heap_root->index;
    SET_PRIORITY(pid[nextRun], SCHED_FIFO, PRIORITY_LOW);
}
void running_next_process() {
    int nextRun = heap_root->index;
    heap_root = pop_heap(heap_root);
    SET_PRIORITY(pid[nextRun], SCHED_FIFO, PRIORITY_HIGH);	
    running = 1;						
}
void newt_priority() {
    if (!running && heap_root != NULL)		
        running_next_process();
    if (running && heap_root != NULL)	
        increase_next_process_priority();
}
int main (void){    
    scanf("%d", &num_process);
    for (int i = 0; i < num_process; ++i){
        p[i].process_id = i;
        scanf("%s%u%u", p[i].N, &p[i].R, &p[i].T);
        strcpy(N[i], p[i].N);
        R[i] = p[i].R;
        T[i] = p[i].T;
    }
	qsort(p, num_process, sizeof(Input), cmpt);
	qsort(p, num_process, sizeof(Input), cmpr);
    for(int i=0; i<num_process; i++){
        strcpy(p[i].N, N[i]);
        p[i].R = R[i];
        p[i].T = T[i];
    }
    struct sigaction act;
	act.sa_flags = 0;
	act.sa_handler = sig_child;
	sigfillset(&act.sa_mask);
	sigaction(SIGCHLD, &act, NULL);
    
    int next = p[now].process_id;    
    for (int t = 0, i = num_process; i > 0; ++t) {
        newt_priority();
        while (now < num_process && t == p[next].R) {
            decrease_next_process_priority();
            create_process(&pid[next], p[next].N, next, p[next].T);
            heap_root = insert_heap(heap_root, p[next].T, next);
            next = p[++now].process_id;
            newt_priority();
        }
        volatile unsigned long i;
        for (i = 0; i < TIME_UNIT; i++);
    }
}
