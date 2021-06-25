#include "common.h"
#include "dev/pic.h"
#include"vgatext.h"
#include "io.h"
void init_pic(){
    putstring("init_pic()...");
    unsigned char a1,a2;
    a1=inb(0x21);
    a2=inb(0xa1);

    outb(0x20,0x11);
    io_wait();
    outb(0xa0,0x11);
    io_wait();
    outb(0x21,0x20);
    io_wait();
    outb(0xa1,0x28);
    io_wait();
    outb(0x21,4);
    io_wait();
    outb(0xa1,2);
    io_wait();

    outb(0x21,1);
    io_wait();
    outb(0xa1,1);
    io_wait();

    outb(0x21,a1);
    io_wait();
    outb(0xa1,a2);
    io_wait();

    putstring(donemsg);
}