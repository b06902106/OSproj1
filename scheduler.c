#define _GNU_SOURCE
#include <stdio.h>
#include "include/priority.h"
#define MAX_SCHEDULE_NAME 5
int main(){
    char policy[MAX_SCHEDULE_NAME];
    scanf("%s", policy);
    USE_CPU(getpid(), 0);
    SET_PRIORITY(getpid(), SCHED_FIFO, PRIORITY_INIT);

    fflush(stdin);
    if(policy[0]=='F') execlp("./FIFO", "./FIFO", NULL); 
    else if(policy[0]=='R') execlp("./RR", "./RR", NULL); 
    else if(policy[0]=='S') execlp("./SJF", "./SJF", NULL); 
    else if(policy[0]=='P') execlp("./PSJF", "./PSJF", NULL); 
    else fprintf(stderr, "no such policy"); 
    return 0;
}
