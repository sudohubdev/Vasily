#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include "common.h"
#include <multiboot.h>
#include "vgatext.h"

extern char donemsg_str[];

extern char* donemsg;
extern multiboot_info_t globl_info;
extern unsigned int __krnl_end;

#endif // COMMON_H_INCLUDED

