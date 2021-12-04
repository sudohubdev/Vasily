
struct pcidev{
    unsigned char bus,dev,func;
    unsigned char classcode,subclass;
    unsigned short vendorid,devid;
    struct pcidev* next,*prev;
};

void init_pci();
