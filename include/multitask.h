#pragma once

struct task{
    void* next,*prev;//0,4
    unsigned int* eip,esp,cr3;//8,12,16
    unsigned short cs,ds,ss;//18,20,22
    unsigned char ring;//24
    char* task_name;
    unsigned long long task_id;
}__attribute__((packed));

/* multitask stack frame:  (X86 STACK FRAME GOES DOWNWARDS IN MEMORY)
 *      eax
 *      ebx
 *      ecx
 *      edx
 *      esi
 *      edi
 *      ebp
 *      es
 *      fs
 *      gs
 *      eip
 *      cs
 *      eflags
 *      esp (priv change)
 *      
 */

void init_multitask();
