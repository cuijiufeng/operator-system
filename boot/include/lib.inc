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