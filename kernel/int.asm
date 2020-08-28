; 2020年07月30日 14:11:28
; 所有的中断处理程序入口
;==================================================================================================

extern IRQ_TABLE
extern SYS_CALL_TABLE
extern syserrHandler
global sysCall
global divideError,singleStepException,nmi,breakpointException,overflow,boundsCheck,invalOpcode,coprNotAvailable
global doubleFault,coprSegOverrun,invalTss,segmentNotPresent,stackException,generalProtection,pageFault,coprError
global hwint00,hwint01,hwint02,hwint03,hwint04,hwint05,hwint06,hwint07,hwint08,hwint09,hwint10,hwint11,hwint12,hwint13,hwint14,hwint15

;==================================================================================================================
EOI				equ	20H						;ocw2,中断结束控制字
INT_M_CTL		equ	20H						;主8259的偶地址端口20H
INT_M_CTLMASK	equ	21H						;主8259的奇地址端口21H
INT_S_CTL		equ	0A0H					;从8259的偶地址端口A0H
INT_S_CTLMASK	equ	0A1H					;从8259的偶地址端口A1H
;==================================================================================================================

;切换段寄存器值
;==================================================================================================================
%macro use_ring0 0 
	mov dx,ss
	mov ds,dx					;┓
	mov es,dx					;┃ 
	mov fs,dx					;┣ 使用ring0时的段寄存器的值
	mov gs,dx					;┛
%endmacro
;==================================================================================================================

;中断发生时的寄存器保存工作
;==================================================================================================================
%macro save_reg 0
	push ds						;┓   
	push es						;┃
	push fs						;┃
	push gs						;┣ 保存ring3时寄存器的值
	pushf						;┃
	pushad						;┛
%endmacro
;==================================================================================================================

;中断返回时恢复寄存器的值
;==================================================================================================================
%macro load_reg 0  
	popad						;┓  
	popf						;┃ 
	pop gs						;┣ 恢复ring3时寄存器的值
	pop fs						;┃   
	pop es						;┃ 
	pop ds						;┛ 
%endmacro
;==================================================================================================================

;中断、异常-------异常
;==================================================================================================================
divideError:
	use_ring0
	push 0FFFFFFFFH				;压入中断出错码
	push 0H						;压入中断向量号
	call syserrHandler
	add esp,8
	jmp $
singleStepException:
	use_ring0
	push 0FFFFFFFFH
	push 1H
	call syserrHandler
	add esp,8
	jmp $
nmi:
	use_ring0
	push 0FFFFFFFFH
	push 2H
	call syserrHandler
	add esp,8
	jmp $
breakpointException:
	use_ring0
	push 0FFFFFFFFH
	push 3H
	call syserrHandler
	add esp,8
	jmp $
overflow:
	use_ring0
	push 0FFFFFFFFH
	push 4H
	call syserrHandler
	add esp,8
	jmp $
boundsCheck:
	use_ring0
	push 0FFFFFFFFH
	push 5H
	call syserrHandler
	add esp,8
	jmp $
invalOpcode:
	use_ring0
	push 0FFFFFFFFH
	push 6H
	call syserrHandler
	add esp,8
	jmp $
coprNotAvailable:
	use_ring0
	push 0FFFFFFFFH
	push 7H
	call syserrHandler
	add esp,8
	jmp $
doubleFault:
	use_ring0
	push 8H
	call syserrHandler
	add esp,4
	jmp $
coprSegOverrun:
	use_ring0
	push 0FFFFFFFFH
	push 9H
	call syserrHandler
	add esp,8
	jmp $
invalTss:
	use_ring0
	push 0AH
	call syserrHandler
	add esp,4
	jmp $
segmentNotPresent:
	use_ring0
	push 0BH
	call syserrHandler
	add esp,4
	jmp $
stackException:
	use_ring0
	push 0CH
	call syserrHandler
	add esp,4
	jmp $
generalProtection:
	use_ring0
	push 0DH
	call syserrHandler
	add esp,4
	jmp $
pageFault:
	use_ring0
	push 0EH
	call syserrHandler
	add esp,4
	jmp $
coprError:
	use_ring0
	push 0FFFFFFFFH
	push 0FH
	call syserrHandler
	add esp,8
	jmp $
;==================================================================================================================

;8259中断
;==================================================================================================================
;带参数宏		 主片
%macro hwint_master 1
	save_reg
	use_ring0
		
	in al,INT_M_CTLMASK			; ┓ 
	or al,(1<<%1)				; ┣ 不允许再发生时钟中断
	out INT_M_CTLMASK,al		; ┛ 
	mov al,	EOI					;中断结束控制字
	out	INT_M_CTL, al			;告诉8259中断结束。这样就可以不停的发生中断
		
	sti							;为了在执行中断程序的时候还可以响应中断，所以要人为打开中断。CPU在响应中断的时候会自动关闭中断
	push %1						;irq号
	call [(IRQ_TABLE + 4 * %1)]	;调用中断处理子程序
	add esp,4					;由调用者清理堆栈，清理掉调用函数前压入的参数irq
		
	load_reg
	
	in al,INT_M_CTLMASK			; ┓ 
	and al,~(1<<%1)				; ┣ 允许再发生时钟中断
	out INT_M_CTLMASK,al		; ┛ 
	iret
%endmacro
;------------------------------------------------------------------------------------------------------------------
ALIGN 16						;此代码以16字节对齐
hwint00:						;中断发生时会切换到内核态，即由ring3-->ring0
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
	save_reg
	use_ring0
		
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
		
	load_reg
	
	in al,INT_S_CTLMASK			; ┓ 
	and al,~(1<<(%1-8))			; ┣ 允许再发生时钟中断
	out INT_S_CTLMASK,al		; ┛ 
	iret
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

;系统调用中断
;==================================================================================================================
sysCall:
	save_reg
	sti							;开中断
	push ebx					;系统调用参数可以有三个
	push ecx
	push edx
	use_ring0
	call [SYS_CALL_TABLE+eax*4]
	add	esp, 4*3				;清理栈系统调用参数
	load_reg
	iret
;==================================================================================================================