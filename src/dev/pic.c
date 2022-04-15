#include "dev/pic.h"
#include "common.h"
#include "io.h"
#include "vgatext.h"
char p[] = "init_pic()...";
#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
void init_pic() {
  putstring(p);
  unsigned char a1, a2;
  a1 = inb(0x21);
  a2 = inb(0xa1);

  outb(0x20, 0x11);
  io_wait();
  outb(0xa0, 0x11);
  io_wait();
  outb(0x21, 0x20);
  io_wait();
  outb(0xa1, 0x28);
  io_wait();
  outb(0x21, 4);
  io_wait();
  outb(0xa1, 2);
  io_wait();

  outb(0x21, 1);
  io_wait();
  outb(0xa1, 1);
  io_wait();

  a2&=0b111111;
  outb(0x21, a1);
  io_wait();
  outb(0xa1, a2);
  io_wait();

  putstring(donemsg);
}
void pic_setmask(unsigned char IRQline) {
    unsigned short port;
    unsigned char value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);        
}
 
void pic_clearmask(unsigned char IRQline) {
    unsigned short port;
        unsigned char value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);        
}
