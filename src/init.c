#include "init.h"
#include "common.h"
#include "vgatext.h"
#include "dev/pic.h"
#include "idt.h"
#include "mem.h"
#include "heap.h"
#include "multitask.h"
#include <multiboot.h>
#include "fs/vfs.h"

extern multiboot_info_t globl_info;
void initialize_crap(){
 
    init_idt();
    init_pic();
    init_heap();
    init_paging();
    init_vfs();
    init_multitask();
    //TODO: KERNEL MODULES
}  
