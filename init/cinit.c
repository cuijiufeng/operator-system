/**
 * 2020年07月17日 10:30:37
 * 1.c代码开始，此函数中主要工作是把gdt等数据结构copy到kernel中，方便控制使用
 * 2.初始化工作
 */
#include	<type.h>
#include	<protect.h>
#include	<int.h>
#include	<mm.h>
#include	<fs.h>
#include	<blk_drv/blk.h>
#include	<lib.h>

//这些是在setup里确定的
#define	EXT_MEM_SIZE	(*((t_32*)0X70000))
#define	ROOT_DEV_TYPE	(*((t_32*)0X70004))
#define	DRIVER_INFO		((t_8*)0X70010)

PUBLIC void cinit()
{
	//初始化8259
	outByte(INT_M_CTL, 0x11);					//主8259A							ICW1
	outByte(INT_M_CTLMASK, INT_VECTOR_IRQ0);	//设置主8259A的中断入口地址为0x20	ICW2
	outByte(INT_M_CTLMASK, 0x4);				//IR2对应'从8259A'					ICW3
	outByte(INT_M_CTLMASK, 0x1);				//									ICW4
	
	outByte(INT_S_CTL, 0x11);					//从8259A							ICW1
	outByte(INT_S_CTLMASK, INT_VECTOR_IRQ8);	//设置从8259A的中断入口地址为0x28	ICW2
	outByte(INT_S_CTLMASK, 0x2);				//对应'主8259A'的IR2				ICW3
	outByte(INT_S_CTLMASK, 0x1);				//									ICW4
	
	outByte(INT_M_CTLMASK, 0xFB);				//屏蔽‘主8259A’所有中断			OCW1
	outByte(INT_S_CTLMASK, 0xFF);				//屏蔽‘从8259A’所有中断			OCW1
	
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
	
	//ROOT_DEV
	ROOT_DEV = ROOT_DEV_TYPE;
	//硬盘参数
	memcpy(&HD_INFO, DRIVER_INFO, 0x10);

	initIdtDesc();
	initMemory(EXT_MEM_SIZE);
}