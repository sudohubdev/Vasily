#include "dev/sata.h"
#include "vgatext.h"
#include "dev/pci.h"
#include "common.h"
#include "mem.h"
#include "fs/devfs.h"
#include "fs/vfs.h"
#include "heap.h"
#include "idt.h"
#include "klibc/string.h"

#define ATA_CMD_READ_DMA_EX 0x25
#define ATA_CMD_WRITE_DMA_EX     0x35
#define HBA_PxIS_TFES (1 << 30)

typedef enum
{
	FIS_TYPE_REG_H2D	= 0x27,	// Register FIS - host to device
	FIS_TYPE_REG_D2H	= 0x34,	// Register FIS - device to host
	FIS_TYPE_DMA_ACT	= 0x39,	// DMA activate FIS - device to host
	FIS_TYPE_DMA_SETUP	= 0x41,	// DMA setup FIS - bidirectional
	FIS_TYPE_DATA		= 0x46,	// Data FIS - bidirectional
	FIS_TYPE_BIST		= 0x58,	// BIST activate FIS - bidirectional
	FIS_TYPE_PIO_SETUP	= 0x5F,	// PIO setup FIS - device to host
	FIS_TYPE_DEV_BITS	= 0xA1,	// Set device bits FIS - device to host
} FIS_TYPE;

void sata_irq(){
    asm("lgdt 0");
}__attribute__((interrupt));

#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	0x96690101	// Port multiplier

#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATA 1
#define AHCI_DEV_SEMB 2
#define AHCI_DEV_PM 3
#define AHCI_DEV_SATAPI 4

#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3
 
void*	AHCI_BASE;	// 4M
 
#define HBA_PxCMD_ST    0x0001
#define HBA_PxCMD_FRE   0x0010
#define HBA_PxCMD_FR    0x4000
#define HBA_PxCMD_CR    0x8000
 
/*
 * struct pcidev{
    unsigned char bus,dev,func;
    unsigned char classcode,subclass,irq;
    unsigned short vendorid,devid;
    struct pcidev* next,*prev;
};
*/
sata_device* satroot;

#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08
 

int sata_write(sata_device *in, uint32_t startl, uint32_t starth, uint32_t count, uint16_t *buf)
{
    ahci_port *port=in->portptr;
	port->is = (uint32_t) -1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter

	int slot = find_cmdslot(port,in->ctrl);
	if (slot == -1)
		return 0;

	ahci_header *cmdheader = (ahci_header*)port->clb;

	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D)/sizeof(uint32_t);	// Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;	// PRDT entries count

	ahci_cmd_tbl *cmdtbl = (ahci_cmd_tbl*)(cmdheader->ctba);
	memset(cmdtbl, 0, sizeof(ahci_cmd_tbl) +
 		(cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));
    int i;
	// 8K bytes (16 sectors) per PRDT
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint32_t) buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024-1;	// 8K bytes (this value should always be set to 1 less than the actual value)
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4*1024;	// 4K words
		count -= 16;	// 16 sectors
		

	}
        // Last entry
	cmdtbl->prdt_entry[i].dba = (uint32_t) buf;
	cmdtbl->prdt_entry[i].dbc = (count<<9)-1;	// 512 bytes per sector
	cmdtbl->prdt_entry[i].i = 1;
	// Setup command
	FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);
 
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_WRITE_DMA_EX;
 
	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode
 
	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);
 
	cmdfis->countl = count & 0xFF;
	cmdfis->counth = (count >> 8) & 0xFF;
 
	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		putstring("Port is hung\n");
		return 0;
	}
 
	port->ci = 1<<slot;	// Issue command
 
	// Wait for completion
	while (1)
	{       // putstring(".");

		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is & HBA_PxIS_TFES)	// Task file error
		{
			putstring("WRITE disk error\n");
			return 0;
		}
	}
 
	// Check again
	if (port->is & HBA_PxIS_TFES)
	{
		putstring("WRITE disk error\n");
		return 0;
	}
 
	return 1;
}

int sata_read(sata_device *in, uint32_t startl, uint32_t starth, uint32_t count, uint16_t *buf)
{
    if(count==0){
        return 1;
    }
    ahci_port *port=in->portptr;
	port->is = (uint32_t) -1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter

	int slot = find_cmdslot(port,in->ctrl);
	if (slot == -1)
		return 0;

	ahci_header *cmdheader = (ahci_header*)port->clb;

	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D)/sizeof(uint32_t);	// Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;	// PRDT entries count

	ahci_cmd_tbl *cmdtbl = (ahci_cmd_tbl*)(cmdheader->ctba);
	memset(cmdtbl, 0, sizeof(ahci_cmd_tbl) +
 		(cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));
    int i;
	// 8K bytes (16 sectors) per PRDT
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint32_t) buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024-1;	// 8K bytes (this value should always be set to 1 less than the actual value)
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4*1024;	// 4K words
		count -= 16;	// 16 sectors
		

	}
        // Last entry
	cmdtbl->prdt_entry[i].dba = (uint32_t) buf;
	cmdtbl->prdt_entry[i].dbc = (count<<9)-1;	// 512 bytes per sector
	cmdtbl->prdt_entry[i].i = 1;
	// Setup command
	FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);
 
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_READ_DMA_EX;
 
	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode
 
	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);
 
	cmdfis->countl = count & 0xFF;
	cmdfis->counth = (count >> 8) & 0xFF;
 
	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		putstring("Port is hung\n");
		return 0;
	}
 
	port->ci = 1<<slot;	// Issue command
 
	// Wait for completion
	while (1)
	{       // putstring(".");

		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is & HBA_PxIS_TFES)	// Task file error
		{
			putstring("Read disk error\n");
			return 0;
		}
	}
 
	// Check again
	if (port->is & HBA_PxIS_TFES)
	{
		putstring("Read disk error\n");
		return 0;
	}
 
	return 1;
}
 
// Find a free command list slot
int find_cmdslot(ahci_port *port,ahci_mem *abar)
{
	// If not set in SACT and CI, the slot is free

	uint32_t slots = (port->sact | port->ci);

    int cmdslots= (abar->cap & 0x0f00)>>8 ; // Bit 8-12

	for (int i=0; i<cmdslots; i++)
	{
		if ((slots&1) == 0)
			return i;
		slots >>= 1;
	}
	putstring("Cannot find free command list entry\n");
	return -1;
}

void port_rebase(ahci_port *port, int portno)
{
	stop_cmd(port);	// Stop command engine
 
    
    port->cmd=port->cmd & (~(0b1100000000010001));


	// Command list offset: 1K*portno
	// Command list entry size = 32
	// Command list entry maxim count = 32
	// Command list maxim size = 32*32 = 1K per port
	port->clb = AHCI_BASE + (portno<<10);
	port->clbu = 0;
	memset((void*)(port->clb), 0, 1024);
	// FIS offset: 32K+256*portno
	// FIS entry size = 256 bytes per port
	port->fb = AHCI_BASE + (32<<10) + (portno<<8);

	port->fbu = 0;
	memset((void*)(port->fb), 0, 256);

    
    port->serr=1;

    
	// Command table offset: 40K + 8K*portno
	// Command table size = 256*32 = 8K per port
	ahci_header *cmdheader = (ahci_header*)(port->clb);

	for (int i=0; i<32; i++)
	{
		cmdheader[i].prdtl = 8;	// 8 prdt entries per command table
					// 256 bytes per command table, 64+16+48+16*8
		// Command table offset: 40K + 8K*portno + cmdheader_index*256
		cmdheader[i].ctba = AHCI_BASE + (40<<10) + (portno<<13) + (i<<8);
		cmdheader[i].ctbau = 0;
		memset((void*)cmdheader[i].ctba, 0, 256);
	}

	start_cmd(port);	// Start command engine
}
 
// Start command engine
void start_cmd(ahci_port *port)
{

	// Wait until CR (bit15) is cleared
	while (port->cmd & HBA_PxCMD_CR)
		;
 
	// Set FRE (bit4) and ST (bit0)
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST; 

}
 
// Stop command engine
void stop_cmd(ahci_port *port)
{
	// Clear ST (bit0)
	port->cmd &= ~HBA_PxCMD_ST;
 
	// Clear FRE (bit4)
	port->cmd &= ~HBA_PxCMD_FRE;
 
	// Wait until FR (bit14), CR (bit15) are cleared
	while(1)
	{
		if (port->cmd & HBA_PxCMD_FR)
			continue;
		if (port->cmd & HBA_PxCMD_CR)
			continue;
		break;
	}
 
}

static int check_type(ahci_port *port)
{
	uint32_t ssts = port->ssts;
 
	uint8_t ipm = (ssts >> 8) & 0x0F;
	uint8_t det = ssts & 0x0F;
 
	if (det != HBA_PORT_DET_PRESENT)	// Check drive status
		return AHCI_DEV_NULL;
	if (ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;
 
	switch (port->sig)
	{
	case SATA_SIG_ATAPI:
		return AHCI_DEV_SATAPI;
	case SATA_SIG_SEMB:
		return AHCI_DEV_SEMB;
	case SATA_SIG_PM:
		return AHCI_DEV_PM;
	default:
		return AHCI_DEV_SATA;
	}
}

static int drive_counter=0;


decl_read(sata_devfs_read){
    int inode=fd_node_find(fd)->inode;
    sata_device* it=satroot;
    while(it)
    {
        if(it->devfs_inode==inode){
            unsigned int off2=off%512;

            char* buf2=khmalloc(512);
                
            sata_read(it,off/512,0,1,buf2);
            memcpy(buf,buf2+off2,512-off2);
            buf+=512-off2;
            off+=512-off2;
            count-=512-off2;
            
            unsigned int count2=count%512;

            sata_read(it,off/512,0,count/512,buf);
                
            off+=count;
            sata_read(it,off/512,0,1,buf2);
            memcpy(buf+count-count2,buf2,count%512);
            
            khfree(buf2);
            
        }
        
            
            
        it=it->next;
    }
    
    
    return 0;// ide_read_sectors(inode,count/512,off/512,0x10,(unsigned int)buf);

    
}   

decl_write(sata_devfs_write){
    int inode=fd_node_find(fd)->inode;
    sata_device* it=satroot;

    while(it)
    {
            unsigned int off2=off%512;

            char* buf2=khmalloc(512);
            
            sata_read(it,off/512,0,1,buf2);

            memcpy(buf2+off2,buf,512-off2);
            sata_write(it,off/512,0,1,buf2);
            
            buf+=512-off2;
            off+=512-off2;
            count-=512-off2;
            
            unsigned int count2=count%512;

            sata_write(it,off/512,0,count/512,buf);
                
            off+=count;
            sata_read(it,off/512,0,1,buf2);
            memcpy(buf2,buf+count-count2,count%512);
            sata_write(it,off/512,0,1,buf2);
            
            
            khfree(buf2);
            
            
        it=it->next;
    }
    
    return 0; //ide_write_sectors(inode,count/512,off/512,0x10,(unsigned int)buf);


}

int controller_count=0;

static void sata_ctrl_init(struct pcidev* it){
    int bar5=readconfword32(it->bus,it->dev,it->func,0x24)&(~(unsigned int)0b1111);
    extern pagedir_t* krnl_pagedir;
    krnl_map_page(krnl_pagedir, bar5, 0x800000+controller_count*0x1000);
    krnl_map_page(krnl_pagedir, bar5+0x1000, 0x800000+controller_count*0x1000+0x1000);
    struct ahci_mem* reg=0x800000+controller_count*0x2000;
    ++controller_count;
    unsigned int pi=reg->pi;
    int i=0;
    sata_device* it2=satroot=khmalloc(sizeof(sata_device));
    
    AHCI_BASE=khmalloc(0x110000);
    AHCI_BASE=AHCI_BASE+(0x80-((unsigned int)AHCI_BASE%0x80));
    
    while(i<32){
        if (pi&1){
            int dt = check_type(&reg->ports[i]);
            if(dt!=AHCI_DEV_NULL){
                it2->next=khmalloc(sizeof(sata_device));
                struct vfs_node *newfile = devfs_int_creat(sata_devfs_read,sata_devfs_write); 
               // (j == 0 ? ideit : ideit->second)->ide_devices[i].devfs_inode = newfile->inode;
                reg->ghc=(unsigned int)(1<<31);
                reg->ghc=(unsigned int)(1<<0);
                reg->ghc=(unsigned int)(1<<31);
                reg->ghc=(unsigned int)(1<<1);
                port_rebase(&reg->ports[i],i);
                
                
                newfile->name[0]='s';
                newfile->name[1]='d';
                newfile->name[2]='A'+(drive_counter/10);
                newfile->name[3]='a'+(drive_counter++);
                it2->devfs_inode=newfile->inode;
                it2->port=i;
                it2->portptr=&reg->ports[i];
                it2->ctrl=reg;
                


                putstring("dev num: ");
                putunum(i,10);
                putstring(" devfs inode ");
                putunum(newfile->inode,10);
                putstring(" devfs filename ");
                putstring(newfile->name);
                putstring("\n");
                it2=it2->next;
                
            }
            
        }
        pi>>=1;
        ++i;
    }

}


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
                putstring("\n");
                sata_ctrl_init(it);
              
        }
        
          it=it->next;   
      }

      
      putstring(donemsg_str);
      

}
