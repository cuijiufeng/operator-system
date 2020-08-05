/**
 * 2020年07月19日 08/51/29
 * 关于保存模式的一些定义
 */
#ifndef _OS_PROTECT_H_
#define _OS_PROTECT_H_

//---------------------------------------------------------------------------------------
#define	GDT_SIZE			128
#define	IDT_SIZE			256
/* 权限 */
#define	PRIVILEGE_KERNEL	0
#define	PRIVILEGE_TASK		1
#define	PRIVILEGE_USER		3
/* 存储段描述符类型值说明 */
#define	DA_DRW				0x92	/* 存在的可读写数据段属性值		*/
#define	DA_C				0x98	/* 存在的只执行代码段属性值		*/
/* 系统段描述符类型值说明	 */
#define	DA_LDT				0x82	/* 局部描述符表段类型值		*/
#define	DA_386TSS			0x89	/* 可用 386 任务状态段类型值*/
#define	DA_386IGate			0x8E	/* 386 中断门类型值			*/
//选择子索引
#define	INDEX_DUMMY			0		// ┓
#define	INDEX_FLAT_C		1		// ┃
#define	INDEX_FLAT_RW		2		// ┣ LOADER 里面已经确定了的.
#define	INDEX_VIDEO			3		// ┛
#define	INDEX_TSS			4		
#define	INDEX_LDT_FIRST		5
//选择子
#define	SELECTOR_DUMMY		0		// ┓
#define	SELECTOR_KERNEL_CS	0x08	// ┃
#define	SELECTOR_KERNEL_DS	0x10	// ┣ LOADER 里面已经确定了的.
#define	SELECTOR_VIDEO		(0x18+3)// ┛<-- RPL=3
//选择子类型值说明
#define	SA_RPL_MASK			0xFFFC
#define	SA_RPL0				0
#define	SA_RPL1				1
#define	SA_RPL2				2
#define	SA_RPL3				3
#define	SA_TI_MASK			0xFFFB
#define	SA_TIG				0			//描述符GDT在中
#define	SA_TIL				4			//描述符LDT在中
//---------------------------------------------------------------------------------------

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

typedef struct s_tss {
	t_32	backlink;
	t_32	esp0;		/* stack pointer to use during interrupt */
	t_32	ss0;		/*   "   segment  "  "    "        "     */
	t_32	esp1;
	t_32	ss1;
	t_32	esp2;
	t_32	ss2;
	t_32	cr3;
	t_32	eip;
	t_32	flags;
	t_32	eax;
	t_32	ecx;
	t_32	edx;
	t_32	ebx;
	t_32	esp;
	t_32	ebp;
	t_32	esi;
	t_32	edi;
	t_32	es;
	t_32	cs;
	t_32	ss;
	t_32	ds;
	t_32	fs;
	t_32	gs;
	t_32	ldt;
	t_16	trap;
	t_16	iobase;	/* I/O位图基址大于或等于TSS段界限，就表示没有I/O许可位图 */
	/*t_8	iomap[2];*/
} TASK_STATE_SEGMENT;

//---------------------------------------------------------------------------------------
t_8					GDT_PTR[6];			//0~15:limit	16~47:base	用于保存寄存器gdtr中的值，前两个字节保存gdt的界限，后四个字节保存gdt的地址
DESCRIPTOR			GDT[GDT_SIZE];		//定义描述符表
t_8					IDT_PTR[6];
GATE				IDT[IDT_SIZE];
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
//相关函数声明
PUBLIC 	void 	initIdtDesc();
PUBLIC	void	setIdtDesc(t_8 vector, t_8 desc_type, T_PF_INT_HANDLER handler, t_8 privilege);
PUBLIC	void	setGdtDesc(DESCRIPTOR* p_desc, t_32 base, t_32 limit, t_16 attribute);
//---------------------------------------------------------------------------------------

#endif // !_OS_PROTECT_H_
