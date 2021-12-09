#include "panic.h"
void __internal_panic(char *file, unsigned int line, char *msg) {
  set_term_colour(0xf0);
  putstring("\n__internal_panic(");
  putstring(file);
  putstring(", \0");
  putunum(line, 10);
  putstring(", \0");
  putstring(msg);
  putstring(")");
  asm("cli;hlt;");
}