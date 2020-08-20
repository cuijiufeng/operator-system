; 2020年07月17日 10:02:10
; kernel用到的常量数据及导入导出标号
;==================================================================================================

;导出
global	_start						;导出_start

global hwint00
global hwint01
global hwint02
global hwint03
global hwint04
global hwint05
global hwint06
global hwint07
global hwint08
global hwint09
global hwint10
global hwint11
global hwint12
global hwint13
global hwint14
global hwint15

;导入(声明)
extern	KERNEL_STACK
extern	GDT_PTR						;gdtr寄存器的值
extern	IDT_PTR
extern	IRQ_TABLE
extern	cinit
;==================================================================================================================
PAGE_SIZE		equ	4096
;选择子
SELECTOR_DUMMY			equ	0		;┓
SELECTOR_KERNEL_CS		equ	08H		;┣ LOADER 里面已经确定了的.
SELECTOR_KERNEL_DS		equ	10H		;┛
SELECTOR_FIRST_TASK_TSS	equ	8H
SELECTOR_FIRST_TASK_LDT	equ	20H
SA_RPL0		equ	0
SA_RPL1		equ	1
SA_RPL2		equ	2
SA_RPL3		equ	3
SA_TIG		equ	0			;描述符GDT在中
SA_TIL		equ	4			;描述符LDT在中
;==================================================================================================================
;==================================================================================================================
EOI				equ	20H						;ocw2,中断结束控制字
INT_M_CTL		equ	20H						;主8259的偶地址端口20H
INT_M_CTLMASK	equ	21H						;主8259的奇地址端口21H
INT_S_CTL		equ	0A0H					;从8259的偶地址端口A0H
INT_S_CTLMASK	equ	0A1H					;从8259的偶地址端口A1H
;==================================================================================================================