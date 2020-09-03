; 2020年08月22日 20/11/55
; 调用系统调用函数
;==================================================================================================

;==================================================================================================
;PROCESS-PCB进程控制块结构体成员偏移量
STATE				equ	0
COUNTER				equ	4
;进程状态
TASK_RUNNING		equ	0
TASK_INTERRUPTIBLE	equ	1
;==================================================================================================

extern	CURRENT
extern	findEmptyProcess
extern	copyProcess
extern	schedule
global	sysFork,sysPause
;系统调用中断处理子函数
;==================================================================================================
sysFork:
	call findEmptyProcess
	cmp eax, -1
	je _1
	push eax					;进程槽位号
	call copyProcess
	add esp, 4
_1:
	ret
sysPause:
	mov eax, [CURRENT]
	add eax, STATE
	mov dword [eax], TASK_INTERRUPTIBLE	;CURRENT->state=TASK_INTERRUPTIBLE
	call schedule
	mov eax, 0					;返回值
	ret
;==================================================================================================