/**
 * 2020年07月17日 10:23:19
 * 定义一些类型
 */
 
#ifndef	_OS_TYPE_H_
#define	_OS_TYPE_H_

#define	PUBLIC
#define	PRIVATE	static			//static类型修饰之后只能在本文件中是可见的
#define	EXTERN	extern
#define FALSE	0				//假值
#define TRUE	1				//真值
#define	NULL	0

typedef int 			t_32;
typedef short			t_16;
typedef char			t_8;
typedef unsigned int 	u_32;
typedef unsigned short	u_16;
typedef unsigned char	u_8;
typedef unsigned int	t_port;
typedef unsigned char	t_bool;

typedef void(*T_PF_INT_HANDLER)();			//定义一个函数指针类型t_pf_int_handler，t_pf_int_handler类型的变量指向void function()类型的函数
typedef void(*T_PF_IRQ_HANDLER)(int irq);	//8259中断处理子程序的函数类型
typedef void *T_PF_SYS_CALL;				//系统调用中断处理子程序的函数类型。这样，无论系统调用何种函数，都不会有编译错误
typedef void(*T_PF_SA_HANDLER)(int);		//对应某信号要采取的行动
typedef void(*T_PF_SA_RESTORER)();			//信号恢复函数指针

#endif