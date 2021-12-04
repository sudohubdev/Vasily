#include "fs/vfs.h"
#include "fs/devfs.h"
#include "vgatext.h"
#include "common.h"
#include "heap.h"
#include "klibc/string.h"

extern unsigned int cursorpos[2];

struct vfs_node* memroot;

/*
struct vfs_node{
    decl_open((*open));
    decl_close((*close));
    decl_read((*read));
    decl_write((*write));
    decl_readdir((*readdir));
    decl_finddir((*finddir));
    struct vfs_node* mountpoint; //mountpoint is null if nothing is mounted
    char name[255];
    unsigned short perms;
    unsigned int uid,gid;
    unsigned long long sz;
    unsigned int inode;
    struct vfs_node* next,*child;
    char type;
};

#define decl_open(name) int name(const char* path, int flags) 
#define decl_close(name) int name(int fd) 
#define decl_read(name) unsigned long name(int fd, void* buf,unsigned long count)
#define decl_write(name) unsigned long name(int fd,void* buf,unsigned long count)
#define decl_finddir(name) struct vfs_node* name(struct vfs_node* dir,char* n)
*/

void init_vfs(){
    putstring("init_vfs...");
    
    memroot=khmalloc(sizeof(struct vfs_node));
    memroot->perms=0b111101101;
    memroot->type=vfsdir;
    memroot->open=open;
    memroot->close=close;
    memroot->read=read;
    memroot->write=write;
    memroot->readdir=readdir;
    memroot->finddir=finddir;
    memroot->mountpoint=0;
    strcpy(memroot->name,"memroot");
    memroot->next=0;
    
    
    devfs_init();
    

    
    putstring(donemsg);
}


decl_open(open){

    return 0;
}
decl_close(close){
    return 0;
}
decl_read(read){
    return 0;
}
decl_write(write){
    return 0;
}
decl_readdir(readdir){
    return 0;
}
decl_finddir(finddir){
    return 0;
}

