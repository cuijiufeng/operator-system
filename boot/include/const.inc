;2020年07月05日 16/31/39
;一些常量信息
;========================================================================================================

TOP_OF_STACK				equ		7c00H		;boot状态下的堆栈栈顶指针
BUFFER_ROOT_ADDRESS			equ		6000H		;把软盘根目录扇区内容读到此基地址
BUFFER_ROOT_OFFSET			equ		0100H		;把软盘根目录扇区内容读到此偏移地址

BUFFER_FAT_ADDRESS			equ		6000H		;把FAT扇区加载到此基地址
BUFFER_FAT_OFFSET			equ		0100H		;把FAT扇区加载到此偏移地址

BASE_OF_LOADER				equ		8000H		;loader内容加载到此地址,基地址
OFFSET_OF_LOADER			equ		0100H		;loader内容加载到此地址,偏移地址

BASE_OF_KERNEL				equ		9000H		;Kernel内容加载到此地址,基地址
OFFSET_OF_KERNEL			equ		0H			;Kernel内容加载到此地址,偏移地址

BASE_OF_LOADER_PHY_ADDR			equ		BASE_OF_LOADER*10H	;loader被加载到上地址，则	标号的物理地址=loader基地址*10H+标号偏移地址
BASE_OF_KERNEL_FILE_PHY_ADDR	equ		BASE_OF_KERNEL*10H
KERNEL_ENTRY_POINT_PHY_ADDR		equ		030400h				;注意：1、必须与 MAKEFILE 中参数 -Ttext 的值相等!!