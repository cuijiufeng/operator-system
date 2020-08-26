#2020年07月11日 21/42/08
#全部
#==============================================================================
include	makefile.head

LDFLAGS		+= -Ttext 0x10000 -e _start
OBJS		= kernel/kernel.o lib/lib.o init/init.o mm/mm.o

.PHONY : all clean

all : makeboot kernel.bin

makeboot : 
	cd boot && $(MAKE) all
	
clean : 
	cd boot && $(MAKE) clean
	cd init && $(MAKE) clean
	cd kernel && $(MAKE) clean
	cd lib && $(MAKE) clean
	cd mm && $(MAKE) clean
	rm -f kernel.bin

#kernel.o在链接的时候需要放在第一位置
kernel.bin : $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

init/init.o :
	cd init && $(MAKE) all
kernel/kernel.o : 
	cd kernel && $(MAKE) all
lib/lib.o : 
	cd lib && $(MAKE) all
mm/mm.o : 
	cd mm && $(MAKE) all