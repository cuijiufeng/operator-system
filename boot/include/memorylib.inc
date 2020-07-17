; 2020��07��12�� 09/33/34
; ��ʵģʽ�»�ȡ�ڴ���Ϣ��һЩ��������
;========================================================================================================

;����ڴ���Ϣ��int 15H�ж�
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

;�����ڴ��С
;-------------------------------------------------------------------------------------
getMemerySize:
	mov ax,cs
	mov es,ax
	mov di,_CheckMemeryBuffer
	xor cx,cx
	mov cl,[ds:_CheckMemeryNumber]
	.loopSize:
		cmp dword [es:di+16],1				;type ������Ͳ�Ϊ1�����ʾΪ�����õ��ڴ�
		jne .continue
		mov eax,[es:di]						;addrLow
		add eax,[es:di+8]					;lengthLow
		cmp eax,[ds:_MemerySize+9]			;addrLow+lengthLow>memerySize
		jb .continue
		mov [ds:_MemerySize+9],eax			;���õ�����ڴ�
		.continue:
		add di,20
	loop .loopSize
	ret

;��ӡ�ڴ���Ϣ
;-------------------------------------------------------------------------------------
disMemInfo:
	mov ax,0B800H
	mov es,ax
	mov di,320H
	mov bx,_CheckMemeryBuffer			;��������
	mov cx,[ds:_CheckMemeryNumber]		;һ���ж�����
	.1:
		push cx
		mov cx,5							;��ӡһ���е�5����Ա
		.2:
			mov eax,[ds:bx]						;ȡ��һ����Ա
			call displayFourByteData			;��ӡһ����Ա
			add bx,4							;��һ����Ա
		loop .2
		pop cx
		add di,60							;����һ�д�ӡ
	loop .1
	ret

;��ӡ�ڴ�Ĵ�С
;-----------------------------------------------------------------------------------------------------	
disMemSize:
	mov ax,0B800H
	mov es,ax
	mov di,780H				;��ӡ����12��
	mov bx,_MemerySize
	mov cx,9				;��ӡ'Mem Size:'�ַ���һ����9��

	mov ah,0FH
	.displayMemSizeLoop:
		mov al,[ds:bx]
		mov [es:di],ax
		inc bx
		add di,2
	loop .displayMemSizeLoop	;��ӡ'Mem Size:'�ַ�
	mov eax,[ds:bx]			;��ȡ_MemerySize+9��ַ������
	call displayFourByteData
	ret