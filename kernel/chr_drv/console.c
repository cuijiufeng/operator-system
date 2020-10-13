/**
 * 2020��09��05�� 20/29/22
 * ����̨
 */
#include	<type.h>
#include	<fs.h>
#include	<mm.h>
#include	<signal.h>
#include	<protect.h>
#include	<process.h>
#include	<chr_drv/termios.h>
#include	<chr_drv/tty.h>
#include	<lib.h>

#define	VIDEO_NUM_COLUMNS 	80		//һ�ж�����
#define	VIDEO_SIZE_ROW 		160		//һ�ж����ֽ���
#define	VIDEO_NUM_LINES		25		//һ��������
#define	VIDEO_PAGE 			1		//��ʼ��ʾҳ��
#define	VIDEO_MEM_START 	0xB8000 //�Դ���ʼ��ַ
#define	VIDEO_MEM_END 		0xC0000 //�Դ������ַ+1
#define	VIDEO_PORT_REG 		0x3D4	//���ƶ˿�
#define	VIDEO_PORT_VAL 		0x3D5	//���ݶ˿�
#define VIDEO_IDX_CURSOR_H	0xE		//register index of cursor position (MSB)
#define VIDEO_IDX_CURSOR_L	0xF		//register index of cursor position (LSB)

PRIVATE	u_32	origin = 0xB8000;			//����ʱ��ʼ�ڴ��ַ
PRIVATE	u_32	scr_end = 0xB8000+25*160;	//����ʱ�����ڴ��ַ
PRIVATE	u_32	x = 0;                      //��ǰ����к�
PRIVATE	u_32	y = 9;                      //��ǰ����к�
PRIVATE	u_32	pos;		  //����Ӧ����ʾ��λ��
PRIVATE	u_32	top = 0;      //����ʱ�����к�
PRIVATE	u_32	bottom = 25;  //����ʱ�����к�
PRIVATE	u_16	attr=0x0F00;  //��ӡ�ַ�������

PRIVATE void	del();
PRIVATE void	cr();
PRIVATE	void	lf();
PRIVATE	void	scrollDown();
PRIVATE	void	scrollUp();
PRIVATE void	setOrigin();
PRIVATE	void	setCursor();

PUBLIC	void	initConsole()
{
	x = 0;
	y = 9;
	top = 0;
	bottom = 25;
	pos = 0xB8000 + y * 160 + x * 2;
	setOrigin();
	setCursor();
}

PUBLIC	void conWrite(TTY_STRUCT* tty)
{
	int nr;
	char ch;
	
	nr = (tty->write_q.head - tty->write_q.tail) % TTY_BUF_SIZE;
	while(nr--)
	{
		ch = tty->write_q.buf[tty->write_q.tail];
		tty->write_q.tail = (tty->write_q.tail + 1) % TTY_BUF_SIZE;
		//����ǿɴ�ӡ�ַ�
		if(ch>31 && ch<127)
		{
			//����ǵ�����β����
			if (x >= VIDEO_NUM_COLUMNS) 
			{
				x -= VIDEO_NUM_COLUMNS;
				pos -= VIDEO_SIZE_ROW;
				lf();
			}
			*((u_16*)pos) = attr | ch;
			x++;
			pos += 2;
		}
		//ESC�ַ�
		else if (ch == 27)
		{
		}
		//\n��\v��\f�ַ�
		else if (ch == 10 || ch == 11 || ch == 12)
		{
			lf();
		}
		//\r�س���
		else if (ch == 13)
		{
			cr();
		}
		//�����ַ�
		else if (ch == tty->termios.c_cc[VERASE])
		{
			del();
		}
		//�˸��ַ�
		else if (ch == 8) 
		{
			if (x) 
			{
				x--;
				pos -= 2;
			}
		}
		//\t�Ʊ��
		else if (ch == 9) 
		{
			ch = 8 - (x & 7);
			x += ch;
			pos += ch << 1;
			if (x > VIDEO_NUM_COLUMNS) 
			{
				x -= VIDEO_NUM_COLUMNS;
				pos -= VIDEO_SIZE_ROW;
				lf();
			}
			ch = 9;
		}
	}
	setCursor();
}

//����
PRIVATE	void	lf()
{
	if (y+1 < bottom) 
	{
		y++;
		pos += VIDEO_SIZE_ROW;
		return;
	}
	scrollUp();
}

//�س�
PRIVATE void	cr()
{
	pos -= x*2;
	x = 0;
}

//ɾ���ַ�
PRIVATE void	del()
{
	if (x) 
	{
		pos -= 2;
		x--;
		*((u_16*)pos) = attr | 0x20;
	}
}

//���ù���λ��
PRIVATE	void	setCursor()
{
	//�ù������ӡ���ַ�
	disableInt();
	outByte(VIDEO_PORT_REG, VIDEO_IDX_CURSOR_H);
	outByte(VIDEO_PORT_VAL, ((pos - VIDEO_MEM_START) >> 9) & 0xFF);
	outByte(VIDEO_PORT_REG, VIDEO_IDX_CURSOR_L);
	outByte(VIDEO_PORT_VAL, ((pos - VIDEO_MEM_START) >> 1) & 0xFF);
	enableInt();
}

//���ó�ʼ��ʾ�ڴ��ַ
PRIVATE void	setOrigin()
{
	disableInt();
	outByte(VIDEO_PORT_REG, 12);
	outByte(VIDEO_PORT_VAL, ((origin - VIDEO_MEM_START) >> 9) & 0xFF);
	outByte(VIDEO_PORT_REG, 13);
	outByte(VIDEO_PORT_VAL, ((origin - VIDEO_MEM_START) >> 1) & 0xFF);
	enableInt();
}

//��Ļ���Ϲ���
PRIVATE	void	scrollUp()
{
	if (scr_end < VIDEO_MEM_END)
	{
		origin += VIDEO_SIZE_ROW;
		scr_end += VIDEO_SIZE_ROW;
		y += 1;
		pos += VIDEO_SIZE_ROW;
		setOrigin();
		setCursor();
	}
}

//��Ļ���¹���
PRIVATE	void	scrollDown()
{
	if (origin > VIDEO_MEM_START)
	{
		origin -= VIDEO_SIZE_ROW;
		scr_end -= VIDEO_SIZE_ROW;
		y -= 1;
		pos -= VIDEO_SIZE_ROW;
		setOrigin();
		setCursor();
	}
}