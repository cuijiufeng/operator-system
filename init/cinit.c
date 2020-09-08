/**
 * 2020年07月17日 10:30:37
 * 1.c代码开始，此函数中主要工作是把gdt等数据结构copy到kernel中，方便控制使用
 * 2.初始化工作
 */
#include	<type.h>
#include	<mm.h>
#include	<protect.h>
#include	<int.h>
#include	<blk_drv/blk.h>
#include	<lib.h>
#include	<time.h>

//这些是在setup里确定的
#define	EXT_MEM_SIZE	(*((t_32*)0x9A000))
#define	ROOT_DEV_TYPE	(*((t_32*)0x9A004))
#define	DRIVER_INFOS	((t_8*)0x9A010)
t_32				ROOT_DEV;

PUBLIC void cinit()
{
	//gdt拷贝
	memcpy(&GDT, (void*)(*((t_32*)(&GDT_PTR[2]))), *((t_16*)(&GDT_PTR[0]))+1);

	//取得gdt_ptr的地址
	t_16*	p_gdt_limit = (t_16*)(&GDT_PTR[0]);
	t_32*	p_gdt_base = (t_32*)(&GDT_PTR[2]);
	//修改gdt_ptr的值
	*p_gdt_limit = GDT_SIZE * sizeof(DESCRIPTOR) - 1;
	*p_gdt_base = (t_32)&GDT;
	
	//取得idt_ptr的地址
	t_16*	p_idt_limit = (t_16*)(&IDT_PTR[0]);
	t_32*	p_idt_base = (t_32*)(&IDT_PTR[2]);
	//修改idt_ptr的值
	*p_idt_limit = IDT_SIZE * sizeof(GATE) - 1;
	*p_idt_base = (t_32)&IDT;

	lgdtr(GDT_PTR);	//设置新的GDT
	lidtr(IDT_PTR);	//设置新的IDT
	
	//ROOT_DEV
	ROOT_DEV = ROOT_DEV_TYPE;
	//硬盘参数
	memcpy(&DRIVE_INFO, DRIVER_INFOS, 0x10);

	initIdtDesc();
	init8259A();
	initTime();
	initMemory(EXT_MEM_SIZE);
	initSchedule();
	initTty();

	//设置ltr、lldt
	ltr(SELECTOR_FIRST_TASK_TSS);
	lldtr(SELECTOR_FIRST_TASK_LDT);
}