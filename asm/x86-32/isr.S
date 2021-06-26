.section .text
.globl pit_isr
.globl key_isr
.globl int_stub
.globl irq_stub
pit_isr:
    movb $0x20,%al
    outb %al,$0x20
    iretl
key_isr:
    movb $0x20,%al
    outb %al,$0x20
    iretl
int_stub:
    iretl
irq_stub:
    movb $0x20,%al
    outb %al,$0x20
    iretl