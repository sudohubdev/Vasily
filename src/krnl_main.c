#include "init.h"
#include <multiboot.h>
multiboot_info_t globl_info;
void krnl_main(multiboot_info_t* inf){
    for(unsigned int i=0;i<=sizeof(multiboot_info_t);++i){
        ((unsigned char*)&globl_info)[i]=((unsigned char*)inf)[i];
    }
    initialize_crap();
}