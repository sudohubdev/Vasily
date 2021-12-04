 
 .globl memset
 .globl memcpy
 .globl strcpy
 
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
            

            
