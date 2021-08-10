#ifndef _panic_
#define _panic_

void __internal_panic(char* file,unsigned int line,char* msg);

#include"vgatext.h"
#define panic(msg) __internal_panic(__FILE__,__LINE__,msg);

#endif