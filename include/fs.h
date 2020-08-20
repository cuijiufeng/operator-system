/**
 * 2020年07月30日 16:50:22
 * 文件系统
 */
#ifndef	_OS_FS_H_
#define	_OS_FS_H_

#define		NR_OPEN 20
typedef	struct task_struct PROCESS;
t_32		ROOT_DEV;

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

#endif	_OS_FS_H_