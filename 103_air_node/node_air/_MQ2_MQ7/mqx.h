#ifndef __MQX_H
#define __MQX_H

#include "stm32f1xx.h"
#include "main.h"
#include "adc.h"
#include "bsp_uart.h"

#include "mqtt.h"


float * AdcGetVal(void);
void showMQX(float *arr);

#endif
