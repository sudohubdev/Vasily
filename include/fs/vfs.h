#pragma once
#define decl_open(name) int name(const char* path, int flags) 
#define decl_close(name) int name(int fd) 
#define decl_read(name) unsigned long name(int fd, void* buf,unsigned long count)
#define decl_write(name) unsigned long name(int fd,void* buf,unsigned long count)


struct vfs_node{
    char isdir;
    char ismounted;
    decl_open((*open));
    decl_close((*close));
    decl_read((*read));
    decl_write((*write));
    char name[255];
    unsigned short perms;
    unsigned int uid,gid;
    unsigned long long sz;
};

void init_vfs();



int open(const char* path, int flags);
int close(int fd);
unsigned long read(int fd, void* buf,unsigned long count);
unsigned long write(int fd,void* buf,unsigned long count);
