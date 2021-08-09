
.section .text
.code32
.extern krnl_main
.globl _start
_start:
    nop
    cmpl $0x2badb002,%eax
    jz s
kill:
    lidt 0
    int $0x10
s:
    lgdt gdt_info
    ljmp $0x8,$switch_to_our_seg

multiboot_info: .long 0

switch_to_our_seg:
    movl %ebx,multiboot_info
    movw $0x10,%bx
    movw %bx,%ds
    movw %bx,%es
    movw %bx,%fs 
    movw %bx,%gs
    movw %bx,%ss 
    movl $tmp_stack,%esp
    movl %esp,%ebp
    pushl multiboot_info

    finit //initialize 80387 so we can do float

    call krnl_main
    a:
    sti
    hlt
    jmp a
.section .data

gdt_info:
    .word end_gdt-gdt-1
    .long gdt
gdt: 
zero: .long 0,0
flatcode: .byte 0xff,0xff,0,0,0,0b10011010,0b11001111,0
flatdata: .byte 0xff,0xff,0,0,0,0b10010010,0b11001111,0
end_gdt:
.section bss
    .skip 8192
tmp_stack: //x86 stack grows downwards
    .skip 8192
