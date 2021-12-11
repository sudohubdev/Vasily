
.section .text
.code32
.extern krnl_main
.extern sys_tss_table
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
    movl $sys_tss_table,%ebx
    movw %bx,systss_special
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
.globl gdt_info
gdt_info:
    .word end_gdt-gdt-1
    .long gdt
gdt: 
zero: .long 0,0
flatcode: .byte 0xff,0xff,0,0,0,0b10011010,0b11001111,0
flatdata: .byte 0xff,0xff,0,0,0,0b10010010,0b11001111,0
usercode: .byte 0xff,0xff,0,0,0,0b11111010,0b11001111,0
userdata: .byte 0xff,0xff,0,0,0,0b11110010,0b11001111,0
systss:   .byte 0x68,0x00
systss_special: .byte 0,0,0,0b10001001,0b11000000,0
realcode:    .byte 0xff, 0xff, 0, 0, 0, 0b10011010, 0b10001111, 0
realdata:    .byte 0xff, 0xff, 0, 0, 0, 0b10010010, 0b10001111, 0
end_gdt:

.globl vbe_struct

    
realidt:
    .word 0x3ff
    .int 0

.section bss
    .skip 16384
tmp_stack: //x86 stack grows downwards
    .skip 16384

vbe_struct:
    sig: .int 0
    .comm data,512-4
