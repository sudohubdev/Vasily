#include "syscall.h"
#include "fs/vfs.h"
/*(edi is syscall code) */
int c_syscall(int edi, int esi, int edx, int ecx, int ebx, int eax, int ebp) {
    if(edi==0){
        open((struct vfs_node*)esi,edx);
    }
    return 0;
}
