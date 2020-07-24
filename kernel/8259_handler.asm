; 2020��07��22�� 20/12/38
; 8259�жϴ����ӳ���
;==================================================================================================

%include	"kernel.inc"
extern STACK_TOP
extern restart
extern restart_reenter
global hwint00
global hwint01
global hwint02
global hwint03
global hwint04
global hwint05
global hwint06
global hwint07
global hwint08
global hwint09
global hwint10
global hwint11
global hwint12
global hwint13
global hwint14
global hwint15

;�жϷ���ʱ�ļĴ������湤��
;==================================================================================================================
save:
	pushad						; ��
	push ds						; ��   
	push es						; �� ����ring1ʱ�Ĵ�����ֵ
	push fs						; ��      
	push gs						; �� 
	mov dx,ss					; ��
	mov ds,dx					; �� ʹ��ring0ʱ�ļĴ�����ֵ
	mov es,dx					; ��
		
	mov esi,esp					;esi=���̱����ʼ��ַ
		
	inc	dword [K_RE_ENTRY]		;�����ж�k_reenter�ͼ�һ
	cmp	dword [K_RE_ENTRY], 0
	jne .1						;�ж�����ʱ����.1��ͨ�������˳��ִ��
		
	mov esp,STACK_TOP			;�л����ں�ջ����Ϊ��ʱ�Ҷ�ջ���κ�һ�����������ƻ����̱�
		
	push restart
	jmp [esi+RETADR-P_STACKBASE]
.1:								;�ж�����.�Ѿ����ں�ջ������Ҫ���л�
	push restart_reenter
	jmp [esi+RETADR-P_STACKBASE];���ص�call save��߼���ִ��
								;����saveǰ��esp��ָ����̱�ģ�������save��esp��ָ���ں�ջ�ġ�esp��ָ��ͬ����Ȼ�Ͳ���ret
;==================================================================================================================

;==================================================================================================================
;��������		��Ƭ
%macro hwint_master 1
	call save
		
	in al,INT_M_CTLMASK			; �� 
	or al,(1<<%1)				; �� �������ٷ���ʱ���ж�
	out INT_M_CTLMASK,al		; �� 
		
	mov al,	EOI					;�жϽ���������
	out	INT_M_CTL, al			;����8259�жϽ����������Ϳ��Բ�ͣ�ķ����ж�
		
	sti							;Ϊ����ִ���жϳ����ʱ�򻹿�����Ӧ�жϣ�����Ҫ��Ϊ���жϡ�CPU����Ӧ�жϵ�ʱ����Զ��ر��ж�

	push %1						;irq��
	call [(IRQ_TABLE + 4 * %1)]	;�����жϴ����ӳ���
	add esp,4					;�ɵ����������ջ����������ú���ǰѹ��Ĳ���irq
		
	cli							;�������Ҫ���жϹرյ�����߾�Ҫ�뿪�ں�ջ��׼���л����̡���������жϣ��ͻ��ƻ������̱�
		
	in al,INT_M_CTLMASK			; �� 
	and al,~(1<<%1)				; �� �����ٷ���ʱ���ж�
	out INT_M_CTLMASK,al		; �� 
		
	ret
%endmacro
;------------------------------------------------------------------------------------------------------------------
ALIGN 16						;�˴�����16�ֽڶ���
hwint00:						;�жϷ���ʱ���л����ں�̬������ring1-->ring0
	hwint_master 0		;irq��
		
ALIGN 16
hwint01:
	hwint_master 1		;irq��
		
ALIGN 16
hwint02:
	hwint_master 2
		
ALIGN 16
hwint03:
	hwint_master 3
		
ALIGN 16
hwint04:
	hwint_master 4
		
ALIGN 16
hwint05:
	hwint_master 5
		
ALIGN 16
hwint06:
	hwint_master 6
		
ALIGN 16
hwint07:
	hwint_master 7
;==================================================================================================================

;==================================================================================================================
;��������		��Ƭ
%macro hwint_slave 1
	call save
		
	in al,INT_S_CTLMASK			; �� 
	or al,(1<<(%1-8))			; �� �������ٷ���ʱ���ж�
	out INT_S_CTLMASK,al		; �� 
		
	mov al,	EOI					;�жϽ���������
	out	INT_M_CTL, al			;����8259�жϽ����������Ϳ��Բ�ͣ�ķ����ж�
	mov al,	EOI					;�жϽ���������
	out	INT_S_CTL, al			;����8259�жϽ����������Ϳ��Բ�ͣ�ķ����ж�
		
	sti							;Ϊ����ִ���жϳ����ʱ�򻹿�����Ӧ�жϣ�����Ҫ��Ϊ���жϡ�CPU����Ӧ�жϵ�ʱ����Զ��ر��ж�

	push %1						;irq��
	call [(IRQ_TABLE + 4 * %1)]	;�����жϴ����ӳ���
	add esp,4					;�ɵ����������ջ����������ú���ǰѹ��Ĳ���irq
		
	cli							;�������Ҫ���жϹرյ�����߾�Ҫ�뿪�ں�ջ��׼���л����̡���������жϣ��ͻ��ƻ������̱�
		
	in al,INT_S_CTLMASK			; �� 
	and al,~(1<<(%1-8))			; �� �����ٷ���ʱ���ж�
	out INT_S_CTLMASK,al		; �� 
		
	ret
%endmacro
;------------------------------------------------------------------------------------------------------------------
ALIGN 16
hwint08:
	hwint_slave 8
		
ALIGN 16
hwint09:
	hwint_slave 9
		
ALIGN 16
hwint10:
	hwint_slave 10
		
ALIGN 16
hwint11:
	hwint_slave 11
		
ALIGN 16
hwint12:
	hwint_slave 12
		
ALIGN 16
hwint13:
	hwint_slave 13
		
ALIGN 16
hwint14:
	hwint_slave 14
		
ALIGN 16
hwint15:
	hwint_slave 15
;==================================================================================================================