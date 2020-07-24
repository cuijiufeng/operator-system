;2020年07月05日 16/31/39
;一些常量信息
;========================================================================================================

TOP_OF_STACK_BOOT				equ		7c00H				;boot的堆栈栈顶指针
TOP_OF_STACK_LOADER				equ		0100H				;实模式下loader的栈顶指针
			
PAGE_DIR_BASE					equ		100000H				;页目标录表开始1MB 
PAGE_TBL_BASE					equ		101000H				;页表开始地址1MB+4KB
			
CHECK_MEMORY_BASE				equ		7000H				;内存信息数据结构基地址	
MEMORY_NUMBER_OFFSET			equ		0000H				;内存信息数据结构条目数偏移
MEMORY_SIZE_OFFSET				equ		0010H				;内存信息大小数据偏移
MEMORY_BUFFER_OFFSET			equ		0100H				;内存信息数据结构偏移
MEMORY_NUMBER_PHY_ADDR			equ		CHECK_MEMORY_BASE*10H+MEMORY_NUMBER_OFFSET
MEMORY_SIZE_PHY_ADDR			equ		CHECK_MEMORY_BASE*10H+MEMORY_SIZE_OFFSET
MEMORY_BUFFER_PHY_ADDR			equ		CHECK_MEMORY_BASE*10H+MEMORY_BUFFER_OFFSET
			
BUFFER_ROOT_ADDRESS				equ		6000H				;把软盘根目录扇区内容读到此基地址
BUFFER_ROOT_OFFSET				equ		0100H				;把软盘根目录扇区内容读到此偏移地址
			
BUFFER_FAT_ADDRESS				equ		6000H				;把FAT扇区加载到此基地址
BUFFER_FAT_OFFSET				equ		0100H				;把FAT扇区加载到此偏移地址
			
BASE_OF_LOADER					equ		8000H				;loader内容加载到此地址,基地址
OFFSET_OF_LOADER				equ		0100H				;loader内容加载到此地址,偏移地址
BASE_OF_LOADER_PHY_ADDR			equ		BASE_OF_LOADER*10H	;loader被加载到上地址，则	标号的物理地址=loader基地址*10H+标号偏移地址
	
BASE_OF_KERNEL					equ		9000H				;Kernel内容加载到此地址,基地址
OFFSET_OF_KERNEL				equ		0H					;Kernel内容加载到此地址,偏移地址
BASE_OF_KERNEL_FILE_PHY_ADDR	equ		BASE_OF_KERNEL*10H

KERNEL_ENTRY_POINT_PHY_ADDR		equ		030400h				;注意：1、必须与 MAKEFILE 中参数 -Ttext 的值相等!!

;下面是载入文件所需要的常量值 
;----------------------------------------------------------------------------------------------------
ROOT_DERCOTRY_START		equ		19				;根目录起始扇区
THE_SECTOR_FILE_COUNT	equ		16				;一个扇区有16个文件条目描述符
CHARLENGTH_OF_FILE_NAME	equ		11				;文件名字的字符长度
DATA_SECTOR_START		equ		19+14-2			;数据区的开始扇区号
FAT_SECTOR_START		equ		1				;FAT扇区开始扇区号
;----------------------------------------------------------------------------------------------------