; 2020年07月21日 22/23/19
; 操作硬件的一些函数
;==================================================================================================

global	nop
global	outByte
global	inByte
global	portRead
global	portWrite
global	lgdtr
global	lidtr
global	ltr
global	lldtr
global	loadCR3
global	getDescLimit
global	disableIrq
global	enableIrq
global	disableInt
global	enableInt
INT_M_CTL		equ	20H						;主8259的偶地址端口20H
INT_M_CTLMASK	equ	21H						;主8259的奇地址端口21H
INT_S_CTL		equ	0A0H					;从8259的偶地址端口A0H
INT_S_CTLMASK	equ	0A1H					;从8259的偶地址端口A1H

[section .text]
;void	nop();
;-------------------------------------------------------------------------------------------
nop:
	nop
	ret

;void	disableInt();
;-------------------------------------------------------------------------------------------
disableInt:
	cli
	ret
;-------------------------------------------------------------------------------------------

;void	enableInt();
;-------------------------------------------------------------------------------------------
enableInt:
	sti
	ret
;-------------------------------------------------------------------------------------------

;void	lgdtr(u_32 gdt_ptr);
;-------------------------------------------------------------------------------------------
lgdtr:
	mov eax, [ss:esp+4]
	lgdt [ds:eax]
	;jmp 8:LABEL	;这个跳转指令会刷新段寄存器的值
	;LABEL:
	ret
;-------------------------------------------------------------------------------------------

;void	lidtr(t_32 idt_ptr);
;-------------------------------------------------------------------------------------------
lidtr:
	mov eax, [ss:esp+4]
	lidt [ds:eax]
	ret
;-------------------------------------------------------------------------------------------

;void	ltr(u_16 ltr);
;-------------------------------------------------------------------------------------------
ltr:
	mov ax, [ss:esp+4]
	ltr ax
	ret
;-------------------------------------------------------------------------------------------

;void	lldtr(u_16 ldt);
;-------------------------------------------------------------------------------------------
lldtr:
	mov ax, [ss:esp+4]
	lldt ax
	ret
;-------------------------------------------------------------------------------------------

;u_32	getDescLimit(u_16 segment);
;-------------------------------------------------------------------------------------------
getDescLimit:
	mov ax, [ss:esp+4]
	lsl eax, ax				;加载段界限指令
	ret
;-------------------------------------------------------------------------------------------

;void	loadCR3(u_32 addr);
;-------------------------------------------------------------------------------------------
loadCR3:
	mov eax, [ss:esp+4]
	mov cr3, eax
	ret
;-------------------------------------------------------------------------------------------

;void	portRead(t_port port, u_32 buf, u_32 nr);
;-------------------------------------------------------------------------------------------
portRead:
	mov edx, [ss:esp+4]
	mov edi, [ss:esp+8]
	mov ecx, [ss:esp+12]
	cld
	rep insw
	ret

;void	portWrite(t_port port, u_32 buf, u_32 nr);
;-------------------------------------------------------------------------------------------
portWrite:
	mov edx, [ss:esp+4]
	mov edi, [ss:esp+8]
	mov ecx, [ss:esp+12]
	cld
	rep outsw
	ret

;void	outByte(t_port port, t_8 value);
;-------------------------------------------------------------------------------------------
;往port端口中写入value值
outByte:
	mov edx,[ss:esp+4]		;port
	mov al,[ss:esp+4+4]		;value
	out dx,al
	nop						;一点延迟
	nop
	ret
;-------------------------------------------------------------------------------------------

;t_8	inByte(t_port port);
;-------------------------------------------------------------------------------------------
;从port端口中的读入一个值返回
inByte:
	mov edx,[ss:esp+4]		;port
	xor eax,eax
	in al,dx				;返回值放在al中来返回
	nop						;一点延迟
	nop
	ret
;-------------------------------------------------------------------------------------------

;void	disableIrq(int irq);	
;-------------------------------------------------------------------------------------------
disableIrq:
	mov	ecx, [esp + 4]		; irq
	pushf
	cli
	mov	ah, 1
	rol	ah, cl				; ah = (1 << (irq % 8))
	cmp	cl, 8
	jae	disable_8			;主片：irq<8	从片：irq >= 8
disable_0:					;主片
	in	al, INT_M_CTLMASK	;读入所有8259中断开关状态
	test	al, ah			;判断该irq号8259中断开关状态
	jnz	dis_already			;如果是关状态
	or	al, ah
	out	INT_M_CTLMASK, al	;关闭该irq号8259中断
	popf
	ret
disable_8:
	in	al, INT_S_CTLMASK
	test	al, ah
	jnz	dis_already			;如果是关状态
	or	al, ah
	out	INT_S_CTLMASK, al	;关闭该irq号8259中断
	popf
	ret
dis_already:				;如果是关状态，则不需要关闭了
	popf
	ret
;-------------------------------------------------------------------------------------------

;void 	enableIrq(int irq);
;-------------------------------------------------------------------------------------------
enableIrq:
    mov	ecx, [esp + 4]		; irq
    pushf
    cli
    mov	ah, ~1
    rol	ah, cl				; ah = ~(1 << (irq % 8))
    cmp	cl, 8
    jae	enable_8			;主片：irq<8	从片：irq >= 8
enable_0:
    in	al, INT_M_CTLMASK
    and	al, ah
    out	INT_M_CTLMASK, al	;打开该irq号8259中断
    popf
    ret
enable_8:
    in	al, INT_S_CTLMASK
    and	al, ah
    out	INT_S_CTLMASK, al	;打开该irq号8259中断
    popf
    ret
;-------------------------------------------------------------------------------------------