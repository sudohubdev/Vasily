#include "init.h"
#include <multiboot.h>
#include "vgatext.h"

multiboot_info_t globl_info;
typedef struct __attribute__ ((packed)) {
    unsigned short di, si, bp, sp, bx, dx, cx, ax;
    unsigned short gs, fs, es, ds, eflags;
} regs16_t;
extern void int32(unsigned char num,regs16_t *regs);

extern struct vbe_info_structure vbe_struct;
unsigned char stack[512]={0};
    regs16_t rs __attribute__((section(".data")));

void krnl_main(multiboot_info_t* inf){
    rs.ax=0x4f00;
    rs.di=(unsigned short)(&vbe_struct);
    int32(0x10,&rs);
    asm("cli;lgdt gdt_info");
    for(unsigned int i=0;i<=sizeof(multiboot_info_t);++i){
        ((unsigned char*)&globl_info)[i]=((unsigned char*)inf)[i];
    }

    initialize_crap();

    
}
