/**
 * 2020年07月17日 10:24:15
 * 定义一些全局的常量
 */
 
#ifndef	_OS_GLOBAL_H_
#define	_OS_GLOBAL_H_

#define	PUBLIC
#define	PRIVATE	static			//static类型修饰之后只能在本文件中是可见的
#define FALSE	0				//假值
#define TRUE	1				//真值
#define	NULL	0

t_8			GDT_PTR[6];			//0~15:limit	16~47:base	用于保存寄存器gdtr中的值，前两个字节保存gdt的界限，后四个字节保存gdt的地址
DESCRIPTOR	GDT[GDT_SIZE];		//定义描述符表
t_8			IDT_PTR[6];
GATE		IDT[IDT_SIZE];

#endif