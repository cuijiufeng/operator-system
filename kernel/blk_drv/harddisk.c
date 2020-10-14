/**
 * 2020年09月03日 21/10/09
 * 1.硬盘操作相关函数
 */
#include	<type.h>
#include	<fs.h>
#include	<mm.h>
#include	<int.h>
#include	<blk_drv/blk.h>
#include	<protect.h>
#include	<lib.h>
#include	<print.h>

BLK_DEV				BLK_DEV_INFO[NR_BLK_DEV];
HARD_DISK			HARD_DISK_INFO = { 0,};
HD					HD_INFO[5];
T_PF_IRQ_HANDLER	DEVICE_INTR = NULL;
PRIVATE t_32 reset = 0;
PRIVATE t_32 recalibrate = 0;

//初始化硬盘
PUBLIC	void	initHarddisk()
{
	reset = 0;
	recalibrate = 0;
	BLK_DEV_INFO[MAJOR_NR].request_fn = harddiskRequest;
	putIrqHandler(AT_WINI_IRQ, DEVICE_INTR);
	//开启AT温盘中断
	enableIrq(AT_WINI_IRQ);
}

PUBLIC	t_32	sysSetup(t_8* drv_info)
{
	u_8 cmos_disks;
	BUFFER_HEAD* bh;

	//设置磁盘参数及类型信息
	HARD_DISK_INFO.head = *((u_8*)(drv_info + 2));
	HARD_DISK_INFO.sector = *((u_8*)(drv_info + 14));
	HARD_DISK_INFO.cylinder = *((u_16*)(drv_info));
	HARD_DISK_INFO.wpcom = *((u_16*)(drv_info + 5));
	HARD_DISK_INFO.lzone = *((u_8*)(drv_info + 12));
	HARD_DISK_INFO.ctrl = *((u_8*)(drv_info + 8));
	outByte(CMOS_CTL, CMOS_DATA_DISKS);
	cmos_disks = inByte(CMOS_VALUE);
	//判断是否存在AT兼容的硬盘
	if (!(cmos_disks & 0xf0))
	{
		while (1);
	}
	//设置磁盘分区信息
	HD_INFO[0].start_sector = 0;
	HD_INFO[0].cnt_sectors = HARD_DISK_INFO.head * HARD_DISK_INFO.sector * HARD_DISK_INFO.cylinder;
	//todo注释
	if (!(bh = bread(0x300, 0)))
	{
		printk("Unable to read partition table of drive\n\r");
		while (1);
	}
	if (bh->b_data[510] != 0x55 || (unsigned char)bh->b_data[511] != 0xAA) 
	{
		printk("Bad partition table on drive\n\r");
		while (1);
	}
	//p = 0x1BE + (void *)bh->b_data;
	//for (i = 1; i < 5; i++, p++) 
	//{
	//	hd[i + 5 * drive].start_sect = p->start_sect;
	//	hd[i + 5 * drive].nr_sects = p->nr_sects;
	//}
	//brelse(bh);
	//if (NR_HD)
	//	printk("Partition table%s ok.\n\r", (NR_HD > 1) ? "s" : "");
	//rd_load();
	//mount_root();
	return 0;
}

//检测硬盘执行命令后的状态
PRIVATE int winResult()
{
	int i = inByte(HD_STATUS);

	if ((i & (BUSY_STAT | READY_STAT | WRERR_STAT | SEEK_STAT | ERR_STAT)) == (READY_STAT | SEEK_STAT))
		return(0); /* ok */
	if (i & 1) 
		i = inByte(HD_ERROR);
	return (1);
}

//读写硬盘失败处理函数
PRIVATE void badRwIntr(void)
{
	REQUEST* curr = BLK_DEV_INFO[MAJOR_NR].current_request;
	if (++curr->errors >= MAX_ERRORS)
		endRequest(0);
	if (curr->errors > MAX_ERRORS / 2)
		reset = 1;
}

PRIVATE void readIntr()
{
	REQUEST* curr = BLK_DEV_INFO[MAJOR_NR].current_request;
	if (winResult()) 
	{
		badRwIntr();
		harddiskRequest();
		return;
	}
	portRead(HD_DATA, curr->buffer, 256);
	curr->errors = 0;
	curr->buffer += 512;
	curr->sector++;
	if (--curr->nr_sectors) 
	{
		DEVICE_INTR = &readIntr;
		return;
	}
	endRequest(1);
	harddiskRequest();
}

PRIVATE void writeIntr()
{
	REQUEST* curr = BLK_DEV_INFO[MAJOR_NR].current_request;
	if (winResult()) 
	{
		badRwIntr();
		harddiskRequest();
		return;
	}
	if (--curr->nr_sectors) 
	{
		curr->sector++;
		curr->buffer += 512;
		DEVICE_INTR = &writeIntr;
		portWrite(HD_DATA, curr->buffer, 256);
		return;
	}
	endRequest(1);
	harddiskRequest();
}

//硬盘复位中断调用函数
PRIVATE void recalIntr()
{
	if (winResult())
		badRwIntr();
	harddiskRequest();
}

PRIVATE int driveBusy()
{
	u_32 i;

	for (i = 0; i < 10000; i++)
		if (READY_STAT == (inByte(HD_STATUS) & (BUSY_STAT | READY_STAT)))
			break;
	i = inByte(HD_STATUS);
	i &= BUSY_STAT | READY_STAT | SEEK_STAT;
	if (i == (READY_STAT | SEEK_STAT))
		return(0);
	printk("HD controller times out\n\r");
	return(1);
}

PRIVATE int controllerReady()
{
	int retries = 100000;

	while (--retries && (inByte(HD_STATUS) & 0x80));
	return (retries);
}

PRIVATE void resetController()
{
	int	i;

	outByte(4, HD_CMD);
	for (i = 0; i < 100; i++) 
		nop();
	outByte(HARD_DISK_INFO.ctrl & 0x0f, HD_CMD);
	if (driveBusy())
		printk("HD-controller still busy\n\r");
	if ((i = inByte(HD_ERROR)) != 1)
		printk("HD-controller reset failed: %02x\n\r", i);
}

PRIVATE void hdOut(u_32 drive, u_32 nsect, u_32 sect, u_32 head, u_32 cyl, u_32 cmd, void(*intr_addr)(void))
{
	t_port port;

	if (drive > 1 || head > 15)
	{
		printk("Trying to write bad sector");
		while (1);
	}
	if (!controllerReady())
	{
		printk("HD controller not ready");
		while (1);
	}
	DEVICE_INTR = intr_addr;
	outByte(HD_CMD, HARD_DISK_INFO.ctrl);
	port = HD_DATA;
	outByte(++port, HARD_DISK_INFO.wpcom >> 2);
	outByte(++port, nsect);
	outByte(++port, sect);
	outByte(++port, cyl);
	outByte(++port, cyl >> 8);
	outByte(++port, 0xA0 | (drive << 4) | head);
	outByte(++port, cmd);
}

PRIVATE void resetHd(int nr)
{
	resetController();
	hdOut(nr, HARD_DISK_INFO.sector, HARD_DISK_INFO.sector, HARD_DISK_INFO.head - 1, HARD_DISK_INFO.cylinder, WIN_SPECIFY, &recalIntr);
}

PUBLIC	void harddiskRequest()
{
	int i, r = 0;
	u_32 block, dev;
	u_32 sec, head, cyl;
	u_32 nsect;
	REQUEST* curr = BLK_DEV_INFO[MAJOR_NR].current_request;

repeat:
	if (!curr)
		return;
	if (MAJOR(curr->dev) != MAJOR_NR)
	{
		printk("harddisk request list destroyed");
		if (curr->bh)
		{
			if (!curr->bh->b_lock)
				printk("harddisk block not locked");
		}
	}
	dev = MINOR(curr->dev);
	block = curr->sector;
	//如果设备号超出磁盘分区信息数或块号大于最大扇区号
	if (dev >= 5 || (block + 2) > (HD_INFO[dev].start_sector + HD_INFO[dev].cnt_sectors - 1)) {
		endRequest(0);
		goto repeat;
	}
	block += HD_INFO[dev].start_sector;
	dev /= 5;
	//设置磁盘头，柱面，扇区等信息
	__asm__("divl %4":"=a" (block), "=d" (sec) : "0" (block), "1" (0), "r" (HARD_DISK_INFO.sector));
	__asm__("divl %4":"=a" (cyl), "=d" (head) : "0" (block), "1" (0), "r" (HARD_DISK_INFO.head));
	sec++;
	nsect = curr->nr_sectors;
	if (reset) 
	{
		reset = 0;
		recalibrate = 1;
		resetHd(MINOR(curr->dev) / 5);
		return;
	}
	if (recalibrate) 
	{
		recalibrate = 0;
		hdOut(dev, HARD_DISK_INFO.sector, 0, 0, 0, WIN_RESTORE, &recalIntr);
		return;
	}
	if (curr->cmd == WRITE)
	{
		hdOut(dev, nsect, sec, head, cyl, WIN_WRITE, &writeIntr);
		for (i = 0; i < 3000 && !(r = inByte(HD_STATUS)&DRQ_STAT); i++)
			/* nothing */;
		if (!r) 
		{
			badRwIntr();
			goto repeat;
		}
		portWrite(HD_DATA, curr->buffer, 256);
	}
	else if (curr->cmd == READ)
	{
		hdOut(dev, nsect, sec, head, cyl, WIN_READ, &readIntr);
	}
	else
		printk("unknown hd-command");
}