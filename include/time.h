/**
 * 2020年08月21日 19/44/01
 * 时间 
 */
#ifndef _OS_TIME_H_
#define	_OS_TIME_H_

#define MINUTE		60				//1分钟60秒
#define HOUR		(60*MINUTE)		//1小时多少秒
#define DAY			(24*HOUR)		//1天多少秒
#define YEAR		(365*DAY)		//1年多少秒
#define TIMER0          0x40		//8253计数器0端口
#define TIMER_MODE      0x43		//8253控制端口
#define RATE_GENERATOR	0x34		//00-11-010-0 : 计数器0-写初值(先写低8位，再写高8位)-模式2(分频器)-二进制计数
#define TIMER_FREQ		1193182L	//8253输入频率
#define HZ				100			//8253输出频率(10ms:10毫秒)
#define	CMOS_CTL		0x70
#define	CMOS_VALUE		0x71
#define	CMOS_DATA_SEC	0x80
#define	CMOS_DATA_MIN	0x82
#define	CMOS_DATA_HOUR	0x84
#define	CMOS_DATA_DAY	0x87
#define	CMOS_DATA_MON	0x88
#define	CMOS_DATA_YEAR	0x89
//读取出来的数值是BCD码，BCD码是用4位二进制来表示十进制数的
#define	BCD_TO_BIN(val)	(((val)&0xF) + ((val)>>4)*10)

typedef struct time {
	u_16	tm_sec;		//秒数[0-59]
	u_16	tm_min;		//分钟数[0-59]
	u_16	tm_hour;	//小时数[0-59]
	u_16	tm_mday;	//1人月的天数[0-31]
	u_16	tm_mon;		//1年中月份[0-11]
	u_16	tm_year;	//从1900年开始的年数
	u_16	tm_wday;	//1个星期中的某天[0-6]
	u_16	tm_yday;	//1年中的某天[0-365]
} TIME;

//---------------------------------------------------------------------------------------
EXTERN	u_32	TICKS;			//时钟中断发生的次数
EXTERN	u_32	STARTUP_TIME;	//从1970年1月1日08:00:00到现在的秒数
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
//相关函数声明
PUBLIC	void	initTime(); 
PRIVATE	u_32	mkTime(TIME* time);
PUBLIC	void	timerHandler(int irq);
//---------------------------------------------------------------------------------------

#endif // !_OS_TIME_H_
