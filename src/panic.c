#include "panic.h"
#include "errorscn.h"



//void exception_handler(unsigned int eip,unsigned int code2,unsigned int flags,unsigned int cs,unsigned int edi,unsigned int esi,unsigned int ebp,unsigned int esp,unsigned int ebx,unsigned int edx,unsigned int ecx,unsigned int eax);

void __internal_panic(char *file, unsigned int line, char *msg) {
    file=file+1;
    extern unsigned int err_code;
  err_code=70;
  exception_handler(msg,0,line,0,0,0,0,0,0,0,0,0,0);
  set_term_colour(0xf0);
  /*putstring("\n__internal_panic(");
  putstring(file);
  putstring(", \0");
  putunum(line, 10);
  putstring(", \0");
  putstring(msg);
  putstring(")");*/
  asm("cli;hlt;");
}
