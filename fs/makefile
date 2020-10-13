# 2020年07月19日 09/11/26
# lib
#==============================================================================
include	../makefile.head

LDFLAGS		+= -r
OBJS		= read_write.o buffer.o inode.o super.o char_dev.o

.PHONY : all clean
%.o : %.asm
	$(ASM) $(ASMFLAGS) -I$(IPATH) $< -o $@
%.o : %.c
	$(GCC) $(GCCFLAGS) -I$(IPATH) $< -o $@

all : fs.o

clean : 
	rm -rf $(OBJS) fs.o
	
fs.o : $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

# 依赖
read_write.o : read_write.c
buffer.o : buffer.c
inode.o :  inode.c
super.o : super.c
char_dev.o : char_dev.c