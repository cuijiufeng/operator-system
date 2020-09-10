/**
 * 2020��09��09�� 15/56/42
 * ��ӡ�����غ���
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
	//...��һ�������ĵ�ַ
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

	//������ʽ������fmt
	for (ch = buf; *fmt; fmt++)
	{
		//�������%d�ȸ�ʽ�����������buf����
		if (*fmt != '%') {
			*ch++ = *fmt;
			continue;
		}

		flags = 0;
		repeat:
		++fmt;		//����%�ַ�
		switch (*fmt)
		{
			case '-': flags |= LEFT; goto repeat;		//�����
			case '0': flags |= ZEROPAD; goto repeat;	//��λ��0
		}

		//����������
		field_width = -1;
		//����������ַ�
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
				//�����������
				if (!(flags & LEFT))
				{
					//ǰ�Լ�����һ��λ�÷�%c�ַ�
					while (--field_width > 0)
					{
						*ch++ = ' ';
					}
				}
				//ȡ�ַ�����buf����
				*ch++ = *((char*)arg_addr);
				//��һ�������ĵ�ַ
				arg_addr++;
				//�������������
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
				//ȡ�ַ������׵�ַ
				s = (char*)(*arg_addr);
				//��һ�������ĵ�ַ
				arg_addr++;
				//���sΪ��
				if (!s)
				{
					s = "<NULL>";
				}
				len = strlen(s);
				//����������
				if (!(flags & LEFT))
				{
					while (len < field_width--)
					{
						*ch++ = ' ';
					}
				}
				//�����ַ�
				for (i = 0; i < len; ++i)
				{
					*ch++ = *s++;
				}
				//������Ҷ���
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
			//���ֻ��һ��������%�ַ������ӡ%�ַ�
			default:
			{
				//��ӡ���Թ�ȥ��%�ַ�
				if (*fmt != '%')
				{
					*ch++ = '%';
				}
				//��ӡ��ǰ�ַ�
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
	*ch = '\0';			//�ַ���������־
	return ch - buf;	//�ַ�����
}

PRIVATE char* number(char* str, int num, int base, int width, int flags)
{
	int i = 0;
	char sign=0;
	char tmp[10];
	char c = flags & ZEROPAD ? '0' : ' ';
	const char up[] = "0123456789ABCDEF";
	const char low[] = "0123456789abcdef";
	//������з�����������С��0���������ӡ��������ǰ��Ӹ��ż���
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
		//����ת��
		while (num != 0)
		{
			tmp[i++] = (flags & SMALL) ? low[num % base] : up[num % base];
			num /= base;
		}
	}
	//��ʾ����
	if (sign)
	{
		*str++ = sign;
	}
	//�����ʮ���������2����ʾ0x
	if (base == 16)
	{
		width -= 2;
		*str++ = '0';
		*str++ = (flags & SMALL) ? 'x' : 'X';
	}
	width -= i;
	//������������
	if (!(flags&LEFT))
	{
		while (width-- > 0)
		{
			*str++ = c;
		}
	}
	//��ʾ����
	while (i-- > 0) 
	{
		*str++ = tmp[i];
	}
	//����������
	while (width-- > 0)
	{
		*str++ = c;
	}
	return str;
}