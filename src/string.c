#include "klibc/string.h"
unsigned int strlen(const char* str){
    unsigned int i;
    for(i=0;str[i];++i);
    return i;
}
