; 2020年07月21日 22/23/19
; 操作硬件的一些函数
;==================================================================================================

global	outByte
global	inByte
global	disableInt
global	enableInt
global	disableIrq
global	enableIrq
global	cleanScreen
global	displayStr
INT_M_CTL		equ	20H						;主8259的偶地址端口20H
INT_M_CTLMASK	equ	21H						;主8259的奇地址端口21H
INT_S_CTL		equ	0A0H					;从8259的偶地址端口A0H
INT_S_CTLMASK	equ	0A1H					;从8259的偶地址端口A1H

[section .text]
;void cleanScreen();						清空屏幕
;-------------------------------------------------------------------------------------------
cleanScreen:
	push di
	push ax
	push cx
	mov di,0
	mov ax,0700H
	mov cx,2000
	.clear:
		mov [gs:di],ax
		add di,2
	loop .clear
	pop cx
	pop ax
	pop di
	ret
;-------------------------------------------------------------------------------------------

;void displayStr(char* str, int pos);		;打印字符串
;-------------------------------------------------------------------------------------------
displayStr:
	push ebp
	mov ebp,esp
		
	mov esi,[ss:ebp+8]
	mov edi,[ss:ebp+12]
	mov ah,0Fh
	.1:
		mov al,[ds:esi]			;取一个字符
		inc esi					;下一个字符
		cmp al,0				;c语言以0作为字符串的结束标志，如果是0的话就说明字符串结束了(不是字符0,是ascii码值为0)
		je	.2
		cmp al,0Ah				;是回车吗?回车换行
		jnz .3
		push eax				;ah中保存着字符的颜色
		mov eax,edi
		mov bl,160				;一行一共80个字符
		div bl					;光标现在处于第几行
		and eax,0FFh			;只留下除法的商
		inc eax					;光标移到下一行
		mov bl,160
		mul bl
		mov edi,eax
		pop eax
		jmp .1
	.3:							;如果是普通字符就打印输出
		mov [gs:edi],ax
		add edi,2
		jmp .1
	.2:							;如果到字符串的结尾
	pop ebp
	ret
;-------------------------------------------------------------------------------------------

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