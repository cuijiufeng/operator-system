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
#define	PRIVILEGE_KERNEL	(0<<5)
#define	PRIVILEGE_USER		(3<<5)
/* 存储段描述符类型值说明 */
#define	DA_32				0x4000	//32 位段
#define	DA_LIMIT_4K			0x8000	//段界限粒度为 4K 字节
#define DA_DR				0x90	//存在的只读数据段类型值
#define DA_DRW				0x92	//存在的可读写数据段属性值
#define DA_DRWA				0x93	//存在的已访问可读写数据段类型值
#define DA_C				0x98	//存在的只执行代码段属性值
#define DA_CR				0x9A	//存在的可执行可读代码段属性值
#define DA_CCO				0x9C	//存在的只执行一致代码段属性值
#define DA_CCOR				0x9E	//存在的可执行可读一致代码段属性值
/* 系统段描述符类型值说明	 */
#define	DA_LDT				0x82	/* 局部描述符表段类型值		*/
#define	DA_386TSS			0x89	/* 可用 386 任务状态段类型值*/
#define	DA_386IGate			0x8E	/* 386 中断门类型值			*/
//选择子
#define	SELECTOR_DUMMY		0		// ┓
#define	SELECTOR_KERNEL_CS	0x08	// ┣ LOADER 里面已经确定了的.
#define	SELECTOR_KERNEL_DS	0x10	// ┛
#define	SELECTOR_FIRST_TASK_TSS	0x18
#define	SELECTOR_FIRST_TASK_LDT	0x20
#define	SELECTOR_LDT_DUMMY	0
#define	SELECTOR_LDT_CS		0x8
#define	SELECTOR_LDT_DS		0x10
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
	u_16	limit_low;
	u_16	base_low;
	u_8		base_mid;
	u_8		attr1;
	u_8		limit_high_attr2;
	u_8		base_high;
} DESCRIPTOR;

//门描述符结构体
typedef struct s_gate
{
	u_16	offset_low;
	u_16	selector;
	u_8		dcount;
	u_8		attr;
	u_16	offset_high;
} GATE;

typedef struct s_tss {
	u_32	backlink;
	u_32	esp0;		/* stack pointer to use during interrupt */
	u_32	ss0;		/*   "   segment  "  "    "        "     */
	u_32	esp1;
	u_32	ss1;
	u_32	esp2;
	u_32	ss2;
	u_32	cr3;
	u_32	eip;
	u_32	flags;
	u_32	eax;
	u_32	ecx;
	u_32	edx;
	u_32	ebx;
	u_32	esp;
	u_32	ebp;
	u_32	esi;
	u_32	edi;
	u_32	es;
	u_32	cs;
	u_32	ss;
	u_32	ds;
	u_32	fs;
	u_32	gs;
	u_32	ldt;
	u_16	trap;
	u_16	iobase;	/* I/O位图基址大于或等于TSS段界限，就表示没有I/O许可位图 */
	/*t_8	iomap[2];*/
} TSS;

//---------------------------------------------------------------------------------------
u_8			KERNEL_STACK[PAGE_SIZE];	//内核栈
u_8			GDT_PTR[6];					//0~15:limit	16~47:base	用于保存寄存器gdtr中的值，前两个字节保存gdt的界限，后四个字节保存gdt的地址
DESCRIPTOR	GDT[GDT_SIZE];				//定义描述符表
u_8			IDT_PTR[6];					//中断描述符表基址与界限
GATE		IDT[IDT_SIZE];				//中断描述符表
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
//8259相关函数声明
PUBLIC	void	init8259A();
PUBLIC	void	setIrqHandler(int irq, T_PF_IRQ_HANDLER handler);
//相关函数声明
PUBLIC 	void 	initIdtDesc();
PUBLIC	void	setDescBase(DESCRIPTOR* p_desc, u_32 base);
PUBLIC	u_32	getDescBase(DESCRIPTOR* p_desc);
PUBLIC	void	setIdtDesc(u_8 vector, u_8 desc_type, T_PF_INT_HANDLER handler, u_8 privilege);
PUBLIC	void	setGdtDesc(DESCRIPTOR* p_desc, u_32 base, u_32 limit, u_16 attribute);
PUBLIC	void	setTss(TSS* p_tss, u_32	backlink, u_32 esp0, u_32 ss0, u_32 esp1, u_32 ss1, u_32 esp2, u_32 ss2, u_32 cr3, u_32 eip, u_32 flags,
					u_32 eax, u_32 ecx, u_32 edx, u_32 ebx, u_32 esp, u_32 ebp, u_32 esi, u_32 edi, u_32 es, u_32 cs, u_32 ss, u_32 ds, u_32 fs, 
					u_32 gs, u_32 ldt, u_16 trap, u_16 iobase);
//---------------------------------------------------------------------------------------

#endif // !_OS_PROTECT_H_
