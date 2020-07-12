; 2020��07��11�� 17/44/59
; loader��kernel���ڴ�
;1.�����ں�(kernel)���ڴ�
;2.���뱣��ģʽ
;	1)��ʵģʽ�»�ȡ�ڴ���Ϣ���ڴ��С��Ϊ����ģʽ�¿�����ҳ������׼��
;	2)���뱣��ģʽ
;	3)������ҳ����
;3.���·����ں�
;4.���ں˽�������Ȩ
;==================================================================================================

org	0100H				
;������������ļ���ַ��һЩ����ֵ
%include "const.inc"			
;�����������ṹ���䳣��ֵ
%include "pm.inc"			
jmp LABEL_START
;����dos����ʶ���BPB��ͷ��Ϣ��һЩ�������̵ĳ���ֵ
%include "fat12hdr.inc"	

;��������
;==================================================================================================
LABEL_GDT:			Descriptor		0,			0,				0
LABEL_DESC_FLAT_C:	Descriptor		0,			0FFFFFH,		DA_32 | DA_LIMIT_4K | DA_CR		;0~4Gִֻ�ж�
LABEL_DESC_FLAT_RW:	Descriptor		0,			0FFFFFH,		DA_32 | DA_LIMIT_4K | DA_DRW	;0~4G�ɶ�д��
LABEL_VIDEO:		Descriptor		0B8000H,	0FFFFH,			DA_DRW | DA_DPL3
	
;GDTR
GDT_LEN	equ		$ - LABEL_GDT
GdtPtr:	dw		GDT_LEN - 1
		dd		BASE_OF_LOADER_PHY_ADDR + LABEL_GDT
		;��ǰ��ʱ���ڳ������޸�GdtPtr��ֵ����Ϊ���ǲ�֪���ε�ַ��������loader�������Լ����ؽ��ڴ�ģ���������֪���ε�ַ
			
;ѡ����
SELECTOR_FLAT_C		equ		LABEL_DESC_FLAT_C - LABEL_GDT
SELECTOR_FLOAT_RW	equ		LABEL_DESC_FLAT_RW - LABEL_GDT
SELECTOR_VIDEO		equ		LABEL_VIDEO - LABEL_GDT + SA_RPL3

;���뿪ʼ��
;==================================================================================================		
LABEL_START:
	mov ax,cs
	mov ds,ax
	mov ss,ax
	mov sp,TOP_OF_STACK_LOADER

	call getMemeryInfo						;����ڴ���Ϣ
	call getMemerySize						;�����ڴ�Ĵ�С
		
	mov si,_MemeryInfoTableHead
	mov di,280H
	call displayString						;��ӡ�ڴ���Ϣ��ı�ͷ
		
	call disMemInfo							;��ӡ�ڴ���Ϣ
	call disMemSize							;��ӡ�ڴ�Ĵ�С

	;�����ں�
	mov ax,BUFFER_ROOT_ADDRESS
	mov es,ax
	mov ax,ROOT_DERCOTRY_START				;Ҫ����ʼ������

	TRAVERSE_THE_ROOT_SECTOR:
		mov bx,BUFFER_ROOT_OFFSET
		mov cx,1
		call readSector						;������
		call findTheKernel					;Ѱ��Kernel�ļ�
		cmp cx,1							;�ж��Ƿ��ҵ��ļ�
		je FIND_THE_LOADER_FILE
		inc ax								;��һ������
		dec word [ds:RootDirctoryCount]		;��Ŀ¼�����ĸ���
		cmp word [ds:RootDirctoryCount],0	;��Ŀ¼�����Ƿ��Ѿ�������
	jne TRAVERSE_THE_ROOT_SECTOR
		
	NOT_FIND_THE_LOADER_FILE:
	mov si,StringNotFindKernel
	mov di,1E0H
	call displayString
	jmp $
		
	;�ҵ�kernel�ļ�
	FIND_THE_LOADER_FILE:
	add bx,01AH							;�ļ�������Ŀ��DIR_FstClus���ԣ�����¼���ļ��������Ǹ�����
	mov ax,[es:bx]						;�ݴ�
	mov dx,BASE_OF_KERNEL				;���ļ������ݼ��ص������ַ������ַ
	mov es,dx
	mov bx,OFFSET_OF_KERNEL				;���ļ������ݼ��ص������ַ��ƫ�Ƶ�ַ

	mov di,140H
	;��ѭ����ȡkernel���ݵ��ڴ沢�Ҷ�ȡFAT��������FAT�����в���kernel�ļ��Ƿ��к������
	LOOP_LOADING_KERNEL_FILE:
		push ax
		add ax,DATA_SECTOR_START		;�ļ����ݵ�������
		mov cx,1						;��һ������
		call readSector					;���ļ������� 
		mov si,StringLoadingkernel
		call displayString
		pop ax
		call readFATValue				;��ȡFATֵ
		cmp ax,0FF7H					;������ڵ���0FF7H��ͣ����
		jnb LOAD_KERNEL_OVER
		add bx,512						;������
	jmp LOOP_LOADING_KERNEL_FILE
		
	;Kernel�ļ���ȫ�����ڴ�֮��תȥִ��Kernel�ļ�
	LOAD_KERNEL_OVER:
	call killMotor
	mov si,StringLoadedKernel
	mov di,1E0H
	call displayString
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;׼�����뱣��ģʽ
	;ֱ�ӴӼ���gdtr��ʼ
	lgdt	[ds:GdtPtr]		;����gdtr
	cli						;���ж�
		
	in al,92H
	or al,00000010b
	out 92H,al				;�򿪵�ַ��A20
		
	mov eax,cr0
	or eax,1
	mov cr0,eax				;�޸ļĴ����б���ģʽ��־λ
		
	;���뱣��ģʽ��0~4Gִ�ж�ѡ����:(loader����ַ*10H+LABEL_PM_START���ƫ�Ƶ�ַ)
	jmp dword	SELECTOR_FLAT_C:(BASE_OF_LOADER_PHY_ADDR+LABEL_PM_START)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;--------------------------------------------------------------------------------------------------
;Ѱ��Kernel�ļ�(cx==0	û���ҵ��ļ�		cx==1	�ҵ��ļ�		es:bx	ָ���ļ�������Ŀ)
findTheKernel:
	mov cx,THE_SECTOR_FILE_COUNT			;һ��������16���ļ���Ŀ������
	TRAVERSE_FILE_ENTRY:
		push cx
		mov cx,CHARLENGTH_OF_FILE_NAME		;�ļ�������һ����11���ַ�
		mov si,FileNameKernel				;�ļ������ַ�����ƫ�Ƶ�ַ
		TRAVERSE_FILE_CHAR:
			mov dh,[es:bx]
			mov dl,[ds:si]
			cmp dh,dl						;�ַ��Ƚ�
			jne	CONTINUE_TRAVERSE_FILE_ENTRY	;��һ���ַ�����ͬ�Ͳ���Ҫ�ҵ��ļ�
			inc bx
			inc si							;��һ���ַ�
		loop TRAVERSE_FILE_CHAR
		and bx,0FFE0H					;ָ���ļ�������Ŀ�Ŀ�ʼ
		pop cx
		mov cx,1						;����ҵ��ļ�
		ret
		CONTINUE_TRAVERSE_FILE_ENTRY:
		and bx,0FFE0H					;��bx�Ƶ��ļ���������Ŀ�Ŀ�ͷ
		add bx,32						;��һ���ļ�������Ŀ
		pop cx
	loop TRAVERSE_FILE_ENTRY
	mov cx,0								;���û���ҵ��ļ�
	ret

;���뺯��������һЩ����
;==================================================================================================
%include	"memorylib.inc"
%include	"lib.inc"							;������ӡ�ڴ���Ϣ�ĺ���
%include	"floppylib.inc"
RootDirctoryCount:		dw		14							;224*32/512=14	BPB_RootEntCnt*32/BPB_BytsPerSec=14	��Ŀ¼������������
EvenOrOdd:				db		0							;��������ż��
FileNameKernel:			db		'KERNEL  BIN'				;11���ֽڵ��ļ�����
StringLoadedKernel:		db		'loaded the kernel',0
StringNotFindKernel:	db		'not find the kernel',0
StringLoadingkernel:	db		'.',0

;32λ����Σ���ʵģʽ����.����ģʽ��
;==================================================================================================
[section .s32]
ALIGN	32								;32���ֽڶ���
[BITS 32]
LABEL_PM_START:
	mov ax,SELECTOR_VIDEO
	mov gs,ax
	mov ax,SELECTOR_FLOAT_RW
	mov ds,ax
	mov ss,ax
	mov es,ax
	mov fs,ax
	mov esp,TOP_OF_STACK
		
	call setupPageing				;������ҳ����

	call initKernel					;���·����ں�kernel
		
	jmp SELECTOR_FLAT_C:KERNEL_ENTRY_POINT_PHY_ADDR	;�����ں�,���ں˽�������Ȩ

;������ҳ����.Ϊ�򻯴���, �������Ե�ַ��Ӧ��ȵ������ַ. ���Ҳ������ڴ�ն�.
;-----------------------------------------------------------------------------------------------------
setupPageing:
	xor edx,edx
	mov eax,[ds:MEMERY_SIZE+9]				;����ڴ��С
	mov ebx,400000H							;һ��ҳ�����Ѱַ4MB
	div ebx
	cmp edx,0								;���������Ϊ�㣬����Ҫ��һ��ҳ��
	je .isZero								;32MB�ڴ������·�8ҳ
.notZero:
	inc eax
.isZero:
	push eax								;�ݴ���Ҫ��ʼ����ҳ�����
		
	;��ʼ��ҳĿ¼
	mov cx,ax										;ѭ������
	mov ax,SELECTOR_FLOAT_RW
	mov es,ax
	mov edi,PAGE_DIR_BASE
	mov eax,PAGE_TBL_BASE | PG_P  | PG_USU | PG_RWW
	.loopDirFor:
		mov [es:edi],eax
		add eax,4096
		add edi,4
	loop .loopDirFor
			
	;��ʼ������ҳ��
	pop eax
	mov ebx,1024
	mul ebx
	mov ecx,eax
	mov ax,SELECTOR_FLOAT_RW
	mov es,ax
	mov edi,PAGE_TBL_BASE
	mov eax,PG_P  | PG_USU | PG_RWW
	.loopTblFor:
		mov [es:edi],eax
		add eax,4096
		add edi,4
	loop .loopTblFor
			
	mov eax,PAGE_DIR_BASE				;������ҳ����		
	mov cr3,eax
	mov eax,cr0
	or eax,80000000H
	mov cr0,eax
	ret

;��kernel���ص���ȷ��λ��.�ҳ�ÿ��progream header����������Ϣ�����ڴ渴��
;-----------------------------------------------------------------------------------------------------
initKernel:
	xor ecx,ecx
	mov cx,[ds:BASE_OF_KERNEL_FILE_PHY_ADDR+2Ch]	;program header����						e_phnum-->cx
	movzx	ecx, cx									;��cx��ֵ��չ�����ֽ�װ��ecx
	mov esi,BASE_OF_KERNEL_FILE_PHY_ADDR			;kernel�ļ����ڴ��еĿ�ʼ��
	add esi,[ds:BASE_OF_KERNEL_FILE_PHY_ADDR+1Ch]	;program header table���ļ��е�ƫ����	e_phoff-->esi	
	.begin:
		mov eax,[ds:esi+0h]						;										p_type-->eax
		cmp	eax,0								;���program header������Ϊ0���������һ��program header
		je .noAcation
		push dword [ds:esi+10h]					;�εĴ�С								p_filesz-->��ջ
		mov eax,[ds:esi+4h]						;�����ļ��е�ƫ��						p_offset-->eax
		add eax,BASE_OF_KERNEL_FILE_PHY_ADDR
		push eax								;										(p_offset+BaseOfKernelFilePhyAdd)-->��ջ
		push dword [ds:esi+8h]					;��Ҫ���ص��ڴ��еĴ˵�ַȥ				p_vaddr-->��ջ
		call memCpy
		add esp,12								;����ջ������ɾ����
		.noAcation:
		add esi,20H								;��һ��program header
		dec ecx
	jnz .begin									;zf������0ʱ��ת���ж�cx�ĸı��Ƿ�����zfλ�ı�
	ret

;MemCpy(dst, src, size)	��src�ڴ��ַ��size���ֽڿ�����dst�ڴ��ַȥ.����Ĳ�������ջ
;-----------------------------------------------------------------------------------------------------
memCpy:
	push	ebp
	mov	ebp, esp
	
	push	esi
	push	edi
	push	ecx							;�ݴ�Ĵ����е�ֵ
	
	mov	edi, [ebp + 8]					;Destination
	mov	esi, [ebp + 12]					;Source
	mov	ecx, [ebp + 16]					;Counter
	.1:
		cmp	ecx, 0							;�жϼ�����
		jz	.2								;������Ϊ��ʱ����
	
		mov	al, [ds:esi]					; ��
		inc	esi								; ��
											; �� ���ֽ��ƶ�
		mov	byte [es:edi], al				; ��
		inc	edi								; ��
	
		dec	ecx								;��������һ
	jmp	.1								;ѭ��
	.2:
	mov	eax, [ebp + 8]					;����ֵ,����Ŀ���ַ��ƫ��ֵDestination
	
	pop	ecx
	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp								;�ָ��Ĵ�����ԭ�ȵ�ֵ
	ret

;==================================================================================================
[section .data]
ALIGN	32
;���ڱ����ڴ���Ϣ���ڴ�����
LABEL_SEG_DATA:
	_CheckMemeryNumber:		db		0
	_CheckMemeryBuffer:		times	256	db	0
	_MemerySize:			dd		'Mem Size:',0											;�ڴ�Ĵ�С������ _MemerySize+9 ��ַ		
	_MemeryInfoTableHead:	db		'BaseAddrL BaseAddrH LengthLow LengthHigh     Type',0	;ʵģʽ�µ�Ѱַ��ʽ
	
CHECK_MEMERY_NUMBER	equ	BASE_OF_LOADER_PHY_ADDR + _CheckMemeryNumber
CHECK_MEMERY_BUFFER	equ	BASE_OF_LOADER_PHY_ADDR + _CheckMemeryBuffer
MEMERY_SIZE			equ	BASE_OF_LOADER_PHY_ADDR + _MemerySize

;==================================================================================================
[section .stack]
ALIGN	32
	times	100	db	0
	TOP_OF_STACK	equ	BASE_OF_LOADER_PHY_ADDR + $		;����ģʽ�µ�ջ��