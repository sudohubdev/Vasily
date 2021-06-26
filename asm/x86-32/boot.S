
.section .text
.code32
.extern krnl_main
.globl _start
_start:
    nop
    lgdt gdt_info
    ljmp $0x8,$switch_to_our_seg
switch_to_our_seg:

    movw $0x10,%bx
    movl %bx,%ds
    movl %bx,%es
    movl %bx,%fs 
    movl %bx,%gs
    movl %bx,%ss 
    movl $tmp_stack,%esp
    movl %esp,%ebp
    call krnl_main
    a:
    sti
    hlt
    jmp a
    
gdt_info:
    .word end_gdt-gdt-1
    .long gdt
gdt: 
zero: .long 0,0
flatcode: .byte 0xff,0xff,0,0,0,0b10011010,0b11001111,0
flatdata: .byte 0xff,0xff,0,0,0,0b10010010,0b11001111,0
end_gdt:
.section bss
    .skip 2048
tmp_stack: //x86 stack grows downwards
