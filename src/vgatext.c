#include "vgatext.h"
#include "io.h"
void buff_putchar(int x,int y,short c,unsigned short colour){
    *(unsigned short*)(0xb8000+y*80*2+x*2)=(colour<<8 | c);
}
unsigned char cursorpos[2]={0,0};
void text_scroll(){
    for(int x=0;x<80;++x){
        for(int y=0;y<25;++y){
            *(unsigned short*)(0xb8000+y*80*2+x*2)=*(unsigned short*)(0xb8000+80*2+y*80*2+x*2);
        }
    }
    --cursorpos[1];
    move_cursor(cursorpos[0],cursorpos[1]);
}
void putstring(const char* s){
    do{
        if(*s=='\n'){
            ++cursorpos[1];
            cursorpos[0]=0;
            if(cursorpos[1]>=25){
                text_scroll();
            }
        }
        else{
            buff_putchar(cursorpos[0],cursorpos[1],*s,0x7);
            ++cursorpos[0];
            if(cursorpos[0]==80){
                ++cursorpos[1];
                cursorpos[0]=0;
                if(cursorpos[1]>=25){
                    text_scroll();
                }
            }
        }
        move_cursor(cursorpos[0],cursorpos[1]);

    }while(*++s);
}
void move_cursor(int x,int y){
    unsigned short pos=y*80+x;
    outb(0x3d4,0xf);
    outb(0x3d5,(unsigned char)(pos&0xff));
    outb(0x3d4,0xe);
    outb(0x3d5,(unsigned char)((pos>>8)&0xff));
}