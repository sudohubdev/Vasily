#include "vgatext.h"
struct idt_info{
    unsigned short limit;
    unsigned int base;
}__attribute__((packed));
struct idt{
    unsigned short offset_l;
    unsigned short selector;
    unsigned char z;
    unsigned char type_attr;
    unsigned short offset_h;
}__attribute__((packed));
void init_idt();