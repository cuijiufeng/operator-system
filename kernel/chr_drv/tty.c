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
#include	<lib.h>

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
	initConsole();
	
	//���������ж�
	enableIrq(KEYBOARD_IRQ);
	putIrqHandler(KEYBOARD_IRQ, keyboardHandler);
}

//todo
PUBLIC	int	ttyRead(unsigned channel, char * buf, int nr)
{

}

PUBLIC	int ttyWrite(unsigned channel, char * buf, int nr)
{
	int cr_flag=0;
	char c, *b = buf;
	//����̨�ն�
	TTY_STRUCT* tty = TTY_TABLE + channel;
	while (nr > 0)
	{
		while(nr >0 && !fullQueue(&tty->write_q))
		{
			c=*b;
			//���ģʽ��־����������־λ
			if(tty->termios.c_oflag & OPOST)
			{
				//�����ʱ���س���CRӳ��ɻ��з�NL
				if (c=='\r' && (tty->termios.c_oflag & OCRNL))
				{
					c='\n';
				}
				//���з�NLִ�лس����Ĺ���
				else if (c=='\n' && (tty->termios.c_oflag & ONLRET))
				{
					c='\r';
				}
				//�����ʱ�����з�NLӳ��ɻس�-���з�CR-NL
				if (c=='\n' && !cr_flag && (tty->termios.c_oflag & ONLCR)) 
				{
					cr_flag = 1;
					//����س���
					tty->write_q.buf[tty->write_q.head] = 13;
					tty->write_q.head = (tty->write_q.head + 1) % TTY_BUF_SIZE;
					continue;
				}
				//�����ʱ��Сд�ַ�ת���ɴ�д�ַ�
				if (tty->termios.c_oflag & OLCUC)
				{
					c = c - 0x20;
				}
			}
			tty->write_q.buf[tty->write_q.head] = c;
			tty->write_q.head = (tty->write_q.head + 1) % TTY_BUF_SIZE;
			b++;
			nr--;
			cr_flag = 0;
		}
		tty->write(tty);
	}
	return b - buf;
}

PUBLIC	t_bool	fullQueue(TTY_QUEUE* queue)
{
	return queue->tail == ((queue->head + 1) % TTY_BUF_SIZE);
}