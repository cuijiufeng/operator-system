; 2020年07月11日 17/44/59
; loader读kernel进内存
;1.加载内核(kernel)到内存
;2.跳入保护模式
;	1)在实模式下获取内存信息及内存大小，为保护模式下开启分页机制作准备
;	2)跳入保护模式
;	3)开启分页机制
;3.重新放置内核
;4.向内核交出控制权
;==================================================================================================

org	0100H				
;导入关于载入文件地址的一些常量值
%include "const.inc"			
;导入描述符结构及其常量值
%include "pm.inc"			
jmp LABEL_START
;导入dos可以识别的BPB等头信息及一些关于软盘的常量值
%include "fat12hdr.inc"	

;描述符表
;==================================================================================================
LABEL_GDT:			Descriptor		0,			0,				0
LABEL_DESC_FLAT_C:	Descriptor		0,			0FFFFFH,		DA_32 | DA_LIMIT_4K | DA_CR		;0~4G只执行段
LABEL_DESC_FLAT_RW:	Descriptor		0,			0FFFFFH,		DA_32 | DA_LIMIT_4K | DA_DRW	;0~4G可读写段
LABEL_VIDEO:		Descriptor		0B8000H,	0FFFFH,			DA_DRW | DA_DPL3
	
;GDTR
GDT_LEN	equ		$ - LABEL_GDT
GdtPtr:	dw		GDT_LEN - 1
		dd		BASE_OF_LOADER_PHY_ADDR + LABEL_GDT
		;以前的时候在程序里修改GdtPtr的值，因为我们不知道段地址，而现在loader是我们自己加载进内存的，所以我们知道段地址
			
;选择子
SELECTOR_FLAT_C		equ		LABEL_DESC_FLAT_C - LABEL_GDT
SELECTOR_FLOAT_RW	equ		LABEL_DESC_FLAT_RW - LABEL_GDT
SELECTOR_VIDEO		equ		LABEL_VIDEO - LABEL_GDT + SA_RPL3

;代码开始处
;==================================================================================================		
LABEL_START:
	mov ax,cs
	mov ds,ax
	mov ss,ax
	mov sp,TOP_OF_STACK_LOADER

	call getMemeryInfo						;获得内存信息
	call getMemerySize						;计算内存的大小
		
	mov si,_MemeryInfoTableHead
	mov di,280H
	call displayString						;打印内存信息表的表头
		
	call disMemInfo							;打印内存信息
	call disMemSize							;打印内存的大小

	;载入内核
	mov ax,BUFFER_ROOT_ADDRESS
	mov es,ax
	mov ax,ROOT_DERCOTRY_START				;要读起始扇区号

	TRAVERSE_THE_ROOT_SECTOR:
		mov bx,BUFFER_ROOT_OFFSET
		mov cx,1
		call readSector						;读扇区
		call findTheKernel					;寻找Kernel文件
		cmp cx,1							;判断是否找到文件
		je FIND_THE_LOADER_FILE
		inc ax								;下一个扇区
		dec word [ds:RootDirctoryCount]		;根目录扇区的个数
		cmp word [ds:RootDirctoryCount],0	;根目录扇区是否已经遍历完
	jne TRAVERSE_THE_ROOT_SECTOR
		
	NOT_FIND_THE_LOADER_FILE:
	mov si,StringNotFindKernel
	mov di,1E0H
	call displayString
	jmp $
		
	;找到kernel文件
	FIND_THE_LOADER_FILE:
	add bx,01AH							;文件描述条目的DIR_FstClus属性，它记录了文件内容在那个扇区
	mov ax,[es:bx]						;暂存
	mov dx,BASE_OF_KERNEL				;把文件的内容加载到这个地址，基地址
	mov es,dx
	mov bx,OFFSET_OF_KERNEL				;把文件的内容加载到这个地址，偏移地址

	mov di,140H
	;此循环读取kernel内容到内存并且读取FAT扇区，在FAT扇区中查找kernel文件是否还有后继扇区
	LOOP_LOADING_KERNEL_FILE:
		push ax
		add ax,DATA_SECTOR_START		;文件内容的扇区号
		mov cx,1						;读一个扇区
		call readSector					;读文件的内容 
		mov si,StringLoadingkernel
		call displayString
		pop ax
		call readFATValue				;读取FAT值
		cmp ax,0FF7H					;如果大于等于0FF7H就停下来
		jnb LOAD_KERNEL_OVER
		add bx,512						;缓冲区
	jmp LOOP_LOADING_KERNEL_FILE
		
	;Kernel文件完全载入内存之后转去执行Kernel文件
	LOAD_KERNEL_OVER:
	call killMotor
	mov si,StringLoadedKernel
	mov di,1E0H
	call displayString
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;准备进入保护模式
	;直接从加载gdtr开始
	lgdt	[ds:GdtPtr]		;加载gdtr
	cli						;关中断
		
	in al,92H
	or al,00000010b
	out 92H,al				;打开地址线A20
		
	mov eax,cr0
	or eax,1
	mov cr0,eax				;修改寄存器中保护模式标志位
		
	;跳入保护模式，0~4G执行段选择子:(loader基地址*10H+LABEL_PM_START标号偏移地址)
	jmp dword	SELECTOR_FLAT_C:(BASE_OF_LOADER_PHY_ADDR+LABEL_PM_START)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;--------------------------------------------------------------------------------------------------
;寻找Kernel文件(cx==0	没有找到文件		cx==1	找到文件		es:bx	指向文件描述条目)
findTheKernel:
	mov cx,THE_SECTOR_FILE_COUNT			;一个扇区有16个文件条目描述符
	TRAVERSE_FILE_ENTRY:
		push cx
		mov cx,CHARLENGTH_OF_FILE_NAME		;文件的名字一共有11个字符
		mov si,FileNameKernel				;文件名字字符串的偏移地址
		TRAVERSE_FILE_CHAR:
			mov dh,[es:bx]
			mov dl,[ds:si]
			cmp dh,dl						;字符比较
			jne	CONTINUE_TRAVERSE_FILE_ENTRY	;有一个字符不相同就不是要找的文件
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
%include	"memorylib.inc"
%include	"lib.inc"							;包含打印内存信息的函数
%include	"floppylib.inc"
RootDirctoryCount:		dw		14							;224*32/512=14	BPB_RootEntCnt*32/BPB_BytsPerSec=14	根目录区的扇区个数
EvenOrOdd:				db		0							;奇数还是偶数
FileNameKernel:			db		'KERNEL  BIN'				;11个字节的文件名字
StringLoadedKernel:		db		'loaded the kernel',0
StringNotFindKernel:	db		'not find the kernel',0
StringLoadingkernel:	db		'.',0

;32位代码段，由实模式跳入.保护模式下
;==================================================================================================
[section .s32]
ALIGN	32								;32个字节对齐
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
		
	call setupPageing				;开启分页机制

	call initKernel					;重新放置内核kernel
		
	jmp SELECTOR_FLAT_C:KERNEL_ENTRY_POINT_PHY_ADDR	;跳入内核,向内核交出控制权

;开启分页机制.为简化处理, 所有线性地址对应相等的物理地址. 并且不考虑内存空洞.
;-----------------------------------------------------------------------------------------------------
setupPageing:
	xor edx,edx
	mov eax,[ds:MEMERY_SIZE+9]				;获得内存大小
	mov ebx,400000H							;一个页表可以寻址4MB
	div ebx
	cmp edx,0								;如果余数不为零，则需要加一个页表
	je .isZero								;32MB内存的情况下分8页
.notZero:
	inc eax
.isZero:
	push eax								;暂存需要初始化的页表个数
		
	;初始化页目录
	mov cx,ax										;循环次数
	mov ax,SELECTOR_FLOAT_RW
	mov es,ax
	mov edi,PAGE_DIR_BASE
	mov eax,PAGE_TBL_BASE | PG_P  | PG_USU | PG_RWW
	.loopDirFor:
		mov [es:edi],eax
		add eax,4096
		add edi,4
	loop .loopDirFor
			
	;初始化所有页表
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
			
	mov eax,PAGE_DIR_BASE				;开启分页机制		
	mov cr3,eax
	mov eax,cr0
	or eax,80000000H
	mov cr0,eax
	ret

;把kernel加载到正确的位置.找出每个progream header，根据其信息进行内存复制
;-----------------------------------------------------------------------------------------------------
initKernel:
	xor ecx,ecx
	mov cx,[ds:BASE_OF_KERNEL_FILE_PHY_ADDR+2Ch]	;program header个数						e_phnum-->cx
	movzx	ecx, cx									;把cx中值扩展两个字节装入ecx
	mov esi,BASE_OF_KERNEL_FILE_PHY_ADDR			;kernel文件在内存中的开始处
	add esi,[ds:BASE_OF_KERNEL_FILE_PHY_ADDR+1Ch]	;program header table在文件中的偏移量	e_phoff-->esi	
	.begin:
		mov eax,[ds:esi+0h]						;										p_type-->eax
		cmp	eax,0								;如果program header的类型为0，则进行下一个program header
		je .noAcation
		push dword [ds:esi+10h]					;段的大小								p_filesz-->入栈
		mov eax,[ds:esi+4h]						;段在文件中的偏移						p_offset-->eax
		add eax,BASE_OF_KERNEL_FILE_PHY_ADDR
		push eax								;										(p_offset+BaseOfKernelFilePhyAdd)-->入栈
		push dword [ds:esi+8h]					;段要加载到内存中的此地址去				p_vaddr-->入栈
		call memCpy
		add esp,12								;把入栈的数据删除掉
		.noAcation:
		add esi,20H								;下一个program header
		dec ecx
	jnz .begin									;zf不等于0时跳转，判断cx的改变是否引起zf位改变
	ret

;MemCpy(dst, src, size)	把src内存地址的size个字节拷贝到dst内存地址去.后面的参数先入栈
;-----------------------------------------------------------------------------------------------------
memCpy:
	push	ebp
	mov	ebp, esp
	
	push	esi
	push	edi
	push	ecx							;暂存寄存器中的值
	
	mov	edi, [ebp + 8]					;Destination
	mov	esi, [ebp + 12]					;Source
	mov	ecx, [ebp + 16]					;Counter
	.1:
		cmp	ecx, 0							;判断计数器
		jz	.2								;计数器为零时跳出
	
		mov	al, [ds:esi]					; ┓
		inc	esi								; ┃
											; ┣ 逐字节移动
		mov	byte [es:edi], al				; ┃
		inc	edi								; ┛
	
		dec	ecx								;计数器减一
	jmp	.1								;循环
	.2:
	mov	eax, [ebp + 8]					;返回值,返回目标地址的偏移值Destination
	
	pop	ecx
	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp								;恢复寄存器中原先的值
	ret

;==================================================================================================
[section .data]
ALIGN	32
;用于保存内存信息的内存区域
LABEL_SEG_DATA:
	_CheckMemeryNumber:		db		0
	_CheckMemeryBuffer:		times	256	db	0
	_MemerySize:			dd		'Mem Size:',0											;内存的大小保存在 _MemerySize+9 地址		
	_MemeryInfoTableHead:	db		'BaseAddrL BaseAddrH LengthLow LengthHigh     Type',0	;实模式下的寻址方式
	
CHECK_MEMERY_NUMBER	equ	BASE_OF_LOADER_PHY_ADDR + _CheckMemeryNumber
CHECK_MEMERY_BUFFER	equ	BASE_OF_LOADER_PHY_ADDR + _CheckMemeryBuffer
MEMERY_SIZE			equ	BASE_OF_LOADER_PHY_ADDR + _MemerySize

;==================================================================================================
[section .stack]
ALIGN	32
	times	100	db	0
	TOP_OF_STACK	equ	BASE_OF_LOADER_PHY_ADDR + $		;保护模式下的栈段