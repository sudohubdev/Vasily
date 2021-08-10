#include "multitask.h"
#include "common.h"
#include "heap.h"
#include "panic.h"

int enable_multitask;

struct task* kernel_task;

struct task* current_task;

unsigned long long proc_id_counter=0;

/*
struct task{
    void* next,prev;//0,4
    unsigned int* eip,esp,cr3;//8,12,16
    unsigned short cs,ds,ss;//18,20,22
    unsigned char ring;//24
    char* task_name;
    unsigned long long task_id;
}__attribute__((packed));
*/
char kernel_task_name[]="Kernel";

void init_multitask(){
    putstring("init_multitask()...");

    current_task=kernel_task=(struct task*)khmalloc(sizeof(struct task));

    current_task->task_id=proc_id_counter++;
    current_task->task_name=&kernel_task_name;
    
    enable_multitask=1;
    
    
    putstring(donemsg);

}
