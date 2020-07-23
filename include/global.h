/**
 * 2020年07月17日 10:24:15
 * 定义一些全局的变量
 */
 
#ifndef	_OS_GLOBAL_H_
#define	_OS_GLOBAL_H_

t_8					GDT_PTR[6];			//0~15:limit	16~47:base	用于保存寄存器gdtr中的值，前两个字节保存gdt的界限，后四个字节保存gdt的地址
DESCRIPTOR			GDT[GDT_SIZE];		//定义描述符表
t_8					IDT_PTR[6];
GATE				IDT[IDT_SIZE];
TASK_STATE_SEGMENT	TSS;

t_32				K_RE_ENTRY;			//为了解决中断重入增加的变量
void*				P_PROCESS_READY;	//进程表指针
T_PF_IRQ_HANDLER	IRQ_TABLE[NR_IRQ];	//8259中断调用子程序表

#endif