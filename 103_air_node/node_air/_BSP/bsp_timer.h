#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

#include "stm32f1xx.h"

#include "tim.h"


#define LOOPTIME                 100 	            //��������ѭ����ʱʱ�䣺100ms
#define BLINK			        (500/LOOPTIME)		//5 LED������˸ʱ�䣺0.5 S
#define USER_DEBUG_OUT		    (1000/LOOPTIME)		//10 ���д�����ʾ��1s
#define MQTTHEART               (5000/LOOPTIME)		//50 MQTT������������5s
#define PUBLISH_DATA		    (1000/LOOPTIME)		//10	ÿ��һ�����һ������



#define delay_us			TIM2_Delay_us
#define delay_ms			TIM2_Delay_ms

void TIM2_Delay_us(uint16_t us);
void TIM2_Delay_ms(uint16_t ms);

#endif
