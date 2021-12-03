#pragma once

    /*(edi is syscall code) */

int c_syscall(int edi,int esi,int edx,int ecx,int ebx,int eax,int ebp);
void syscall_sr();
