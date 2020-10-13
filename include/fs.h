/**
 * 2020年07月30日 16:50:22
 * 文件系统
 */
#ifndef	_OS_FS_H_
#define	_OS_FS_H_

#define		MAJOR(a)		(((u_32)(a))>>8)
#define		MINOR(a)		((a)&0xff)

#define		CMOS_DATA_DISKS	0x92
#define		NR_OPEN			20
#define		NR_INODE		32
#define		NR_SUPER		8
#define		NR_HASH			307
#define		BLOCK_SIZE		1024//缓冲区以块管理，每块的字节数大小
#define		INODES_PER_BLOCK ((BLOCK_SIZE)/(sizeof(DINODE)))
#define		START_BUFFER_ADDR (0x100000)
#define		UNUSED			0	//未使用
#define		DEV_MEM			1	//内存设备	/dev/mem
#define		DEV_FD			2	//软盘设备	/dev/fd
#define		DEV_HD			3	//硬盘设备	/dev/hd
#define		DEV_TTYX		4	//tty串行终端设备	/dev/ttyx
#define		DEV_TTY			5	//tty终端设备		/dev/tty
#define		DEV_LP			6	//打印设备			/dev/lp
#define		DEV_UNNAME_PIPE 7	//没有命名的管道	unamed pipes

#define		READ 0
#define		WRITE 1
#define		READA 2		/* read-ahead - don't pause */
#define		WRITEA 3	/* "write-ahead" - silly, but somewhat useful */
typedef	struct task_struct PROCESS;

typedef	struct buffer_head {
	t_8*	b_data;				//指针
	u_32	b_block_nr;			//块号
	u_16	b_dev;				//数据设备号
	u_8		b_uptodate;			//更新标志
	u_8		b_dirt;				//修改标志，0:未修改 1:已修改
	u_8		b_count;			//使用的用户数
	u_8		b_lock;				//缓冲区是否被锁定0 - 未锁定, 1 - 已锁定
	PROCESS*	b_wait;			//指向等待该缓冲区解锁的任务
	//下面四个指针用于缓冲区的管理
	struct buffer_head*	b_prev;		//hash队列上前一块
	struct buffer_head*	b_next;		//hash队列上下一块
	struct buffer_head*	b_prev_free;//空闲表上前一块
	struct buffer_head*	b_next_free;//空闲表上下一块
} BUFFER_HEAD;

typedef struct d_inode {
	u_16 i_mode;
	u_16 i_uid;
	u_32 i_size;
	u_32 i_time;
	u_8 i_gid;
	u_8 i_nlinks;
	u_16 i_zone[9];
} DINODE;

typedef	struct m_inode {
	u_16	i_mode;				//文件类型和属性
	u_16	i_uid;				//用户id
	u_32	i_size;				//文件大小
	u_32	i_mtime;			//文件修改时间
	u_8		i_gid;				//组id
	u_8		i_nlinks;			//文件目录项链接数
	u_16	i_zone[9];			//
	/* these are in memory also */
	PROCESS* i_wait;			//等待该i结点的进程
	u_32	i_atime;			//最后访问时间
	u_32	i_ctime;			//i结点自身修改时间
	u_16	i_dev;				//i结点所在的设备号
	u_16	i_num;				//i结点号
	u_16	i_count;			//i结点被使用的次数
	u_8		i_lock;				//锁定标志
	u_8		i_dirt;				//已修改标志
	u_8		i_pipe;				//管道标志
	u_8		i_mount;			//安装标志
	u_8		i_seek;				//搜寻标志
	u_8		i_update;			//更新标志
} MINODE;

typedef	struct file {
	u_16	f_mode;			//文件操作模式
	u_16	f_flags;		//文件打开和控制标志位
	u_16	f_count;		//对应文件句柄数
	MINODE* f_inode;		//指向对应的i结点
	t_32	f_pos;			//文件位置
} FILE;

typedef struct super_block {
	u_16 s_ninodes;
	u_16 s_nzones;
	u_16 s_imap_blocks;
	u_16 s_zmap_blocks;
	u_16 s_firstdatazone;
	u_16 s_log_zone_size;
	u_32 s_max_size;
	u_16 s_magic;
	/* These are only in memory */
	BUFFER_HEAD* s_imap[8];
	BUFFER_HEAD* s_zmap[8];
	u_16 s_dev;
	MINODE* s_isup;
	MINODE* s_imount;
	u_32 s_time;
	PROCESS* s_wait;
	u_8 s_lock;
	u_8 s_rd_only;
	u_8 s_dirt;
} SUPER_BLOCK;

//缓冲区块总数
EXTERN	u_32	NR_BUFFERS;
//缓冲块hash数组
EXTERN BUFFER_HEAD* BUFFER_HASH_TABLE[NR_HASH];
EXTERN SUPER_BLOCK SUPER_BLK[];
EXTERN CRW_PTR CRW_TABLE[];

//hash算法
#define _HASHFN(dev,block) (((u_16)(dev^block))%NR_HASH)
#define HASH(dev,block) BUFFER_HASH_TABLE[_HASHFN(dev,block)]
//(缓冲块的修改标志<<1)+锁定标志 组成缓冲块的权重值
#define BADNESS(bh) (((bh)->b_dirt<<1)+(bh)->b_lock)

PUBLIC	void	initBuffer(t_32 mem_size);
PUBLIC	BUFFER_HEAD*	bread(int dev, int block);
PUBLIC	void brelse(BUFFER_HEAD* buf);
PUBLIC	BUFFER_HEAD* getblk(int dev, int block);
PUBLIC	BUFFER_HEAD* getHashTable(int dev, int block);
PRIVATE BUFFER_HEAD* findBuffer(int dev, int block);
PRIVATE void waitOnBuffer(BUFFER_HEAD* bh);
PUBLIC	int syncDev(int dev);

PUBLIC	void syncInodes();
PRIVATE void waitOnInode(MINODE* inode);
PRIVATE void writeInode(MINODE* inode);
PRIVATE void removeFromQueues(BUFFER_HEAD* bh);
PRIVATE void insertIntoQueues(BUFFER_HEAD* bh);
PRIVATE int rwTtyx(int rw, u_32 minor, char * buf, int count, u_32* pos);

#endif	//_OS_FS_H_