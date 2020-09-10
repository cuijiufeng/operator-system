/**
 * 2020年09月09日 15/56/42
 * 打印输出相关函数
 */
#include	<type.h>
#include	<print.h>
#include	<fs.h>
#include	<mm.h>
#include	<signal.h>
#include	<protect.h>
#include	<process.h>
#include	<chr_drv/termios.h>
#include	<chr_drv/tty.h>
#include	<lib.h>

PRIVATE	char	buf[512];

PUBLIC	int	printf(const char* fmt, ...)
{
	//...第一个参数的地址
	char* arg_addr = (char*)&fmt + 4;
	int i = vsprintf(buf, fmt, arg_addr);
	ttyWrite(0, buf, i);
	return i;
}

PUBLIC	int	vsprintf(char buf[], const char * fmt, int* arg_addr)
{
	char* ch;
	int flags;
	int field_width;

	//遍历格式化参数fmt
	for (ch = buf; *fmt; fmt++)
	{
		//如果不是%d等格式化参数则放入buf数组
		if (*fmt != '%') {
			*ch++ = *fmt;
			continue;
		}

		flags = 0;
		repeat:
		++fmt;		//跳过%字符
		switch (*fmt)
		{
			case '-': flags |= LEFT; goto repeat;		//左对齐
			case '0': flags |= ZEROPAD; goto repeat;	//空位补0
		}

		//计算参数宽度
		field_width = -1;
		//如果是数字字符
		if (*fmt >= '0' && *fmt <= '9')
		{
			int i = 0;
			while (*fmt >= '0' && *fmt <= '9')
			{
				field_width = i * 10 + *fmt - '0';
				++fmt;
			}
		}

		switch (*fmt)
		{
			//%c
			case 'c':
			{
				//如果是左对齐的
				if (!(flags & LEFT))
				{
					//前自减，留一个位置放%c字符
					while (--field_width > 0)
					{
						*ch++ = ' ';
					}
				}
				//取字符放入buf数组
				*ch++ = *((char*)arg_addr);
				//下一个参数的地址
				arg_addr++;
				//如果不是左对齐的
				while (--field_width > 0)
				{
					*ch++ = ' ';
				}
			} break;
			//%s
			case 's':
			{
				char* s;
				int len, i;
				//取字符串的首地址
				s = (char*)(*arg_addr);
				//下一个参数的地址
				arg_addr++;
				//如果s为空
				if (!s)
				{
					s = "<NULL>";
				}
				len = strlen(s);
				//如果是左对齐
				if (!(flags & LEFT))
				{
					while (len < field_width--)
					{
						*ch++ = ' ';
					}
				}
				//拷贝字符
				for (i = 0; i < len; ++i)
				{
					*ch++ = *s++;
				}
				//如果是右对齐
				while (len < field_width--)
				{
					*ch++ = ' ';
				}
			} break;
			case 'x': 
			{
				flags |= SMALL;
			}
			case 'X':
			{
				ch = number(ch, *arg_addr, 16, field_width, flags);
				arg_addr++;
			} break;
			case 'd':
			case 'i':
			{
				flags |= SIGN;
			}
			case 'u':
			{
				ch = number(ch, *arg_addr, 10, field_width, flags);
				arg_addr++;
			} break;
			//如果只是一个单纯的%字符，则打印%字符
			default:
			{
				//打印忽略过去的%字符
				if (*fmt != '%')
				{
					*ch++ = '%';
				}
				//打印当前字符
				if (*fmt)
				{
					*ch++ = *fmt;
				}
				else
				{
					--fmt;
				}
			} break;
		}
	}
	*ch = '\0';			//字符串结束标志
	return ch - buf;	//字符个数
}

PRIVATE char* number(char* str, int num, int base, int width, int flags)
{
	int i = 0;
	char sign=0;
	char tmp[10];
	char c = flags & ZEROPAD ? '0' : ' ';
	const char up[] = "0123456789ABCDEF";
	const char low[] = "0123456789abcdef";
	//如果是有符号数并且是小于0的数，则打印正数再在前面加负号即可
	if (flags & SIGN && num < 0)
	{
		sign = '-';
		num = -num;
		width--;
	}
	if (num == 0)
	{
		tmp[i++] = '0';
	}
	else
	{
		//进制转换
		while (num != 0)
		{
			tmp[i++] = (flags & SMALL) ? low[num % base] : up[num % base];
			num /= base;
		}
	}
	//显示符号
	if (sign)
	{
		*str++ = sign;
	}
	//如果是十六进制则减2，显示0x
	if (base == 16)
	{
		width -= 2;
		*str++ = '0';
		*str++ = (flags & SMALL) ? 'x' : 'X';
	}
	width -= i;
	//如果不是左对齐
	if (!(flags&LEFT))
	{
		while (width-- > 0)
		{
			*str++ = c;
		}
	}
	//显示数字
	while (i-- > 0) 
	{
		*str++ = tmp[i];
	}
	//如果是左对齐
	while (width-- > 0)
	{
		*str++ = c;
	}
	return str;
}