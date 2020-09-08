/**
 * 2020年07月30日 16:15:16
 * 块设备的头文件
 */
#ifndef	_OS_BLK_H_
#define	_OS_BLK_H_

//BIOS读取到的硬盘参数信息结构
typedef struct drive_info
{
	t_16	cylinders;		//0x00字柱面数
	t_8	heads;				//0x02字节磁头数
	t_16	dec_cylind;		//0x03字开始减小写电流的柱面(仅PCXT使用,其它为0)
	t_16	compensate;		//0x05字开始写前预补偿柱面号(乘4)
	t_8	ECC_len;			//0x07字节最大ECC猝发长度(仅XT使用,其它为0)
	t_8 ctrl_byte;			//0x08字节控制字节(驱动器步进选择)
	t_8 complex;			//位0未用,位1保留(0)(关闭IRQ),位2允许复位,位3若磁头数大于8则置1,位4未用(0),位5若在柱面数+1处有生产商的坏区图,则置1,位6禁止ECC重试,位7禁止访问重试。
	t_8	stand_timeout;		//0x09字节标准超时值(仅XT使用,其它为0)
	t_8	format_timeout;		//0x0A字节格式化超时值(仅XT使用,其它为0)
	t_8	test_timeout;		//0x0B字节检测驱动器超时值(仅XT使用,其它为0)
	t_8	stop_cylind;		//0x0C字磁头着陆(停止)柱面号
	t_8	sectors;			//0x0E字节每磁道扇区数
	t_8	blank;				//0x0F字节保留。
} DRIVE_INFO_TABLE;

//磁盘参数及类型信息结构
typedef struct hard_disk
{
	u_32	head;		//磁头数
	u_32	sector;		//每磁道扇区数
	u_32	cylinder;	//柱面数
	u_32	wpcom;		//写前预补偿柱面号
	u_32	lzone;		//磁头着陆区柱面号
	u_32	ctrl;		//控制字节
} HARD_DISK;

//磁盘分区信息结构
typedef struct hd
{
	u_32	start_sector;	//分区起始扇区号
	u_32	cnt_sectors;	//分区扇区总数
} HD;

EXTERN	t_32				ROOT_DEV;
EXTERN	DRIVE_INFO_TABLE	DRIVE_INFO;
EXTERN	HARD_DISK			HARD_DISK_INFO;
//0表示硬盘整体信息，其余表示各分区信息
EXTERN	HD					HD_INFO[];

#endif