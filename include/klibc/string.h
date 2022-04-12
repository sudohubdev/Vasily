/* 
    memset:    
        movl 4(%esp),%edi;  
        movb 8(%esp),%al;   
        movl 12(%esp),%ecx;        pushfl;\
        cld;
        rep stosb;
        popfl;
        ret;
    memcpy:     
        movl 4(%esp),%esi;
        movl 8(%esp),%edi;
        movl 12(%esp),%ecx;
        pushfl;
        cld;
        rep movsb;
        popfl;
        ret;
*/
void * memset ( void * ptr, int value, unsigned int num );
void * memcpy ( void * destination, const void * source, unsigned int num);
char * strcpy ( char * destination, const char * source );
int strcmp(const char *l, const char *r);
unsigned int strlen(const char* str);
