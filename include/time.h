/**
 * 2020��08��21�� 19/44/01
 * ʱ�� 
 */
#ifndef _OS_TIME_H_
#define	_OS_TIME_H_

#define MINUTE		60				//1����60��
#define HOUR		(60*MINUTE)		//1Сʱ������
#define DAY			(24*HOUR)		//1�������
#define YEAR		(365*DAY)		//1�������
#define	CMOS_CTL	0x70
#define	CMOS_VALUE	0x71
#define	CMOS_DATA_SEC	0x80
#define	CMOS_DATA_MIN	0x82
#define	CMOS_DATA_HOUR	0x84
#define	CMOS_DATA_DAY	0x87
#define	CMOS_DATA_MON	0x88
#define	CMOS_DATA_YEAR	0x89
//��ȡ��������ֵ��BCD�룬BCD������4λ����������ʾʮ��������
#define	BCD_TO_BIN(val)	(((val)&0xF) + ((val)>>4)*10)

u_32	TICKS;

typedef struct time {
	u_16	tm_sec;		//����[0-59]
	u_16	tm_min;		//������[0-59]
	u_16	tm_hour;	//Сʱ��[0-59]
	u_16	tm_mday;	//1���µ�����[0-31]
	u_16	tm_mon;		//1�����·�[0-11]
	u_16	tm_year;	//��1900�꿪ʼ������
	u_16	tm_wday;	//1�������е�ĳ��[0-6]
	u_16	tm_yday;	//1���е�ĳ��[0-365]
} TIME;

//---------------------------------------------------------------------------------------
u_32	STARTUP_TIME;			//��1970��1��1��08:00:00�����ڵ�����
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//��غ�������
PUBLIC	void	initTime(); 
PRIVATE	u_32	mkTime(TIME* time);
//---------------------------------------------------------------------------------------

#endif // !_OS_TIME_H_
