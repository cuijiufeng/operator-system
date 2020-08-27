/**
 * 2020年08月15日 10/10/34
 * 进程
 */
#include	<type.h>
#include	<time.h>
#include	<lib.h>
#include	<signal.h>
#include	<fs.h>
#include	<mm.h>
#include	<protect.h>
#include	<process.h>

PRIVATE	int	last_pid;

PUBLIC	void initSchedule()
{
	//初始化默认任务
	memset(&INIT_TASKS[0].task, 0, sizeof(PROCESS_UNION));
	INIT_TASKS[0].task.counter = 15;
	INIT_TASKS[0].task.priority = 15;
	INIT_TASKS[0].task.father = -1;
	INIT_TASKS[0].task.tty = -1;
	INIT_TASKS[0].task.umask = 0022;
	//ldt
	setGdtDesc(&INIT_TASKS[0].task.ldt[0], 0, 0, 0);
	setGdtDesc(&INIT_TASKS[0].task.ldt[1], 0, 0x9F, DA_LIMIT_4K | DA_32 | DA_CR | PRIVILEGE_USER);
	setGdtDesc(&INIT_TASKS[0].task.ldt[2], 0, 0x9F, DA_LIMIT_4K | DA_32 | DA_DRW | PRIVILEGE_USER);
	//tss
	setTss(&INIT_TASKS[0].task.tss, 0, KERNEL_STACK + PAGE_SIZE, SELECTOR_KERNEL_DS, 0, 0, 0, 0, PAGE_DIR, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		SELECTOR_KERNEL_DS | SA_TIL | SA_RPL3, SELECTOR_KERNEL_DS | SA_TIL | SA_RPL3, SELECTOR_KERNEL_DS | SA_TIL | SA_RPL3, 
		SELECTOR_KERNEL_DS | SA_TIL | SA_RPL3, SELECTOR_KERNEL_DS | SA_TIL | SA_RPL3, SELECTOR_KERNEL_DS | SA_TIL | SA_RPL3, 
		SELECTOR_FIRST_TASK_LDT, 0, sizeof(TSS));

	//设置tss这个描述符
	setGdtDesc(&GDT[SELECTOR_FIRST_TASK_TSS>>3], &INIT_TASKS[0].task.tss, sizeof(INIT_TASKS[0].task.tss), DA_386TSS);
	//设置ldt描述符表在gdt中
	setGdtDesc(&GDT[SELECTOR_FIRST_TASK_LDT>>3], &INIT_TASKS[0].task.ldt, sizeof(INIT_TASKS[0].task.ldt), DA_LDT);
}

PUBLIC	t_32	findEmptyProcess()
{
	int i;
	++last_pid;
	//为新的进程槽中的进程生成一个pid，循环用来保证新的pid是没有被用过的
	for (i = 0; i < NR_TASKS; i++)
	{
		//如果新的进程pid已经被使用
		if (TASKS[i] && TASKS[i]->pid == last_pid)
		{
			++last_pid;
			i = -1;
		}
	}
	//寻找进程槽位
	for (i = 0; i < NR_TASKS; i++)
	{
		//如果进程槽位没有被使用
		if (!TASKS[i])
		{
			return i;
		}
	}
	//如果所有的进程槽位都已经被使用
	return -1;
}

//拷贝进程，返回进程的pid。nr是进程槽位号，none是系统调用中断处理子程序的返回地址
PUBLIC	t_32	copyProcess(t_32 nr, u_32 ebp, u_32 edi, u_32 esi, u_32 gs, u_32 none,
	u_32 ebx, u_32 ecx, u_32 edx,	//系统调用的参数
	u_32 fs, u_32 es, u_32 ds,		//保存ring3时寄存器的值
	u_32 eip, u_32 cs, u_32 eflags, u_32 esp, u_32 ss)	//ring3时的值
{
	PROCESS* p;
	p = (PROCESS*)getFreePage();
	//如果没有空闲的内存页
	if (!p) 
	{
		return -1;					//如果失败返回-1
	}
	TASKS[nr] = p;
	*p = *CURRENT;
	//设置进程的属性
	p->state = TASK_UNINTERRUPTIBLE;
	p->pid = last_pid;
	p->father = CURRENT->pid;
	p->counter = p->priority;
	p->signal = 0;
	p->alarm = 0;
	p->leader = 0;		/* process leadership doesn't inherit */
	p->utime = p->stime = 0;
	p->cutime = p->cstime = 0;
	p->start_time = TICKS;
	p->tss.backlink = 0;
	p->tss.esp0 = KERNEL_STACK + PAGE_SIZE;
	p->tss.ss0 = SELECTOR_KERNEL_DS;
	p->tss.eip = eip;
	p->tss.flags = eflags;
	p->tss.eax = 0;
	p->tss.ecx = ecx;
	p->tss.edx = edx;
	p->tss.ebx = ebx;
	p->tss.esp = esp;
	p->tss.ebp = ebp;
	p->tss.esi = esi;
	p->tss.edi = edi;
	p->tss.es = es & 0xffff;
	p->tss.cs = cs & 0xffff;
	p->tss.ss = ss & 0xffff;
	p->tss.ds = ds & 0xffff;
	p->tss.fs = fs & 0xffff;
	p->tss.gs = gs & 0xffff;
	p->tss.ldt = (nr<<3) + SELECTOR_FIRST_TASK_LDT;
	p->tss.trap = 0;
	p->tss.iobase = sizeof(TSS);
	if(copyMem(nr, p))
	{
		//todo 释放内存页
		return -1;
	}
	//todo
	return last_pid;			//如果拷贝进程成功，则返回子进程的的pid
}

//拷贝进程的内存
PRIVATE	t_32	copyMem(t_32 nr, PROCESS* p)
{
	u_32 data_limit, code_limit;
	u_32 old_code_base, old_data_base;
	u_32 new_code_base, new_data_base;
	new_code_base = new_data_base = nr*0x4000000;	//新进程的代码段基址与数据段基址一样，使用进程槽位号*64MB
	code_limit = getDescLimit(SELECTOR_LDT_CS | SA_TIL | SA_RPL3);
	data_limit = getDescLimit(SELECTOR_LDT_DS | SA_TIL | SA_RPL3);
	old_code_base = getDescBase(&CURRENT->ldt[1]);
	old_data_base = getDescBase(&CURRENT->ldt[2]);
	if(old_code_base != old_data_base)				//不允许数据段基址与代码段基址不相等
	{
		return -1;
	}
	setDescBase(&p->ldt[1], new_code_base);			//设置新进程的代码段基址和数据段基址
	setDescBase(&p->ldt[2], new_data_base);
	//其实并没有真正的拷贝进程的内存页，而是让新进程与父进程的线性地址映射到同一段物理地址上，两个进程共享这段内存
	if(copyPageTables(new_data_base, old_data_base, data_limit+1))
	{
		
	}
	return 0;
}