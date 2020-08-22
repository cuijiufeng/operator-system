/**
 * 2020年08月15日 10/10/34
 * 进程
 */
#include	<type.h>
#include	<lib.h>
#include	<signal.h>
#include	<fs.h>
#include	<mm.h>
#include	<protect.h>
#include	<process.h>

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
	setGdtDesc(&INIT_TASKS[0].task.ldt[1], 0, 0x100, DA_LIMIT_4K | DA_32 | DA_CR | PRIVILEGE_USER);
	setGdtDesc(&INIT_TASKS[0].task.ldt[2], 0, 0x100, DA_LIMIT_4K | DA_32 | DA_DRW | PRIVILEGE_USER);
	//tss
	setTss(&INIT_TASKS[0].task.tss, 0, KERNEL_STACK + PAGE_SIZE, SELECTOR_KERNEL_DS, 0, 0, 0, 0, PAGE_DIR, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		SELECTOR_KERNEL_DS | SA_TIL | SA_RPL3, SELECTOR_KERNEL_DS | SA_TIL | SA_RPL3, SELECTOR_KERNEL_DS | SA_TIL | SA_RPL3, 
		SELECTOR_KERNEL_DS | SA_TIL | SA_RPL3, SELECTOR_KERNEL_DS | SA_TIL | SA_RPL3, SELECTOR_KERNEL_DS | SA_TIL | SA_RPL3, 
		SELECTOR_LDT_DUMMY | SA_RPL3, 0x80000000, sizeof(TSS));

	//设置tss这个描述符
	setGdtDesc(&GDT[SELECTOR_FIRST_TASK_TSS>>3], &INIT_TASKS[0].task.tss, sizeof(INIT_TASKS[0].task.tss), DA_386TSS);
	//设置ldt描述符表在gdt中
	setGdtDesc(&GDT[SELECTOR_FIRST_TASK_LDT>>3], &INIT_TASKS[0].task.ldt, sizeof(INIT_TASKS[0].task.ldt), DA_LDT);
}