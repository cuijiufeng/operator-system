; 2020年07月21日 22/23/19
; 操作硬件的一些函数
;==================================================================================================

global	outByte
global	inByte
global	disableInt
global	enableInt
global	disableIrq
global	enableIrq
INT_M_CTL		equ	20H						;主8259的偶地址端口20H
INT_M_CTLMASK	equ	21H						;主8259的奇地址端口21H
INT_S_CTL		equ	0A0H					;从8259的偶地址端口A0H
INT_S_CTLMASK	equ	0A1H					;从8259的偶地址端口A1H

[section .text]
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
	mov	eax, 1				;关闭成功，返回值1
	ret
disable_8:
	in	al, INT_S_CTLMASK
	test	al, ah
	jnz	dis_already			;如果是关状态
	or	al, ah
	out	INT_S_CTLMASK, al	;关闭该irq号8259中断
	popf
	mov	eax, 1				;关闭成功，返回值1
	ret
dis_already:				;如果是关状态，则不需要关闭了
	popf
	xor	eax, eax			;关闭失败，返回值0
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