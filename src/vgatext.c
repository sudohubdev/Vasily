#include "vgatext.h"
void buff_putchar(int x,int y,short c,unsigned short colour){
    *(unsigned short*)(0xb8000+y*80*2+x*2)=(colour<<8 | c);
}
