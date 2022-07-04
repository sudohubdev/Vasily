#include "fs/fat.h"
#include "fs/vfs.h"
#include "fs/devfs.h"
#include "heap.h"
#include "vgatext.h"
#include "klibc/string.h"

struct fat_fs_entry{
  int type;//shoudl always be fat16  for now
  unsigned int cluster_sizeinbytes;
  unsigned int fatsector;
  unsigned int bytespersector;
  struct fat16_bpb bpb;
  unsigned char *fattab;
  struct fat_fs_entry *next;
  struct vfs_node* disk;
};

struct fat_fs_entry *fatroot, *fatiter=0;
struct fat16_bpb detect;

int detect_fat(int fd){
    read(fd,&detect,512,0);
    if(detect.eb3c90[0]==0xeb&&detect.eb3c90[2]==0x90||detect.eb3c90[0]==0xe9){
        if(detect.sig==0x28||detect.sig==0x29){
            return 1;
        }  
        return 0;
    }
    return 0;
}

void init_fat(){
    struct vfs_node* iter;
    struct vfs_node* root=iter=devfs_root->child;
    fatroot=khmalloc(sizeof(struct fat_fs_entry));
    
    do{
            int fd=open(iter,0);
            if(detect_fat(fd)){
                putstring(iter->name);
                putstring(" has fat on it\n");
                
                
                if(fatiter==0){
                    fatiter=fatroot;
                }
                else{
                    fatiter->next=khmalloc(sizeof(struct fat_fs_entry));
                    fatiter=fatiter->next;
                }
                fatiter->type=16;
                fatiter->cluster_sizeinbytes=detect.sectorspercluster*detect.bytespersector;
                fatiter->fatsector=detect.reservedsectors;
                fatiter->bytespersector=detect.bytespersector;
                memcpy(&fatiter->bpb,&detect,sizeof(detect));
                fatiter->fattab=khmalloc(detect.bytespersector);
                fatiter->disk=iter;
                
                
            }
            close(fd);
        
        
    }while(iter=iter->next);
    
}
