asm("\
    memset:    \
        movl 4(%esp),%edi;   \
        movb 8(%esp),%al;   \
        movl 12(%esp),%ecx;\
        pushfl;\
        cld;\
        rep stosb;\
        popfl;\
        ret;\
    memcpy:     \
        movl 4(%esp),%esi;\
        movl 8(%esp),%edi;\
        movl 12(%esp),%ecx;\
        pushfl;\
        cld;\
        rep movsb;\
        popfl;\
        ret;\
");
