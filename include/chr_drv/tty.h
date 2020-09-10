/**
 * 2020��09��05�� 14/00/18
 * �ն�
 */
#ifndef _OS_TTY_H_
#define	_OS_TTY_H_

/*	intr=^C		quit=^|		erase=del	kill=^U
	 eof=^D		vtime=\0	vmin=\1		sxtc=\0
	 start=^Q	stop=^S		susp=^Z		eol=\0
	 reprint=^R	discard=^U	werase=^W	lnext=^V
	 eol2=\0
*/
#define INIT_C_CC "\003\034\177\025\004\0\1\0\021\023\032\0\022\017\027\026\0"
#define TTY_BUF_SIZE 512

//tty�ַ�������С�����tty����д�͸����������
typedef struct tty_queue {
	u_32 data;		//���л����������ַ�����ֵ(���ǵ�ǰ�ַ���)�����ڴ����նˣ����Ŵ��ж˿ڵĵ�ַ
	u_32 head;		//ͷָ��
	u_32 tail;		//βָ��
	PROCESS* proc_list;		//�ȴ������б�
	u_32 buf[TTY_BUF_SIZE];	//���л��������������
} TTY_QUEUE;

//tty���ݽṹ
typedef struct tty_struct {
	TERMIOS termios;						//�ն�io���ԺͿ����ַ����ݽṹ
	u_32	pgrp;							//�������
	int		stopped;						//ֹͣ��־
	void (*write)(struct tty_struct* tty);	//ttyд����ָ��
	TTY_QUEUE read_q;						//tty������
	TTY_QUEUE write_q;						//ttyд����
	TTY_QUEUE secondary;					//tty��������
} TTY_STRUCT;

//tty��д���������е�ַ��
EXTERN	TTY_QUEUE* TTY_QUEUE_LIST[];
//tty���ݽṹ���顣���а���������ʼ����tty_struct,�ֱ��ǿ���̨�������ն�1�ʹ����ն�2
EXTERN	TTY_STRUCT TTY_TABLE[];

PUBLIC	int ttyWrite(unsigned channel, char * buf, int nr);

#endif // !_OS_TTY_H_
