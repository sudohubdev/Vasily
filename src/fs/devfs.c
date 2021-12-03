#include "fs/devfs.h"
#include "heap.h"

/*
struct vfs_node{
    char isdir;
    decl_open((*open));
    decl_close((*close));
    decl_read((*read));
    decl_write((*write));
    decl_finddir((*finddir));
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

struct vfs_node* devfs_root;

decl_open(devfs_open){
    
}
decl_close(devfs_close){
    
}
decl_readdir(devfs_readdir){
    
}

decl_read(devfs_read){
    
}

decl_write(devfs_write){
    
}


void devfs_init(){
        devfs_root=khmalloc(sizeof(struct vfs_node));
        devfs_root->perms=0b111101101;
        devfs_root->open=devfs_open;
        devfs_root->close=devfs_close;
        devfs_root->read=devfs_read;
        devfs_root->write=devfs_write;
        devfs_root->readdir=devfs_readdir;
}
