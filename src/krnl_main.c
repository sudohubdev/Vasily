#include "common.h"
#include "init.h"
#include "vgatext.h"
#include <multiboot.h>
#include "klibc/string.h"

multiboot_info_t globl_info;

void krnl_main(multiboot_info_t *inf) {

  asm("cli;lgdt gdt_info");
  memcpy(&globl_info,inf,sizeof(multiboot_info_t));
  initialize_crap();
  asm("int $0x60");
}
