#2020年07月11日 21/42/08
#全部
#==============================================================================

LD			= ld
DEBUG		= debug
ifdef		DEBUG
	LDARG		= -m elf_i386 -Ttext 0x30400
else
	LDARG		= -m elf_i386 -s -Ttext 0x30400
endif

OUTPUT		= ./output
TARGET		= kernel.bin
DEPENDENTSPATH	= ./boot ./kernel ./lib

.PHONY : all clean

all : createimage makedependents $(TARGET) image

createimage : 
	dd if=/dev/zero of=$(OUTPUT)/os.img bs=512 count=2880

image : 
	mount $(OUTPUT)/os.img /media/ -o loop
	cp -f $(OUTPUT)/kernel.bin /media/
	umount $(OUTPUT)/os.img

#makefile的foreach函数，作用是是循环遍历
makedependents : 
	$(foreach depend, $(DEPENDENTSPATH), \
		cd $(depend) && \
		$(MAKE) all && \
		cd ..; \
	)

clean : 
	$(foreach depend, $(DEPENDENTSPATH), \
		cd $(depend) && \
		$(MAKE) clean && \
		cd ..; \
	)
	rm -f $(OUTPUT)/$(TARGET)
	rm -f $(OUTPUT)/os.img

#kernel.o在链接的时候需要放在第一位置，不知道为什么？
$(TARGET) : ./output/kernel/kernel.o ./output/kernel/cinit.o ./output/lib/memlib.o ./output/kernel/i8259.o ./output/lib/klib.o ./output/kernel/protect.o \
			./output/kernel/interrupt.o ./output/lib/clib.o ./output/kernel/8259_handler.o ./output/kernel/syserr_handler.o
	$(LD) $(LDARG) $^ -o $(OUTPUT)/$@