#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

#include "stm32f1xx.h"

#include "tim.h"






#define delay_us			TIM2_Delay_us
#define delay_ms			TIM2_Delay_ms

void TIM2_Delay_us(uint16_t us);
void TIM2_Delay_ms(uint16_t ms);

#endif
