#2020年07月11日 21/42/08
#全部
#==============================================================================

MAKE		= make
ASM			= nasm
GCC			= gcc
LD			= ld
ASMARG		= -f elf
GCARG		= -c
LDARG		= -m elf_i386 -s -Ttext 0x30400

IMGPATH		= ./output
IPATH		= ./include
SRCPATH		= ./kernel
OUTPUT		= ./output/kernel
TARGET		= kernel.o
KERNELTARGET= kernel.bin

.PHONY : all clean

all : mkdir createimage makeboot $(TARGET) $(KERNELTARGET) image

mkdir : 
	mkdir -p $(OUTPUT)

createimage : 
	dd if=/dev/zero of=$(IMGPATH)/os.img bs=512 count=2880

image : 
	mount $(IMGPATH)/os.img /tmp -o loop
	cp -f $(OUTPUT)/kernel.bin /tmp
	umount /tmp

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

#makefile的foreach函数，作用是是循环遍历
$(KERNELTARGET) : $(TARGET)
	$(LD) $(LDARG) $(foreach var, $^, $(OUTPUT)/$(var)) -o $(OUTPUT)/$@