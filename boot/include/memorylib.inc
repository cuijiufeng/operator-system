; 2020年07月12日 09/33/34
; 在实模式下获取内存信息的一些函数调用
;========================================================================================================

;获得内存信息，int 15H中断
;-------------------------------------------------------------------------------------
getMemeryInfo:
	push es
	mov ax, CHECK_MEMORY_BASE
	mov es, ax
	mov di, MEMORY_BUFFER_OFFSET
	mov ebx,0
	.loop:
		mov eax,0E820H
		mov ecx,20
		mov edx,534D4150H
		int 15H
		jc .fail
		inc byte [es:MEMORY_NUMBER_OFFSET]
		add di,20
		cmp ebx,0
		jz .over
	loop .loop
	.fail:
		mov byte [es:MEMORY_NUMBER_OFFSET],0
	.over:
	pop es
	ret

;计算内存大小
;-------------------------------------------------------------------------------------
getMemerySize:
	push ds
	mov ax, CHECK_MEMORY_BASE
	mov ds, ax
	mov di,MEMORY_BUFFER_OFFSET
	xor cx,cx
	mov cl,[ds:MEMORY_NUMBER_OFFSET]
	.loopSize:
		cmp dword [ds:di+16],1				;type 如果类型不为1，则表示为不可用的内存
		jne .continue
		mov eax,[ds:di]						;addrLow
		add eax,[ds:di+8]					;lengthLow
		cmp eax,[ds:MEMORY_SIZE_OFFSET]		;addrLow+lengthLow>memerySize
		jb .continue
		mov [ds:MEMORY_SIZE_OFFSET],eax		;可用的最大内存
		.continue:
		add di,20
	loop .loopSize
	pop ds
	ret

;打印内存信息
;-------------------------------------------------------------------------------------
disMemInfo:
	push ds
	mov ax, CHECK_MEMORY_BASE
	mov ds, ax
	mov di,3c0H
	mov bx,MEMORY_BUFFER_OFFSET			;数据在这
	mov cx,[ds:MEMORY_NUMBER_OFFSET]	;一共有多少行
	.1:
		push cx
		mov cx,5						;打印一行中的5个成员
		.2:
			mov eax,[ds:bx]				;取到一个成员
			call displayFourByteData	;打印一个成员
			add bx,4					;下一个成员
		loop .2
		pop cx
		add di,60						;到下一行打印
	loop .1
	pop ds
	ret

;打印内存的大小
;-----------------------------------------------------------------------------------------------------	
disMemSize:
	push ds
	
	mov si,_MemerySizeStr
	mov di,780H
	call displayString				;打印'Mem Size:'字符
	
	mov ax, CHECK_MEMORY_BASE
	mov ds, ax
	mov eax, [ds:MEMORY_SIZE_OFFSET]
	call displayFourByteData
	
	pop ds
	ret