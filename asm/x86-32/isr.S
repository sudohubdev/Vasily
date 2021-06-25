.section .text
.globl pit_isr
pit_isr:
    movb $0x20,%al
    outb %al,$0x20
    iretl