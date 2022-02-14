#include "menu.h"




uint8_t *myStrPtr[ARRAY_LEN] =        // char指针的数组
{ // 墨菲定律的几条推论：
	"第一行",
	"第二行",
	"第三行",
	"第四行",
	"显示温湿度"
};


void select_dis(uint8_t *pStr[], uint8_t number)
{
	uint8_t *origin = *myStrPtr;
	uint8_t *str;
	char *selected;
	char *s = "-->";
	switch(number){
		case 0: 
			*selected = origin[0];
			*pStr[0] = (uint8_t)strcat(s,selected);
			break;
	}
}

void Dis_Page(uint8_t *pStr[])
{
	LCD_Print(0, 0, &(*pStr[0]),TYPE16X16,TYPE8X16); 
    LCD_Print(0, 16, &(*pStr[1]),TYPE16X16,TYPE8X16); 
	LCD_Print(0, 32, &(*pStr[2]),TYPE16X16,TYPE8X16); 
	LCD_Print(0, 48, &(*pStr[4]),TYPE16X16,TYPE8X16);  
}


/**
 * @brief 
 * 
 */
void fun1(void)
{  
    
}

/**
 * @brief 
 * 
 */
void fun2(void)
{  
    
}

/**
 * @brief 
 * 
 */
void fun3(void)
{  
    
}

/**
 * @brief 
 * 
 */
void fun4(void)
{  
    
}

/**
 * @brief 
 * 
 */
key_table  table[4]=
{
	{0,3,1,4,(*fun1)},//第一层，显示【清华大学】、北京大学、重庆三峡学院、返回
	{1,0,2,8,(*fun2)},//第一层，显示清华大学、【北京大学】、重庆三峡学院、返回	
	{2,1,3,12,(*fun3)},//第一层，显示清华大学、北京大学、【重庆三峡学院】、返回
	{3,2,0,25,(*fun4)},//第一层，显示清华大学、北京大学、重庆三峡学院、【返回】
};


