/**
 * 2020年08月15日 10/21/25
 * 信号头文件
 */
#ifndef _OS_SIGNAL_H_
#define	_OS_SIGNAL_H_

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
