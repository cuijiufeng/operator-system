/**
 * 2020年07月19日 08/51/29
 * 关于保存模式的一些定义
 */

#ifndef _OS_PROTECT_H_
#define _OS_PROTECT_H_

#define	GDT_SIZE	128
#define	IDT_SIZE	256

 //GDT描述符结构体
typedef struct s_descriptor
{
	t_16	limit_low;
	t_16	base_low;
	t_8		base_mid;
	t_8		attr1;
	t_8		limit_high_attr2;
	t_8		base_high;
} DESCRIPTOR;

//门描述符结构体
typedef struct s_gate
{
	t_16	offset_low;
	t_16	selector;
	t_8		dcount;
	t_8		attr;
	t_16	offset_high;
} GATE;

#endif // !_OS_PROTECT_H_
