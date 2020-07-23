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

	xor eax, eax
	mov ax, SELECTOR_TSS
	ltr ax						;设置tr

	jmp SELECTOR_KERNEL_CS:CMAIN	;这个跳转指令强制使用刚刚初始化的结构
CMAIN:

H:
	hlt							;使程序停止运行，处理器进入暂停状态，不执行任何操作，不影响标志。
	jmp	H						;当复位（外语：RESET）线上有复位信号、CPU响应非屏蔽中断、
								;CPU响应可屏蔽中断3种情况之一时，CPU脱离暂停状态，执行HLT的下一条指令。
;==================================================================================================
;==================================================================================================================
restart:
	mov	esp, [P_PROCESS_READY]			; esp指向进程表,离开内核栈
	lldt [esp + P_LDT_SEL] 				;每个进程都有自己的LDT，当进程切换的时候需要重新加载ldtr
		
	mov eax,esp
	add eax,P_STACKTOP
	mov	dword [TSS + TSS3_S_SP0], eax	;TSS保存s_process中s_stackframe的最高地址
		
restart_reenter:
	dec dword [K_RE_ENTRY]				;结束中断k_reenter就减一
		
	pop gs						; ┓
	pop fs						; ┃  
	pop es						; ┣ 恢复ring1时寄存器的值
	pop ds						; ┃  
	popad						; ┛	;依次从栈中弹出寄存器的值。顺序为EDI,ESI,EBP,ESP,EBX,EDX,ECX,EAX。恢复寄存器的值。
	add	esp, 4							;跳过retaddr成员
		
	iretd								;从栈中弹出4个字节到EIP，再弹出4个字节中的低2个字节到CS，再弹出4个字节到EFLAGS
											;ring0--->ring1
											;p_process_ready->regs.eflags=0x1202;	//IF=1, IOPL=1	所以执行完iretd时中断被打开
;==================================================================================================================