#include "init.h"
#include "common.h"
#include "vgatext.h"
#include "dev/pic.h"
void initialize_crap(){
    init_idt();
    init_pic();
}