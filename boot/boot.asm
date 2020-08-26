; 2020年07月05日 15/52/42
; 在boot里将setup读入到内存并且执行setup
;==================================================================================================

;boot状态
org 7c00H
;导入常量信息			
%include	"const.inc"		

;短跳转，跳过FAT12头信息
jmp short LABEL_START
;FAT12头信息
%include	"fat12hdr.inc"

;代码开始处
;==================================================================================================
LABEL_START:
		;初始化寄存器
		mov ax,cs
		mov ds,ax
		mov ss,ax
		mov ax,0B800H
		mov gs,ax
		;boot状态下的堆栈栈顶指针
		mov sp,7c00H
		
		;清空屏幕
		call clearScreen
		
		;软驱复位
		mov ah,00h
		mov dl,0h
		int 13H								
		
		;初始化准备解析软盘
		mov ax,BUFFER_ROOT_ADDRESS
		mov es,ax
		mov ax,ROOT_DERCOTRY_START			
	
	;此循环读取根目录扇区到内存并且在根目录扇区中查找setup文件描述条目
	TRAVERSE_THE_ROOT_SECTOR:
		mov bx,BUFFER_ROOT_OFFSET
		mov cx,1
		call readSector						;读扇区
		call findTheLoaer					;寻找setup文件
		cmp cx,1							;判断是否找到文件
		je FIND_THE_SETUP_FILE
		inc ax								;下一个扇区
		dec word [ds:RootDirctoryCount]		;根目录扇区的个数
		cmp word [ds:RootDirctoryCount],0	;根目录扇区是否已经遍历完
	jne TRAVERSE_THE_ROOT_SECTOR
	
	;未找到setup文件
	NOT_FIND_THE_SETUP_FILE:
		mov si,StringNotFindSetup
		mov di,0A0H
		call displayString
		jmp $
	
	;找到SETUP文件
	FIND_THE_SETUP_FILE:
		add bx,01AH							;文件描述条目的DIR_FstClus属性，它记录了文件内容在那个扇区
		mov ax,[es:bx]						;暂存
		mov dx,BASE_OF_SETUP				;把文件的内容加载到这个地址，基地址
		mov es,dx
		mov bx,OFFSET_OF_SETUP				;把文件的内容加载到这个地址，偏移地址
	
		mov di,00H
	;此循环读取SETUP内容到内存并且读取FAT扇区，在FAT扇区中查找SETUP文件是否还有后继扇区
	LOOP_LOADING_SETUP_FILE:
		push ax
		add ax,DATA_SECTOR_START			;文件内容的扇区号
		mov cx,1							;读一个扇区
		call readSector						;读文件的内容
		mov si,StringLoadingSetup
		call displayString
		pop ax                          	
		call readFATValue					;读取FAT值
		cmp ax,0FF7H						;如果大于等于0FF7H就停下来
		jnb LOAD_SETUP_OVER              	
		add bx,512							;缓冲区
	jmp LOOP_LOADING_SETUP_FILE
	
	;SETUP文件完全载入内存之后转去执行SETUP文件
	LOAD_SETUP_OVER:
		mov si,StringLoadedSetup
		mov di,0A0H
		call displayString
		jmp BASE_OF_SETUP:OFFSET_OF_SETUP		;执行SETUP

;寻找SETUP文件(cx==0	没有找到文件		cx==1	找到文件		es:bx	指向文件描述条目)
;--------------------------------------------------------------------------------------------------
findTheLoaer:
	mov cx,THE_SECTOR_FILE_COUNT			;一个扇区有16个文件条目描述符
	
	TRAVERSE_FILE_ENTRY:
		push cx
		mov cx,CHARLENGTH_OF_FILE_NAME		;文件的名字一共有11个字符
		mov si,FileNameSetup				;文件名字字符串的偏移地址
		TRAVERSE_FILE_CHAR:
			mov dh,[es:bx]
			mov dl,[ds:si]
			cmp dh,dl						;字符比较
			jne	CONTINUE_TRAVERSE_FILE_ENTRY;有一个字符不相同就不是要找的文件
			inc bx
			inc si							;下一个字符
		loop TRAVERSE_FILE_CHAR
		and bx,0FFE0H					;指向文件描述条目的开始
		pop cx
		mov cx,1						;标记找到文件
		ret
		CONTINUE_TRAVERSE_FILE_ENTRY:
		and bx,0FFE0H					;让bx移到文件描述符条目的开头
		add bx,32						;下一个文件描述条目
		pop cx
	loop TRAVERSE_FILE_ENTRY
	mov cx,0								;标记没有找到文件
	ret

;清空屏幕
;--------------------------------------------------------------------------------------------------
clearScreen:
	mov di,0
	mov ax,0F00H
	mov cx,2000
	.clear:
		mov [gs:di],ax
		add di,2
	loop .clear
	ret

;打印字符串
;--------------------------------------------------------------------------------------------------
displayString:
	push ax
	mov ah, 0FH
	.display:
		mov al, [ds:si]
		cmp al, 0
		je .over
		mov [gs:di], ax
		inc si
		add di, 2
	jmp .display
	.over:
	pop ax
	ret	

;导入函数及定义一些变量
;==================================================================================================
%include	"lib/floppylib.inc"
RootDirctoryCount:		dw		14						;224*32/512=14	BPB_RootEntCnt*32/BPB_BytsPerSec=14	根目录区的扇区个数
EvenOrOdd:				db		0						;奇数还是偶数
FileNameSetup:			db		'SETUP   BIN'			;11个字节的文件名字
StringLoadedSetup:		db		'loaded the setup',0
StringNotFindSetup:		db		'not find the setup',0
StringLoadingSetup:	db		'.',0
	
;空余字节填补及boot结束标志
;==================================================================================================
times	510-($-$$)	db	0
dw		0AA55H