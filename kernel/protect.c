/**
 * 2020年07月22日 19/37/55
 * 保护模式相关操作及初始化
 */
#include	<type.h>
#include	<mm.h>
#include	<protect.h>
#include	<int.h>
#include	<blk_drv/blk.h>
#include	<syscall.h>

u_8			USER_STACK[PAGE_SIZE];		//进程0ring0栈
u_8			GDT_PTR[6];					//0~15:limit	16~47:base	用于保存寄存器gdtr中的值，前两个字节保存gdt的界限，后四个字节保存gdt的地址
DESCRIPTOR	GDT[GDT_SIZE];				//定义描述符表
u_8			IDT_PTR[6];					//中断描述符表基址与界限
GATE		IDT[IDT_SIZE];				//中断描述符表

PUBLIC	void	init8259A()
{
	//初始化8259
	outByte(INT_M_CTL, 0x11);					//主8259A							ICW1
	outByte(INT_M_CTLMASK, INT_VECTOR_IRQ0);	//设置主8259A的中断入口地址为0x20	ICW2
	outByte(INT_M_CTLMASK, 0x4);				//IR2对应'从8259A'					ICW3
	outByte(INT_M_CTLMASK, 0x1);				//									ICW4

	outByte(INT_S_CTL, 0x11);					//从8259A							ICW1
	outByte(INT_S_CTLMASK, INT_VECTOR_IRQ8);	//设置从8259A的中断入口地址为0x28	ICW2
	outByte(INT_S_CTLMASK, 0x2);				//对应'主8259A'的IR2				ICW3
	outByte(INT_S_CTLMASK, 0x1);				//									ICW4

	outByte(INT_M_CTLMASK, 0xFB);				//屏蔽‘主8259A’所有中断			OCW1
	outByte(INT_S_CTLMASK, 0xFF);				//屏蔽‘从8259A’所有中断			OCW1
}

PUBLIC void initIdtDesc()
{
	//初始化异常中断
	setIdtDesc(INT_VECTOR_DIVIDE,		DA_386IGate, divideError,			PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_DEBUG,		DA_386IGate, singleStepException,	PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_NMI,			DA_386IGate, nmi,					PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_BREAKPOINT,	DA_386IGate, breakpointException,	PRIVILEGE_USER);
	setIdtDesc(INT_VECTOR_OVERFLOW,		DA_386IGate, overflow,				PRIVILEGE_USER);
	setIdtDesc(INT_VECTOR_BOUNDS,		DA_386IGate, boundsCheck,			PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_INVAL_OP,		DA_386IGate, invalOpcode,			PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_COPROC_NOT,	DA_386IGate, coprNotAvailable,		PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_DOUBLE_FAULT,	DA_386IGate, doubleFault,			PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_COPROC_SEG,	DA_386IGate, coprSegOverrun,		PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_INVAL_TSS,	DA_386IGate, invalTss,				PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_SEG_NOT,		DA_386IGate, segmentNotPresent,		PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_STACK_FAULT,	DA_386IGate, stackException,		PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_PROTECTION,	DA_386IGate, generalProtection,		PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_PAGE_FAULT,	DA_386IGate, pageFault,				PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_COPROC_ERR,	DA_386IGate, coprError,				PRIVILEGE_KERNEL);

	//初始化8259中断的IDT
	setIdtDesc(INT_VECTOR_IRQ0 + 0, DA_386IGate, hwint00, PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_IRQ0 + 1, DA_386IGate, hwint01, PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_IRQ0 + 2, DA_386IGate, hwint02, PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_IRQ0 + 3, DA_386IGate, hwint03, PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_IRQ0 + 4, DA_386IGate, hwint04, PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_IRQ0 + 5, DA_386IGate, hwint05, PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_IRQ0 + 6, DA_386IGate, hwint06, PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_IRQ0 + 7, DA_386IGate, hwint07, PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_IRQ8 + 0, DA_386IGate, hwint08, PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_IRQ8 + 1, DA_386IGate, hwint09, PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_IRQ8 + 2, DA_386IGate, hwint10, PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_IRQ8 + 3, DA_386IGate, hwint11, PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_IRQ8 + 4, DA_386IGate, hwint12, PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_IRQ8 + 5, DA_386IGate, hwint13, PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_IRQ8 + 6, DA_386IGate, hwint14, PRIVILEGE_KERNEL);
	setIdtDesc(INT_VECTOR_IRQ8 + 7, DA_386IGate, hwint15, PRIVILEGE_KERNEL);

	//系统调用中断
	setIdtDesc(INT_VECTOR_SYSCALL, DA_386IGate, sysCall, PRIVILEGE_USER);
}

//设置某个8259中断处理程序的子程序
PUBLIC	void	putIrqHandler(int irq, T_PF_IRQ_HANDLER handler)
{
	IRQ_TABLE[irq] = handler;				//设置irq号8259中断处理子程序
}

PUBLIC	u_32	getDescBase(DESCRIPTOR* p_desc)
{
	return ((p_desc->base_high << 24) | (p_desc->base_mid << 16) | p_desc->base_low);
}

PUBLIC	void	setDescBase(DESCRIPTOR* p_desc, u_32 base)
{
	p_desc->base_low = base & 0xFFFF;
	p_desc->base_mid = (base >> 16) & 0xFF;
	p_desc->base_high = (base >> 24) & 0xFF;
}

//初始化一个IDT项			  向量号		类型			中断程序偏移				特权级
PUBLIC	void	setIdtDesc(u_8 vector, u_8 desc_type, T_PF_INT_HANDLER handler, u_8 privilege)
{
	GATE* p_gate = &IDT[vector];				//对应向量号位置的门描述符

	u_32 base = (u_32)handler;
	p_gate->offset_low = base & 0xFFFF;
	p_gate->selector = SELECTOR_KERNEL_CS;
	p_gate->attr = desc_type | privilege;
	p_gate->offset_high = (base >> 16) & 0xFFFF;
}

//初始化GDT的一个描述符项
PUBLIC	void	setGdtDesc(DESCRIPTOR* p_desc, u_32 base, u_32 limit, u_16 attribute)
{
	p_desc->limit_low = limit & 0xFFFF;							//段界限低2个字节
	p_desc->base_low = base & 0xFFFF;							//段基址低2个字节
	p_desc->base_mid = (base >> 16) & 0xFF;						//段基址mid1个字节
	p_desc->attr1 = attribute & 0xFF;							//属性的低1个字节
	p_desc->limit_high_attr2 = (limit >> 16) & 0x0F | (attribute >> 8) & 0xF0;	//界限+属性，共1个字节
	p_desc->base_high = (base >> 24) & 0xFF;						//段基址高1个字节
}

//设置tss
PUBLIC	void	setTss(TSS* p_tss, u_32	backlink, u_32 esp0, u_32 ss0, u_32 esp1, u_32 ss1, u_32 esp2, u_32 ss2, u_32 cr3, u_32 eip, u_32 flags,
					u_32 eax, u_32 ecx, u_32 edx, u_32 ebx, u_32 esp, u_32 ebp, u_32 esi, u_32 edi, u_32 es, u_32 cs, u_32 ss, u_32 ds, u_32 fs,
					u_32 gs, u_32 ldt, u_16 trap, u_16 iobase)
{
	p_tss->backlink = backlink;
	p_tss->esp0 = esp0;
	p_tss->ss0 = ss0;
	p_tss->esp1 = esp1;
	p_tss->ss1 = ss1;
	p_tss->esp2 = esp2;
	p_tss->ss2 = ss2;
	p_tss->cr3 = cr3;
	p_tss->eip = eip;
	p_tss->flags = flags;
	p_tss->eax = eax;
	p_tss->ecx = ecx;
	p_tss->edx = edx;
	p_tss->ebx = ebx;
	p_tss->esp = esp;
	p_tss->ebp = ebp;
	p_tss->esi = esi;
	p_tss->edi = edi;
	p_tss->es = es;
	p_tss->cs = cs;
	p_tss->ss = ss;
	p_tss->ds = ds;
	p_tss->fs = fs;
	p_tss->gs = gs;
	p_tss->ldt = ldt;
	p_tss->trap = trap;
	p_tss->iobase = iobase;
}