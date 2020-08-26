/**
 * 2020��08��21�� 19/55/53
 * ʱ��
 */
#include	<type.h>
#include	<mm.h>
#include	<int.h>
#include	<time.h>
#include	<protect.h>
#include	<lib.h>

PUBLIC	void	initTime()
{
	TICKS = 0;
	TIME time;
	outByte(CMOS_CTL, CMOS_DATA_SEC);
	time.tm_sec = BCD_TO_BIN(inByte(CMOS_VALUE));	//��ȡ����

	outByte(CMOS_CTL, CMOS_DATA_MIN);
	time.tm_min = BCD_TO_BIN(inByte(CMOS_VALUE));	//��ȡ����

	outByte(CMOS_CTL, CMOS_DATA_HOUR);
	time.tm_hour = BCD_TO_BIN(inByte(CMOS_VALUE));	//��ȡСʱ��

	outByte(CMOS_CTL, CMOS_DATA_DAY);
	time.tm_mday = BCD_TO_BIN(inByte(CMOS_VALUE));	//��ȡ����

	outByte(CMOS_CTL, CMOS_DATA_MON);
	time.tm_mon = BCD_TO_BIN(inByte(CMOS_VALUE));	//��ȡ����

	outByte(CMOS_CTL, CMOS_DATA_YEAR);
	time.tm_year = BCD_TO_BIN(inByte(CMOS_VALUE));	//��ȡ����

	time.tm_mon--;									//tm_mon��[0-11],���Լ�һ
	STARTUP_TIME = mkTime(&time);					//������1970��1��1��08:00:00�����ڵ�����

	setIrqHandler(CLOCK_IRQ, timerHandler);			//����ʱ���жϴ����ӳ���
	enableIrq(CLOCK_IRQ);							//��ʱ���ж�
}

PRIVATE	u_32	mkTime(TIME* time)
{
	//ÿ��������Ӧ������,�����������������
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
	//tm_year�����ǵ�ǰ��������λ[0-99]
	if (time->tm_year > 70)
	{
		year = time->tm_year - 70;
	}
	else
	{
		year = time->tm_year + 100 - 70;
	}
	//(year+1)/4������һ��ħ��ֵ����������1970��������������(1970�굽1971��year=1��1972��ʱyear=2��1973��ʱyear=3����ʱ���ù���1972��һ�����꣬
	//����(year+1)/4)�൱�ӵ�һ�����꿪ʼ��ģ�������ÿ4��һ������
	rs = YEAR*year + DAY * ((year + 1) / 4);	//��������ͨ���һ��
	rs += month[time->tm_mon];					//��������������
	//���((year + 2) % 4)==0��ʾ��ǰ��������
	if (time->tm_mon > 1 && ((year + 2) % 4))
	{
		rs -= DAY;								//�����ǰ�겻�����꣬Ӧ�ü���2���еĶ������һ��
	}
	rs += DAY * (time->tm_mday - 1);				//�����յ�����
	rs += HOUR * time->tm_hour;
	rs += MINUTE * time->tm_min;
	rs += time->tm_sec;
	return rs;
}