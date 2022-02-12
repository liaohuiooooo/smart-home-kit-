#include "bsp_timer.h"


void TIM2_Delay_us(uint16_t us)
{
	__HAL_TIM_SetCounter(&htim2, 0);//重置自动重装在寄存器Counter
	
	__HAL_TIM_ENABLE(&htim2);	//开启定时器TIM1
	
	/*__HAL_TIM_GetCounter()函数查询Counter的数量*/
	while(__HAL_TIM_GetCounter(&htim2) < us); //等待定时任务结束，单位us
	
	__HAL_TIM_DISABLE(&htim2); //关闭定时器TIM1
}

void TIM2_Delay_ms(uint16_t ms){
	uint16_t t;
	for(t=0;t<1000;t++)
		TIM2_Delay_us(ms);
}
