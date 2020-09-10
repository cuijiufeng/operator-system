# 2020年07月19日 10/53/44
# 编译kernel相关
#==============================================================================
include	../makefile.head

LDFLAGS		+= -r
OBJS		= krn.o time.o protect.o process.o syscall.o int.o int_handler.o signal.o main.o print.o

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
sys_call.o : sys_call.asm

protect.o : protect.c
int_handler.o : int_handler.c
process.o : process.c
signal.o : signal.c
time.o : time.c
main.o : main.c
print.o : print.c