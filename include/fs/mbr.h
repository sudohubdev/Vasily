

typedef struct lepartentry{
    unsigned char attrib;
    char chsaddrstart[3];
    unsigned char parttype;
    char chsaddrlast[3];
    unsigned int lba;
    unsigned int sizesect;
    
}__attribute__((packed))mbr_entry;

typedef struct lembr_sect{
    char code[440];
    unsigned int diskid;
    short bloat;
    mbr_entry entry[4];
    unsigned short sig;//0x55aa
    
} __attribute__((packed)) mbr_sect;

void init_mbr();
