# 2020年07月19日 10/53/44
# 编译kernel相关
#==============================================================================
include	../makefile.head

LDFLAGS		+= -r
OBJS		= krn.o int.o protect.o traps.o

.PHONY : all clean
%.o : %.asm
	$(ASM) $(ASMFLAGS) -I$(IPATH) $< -o $@
%.o : %.c
	$(GCC) $(GCCFLAGS) -I$(IPATH) $< -o $@

all : kernel.o

clean : 
	rm -rf $(OBJS) kernel.o
	
kernel.o : $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

# 依赖
krn.o : krn.asm
int.o : int.asm

protect.o : protect.c
traps.o : traps.c