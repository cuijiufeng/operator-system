; 2020年07月12日 11/47/14
; kernel
;==================================================================================================

%include	"krn.inc"
;代码开始处
;==================================================================================================
[section .text]
_start:
	mov 	esp, KERNEL_STACK+PAGE_SIZE	;设置栈顶
	sgdt 	[ds:GDT_PTR]		;sgdt指令。保存gdtr寄存器中的值到gdt_ptr内存地址中去
	call	cinit
MOVE_TO_USER:
	sti
	mov eax, esp
	push SELECTOR_KERNEL_DS|SA_TIL|SA_RPL3	;push ss
	push eax								;push esp
	pushf									;push eflags
	push SELECTOR_KERNEL_CS|SA_TIL|SA_RPL3	;push cs
	push _1									;push eip
	iret									;任务门特权级改变
_1:
	mov ax, SELECTOR_KERNEL_DS|SA_TIL|SA_RPL3
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	jmp $
;==================================================================================================
;==================================================================================================================
;restart:
;	mov	esp, [P_PROCESS_READY]			; esp指向进程表,离开内核栈
;	lldt [esp + P_LDT_SEL] 				;每个进程都有自己的LDT，当进程切换的时候需要重新加载ldtr
;		
;	mov eax,esp
;	add eax,P_STACKTOP
;	mov	dword [TSS + TSS3_S_SP0], eax	;TSS保存s_process中s_stackframe的最高地址
;		
;restart_reenter:
;	dec dword [K_RE_ENTRY]				;结束中断k_reenter就减一
;		
;	pop gs						; ┓
;	pop fs						; ┃  
;	pop es						; ┣ 恢复ring1时寄存器的值
;	pop ds						; ┃  
;	popad						; ┛	;依次从栈中弹出寄存器的值。顺序为EDI,ESI,EBP,ESP,EBX,EDX,ECX,EAX。恢复寄存器的值。
;	add	esp, 4							;跳过retaddr成员
;		
;	iretd								;从栈中弹出4个字节到EIP，再弹出4个字节中的低2个字节到CS，再弹出4个字节到EFLAGS
;											;ring0--->ring1
;											;p_process_ready->regs.eflags=0x1202;	//IF=1, IOPL=1	所以执行完iretd时中断被打开
;==================================================================================================================

;中断发生时的寄存器保存工作
;==================================================================================================================
save:
;	pushad						; ┓
;	push ds						; ┃   
;	push es						; ┣ 保存ring1时寄存器的值
;	push fs						; ┃      
;	push gs						; ┛ 
;	mov dx,ss					; ┓
;	mov ds,dx					; ┣ 使用ring0时的寄存器的值
;	mov es,dx					; ┛
;		
;	mov esi,esp					;esi=进程表的起始地址
;		
;	inc	dword [K_RE_ENTRY]		;发生中断k_reenter就加一
;	cmp	dword [K_RE_ENTRY], 0
;	jne .1						;中断重入时跳入.1，通常情况下顺序执行
;		
;	mov esp,STACK_TOP			;切换到内核栈。因为此时我对栈的任何一个操作都会破坏进程表
;		
;	push restart
;	jmp [esi+RETADR-P_STACKBASE]
;.1:								;中断重入.已经在内核栈，不需要再切换
;	push restart_reenter
;	jmp [esi+RETADR-P_STACKBASE];返回到call save后边继续执行
;								;调用save前的esp是指向进程表的，而调用save后esp是指向内核栈的。esp的指向不同，自然就不能ret
;==================================================================================================================

;==================================================================================================================
;带参数宏		从片
%macro hwint_master 1
	call save
		
	in al,INT_M_CTLMASK			; ┓ 
	or al,(1<<%1)				; ┣ 不允许再发生时钟中断
	out INT_M_CTLMASK,al		; ┛ 
		
	mov al,	EOI					;中断结束控制字
	out	INT_M_CTL, al			;告诉8259中断结束。这样就可以不停的发生中断
		
	sti							;为了在执行中断程序的时候还可以响应中断，所以要人为打开中断。CPU在响应中断的时候会自动关闭中断

	push %1						;irq号
	call [(IRQ_TABLE + 4 * %1)]	;调用中断处理子程序
	add esp,4					;由调用者清理堆栈，清理掉调用函数前压入的参数irq
		
	cli							;到这里就要把中断关闭掉，后边就要离开内核栈，准备切换进程。如果发生中断，就会破坏掉进程表
		
	in al,INT_M_CTLMASK			; ┓ 
	and al,~(1<<%1)				; ┣ 允许再发生时钟中断
	out INT_M_CTLMASK,al		; ┛ 
		
	ret
%endmacro
;------------------------------------------------------------------------------------------------------------------
ALIGN 16						;此代码以16字节对齐
hwint00:						;中断发生时会切换到内核态，即由ring1-->ring0
	hwint_master 0		;irq号
		
ALIGN 16
hwint01:
	hwint_master 1		;irq号
		
ALIGN 16
hwint02:
	hwint_master 2
		
ALIGN 16
hwint03:
	hwint_master 3
		
ALIGN 16
hwint04:
	hwint_master 4
		
ALIGN 16
hwint05:
	hwint_master 5
		
ALIGN 16
hwint06:
	hwint_master 6
		
ALIGN 16
hwint07:
	hwint_master 7
;==================================================================================================================

;==================================================================================================================
;带参数宏		从片
%macro hwint_slave 1
	call save
		
	in al,INT_S_CTLMASK			; ┓ 
	or al,(1<<(%1-8))			; ┣ 不允许再发生时钟中断
	out INT_S_CTLMASK,al		; ┛ 
		
	mov al,	EOI					;中断结束控制字
	out	INT_M_CTL, al			;告诉8259中断结束。这样就可以不停的发生中断
	mov al,	EOI					;中断结束控制字
	out	INT_S_CTL, al			;告诉8259中断结束。这样就可以不停的发生中断
		
	sti							;为了在执行中断程序的时候还可以响应中断，所以要人为打开中断。CPU在响应中断的时候会自动关闭中断

	push %1						;irq号
	call [(IRQ_TABLE + 4 * %1)]	;调用中断处理子程序
	add esp,4					;由调用者清理堆栈，清理掉调用函数前压入的参数irq
		
	cli							;到这里就要把中断关闭掉，后边就要离开内核栈，准备切换进程。如果发生中断，就会破坏掉进程表
		
	in al,INT_S_CTLMASK			; ┓ 
	and al,~(1<<(%1-8))			; ┣ 允许再发生时钟中断
	out INT_S_CTLMASK,al		; ┛ 
		
	ret
%endmacro
;------------------------------------------------------------------------------------------------------------------
ALIGN 16
hwint08:
	hwint_slave 8
		
ALIGN 16
hwint09:
	hwint_slave 9
		
ALIGN 16
hwint10:
	hwint_slave 10
		
ALIGN 16
hwint11:
	hwint_slave 11
		
ALIGN 16
hwint12:
	hwint_slave 12
		
ALIGN 16
hwint13:
	hwint_slave 13
		
ALIGN 16
hwint14:
	hwint_slave 14
		
ALIGN 16
hwint15:
	hwint_slave 15
;==================================================================================================================