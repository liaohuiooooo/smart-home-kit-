#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "stm32f4xx.h"
#include "main.h"

#define L_ON 		HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin, GPIO_PIN_RESET);		
#define L_OFF 		HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_SET)
#define L_TOOGLE		HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin)
	





#endif /*__BSP_LED_H */
