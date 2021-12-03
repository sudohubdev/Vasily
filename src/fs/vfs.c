#include "fs/vfs.h"
#include "fs/devfs.h"
#include "vgatext.h"
#include "common.h"
#include "heap.h"

extern unsigned int cursorpos[2];

struct vfs_node* root, *devfs;

/*
struct vfs_node{
    char isdir;
    char ismounted;
    decl_open((*open));
    decl_close((*close));
    decl_read((*read));
    decl_write((*write));
    decl_finddir((*finddir));
    decl_open((*mount_open));
    decl_close((*mount_close));
    decl_read((*mount_read));
    decl_write((*mount_write));
    decl_finddir((*mount_finddir));
    char name[255];
    unsigned short perms;
    unsigned int uid,gid;
    unsigned long long sz;
};

#define decl_open(name) int name(const char* path, int flags) 
#define decl_close(name) int name(int fd) 
#define decl_read(name) unsigned long name(int fd, void* buf,unsigned long count)
#define decl_write(name) unsigned long name(int fd,void* buf,unsigned long count)
#define decl_finddir(name) struct vfs_node* name(struct vfs_node* dir,char* n)
*/

void init_vfs(){
    putstring("init_vfs...");
    
    root=khmalloc(sizeof(struct vfs_node));
    
    devfs_init();
    
    putstring(donemsg);
}

