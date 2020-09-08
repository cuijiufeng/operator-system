# 2020年09月05日 14/01/02
# 字符设备
#==============================================================================
include	../../makefile.head

IPATH		= ../../include/
LDFLAGS		+= -r
OBJS		= keyboard.o tty.o console.o serial.o

.PHONY : all clean
%.o : %.asm
	$(ASM) $(ASMFLAGS) -I$(IPATH) $< -o $@
%.o : %.c
	$(GCC) $(GCCFLAGS) -I$(IPATH) $< -o $@

all : chr_drv.o

clean : 
	rm -rf $(OBJS) chr_drv.o
	
chr_drv.o : $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

# 依赖
tty.o : tty.c
keyboard.o : keyboard.c
console.o : console.c
serial.o : serial.c