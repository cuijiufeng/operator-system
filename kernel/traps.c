/**
 * 2020年07月22日 22/33/39
 * 中断处理程序
 */
#include	<type.h>
#include	<lib.h>

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