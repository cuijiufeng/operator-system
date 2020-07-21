; 2020年07月12日 11/47/14
; kernel
;==================================================================================================

%include	"kernel.inc"
;代码开始处
;==================================================================================================
;bss段(bss segment)通常是指用来存放程序中未初始化的全局变量的一块内存区域。
[section .bss]
;设置2KB的栈空间
StackSapce	resb	2*1024		;resb伪指令与db,dw,dd伪指令类似。声明2*1024个字节的数据
STACK_TOP:						;栈顶

;代码段(text segment)通常是指用来存放程序执行代码的一块内存区域。
[section .text]
_start:
	mov 	esp, STACK_TOP		;设置栈顶
	sgdt 	[ds:GDT_PTR]		;sgdt指令。保存gdtr寄存器中的值到gdt_ptr内存地址中去
	call	cinit
	lgdt	[ds:GDT_PTR]		;设置新的GDT
	lidt	[ds:IDT_PTR]		;设置新的IDT
	
H:
	hlt							;使程序停止运行，处理器进入暂停状态，不执行任何操作，不影响标志。
	jmp	H						;当复位（外语：RESET）线上有复位信号、CPU响应非屏蔽中断、
								;CPU响应可屏蔽中断3种情况之一时，CPU脱离暂停状态，执行HLT的下一条指令。
		