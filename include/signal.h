/**
 * 2020年08月15日 10/21/25
 * 信号头文件
 */
#ifndef _OS_SIGNAL_H_
#define	_OS_SIGNAL_H_
//nr号信号对应的值
#define	S(nr)		(1<<((nr)-1))
//阻塞信号对应的值的并值
#define	BLOCKABLE	(~(S(SIGKILL) | S(SIGSTOP)))
#define SIGHUP		 1
#define SIGINT		 2
#define SIGQUIT		 3
#define SIGILL		 4
#define SIGTRAP		 5
#define SIGABRT		 6
#define SIGIOT		 6
#define SIGUNUSED	 7
#define SIGFPE		 8
#define SIGKILL		 9
#define SIGUSR1		10
#define SIGSEGV		11
#define SIGUSR2		12
#define SIGPIPE		13
#define SIGALRM		14
#define SIGTERM		15
#define SIGSTKFLT	16
#define SIGCHLD		17
#define SIGCONT		18
#define SIGSTOP		19
#define SIGTSTP		20
#define SIGTTIN		21
#define SIGTTOU		22

typedef	struct sigaction {
	T_PF_SA_HANDLER handler;		//对应某信号要采取的行动
	u_32	sa_mask;				//对信号的屏蔽码
	u_32	sa_flags;				//改变信号处理进程的信号集
	T_PF_SA_RESTORER restorer;		//恢复函数指针
} SIGNAL;

//---------------------------------------------------------------------------------------
//相关函数声明
PUBLIC	void	setSignal(SIGNAL* signal, T_PF_SA_HANDLER handler, u_32 sa_mask, u_32 sa_flags, T_PF_SA_RESTORER restorer);
//---------------------------------------------------------------------------------------

#endif // !_OS_SIGNAL_H_
