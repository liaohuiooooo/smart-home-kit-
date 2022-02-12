#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

#include "stm32f1xx.h"

#include "tim.h"


#define LOOPTIME                 100 	            //程序周期循环延时时间：100ms
#define BLINK			        (500/LOOPTIME)		//5 LED运行闪烁时间：0.5 S
#define USER_DEBUG_OUT		    (1000/LOOPTIME)		//10 运行串口提示：1s
#define MQTTHEART               (5000/LOOPTIME)		//50 MQTT发送心跳包：5s
#define PUBLISH_DATA		    (1000/LOOPTIME)		//10	每隔一秒更新一次数据



#define delay_us			TIM2_Delay_us
#define delay_ms			TIM2_Delay_ms

void TIM2_Delay_us(uint16_t us);
void TIM2_Delay_ms(uint16_t ms);

#endif
