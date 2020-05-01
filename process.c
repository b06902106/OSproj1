#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/syscall.h>
#define TIME_UNIT 1000000UL
int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "usage: ./this_program process_name process_idx exec_time\n");
        return 0;
    }
    
    pid_t pid = getpid();
    
    int idx = atoi(argv[2]);
    int exec_time = atoi(argv[3]);

    
    long start_time = syscall(335);
    for (int i = 0; i < exec_time; i++){ 
        volatile unsigned long i;
        for (i = 0; i < TIME_UNIT; i++);
    }
    long end_time = syscall(335);

    static const long BASE = 1000000000;
    char buf[500];
    sprintf(buf, "[Project1] %d %ld.%09ld %ld.%09ld\n", pid, start_time / BASE, start_time % BASE, end_time / BASE, end_time % BASE);
    printf("%s %d\n", argv[1], pid);
    syscall(334, buf);
    return 0;
}
