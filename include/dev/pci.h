
struct pcidev{
    unsigned char bus,dev,func;
    unsigned char classcode,subclass;
    unsigned short vendorid;
    unsigned short devid;
    struct pcidev* next;
};

void init_pci();
