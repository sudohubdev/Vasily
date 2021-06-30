ENTRY(_start)

SECTIONS{
    . = 0x00;
    __krnl_start = .;
    .text BLOCK(4k) : ALIGN(4k)
    {
        *(.multiboot)
        *(.text)
    }
    .data BLOCK(4k) : ALIGN(4k)
    {
        *(.data)
    }
    .rodata BLOCK(4k) : ALIGN(4k)
    {
        *(.rodata)
    }


    
    .bss BLOCK(4k) : ALIGN(4k) 
    {
        *(COMMON)
        *(.bss)
    }
     __krnl_end = .;
}