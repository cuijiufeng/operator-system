/**
 * 2020年07月17日 10:30:37
 * 1.c代码开始，此函数中主要工作是把gdt等数据结构copy到kernel中，方便控制使用
 * 2.初始化工作
 */
#include	"type.h"
#include	"protect.h"
#include	"interrupt.h"
#include	"global.h"
 
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

	 //初始化TSS
	 memset(&TSS, 0, sizeof(TSS));
	 TSS.ss0 = SELECTOR_KERNEL_DS;
	 TSS.iobase = sizeof(TSS);	//没有I/O许可位图

	 init8259A();
	 initProtect();
 }