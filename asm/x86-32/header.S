#define ASM_FILE 1
#include <multiboot.h>
#define krnl_flags MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_VIDEO_MODE 
.section .multiboot
multiboot_header:
.align 4
.long MULTIBOOT_HEADER_MAGIC
.long krnl_flags
.long -(MULTIBOOT_HEADER_MAGIC+krnl_flags)
.long 0
.long 0
.long 0
.long 0
.long 0
istextmode: .long 0
width:.long 1280
length:.long 720
depth: .long 0
