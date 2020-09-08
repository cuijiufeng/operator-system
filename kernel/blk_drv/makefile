# 2020年09月03日 21/12/58
# 块设备
#==============================================================================
include	../../makefile.head

IPATH		= ../../include/
LDFLAGS		+= -r
OBJS		= harddisk.o

.PHONY : all clean
%.o : %.asm
	$(ASM) $(ASMFLAGS) -I$(IPATH) $< -o $@
%.o : %.c
	$(GCC) $(GCCFLAGS) -I$(IPATH) $< -o $@

all : blk_drv.o

clean : 
	rm -rf $(OBJS) blk_drv.o
	
blk_drv.o : $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

# 依赖
harddisk.o : harddisk.c