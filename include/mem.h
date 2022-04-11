#pragma once

void init_paging();
typedef union{
    struct{
            unsigned int p:1;
            unsigned int r:1;
            unsigned int u:1;
            unsigned int w:1;
            unsigned int c:1;
            unsigned int a:1;
            unsigned int d:1;
            unsigned int z:1;
            unsigned int g:1;
            unsigned int krnl_info:3;
            unsigned int phys_addr:20;

    };
    unsigned int raw;
}page_t;
typedef union{
        struct{
            unsigned int p:1;
            unsigned int r:1;
            unsigned int u:1;
            unsigned int w:1;
            unsigned int d:1;
            unsigned int a:1;
            unsigned int z:1;
            unsigned int s:1;
            unsigned int g:1;
            unsigned int krnl_info:3;
            unsigned int page_table_addr:20;/*&page_t page_table[1024]*/
        };
        unsigned int raw;
}pagedir_t;
void set_usage_bm(unsigned int addr);
void unset_usage_bm(unsigned int addr);
int test_usage_bm(unsigned int addr);
void krnl_map_page_uncacheable(pagedir_t *p, unsigned int physaddr, unsigned int virtaddr);
