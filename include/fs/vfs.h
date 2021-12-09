#pragma once

#define vfsfile 0x1
#define vfsdir 0x2
#define vfsblk 0x3
#define vfschardev 0x4
#define vfspipe 0x5
#define vfslink 0x6
#define vfsmnt 0x8

struct vfs_node;

struct dirent{
    unsigned int inode;
     char name[128];
};

#define decl_open(name) int name(const struct vfs_node* in, int flags) 
#define decl_close(name) int name(int fd) 
#define decl_read(name) unsigned long name(int fd, void* buf,unsigned long count)
#define decl_write(name) unsigned long name(int fd,void* buf,unsigned long count)
#define decl_readdir(name) struct vfs_node* name(const struct vfs_node* dir)
#define decl_finddir(name) struct vfs_node* name(const char* in)

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

void init_vfs();
extern struct vfs_node *memroot;

int internal_mount(struct vfs_node *src,struct vfs_node *tar,const char* fsstring,unsigned long mountflags,const char* fsspec_flags);

decl_open(open);
decl_close(close);
decl_read(read);
decl_write(write);
decl_readdir(readdir);
decl_finddir(finddir);
