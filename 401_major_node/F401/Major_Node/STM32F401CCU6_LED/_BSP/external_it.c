#include "external_it.h"
uint8_t dis_flag = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	dis_flag = 1; 
	
	if(GPIO_Pin == BT_Pin){
		L_TOOGLE;
	}
	if(GPIO_Pin == TOUCH1_Pin)
	{
		
		func_index = table[func_index].enter;
	}

	if(GPIO_Pin == TOUCH2_Pin){
		user_debug("ио");
		func_index = table[func_index].up;
	}
	
	if(GPIO_Pin == TOUCH3_Pin){
		user_debug("об");
		func_index = table[func_index].down;
	}
}
