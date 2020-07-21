/**
 * 2020年07月21日 21/56/00
 * 8259相关操作及其初始化
 */
#include	"type.h"
#include	"protect.h"
#include	"global.h"
#include	"interrupt.h"
#include	"klib.h"

PUBLIC init8259A()
{
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
}