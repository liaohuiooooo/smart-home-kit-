/**
 * @file            dev_cog12864.c
 * @brief           COG LCD����
 * @author          wujique
 * @date            2018��1��10�� ������
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2018��1��10�� ������
 *   ��    ��:         �ݼ�ȸ������
 *   �޸�����:   �����ļ�
		��Ȩ˵����
		1 Դ����ݼ�ȸ���������С�
		2 �������ڵ�������ҵ��;�����׿��������۳��⣩��������Ȩ��
		3 �ݼ�ȸ�����Ҳ��Դ��빦�����κα�֤����ʹ�������в��ԣ�����Ը���
		4 �������޸�Դ�벢�ַ���������ֱ�����۱�������������ұ�����Ȩ˵����
		5 �緢��BUG�����Ż�����ӭ�������¡�����ϵ��code@wujique.com
		6 ʹ�ñ�Դ�����൱����ͬ����Ȩ˵����
		7 ���ַ����Ȩ��������ϵ��code@wujique.com
		8 һ�н���Ȩ���ݼ�ȸ���������С�
*/
/*

	COG LCD ������

*/


#include <stdarg.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "main.h"
#include "wujique_log.h"
#include "wujique_sysconf.h"
#include "alloc.h"
#include "dev_lcdbus.h"
#include "dev_lcd.h"
#include "dev_str7565.h"


/*
	����ʹ�õ����ݽṹ��������
*/
struct _cog_drv_data
{
	u8 gram[8][128];

	/*ˢ������*/
	u16 sx;
	u16 ex;
	u16 sy;
	u16 ey;
	u16 disx;
	u16 disy;

};	


#ifdef TFT_LCD_DRIVER_COG12864

s32 drv_ST7565_init(DevLcdNode *lcd);
static s32 drv_ST7565_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color);
s32 drv_ST7565_color_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 color);
s32 drv_ST7565_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color);
static s32 drv_ST7565_display_onoff(DevLcdNode *lcd, u8 sta);
s32 drv_ST7565_prepare_display(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey);
static void drv_ST7565_scan_dir(DevLcdNode *lcd, u8 dir);
void drv_ST7565_lcd_bl(DevLcdNode *lcd, u8 sta);
s32 drv_ST7565_flush(DevLcdNode *lcd, u16 *color, u32 len);


/*

	����һ��TFT LCD��ʹ��ST7565����IC���豸

*/
_lcd_drv CogLcdST7565Drv = {
							.id = 0X7565,

							.init = drv_ST7565_init,
							.draw_point = drv_ST7565_drawpoint,
							.color_fill = drv_ST7565_color_fill,
							.fill = drv_ST7565_fill,
							.onoff = drv_ST7565_display_onoff,
							.prepare_display = drv_ST7565_prepare_display,
							.set_dir = drv_ST7565_scan_dir,
							.backlight = drv_ST7565_lcd_bl,
							.flush = drv_ST7565_flush
							};

void drv_ST7565_lcd_bl(DevLcdNode *lcd, u8 sta)
{
	DevLcdBusNode * node;
	
	node = bus_lcd_open(lcd->dev.buslcd);
	bus_lcd_bl(node, sta);
	bus_lcd_close(node);
}
	
/**
 *@brief:      drv_ST7565_scan_dir
 *@details:    �����Դ�ɨ�跽�� ������Ϊ�����Ƕ�
 *@param[in]   u8 dir  
 *@param[out]  ��
 *@retval:     static
 */
static void drv_ST7565_scan_dir(DevLcdNode *lcd, u8 dir)
{
	return;
}

/**
 *@brief:      drv_ST7565_set_cp_addr
 *@details:    ���ÿ����������е�ַ��Χ
 *@param[in]   u16 sc  
               u16 ec  
               u16 sp  
               u16 ep  
 *@param[out]  ��
 *@retval:     
 */
#if 0
static s32 drv_ST7565_set_cp_addr(DevLcd *lcd, u16 sc, u16 ec, u16 sp, u16 ep)
{
	return 0;
}
#endif
/**
 *@brief:      drv_ST7565_refresh_gram
 *@details:       ˢ��ָ��������Ļ��
                  �����Ǻ���ģʽ����
 *@param[in]   u16 sc  
               u16 ec  
               u16 sp  
               u16 ep  
 *@param[out]  ��
 *@retval:     static
 */
static s32 drv_ST7565_refresh_gram(DevLcdNode *lcd, u16 sc, u16 ec, u16 sp, u16 ep)
{	
	struct _cog_drv_data *drvdata; 
	u8 i;
	DevLcdBusNode *node;
	
	node = bus_lcd_open(lcd->dev.buslcd);

	//uart_printf("drv_ST7565_refresh:%d,%d,%d,%d\r\n", sc,ec,sp,ep);
	drvdata = (struct _cog_drv_data *)lcd->pri;
	
    for(i=sp/8; i <= ep/8; i++)
    {
        bus_lcd_write_cmd (node, 0xb0+i);    //����ҳ��ַ��0~7��
        bus_lcd_write_cmd(node, ((sc>>4)&0x0f)+0x10);      //������ʾλ�á��иߵ�ַ
        bus_lcd_write_cmd(node, sc&0x0f);      //������ʾλ�á��е͵�ַ

         bus_lcd_write_data(node, &(drvdata->gram[i][sc]), ec-sc+1);

	}
	bus_lcd_close(node);
	
	return 0;
}

/**
 *@brief:      drv_ST7565_display_onoff
 *@details:    ��ʾ��ر�
 *@param[in]   u8 sta  
 *@param[out]  ��
 *@retval:     static
 */
static s32 drv_ST7565_display_onoff(DevLcdNode *lcd, u8 sta)
{
	DevLcdBusNode * node;
	
	node = bus_lcd_open(lcd->dev.buslcd);

	if(sta == 1)
	{
		bus_lcd_write_cmd (node, 0XCF);  //DISPLAY ON
	}
	else
	{
		bus_lcd_write_cmd (node, 0XCE);  //DISPLAY OFF	
	}
	bus_lcd_close(node);
	return 0;
}

/**
 *@brief:      drv_ST7565_init
 *@details:    
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ST7565_init(DevLcdNode *lcd)
{
	DevLcdBusNode * node;
	
	node = bus_lcd_open(lcd->dev.buslcd);

	bus_lcd_rst(node, 1);
	Delay(50);
	bus_lcd_rst(node, 0);
	Delay(50);
	bus_lcd_rst(node, 1);
	Delay(50);
	
	bus_lcd_write_cmd (node, 0xe2);//��λ
	Delay(50);
	bus_lcd_write_cmd (node, 0x2c);//��ѹ����1
	Delay(50);
	bus_lcd_write_cmd (node, 0x2e);//��ѹ����2
	Delay(50);
	bus_lcd_write_cmd (node, 0x2f);//��ѹ����3
	Delay(50);
	
	bus_lcd_write_cmd (node, 0x24);//�Աȶȴֵ�����Χ0X20��0X27
	bus_lcd_write_cmd (node, 0x81);//�Աȶ�΢��
	bus_lcd_write_cmd (node, 0x25);//�Աȶ�΢��ֵ 0x00-0x3f
	
	bus_lcd_write_cmd (node, 0xa2);// ƫѹ��
	bus_lcd_write_cmd (node, 0xc8);//��ɨ�裬���ϵ���
	bus_lcd_write_cmd (node, 0xa0);//��ɨ�裬������
	bus_lcd_write_cmd (node, 0x40);//��ʼ�У���һ��
	bus_lcd_write_cmd (node, 0xaf);//����ʾ

	bus_lcd_close(node);
	
	wjq_log(LOG_INFO, "drv_ST7565_init finish\r\n");

	/*�����Դ棬�����ͷ�*/
	lcd->pri = (void *)wjq_malloc(sizeof(struct _cog_drv_data));
	memset((char*)lcd->pri, 0x00, 128*8);//Ҫ��Ϊ��̬�ж��Դ��С
	
	//drv_ST7565_refresh_gram(lcd, 0,127,0,63);

	return 0;
}

/**
 *@brief:      drv_ST7565_xy2cp
 *@details:    ��xy����ת��ΪCP����
 			   ����COG�ڰ�����˵��CP������Ǻ������꣬
 			   ת������CP���껹�Ǻ������꣬
 			   Ҳ����˵������ģʽ������Ҫ��XY����תCP����
 			   ��������Ҫת��
 *@param[in]   ��
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ST7565_xy2cp(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey, u16 *sc, u16 *ec, u16 *sp, u16 *ep)
{

	return 0;
}
/**
 *@brief:      drv_ST7565_drawpoint
 *@details:    ����
 *@param[in]   u16 x      
               u16 y      
               u16 color  
 *@param[out]  ��
 *@retval:     static
 */
static s32 drv_ST7565_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color)
{
	u16 xtmp,ytmp;
	u16 page, colum;

	struct _cog_drv_data *drvdata;
	
	DevLcdBusNode * node;
	
	drvdata = (struct _cog_drv_data *)lcd->pri;

	if(x > lcd->width)
		return -1;
	if(y > lcd->height)
		return -1;

	if(lcd->dir == W_LCD)
	{
		xtmp = x;
		ytmp = y;
	}
	else//�����������XY����Դ��ӳ��Ҫ�Ե�
	{
		xtmp = y;
		ytmp = lcd->width-1-x;
	}
	
	page = ytmp/8; //ҳ��ַ
	colum = xtmp;//�е�ַ
	
	if(color == BLACK)
	{
		drvdata->gram[page][colum] |= (0x01<<(ytmp%8));
	}
	else
	{
		drvdata->gram[page][colum] &= ~(0x01<<(ytmp%8));
	}

	/*Ч�ʲ���*/
	node = bus_lcd_open(lcd->dev.buslcd);
    bus_lcd_write_cmd (node, 0xb0 + page );   
    bus_lcd_write_cmd (node, ((colum>>4)&0x0f)+0x10); 
    bus_lcd_write_cmd (node, colum&0x0f);    
    bus_lcd_write_data (node, &(drvdata->gram[page][colum]), 1);
	bus_lcd_close (node);
	return 0;
}
/**
 *@brief:      drv_ST7565_color_fill
 *@details:    ��һ�������趨Ϊĳ����ɫ
 *@param[in]   u16 sx     
               u16 sy     
               u16 ex     
               u16 ey     
               u16 color  
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ST7565_color_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 color)
{
	u16 i,j;
	u16 xtmp,ytmp;
	u16 page, colum;

	
	struct _cog_drv_data *drvdata;

	//uart_printf("drv_ST7565_fill:%d,%d,%d,%d\r\n", sx,ex,sy,ey);

	drvdata = (struct _cog_drv_data *)lcd->pri;

	/*��ֹ�������*/
	if(sy >= lcd->height)
	{
		sy = lcd->height-1;
	}
	if(sx >= lcd->width)
	{
		sx = lcd->width-1;
	}
	
	if(ey >= lcd->height)
	{
		ey = lcd->height-1;
	}
	if(ex >= lcd->width)
	{
		ex = lcd->width-1;
	}
	
	for(j=sy;j<=ey;j++)
	{
		//uart_printf("\r\n");
		
		for(i=sx;i<=ex;i++)
		{

			if(lcd->dir == W_LCD)
			{
				xtmp = i;
				ytmp = j;
			}
			else//�����������XY����Դ��ӳ��Ҫ�Ե�
			{
				xtmp = j;
				ytmp = lcd->width-1-i;
			}

			page = ytmp/8; //ҳ��ַ
			colum = xtmp;//�е�ַ
			
			if(color == BLACK)
			{
				drvdata->gram[page][colum] |= (0x01<<(ytmp%8));
				//uart_printf("*");
			}
			else
			{
				drvdata->gram[page][colum] &= ~(0x01<<(ytmp%8));
				//uart_printf("-");
			}
		}
	}

	/*
		ֻˢ����Ҫˢ�µ�����
		���귶Χ�Ǻ���ģʽ
	*/
	if(lcd->dir == W_LCD)
	{
		drv_ST7565_refresh_gram(lcd, sx,ex,sy,ey);
	}
	else
	{
		drv_ST7565_refresh_gram(lcd, sy, ey, lcd->width-ex-1, lcd->width-sx-1); 	
	}
		
	return 0;
}


/**
 *@brief:      drv_ST7565_color_fill
 *@details:    ����������
 *@param[in]   u16 sx      
               u16 sy      
               u16 ex      
               u16 ey      
               u16 *color  ÿһ�������ɫ����
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ST7565_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color)
{
	u16 i,j;
	u16 xtmp,ytmp;
	u16 xlen,ylen;
	u16 page, colum;
	u32 index;
	
	struct _cog_drv_data *drvdata;

	//uart_printf("drv_ST7565_fill:%d,%d,%d,%d\r\n", sx,ex,sy,ey);

	drvdata = (struct _cog_drv_data *)lcd->pri;

	/*xlen��ylen������ȡ���ݵģ�������LCD*/
	xlen = ex-sx+1;//ȫ����
	ylen = ey-sy+1;

	/*��ֹ�������*/
	if(sy >= lcd->height)
	{
		sy = lcd->height-1;
	}
	if(sx >= lcd->width)
	{
		sx = lcd->width-1;
	}
	
	if(ey >= lcd->height)
	{
		ey = lcd->height-1;
	}
	if(ex >= lcd->width)
	{
		ex = lcd->width-1;
	}
	
	for(j=sy;j<=ey;j++)
	{
		//uart_printf("\r\n");

		index = (j-sy)*xlen;
		
		for(i=sx;i<=ex;i++)
		{

			if(lcd->dir == W_LCD)
			{
				xtmp = i;
				ytmp = j;
			}
			else//�����������XY����Դ��ӳ��Ҫ�Ե�
			{
				xtmp = j;
				ytmp = lcd->width-1-i;
			}

			page = ytmp/8; //ҳ��ַ
			colum = xtmp;//�е�ַ
			
			if(*(color+index+i-sx) == BLACK)
			{
				drvdata->gram[page][colum] |= (0x01<<(ytmp%8));
				//uart_printf("*");
			}
			else
			{
				drvdata->gram[page][colum] &= ~(0x01<<(ytmp%8));
				//uart_printf("-");
			}
		}
	}

	/*
		ֻˢ����Ҫˢ�µ�����
		���귶Χ�Ǻ���ģʽ
	*/
	if(lcd->dir == W_LCD)
	{
		drv_ST7565_refresh_gram(lcd, sx,ex,sy,ey);
	}
	else
	{

		drv_ST7565_refresh_gram(lcd, sy, ey, lcd->width-ex-1, lcd->width-sx-1); 	
	}
	//uart_printf("refresh ok\r\n");		
	return 0;
}

s32 drv_ST7565_prepare_display(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey)
{
	struct _cog_drv_data *drvdata;
	drvdata = (struct _cog_drv_data *)lcd->pri;

	drvdata->ex = ex;
	drvdata->sx = sx;
	drvdata->sy = sy;
	drvdata->ey = ey;

	drvdata->disx = sx;
	drvdata->disy = sy;
	
	return 0;
}

s32 drv_ST7565_flush(DevLcdNode *lcd, u16 *color, u32 len)
{
	u16 i,j;
	u16 xtmp,ytmp;
	u16 page, colum;
	u32 index;
	
	u16 sx,ex,sy,ey;
	
	struct _cog_drv_data *drvdata;

	//uart_printf("drv_ST7565_fill:%d,%d,%d,%d\r\n", sx,ex,sy,ey);

	drvdata = (struct _cog_drv_data *)lcd->pri;
	
	sx = lcd->width;
	ex = 0;
	sy = lcd->height;
	ey = 0;

	index = 0;
	while(1)
	{
		if(index >= len)
			break;

		if((drvdata->disx < lcd->width)&&(drvdata->disy < lcd->height))
		{			
			if(sx >drvdata->disx)
				sx = drvdata->disx;

		
			if(ex < drvdata->disx)
				ex = drvdata->disx;

			if(sy >drvdata->disy)
				sy = drvdata->disy;

			if(ey < drvdata->disy)
				ey = drvdata->disy;

			if(lcd->dir == W_LCD)
			{
				xtmp = drvdata->disx;
				ytmp = drvdata->disy;
			}
			else//�����������XY����Դ��ӳ��Ҫ�Ե�
			{
				xtmp = drvdata->disy;
				ytmp = lcd->width-drvdata->disx;
			}

			//wjq_log(LOG_DEBUG, " %d, %d \r\n", xtmp,ytmp);
			
			page = ytmp/8; //ҳ��ַ
			colum = xtmp;//�е�ַ
				
			if(*(color+index) == BLACK)
			{
				drvdata->gram[page][colum] |= (0x01<<(ytmp%8));
				//uart_printf("*");
			}
			else
			{
				drvdata->gram[page][colum] &= ~(0x01<<(ytmp%8));
				//uart_printf("-");
			}
		}

		drvdata->disx++;
		if(drvdata->disx > drvdata->ex)
		{
			drvdata->disx = drvdata->sx;
			drvdata->disy++;
			
			if(drvdata->disy> drvdata->ey)
				drvdata->disy = drvdata->sy;
		}
		index++;
		
	}

	/*
		ֻˢ����Ҫˢ�µ�����
		���귶Χ�Ǻ���ģʽ
	*/
	if(lcd->dir == W_LCD)
	{
		drv_ST7565_refresh_gram(lcd, sx,ex,sy,ey);
	}
	else
	{

		drv_ST7565_refresh_gram(lcd, sy, ey, lcd->width-ex-1, lcd->width-sx-1); 	
	}
	//uart_printf("refresh ok\r\n");		
	return 0;
} 

#endif


/*

	OLED �� COG LCD ��������
	������ʼ����һ��
	OLED������������SSD1315��SSD1615��һ���ġ�
*/
#ifdef TFT_LCD_DRIVER_SSD1615

/**
 *@brief:	   drv_ssd1615_init
 *@details:    
 *@param[in]   void  
 *@param[out]  ��
 *@retval:	   
 */
s32 drv_ssd1615_init(DevLcdNode *lcd)
{
	DevLcdBusNode * node;
	
	node = bus_lcd_open(lcd->dev.buslcd);

	bus_lcd_rst(node, 1);
	Delay(50);
	bus_lcd_rst(node, 0);
	Delay(50);
	bus_lcd_rst(node, 1);
	Delay(50);
	
	bus_lcd_write_cmd (node, 0xAE);//--turn off oled panel
	bus_lcd_write_cmd (node, 0x00);//---set low column address
	bus_lcd_write_cmd (node, 0x10);//---set high column address
	bus_lcd_write_cmd (node, 0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	bus_lcd_write_cmd (node, 0x81);//--set contrast control register
	bus_lcd_write_cmd (node, 0xCF); // Set SEG Output Current Brightness
	bus_lcd_write_cmd (node, 0xA1);//--Set SEG/Column Mapping	  0xa0���ҷ��� 0xa1����
	bus_lcd_write_cmd (node, 0xC8);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
	bus_lcd_write_cmd (node, 0xA6);//--set normal display
	bus_lcd_write_cmd (node, 0xA8);//--set multiplex ratio(1 to 64)
	bus_lcd_write_cmd (node, 0x3f);//--1/64 duty
	bus_lcd_write_cmd (node, 0xD3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	bus_lcd_write_cmd (node, 0x00);//-not offset
	bus_lcd_write_cmd (node, 0xd5);//--set display clock divide ratio/oscillator frequency
	bus_lcd_write_cmd (node, 0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	bus_lcd_write_cmd (node, 0xD9);//--set pre-charge period
	bus_lcd_write_cmd (node, 0xF1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	bus_lcd_write_cmd (node, 0xDA);//--set com pins hardware configuration
	bus_lcd_write_cmd (node, 0x12);
	bus_lcd_write_cmd (node, 0xDB);//--set vcomh
	bus_lcd_write_cmd (node, 0x40);//Set VCOM Deselect Level
	bus_lcd_write_cmd (node, 0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	bus_lcd_write_cmd (node, 0x02);//
	bus_lcd_write_cmd (node, 0x8D);//--set Charge Pump enable/disable
	bus_lcd_write_cmd (node, 0x14);//--set(0x10) disable
	bus_lcd_write_cmd (node, 0xA4);// Disable Entire Display On (0xa4/0xa5)
	bus_lcd_write_cmd (node, 0xA6);// Disable Inverse Display On (0xa6/a7) 
	bus_lcd_write_cmd (node, 0xAF);//--turn on oled panel

	bus_lcd_write_cmd (node, 0xAF);//--turn on oled panel 
	bus_lcd_close (node);
	

	lcd->pri = (void *)wjq_malloc(sizeof(struct _cog_drv_data));
	memset((char*)lcd->pri, 0x00, 128*8);//Ҫ��Ϊ��̬�ж��Դ��С
	
	//drv_ST7565_refresh_gram(lcd, 0,127,0,63);
	
	wjq_log(LOG_INFO, "dev_ssd1615_init finish\r\n");
	return 0;
}

/**
 *@brief:      drv_ssd1615_display_onoff
 *@details:    SSD1615�򿪻�ر���ʾ
 *@param[in]   DevLcd *lcd  
               u8 sta       
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ssd1615_display_onoff(DevLcdNode *lcd, u8 sta)
{
	DevLcdBusNode * node;
	
	node = bus_lcd_open(lcd->dev.buslcd);

	if(sta == 1)
	{
    	bus_lcd_write_cmd (node, 0X8D);  //SET DCDC����
    	bus_lcd_write_cmd (node, 0X14);  //DCDC ON
    	bus_lcd_write_cmd (node, 0XAF);  //DISPLAY ON
	}
	else
	{
		bus_lcd_write_cmd (node, 0X8D);  //SET DCDC����
    	bus_lcd_write_cmd (node, 0X10);  //DCDC OFF
    	bus_lcd_write_cmd (node, 0XAE);  //DISPLAY OFF	
	}
	bus_lcd_close (node);
	
	return 0;
}

_lcd_drv OledLcdSSD1615rv = {
							.id = 0X1315,

							.init = drv_ssd1615_init,
							.draw_point = drv_ST7565_drawpoint,
							.color_fill = drv_ST7565_color_fill,
							.fill = drv_ST7565_fill,
							.onoff = drv_ssd1615_display_onoff,
							.prepare_display = drv_ST7565_prepare_display,
							.set_dir = drv_ST7565_scan_dir,
							.backlight = drv_ST7565_lcd_bl,
							.flush = drv_ST7565_flush
							};
#endif

