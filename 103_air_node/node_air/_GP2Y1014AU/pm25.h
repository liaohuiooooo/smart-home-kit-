#ifndef __PM25_H
#define __PM25_H

#include "stm32f1xx.h"
#include "main.h"
#include "bsp_timer.h"
#include "adc.h"
#include "bsp_uart.h"

#define PLED_ON     HAL_GPIO_WritePin(PM_LED_GPIO_Port, PM_LED_Pin, GPIO_PIN_SET)
#define PLED_OFF     HAL_GPIO_WritePin(PM_LED_GPIO_Port, PM_LED_Pin, GPIO_PIN_RESET)


uint16_t GetGP2Y(void);

#endif
