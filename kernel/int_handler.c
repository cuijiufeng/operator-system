/**
 * 2020年07月22日 22/33/39
 * 中断处理程序
 */
#include	<type.h>
#include	<int.h>
#include	<syscall.h>
#include	<time.h>
#include	<fs.h>
#include	<mm.h>
#include	<signal.h>
#include	<protect.h>
#include	<process.h>
#include	<lib.h>

//---------------------------------------------------------------------------------------
//8259中断调用子程序表
T_PF_IRQ_HANDLER	IRQ_TABLE[NR_IRQ];	
//函数指针数组，系统调用中断处理子程序
T_PF_SYS_CALL	SYS_CALL_TABLE[] = { sysFork,sysPause };
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

	cleanScreen();
	displayStr("Exception! --> ", 0);
	displayStr(err_description[err_no], sizeof("Exception! --> ")*2);//打印中断提示信息
	displayStr("\n", (sizeof("Exception! --> ")+sizeof(err_description[err_no])) * 2);

	displayStr("Vector:", 160);
	displayInt(err_no, 160 +sizeof("Vector:")*2);						//打印向量号
	displayStr("\n", 160 + (sizeof("Vector:") + sizeof(err_no)) * 2);

	if (err_code != 0xFFFFFFFF)
	{
		displayStr("Error code:", 320);
		displayInt(err_code, 320 + sizeof("Error code:") * 2);		//如果有出错码，把出错码打印出来
		displayStr("\n", 320 + (sizeof("Error code:") + sizeof(err_code)) * 2);
	}

	displayStr("EIP:", 480);
	displayInt(eip, 480 + sizeof("EIP:") * 2);						//打印eip
	displayStr("\n", 480 + (sizeof("EIP:") + sizeof(eip)) * 2);

	displayStr("CS:", 640);
	displayInt(cs, 640 + sizeof("CS:") * 2);						//打印cs
	displayStr("\n", 640 + (sizeof("CS:") + sizeof(cs)) * 2);

	displayStr("EFLAGS:", 800);
	displayInt(eflags, 800 + sizeof("EFLAGS:") * 2);				//打印标志寄存器
	displayStr("\n", 800 + (sizeof("EFLAGS:") + sizeof(eflags)) * 2);
}

//时钟中断处理子程序
PUBLIC	void	timerHandler(int irq)
{
	int i, num = TICKS;
	char ch;
	char str[8], *p = str;
	for (i = 28; i >= 0; i -= 4)
	{
		ch = (num >> i) & 0xF;
		ch += '0';
		if (ch > '9')
		{
			ch += 7;
		}
		*p++ = ch;
	}
	*((u_16*)(0xb8000)) = (0x0F00 | str[0]);
	*((u_16*)(0xb8002)) = (0x0F00 | str[1]);
	*((u_16*)(0xb8004)) = (0x0F00 | str[2]);
	*((u_16*)(0xb8006)) = (0x0F00 | str[3]);
	*((u_16*)(0xb8008)) = (0x0F00 | str[4]);
	*((u_16*)(0xb800A)) = (0x0F00 | str[5]);
	*((u_16*)(0xb800C)) = (0x0F00 | str[6]);
	*((u_16*)(0xb800E)) = (0x0F00 | str[7]);

	TICKS++;						//时钟中断发生次数加一
	if ((--CURRENT->counter) > 0)	//当前进程的时间片减一,如果减到=0则重新调度
	{
		return 0;
	}
	schedule();						//进程调度
}