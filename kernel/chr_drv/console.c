/**
 * 2020年09月05日 20/29/22
 * 控制台
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

#define	VIDEO_NUM_COLUMNS 	80		//一行多少列
#define	VIDEO_SIZE_ROW 		160		//一行多少字节数
#define	VIDEO_NUM_LINES		25		//一屏多少行
#define	VIDEO_PAGE 			1		//初始显示页面
#define	VIDEO_MEM_START 	0xB8000 //显存起始地址
#define	VIDEO_MEM_END 		0xC0000 //显存结束地址+1
#define	VIDEO_PORT_REG 		0x3D4	//控制端口
#define	VIDEO_PORT_VAL 		0x3D5	//数据端口
#define VIDEO_IDX_CURSOR_H	0xE		//register index of cursor position (MSB)
#define VIDEO_IDX_CURSOR_L	0xF		//register index of cursor position (LSB)

PRIVATE	u_32	origin = 0xB8000;			//滚屏时起始内存地址
PRIVATE	u_32	scr_end = 0xB8000+25*160;	//滚屏时结束内存地址
PRIVATE	u_32	x = 0;                      //当前光标列号
PRIVATE	u_32	y = 9;                      //当前光标行号
PRIVATE	u_32	pos;		  //光标对应的显示存位置
PRIVATE	u_32	top = 0;      //滚屏时顶行行号
PRIVATE	u_32	bottom = 25;  //滚屏时底行行号
PRIVATE	u_16	attr=0x0F00;  //打印字符的属性

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
		//如果是可打印字符
		if(ch>31 && ch<127)
		{
			//如果是到达行尾则换行
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
		//ESC字符
		else if (ch == 27)
		{
		}
		//\n、\v、\f字符
		else if (ch == 10 || ch == 11 || ch == 12)
		{
			lf();
		}
		//\r回车符
		else if (ch == 13)
		{
			cr();
		}
		//擦出字符
		else if (ch == tty->termios.c_cc[VERASE])
		{
			del();
		}
		//退格字符
		else if (ch == 8) 
		{
			if (x) 
			{
				x--;
				pos -= 2;
			}
		}
		//\t制表符
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

//换行
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

//回车
PRIVATE void	cr()
{
	pos -= x*2;
	x = 0;
}

//删除字符
PRIVATE void	del()
{
	if (x) 
	{
		pos -= 2;
		x--;
		*((u_16*)pos) = attr | 0x20;
	}
}

//设置光标的位置
PRIVATE	void	setCursor()
{
	//让光标跟随打印的字符
	disableInt();
	outByte(VIDEO_PORT_REG, VIDEO_IDX_CURSOR_H);
	outByte(VIDEO_PORT_VAL, ((pos - VIDEO_MEM_START) >> 9) & 0xFF);
	outByte(VIDEO_PORT_REG, VIDEO_IDX_CURSOR_L);
	outByte(VIDEO_PORT_VAL, ((pos - VIDEO_MEM_START) >> 1) & 0xFF);
	enableInt();
}

//设置初始显示内存地址
PRIVATE void	setOrigin()
{
	disableInt();
	outByte(VIDEO_PORT_REG, 12);
	outByte(VIDEO_PORT_VAL, ((origin - VIDEO_MEM_START) >> 9) & 0xFF);
	outByte(VIDEO_PORT_REG, 13);
	outByte(VIDEO_PORT_VAL, ((origin - VIDEO_MEM_START) >> 1) & 0xFF);
	enableInt();
}

//屏幕向上滚动
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

//屏幕向下滚动
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