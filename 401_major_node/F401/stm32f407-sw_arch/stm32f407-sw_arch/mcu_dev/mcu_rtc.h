#ifndef _MCU_RTC_H_
#define _MCU_RTC_H_

typedef long time_t;

/*
	��1970��1��1��0:0:0��ʼ���˶�����
*/
struct timeval {
	time_t tv_sec; /*��*/    
	time_t tv_usec; /*΢��*/ 
};

struct tm {
	int tm_sec; /*�룬������Χ0-59�� ��������61*/    
	int tm_min; /*���ӣ�0-59*/    
	int tm_hour; /*Сʱ�� 0-23*/     
	int tm_mday; /*�գ���һ�����еĵڼ��죬1-31*/    
	int tm_mon; /*�£� ��һ������0-11*/  
	int tm_year; /*�꣬ ��1900�����Ѿ�������*/ 
	int tm_wday; /*���ڣ�һ���еĵڼ��죬 ������������0-6*/    
	int tm_yday; /*�ӽ���1��1�յ�Ŀǰ����������Χ0-365*/    
	int tm_isdst; /*����ʱ��ʶ�� 1: �� DST��0: ���� DST�����������˽�*/ 
	long int tm_gmtoff;/*ָ�������ڱ���߶���ʱ����UTC����ʱ����������UTC����ʱ���ĸ�����*/
	const char *tm_zone;     /*��ǰʱ��������(�뻷������TZ�й�)*/
};

extern s32 mcu_rtc_init(void);
extern s32 mcu_rtc_set_time(u8 hours, u8 minutes, u8 seconds);
extern s32 mcu_rtc_set_date(u8 year, u8 weekday, u8 month, u8 date);
#endif
