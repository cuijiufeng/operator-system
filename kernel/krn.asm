; 2020年07月12日 11/47/14
; kernel
;==================================================================================================

;==================================================================================================
global	_start						;导出_start
global	SWITCH_TO
;导入(声明)
extern	cinit
extern	main
extern	KERNEL_STACK				;内核栈
extern	INIT_TASKS					;初始任务的栈
extern	TASKS
extern	CURRENT
extern	GDT_PTR						;gdtr寄存器的值
extern	IDT_PTR						;idtr寄存器的值
PAGE_SIZE		equ	4096			;页大小
SA_RPL0			equ	0
SA_RPL1			equ	1
SA_RPL2			equ	2
SA_RPL3			equ	3
SA_TIG			equ	0				;描述符GDT在中
SA_TIL			equ	4				;描述符LDT在中
;选择子
SELECTOR_DUMMY			equ	0		;┓
SELECTOR_KERNEL_CS		equ	08H		;┣ LOADER 里面已经确定了的.
SELECTOR_KERNEL_DS		equ	10H		;┛
SELECTOR_FIRST_TASK_TSS	equ	18H
SELECTOR_FIRST_TASK_LDT	equ	20H
;==================================================================================================

;代码开始处
;==================================================================================================
[section .text]
_start:
	mov 	esp, KERNEL_STACK+PAGE_SIZE		;设置栈顶
	sgdt 	[ds:GDT_PTR]					;sgdt指令。保存gdtr寄存器中的值到gdt_ptr内存地址中去
	call	cinit
MOVE_TO_USER:
	mov eax, 3002H							;IOPL=3
	push eax
	popfd									;修改eflags的值
	push SELECTOR_KERNEL_DS|SA_TIL|SA_RPL3	;push ss
	push INIT_TASKS	+ PAGE_SIZE				;push esp
	pushf									;push eflags
	push SELECTOR_KERNEL_CS|SA_TIL|SA_RPL3	;push cs
	push MAIN								;push eip
	iret									;任务门特权级改变
SWITCH_TO:
	mov eax, [esp+4]						;进程槽位号
	shl eax, 2								;进程相对TASKS的偏移量
	add eax, TASKS							;进程槽位的地址
	mov eax, [eax]							;进程的地址
	mov edx, CURRENT
	cmp dword [edx], eax					;如果是当前正在执行的进程则返回
	je RET
	xchg [edx], eax							;设置CURRENT新值
	mov eax, [esp+4]
	shl eax, 4								;任务tss选择子
	add eax, SELECTOR_FIRST_TASK_TSS
	push eax								;seg
	push 0									;offset
	jmp far [esp]							;任务门
	add esp, 8
	RET:
	ret
MAIN:
	mov ax, SELECTOR_KERNEL_DS|SA_TIL|SA_RPL3
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	sti										;开中断
	call	main
;==================================================================================================