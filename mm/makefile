# 2020年07月30日 19/18/35
# 内存
#==============================================================================
include	../makefile.head

LDFLAGS		+= -r
OBJS		= memory.o

.PHONY : all clean
%.o : %.asm
	$(ASM) $(ASMFLAGS) -I$(IPATH) $< -o $@
%.o : %.c
	$(GCC) $(GCCFLAGS) -I$(IPATH) $< -o $@

all : mm.o

clean : 
	rm -rf $(OBJS) mm.o
	
mm.o : $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

# 依赖
memory.o : memory.c