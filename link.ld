ENTRY(_start)

SECTIONS{
    . = 0x10000;
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
    . = . + 16K;
     __krnl_end = ALIGN(16k);
}
