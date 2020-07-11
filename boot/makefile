#2020年07月11日 14/36/51
#编译boot相关
#==============================================================================

ASM			= nasm
IPATH		= ./include
OUTPUT		= ../output
TARGET		= boot.bin loader.bin

.PHONY : all clean

all : $(TARGET) createImage image

createImage : 
	dd if=/dev/zero of=$(OUTPUT)/os.img bs=512 count=2880

image : 
	dd if=$(OUTPUT)/boot/boot.bin of=$(OUTPUT)/os.img bs=512 count=1 conv=notrunc
	mount $(OUTPUT)/os.img /tmp -o loop
	cp -f $(OUTPUT)/boot/loader.bin /tmp
	umount /tmp


clean :
	rm -f $(OUTPUT)/boot/*.bin
	rm -f $(OUTPUT)/os.img

boot.bin : boot.asm
	$(ASM) -I$(IPATH) $< -o $(OUTPUT)/boot/$@

loader.bin : loader.asm
	$(ASM) -I$(IPATH) $< -o $(OUTPUT)/boot/$@