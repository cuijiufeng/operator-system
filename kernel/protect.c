/**
 * 2020��07��22�� 19/37/55
 * ����ģʽ��ز�������ʼ��
 */
#include	"type.h"
#include	"protect.h"
#include	"interrupt.h"
#include	"global.h"

PUBLIC void initProtect()
{
	//��ʼ���쳣�ж�
	initIdtDesc(INT_VECTOR_DIVIDE,		DA_386IGate, divideError,			PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_DEBUG,		DA_386IGate, singleStepException,	PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_NMI,			DA_386IGate, nmi,					PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_BREAKPOINT,	DA_386IGate, breakpointException,	PRIVILEGE_USER);
	initIdtDesc(INT_VECTOR_OVERFLOW,	DA_386IGate, overflow,				PRIVILEGE_USER);
	initIdtDesc(INT_VECTOR_BOUNDS,		DA_386IGate, boundsCheck,			PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_INVAL_OP,	DA_386IGate, invalOpcode,			PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_COPROC_NOT,	DA_386IGate, coprNotAvailable,		PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_DOUBLE_FAULT,DA_386IGate, doubleFault,			PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_COPROC_SEG,	DA_386IGate, coprSegOverrun,		PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_INVAL_TSS,	DA_386IGate, invalTss,				PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_SEG_NOT,		DA_386IGate, segmentNotPresent,		PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_STACK_FAULT,	DA_386IGate, stackException,		PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_PROTECTION,	DA_386IGate, generalProtection,		PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_PAGE_FAULT,	DA_386IGate, pageFault,				PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_COPROC_ERR,	DA_386IGate, coprError,				PRIVILEGE_KERNEL);

	//��ʼ��8259�жϵ�IDT
	initIdtDesc(INT_VECTOR_IRQ0 + 0, DA_386IGate, hwint00, PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_IRQ0 + 1, DA_386IGate, hwint01, PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_IRQ0 + 2, DA_386IGate, hwint02, PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_IRQ0 + 3, DA_386IGate, hwint03, PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_IRQ0 + 4, DA_386IGate, hwint04, PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_IRQ0 + 5, DA_386IGate, hwint05, PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_IRQ0 + 6, DA_386IGate, hwint06, PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_IRQ0 + 7, DA_386IGate, hwint07, PRIVILEGE_KERNEL);

	initIdtDesc(INT_VECTOR_IRQ8 + 0, DA_386IGate, hwint08, PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_IRQ8 + 1, DA_386IGate, hwint09, PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_IRQ8 + 2, DA_386IGate, hwint10, PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_IRQ8 + 3, DA_386IGate, hwint11, PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_IRQ8 + 4, DA_386IGate, hwint12, PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_IRQ8 + 5, DA_386IGate, hwint13, PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_IRQ8 + 6, DA_386IGate, hwint14, PRIVILEGE_KERNEL);
	initIdtDesc(INT_VECTOR_IRQ8 + 7, DA_386IGate, hwint15, PRIVILEGE_KERNEL);

	//��� GDT �� TSS ���������
	initDescriptor(
		&GDT[INDEX_TSS],
		vir2phys(seg2phys(SELECTOR_KERNEL_DS), &TSS),
		sizeof(TSS) - 1,
		DA_386TSS);
}


//��ʼ��һ��IDT��			  ������		����			�жϳ���ƫ��				��Ȩ��
PUBLIC	void	initIdtDesc(t_8 vector, t_8 desc_type, T_PF_INT_HANDLER handler, t_8 privilege)
{
	GATE* p_gate = &IDT[vector];				//��Ӧ������λ�õ���������

	t_32 base = (t_32)handler;
	p_gate->offset_low = base & 0xFFFF;
	p_gate->selector = SELECTOR_KERNEL_CS;
	p_gate->attr = desc_type | (privilege << 5);
	p_gate->offset_high = (base >> 16) & 0xFFFF;
}

//��ʼ��GDT��һ����������
PUBLIC	void	initDescriptor(DESCRIPTOR* p_desc, t_32 base, t_32 limit, t_16 attribute)
{
	p_desc->limit_low = limit & 0xFFFF;							//�ν��޵�2���ֽ�
	p_desc->base_low = base & 0xFFFF;							//�λ�ַ��2���ֽ�
	p_desc->base_mid = (base >> 16) & 0xFF;						//�λ�ַmid1���ֽ�
	p_desc->attr1 = attribute & 0xFF;							//���Եĵ�1���ֽ�
	p_desc->limit_high_attr2 = (limit >> 16) & 0x0F | (attribute >> 8) & 0xF0;	//����+���ԣ���1���ֽ�
	p_desc->base_high = (base >> 24) & 0xFF;						//�λ�ַ��1���ֽ�
}

//�ɶ�������Ե�ַ
PUBLIC t_32 seg2phys(t_16 seg)
{
	DESCRIPTOR* p_desc = &GDT[seg >> 3];		//����(�λ�ַ/ѡ����)�õ���Ӧ����λ�õ�GDT������
	return (p_desc->base_high << 24) | (p_desc->base_mid << 16) | (p_desc->base_low);
}