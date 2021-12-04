#include "io.h"
void outb(unsigned short port,unsigned char data){
    asm("outb %0,%1"::"a"(data),"d"(port):);
}
unsigned char inb(unsigned short port){
    unsigned char data;
    asm("inb %1,%0":"=a"(data):"d"(port));
    return data;
}
void outl(unsigned short port,unsigned int data){
    asm("outl %0,%1"::"a"(data),"d"(port):);

}
unsigned int inl(unsigned short port){
    unsigned int data=0;
    asm("inl %1,%0":"=a"(data):"d"(port));
    return data;
}


volatile void io_wait(){
    asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;");
}
