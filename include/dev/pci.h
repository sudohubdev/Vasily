
extern struct pcidev* pciroot;

struct pcidev{
    unsigned char bus,dev,func;
    unsigned char classcode,subclass;
    unsigned short vendorid,devid;
    struct pcidev* next,*prev;
};

unsigned short readconfword(unsigned char bus, unsigned char dev, unsigned char func, unsigned char offset);

void init_pci();
