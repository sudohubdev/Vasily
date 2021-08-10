#include "fs/vfs.h"
#include "vgatext.h"
#include "common.h"
#include "heap.h"

struct vfs_node* root, *devfs;

void init_vfs(){
    putstring("init_vfs...");
    
    putstring(donemsg);
}
