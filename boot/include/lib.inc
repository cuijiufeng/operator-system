;2020年07月05日 16/31/26
;抽取的函数
;==================================================================================================


;清空屏幕
;==================================================================================================
clearScreen:
	mov ax,0B800H
	mov es,ax
	mov di,0
	mov ax,0700H
	mov cx,2000
	.clear:
		mov [es:di],ax
		add di,2
	loop .clear
	ret

;打印字符串
;==================================================================================================
displayString:
	push ax
	push es
	mov ax, 0B800H
	mov es, ax
	mov ah, 07H
	.display:
		mov al, [ds:si]
		cmp al, 0
		je .over
		mov [es:di], ax
		inc si
		add di, 2
	jmp .display
	.over:
	pop es
	pop ax
	ret

;打印eax中的值
;==================================================================================================
displayFourByteData:
	push bp
	push eax
	mov bp,sp
		
	mov eax,[ss:bp]
	shr eax,24				;打印最高一字节
	call displayAX
		
	mov eax,[ss:bp]
	shr eax,16				;打印第二个字节
	call displayAX
		
	mov eax,[ss:bp]
	shr eax,8				;打印第三个字节
	call displayAX
		
	mov eax,[ss:bp]
	call displayAX			;打印低一个字节
		
	mov al,'H'
	mov ah,0fH
	mov [es:di],ax			;每个成员后边打印一个H
	add di,4
		
	pop eax
	pop bp
	ret

;打印ax中的值		
;==================================================================================================
displayAX:
	push dx
	push cx
		
	mov cx,2			;一个字节两个字符
	mov ah,0FH			;设置字的颜色
	mov dl,al			;暂存al中的值
	shr al,4			;准备打印高四位
	.loop:
		and al,1111B
		cmp al,9
		ja .char			;如果是a~F字符
	.int:					;如果是数字
		add al,'0'			;变成数字字符
		mov [es:di],ax		;打印
		jmp .over
	.char:
		add al,55			;变成字符
		mov [es:di],ax
	.over:
		add di,2			;下一个打印位置
		mov al,dl			;准备打印低四位
	loop .loop
		
	pop cx
	pop dx
	ret