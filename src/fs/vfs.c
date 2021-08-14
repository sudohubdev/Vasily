#include "fs/vfs.h"
#include "vgatext.h"
#include "common.h"
#include "heap.h"

extern unsigned int cursorpos[2];

struct vfs_node* root, *devfs;

void init_vfs(){
    putstring("init_vfs...");
    
    
    putstring(donemsg);
}
