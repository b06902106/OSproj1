#include<linux/linkage.h>
#include<linux/kernel.h>

asmlinkage void sys_print(char *s){
    printk(s);
    return;
}
