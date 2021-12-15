#include "vgatext.h"
#include "klibc/string.h"
int e0_mode=0;
extern unsigned char booticon_tga[];
void keyboard_handler(unsigned int scancode){
    if(e0_mode==1){
         e0_mode=0;
         extern int enable_log;
         if(scancode==0x48){
             enable_log=!enable_log;
             if(enable_log==1){
                memset((void*)globl_info.framebuffer_addr,0,gres[0]*gres[1]*(globl_info.framebuffer_bpp/8));
                buf_flush();
             }
             else{
                memset((void*)globl_info.framebuffer_addr,0,gres[0]*gres[1]*(globl_info.framebuffer_bpp/8));
                if(globl_info.framebuffer_type!=2){
                    for(int x=0;x<124;++x)
                        for(int y=0;y<128;++y){
                            putpixel(*((unsigned int*)&booticon_tga[(x/4+31*(y/4))*4+18]), x+(gres[0]/2-62), y+(gres[1]/2-64));
                    }
                }
             }
         }

    }

    else{
         
    }
     
    if(scancode==0xe0){
        e0_mode=1;
    }

 }
