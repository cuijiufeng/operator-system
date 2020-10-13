#2020年07月11日 21/42/08
#全部
#==============================================================================
include	makefile.head

LDFLAGS		+= -Ttext 0x10000
OBJS		= kernel/kernel.o init/init.o mm/mm.o lib/lib.o fs/fs.o kernel/blk_drv/blk_drv.o kernel/chr_drv/chr_drv.o

.PHONY : all clean

all : makeboot kernel.bin

makeboot : 
	cd boot && $(MAKE) all
	
clean : 
	cd boot && $(MAKE) clean
	cd init && $(MAKE) clean
	cd lib && $(MAKE) clean
	cd mm && $(MAKE) clean
	cd fs && $(MAKE) clean
	cd kernel && $(MAKE) clean
	cd kernel/blk_drv && $(MAKE) clean
	cd kernel/chr_drv && $(MAKE) clean
	rm -f kernel.bin

#kernel.o在链接的时候需要放在第一位置
kernel.bin : $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

init/init.o :
	cd init && $(MAKE) all
lib/lib.o : 
	cd lib && $(MAKE) all
mm/mm.o : 
	cd mm && $(MAKE) all
fs/fs.o : 
	cd fs && $(MAKE) all
kernel/kernel.o : 
	cd kernel && $(MAKE) all
kernel/blk_drv/blk_drv.o : 
	cd kernel/blk_drv && $(MAKE) all
kernel/chr_drv/chr_drv.o : 
	cd kernel/chr_drv && $(MAKE) all