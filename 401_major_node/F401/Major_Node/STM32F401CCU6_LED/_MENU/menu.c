#include "menu.h"


uint8_t func_index = 0;

#define LINE1 0
#define LINE2 16
#define LINE3 32
#define LINE4 48

void fun1()
{
	LCD_CLS();
    LCD_Print(0, LINE1, "-->主页",TYPE16X16,TYPE8X16);
	LCD_Print(0, LINE2, "第一行",TYPE16X16,TYPE8X16);
	LCD_Print(0, LINE3, "第二行",TYPE16X16,TYPE8X16);
	LCD_Print(0, LINE4, "第三行",TYPE16X16,TYPE8X16);
}

void fun2()
{
	LCD_CLS();
    LCD_Print(0, LINE1, "主页",TYPE16X16,TYPE8X16);
	LCD_Print(0,LINE2, "-->第一行",TYPE16X16,TYPE8X16);
	LCD_Print(0, LINE3, "第二行",TYPE16X16,TYPE8X16);
	LCD_Print(0, LINE4, "第三行",TYPE16X16,TYPE8X16);
}

void fun3()
{
	LCD_CLS();
    LCD_Print(0, LINE1, "主页",TYPE16X16,TYPE8X16);
	LCD_Print(0, LINE2, "第一行",TYPE16X16,TYPE8X16);
	LCD_Print(0, LINE3, "-->第二行",TYPE16X16,TYPE8X16);
	LCD_Print(0, LINE4, "第三行",TYPE16X16,TYPE8X16);
}

void fun4()
{
	LCD_CLS();
    LCD_Print(0, LINE1, "主页",TYPE16X16,TYPE8X16);
	LCD_Print(0, LINE2, "第一行",TYPE16X16,TYPE8X16);
	LCD_Print(0, LINE3, "第二行",TYPE16X16,TYPE8X16);
	LCD_Print(0, LINE4, "-->第三行",TYPE16X16,TYPE8X16);
}
void fun5()
{
	LCD_CLS();
	LCD_Print(0, LINE1, "1",TYPE16X16,TYPE8X16);
	Draw_BMP(0,0,bmp1);
}



key_table table[10] =
{
	{0,3,1,4,(*fun1)},
	{1,0,2,9,(*fun2)},
	{2,1,3,9,(*fun3)},   
	{3,2,0,9,(*fun4)},  

	{4,4,4,4,(*fun5)},                                               
};



