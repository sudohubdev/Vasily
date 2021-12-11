ENTRY(_start)

SECTIONS{
    . = 0x40000;
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
    
    .bss BLOCK(4k) : ALIGN(4k) 
    {
        *(COMMON)
        *(.bss)
    }
    
    .rodata BLOCK(4k) : ALIGN(4k)
    {
        *(.rodata)
    }
    
    . = . + 4K;
    
     __krnl_end = ALIGN(16k);
}
