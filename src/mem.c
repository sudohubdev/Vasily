#include "mem.h"
#include "common.h"
#include "heap.h"
#include "klibc/string.h"
#include "panic.h"
#include "vgatext.h"
#include <multiboot.h>
#define INDEX_FROM_BIT(a) (a / (8 * 4))
#define OFFSET_FROM_BIT(a) (a % (8 * 4))
unsigned int max_index;
unsigned int *kernel_page_usage_bitmap;
pagedir_t *krnl_pagedir = 0;
void set_usage_bitmap(unsigned int *bm, unsigned int addr) {
  bm[INDEX_FROM_BIT(addr / 0x1000)] |= 1 << (OFFSET_FROM_BIT(addr / 0x1000));
}
void unset_usage_bitmap(unsigned int *bm, unsigned int addr) {
  bm[INDEX_FROM_BIT(addr / 0x1000)] &= (~(1 << (OFFSET_FROM_BIT(addr))));
}
int test_usage_bitmap(unsigned int *bm, unsigned int addr) {
  return (bm[INDEX_FROM_BIT(addr / 0x1000)] & (1 << (OFFSET_FROM_BIT(addr))))
             ? 0
             : 1;
}
unsigned int find_first_from_bm() {
  for (unsigned int i = 0; i < max_index; ++i) {
    if (kernel_page_usage_bitmap[i] != 0xffffffff) {
      for (int j = 0; j < 32; ++j) {
        if (test_usage_bitmap(kernel_page_usage_bitmap,
                              j * 0x1000 + i * 0x20000)) {
          return j * 0x1000 + i * 0x20000;
        }
      }
    }
  }
  panic("OOM");
  return -1;
}
void krnl_map_page(pagedir_t *p, unsigned int physaddr, unsigned int virtaddr) {
  if (p[virtaddr / 0x400000].raw == 0) {
    p[virtaddr / 0x400000].raw =
        (unsigned int)khamalloc(sizeof(page_t) * (0x400000 / 0x1000));
  }
  p[virtaddr / 0x400000].s = 0;
  p[virtaddr / 0x400000].u = 0;
  p[virtaddr / 0x400000].r = 1;
  p[virtaddr / 0x400000].p = 1;

  page_t *page = &((page_t *)(p[virtaddr / 0x400000].page_table_addr
                              << 12))[virtaddr % 0x400000 / 0x1000];
  page->raw = physaddr & 0xfffff000;
  page->p = 1;
  page->r = 1;
  page->u = 0;
}
 void flush_page_table(pagedir_t* p);
//  asm volatile("movl krnl_pagedir,%%eax;movl %%eax,%%cr3;movl %%cr0,%%eax;orl
//  $0x80000000,%%eax;xchgw %%bx,%%bx ;movl %%eax,%%cr0;cli;hlt"::"r"(p):"eax");
//}
asm("flush_page_table:\
            movl 4(%esp),%eax;\
            movl %eax,%cr3;\
            movl %cr0,%eax;\
            orl $0x80000001,%eax;\
            movl %eax,%cr0;\
            ret;\
            ");
void *realloc_test;
extern multiboot_info_t globl_info;
void init_paging() {
  putstring("init_paging...");
  max_index = INDEX_FROM_BIT((globl_info.mem_upper + 2048) / 4);
  kernel_page_usage_bitmap =
      khmalloc(INDEX_FROM_BIT((globl_info.mem_upper + 1024) / 4));

  krnl_pagedir = khamalloc(sizeof(pagedir_t) * 1024);
  krnl_pagedir[0].raw =
      (unsigned int)khamalloc(sizeof(page_t) * (0x400000 / 0x1000));
  krnl_pagedir[0].s = 0;
  krnl_pagedir[0].u = 0;
  krnl_pagedir[0].r = 1;
  krnl_pagedir[0].p = 1;
  for (unsigned int i = 0; i <= 0x200000; i += 0x1000)
    krnl_map_page(krnl_pagedir, i, i);
  for (unsigned int i = 0; i < (globl_info.framebuffer_height * (1 + globl_info.framebuffer_width) * (globl_info.framebuffer_bpp / 8) / 0x2000)*2;++i){
      krnl_map_page(krnl_pagedir, globl_info.framebuffer_addr + i * 0x1000,globl_info.framebuffer_addr + i * 0x1000);
  }
  flush_page_table(krnl_pagedir);
  putstring(donemsg);
}
