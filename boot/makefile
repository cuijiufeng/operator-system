#2020年07月11日 14/36/51
#编译boot相关
#==============================================================================

ASM			= nasm
IPATH		= ./include/
OUTPUT		= ../output/boot
IMGPATH		= ../output
TARGET		= boot.bin loader.bin

.PHONY : all clean

all : mkdir $(TARGET) image

mkdir : 
	mkdir -p $(OUTPUT)

image : 
	dd if=$(OUTPUT)/boot.bin of=$(IMGPATH)/os.img bs=512 count=1 conv=notrunc
	mount $(IMGPATH)/os.img /media/ -o loop
	cp -f $(OUTPUT)/loader.bin /media/
	umount $(IMGPATH)/os.img

#使用循环遍历变量
clean :
	$(foreach var, $(TARGET), rm -f $(OUTPUT)/$(var))
	rm -rf $(OUTPUT)

boot.bin : boot.asm
	$(ASM) -I$(IPATH) $< -o $(OUTPUT)/$@

loader.bin : loader.asm
	$(ASM) -I$(IPATH) $< -o $(OUTPUT)/$@