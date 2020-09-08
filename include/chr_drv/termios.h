/**
 * 2020��09��05�� 19/35/25
 * �ն�I/O�ӿڶ���
 */
#ifndef _OS_TERMIOS_H_
#define	_OS_TERMIOS_H_

//c_iflag��־λ
#define IGNBRK	0000001	   //����ʱ����break����
#define BRKINT	0000002	   //��breakʱ����sigint�ź�
#define IGNPAR	0000004	   //������żУ�������ַ�
#define PARMRK	0000010	   //��־��żУ���
#define INPCK	0000020	   //����������żУ��
#define ISTRIP	0000040	   //�����ַ���8λ
#define INLCR	0000100	   //����ʱ�����з�NLӳ��ɻس���CR
#define IGNCR	0000200	   //���Իس���CR
#define ICRNL	0000400	   //������ʱ���س���CRӳ��ɻ��з�NL
#define IUCLC	0001000	   //������ʱ����д�ַ�ת����Сд�ַ�
#define IXON	0002000	   //�����ڿ�ʼ/ֹͣ(XON/XOFF)�������
#define IXANY	0004000	   //�����κ��ַ��������
#define IXOFF	0010000	   //����ʼ/ֹͣ(XON/XOFF)�������
#define IMAXBEL	0020000	   //���������ʱ����

//c_oflag��־λ
#define OPOST	0000001	   //ִ���������
#define OLCUC	0000002	   //�����ʱ��Сд�ַ�ת���ɴ�д�ַ�
#define ONLCR	0000004	   //�����ʱ�����з�NLӳ��ɻس�-���з�CR-NL
#define OCRNL	0000010	   //�����ʱ���س���CRӳ��ɻ��з�NL
#define ONOCR	0000020	   //��0�в�����س���CR
#define ONLRET	0000040	   //���з�NLִ�лس����Ĺ���
#define OFILL	0000100	   //�ӳ�ʱʹ������ַ�����ʹ��ʱ���ӳ�
#define OFDEL	0000200	   //����ַ���ASCII��DEL�����δ���ã���ʹ��ASCII NULL
#define NLDLY	0000400	   //ѡ�����ӳ�
#define   NL0	0000000	   //�����ӳ�����0
#define   NL1	0000400	   //�����ӳ�����1
#define CRDLY	0003000	   //ѡ��س��ӳ�
#define   CR0	0000000	   //�س��ӳ�����0
#define   CR1	0001000	   //�س��ӳ�����1
#define   CR2	0002000	   //�س��ӳ�����2
#define   CR3	0003000	   //�س��ӳ�����3
#define TABDLY	0014000	   //ѡ��ˮƽ�Ʊ��ӳ�
#define   TAB0	0000000	   //ˮƽ�Ʊ��ӳ�����0
#define   TAB1	0004000	   //ˮƽ�Ʊ��ӳ�����1
#define   TAB2	0010000	   //ˮƽ�Ʊ��ӳ�����2
#define   TAB3	0014000	   //ˮƽ�Ʊ��ӳ�����3
#define   XTABS	0014000	   //���Ʊ��TAB���ɿո񣬸�ֵ��ʾ�ո���
#define BSDLY	0020000	   //ѡ���˸��ӳ�
#define   BS0	0000000	   //�˸��ӳ�����0
#define   BS1	0020000	   //�˸��ӳ�����1
#define VTDLY	0040000	   //�����Ʊ��ӳ�
#define   VT0	0000000	   //�����Ʊ��ӳ�����0
#define   VT1	0040000	   //�����Ʊ��ӳ�����1
#define FFDLY	0040000	   //ѡ��ҳ�ӳ�
#define   FF0	0000000	   //��ҳ�ӳ�����0
#define   FF1	0040000	   //��ҳ�ӳ�����1


//c_cflag��־λ
#define CBAUD	0000017	   //��������λ������
#define  B0		0000000    //�Ҷ���·
#define  B50	0000001	   //������50
#define  B75	0000002	   //������75
#define  B110	0000003	   //������110
#define  B134	0000004	   //������134
#define  B150	0000005	   //������150
#define  B200	0000006	   //������200
#define  B300	0000007	   //������300
#define  B600	0000010	   //������600
#define  B1200	0000011	   //������1200
#define  B1800	0000012	   //������1800
#define  B2400	0000013	   //������2400
#define  B4800	0000014	   //������4800
#define  B9600	0000015	   //������9600
#define  B19200	0000016	   //������19200
#define  B38400	0000017	   //������38400
#define EXTA B19200		   //��չ������A
#define EXTB B38400		   //��չ������B
#define CSIZE	0000060	   //�ַ�λ���������
#define   CS5	0000000	   //ÿ�ַ�5����λ
#define   CS6	0000020	   //ÿ�ַ�6����λ
#define   CS7	0000040	   //ÿ�ַ�7����λ
#define   CS8	0000060	   //ÿ�ַ�8����λ
#define CSTOPB	0000100	   //��������ֹͣλ��������1��
#define CREAD	0000200	   //�������
#define CPARENB	0000400	   //�������ʱ������żλ������ʱ������żУ��
#define CPARODD	0001000	   //����/���У������У��
#define HUPCL	0002000	   //�����̹رչҶ�
#define CLOCAL	0004000	   //���Ե��ƽ����������·
#define CIBAUD	03600000	//���벨����(δʹ��)
#define CRTSCTS	020000000000//������

#define PARENB CPARENB		//�������ʱ������żλ������ʱ������żУ��
#define PARODD CPARODD		//����/���У������У��


//c_lflag��־λ
#define ISIG	0000001		//���յ��ַ�INTR\QUIT\SUSP\DSUSP��������Ӧ���ź�
#define ICANON	0000002		//�����淶ģʽ
#define XCASE	0000004		//��������ICANON�����ն��Ǵ�д�ַ���
#define ECHO	0000010		//����ʾ�����ַ�
#define ECHOE	0000020		//��������ICANON����ERASE/WERASE��̽��ǰһ�ַ�/����
#define ECHOK	0000040		//��������ICANON����KILL�ַ���̽�յ�ǰ��
#define ECHONL	0000100		//��������ICANON����ʹECHOû�п���Ҳ����NL�ַ���
#define NOFLSH	0000200		//������SIGINT��SIGQUIT�ź�ʱ��ˢ������������У�������SIGSUSP�ź�ʱ��ˢ���������
#define TOSTOP	0000400		//����SIGTTOU�źŵ���̨�����飬�ú�̨������ͼд�Լ��Ŀ����ն�
#define ECHOCTL	0001000		//��������ECHO�����TAB\NL\START��STOP���⳷��ASCII�����źŽ������Գɽ�^xʽ����Xֵ�ǿ��Ʒ�+0x40
#define ECHOPRT	0002000		//��������ICANON��ICHO�����ַ��ڲ���ʱ����ʾ
#define ECHOKE	0004000		//��������ICANON����KILLͨ���������ϵ������ַ�������
#define FLUSHO	0010000		//�����ˢ�£�ͨ������DISCARD�ַ����ñ�־����ת
#define PENDIN	0040000		//����һ���ַ��Ƕ�ʱ����������е������ַ���������
#define IEXTEN	0100000		//����ʵ��ʱ��������봦��

//mode��·�źŷ��ų���
#define TIOCM_LE	0x001	//��·����
#define TIOCM_DTR	0x002	//�����ն˾���
#define TIOCM_RTS	0x004	//������
#define TIOCM_ST	0x008	//�������ݷ���
#define TIOCM_SR	0x010	//�������ݽ���
#define TIOCM_CTS	0x020	//�������
#define TIOCM_CAR	0x040	//�ز����
#define TIOCM_RNG	0x080	//����ָʾ
#define TIOCM_DSR	0x100	//�����豸����
#define TIOCM_CD	TIOCM_CAR
#define TIOCM_RI	TIOCM_RNG

//c_cc�����е��ַ�
#define VINTR 0				//c_cc[VINTR]	= INTER		(^C) \003 �ж��ַ�
#define VQUIT 1				//c_cc[VQUIT]	= QUE		(^\) \003 �˳��ַ�
#define VERASE 2			//c_cc[VERASE]	= ERASE		(^H) \003 �����ַ�
#define VKILL 3				//c_cc[VKILL]	= KILL		(^U) \003 ��ֹ�ַ�
#define VEOF 4				//c_cc[VEOF]	= EOF		(^D) \003 �ļ������ַ�
#define VTIME 5				//c_cc[VTIME]	= TIME		(\0) \003 ��ʱ��ֵ�ַ�
#define VMIN 6				//c_cc[VMIN]	= MIN		(\1) \003 ��ʱֵ
#define VSWTC 7				//c_cc[VSWTC]	= SWTC		(\0) \003 �����ַ�
#define VSTART 8			//c_cc[VSTART]	= START		(^Q) \003 ��ʼ�ַ�
#define VSTOP 9				//c_cc[VSTOP]	= STOP		(^S) \003 ֹͣ�ַ�
#define VSUSP 10			//c_cc[VSUSP]	= SUSP		(^Z) \003 �����ַ�
#define VEOL 11				//c_cc[VEOL]	= EOL		(\0) \003 �н����ַ�
#define VREPRINT 12			//c_cc[VREPRINT]	= REPRINT	(^R) \003 ����ʾ�ַ�
#define VDISCARD 13			//c_cc[VDISCARD]	= DISCARD	(^O) \003 �����ַ�
#define VWERASE 14			//c_cc[VWERASE]	= WERASE	(^W) \003 ���ʲ����ַ�
#define VLNEXT 15			//c_cc[VLNEXT]	= LNEXT		(^V) \003 ��һ���ַ�
#define VEOL2 16			//c_cc[VEOL2]	= EOL2		(\0) \003 �н����ַ�2


//POSIX��termios�ṹ
#define	NCCS	17				//termios�ṹ�п����ַ�����ĳ���
typedef struct termios {
	u_32 c_iflag;		//����ģʽ��־
	u_32 c_oflag;		//���ģʽ��־
	u_32 c_cflag;		//����ģʽ��־
	u_32 c_lflag;		//����ģʽ��־
	u_8 c_line;			//�̹߳��(����)
	u_8 c_cc[NCCS];		//�����ַ�����
} TERMIOS;

#endif // !_OS_TERMIOS_H_
