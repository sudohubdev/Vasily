#pragma once
#include <multiboot.h>

extern unsigned int gres[2],tres[2];

void move_cursor(int x,int y);
void putstring(const char* s);
void text_scroll();
void putunum(unsigned int i,int base);
void set_term_colour(unsigned short c);
void putstring_xy(const char *s,unsigned int x,unsigned int y,unsigned int fc);
void putpixel(unsigned int c, unsigned int x, unsigned int y);
void putunum_xy(unsigned int i, int base,int xoff,unsigned int y,unsigned int fc) ;
void buf_flush();
struct vbe_info_structure {
   char sig[4];             // == "VESA"
   unsigned short VbeVersion;                 // == 0x0300 for VBE 3.0
   unsigned short OemStringPtr[2];            // isa vbeFarPtr
   unsigned charCapabilities[4];
   unsigned short VideoModePtr[2];         // isa vbeFarPtr
   unsigned short TotalMemory;             // as # of 64KB blocks
} __attribute__((packed));

void init_vga();
extern multiboot_info_t globl_info;
extern unsigned short *buf_ptr;
