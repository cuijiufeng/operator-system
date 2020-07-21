/**
 * 2020年07月21日 22/30/11
 * 关于调用到的函数的声明
 */
#ifndef _OS_KLIB_H_
#define _OS_KLIB_H_

 //klib.asm
void	outByte(t_port port, t_8 value);
t_8		inByte(t_port port);
void	disableInt();
void	enableInt();
void	disableIrq(int irq);
void 	enableIrq(int irq);

#endif // !_OS_KLIB_H_
