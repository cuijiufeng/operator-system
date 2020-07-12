; 2020��07��05�� 15/52/42
; ��boot�ｫloader���뵽�ڴ沢��ִ��loader
;==================================================================================================

;boot״̬
org 7c00H
;���볣����Ϣ			
%include	"const.inc"		

;����ת������FAT12ͷ��Ϣ
jmp short LABEL_START
;FAT12ͷ��Ϣ
%include	"fat12hdr.inc"

;���뿪ʼ��
;==================================================================================================
LABEL_START:
		;��ʼ���Ĵ���
		mov ax,cs
		mov ds,ax
		mov ss,ax
		;boot״̬�µĶ�ջջ��ָ��
		mov sp,TOP_OF_STACK_BOOT
		
		;�����Ļ
		call clearScreen
		
		;������λ
		mov ah,00h
		mov dl,0h
		int 13H								
		
		;��ʼ��׼����������
		mov ax,BUFFER_ROOT_ADDRESS
		mov es,ax
		mov ax,ROOT_DERCOTRY_START			
	
	;��ѭ����ȡ��Ŀ¼�������ڴ沢���ڸ�Ŀ¼�����в���loader�ļ�������Ŀ
	TRAVERSE_THE_ROOT_SECTOR:
		mov bx,BUFFER_ROOT_OFFSET
		mov cx,1
		call readSector						;������
		call findTheLoaer					;Ѱ��loader�ļ�
		cmp cx,1							;�ж��Ƿ��ҵ��ļ�
		je FIND_THE_LOADER_FILE
		inc ax								;��һ������
		dec word [ds:RootDirctoryCount]		;��Ŀ¼�����ĸ���
		cmp word [ds:RootDirctoryCount],0	;��Ŀ¼�����Ƿ��Ѿ�������
	jne TRAVERSE_THE_ROOT_SECTOR
	
	;δ�ҵ�loader�ļ�
	NOT_FIND_THE_LOADER_FILE:
		mov si,StringNotFindLoad
		mov di,0A0H
		call displayString
		jmp $
	
	;�ҵ�loader�ļ�
	FIND_THE_LOADER_FILE:
		add bx,01AH							;�ļ�������Ŀ��DIR_FstClus���ԣ�����¼���ļ��������Ǹ�����
		mov ax,[es:bx]						;�ݴ�
		mov dx,BASE_OF_LOADER				;���ļ������ݼ��ص������ַ������ַ
		mov es,dx
		mov bx,OFFSET_OF_LOADER				;���ļ������ݼ��ص������ַ��ƫ�Ƶ�ַ
	
		mov di,00H
	;��ѭ����ȡloader���ݵ��ڴ沢�Ҷ�ȡFAT��������FAT�����в���loader�ļ��Ƿ��к������
	LOOP_LOADING_LOADER_FILE:
		push ax
		add ax,DATA_SECTOR_START			;�ļ����ݵ�������
		mov cx,1							;��һ������
		call readSector						;���ļ�������
		mov si,StringLoadingLoader
		call displayString
		pop ax                          	
		call readFATValue					;��ȡFATֵ
		cmp ax,0FF7H						;������ڵ���0FF7H��ͣ����
		jnb LOAD_LOADER_OVER              	
		add bx,512							;������
	jmp LOOP_LOADING_LOADER_FILE
	
	;loader�ļ���ȫ�����ڴ�֮��תȥִ��loader�ļ�
	LOAD_LOADER_OVER:
		mov si,StringLoadedLoad
		mov di,0A0H
		call displayString
		jmp BASE_OF_LOADER:OFFSET_OF_LOADER		;ִ��loader

;Ѱ��loader�ļ�(cx==0	û���ҵ��ļ�		cx==1	�ҵ��ļ�		es:bx	ָ���ļ�������Ŀ)
;--------------------------------------------------------------------------------------------------
findTheLoaer:
	mov cx,THE_SECTOR_FILE_COUNT			;һ��������16���ļ���Ŀ������
	
	TRAVERSE_FILE_ENTRY:
		push cx
		mov cx,CHARLENGTH_OF_FILE_NAME		;�ļ�������һ����11���ַ�
		mov si,FileNameLoader				;�ļ������ַ�����ƫ�Ƶ�ַ
		TRAVERSE_FILE_CHAR:
			mov dh,[es:bx]
			mov dl,[ds:si]
			cmp dh,dl						;�ַ��Ƚ�
			jne	CONTINUE_TRAVERSE_FILE_ENTRY;��һ���ַ�����ͬ�Ͳ���Ҫ�ҵ��ļ�
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

;�����Ļ
;--------------------------------------------------------------------------------------------------
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

;��ӡ�ַ���
;--------------------------------------------------------------------------------------------------
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

;���뺯��������һЩ����
;==================================================================================================
%include	"floppylib.inc"
RootDirctoryCount:		dw		14						;224*32/512=14	BPB_RootEntCnt*32/BPB_BytsPerSec=14	��Ŀ¼������������
EvenOrOdd:				db		0						;��������ż��
FileNameLoader:			db		'LOADER  BIN'			;11���ֽڵ��ļ�����
StringLoadedLoad:		db		'loaded the loader',0
StringNotFindLoad:		db		'not find the loder',0
StringLoadingLoader:	db		'.',0
	
;�����ֽ����boot������־
;==================================================================================================
times	510-($-$$)	db	0
dw		0AA55H