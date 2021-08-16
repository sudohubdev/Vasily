#include "multitask.h"
#include "common.h"
#include "heap.h"
#include "panic.h"

tss_entry_t sys_tss_table __attribute__((section (".data")));

int enable_multitask;

struct task* kernel_task;

struct task* current_task;

unsigned long long proc_id_counter=0;

/*
struct task{
    void* next,*prev;//0,4
    unsigned int* eip,*esp,*kernel_esp,*cr3,*eflags;//8,12,16,20,24
    unsigned char ring;//28
    char* task_name;
    unsigned long long task_id;
    void *task_stack,*kernel_stack;
    
}__attribute__((packed));

*/
char kernel_task_name[]="vmvasily";



void init_multitask(){
    putstring("init_multitask()...");
    current_task=kernel_task=(struct task*)khmalloc(sizeof(struct task));
    current_task->task_id=proc_id_counter++;
    current_task->task_name=(char*)&kernel_task_name;
    current_task->ring=0;
    current_task->next=0;
    void* reg_cr3;
    asm("movl %%cr3,%%eax; movl %%eax, %0":"=g"(reg_cr3)::"eax");
    current_task->cr3=reg_cr3;    
    enable_multitask=1;
    asm("sti;");
    putstring(donemsg);

}
