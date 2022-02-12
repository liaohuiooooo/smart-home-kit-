#ifndef _LQOLED_H
#define _LQOLED_H

#include "main.h"
#include "stm32f4xx.h"
#include "stdlib.h"

//汉字大小，英文数字大小
#define 	TYPE8X16		1
#define 	TYPE16X16		2
#define 	TYPE6X8			3

//-----------------OLED端口定义----------------  					   

#define LCD_SCL_CLR()	HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_RESET)
#define LCD_SCL_SET()	HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_SET)

#define LCD_SDA_CLR()	HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_RESET)
#define LCD_SDA_SET()	HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_SET)

#define LCD_RST_CLR()	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET)
#define LCD_RST_SET()	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET)

#define LCD_DC_CLR()	HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET)
#define LCD_DC_SET()	HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET)

//CS管脚请接地

extern const uint8_t bmp[];
extern const uint8_t F16x16_Idx[];
extern const uint8_t F16x16[];
extern const uint8_t nonside[];


extern void LCD_Init(void);
extern void LCD_CLS(void);
extern void LCD_CLS_y(char y);
extern void LCD_CLS_line_area(uint8_t start_x,uint8_t start_y,uint8_t width);
extern void LCD_P6x8Str(uint8_t x,uint8_t y,uint8_t *ch,const uint8_t *F6x8);
extern void LCD_P8x16Str(uint8_t x,uint8_t y,uint8_t *ch,const uint8_t *F8x16);
extern void LCD_P14x16Str(uint8_t x,uint8_t y,uint8_t ch[],const uint8_t *F14x16_Idx,const uint8_t *F14x16);
extern void LCD_P16x16Str(uint8_t x,uint8_t y,uint8_t *ch,const uint8_t *F16x16_Idx,const uint8_t *F16x16);
//extern void LCD_Print(uint8_t x, uint8_t y, uint8_t *ch);
extern void LCD_PutPixel(uint8_t x,uint8_t y);
extern void LCD_Print(uint8_t x, uint8_t y, uint8_t *ch,uint8_t char_size, uint8_t ascii_size);
extern void LCD_Rectangle(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t gif);
extern void Draw_BMP(uint8_t x,uint8_t y,const uint8_t *bmp); 
extern void LCD_Fill(uint8_t dat);

#endif

