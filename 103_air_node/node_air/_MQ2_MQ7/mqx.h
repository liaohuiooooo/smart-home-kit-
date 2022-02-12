#ifndef __MQX_H
#define __MQX_H

#include "stm32f1xx.h"
#include "main.h"
#include "adc.h"
#include "bsp_uart.h"



float * AdcGetVal(void);
void showMQX(float *arr);

#endif
