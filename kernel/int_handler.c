/**
 * 2020年07月22日 22/33/39
 * 中断处理程序
 */
#include	<type.h>
#include	<fs.h>
#include	<int.h>
#include	<blk_drv/blk.h>
#include	<syscall.h>
#include	<lib.h>

//---------------------------------------------------------------------------------------
//8259中断调用子程序表
T_PF_IRQ_HANDLER	IRQ_TABLE[NR_IRQ];	
//函数指针数组，系统调用中断处理子程序
T_PF_SYS_CALL	SYS_CALL_TABLE[] = { sysFork,sysPause,sysSetup,sysWrite };
//---------------------------------------------------------------------------------------

PUBLIC	void	syserrHandler(int err_no, int err_code, int eip, int cs, int eflags)
{
	int i;
	char err_description[][64] = {	//中断，异常信息
		"#DE Divide Error",
		"#DB RESERVED",
		"- NMI Interrupt",
		"#BP Breakpoint",
		"#OF Overflow",
		"#BR BOUND Range Exception",
		"#UD Invalid Opcode {Undefined Opcode}",
		"#NM Device not Available {No Math Coprocessor}",
		"#DF Double Fault",
		"Coprocessor Segment Overrun {reserved}",
		"#TS Invalid TSS",
		"#NP Segment Not Present",
		"#SS Stack-Segment Fault",
		"#GP General Protection",
		"#PF Page Fault",
		"- {Intel reserved. Do Notuse}",
		"#MF x87 FPU Floating-Poing Error {Math Fault}",
		"#AC Alignment Check",
		"#MC Machine Check",
		"#XF SIMD Floating-Poing Exception"
	};

	printk("Exception! --> %s\n", err_description[err_no]);	//打印中断提示信息
	printk("Vector:%X\n", err_no);			//打印向量号
	printk("Error code:%X\n", err_code);	//出错码
	printk("EIP:%X\n", eip);				//打印eip
	printk("CS:%X\n", cs);					//打印cs
	printk("EFLAGS:%X\n", eflags);			//打印标志寄存器
}