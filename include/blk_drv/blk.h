/**
 * 2020年07月30日 16:15:16
 * 块设备的头文件
 */
#ifndef	_OS_BLK_H_
#define	_OS_BLK_H_

#define MAJOR_NR	3
#define NR_REQUEST	32
#define NR_BLK_DEV	7
#define MAX_ERRORS	7
#define HD_CMD		0x3f6
 /* Hd controller regs. Ref: IBM AT Bios-listing */
#define HD_DATA		0x1f0	/* _CTL when writing */
#define HD_ERROR	0x1f1	/* see err-bits */
#define HD_STATUS	0x1f7	/* see status-bits */

/* Bits of HD_STATUS */
#define ERR_STAT	0x01
#define INDEX_STAT	0x02
#define ECC_STAT	0x04	/* Corrected error */
#define DRQ_STAT	0x08
#define SEEK_STAT	0x10
#define WRERR_STAT	0x20
#define READY_STAT	0x40
#define BUSY_STAT	0x80

 /* Values for HD_COMMAND */
#define WIN_RESTORE		0x10
#define WIN_READ		0x20
#define WIN_WRITE		0x30
#define WIN_SPECIFY		0x91

#define IN_ORDER(s1,s2) \
((s1)->cmd<(s2)->cmd || ((s1)->cmd==(s2)->cmd && ((s1)->dev < (s2)->dev || ((s1)->dev == (s2)->dev && (s1)->sector < (s2)->sector))))

typedef struct request {
	t_32 dev;		/* -1 if no request */
	t_32 cmd;		/* READ or WRITE */
	t_32 errors;
	u_32 sector;
	u_32 nr_sectors;
	char* buffer;
	PROCESS* waiting;
	BUFFER_HEAD* bh;
	struct request* next;
} REQUEST;

typedef struct blk_dev{
	void(*request_fn)(void);
	REQUEST* current_request;
} BLK_DEV;

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
EXTERN	PROCESS*			WAIT_FOR_REQUEST;
EXTERN	REQUEST				REQUEST_INFO[NR_REQUEST];
EXTERN	BLK_DEV				BLK_DEV_INFO[NR_BLK_DEV];
EXTERN	HARD_DISK			HARD_DISK_INFO;
//0表示硬盘整体信息，其余表示各分区信息
EXTERN	HD					HD_INFO[];

PUBLIC	void ll_rw_block(int rw, BUFFER_HEAD* bh);
PUBLIC	void lockBuffer(BUFFER_HEAD* bh);
PUBLIC	void unlockBuffer(BUFFER_HEAD* bh);
PRIVATE void makeRequest(int major, int rw, BUFFER_HEAD* bh);
PUBLIC	void end_request(int uptodate);
PUBLIC	void harddiskRequest();

#endif