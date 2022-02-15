#ifndef __MENU_H
#define __MENU_H

#include "stm32f4xx.h"
#include "OLED.h"

#define Funcition_number




typedef struct
{
     unsigned char current;
     unsigned char up;//�ϼ�
     unsigned char down;//�¼�
     unsigned char enter;//ȷ�ϼ�
     void (*current_operation)();
} key_table;



extern key_table table[10];
extern uint8_t func_index;

#endif /*__MENU_H */


