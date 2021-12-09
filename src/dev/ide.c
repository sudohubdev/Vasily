#include "dev/ide.h"
#include "common.h"
#include "dev/pci.h"
#include "fs/devfs.h"
#include "io.h"
#include "vgatext.h"
#include "heap.h"
#include "posix/errno.h"
int dev_it = 0;


int idectrl_id_count=0;
struct IDEChannelRegisters* idechannelroot,*ideit;

unsigned char ide_buf[2048] = {0};
static unsigned char ide_irq_invoked = 0;
static unsigned char atapi_packet[12] = {0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};



struct IDEChannelRegisters* getchannel(unsigned int id){
    struct IDEChannelRegisters* iter=idechannelroot;
    while(iter){
        if(iter->id==id){
            goto breakoutofloop;
        }
        if(iter->second->id==id){
            iter=iter->second;
            goto breakoutofloop;
        }
        iter=iter->next;
    }
    breakoutofloop:
    return iter;
}

void ide_read_buffer(struct IDEChannelRegisters* channel, unsigned char reg,
                     unsigned int buffer, unsigned int quads) {
  /* WARNING: This code contains a serious bug. The inline assembly trashes ES
   * and ESP for all of the code the compiler generates between the inline
   *           assembly blocks.
   */
  if (reg > 0x07 && reg < 0x0C)
    ide_write(channel, ATA_REG_CONTROL, 0x80 | channel->nIEN);
  asm("pushw %es; movw %ds, %ax; movw %ax, %es");
  if (reg < 0x08)
    insl(channel->base + reg - 0x00, (unsigned int*)buffer, quads);
  else if (reg < 0x0C)
    insl(channel->base + reg - 0x06, (unsigned int*)buffer, quads);
  else if (reg < 0x0E)
    insl(channel->ctrl + reg - 0x0A,(unsigned int*) buffer, quads);
  else if (reg < 0x16)
    insl(channel->bmide + reg - 0x0E, (unsigned int*)buffer, quads);
  asm("popw %es;");
  if (reg > 0x07 && reg < 0x0C)
    ide_write(channel, ATA_REG_CONTROL, channel->nIEN);
}

unsigned char ide_read(struct IDEChannelRegisters* channel, unsigned char reg) {
  unsigned char result=0;
  if (reg > 0x07 && reg < 0x0C)
    ide_write(channel, ATA_REG_CONTROL, 0x80 | channel->nIEN);
  if (reg < 0x08)
    result = inb(channel->base + reg - 0x00);
  else if (reg < 0x0C)
    result = inb(channel->base + reg - 0x06);
  else if (reg < 0x0E)
    result = inb(channel->ctrl + reg - 0x0A);
  else if (reg < 0x16)
    result = inb(channel->bmide + reg - 0x0E);
  if (reg > 0x07 && reg < 0x0C)
    ide_write(channel, ATA_REG_CONTROL, channel->nIEN);
  return result;
}

void ide_write(struct IDEChannelRegisters *channel, unsigned char reg, unsigned char data) {
  if (reg > 0x07 && reg < 0x0C)
    ide_write(channel, ATA_REG_CONTROL, 0x80 | channel->nIEN);
  if (reg < 0x08)
    outb(channel->base + reg - 0x00, data);
  else if (reg < 0x0C)
    outb(channel->base + reg - 0x06, data);
  else if (reg < 0x0E)
    outb(channel->ctrl + reg - 0x0A, data);
  else if (reg < 0x16)
    outb(channel->bmide + reg - 0x0E, data);
  if (reg > 0x07 && reg < 0x0C)
    ide_write(channel, ATA_REG_CONTROL, channel->nIEN);
}
void ctrl_init(unsigned int b0, unsigned int b1, unsigned int b2,
               unsigned int b3, unsigned int b4) {
  int  k, count = 0;

  
  // 1- Detect I/O Ports which interface IDE Controller:
  ideit->id=idectrl_id_count++;
  ideit->base = (b0 & 0xFFFFFFFC) + 0x1F0 * (!b0);
  ideit->ctrl = (b1 & 0xFFFFFFFC) + 0x3F6 * (!b1);
  ideit->bmide = (b4 & 0xFFFFFFFC) + 0;   // Bus Master IDE
  ideit->second=khmalloc(sizeof(struct IDEChannelRegisters));
  
  
  ideit->second->base = (b2 & 0xFFFFFFFC) + 0x170 * (!b2);
  ideit->second->ctrl = (b3 & 0xFFFFFFFC) + 0x376 * (!b3);
  ideit->second->bmide = (b4 & 0xFFFFFFFC) + 8; // Bus Master IDE
  ideit->second->id=idectrl_id_count++;
  // 2- Disable IRQs:
  ide_write(ideit, ATA_REG_CONTROL, 2);
  ide_write(ideit->second, ATA_REG_CONTROL, 2);

  for (char i = 0; i < 2; i++)
    for (char j = 0; j < 2; j++) {

      unsigned char err = 0, type = IDE_ATA, status;
      (i==0?ideit:ideit->second)->ide_devices[count].Reserved = 0; // Assuming that no drive here.

      // (I) Select Drive:
      ide_write(i==0?ideit:ideit->second, ATA_REG_HDDEVSEL, 0xA0 | (j << 4)); // Select Drive.
      io_wait();
      io_wait();
      io_wait();
      io_wait();

      // Wait 1ms for drive select to work.

      // (II) Send ATA Identify Command:
      ide_write(i==0?ideit:ideit->second, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
      io_wait();
      io_wait();
      io_wait();
      io_wait();
      // This function should be implemented in your OS. which waits for 1 ms.
      // it is based on System Timer Device Driver.

      // (III) Polling:
      if (ide_read(i==0?ideit:ideit->second, ATA_REG_STATUS) == 0)
        continue; // If Status = 0, No Device.

      while (1) {
        status = ide_read(i==0?ideit:ideit->second, ATA_REG_STATUS);
        if ((status & ATA_SR_ERR)) {
          err = 1;
          break;
        } // If Err, Device is not ATA.
        if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ))
          break; // Everything is right.
      }

      // (IV) Probe for ATAPI Devices:

      if (err != 0) {
        unsigned char cl = ide_read(i==0?ideit:ideit->second, ATA_REG_LBA1);
        unsigned char ch = ide_read(i==0?ideit:ideit->second, ATA_REG_LBA2);

        if (cl == 0x14 && ch == 0xEB)
          type = IDE_ATAPI;
        else if (cl == 0x69 && ch == 0x96)
          type = IDE_ATAPI;
        else
          continue; // Unknown Type (may not be a device).

        ide_write(i==0?ideit:ideit->second, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
        io_wait();
        io_wait();
        io_wait();
        io_wait();
      }

      // (V) Read Identification Space of the Device:
      ide_read_buffer(i==0?ideit:ideit->second, ATA_REG_DATA, (unsigned int)ide_buf, 128);
      // (VI) Read Device Parameters:
      (i==0?ideit:ideit->second)->ide_devices[count].Reserved = 1;
      (i==0?ideit:ideit->second)->ide_devices[count].Type = type;
      (i==0?ideit:ideit->second)->ide_devices[count].Channel = idectrl_id_count+i;
      (i==0?ideit:ideit->second)->ide_devices[count].Drive = j;
      (i==0?ideit:ideit->second)->ide_devices[count].Signature =
          *((unsigned short *)(ide_buf + ATA_IDENT_DEVICETYPE));
      (i==0?ideit:ideit->second)->ide_devices[count].Capabilities =
          *((unsigned short *)(ide_buf + ATA_IDENT_CAPABILITIES));
      (i==0?ideit:ideit->second)->ide_devices[count].CommandSets =
          *((unsigned int *)(ide_buf + ATA_IDENT_COMMANDSETS));

      // (VII) Get Size:
      if ((i==0?ideit:ideit->second)->ide_devices[count].CommandSets & (1 << 26))
        // Device uses 48-Bit Addressing:
        (i==0?ideit:ideit->second)->ide_devices[count].Size =
            *((unsigned int *)(ide_buf + ATA_IDENT_MAX_LBA_EXT));
      else
        // Device uses CHS or 28-bit Addressing:
        (i==0?ideit:ideit->second)->ide_devices[count].Size =
            *((unsigned int *)(ide_buf + ATA_IDENT_MAX_LBA));

      // (VIII) String indicates model of device (like Western Digital HDD and
      // SONY DVD-RW...):
      for (k = 0; k < 40; k += 2) {
        (i==0?ideit:ideit->second)->ide_devices[count].Model[k] = ide_buf[ATA_IDENT_MODEL + k + 1];
        (i==0?ideit:ideit->second)->ide_devices[count].Model[k + 1] = ide_buf[ATA_IDENT_MODEL + k];
      }
      (i==0?ideit:ideit->second)->ide_devices[count].Model[40] = 0; // Terminate String.

      count++;
    }

  // 4- Print Summary:
  for(int j=0;j<2;j++)
  for (int i = 0; i < 4; i++)
    if ((j==0?ideit:ideit->second)->ide_devices[i].Reserved == 1) {
      // printk(" Found %s Drive %dGB - %s\n",(const char *[]){"ATA",
      // "ATAPI"}[ide_devices[i].Type], ide_devices[i].Size / 1024 / 1024 /
      // 2,ide_devices[i].Model);
      struct vfs_node *newfile = devfs_int_creat();
      (j==0?ideit:ideit->second)->ide_devices[i].devfs_inode=newfile->inode;
      putstring("dev model: ");
      putstring((j==0?ideit:ideit->second)->ide_devices[i].Model);
      putstring(" devfs inode ");
      putunum((j==0?ideit:ideit->second)->ide_devices[i].devfs_inode,10);
      putstring("\n");
      ++dev_it;
    }
 
}
unsigned char ide_polling(struct IDEChannelRegisters *channel, unsigned int advanced_check) {
 
   // (I) Delay 400 nanosecond for BSY to be set:
   // -------------------------------------------------
   for(int i = 0; i < 4; i++)
      ide_read(channel, ATA_REG_ALTSTATUS); // Reading the Alternate Status port wastes 100ns; loop four times.
 
   // (II) Wait for BSY to be cleared:
   // -------------------------------------------------
   while (ide_read(channel, ATA_REG_STATUS) & ATA_SR_BSY)
      ; // Wait for BSY to be zero.
 
   if (advanced_check) {
      unsigned char state = ide_read(channel, ATA_REG_STATUS); // Read Status Register.
 
      // (III) Check For Errors:
      // -------------------------------------------------
      if (state & ATA_SR_ERR)
         return 2; // Error.
 
      // (IV) Check If Device fault:
      // -------------------------------------------------
      if (state & ATA_SR_DF)
         return 1; // Device Fault.
 
      // (V) Check DRQ:
      // -------------------------------------------------
      // BSY = 0; DF = 0; ERR = 0 so we should check for DRQ now.
      if ((state & ATA_SR_DRQ) == 0)
         return 3; // DRQ should be set
 
   }
 
   return 0; // No Error.
 
}

unsigned char ide_ata_access(unsigned char direction,unsigned char drive_inode,
                             unsigned int lba, unsigned char numsects,
                             unsigned short selector, unsigned int edi) {
  struct IDEChannelRegisters* iter=idechannelroot;
  unsigned int drive=0,channel;
  while(iter){
        for(unsigned int i=0;i<3;++i){
            if(iter->ide_devices[i].devfs_inode==drive_inode){
                drive=i;
                channel=iter->id;
                goto breakout;
            }
            if(iter->second->ide_devices[i].devfs_inode==drive_inode){
                drive=i;
                channel=iter->second->id;
                goto breakout;
            }
        }
        iter=iter->next;
  }
  if(iter==0){
      
        return ENODEV;
  }
  breakout:
  unsigned char lba_mode /* 0: CHS, 1:LBA28, 2: LBA48 */,
      dma /* 0: No DMA, 1: DMA */, cmd;
  unsigned char lba_io[6];
  //unsigned int channel = ide_devices[drive].Channel; // Read the Channel.
  unsigned int slavebit =
      getchannel(channel)->ide_devices[drive].Drive; // Read the Drive [Master/Slave]
  
  unsigned int bus =
      getchannel(channel)->base; // Bus Base, like 0x1F0 which is also data port.
  unsigned int words =
      256; // Almost every ATA drive has a sector-size of 512-byte.
  unsigned short cyl, i;
  unsigned char head, sect, err;
  ide_write(getchannel(channel), ATA_REG_CONTROL,
           getchannel(channel)->nIEN = (ide_irq_invoked = 0x0) + 0x02);
  // (I) Select one from LBA28, LBA48 or CHS;
  if (lba >= 0x10000000) { // Sure Drive should support LBA in this case, or you
                           // are giving a wrong LBA.
    // LBA48:
    lba_mode = 2;
    lba_io[0] = (lba & 0x000000FF) >> 0;
    lba_io[1] = (lba & 0x0000FF00) >> 8;
    lba_io[2] = (lba & 0x00FF0000) >> 16;
    lba_io[3] = (lba & 0xFF000000) >> 24;
    lba_io[4] = 0; // LBA28 is integer, so 32-bits are enough to access 2TB.
    lba_io[5] = 0; // LBA28 is integer, so 32-bits are enough to access 2TB.
    head = 0;      // Lower 4-bits of HDDEVSEL are not used here.
  } else if (getchannel(channel)->ide_devices[drive].Capabilities & 0x200) { // Drive supports LBA?
    // LBA28:
    lba_mode = 1;
    lba_io[0] = (lba & 0x00000FF) >> 0;
    lba_io[1] = (lba & 0x000FF00) >> 8;
    lba_io[2] = (lba & 0x0FF0000) >> 16;
    lba_io[3] = 0; // These Registers are not used here.
    lba_io[4] = 0; // These Registers are not used here.
    lba_io[5] = 0; // These Registers are not used here.
    head = (lba & 0xF000000) >> 24;
  } else {
    // CHS:
    lba_mode = 0;
    sect = (lba % 63) + 1;
    cyl = (lba + 1 - sect) / (16 * 63);
    lba_io[0] = sect;
    lba_io[1] = (cyl >> 0) & 0xFF;
    lba_io[2] = (cyl >> 8) & 0xFF;
    lba_io[3] = 0;
    lba_io[4] = 0;
    lba_io[5] = 0;
    head = (lba + 1 - sect) % (16 * 63) /
           (63); // Head number is written to HDDEVSEL lower 4-bits.
  }
  dma = 0;
  while (ide_read(getchannel(channel), ATA_REG_STATUS) & ATA_SR_BSY){}
    if (lba_mode == 0)
      ide_write(getchannel(channel), ATA_REG_HDDEVSEL,
                0xA0 | (slavebit << 4) | head); // Drive & CHS.
    else
      ide_write(getchannel(channel), ATA_REG_HDDEVSEL,
                0xE0 | (slavebit << 4) | head); // Drive & LBA
    if (lba_mode == 2) {
      ide_write(getchannel(channel), ATA_REG_SECCOUNT1, 0);
      ide_write(getchannel(channel), ATA_REG_LBA3, lba_io[3]);
      ide_write(getchannel(channel), ATA_REG_LBA4, lba_io[4]);
      ide_write(getchannel(channel), ATA_REG_LBA5, lba_io[5]);
    }
    ide_write(getchannel(channel), ATA_REG_SECCOUNT0, numsects);
    ide_write(getchannel(channel), ATA_REG_LBA0, lba_io[0]);
    ide_write(getchannel(channel), ATA_REG_LBA1, lba_io[1]);
    ide_write(getchannel(channel), ATA_REG_LBA2, lba_io[2]);
    if (lba_mode == 0 && dma == 0 && direction == 0)
      cmd = ATA_CMD_READ_PIO;
    if (lba_mode == 1 && dma == 0 && direction == 0)
      cmd = ATA_CMD_READ_PIO;
    if (lba_mode == 2 && dma == 0 && direction == 0)
      cmd = ATA_CMD_READ_PIO_EXT;
    if (lba_mode == 0 && dma == 1 && direction == 0)
      cmd = ATA_CMD_READ_DMA;
    if (lba_mode == 1 && dma == 1 && direction == 0)
      cmd = ATA_CMD_READ_DMA;
    if (lba_mode == 2 && dma == 1 && direction == 0)
      cmd = ATA_CMD_READ_DMA_EXT;
    if (lba_mode == 0 && dma == 0 && direction == 1)
      cmd = ATA_CMD_WRITE_PIO;
    if (lba_mode == 1 && dma == 0 && direction == 1)
      cmd = ATA_CMD_WRITE_PIO;
    if (lba_mode == 2 && dma == 0 && direction == 1)
      cmd = ATA_CMD_WRITE_PIO_EXT;
    if (lba_mode == 0 && dma == 1 && direction == 1)
      cmd = ATA_CMD_WRITE_DMA;
    if (lba_mode == 1 && dma == 1 && direction == 1)
      cmd = ATA_CMD_WRITE_DMA;
    if (lba_mode == 2 && dma == 1 && direction == 1)
      cmd = ATA_CMD_WRITE_DMA_EXT;
    ide_write(getchannel(channel), ATA_REG_COMMAND, cmd); // Send the Command.
    if (dma)
      if (direction == 0){
      }
      // DMA Read.
      else{
      }
    // DMA Write.
    else if (direction == 0){
      // PIO Read.
      for (i = 0; i < numsects; i++) {
        if ((err = ide_polling(getchannel(channel), 1)))
          return err; // Polling, set error and exit if there is.
        asm("pushw %es");
        asm("mov %%ax, %%es" : : "a"(selector));
        asm("rep insw" : : "c"(words), "d"(bus), "D"(edi)); // Receive Data.
        asm("popw %es");
        edi += (words * 2);
      }
    }
    else {
      // PIO Write.
      for (i = 0; i < numsects; i++) {
        ide_polling(getchannel(channel), 0); // Polling.
        asm("pushw %ds");
        asm("mov %%ax, %%ds" ::"a"(selector));
        asm("rep outsw" ::"c"(words), "d"(bus), "S"(edi)); // Send Data
        asm("popw %ds");
        edi += (words * 2);
      }
      ide_write(getchannel(channel), ATA_REG_COMMAND,
                (char[]){ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH,
                         ATA_CMD_CACHE_FLUSH_EXT}[lba_mode]);
      ide_polling(getchannel(channel), 0); // Polling.
    }

    return 0; // Easy, isn't it?
  }


void ide_wait_irq() {
   while (!ide_irq_invoked)
      ;
   ide_irq_invoked = 0;
}

unsigned char ide_atapi_read(unsigned char drive, unsigned int lba, unsigned char numsects,unsigned short selector, unsigned int edi,unsigned int channel) {
    
   unsigned int   slavebit = getchannel(channel)->ide_devices[drive].Drive;
   unsigned int   bus      = getchannel(channel)->base;
   unsigned int   words    = 1024; // Sector Size. ATAPI drives have a sector size of 2048 bytes.
   unsigned char  err;
   int i;
   struct IDEChannelRegisters* channel_ptr=getchannel(channel);
      ide_write(channel_ptr, ATA_REG_CONTROL, getchannel(channel)->nIEN = ide_irq_invoked = 0x0);
   atapi_packet[ 0] = ATAPI_CMD_READ;
   atapi_packet[ 1] = 0x0;
   atapi_packet[ 2] = (lba >> 24) & 0xFF;
   atapi_packet[ 3] = (lba >> 16) & 0xFF;
   atapi_packet[ 4] = (lba >> 8) & 0xFF;
   atapi_packet[ 5] = (lba >> 0) & 0xFF;
   atapi_packet[ 6] = 0x0;
   atapi_packet[ 7] = 0x0;
   atapi_packet[ 8] = 0x0;
   atapi_packet[ 9] = numsects;
   atapi_packet[10] = 0x0;
   atapi_packet[11] = 0x0;
   ide_write(channel_ptr, ATA_REG_HDDEVSEL, slavebit << 4);
   for(int i = 0; i < 4; i++)
       ide_read(channel_ptr, ATA_REG_ALTSTATUS);
    ide_write(channel_ptr, ATA_REG_FEATURES, 0);         // PIO mode.
         ide_write(channel_ptr, ATA_REG_LBA1, (words * 2) & 0xFF);   // Lower Byte of Sector Size.
   ide_write(channel_ptr, ATA_REG_LBA2, (words * 2) >> 8);   // Upper Byte of Sector Size.   // (VI): Send the Packet Command:
   // ------------------------------------------------------------------
   ide_write(channel_ptr, ATA_REG_COMMAND, ATA_CMD_PACKET);      // Send the Command.
 
   // (VII): Waiting for the driver to finish or return an error code:
   // ------------------------------------------------------------------
   if ((err = ide_polling(getchannel(channel), 1))) return err;         // Polling and return if error.
 
   // (VIII): Sending the packet data:
   // ------------------------------------------------------------------
   asm("rep   outsw" : : "c"(6), "d"(bus), "S"(atapi_packet));   // Send Packet Data
       for (i = 0; i < numsects; i++) {
      ide_wait_irq();                  // Wait for an IRQ.
      if ((err = ide_polling(getchannel(channel), 1)))
         return err;      // Polling and return if error.
      asm("pushw %es");
      asm("mov %%ax, %%es"::"a"(selector));
      asm("rep insw"::"c"(words), "d"(bus), "D"(edi));// Receive Data.
      asm("popw %es");
      edi += (words * 2);
   }
      // (X): Waiting for an IRQ:
   // ------------------------------------------------------------------
   ide_wait_irq();
 
   // (XI): Waiting for BSY & DRQ to clear:
   // ------------------------------------------------------------------
   while (ide_read(getchannel(channel), ATA_REG_STATUS) & (ATA_SR_BSY | ATA_SR_DRQ))
      ;
 
   return 0; // Easy, ... Isn't it?
}

unsigned char ide_read_sectors(unsigned char drive_inode, unsigned char numsects, unsigned int lba,unsigned short es, unsigned int buffer) {
       unsigned char err=0;

       unsigned int drive,channel;
       struct IDEChannelRegisters* iter=idechannelroot;
  while(iter){
        for(unsigned int i=0;i<3;++i){
            if(iter->ide_devices[i].devfs_inode==drive_inode){
                drive=i;
                channel=iter->id;
                goto breakout;
            }
            if(iter->second->ide_devices[i].devfs_inode==drive_inode){
                drive=i;
                channel=iter->second->id;
                goto breakout;
            }
        }
        iter=iter->next;
  }
  breakout:
  if(iter==0){
        return ENODEV;
  }
   // 1: Check if the drive presents:
   // ==================================
   if (drive > 3 || getchannel(channel)->ide_devices[drive].Reserved == 0);      // Drive Not Found!
 
   // 2: Check if inputs are valid:
   // ==================================
   else if (((lba + numsects) > getchannel(channel)->ide_devices[drive].Size) && (getchannel(channel)->ide_devices[drive].Type == IDE_ATA))
      ;                     // Seeking to invalid position.
 
   // 3: Read in PIO Mode through Polling & IRQs:
   // ============================================
   else {
      if (getchannel(channel)->ide_devices[drive].Type == IDE_ATA)
         err = ide_ata_access(ATA_READ, drive, lba, numsects, es, buffer);
      else if (getchannel(channel)->ide_devices[drive].Type == IDE_ATAPI)
         for (int i = 0; i < numsects; i++){
            err = ide_atapi_read(drive, lba + i, 1, es, buffer + (i*2048),channel);
      ;  }
   }

    return err;
}
unsigned char ide_write_sectors(unsigned int drive_inode, unsigned char numsects, unsigned int lba,
                       unsigned short es, unsigned int edi) {
       unsigned char err=0;

    unsigned int drive,channel;
    struct IDEChannelRegisters* iter=idechannelroot;
  while(iter){
        for(unsigned int i=0;i<3;++i){
            if(iter->ide_devices[i].devfs_inode==drive_inode){
                drive=i;
                channel=iter->id;
                goto breakout;
            }
        }
        iter=iter->next;
  }
  breakout:
  if(iter==0){
        return ENODEV;
  }
   // 1: Check if the drive presents:
   // ==================================
   if (drive > 3 || getchannel(channel)->ide_devices[drive].Reserved == 0);
   // 2: Check if inputs are valid:
   // ==================================
   else if (((lba + numsects) > getchannel(channel)->ide_devices[drive].Size) && (getchannel(channel)->ide_devices[drive].Type == IDE_ATA))
      ;                     // Seeking to invalid position.
   // 3: Read in PIO Mode through Polling & IRQs:
   // ============================================
   else {
      if (getchannel(channel)->ide_devices[drive].Type == IDE_ATA)
         err = ide_ata_access(ATA_WRITE, drive, lba, numsects, es, edi);
      else if (getchannel(channel)->ide_devices[drive].Type == IDE_ATAPI)
         err = 4; // Write-Protected.
      ;
   }

    return err;
}

char testbuf[1024];

void init_ide() {
  putstring("init_ide()...");
  struct pcidev *it = pciroot;
  idechannelroot=ideit=khmalloc(sizeof(struct IDEChannelRegisters));
  while (it) {
    if (it->classcode == 1 && it->subclass == 1) {
      putstring("\nide controller found at bus ");
      putunum(it->bus, 10);
      putstring(" dev ");
      putunum(it->dev, 10);
      putstring(" func ");
      putunum(it->func, 10);
      putstring("\n");
      ctrl_init((unsigned int)((readconfword(it->bus, it->dev, it->func, 0x12)
                                << 16) |
                               readconfword(it->bus, it->dev, it->func, 0x10)),
                (unsigned int)((readconfword(it->bus, it->dev, it->func, 0x16)
                                << 16) |
                               readconfword(it->bus, it->dev, it->func, 0x14)),
                (unsigned int)((readconfword(it->bus, it->dev, it->func, 0x1a)
                                << 16) |
                               readconfword(it->bus, it->dev, it->func, 0x18)),
                (unsigned int)((readconfword(it->bus, it->dev, it->func, 0x1e)
                                << 16) |
                               readconfword(it->bus, it->dev, it->func, 0x1c)),
                (unsigned int)((readconfword(it->bus, it->dev, it->func, 0x22)
                                << 16) |
                               readconfword(it->bus, it->dev, it->func, 0x20)));
    }
    it = it->next;
  }
  //unsigned char ide_read_sectors(unsigned char drive_inode, unsigned char numsects, unsigned int lba,unsigned short es, unsigned int buffer) {

  ide_read_sectors(0,1,1,0x10,(unsigned int)&testbuf);
  putstring(testbuf);
  putstring(donemsg_str);
}
