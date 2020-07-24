; 2020年07月22日 20/26/03
; 系统错误中断处理子程序
;==================================================================================================

extern	syserrHandler
global divideError
global singleStepException
global nmi
global breakpointException
global overflow
global boundsCheck
global invalOpcode
global coprNotAvailable
global doubleFault
global coprSegOverrun
global invalTss
global segmentNotPresent
global stackException
global generalProtection
global pageFault
global coprError

;中断、异常-------异常
;==================================================================================================================
divideError:
	push 0FFFFFFFFH				;压入中断出错码
	push 0H						;压入中断向量号
	call syserrHandler
	add esp,8
	hlt
singleStepException:
	push 0FFFFFFFFH
	push 1H
	call syserrHandler
	add esp,8
	hlt
nmi:
	push 0FFFFFFFFH
	push 2H
	call syserrHandler
	add esp,8
	hlt
breakpointException:
	push 0FFFFFFFFH
	push 3H
	call syserrHandler
	add esp,8
	hlt
overflow:
	push 0FFFFFFFFH
	push 4H
	call syserrHandler
	add esp,8
	hlt
boundsCheck:
	push 0FFFFFFFFH
	push 5H
	call syserrHandler
	add esp,8
	hlt
invalOpcode:
	push 0FFFFFFFFH
	push 6H
	call syserrHandler
	add esp,8
	hlt
coprNotAvailable:
	push 0FFFFFFFFH
	push 7H
	call syserrHandler
	add esp,8
	hlt
doubleFault:
	push 8H
	call syserrHandler
	add esp,8
	hlt
coprSegOverrun:
	push 0FFFFFFFFH
	push 9H
	call syserrHandler
	add esp,8
	hlt
invalTss:
	push 0AH
	call syserrHandler
	add esp,8
	hlt
segmentNotPresent:
	push 0BH
	call syserrHandler
	add esp,8
	hlt
stackException:
	push 0CH
	call syserrHandler
	add esp,8
	hlt
generalProtection:
	push 0DH
	call syserrHandler
	add esp,8
	hlt
pageFault:
	push 0EH
	call syserrHandler
	add esp,8
	hlt
coprError:
	push 0FFFFFFFFH
	push 0FH
	call syserrHandler
	add esp,8
	hlt
;==================================================================================================================