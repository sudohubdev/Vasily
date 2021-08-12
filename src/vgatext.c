#include "vgatext.h"
#include "io.h"
#include "klibc/string.h"

unsigned short buf[80*25]={0};

unsigned short* buf_ptr=&buf[0];


unsigned short default_colour=0x7;
void buff_putchar(int x,int y,short c,unsigned short colour){
    *(unsigned short*)(buf_ptr+y*80+x)=(colour<<8 | c);
}
unsigned char cursorpos[2]={0,0};
void set_term_colour(unsigned short c){
    default_colour=c;
};
void buf_flush(){
    unsigned short *fb=0xb8000;
    for(unsigned long i=0;i<(cursorpos[1]*80+cursorpos[0]);++i){
        fb[i]=buf[i];
    }
}

void text_scroll(){
    for(int x=0;x<80;++x){
        for(int y=0;y<25;++y){
            *(unsigned short*)(buf_ptr+y*80*2+x*2)=*(unsigned short*)(buf_ptr+80*2+y*80*2+x*2);
        }
    }
    --cursorpos[1];
        move_cursor(cursorpos[0],cursorpos[1]);
    buf_flush();
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
            buff_putchar(cursorpos[0],cursorpos[1],*s,default_colour);
            ++cursorpos[0];
            if(cursorpos[0]==80){
                ++cursorpos[1];
                cursorpos[0]=0;
                if(cursorpos[1]>=25){
                    text_scroll();
                }
            }
        }

    }while((*++s)!=0);
        move_cursor(cursorpos[0],cursorpos[1]);
        buf_flush();

}
int strlen(char* s){
    int i;
    for(i=0;s[i];++i);
    return i;
}
 void reverse(char s[])
 {
     int i, j;
     char c;
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }
 void putunum(unsigned int i,int base){
    char s[10]={0};
    int it=0;
    do{
        s[it++]="0123456789ABCDEF\0"[i%base];
    }while((i/=base)>0);
    reverse(s);
    putstring(s);
    
}
void move_cursor(int x,int y){
    unsigned short pos=y*80+x;
    outb(0x3d4,0xf);
    outb(0x3d5,(unsigned char)(pos&0xff));
    outb(0x3d4,0xe);
    outb(0x3d5,(unsigned char)((pos>>8)&0xff));
}
