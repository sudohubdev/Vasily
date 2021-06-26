#include "idt.h"
#include "common.h"
#include "isr.h"
struct idt_info idtinf;
struct idt krnl_idt[129]={{0,0,0,0,0}};
void set_idt_entry(unsigned int num,void* func,unsigned short sel,unsigned char type){
    krnl_idt[num].offset_l=func;
    krnl_idt[num].selector=sel;
    krnl_idt[num].type_attr=type;
    krnl_idt[num].offset_h=((unsigned int)func>>16);
}
void init_idt(){
    idtinf.base=&krnl_idt;
    idtinf.limit=sizeof(krnl_idt)-1;
    putstring("init_idt()...");
    for(int i=0;i<129;++i){
        set_idt_entry(i,int_stub,0x8,0x8e);
    }
    for(int i=0x20;i<0x30;++i){
        set_idt_entry(i,irq_stub,0x8,0x8e);
    }
    set_idt_entry(0x20,pit_isr,0x8,0x8e);
    set_idt_entry(0x21,key_isr,0x8,0x8e);
    asm("lidt idtinf;");
    putstring(donemsg);
}