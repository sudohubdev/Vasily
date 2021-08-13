#pragma once

void move_cursor(int x,int y);
void putstring(const char* s);
void text_scroll();
void putunum(unsigned int i,int base);
void set_term_colour(unsigned short c);

struct vbe_info_structure {
   char sig[4];             // == "VESA"
   unsigned short VbeVersion;                 // == 0x0300 for VBE 3.0
   unsigned short OemStringPtr[2];            // isa vbeFarPtr
   unsigned charCapabilities[4];
   unsigned short VideoModePtr[2];         // isa vbeFarPtr
   unsigned short TotalMemory;             // as # of 64KB blocks
} __attribute__((packed));
