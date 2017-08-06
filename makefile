CC      = gcc
CXX      = g++
CFLAGS  = -m32 -Wall -fno-builtin -nostdinc -nostdlib -std=c99 -ggdb3 -fpermissive -U linux
#CXXFLAGS  = -m32 -Wall -fno-builtin -nostdinc -nostdlib -ggdb3 | -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
CXXFLAGS = -Iustl -m32 -Wall -fno-builtin -nostdinc++  -nostdlib  -nodefaultlibs -ffreestanding -O2 -Wextra -fno-exceptions -fno-rtti -ggdb3 -mfpmath=387 -U linux
LD      = ld

OBJFILES = \
    boot.o  \
    common/gdt_s.o  \
    common/idt_s.o  \
    common/misc.o  \
    common/ordered_map.o  \
    common/malloc.o  \
    common/printf.o  \
    common/scanf.o  \
    common/screen.o  \
    common/string.o  \
    common/string_ext.o  \
    common/acpi.o  \
    common/assert.o  \
    common/fs.o  \
    common/shutdown.o  \
    common/gdt.o  \
    common/idt.o  \
    common/tui.o  \
    common/gui.o  \
    common/gui_session.o  \
    common/gelements.o  \
    common/timer.o  \
    pci.o  \
    common/keyboard.o  \
    common/heap.o  \
    common/paging.o  \
    common/bios.o  \
    common/vbe.o  \
    common/qdivrem.o  \
    common/udivdi3.o  \
    common/umoddi3.o  \
    common/divdi3.o  \
    common/moddi3.o  \
    common/setjmp.o  \
    common/lab2.o  \
    common/floor.o  \
    common/iostream.o  \
    common/new.o  \
    common/runtime_graphics.o  \
    x86emu/x86emu.o  \
    x86emu/x86emu_util.o  \
    amo/interpolation.o  \
    kvm/module.o  \
    kvm/jit_helpers.o  \
    kvm/internals.o  \
    kvm/log.o  \
    kvm/jit.o  \
    kvm/jit_runtime_api.o  \
    kvm/memorymanager.o  \
    kvm/runtime.o  \
    common/json/json.o  \
    kernel.o

image:
	@echo "Creating hdd.img..."
	#@dd if=/dev/zero of=./hdd.img bs=512 count=32130 1>/dev/null 2>&1
	
	#@echo "Deataching /dev/loop1 and /dev/loop2..."
	#@losetup -d /dev/loop1
	#@losetup -d /dev/loop2
	#@echo "Creating bootable first FAT32 partition..."
	#@losetup /dev/loop1 ./hdd.img
	#@(echo c; echo u; echo n; echo p; echo 1; echo ;  echo ; echo a; echo 1; echo t; echo c; echo w;) | fdisk /dev/loop1 1>/dev/null 2>&1 || true

	#@echo "Mounting partition to /dev/loop2..."
	#@losetup /dev/loop2 ./hdd.img \
	#--offset	32256 \ #`echo \`fdisk -lu /dev/loop1 | sed -n 10p | awk '{print $$3}'\`*512 | bc` \
	#--sizelimit 16450048 \ #`echo \`fdisk -lu /dev/loop1 | sed -n 10p | awk '{print $$4}'\`*512 | bc`
	#@losetup -d /dev/loop1

	#@echo "Format partition..."
	#@mkdosfs /dev/loop2

	@echo "Copy kernel and grub files on partition..."
	@mkdir -p tempdir
	#@mount /dev/loop2 tempdir
	@mkdir tempdir/boot
	@cp -r grub tempdir/boot/
	@cp kernel.bin tempdir/
	@cp initrd.img tempdir/
	#@sleep 1
	#@umount /dev/loop2
	#@losetup -d /dev/loop2

	@echo "Installing GRUB..."
	@mkdir -p tempdir/boot/grub
	@cp grub/grub.cfg tempdir/boot/grub/grub.cfg
	#@cp grub/moddep.lst tempdir/moddep.lst
	grub-mkrescue -o hdd.img tempdir
	#@grub-mkimage -o hdd.img -p /usr/lib/grubi386-pc -O i386-multiboot -d tempdir normal gfxterm gfxmenu gfxterm_menu fat ext2 ntfs cat echo ls memdisk tar sh multiboot
	#@echo "device (hd0) hdd.img \n \
	#		root (hd0,0)         \n \
	#		setup (hd0)          \n \
	#		quit\n" | grub --batch 1>/dev/null
	@rm -r tempdir
	@echo "Done!"

all: kernel.bin
rebuild: clean all
kvm/jit_helpers.o:
	nasm -f elf32 kvm/jit_helpers.asm
common/gdt_s.o:
	nasm -f elf32 common/gdt_s.s
common/misc.o:
	nasm -f elf32 common/misc.s
common/idt_s.o:
	nasm -f elf32 common/idt_s.s
boot.o:
	nasm -f elf32 boot.s
.s.o:
	as -32 -o $@ $<
.c.o:
	$(CC) -Ix86emu -Iinclude $(CFLAGS) -o $@ -c $<
.cpp.o:
	@echo "building $@" 
	$(CXX) -Ix86emu -Iamo -Iinclude -Ikvm -Iinclude/c++ $(CXXFLAGS)  -o $@ -c $<
kernel.bin: $(OBJFILES)
	$(LD) -nostdlib -melf_i386 --oformat=elf32-i386 -T link.ld -o $@ $^ 
	#$(CC) -T link.ld -ffreestanding -O2 -nostdlib -lgcc -o $@ $^
	cp $@ $@.dbg 
	strip $@
clean:
	rm -f $(OBJFILES) hdd.img kernel.bin
