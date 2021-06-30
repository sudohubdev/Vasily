#include "init.h"
#include "common.h"
#include "vgatext.h"
#include "dev/pic.h"
#include "idt.h"
#include "mem.h"
#include <multiboot.h>
extern multiboot_info_t globl_info;
void initialize_crap(){
    init_idt();
    init_pic();
    init_paging();

}  