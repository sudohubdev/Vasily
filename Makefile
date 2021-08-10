CC=i686-elf-gcc
AS=i686-elf-gcc
LD=i686-elf-gcc

x86_asm=$(wildcard asm/x86-32/*.S)
x86_o=$(x86_asm:.S=.o) 
c_src= $(wildcard src/*.c) $(wildcard src/dev/*.c) $(wildcard src/fs/*.c)
c_obj=$(c_src:.c=.o)
LDFLAGS=-ffreestanding -nostdlib -lgcc 
CFLAGS= -Wall -Wextra -ffreestanding -O2 -Wall -Wextra -I/usr/include/multiboot -I./include -std=gnu99
ASFLAGS=-I/usr/include/multiboot
all: vasily.iso $(c_src) $(x86_asm)
test: all
	qemu-system-i386 -cdrom vasily.iso
bochs_test: all
	bochs -f conf -q
vasily.iso:  vmvasily
	cp vmvasily iso/boot/vmvasily
	grub-mkrescue -o vasily.iso iso

vmvasily: $(x86_o) $(c_obj) link.ld 
	$(LD) -Wl,-Tlink.ld,-Map=ld.map -o $@ $(x86_o) $(LDFLAGS) $(c_obj)
