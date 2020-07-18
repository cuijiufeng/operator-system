#2020年07月11日 21/42/08
#全部
#==============================================================================

MAKE		= make
ASM			= nasm
GCC			= gcc
LD			= ld
DEBUG		= debug
ifdef		DEBUG
	ASMARG		= -f elf
	GCCARG		= -g -c -m32
	LDARG		= -m elf_i386 -Ttext 0x30400
else
	ASMARG		= -f elf
	GCCARG		= -c -m32
	LDARG		= -m elf_i386 -s -Ttext 0x30400
endif

IMGPATH		= ./output
IPATH		= ./include/
SRCPATH		= ./kernel
OUTPUT		= ./output/kernel
TARGET		= kernel.o cstart.o
KERNELTARGET= kernel.bin

.PHONY : all clean

all : mkdir createimage makeboot $(TARGET) $(KERNELTARGET) image

mkdir : 
	mkdir -p $(OUTPUT)

createimage : 
	dd if=/dev/zero of=$(IMGPATH)/os.img bs=512 count=2880

image : 
	mount $(IMGPATH)/os.img /media/ -o loop
	cp -f $(OUTPUT)/kernel.bin /media/
	umount $(IMGPATH)/os.img

makeboot : 
	cd boot && $(MAKE) all

clean : 
	cd boot && $(MAKE) clean
	$(foreach var, $(TARGET), rm -f $(OUTPUT)/$(var))
	rm -f $(OUTPUT)/$(KERNELTARGET)
	rm -f $(IMGPATH)/os.img
	rm -rf $(OUTPUT)

kernel.o : $(SRCPATH)/kernel.asm
	$(ASM) $(ASMARG) -I$(IPATH) $< -o $(OUTPUT)/$@
	
cstart.o : $(SRCPATH)/cstart.c
	$(GCC) $(GCCARG) -I$(IPATH) $< -o $(OUTPUT)/$@

#makefile的foreach函数，作用是是循环遍历
$(KERNELTARGET) : $(TARGET)
	$(LD) $(LDARG) $(foreach var, $^, $(OUTPUT)/$(var)) -o $(OUTPUT)/$@