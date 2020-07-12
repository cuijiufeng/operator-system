; 2020年07月12日 09/33/34
; 在实模式下获取内存信息的一些函数调用
;========================================================================================================

;获得内存信息，int 15H中断
;-------------------------------------------------------------------------------------
getMemeryInfo:
	mov ax,cs
	mov es,ax
	mov di,_CheckMemeryBuffer
	mov ebx,0
	.loop:
		mov eax,0E820H
		mov ecx,20
		mov edx,534D4150H
		int 15H
		jc .fail
		inc byte [ds:_CheckMemeryNumber]
		add di,20
		cmp ebx,0
		jz .over
	loop .loop
	.fail:
		mov byte [ds:_CheckMemeryNumber],0
	.over:
	ret

;计算内存大小
;-------------------------------------------------------------------------------------
getMemerySize:
	mov ax,cs
	mov es,ax
	mov di,_CheckMemeryBuffer
	xor cx,cx
	mov cl,[ds:_CheckMemeryNumber]
	.loopSize:
		cmp dword [es:di+16],1				;type 如果类型不为1，则表示为不可用的内存
		jne .continue
		mov eax,[es:di]						;addrLow
		add eax,[es:di+8]					;lengthLow
		cmp eax,[ds:_MemerySize+9]			;addrLow+lengthLow>memerySize
		jb .continue
		mov [ds:_MemerySize+9],eax			;可用的最大内存
		.continue:
		add di,20
	loop .loopSize
	ret

;打印内存信息
;-------------------------------------------------------------------------------------
disMemInfo:
	mov ax,0B800H
	mov es,ax
	mov di,320H
	mov bx,_CheckMemeryBuffer			;数据在这
	mov cx,[ds:_CheckMemeryNumber]		;一共有多少行
	.1:
		push cx
		mov cx,5							;打印一行中的5个成员
		.2:
			mov eax,[ds:bx]						;取到一个成员
			call displayFourByteData			;打印一个成员
			add bx,4							;下一个成员
		loop .2
		pop cx
		add di,60							;到下一行打印
	loop .1
	ret

;打印内存的大小
;-----------------------------------------------------------------------------------------------------	
disMemSize:
	mov ax,0B800H
	mov es,ax
	mov di,780H				;打印到第12行
	mov bx,_MemerySize
	mov cx,9				;打印'Mem Size:'字符，一共有9个

	mov ah,0FH
	.displayMemSizeLoop:
		mov al,[ds:bx]
		mov [es:di],ax
		inc bx
		add di,2
	loop .displayMemSizeLoop	;打印'Mem Size:'字符
	mov eax,[ds:bx]			;获取_MemerySize+9地址的数据
	call displayFourByteData
	ret