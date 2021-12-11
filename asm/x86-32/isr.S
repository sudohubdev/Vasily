.section .text
.globl pit_isr
.globl key_isr
.globl syscall_sr
.globl int_stub
.globl atapi_isr
.globl irq_stub
.extern current_task //IT IS A POINTER 
.extern kernel_task  //IT IS A POINTER
.extern sys_tss_table
.globl error_0
.globl error_2
.globl error_3
.globl error_4
.globl error_5
.globl error_6
.globl error_7
.globl error_8
.globl error_10
.globl error_11
.globl error_12
.globl error_13
.globl error_14
.globl error_17
.globl error_18
.globl error_19
.globl error_20
.globl error_21
.globl error_28
.globl error_29
.globl error_30
.globl err_code

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
 

 
temp: .int 0
/*int c_syscall(int edi(code),int (return int pointer)esi,int edx,int ecx,int ebx,int eax,int ebp);*/
syscall_sr:
    pushal
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax
    pushl %ebp
    call c_syscall
    movl %eax,temp
    add $28,%esp
    popal
    movl temp,%eax
    iret

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
    movw $0x23,%ax
    movw %ax,%ds
    movw %ax,%es
    movw %ax,%fs
    movw %ax,%gs
    
    
    jmp .end
    
    .ring0:
    movl $0x8,8(%esp)
    movw $0x10,%ax
    movw %ax,%ds
    movw %ax,%es
    movw %ax,%fs
    movw %ax,%gs
    
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
    
.extern exception_handler
    
err_code: .int 0
err_eip: .int 0

error_0:
    pushal
    movl $0,err_code

    
    popal
    iretl
error_2:
    pushal
    movl $2,err_code
    
    popal
    iretl
    
error_3:
    pushal
    movl $3,err_code
    
    popal
    iretl
    
error_4:
    pushal
    movl $4,err_code
    
    popal
    iretl
    
error_5:
    pushal
    movl $5,err_code
    
    popal
    iretl
    
error_6:
    pushl %ebx
    movl 4(%esp),%ebx
    movl %ebx,err_eip
    popl %ebx
    pushal
    movl current_task,%ebx
    cmpb $0,28(%ebx)
    jnz 1
    movl $6,err_code
    pushl -44(%esp)
    pushl $0
    pushl err_eip
    call exception_handler
    cli
    hlt
    1:
    
    popal
    iretl
    
error_7:
    pushal
    movl $7,err_code
    
    popal
    iretl
    
error_8:
    pushal
    movl $8,err_code
    
    popal
    
    iretl
error_10:
    pushal
    movl $10,err_code
    
    popal
    iretl
    
error_11:
    pushal
    movl $11,err_code
    
    popal
    iretl
    
error_12:
    pushal
    movl $12,err_code
    
    popal
    iretl
    
error_13:
    pushal
    movl $13,err_code
    
    popal
    iretl
    
error_14:
    pushal
    movl $14,err_code
    
    popal
    iretl
    
error_17:
    pushal
    movl $17,err_code
    
    popal
    iretl
    
error_18:
    pushal
    movl $18,err_code
    
    popal
    iretl
    
error_19:
    pushal
    movl $19,err_code
    
    popal
    iretl
    
error_20:
    pushal
    movl $20,err_code
    
    popal
    iretl
    
error_21:
    pushal
    movl $21,err_code
    
    popal
    
    iretl
error_28:
    pushal
    movl $28,err_code
    
    popal
    iretl
    
error_29:
    pushal
    movl $29,err_code
    
    popal
    iretl
    
error_30:
    pushal
    movl $30,err_code
    
    popal
    iretl
    


.extern ide_irq_invoked

irqstring: .string "ata IRQ!\n\0"

atapi_isr:
    movb $1,ide_irq_invoked
    movb $0x20,%al
    outb %al,$0xa0
    outb %al,$0x20
    iretl
