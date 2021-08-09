.section .text
.globl pit_isr
.globl key_isr
.globl int_stub
.globl irq_stub
.extern current_task //IT IS A POINTER 
.extern kernel_task  //IT IS A POINTER

ebxbac: .int 0
eaxbac: .int 0


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

/* multitask stack frame:  (X86 STACK FRAME GOES DOWNWARDS IN MEMORY)
 *      eax                /^\      |
 *      ebx                 |       |
 *      ecx                 |      \_/
 *      edx                push    pop
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

 
pit_isr:
    pushw %ax
    movb $0x20,%al
    outb %al,$0x20
    popw %ax
    pushf
    cmpl $0,enable_multitask
    jz .s
    popf
    
    
    pushw %gs
    pushw %fs
    pushw %es
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax
    
    movl current_task,%ebx
    movl 40(%esp),%eax
    movl %eax,8(%ebx)
    movl 44(%esp),%eax
    movw %ax,18(%ebx)
    movw %ds,%ax
    movw %ax,20(%ebx)
    movw %ss,%ax
    movw %ax,22(%ebx)    
    movl %cr3,%eax
    movl %eax,16(%ebx)
    movl %esp,12(%ebx)
    
    cmpl $0,4(%ebx)
    jz .s4
    
    cmpb $3,24(%ebx)
    jnz .s4
    
    movl 54(%esp),%eax
    movl %eax,12(%ebx)
    
    
    .s4:
    
    movl (%ebx), %ebx
    
    
    
    cmpl $0, %ebx
    
    movl current_task,%ebx
    
    jz .s2
    
    movl (%ebx), %ebx
    movl %ebx, current_task
    jmp .s3
    
    .s2:
    
    movl kernel_task,%ebx
    movl %ebx, current_task
    
    .s3:
    
    movl current_task,%ebx
    movl 8(%ebx),%eax
    movl %eax,40(%esp)
    movl $0,%eax
    movw 18(%ebx),%ax
    movl %eax,44(%esp)
    movw 20(%ebx),%ax
    movw %ax, %ds
    movw 22(%ebx),%ax
    movw %ax,%ss
    movl 16(%ebx),%eax
    movl %eax,%cr3
    movl 12(%ebx),%esp
    
    popl %eax
    popl %ebx 
    popl %ecx 
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popw %es
    popw %fs
    popw %gs
    
    
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
