#include "posix/errno.h"
#include "fs/mbr.h"
#include "fs/devfs.h"
#include "fs/vfs.h"
#include "common.h"
#include "vgatext.h"
#include "heap.h"
#include "klibc/string.h"

mbr_sect *mbr;


struct mbr_part{
        int inode;
        struct vfs_node* disk;
        struct mbr_part* next;
        unsigned int off;
};

struct mbr_part *mbrroot;

decl_read(mbr_read){
    int inode=fd_node_find(fd);
    
    struct mbr_part *it=mbrroot;
    
    while(it){
        
        if(it->inode==inode){
            int fd=it->disk->open(it->disk,0);
            read(fd,buf,count,off+it->off*512);
            
            close(fd);
            
            return 0;
        }
        
        it=it->next;
    }
    return ENOENT;
}
decl_write(mbr_write){
    int inode=fd_node_find(fd);
    
    struct mbr_part *it=mbrroot;
    
    while(it){
        
        if(it->inode==inode){
            int fd=it->disk->open(it->disk,0);
            write(fd,buf,count,off+it->off*512);
            
            close(fd);
            
            return 0;
        }
        
        it=it->next;
    }
    return ENOENT;
}

void init_mbr(){
    putstring("init_mbr...\n");
    struct vfs_node* it;
  //  it=finddir(memroot,"dev");
    extern struct vfs_node* devfs_root;
    it=devfs_root->child;
    mbr=khmalloc(512);
    struct mbr_part* mbrit=mbrroot=khmalloc(sizeof(struct mbr_part));
    int counter;
    while(it){
        counter=0;
        if(it->name[4]==0){
            int fd=open(it,0);

            it->read(fd,mbr,512,0);

            if(mbr->sig==0xaa55){
                for(unsigned int i=0;i<4;++i){

                    if(mbr->entry[i].parttype!=0&&mbr->entry[i].sizesect!=0){
                        ++counter;
                        struct vfs_node* newfile=devfs_int_creat(mbr_read,mbr_write);
                        memcpy(newfile->name,it->name,sizeof(it->name));
                        newfile->name[4]='0'+counter;
                        putstring("mbr file name: ");
                        putstring(newfile->name);
                        putstring("\n");
                    
                        mbrit->off=mbr->entry[i].lba;
                        mbrit->disk=it;
                        mbrit->inode=newfile->inode;
                        mbrit->next=khmalloc(sizeof(struct mbr_part));
                        mbrit=mbrit->next;

                    }
                }

            }
            memset(mbr,0,512);
            close(fd);
        }
        it=it->next;
        
    }
    
    

    khfree(mbr);
    putstring(donemsg_str);

}
