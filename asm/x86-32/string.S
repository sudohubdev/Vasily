 
 .globl memset
 .globl memcpy
 .globl strcpy
 .globl strlen
 
    memset:    
        movl 4(%esp),%edi;  
        movb 8(%esp),%al;   
        movl 12(%esp),%ecx;       
        pushfl;
        cld;
        rep stosb;
        popfl;
        ret;
    memcpy:     
        movl 8(%esp),%esi;
        movl 4(%esp),%edi;
        movl 12(%esp),%ecx;
        pushfl;
        cld;
        rep movsb;
        popfl;
        ret;
    strcpy: 
        pushfl
        movl 8(%esp),%edi
        movl 12(%esp),%esi
        cld
        jmp a
        b:
            movsb 
        a:
        cmpb $0,(%esi)
        jnz b
        popfl
        ret
    
  /*  strlen:
        pushl %ecx
        pushl %esi
        pushfl 
        cld
        movl 16(%esp),%esi
        xorl %ecx,%ecx
        jmp L2
        L1:
        inc %ecx
        inc %esi
        L2:
        cmpb $0,(%esi)
        jnz L1
        movl %ecx,%eax
        popfl
        popl %esi
        popl %ecx
        ret
*/
