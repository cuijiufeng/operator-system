# 2020年07月29日 23/41/47
# 初始化
#==============================================================================
include	../makefile.head

LDFLAGS		+= -r
OBJS		= cinit.o

.PHONY : all clean
%.o : %.asm
	$(ASM) $(ASMFLAGS) -I$(IPATH) $< -o $@
%.o : %.c
	$(GCC) $(GCCFLAGS) -I$(IPATH) $< -o $@

all : init.o

clean : 
	rm -rf $(OBJS) init.o
	
init.o : $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

# 依赖
cinit.o : cinit.c