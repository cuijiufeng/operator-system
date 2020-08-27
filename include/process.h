/**
 * 2020年08月15日 09/40/32
 * 进程头文件
 */
#ifndef _OS_PROCESS_H_
#define	_OS_PROCESS_H_
#define	NR_INIT_TASKS	1
#define	NR_TASKS	64
#define TASK_RUNNING			0	//正在运行
#define TASK_INTERRUPTIBLE		1	//可中断的
#define TASK_UNINTERRUPTIBLE	2	//不可中断的
#define TASK_ZOMBIE				3	//
#define TASK_STOPPED			4	//停止
//进程控制块PCB
typedef	struct task_struct {
	//这些是硬编码的-别碰
	t_32	state;					//任务的运行状态-1不可运行, 0就绪, >0已停止
	u_32	counter;				//任务的运行时间片
	u_32	priority;				//任务的优先级
	u_32	signal;					//信号，用于进程间的通信
	SIGNAL sigaction[32];			//信号执行属性结构
	u_32	blocked;				//进程信号屏蔽码
	/* various fields */
	u_32	exit_code;				//任务停止执行后的退出码，由父进程来取
	u_32	start_code;				//代码段地址
	u_32	end_code;				//代码长度
	u_32	end_data;				//代码长度+数据长度
	u_32	brk;					//总长度
	u_32	start_stack;			//堆栈段地址
	u_32	pid;					//进程标识号
	t_32	father;					//父进程标识号
	u_32	pgrp;					//进程组号
	u_32	session;				//会话号
	u_32	leader;					//会话首领
	u_16	uid;					//用户标识号id
	u_16	euid;					//有效用户id
	u_16	suid;					//保存的用户id
	u_32	alarm;					//报警定时值
	u_32	utime;					//用户态运行的时间
	u_32	stime;					//系统态运行的时间
	u_32	cutime;					//子进程用户态运行的时间
	u_32	cstime;					//子进程系统态运行的时间
	u_32	start_time;				//进程开始运行时刻
	// 文件系统信息
	t_32	tty;					//进程使用tty终端的子设备号 -1表示没有使用tty
	u_16	umask;					//文件创建属性屏蔽位
	MINODE* pwd;					//当前工作目录i结点的结构指针
	MINODE* root;					//根目录i结点的结构指针
	MINODE* executable;				//执行文件i结点的结构指针
	u_32	close_on_exec;			//执行时关闭文件名柄位图标志
	FILE* filp[NR_OPEN];			//文件结构指针表
	//任务
	DESCRIPTOR	ldt[3];				//局部描述符表 0空 1代码段cs 2数据和堆栈段ds&ss
	TSS			tss;				//TSS
} PROCESS;
//联合体-进程PCB控制块与进程堆栈使用同一块内存
typedef union TASK_UNION {
	PROCESS task;
	u_8 stack[PAGE_SIZE];
} PROCESS_UNION;

//初始化进程
PROCESS_UNION INIT_TASKS[NR_INIT_TASKS];
//进程指针数组
PROCESS* TASKS[NR_TASKS] = {&(INIT_TASKS[0].task),};
//当前正在执行的进程
PROCESS* CURRENT = &(INIT_TASKS[0].task);

//---------------------------------------------------------------------------------------
//相关函数声明
PUBLIC	void initSchedule();
PUBLIC	t_32	findEmptyProcess();
PRIVATE	t_32	copyMem(t_32 nr, PROCESS* p);
PUBLIC	t_32	copyProcess(t_32 nr, u_32 ebp, u_32 edi, u_32 esi, u_32 gs, u_32 none,
			u_32 ebx, u_32 ecx, u_32 edx,
			u_32 fs, u_32 es, u_32 ds,
			u_32 eip, u_32 cs, u_32 eflags, u_32 esp, u_32 ss);
//---------------------------------------------------------------------------------------

#endif // !_OS_PROCESS_H_
