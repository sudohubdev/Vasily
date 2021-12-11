#include "idt.h"
#include "common.h"
#include "isr.h"

#include "syscall.h"
struct idt_info idtinf;
struct idt krnl_idt[129] = {{0, 0, 0, 0, 0}};
void set_idt_entry(unsigned int num, void *func, unsigned short sel,
                   unsigned char type) {
  krnl_idt[num].offset_l = (unsigned int)func;
  krnl_idt[num].selector = sel;
  krnl_idt[num].type_attr = type;
  krnl_idt[num].offset_h = ((unsigned int)func >> 16);
}
void init_idt() {
  idtinf.base = (unsigned int)&krnl_idt;
  idtinf.limit = sizeof(krnl_idt) - 1;
  putstring("init_idt()...");
  for (int i = 0; i < 129; ++i) {
    set_idt_entry(i, int_stub, 0x8, 0x8e);
  }
  for (int i = 0x20; i < 0x30; ++i) {
    set_idt_entry(i, irq_stub, 0x8, 0x8e);
  }
  set_idt_entry(0x20, pit_isr, 0x8, 0x8e);
  set_idt_entry(0x21, key_isr, 0x8, 0x8e);
  set_idt_entry(0x60, syscall_sr, 0x8, 0x8e); 
  set_idt_entry(0x20+14, atapi_isr, 0x8, 0x8e);
  set_idt_entry(0x20+15, atapi_isr, 0x8, 0x8e);
  set_idt_entry(0, error_0, 0x8, 0x8e);
  set_idt_entry(2, error_2, 0x8, 0x8e);
  set_idt_entry(3, error_3, 0x8, 0x8e);
  set_idt_entry(4, error_4, 0x8, 0x8e);
  set_idt_entry(5, error_5, 0x8, 0x8e);
  set_idt_entry(6, error_6, 0x8, 0x8e);
  set_idt_entry(7, error_7, 0x8, 0x8e);
  set_idt_entry(8, error_8, 0x8, 0x8e);
  set_idt_entry(10, error_10, 0x8, 0x8e);
  set_idt_entry(11, error_11, 0x8, 0x8e);
  set_idt_entry(12, error_12, 0x8, 0x8e);
  set_idt_entry(13, error_13, 0x8, 0x8e);
  set_idt_entry(14, error_14, 0x8, 0x8e);
  set_idt_entry(17, error_17, 0x8, 0x8e);
  set_idt_entry(18, error_18, 0x8, 0x8e);
  set_idt_entry(19, error_19, 0x8, 0x8e);
  set_idt_entry(20, error_20, 0x8, 0x8e);
  set_idt_entry(21, error_21, 0x8, 0x8e);
  set_idt_entry(28, error_28, 0x8, 0x8e);
  set_idt_entry(29, error_29, 0x8, 0x8e);
  set_idt_entry(30, error_30, 0x8, 0x8e);

  asm("lidt idtinf;cli");
  putstring(donemsg);
}
