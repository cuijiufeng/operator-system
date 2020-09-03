/**
 * 2020年08月21日 19/55/53
 * 时间
 */
#include	<type.h>
#include	<time.h>
#include	<int.h>
#include	<lib.h>

u_32	TICKS = 0;			//时钟中断发生的次数
u_32	STARTUP_TIME;		//从1970年1月1日08:00:00到现在的秒数

PUBLIC	void	initTime()
{
	TIME time;
	outByte(CMOS_CTL, CMOS_DATA_SEC);
	time.tm_sec = BCD_TO_BIN(inByte(CMOS_VALUE));	//读取秒数

	outByte(CMOS_CTL, CMOS_DATA_MIN);
	time.tm_min = BCD_TO_BIN(inByte(CMOS_VALUE));	//读取分数

	outByte(CMOS_CTL, CMOS_DATA_HOUR);
	time.tm_hour = BCD_TO_BIN(inByte(CMOS_VALUE));	//读取小时数

	outByte(CMOS_CTL, CMOS_DATA_DAY);
	time.tm_mday = BCD_TO_BIN(inByte(CMOS_VALUE));	//读取天数

	outByte(CMOS_CTL, CMOS_DATA_MON);
	time.tm_mon = BCD_TO_BIN(inByte(CMOS_VALUE));	//读取月数

	outByte(CMOS_CTL, CMOS_DATA_YEAR);
	time.tm_year = BCD_TO_BIN(inByte(CMOS_VALUE));	//读取年数

	time.tm_mon--;									//tm_mon是[0-11],所以减一
	STARTUP_TIME = mkTime(&time);					//计算自1970年1月1日08:00:00到现在的秒数

	//初始化 8253 
	outByte(TIMER_MODE, RATE_GENERATOR);			//写8253控制字
	outByte(TIMER0, (t_8)(TIMER_FREQ / HZ));		//写初值，低8位
	outByte(TIMER0, (t_8)((TIMER_FREQ / HZ) >> 8));	//写初值，高8位

	enableIrq(CLOCK_IRQ);							//打开时钟中断
	putIrqHandler(CLOCK_IRQ, timerHandler);			//设置时钟中断处理子程序
}

PRIVATE	u_32	mkTime(TIME* time)
{
	//每个月所对应的秒数,二月以闰年来计算的
	int month[12] = {
		0,
		DAY*(31),
		DAY*(31 + 29),
		DAY*(31 + 29 + 31),
		DAY*(31 + 29 + 31 + 30),
		DAY*(31 + 29 + 31 + 30 + 31),
		DAY*(31 + 29 + 31 + 30 + 31 + 30),
		DAY*(31 + 29 + 31 + 30 + 31 + 30 + 31),
		DAY*(31 + 29 + 31 + 30 + 31 + 30 + 31 + 31),
		DAY*(31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30),
		DAY*(31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31),
		DAY*(31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30)};
	u_32	rs;
	u_32	year;
	//tm_year里存的是当前年数后两位[0-99]
	if (time->tm_year > 70)
	{
		year = time->tm_year - 70;
	}
	else
	{
		year = time->tm_year + 100 - 70;
	}
	//(year+1)/4被称作一个魔幻值，用来计算1970年以来的闰年数(1970年到1971年year=1，1972年时year=2，1973年时year=3，此时正好过了1972年一个闰年，
	//所以(year+1)/4)相当从第一个闰年开始算的，闰年是每4年一个闰年
	rs = YEAR*year + DAY * ((year + 1) / 4);	//闰年会比普通年多一天
	rs += month[time->tm_mon];					//加上月数的秒数
	//如果((year + 2) % 4)==0表示当前年是闰年
	if (time->tm_mon > 1 && ((year + 2) % 4))
	{
		rs -= DAY;								//如果当前年不是闰年，应该减掉2月中的多出来的一天
	}
	rs += DAY * (time->tm_mday - 1);				//加上日的秒数
	rs += HOUR * time->tm_hour;
	rs += MINUTE * time->tm_min;
	rs += time->tm_sec;
	return rs;
}