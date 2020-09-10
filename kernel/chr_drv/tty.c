/**
 * 2020��09��05�� 14/00/18
 * �ն˴��������غ���
 */
#include	<type.h>
#include	<fs.h>
#include	<mm.h>
#include	<signal.h>
#include	<protect.h>
#include	<process.h>
#include	<chr_drv/termios.h>
#include	<chr_drv/tty.h>
#include	<chr_drv/chr.h>
#include	<int.h>

TTY_QUEUE* TTY_QUEUE_LIST[] = { 
	&TTY_TABLE[0].read_q, &TTY_TABLE[0].write_q,
	&TTY_TABLE[1].read_q, &TTY_TABLE[1].write_q,
	&TTY_TABLE[2].read_q, &TTY_TABLE[2].write_q 
};

TTY_STRUCT TTY_TABLE[] = {
	{
		{ICRNL,			//������CRת��ΪNL
		OPOST | ONLCR,	//�����CRת��ΪCRNL
		0,				//����ģʽ��־��
		ISIG | ICANON | ECHO | ECHOCTL | ECHOKE,	//����ģʽ��־��
		0,				//��·���0-tty
		INIT_C_CC},		//�����ַ�����
		0,				//������ʼ������
		0,				//��ʼֹͣ��־
		conWrite,		//����̨д����
		{0,0,0,0,{0,}},	//����̨���������
		{0,0,0,0,{0,}},	//����̨д�������
		{0,0,0,0,{0,}}	//����̨��������
	},{
		{0,				//����ģʽ��־��������ת��
		0,				//���ģʽ��־��������ת��
		B2400 | CS8,	//����ģʽ��־����2400bps��8λ�ṹ�ṹ
		0,				//����ģʽ��־��
		0,				//��·���0-tty
		INIT_C_CC},		//�����ַ�����
		0,				//������ʼ������
		0,				//��ʼֹͣ��־	
		rsWrite,		//����1�ն�д����
		{0x3f8,0,0,0,{0,}},//�����ն�1��������нṹ��ʼֵ
		{0x3f8,0,0,0,{0,}},//�����ն�1д������нṹ��ʼֵ
		{0,0,0,0,{0,}}	//�����ն�1����������нṹ��ʼֵ
	},{
		{0,				//����ģʽ��־��������ת��
		0,				//���ģʽ��־��������ת��
		B2400 | CS8,	//����ģʽ��־����2400bps��8λ�ṹ�ṹ
		0,				//����ģʽ��־��
		0,				//��·���0-tty
		INIT_C_CC},		//�����ַ�����
		0,				//������ʼ������
		0,				//��ʼֹͣ��־	
		rsWrite,		//����1�ն�д����
		{0x2f8,0,0,0,{0,}},//�����ն�1��������нṹ��ʼֵ
		{0x2f8,0,0,0,{0,}},//�����ն�1д������нṹ��ʼֵ
		{0,0,0,0,{0,}}	//�����ն�1����������нṹ��ʼֵ
	}
};

PUBLIC	void	initTty()
{
	//���������ж�
	enableIrq(KEYBOARD_IRQ);
	putIrqHandler(KEYBOARD_IRQ, keyboardHandler);
}

PUBLIC	int ttyWrite(unsigned channel, char * buf, int nr)
{
	char *b = buf;
	//����̨�ն�
	TTY_STRUCT* tty = TTY_TABLE + channel;
	while (nr > 0)
	{

	}
	return b - buf;
}