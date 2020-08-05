/**
 * 2020年07月23日 20/56/08
 * 关于调用到的函数的声明
 */
#include	<type.h>
#include	<lib.h>

//把int数字转换成字符串(十六进制表示)
PRIVATE char* itoa(char str[], int num)
{
	char *p = str;
	char ch;
	int i;
	t_bool	flag = FALSE;
	
	*p++ = '0';
	*p++ = 'x';
	
	if(num == 0)
	{
		*p++ = '0';
	}
	else
	{
		for(i=28; i>=0; i-=4)
		{
			ch = (num>>i) & 0xF;	//取得一个十六进制位
			if(flag || (ch > 0))
			{
				flag = TRUE;
				ch += '0';
				if(ch > '9')
				{
					ch += 7;
				}
				*p++ = ch;
			}
		}
	}
	
	*p = 0;
	return str;
}

//打印int型整数,input为要打印的数字
PUBLIC void displayInt(int input, int pos)
{
	char output[11];				//字符缓存数组	"0xFFFFFFFF"一共11个字符，加上'\0'
	itoa(output, input);
	displayStr(output, pos);	//调用打印函数打印字符串
}