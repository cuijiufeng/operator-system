; 2020年07月05日 15/52/42
; 在boot里将loader读入到内存并且执行loader
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
		mov sp,TOP_OF_STACK
		
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
	
	;此循环读取根目录扇区到内存并且在根目录扇区中查找loader文件描述条目
	TRAVERSE_THE_ROOT_SECTOR:
		mov bx,BUFFER_ROOT_OFFSET
		mov cx,1
		call readSector						;读扇区
		call findTheLoaer					;寻找loader文件
		cmp cx,1							;判断是否找到文件
		je FIND_THE_LOADER_FILE
		inc ax								;下一个扇区
		dec word [ds:RootDirctoryCount]		;根目录扇区的个数
		cmp word [ds:RootDirctoryCount],0	;根目录扇区是否已经遍历完
	jne TRAVERSE_THE_ROOT_SECTOR
	
	;未找到loader文件
	NOT_FIND_THE_LOADER_FILE:
		mov si,StringNotFindLoad
		mov di,0A0H
		call displayString
		jmp $
	
	;找到loader文件
	FIND_THE_LOADER_FILE:
		add bx,01AH							;文件描述条目的DIR_FstClus属性，它记录了文件内容在那个扇区
		mov ax,[es:bx]						;暂存
		mov dx,BASE_OF_LOADER				;把文件的内容加载到这个地址，基地址
		mov es,dx
		mov bx,OFFSET_OF_LOADER				;把文件的内容加载到这个地址，偏移地址
	
		mov di,00H
	;此循环读取loader内容到内存并且读取FAT扇区，在FAT扇区中查找loader文件是否还有后继扇区
	LOOP_LOADING_LOADER_FILE:
		push ax
		add ax,DATA_SECTOR_START			;文件内容的扇区号
		mov cx,1							;读一个扇区
		call readSector						;读文件的内容
		mov si,StringLoadingLoader
		call displayString
		pop ax                          	
		call readFATValue					;读取FAT值
		cmp ax,0FF7H						;如果大于等于0FF7H就停下来
		jnb LOAD_LOADER_OVER              	
		add bx,512							;缓冲区
	jmp LOOP_LOADING_LOADER_FILE
	
	;loader文件完全载入内存之后转去执行loader文件
	LOAD_LOADER_OVER:
		mov si,StringLoadedLoad
		mov di,0A0H
		call displayString
		jmp BASE_OF_LOADER:OFFSET_OF_LOADER		;执行loader

;寻找loader文件(cx==0	没有找到文件		cx==1	找到文件		es:bx	指向文件描述条目)
;--------------------------------------------------------------------------------------------------
findTheLoaer:
	mov cx,THE_SECTOR_FILE_COUNT			;一个扇区有16个文件条目描述符
	
	TRAVERSE_FILE_ENTRY:
		push cx
		mov cx,CHARLENGTH_OF_FILE_NAME		;文件的名字一共有11个字符
		mov si,FileNameLoader				;文件名字字符串的偏移地址
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
	
;导入函数及定义一些变量
;==================================================================================================
	%include	"lib.inc"
	%include	"floppylib.inc"
	RootDirctoryCount:		dw		14						;224*32/512=14	BPB_RootEntCnt*32/BPB_BytsPerSec=14	根目录区的扇区个数
	EvenOrOdd:				db		0						;奇数还是偶数
	FileNameLoader:			db		'LOADER  BIN'			;11个字节的文件名字
	StringLoadedLoad:		db		'loaded the loader',0
	StringNotFindLoad:		db		'not find the loder',0
	StringLoadingLoader:	db		'.',0
	
;空余字节填补及boot结束标志
;==================================================================================================
	times	510-($-$$)	db	0
	dw		0AA55H