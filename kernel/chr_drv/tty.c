/**
 * 2020年09月05日 14/00/18
 * 终端处理操作相关函数
 */
#include	<type.h>
#include	<fs.h>
#include	<mm.h>
#include	<signal.h>
#include	<protect.h>
#include	<process.h>
#include	<chr_drv/termios.h>
#include	<chr_drv/tty.h>
#include	<chr_drv/chr.h>
#include	<int.h>

EXTERN	TTY_QUEUE* TTY_QUEUE_LIST[] = { 
	&TTY_TABLE[0].read_q, &TTY_TABLE[0].write_q,
	&TTY_TABLE[1].read_q, &TTY_TABLE[1].write_q,
	&TTY_TABLE[2].read_q, &TTY_TABLE[2].write_q 
};

EXTERN	TTY_STRUCT TTY_TABLE[] = {
	{
		{ICRNL,			//将输入CR转换为NL
		OPOST | ONLCR,	//将输出CR转换为CRNL
		0,				//控制模式标志集
		ISIG | ICANON | ECHO | ECHOCTL | ECHOKE,	//本地模式标志集
		0,				//线路规程0-tty
		INIT_C_CC},		//控制字符数组
		0,				//所属初始进程组
		0,				//初始停止标志
		conWrite,		//控制台写函数
		{0,0,0,0,{0,}},	//控制台读缓冲队列
		{0,0,0,0,{0,}},	//控制台写缓冲队列
		{0,0,0,0,{0,}}	//控制台辅助队列
	},{
		{0,				//输入模式标志集，无须转换
		0,				//输出模式标志集，无须转换
		B2400 | CS8,	//控制模式标志集，2400bps，8位结构结构
		0,				//本地模式标志集
		0,				//线路规程0-tty
		INIT_C_CC},		//控制字符数组
		0,				//所属初始进程组
		0,				//初始停止标志	
		rsWrite,		//串口1终端写函数
		{0x3f8,0,0,0,{0,}},//串行终端1读缓冲队列结构初始值
		{0x3f8,0,0,0,{0,}},//串行终端1写缓冲队列结构初始值
		{0,0,0,0,{0,}}	//串行终端1辅助缓冲队列结构初始值
	},{
		{0,				//输入模式标志集，无须转换
		0,				//输出模式标志集，无须转换
		B2400 | CS8,	//控制模式标志集，2400bps，8位结构结构
		0,				//本地模式标志集
		0,				//线路规程0-tty
		INIT_C_CC},		//控制字符数组
		0,				//所属初始进程组
		0,				//初始停止标志	
		rsWrite,		//串口1终端写函数
		{0x2f8,0,0,0,{0,}},//串行终端1读缓冲队列结构初始值
		{0x2f8,0,0,0,{0,}},//串行终端1写缓冲队列结构初始值
		{0,0,0,0,{0,}}	//串行终端1辅助缓冲队列结构初始值
	}
};

PUBLIC	void	initTty()
{
	//开启键盘中断
	enableIrq(KEYBOARD_IRQ);
	putIrqHandler(KEYBOARD_IRQ, keyboardHandler);
}