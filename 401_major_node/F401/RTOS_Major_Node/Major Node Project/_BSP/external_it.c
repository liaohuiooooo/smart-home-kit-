#include "main.h"
#include "bsp_led.h"
#include "bsp_uart.h"




void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == BT_Pin){
		L_TOOGLE;
        user_debug("BT");
	}
	if(GPIO_Pin == TOUCH1_Pin)
	{
		L_TOOGLE;
	}
}

