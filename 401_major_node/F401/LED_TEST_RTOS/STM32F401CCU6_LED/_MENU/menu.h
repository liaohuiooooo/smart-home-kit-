#ifndef __MENU_H
#define __MENU_H

#include "stm32f4xx.h"
#include "OLED.h"
#include "bsp_uart.h"
#include "string.h"
#include "stdio.h"
#define ARRAY_LEN 20

typedef struct
{
	uint8_t current;
	uint8_t up;//向上翻索引号
	uint8_t down;//向下翻索引号
	uint8_t enter;//确认索引号
	void (*current_operation)();
}key_table;



void Dis_Page(uint8_t * pStr[]);

extern uint8_t *myStrPtr[ARRAY_LEN];

#endif /*__MENU_H */


