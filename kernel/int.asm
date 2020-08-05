; 2020年07月30日 14:11:28
; 所有的中断处理程序入口
;==================================================================================================

extern syserrHandler
global divideError,singleStepException,nmi,breakpointException,overflow,boundsCheck,invalOpcode,coprNotAvailable
global doubleFault,coprSegOverrun,invalTss,segmentNotPresent,stackException,generalProtection,pageFault,coprError

;中断、异常-------异常
;==================================================================================================================
divideError:
	push 0FFFFFFFFH				;压入中断出错码
	push 0H						;压入中断向量号
	call syserrHandler
	add esp,8
	iret
singleStepException:
	push 0FFFFFFFFH
	push 1H
	call syserrHandler
	add esp,8
	iret
nmi:
	push 0FFFFFFFFH
	push 2H
	call syserrHandler
	add esp,8
	iret
breakpointException:
	push 0FFFFFFFFH
	push 3H
	call syserrHandler
	add esp,8
	iret
overflow:
	push 0FFFFFFFFH
	push 4H
	call syserrHandler
	add esp,8
	iret
boundsCheck:
	push 0FFFFFFFFH
	push 5H
	call syserrHandler
	add esp,8
	iret
invalOpcode:
	push 0FFFFFFFFH
	push 6H
	call syserrHandler
	add esp,8
	iret
coprNotAvailable:
	push 0FFFFFFFFH
	push 7H
	call syserrHandler
	add esp,8
	iret
doubleFault:
	push 8H
	call syserrHandler
	add esp,4
	iret
coprSegOverrun:
	push 0FFFFFFFFH
	push 9H
	call syserrHandler
	add esp,8
	iret
invalTss:
	push 0AH
	call syserrHandler
	add esp,4
	iret
segmentNotPresent:
	push 0BH
	call syserrHandler
	add esp,4
	iret
stackException:
	push 0CH
	call syserrHandler
	add esp,4
	iret
generalProtection:
	push 0DH
	call syserrHandler
	add esp,4
	iret
pageFault:
	push 0EH
	call syserrHandler
	add esp,4
	iret
coprError:
	push 0FFFFFFFFH
	push 0FH
	call syserrHandler
	add esp,8
	iret
;==================================================================================================================