#include "bsp_timer.h"


void TIM1_Delay_us(uint16_t us)
{
	__HAL_TIM_SetCounter(&htim1, 0);//�����Զ���װ�ڼĴ���Counter
	
	__HAL_TIM_ENABLE(&htim1);	//������ʱ��TIM1
	
	/*__HAL_TIM_GetCounter()������ѯCounter������*/
	while(__HAL_TIM_GetCounter(&htim1) < us); //�ȴ���ʱ�����������λus
	
	__HAL_TIM_DISABLE(&htim1); //�رն�ʱ��TIM1
}

void TIM1_Delay_ms(uint16_t ms){
	uint16_t t;
	for(t=0;t<1000;t++)
		TIM1_Delay_us(ms);
}

