/**
 * 2020年09月05日 14/00/18
 * 终端
 */
#ifndef _OS_TTY_H_
#define	_OS_TTY_H_

/*	intr=^C		quit=^|		erase=del	kill=^U
	 eof=^D		vtime=\0	vmin=\1		sxtc=\0
	 start=^Q	stop=^S		susp=^Z		eol=\0
	 reprint=^R	discard=^U	werase=^W	lnext=^V
	 eol2=\0
*/
#define INIT_C_CC "\003\034\177\025\004\0\1\0\021\023\032\0\022\017\027\026\0"
#define TTY_BUF_SIZE 512

//tty字符缓冲队列。用于tty读、写和辅助缓冲队列
typedef struct tty_queue {
	u_32 data;		//队列缓冲区含有字符行数值(不是当前字符数)。对于串口终端，则存放串行端口的地址
	u_32 head;		//头指针
	u_32 tail;		//尾指针
	PROCESS* proc_list;		//等待进程列表
	u_32 buf[TTY_BUF_SIZE];	//队列缓冲区，存放数据
} TTY_QUEUE;

//tty数据结构
typedef struct tty_struct {
	TERMIOS termios;						//终端io属性和控制字符数据结构
	u_32	pgrp;							//进程组号
	int		stopped;						//停止标志
	void (*write)(struct tty_struct* tty);	//tty写函数指针
	TTY_QUEUE read_q;						//tty读队列
	TTY_QUEUE write_q;						//tty写队列
	TTY_QUEUE secondary;					//tty辅助队列
} TTY_STRUCT;

//tty读写缓冲区队列地址表
EXTERN	TTY_QUEUE* TTY_QUEUE_LIST[];
//tty数据结构数组。其中包含三个初始化的tty_struct,分别是控制台，串口终端1和串口终端2
EXTERN	TTY_STRUCT TTY_TABLE[];

PUBLIC	int ttyWrite(unsigned channel, char * buf, int nr);

#endif // !_OS_TTY_H_
