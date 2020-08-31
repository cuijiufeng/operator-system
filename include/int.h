/**
 * 2020年07月21日 22/08/04
 * 关于中断的头文件-函数声明及常量的声明
 */
#ifndef	_OS_INTERRUPT_H_
#define	_OS_INTERRUPT_H_

//---------------------------------------------------------------------------------------
#define	NR_IRQ						16		//对应8259的中断处理子程序的个数
//8259端口
#define INT_M_CTL					0x20	//主8259A对应的端口地址20H
#define INT_M_CTLMASK				0x21	//主8259A对应的端口地址21H
#define INT_S_CTL					0xA0	//从8259A对应的端口地址A0H
#define INT_S_CTLMASK				0xA1	//从8259A对应的端口地址A1H
//8259中断向量号
#define INT_VECTOR_IRQ0				0x20
#define	INT_VECTOR_IRQ8				0x28
#define	CLOCK_IRQ					0
#define	KEYBOARD_IRQ				1
#define	CASCADE_IRQ					2	/* cascade enable for 2nd AT controller */
#define	ETHER_IRQ					3	/* default ethernet interrupt vector */
#define	SECONDARY_IRQ				3	/* RS232 interrupt vector for port 2 */
#define	RS232_IRQ					4	/* RS232 interrupt vector for port 1 */
#define	XT_WINI_IRQ					5	/* xt winchester */
#define	FLOPPY_IRQ					6	/* floppy disk */
#define	PRINTER_IRQ					7
#define	PS2_MOUSE					12
#define	AT_WINI_IRQ					14	/* at winchester */
//异常中断向量号
#define	INT_VECTOR_DIVIDE			0x0
#define	INT_VECTOR_DEBUG			0x1
#define	INT_VECTOR_NMI				0x2
#define	INT_VECTOR_BREAKPOINT		0x3
#define	INT_VECTOR_OVERFLOW			0x4
#define	INT_VECTOR_BOUNDS			0x5
#define	INT_VECTOR_INVAL_OP			0x6
#define	INT_VECTOR_COPROC_NOT		0x7
#define	INT_VECTOR_DOUBLE_FAULT		0x8
#define	INT_VECTOR_COPROC_SEG		0x9
#define	INT_VECTOR_INVAL_TSS		0xA
#define	INT_VECTOR_SEG_NOT			0xB
#define	INT_VECTOR_STACK_FAULT		0xC
#define	INT_VECTOR_PROTECTION		0xD
#define	INT_VECTOR_PAGE_FAULT		0xE
#define	INT_VECTOR_COPROC_ERR		0x10
//系统调用中断号
#define	INT_VECTOR_SYSCALL			0x80
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
//系统错误中断处理程序
void	divideError();
void	singleStepException();
void	nmi();
void	breakpointException();
void	overflow();
void	boundsCheck();
void	invalOpcode();
void	coprNotAvailable();
void	doubleFault();
void	coprSegOverrun();
void	invalTss();
void	segmentNotPresent();
void	stackException();
void	generalProtection();
void	pageFault();
void	coprError();
//8259中断处理函数声明s
void	hwint00();
void	hwint01();
void	hwint02();
void	hwint03();
void	hwint04();
void	hwint05();
void	hwint06();
void	hwint07();
void	hwint08();
void	hwint09();
void	hwint10();
void	hwint11();
void	hwint12();
void	hwint13();
void	hwint14();
void	hwint15();
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
//8259中断调用子程序表
EXTERN	T_PF_IRQ_HANDLER	IRQ_TABLE[NR_IRQ];
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
//中断处理子程序
PUBLIC	void	timerHandler(int irq);
PUBLIC	void	syserrHandler(int err_no, int err_code, int eip, int cs, int eflags);
//---------------------------------------------------------------------------------------

#endif