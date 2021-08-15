#include "vgatext.h"
#include "io.h"
#include <multiboot.h>
#include "klibc/string.h"
#include "heap.h"
#include "font.h"

//TODO: LOOKUP TABLE UNFINISHED!!!!!!!

unsigned int textmode_lookup[]={0,0xff,0xffff00,0xffff,0xff0000,0xff00ff,0xa52a2a,0xaaaaaa};

unsigned int tres[2]={80,25};

unsigned int gres[2]={720,400};


char istext=1;


unsigned short* buf_ptr;

extern multiboot_info_t globl_info;


unsigned short default_colour=0x7;
void buff_putchar(int x,int y,short c,unsigned short colour){
    *(unsigned short*)(buf_ptr+y*tres[0]+x)=(colour<<8 | c);
}
unsigned int cursorpos[2]={0,0};

unsigned int prevcursorpos[2]={0,0};


void set_term_colour(unsigned short c){
    default_colour=c;
};

void putpixel(unsigned int c,unsigned int x,unsigned int y){
    unsigned char *p=globl_info.framebuffer_addr+((y*globl_info.framebuffer_width+x)*(globl_info.framebuffer_bpp/8));
    for(unsigned int i=0;i<(globl_info.framebuffer_bpp/8);++i){
        p[i]=(255&(c>>(i*8)));
    }
    
    
}

void drawchar(unsigned char c, int x, int y, int fgcolor, int bgcolor)
{
	unsigned int cx,cy;
	int mask[8]={1,2,4,8,16,32,64,128};
	unsigned char *glyph=&vga_font[0]+c*16;
 
	for(cy=0;cy<16;cy++){
		for(cx=0;cx<8;cx++){
			putpixel(glyph[cy]&mask[7-cx]?fgcolor:bgcolor,x+cx,y+cy);
		}
	}
}

void buf_flush(){
    unsigned short *fb;
    fb=(short unsigned int*)(unsigned int)globl_info.framebuffer_addr;
    if(globl_info.framebuffer_type==2){
        for(unsigned long i=0;i<(cursorpos[1]*tres[0]+cursorpos[0]);++i){
            fb[i]=buf_ptr[i];
        }
    }
    else{
        for(unsigned long x=0;x<=(cursorpos[0]);++x){
            for(unsigned long y=0;y<=(cursorpos[1]);++y){
                drawchar((char)buf_ptr[y*tres[0]+x],x*8,y*16,textmode_lookup[default_colour],0);
            }
        }
    }
}

void text_scroll(){
    for(unsigned int x=0;x<tres[0];++x){
        for(unsigned int y=0;y<25;++y){
            *(unsigned short*)(buf_ptr+y*tres[0]*2+x*2)=*(unsigned short*)(buf_ptr+tres[0]*2+y*tres[0]*2+x*2);
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
            if(cursorpos[0]==tres[0]){
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
    if(globl_info.framebuffer_type==2){
        unsigned short pos=y*tres[0]+x;
        outb(0x3d4,0xf);
        outb(0x3d5,(unsigned char)(pos&0xff));
        outb(0x3d4,0xe);
        outb(0x3d5,(unsigned char)((pos>>8)&0xff));
    }
            
}

void init_vga(){
    if(globl_info.framebuffer_type==2)
        buf_ptr=khmalloc(2*(tres[0]*2+tres[1]*tres[0]*2+tres[0]*2));
    else{
        gres[0]=globl_info.framebuffer_width;
        gres[1]=globl_info.framebuffer_height;
        tres[0]=gres[0]/8;
        tres[1]=gres[1]/16;
        buf_ptr=khmalloc(2*(tres[0]*2+tres[1]*tres[0]*2+tres[0]*2));
        
    }
}
