#include "mem.h"
#include "vgatext.h"
#include "panic.h"
#include "common.h"
#include "klibc/string.h"
#include "heap.h"
#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))
unsigned int max_index;
unsigned int* kernel_page_usage_bitmap;
pagedir_t* krnl_pagedir=0;
void set_usage_bitmap(unsigned int* bm,unsigned int addr){
    bm[INDEX_FROM_BIT(addr/0x1000)]|=1<<(OFFSET_FROM_BIT(addr/0x1000));
}
void unset_usage_bitmap(unsigned int* bm,unsigned int addr){
    bm[INDEX_FROM_BIT(addr/0x1000)]&=(~(1<<(OFFSET_FROM_BIT(addr))));
}
int test_usage_bitmap(unsigned int* bm,unsigned int addr){
    return (bm[INDEX_FROM_BIT(addr/0x1000)]&(1<<(OFFSET_FROM_BIT(addr))))?0:1;
}
unsigned int find_first_from_bm(unsigned int* bm){
    for(unsigned int i=0;i<max_index;++i){
        if(kernel_page_usage_bitmap[i]!=0xffffffff){
            for(int j=0;j<32;++j){
                if(test_usage_bitmap(kernel_page_usage_bitmap,j*0x1000+i*0x20000)){
                    return j*0x1000+i*0x20000;
                }
            }
        }
    }
    panic("OOM");
    return -1;
}
void krnl_map_page(pagedir_t* p,unsigned int physaddr,unsigned int virtaddr){
    page_t* page=&((page_t*)(p[virtaddr/0x400000].page_table_addr<<12))[virtaddr%0x400000/0x1000];
    page->raw=physaddr;
    page->p=1;
    page->r=1;
    page->u=0;
    
}
//void flush_page_table(pagedir_t* p){
  //  asm volatile("movl krnl_pagedir,%%eax;movl %%eax,%%cr3;movl %%cr0,%%eax;orl $0x80000000,%%eax;xchgw %%bx,%%bx ;movl %%eax,%%cr0;cli;hlt"::"r"(p):"eax");
//}
asm("flush_page_table:\
            movl 4(%esp),%eax;\
            movl %eax,%cr3;\
            movl %cr0,%eax;\
            orl $0x80000001,%eax;\
            movl %eax,%cr0;\
            ret;\
            ");
void* realloc_test;
void init_paging(){
    putstring("init_paging...");
    max_index=INDEX_FROM_BIT((globl_info.mem_upper+1024)/4);
    kernel_page_usage_bitmap=khmalloc(INDEX_FROM_BIT((globl_info.mem_upper+1024)/4));
    krnl_pagedir=khamalloc(sizeof(pagedir_t)*4);
    krnl_pagedir[0].raw=(unsigned int)khamalloc(sizeof(page_t)*(0x400000/0x1000));    
    krnl_pagedir[0].s=0;
    krnl_pagedir[0].u=0;
    krnl_pagedir[0].r=1;
    krnl_pagedir[0].p=1;
    for(unsigned int i=0;i<=0x100000;i+=0x1000)
        krnl_map_page(krnl_pagedir,i,i);
    flush_page_table(krnl_pagedir);
    putstring(donemsg);
}
