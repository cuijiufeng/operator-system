/**
 * 2020年08月15日 10/10/34
 * 进程
 */
#include	<type.h>
#include	<fs.h>
#include	<mm.h>
#include	<signal.h>
#include	<protect.h>
#include	<process.h>
#include	<lib.h>
#include	<time.h>

PRIVATE	int	last_pid;
//初始化进程
PROCESS_UNION INIT_TASKS[NR_INIT_TASKS];
//进程指针数组
PROCESS* TASKS[NR_TASKS] = { &(INIT_TASKS[0].task), };
//当前正在执行的进程
PROCESS* CURRENT = &(INIT_TASKS[0].task);

PUBLIC	void initSchedule()
{
	last_pid = 0;
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
	setTss(&INIT_TASKS[0].task.tss, 0, (u_32)INIT_TASKS + PAGE_SIZE, SELECTOR_KERNEL_DS, 0, 0, 0, 0, PAGE_DIR, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		SELECTOR_KERNEL_DS | SA_TIL | SA_RPL3, SELECTOR_KERNEL_DS | SA_TIL | SA_RPL3, SELECTOR_KERNEL_DS | SA_TIL | SA_RPL3, 
		SELECTOR_KERNEL_DS | SA_TIL | SA_RPL3, SELECTOR_KERNEL_DS | SA_TIL | SA_RPL3, SELECTOR_KERNEL_DS | SA_TIL | SA_RPL3, 
		SELECTOR_FIRST_TASK_LDT, 0, sizeof(TSS));

	//设置tss这个描述符
	setGdtDesc(&GDT[SELECTOR_FIRST_TASK_TSS>>3], &INIT_TASKS[0].task.tss, sizeof(TSS), DA_386TSS);
	//设置ldt描述符表在gdt中
	setGdtDesc(&GDT[SELECTOR_FIRST_TASK_LDT>>3], &INIT_TASKS[0].task.ldt, sizeof(DESCRIPTOR) * 3, DA_LDT);
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
PUBLIC	t_32	copyProcess(t_32 nr, u_32 none, u_32 edx, u_32 ecx, u_32 ebx,						//edx,ecx,ebx系统调用的参数
	u_32 EDI, u_32 ESI, u_32 EBP, u_32 EBX, u_32 EDX, u_32 ECX, u_32 gs, u_32 fs, u_32 es, u_32 ds,	//保存ring3时寄存器的值
	u_32 eip, u_32 cs, u_32 eflags, u_32 esp, u_32 ss)	//ring3时的值
{
	int i;
	FILE* f;
	PROCESS* p = (PROCESS*)getFreePage();
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
	p->start_time = TICKS;
	p->tss.backlink = 0;
	p->tss.esp0 = (u_32)p + PAGE_SIZE;
	p->tss.ss0 = SELECTOR_KERNEL_DS;
	p->tss.eip = eip;
	p->tss.flags = eflags;
	p->tss.eax = 0;
	p->tss.ecx = ECX;
	p->tss.edx = EDX;
	p->tss.ebx = EBX;
	p->tss.esp = esp;
	p->tss.ebp = EBP;
	p->tss.esi = ESI;
	p->tss.edi = EDI;
	p->tss.es = es & 0xffff;
	p->tss.cs = cs & 0xffff;
	p->tss.ss = ss & 0xffff;
	p->tss.ds = ds & 0xffff;
	p->tss.fs = fs & 0xffff;
	p->tss.gs = gs & 0xffff;
	p->tss.ldt = (nr<<4) + SELECTOR_FIRST_TASK_LDT;
	p->tss.trap = 0;
	p->tss.iobase = sizeof(TSS);
	if(copyMem(nr, p))
	{
		//释放内存页
		TASKS[nr] = NULL;
		freePage((u_32)p);
		return -1;
	}
	//拷贝进程打开的文件
	for (i = 0; i < NR_OPEN; i++)
	{
		if ((f = p->filp[i]))
		{
			f->f_count++;
		}
	}
	if (CURRENT->pwd)
	{
		CURRENT->pwd->i_count++;
	}
	if (CURRENT->root)
	{
		CURRENT->root->i_count++;
	}
	if (CURRENT->executable)
	{
		CURRENT->executable->i_count++;
	}
	//设置tss与ldt
	setGdtDesc(&GDT[(SELECTOR_FIRST_TASK_TSS >> 3) + 2*nr], &(TASKS[nr]->tss), sizeof(TSS), DA_386TSS);
	setGdtDesc(&GDT[(SELECTOR_FIRST_TASK_LDT >> 3) + 2*nr], &(TASKS[nr]->ldt), sizeof(DESCRIPTOR)*3, DA_LDT);
	p->state = TASK_RUNNING;
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
		freePageTables(new_data_base, data_limit);//释放页表映射
		return -1;
	}
	return 0;
}

//进程调度函数，在没有进程可以调度的情况下0号进程会被调度
PUBLIC	void	schedule()
{
	int next, c, i;

	//遍历所有的任务(不包括0号进程)
	for (i = 1; i < NR_TASKS; i++) 
	{
		//如果任务不为空
		if (!TASKS[i]) 
		{
			//如果进程是可中断状态并且有信号(信号值不为阻塞信号)
			if ((TASKS[i]->signal & (BLOCKABLE & ~TASKS[i]->blocked)) && TASKS[i]->state == TASK_INTERRUPTIBLE)
			{
				TASKS[i]->state = TASK_RUNNING;
			}
		}
	}

	while (1)
	{
		c = -1;							//默认值为-1，保证0号进程会被默认调度的
		next = 0;						//默认调度0号进程
		//遍历所有的任务(不包括0号进程，0号进程永远会被调度)
		for (i = 1; i < NR_TASKS; i++)
		{
			//如果进程来存在则下个进程
			if (!TASKS[i])
			{
				continue;
			}
			//如果进程是可运行并且时间片最大
			if (TASKS[i]->state == TASK_RUNNING && TASKS[i]->counter > c)
			{
				c = TASKS[i]->counter;	//保存当前进程的进间片
				next = i;				//保存当前进程的进程槽位号
			}
		}
		//如果找到可调度的进程则去调度
		if (c)
		{
			break;
		}
		//如果没有可调度的进程，重新设置进程的时间片
		for (i = 1; i < NR_TASKS; i++)
		{
			if (TASKS[i])
			{
				TASKS[i]->counter = TASKS[i]->priority;
			}
		}
	}
	SWITCH_TO(next);
}

//睡眠进程，如果p不为空,则唤醒p进程
PUBLIC	void sleepOn(PROCESS** p)
{
	PROCESS* tmp;
	//p指向进程等待队列，如果进程等待队列为空，则直接返回
	if (!p) 
	{
		return;
	}
	//0号进程不被状态影响，所以不能睡眠0号进程
	if (CURRENT == &TASKS[0])
	{
		printk("task[0] trying to sleep");
		while (1);
	}
	tmp = *p;			//tmp指向进程等待队列
	*p = CURRENT;		//将当前进程放入进程等待队列
	CURRENT->state = TASK_UNINTERRUPTIBLE;	//设置当前进程的状态
	schedule();			//重新调度进程
	*p = tmp;			//当进程被唤醒时继续往下执行
	//如果进程等待队列里还有等待同一资源的进程应该一同被唤醒
	if (tmp)
	{
		tmp->state = TASK_RUNNING;
	}
}

//唤醒进程 
PUBLIC	void wakeUp(PROCESS** p)
{
	//如果进程等待队列不为空并且等待进程不为空则设置进程的状态
	if (p && *p) {
		(*p)->state = TASK_RUNNING;
		*p = NULL;
	}
}