; 2020年07月19日 09/04/36
; 与内存操作有关的函数
;==================================================================================================

[section .text]
global	memcpy
global	memset

;用于内存拷贝的函数
;-----------------------------------------------------------------------------------------------------------------------
memcpy:
	push	ebp
	mov		ebp, esp
	push	esi
	push	edi
	push	ecx							;暂存寄存器中的值
	
	mov	edi, [ebp + 8]					;Destination
	mov	esi, [ebp + 12]					;Source
	mov	ecx, [ebp + 16]					;Counter
	cld
	rep movsb
	mov	eax, [ebp + 8]					;返回值,返回目标地址的偏移值Destination
	
	pop	ecx
	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp								;恢复寄存器中原先的值
	ret
;-----------------------------------------------------------------------------------------------------------------------

;内存初始化函数
;-----------------------------------------------------------------------------------------------------------------------
memset:
	push ebp
	mov ebp, esp
	push edi
	push edx
	push ecx
		
	mov edi, [ss:ebp+8]					;Destination
	mov edx, [ss:ebp+12]				;ch
	mov ecx, [ss:ebp+16]				;size
	cld
	rep stosb
	pop ecx
	pop edx
	pop edi
	mov esp, ebp
	pop ebp
	ret