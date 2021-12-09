void outb(unsigned short port,unsigned char data);
unsigned char inb(unsigned short port);
void outl(unsigned short port,unsigned int data);
unsigned int inl(unsigned short port);
void insl(unsigned int port, unsigned int *buffer, unsigned int quads);
void io_wait();
