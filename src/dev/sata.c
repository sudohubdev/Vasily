#include "dev/sata.h"
#include "vgatext.h"
#include "dev/pci.h"
#include "common.h"

/*
 * struct pcidev{
    unsigned char bus,dev,func;
    unsigned char classcode,subclass,irq;
    unsigned short vendorid,devid;
    struct pcidev* next,*prev;
};
*/
void init_sata(){
      putstring("init_sata()...");
      
      struct pcidev* it=pciroot;
      
      while(it){
        
          if(it->classcode==1&&it->subclass==6){
                putstring("\nsata controller found at bus ");
                putunum(it->bus, 10);
                putstring(" dev ");
                putunum(it->dev, 10);
                putstring(" func ");
                putunum(it->func, 10);
               // putunum(readconfword(it->bus,it->dev,it->func,0),2);
          }
        
          it=it->next;   
      }
      
      putstring(donemsg_str);
      

}
