/**
 * 2020年07月21日 22/30/11
 * 关于调用到的函数的声明
 */
#ifndef _OS_KLIB_H_
#define _OS_KLIB_H_

//klib.asm
void	outByte(t_port port, u_8 value);
u_8		inByte(t_port port);
void	lgdtr(u_32 gdt_ptr);
void	lidtr(u_32 idt_ptr);
void	ltr(u_16 ltr);
void	lldtr(u_16 ldt);
void	loadCR3(u_32 addr);
u_32	getDescLimit(u_16 segment);
void	disableIrq(int irq);
void 	enableIrq(int irq);
void	displayStr(char* str, int pos);
void	cleanScreen();
//memlib.asm
u_32	memcpy(u_32 dest, u_32 src, u_32 count);
void	memset(u_32 dest, u_8 ch, u_32 count);

#endif // !_OS_KLIB_H_
