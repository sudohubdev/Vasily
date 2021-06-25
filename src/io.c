#include "io.h"
void outb(unsigned short port,unsigned char data){
    asm("outb %0,%1"::"a"(data),"Nd"(port):);
}
unsigned char inb(unsigned short port){
    unsigned char data;
    asm("inb %1,%0"::"a"(data),"Nd"(port));
    return data;
}
volatile void io_wait(){
    asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;");
}