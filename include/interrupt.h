/**
 * 2020��07��21�� 22/08/04
 * �����жϵ�ͷ�ļ�-��������������������
 */
#ifndef	_OS_INTERRUPT_H_
#define	_OS_INTERRUPT_H_

//---------------------------------------------------------------------------------------
//8259�˿�
#define	NR_IRQ			16		//��Ӧ8259���жϴ����ӳ���ĸ���
#define INT_M_CTL		0x20	//��8259A��Ӧ�Ķ˿ڵ�ַ20H
#define INT_M_CTLMASK	0x21	//��8259A��Ӧ�Ķ˿ڵ�ַ21H
#define INT_S_CTL		0xA0	//��8259A��Ӧ�Ķ˿ڵ�ַA0H
#define INT_S_CTLMASK	0xA1	//��8259A��Ӧ�Ķ˿ڵ�ַA1H
//8259�ж�������
#define INT_VECTOR_IRQ0	0x20
#define	INT_VECTOR_IRQ8	0x28
#define	CLOCK_IRQ		0
#define	KEYBOARD_IRQ	1
#define	CASCADE_IRQ		2	/* cascade enable for 2nd AT controller */
#define	ETHER_IRQ		3	/* default ethernet interrupt vector */
#define	SECONDARY_IRQ	3	/* RS232 interrupt vector for port 2 */
#define	RS232_IRQ		4	/* RS232 interrupt vector for port 1 */
#define	XT_WINI_IRQ		5	/* xt winchester */
#define	FLOPPY_IRQ		6	/* floppy disk */
#define	PRINTER_IRQ		7
#define	PS2_MOUSE		12
#define	AT_WINI_IRQ		14	/* at winchester */
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
//8259��غ�������
PUBLIC init8259A();
//---------------------------------------------------------------------------------------

#endif