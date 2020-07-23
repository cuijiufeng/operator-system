# 2020年07月19日 10/53/44
# 编译kernel相关
#==============================================================================

ASM			= nasm
GCC			= gcc
DEBUG		= debug
ifdef		DEBUG
	ASMARG		= -f elf32
	GCCARG		= -g -c -w -m32
else
	ASMARG		= -f elf32
	GCCARG		= -c -w -m32
endif

IPATH		= ../include/
OUTPUT		= ../output/kernel
TARGET		= kernel.o cinit.o i8259.o protect.o interrupt.o 8259_handler.o syserr_handler.o

.PHONY : all clean

all : mkdir $(TARGET)

mkdir : 
	mkdir -p $(OUTPUT)

clean : 
	rm -rf $(OUTPUT)

kernel.o : kernel.asm
	$(ASM) $(ASMARG) -I$(IPATH) $< -o $(OUTPUT)/$@
8259_handler.o : 8259_handler.asm
	$(ASM) $(ASMARG) -I$(IPATH) $< -o $(OUTPUT)/$@
syserr_handler.o : syserr_handler.asm
	$(ASM) $(ASMARG) -I$(IPATH) $< -o $(OUTPUT)/$@
	
cinit.o : cinit.c
	$(GCC) $(GCCARG) -I$(IPATH) $< -o $(OUTPUT)/$@
i8259.o : i8259.c
	$(GCC) $(GCCARG) -I$(IPATH) $< -o $(OUTPUT)/$@
protect.o : protect.c
	$(GCC) $(GCCARG) -I$(IPATH) $< -o $(OUTPUT)/$@
interrupt.o : interrupt.c
	$(GCC) $(GCCARG) -I$(IPATH) $< -o $(OUTPUT)/$@