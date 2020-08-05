#2020年07月11日 14/36/51
#编译boot相关
#==============================================================================
include	../makefile.head

IPATH		:= ./include/
OBJS		= boot.bin setup.bin

.PHONY : all clean
# 静态模式,就类似于模板应该可以这么理解
%.bin : %.asm
	$(ASM) -I$(IPATH) $< -o $@

all : $(OBJS)

clean :
	rm -rf $(OBJS)

# 依赖关系
boot.bin : boot.asm
setup.bin : setup.asm