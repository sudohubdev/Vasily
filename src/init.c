#include "init.h"
#include "common.h"
#include "dev/ide.h"
#include "fs/vfs.h"
#include "heap.h"
#include "idt.h"
#include "mem.h"
#include "multitask.h"
#include "syscall.h"
#include "vgatext.h"
#include <multiboot.h>
#include "dev/pci.h"
#include "dev/pic.h"
#include "dev/sata.h"
#include "fs/fat.h"

#include "panic.h"
#include "fs/mbr.h"
extern multiboot_info_t globl_info;
void initialize_crap() {
  init_heap();
  init_vga();
  init_idt();
  init_pic();
  init_paging();
  init_vfs();
  init_multitask();
  init_pci();
  init_sata();
  init_ide();
  init_mbr();
  init_fat();
  /*  struct vfs_node test;
  extern struct vfs_node* devfs_root;
  void* ptr=finddir(devfs_root,"pdAb1");
  internal_mount(ptr,&test,0,0,0);*/

  //text_scroll();
  // TODO: KERNEL MODULES
}
