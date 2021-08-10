#pragma once
#include "mem.h"
#include "fs/vfs.h"
struct task{
    void* next,*prev;//0,4
    unsigned int* eip,*esp,*kernel_esp,*cr3,*eflags;//8,12,16,20,24
    unsigned char ring;//28
    char* task_name;
    unsigned long long task_id;
    void *task_stack,*kernel_stack;
    struct vfs_node* fds[1024];
    
}__attribute__((packed));



struct tss_entry_struct {
	unsigned int prev_tss; // The previous TSS - with hardware task switching these form a kind of backward linked list.
	unsigned int esp0;     // The stack pointer to load when changing to kernel mode.
	unsigned int ss0;      // The stack segment to load when changing to kernel mode.
	// Everything below here is unused.
	unsigned int esp1; // esp and ss 1 and 2 would be used when switching to rings 1 or 2.
	unsigned int ss1;
	unsigned int esp2;
	unsigned int ss2;
	unsigned int cr3;
	unsigned int eip;
	unsigned int eflags;
	unsigned int eax;
	unsigned int ecx;
	unsigned int edx;
	unsigned int ebx;
	unsigned int esp;
	unsigned int ebp;
	unsigned int esi;
	unsigned int edi;
	unsigned int es;
	unsigned int cs;
	unsigned int ss;
	unsigned int ds;
	unsigned int fs;
	unsigned int gs;
	unsigned int ldt;
	unsigned short trap;
	unsigned short iomap_base;
}__attribute__((packed));
 
typedef struct tss_entry_struct tss_entry_t;

void init_multitask();
