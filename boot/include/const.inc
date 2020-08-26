;2020年07月05日 16/31/39
;一些常量信息
;========================================================================================================
			
PAGE_DIR_BASE					equ		0H					;页目标录表开始
PAGE_TBL_BASE					equ		1000H				;页表开始地址
			
DATA_BASE						equ		9A00H				;内存信息数据结构基地址	
MEMORY_SIZE_OFFSET				equ		0000H				;内存信息大小数据偏移
MEMORY_SIZE_PHY_ADDR			equ		DATA_BASE*10H+MEMORY_SIZE_OFFSET
HD_TYPE_OFFSET					equ		0004H				;硬盘类型数据偏移
HD_PARAM_OFFSET					equ		0010H				;硬盘参数数据结构偏移
			
BUFFER_ROOT_ADDRESS				equ		9B00H				;把软盘根目录扇区内容读到此基地址
BUFFER_ROOT_OFFSET				equ		0000H				;把软盘根目录扇区内容读到此偏移地址
			
BUFFER_FAT_ADDRESS				equ		9B00H				;把FAT扇区加载到此基地址
BUFFER_FAT_OFFSET				equ		0200H				;把FAT扇区加载到此偏移地址
			
BASE_OF_SETUP					equ		9000H				;SETUP内容加载到此地址,基地址
OFFSET_OF_SETUP					equ		0100H				;SETUP内容加载到此地址,偏移地址
BASE_OF_SETUP_PHY_ADDR			equ		BASE_OF_SETUP*10H	;SETUP被加载到上地址，则	标号的物理地址=SETUP基地址*10H+标号偏移地址
	
BASE_OF_KERNEL					equ		8000H				;Kernel内容加载到此地址,基地址
OFFSET_OF_KERNEL				equ		0H					;Kernel内容加载到此地址,偏移地址
BASE_OF_KERNEL_FILE_PHY_ADDR	equ		BASE_OF_KERNEL*10H

KERNEL_ENTRY_POINT_PHY_ADDR		equ		10000H				;注意：1、必须与 MAKEFILE 中参数 -Ttext 的值相等!!

;下面是需要的常量值 
;----------------------------------------------------------------------------------------------------
ROOT_DEV				equ		0301H			;文件系统的类型
ROOT_DERCOTRY_START		equ		19				;根目录起始扇区
THE_SECTOR_FILE_COUNT	equ		16				;一个扇区有16个文件条目描述符
CHARLENGTH_OF_FILE_NAME	equ		11				;文件名字的字符长度
DATA_SECTOR_START		equ		19+14-2			;数据区的开始扇区号
FAT_SECTOR_START		equ		1				;FAT扇区开始扇区号
;----------------------------------------------------------------------------------------------------