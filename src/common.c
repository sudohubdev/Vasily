#include "common.h"
char donemsg_str[] __attribute__((section(".data")))="ok\n";

char* donemsg=&donemsg_str[0];
