.section .text
.globl pit_isr
.globl key_isr
.globl int_stub
.globl irq_stub
.extern current_task //IT IS A POINTER 
.extern kernel_task  //IT IS A POINTER
.extern sys_tss_table

ebxbac: .int 0
eaxbac: .int 0


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

/* multitask stack frame:  (X86 STACK FRAME GOES DOWNWARDS IN MEMORY)
 *      (pushad)  /\ push  \/ pop
        eax
        ecx
        edx
        ebx
        kernel_esp
        ebp
        esi
        edi
 *      eip 32(%esp)
 *      cs
 *      eflags
 *      esp 
 *      ss
 */

 .code32
pit_isr:
    pushw %ax
    movb $0x20,%al
    outb %al,$0x20
    
    movw $0x10,%ax
    movw %ax,%ds
    movw %ax,%es
    movw %ax,%fs
    movw %ax,%gs
    movw %ax,%ss

    
    popw %ax
    
    cmpl $0,enable_multitask
    jz .s
    
    pushal
    movl current_task,%ebx

    
    movl 32(%esp),%eax
    movl %eax,8(%ebx)
    movl 40(%esp),%eax
    movl %eax,24(%ebx)
    cmpl kernel_task,%ebx
    jz .r0
    movl 44(%esp),%eax
    movl %eax,12(%ebx)
    .r0:
    movl %esp,16(%ebx)
    
    cmpl $0, (%ebx)
    jz .s1

    movl %ebx,current_task
    jmp .s2
    
    .s1:
    
    movl kernel_task,%ebx
    movl %ebx,current_task
    
    .s2:
    
    movl 8(%ebx),%eax
    movl %eax,32(%esp)
    movl 24(%ebx),%eax
    movl %eax,40(%esp)
    cmpl kernel_task,%ebx
    jz .1205
    movl 12(%ebx),%eax
    movl %eax,44(%esp)
    .1205:
    movl 16(%ebx),%esp
    movl 16(%ebx),%eax
    movl %eax,sys_tss_table+4
    movl $0x10,sys_tss_table+8
    movl 20(%ebx),%eax
    movl %eax,sys_tss_table+28
       
    
    popal
    
    
    
    
        
    pushl %eax
    movw $0x23,%ax
    movw %ax,%ds
    movw %ax,%es
    movw %ax,%fs
    movw %ax,%gs
    
    movl current_task,%eax
    cmpl kernel_task,%eax
    jz .ring0
    movl $0x23,20(%esp)
    movl $0x1b,8(%esp)
    
    jmp .end
    
    .ring0:
    movl $0x8,8(%esp)
    
    .end:
    
    popl %eax
    
    iretl
    
    
.s: 
    popf
    iretl

key_isr:
    pushw %ax
    movb $0x20,%al
    outb %al,$0x20
    popw %ax
    iretl
int_stub:
    iretl
irq_stub:
    pushw %ax
    movb $0x20,%al
    outb %al,$0x20
    popw %ax
    iretl
