# 2020年07月19日 09/11/26
# lib
#==============================================================================

ASM			= nasm
GCC			= gcc
DEBUG		= debug
ifdef		DEBUG
	ASMARG		= -f elf32
	GCCARG		= -g -c -m32
else
	ASMARG		= -f elf32
	GCCARG		= -c -m32
endif

IPATH		= ../include/
OUTPUT		= ../output/lib
TARGET		= memlib.o klib.o clib.o

.PHONY : all clean

all : mkdir $(TARGET)

mkdir : 
	mkdir -p $(OUTPUT)

clean : 
	rm -rf $(OUTPUT)

memlib.o : memlib.asm
	$(ASM) $(ASMARG) -I$(IPATH) $< -o $(OUTPUT)/$@
klib.o : klib.asm
	$(ASM) $(ASMARG) -I$(IPATH) $< -o $(OUTPUT)/$@

clib.o : klib.c
	$(GCC) $(GCCARG) -I$(IPATH) $< -o $(OUTPUT)/$@