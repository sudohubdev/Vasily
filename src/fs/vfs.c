#include "fs/vfs.h"
#include "fs/devfs.h"
#include "vgatext.h"
#include "common.h"
#include "heap.h"
#include "klibc/string.h"
#include "posix/errno.h"
#include "multitask.h"

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
#define decl_finddir(name) struct vfs_node* name(struct vfs_node* dir,char* nam)
*/
struct vfs_node* mounts[1024];

int internal_mount(struct vfs_node* src, struct vfs_node* tar, const char* fsstring, long unsigned int mountflags, const char* fsspec_flags){
    for(int i=0;i<1024;++i){
        if(mounts[i]==0){
            mounts[i]=src;
            tar->mountpoint=khmalloc(sizeof(struct vfs_node));
            tar->mountindex=i;

            return 0;
        }
            //look for an empty mount entry
    }
    return ENOMEM;
    
}

int umount(struct vfs_node *in){
    if(in->mountpoint==0){
        return EINVAL;
    }
    
    khfree(in->mountpoint);
    mounts[in->mountindex]=0;
    return 0;
}

void init_vfs(){
    putstring("init_vfs...");
    
    
    memroot=khmalloc(sizeof(struct vfs_node));
    memroot->perms=0b111101101;
    memroot->type=vfsdir;
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

struct vfs_node* fd_node_find(int fd){
    return current_task->fds[fd];
}


decl_open(open){
    int i;
    for(i=0;i<1024;++i){
        if(current_task->fds[i]==0){
            current_task->fds[i]=in;
            return i;
        }
    }
    return 0;
}
decl_close(close){

    current_task->fds[fd]=0;
    return 0;
}
decl_read(read){
    if(count==0){
        return 0;
    }
    struct vfs_node* it=fd_node_find(fd);
    return it->read(fd,buf,count,off);
    
}
decl_write(write){
    return 0;
}
decl_readdir(readdir){
    return 0;
}
decl_finddir(finddir){
    struct vfs_node* it=dir->child;
    
    while(it){
        if(!strcmp(it->name,in)){
            return it;
        }   
        it=it->next;
    }
    
    
    return 0;
}

