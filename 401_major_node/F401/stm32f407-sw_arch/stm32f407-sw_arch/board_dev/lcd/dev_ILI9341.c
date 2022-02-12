/**
 * @file            dev_ILI9341.c
 * @brief           TFT LCD ����оƬILI6341��������
 * @author          wujique
 * @date            2017��11��8�� ������
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2017��11��8�� ������
 *   ��    ��:         wujique
 *   �޸�����:   �����ļ�
       	1 Դ����ݼ�ȸ���������С�
		2 �������ڵ�������ҵ��;�����׿��������۳��⣩��������Ȩ��
		3 �ݼ�ȸ�����Ҳ��Դ��빦�����κα�֤����ʹ�������в��ԣ�����Ը���
		4 �������޸�Դ�벢�ַ���������ֱ�����۱���������������뱣��WUJIQUE��Ȩ˵����
		5 �緢��BUG�����Ż�����ӭ�������¡�����ϵ��code@wujique.com
		6 ʹ�ñ�Դ�����൱����ͬ����Ȩ˵����
		7 ���ַ����Ȩ��������ϵ��code@wujique.com
		8 һ�н���Ȩ���ݼ�ȸ���������С�
*/
#include "stdlib.h"
#include "string.h"

#include "stm32f4xx.h"

#include "wujique_sysconf.h"

#include "stm324xg_eval_fsmc_sram.h"
#include "alloc.h"
#include "wujique_log.h"
#include "dev_lcdbus.h"
#include "dev_lcd.h"
#include "dev_ILI9341.h"

#define DEV_ILI9341_DEBUG

#ifdef DEV_ILI9341_DEBUG
#define ILI9341_DEBUG	wjq_log 
#else
#define ILI9341_DEBUG(a, ...)
#endif

extern void Delay(__IO uint32_t nTime);

/*

	9341����

*/
#ifdef TFT_LCD_DRIVER_9341
/*9341�����*/
#define ILI9341_CMD_WRAM 0x2c
#define ILI9341_CMD_SETX 0x2a
#define ILI9341_CMD_SETY 0x2b

s32 drv_ILI9341_init(DevLcdNode *lcd);
static s32 drv_ILI9341_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color);
s32 drv_ILI9341_color_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 color);
s32 drv_ILI9341_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color);
static s32 drv_ILI9341_display_onoff(DevLcdNode *lcd, u8 sta);
s32 drv_ILI9341_prepare_display(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey);
static void drv_ILI9341_scan_dir(DevLcdNode *lcd, u8 dir);
void drv_ILI9341_lcd_bl(DevLcdNode *lcd, u8 sta);
s32 drv_ILI9341_flush(DevLcdNode *lcd, u16 *color, u32 len);


/*

	����һ��TFT LCD��ʹ��ILI9341����IC���豸

*/
_lcd_drv TftLcdILI9341Drv = {
							.id = 0X9341,

							.init = drv_ILI9341_init,
							.draw_point = drv_ILI9341_drawpoint,
							.color_fill = drv_ILI9341_color_fill,
							.fill = drv_ILI9341_fill,
							.onoff = drv_ILI9341_display_onoff,
							.prepare_display = drv_ILI9341_prepare_display,
							.flush = drv_ILI9341_flush,
							.set_dir = drv_ILI9341_scan_dir,
							.backlight = drv_ILI9341_lcd_bl
							};

void drv_ILI9341_lcd_bl(DevLcdNode *lcd, u8 sta)
{
	DevLcdBusNode * node;
	
	node = bus_lcd_open(lcd->dev.buslcd);
	bus_lcd_bl(node, sta);
	bus_lcd_close(node);

}
	
/**
 *@brief:      drv_ILI9341_scan_dir
 *@details:    �����Դ�ɨ�跽�� ������Ϊ�����Ƕ�
 *@param[in]   u8 dir  
 *@param[out]  ��
 *@retval:     static
 */
static void drv_ILI9341_scan_dir(DevLcdNode *lcd, u8 dir)
{
	u16 regval=0;

	/*���ô���ߵ��ұ߻����ұߵ����*/
	switch(dir)
	{
		case R2L_U2D:
		case R2L_D2U:
		case U2D_R2L:
		case D2U_R2L:
			regval|=(1<<6); 
			break;	 
	}

	/*���ô��ϵ��»��Ǵ��µ���*/
	switch(dir)
	{
		case L2R_D2U:
		case R2L_D2U:
		case D2U_L2R:
		case D2U_R2L:
			regval|=(1<<7); 
			break;	 
	}

	/*
		���������һ��������� Reverse Mode
		�������Ϊ1��LCD�������Ѿ����и��жԵ��ˣ�
		�����Ҫ����ʾ�н��е���
	*/
	switch(dir)
	{
		case U2D_L2R:
		case D2U_L2R:
		case U2D_R2L:
		case D2U_R2L:
			regval|=(1<<5);
			break;	 
	}
	/*
		����������RGB����GBR
		���������õ�ת����
	*/	
	regval|=(1<<3);//0:GBR,1:RGB  ��R61408�෴

	DevLcdBusNode * node;
	node = bus_lcd_open(lcd->dev.buslcd);
	
	bus_lcd_write_cmd(node, (0x36));
	u16 tmp[2];
	tmp[0] = regval;
	bus_lcd_write_data(node, (u8*)tmp, 1);
	bus_lcd_close(node);

}

/**
 *@brief:      drv_ILI9341_set_cp_addr
 *@details:    ���ÿ����������е�ַ��Χ
 *@param[in]   u16 sc  
               u16 ec  
               u16 sp  
               u16 ep  
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ILI9341_set_cp_addr(DevLcdNode *lcd, u16 sc, u16 ec, u16 sp, u16 ep)
{

	DevLcdBusNode * node;
	u16 tmp[4];

	node = bus_lcd_open(lcd->dev.buslcd);

	bus_lcd_write_cmd(node, ILI9341_CMD_SETX);
	tmp[0] = (sc>>8);
	tmp[1] = (sc&0XFF);
	tmp[2] = (ec>>8);
	tmp[3] = (ec&0XFF);
	bus_lcd_write_data(node, (u8*)tmp, 4);

	bus_lcd_write_cmd(node, (ILI9341_CMD_SETY));
	tmp[0] = (sp>>8);
	tmp[1] = (sp&0XFF);
	tmp[2] = (ep>>8);
	tmp[3] = (ep&0XFF);
	bus_lcd_write_data(node, (u8*)tmp, 4);

	bus_lcd_write_cmd(node, (ILI9341_CMD_WRAM));
	
	bus_lcd_close(node);
	
	return 0;
}

/**
 *@brief:      drv_ILI9341_display_onoff
 *@details:    ��ʾ��ر�
 *@param[in]   u8 sta  
 *@param[out]  ��
 *@retval:     static
 */
static s32 drv_ILI9341_display_onoff(DevLcdNode *lcd, u8 sta)
{
	DevLcdBusNode * node;
	node = bus_lcd_open(lcd->dev.buslcd);
	
	if(sta == 1)
		bus_lcd_write_cmd(node, (0x29));
	else
		bus_lcd_write_cmd(node, (0x28));

	bus_lcd_close(node);
	
	return 0;
}

/**
 *@brief:      drv_ILI9341_init
 *@details:    ��ʼ��FSMC�����Ҷ�ȡILI9341���豸ID
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ILI9341_init(DevLcdNode *lcd)
{
	u16 data;
	DevLcdBusNode * node;
	u16 tmp[16];
	
	node = bus_lcd_open(lcd->dev.buslcd);

	bus_lcd_rst(node, 1);
	Delay(50);
	bus_lcd_rst(node, 0);
	Delay(50);
	bus_lcd_rst(node, 1);
	Delay(50);

	bus_lcd_write_cmd(node, (0x00d3));
	/*��4���ֽڣ���һ���ֽ���dummy read�� �ڶ��ֽ���0x00�� �����ֽ���93�������ֽ���41*/
	bus_lcd_read_data(node, (u8*)tmp, 4);
	
	data = tmp[2]; 
	data<<=8;
	data |= tmp[3];

	ILI9341_DEBUG(LOG_DEBUG, "read reg:%04x\r\n", data);

	if(data != TftLcdILI9341Drv.id)
	{
		ILI9341_DEBUG(LOG_DEBUG, "lcd drive no 9341\r\n");	
		bus_lcd_close(node);
		return -1;
	}

	bus_lcd_write_cmd(node, (0xCF));//Power control B
	tmp[0] = 0x00;
	tmp[1] = 0xC1;
	tmp[2] = 0x30;
	bus_lcd_write_data(node, (u8*)tmp, 3);
	

	bus_lcd_write_cmd(node, (0xED));//Power on sequence control 
	tmp[0] = 0x64;
	tmp[1] = 0x03;
	tmp[2] = 0x12;
	tmp[3] = 0x81;
	bus_lcd_write_data(node, (u8*)tmp, 4);

	bus_lcd_write_cmd(node, (0xE8));//Driver timing control A
	tmp[0] = 0x85;
	//tmp[1] = 0x01;
	tmp[1] = 0x10;
	tmp[2] = 0x7A;
	bus_lcd_write_data(node, (u8*)tmp, 3);

	bus_lcd_write_cmd(node, (0xCB));//Power control 
	tmp[0] = 0x39;
	tmp[1] = 0x2C;
	tmp[2] = 0x00;
	tmp[3] = 0x34;
	tmp[4] = 0x02;
	bus_lcd_write_data(node, (u8*)tmp, 5);

	bus_lcd_write_cmd(node, (0xF7));//Pump ratio control
	tmp[0] = 0x20;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0xEA));//Driver timing control
	tmp[0] = 0x00;
	tmp[1] = 0x00;
	bus_lcd_write_data(node, (u8*)tmp, 2);

	bus_lcd_write_cmd(node, (0xC0));
	tmp[0] = 0x1B;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0xC1));
	tmp[0] = 0x01;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0xC5));
	tmp[0] = 0x30;
	tmp[1] = 0x30;
	bus_lcd_write_data(node, (u8*)tmp, 2);

	bus_lcd_write_cmd(node, (0xC7));
	tmp[0] = 0xB7;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x36));
	tmp[0] = 0x48;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x3A));
	tmp[0] = 0x55;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0xB1));
	tmp[0] = 0x00;
	tmp[1] = 0x1A;
	bus_lcd_write_data(node, (u8*)tmp, 2);

	bus_lcd_write_cmd(node, (0xB6));
	tmp[0] = 0x0A;
	tmp[1] = 0xA2;
	bus_lcd_write_data(node, (u8*)tmp, 2);

	bus_lcd_write_cmd(node, (0xF2));
	tmp[0] = 0x00;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x26));
	tmp[0] = 0x01;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0xE0));
	tmp[0] = 0x0F; tmp[1] = 0x2A; tmp[2] = 0x28; tmp[3] = 0x08;
	tmp[4] = 0x0E; tmp[5] = 0x08; tmp[6] = 0x54; tmp[7] = 0xa9;
	tmp[8] = 0x43; tmp[9] = 0x0a; tmp[10] = 0x0F; tmp[11] = 0x00;
	tmp[12] = 0x00; tmp[13] = 0x00; tmp[14] = 0x00;
	bus_lcd_write_data(node, (u8*)tmp, 15);

	bus_lcd_write_cmd(node, (0XE1));
	tmp[0] = 0x00; tmp[1] = 0x15; tmp[2] = 0x17; tmp[3] = 0x07;
	tmp[4] = 0x11; tmp[5] = 0x06; tmp[6] = 0x2B; tmp[7] = 0x56;
	tmp[8] = 0x3C; tmp[9] = 0x05; tmp[10] = 0x10; tmp[11] = 0x0F;
	tmp[12] = 0x3F; tmp[13] = 0x3F; tmp[14] = 0x0F;
	bus_lcd_write_data(node, (u8*)tmp, 15);	

	bus_lcd_write_cmd(node, (0x2B));
	tmp[0] = 0x00; tmp[1] = 0x00; tmp[2] = 0x01; tmp[3] = 0x3f;
	bus_lcd_write_data(node, (u8*)tmp, 4);

	bus_lcd_write_cmd(node, (0x2A));
	tmp[0] = 0x00; tmp[1] = 0x00; tmp[2] = 0x00; tmp[3] = 0xef;
	bus_lcd_write_data(node, (u8*)tmp, 4);

	bus_lcd_write_cmd(node, (0x11));
	Delay(120);
	bus_lcd_write_cmd(node, (0x29));

	bus_lcd_close(node);
	
	Delay(50);
	
	return 0;
}
/**
 *@brief:      drv_ILI9341_xy2cp
 *@details:    ��xy����ת��ΪCP����
 *@param[in]   ��
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ILI9341_xy2cp(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey, u16 *sc, u16 *ec, u16 *sp, u16 *ep)
{
	/*
		��ʾXY�᷶Χ
	*/
	if(sx >= lcd->width)
		sx = lcd->width-1;
	
	if(ex >= lcd->width)
		ex = lcd->width-1;
	
	if(sy >= lcd->height)
		sy = lcd->height-1;
	
	if(ey >= lcd->height)
		ey = lcd->height-1;
	/*
		XY�ᣬʵ��Ƕ�����������ȡ���ں�����������
		CP�ᣬ�ǿ������Դ�Ƕȣ�
		XY���ӳ���ϵȡ����ɨ�跽��
	*/
	if(
		(((lcd->scandir&LRUD_BIT_MASK) == LRUD_BIT_MASK)
		&&(lcd->dir == H_LCD))
		||
		(((lcd->scandir&LRUD_BIT_MASK) == 0)
		&&(lcd->dir == W_LCD))
		)
		{
			*sc = sy;
			*ec = ey;
			*sp = sx;
			*ep = ex;
		}
	else
	{
		*sc = sx;
		*ec = ex;
		*sp = sy;
		*ep = ey;
	}
	
	return 0;
}
/**
 *@brief:      drv_ILI9341_drawpoint
 *@details:    ����
 *@param[in]   u16 x      
               u16 y      
               u16 color  
 *@param[out]  ��
 *@retval:     static
 */
static s32 drv_ILI9341_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color)
{
	u16 sc,ec,sp,ep;

	drv_ILI9341_xy2cp(lcd, x, x, y, y, &sc,&ec,&sp,&ep);
	drv_ILI9341_set_cp_addr(lcd, sc, ec, sp, ep);

	DevLcdBusNode * node;
	node = bus_lcd_open(lcd->dev.buslcd);
	
	u16 tmp[2];
	tmp[0] = color;
	bus_lcd_write_data(node, (u8*)tmp, 1);
	bus_lcd_close(node);
 
	return 0;
}
/**
 *@brief:      drv_ILI9341_color_fill
 *@details:    ��һ�������趨Ϊĳ����ɫ
 *@param[in]   u16 sx     
               u16 sy     
               u16 ex     
               u16 ey     
               u16 color  
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ILI9341_color_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 color)
{

	u16 height,width;
	u16 i,j;
	u16 sc,ec,sp,ep;

	drv_ILI9341_xy2cp(lcd, sx, ex, sy, ey, &sc,&ec,&sp,&ep);
	
	drv_ILI9341_set_cp_addr(lcd, sc, ec, sp, ep);

	width=(ec+1)-sc;//�õ����Ŀ�� +1����Ϊ�����0��ʼ
	height=(ep+1)-sp;//�߶�
	
	//uart_printf("ili9341 width:%d, height:%d\r\n", width, height);

	DevLcdBusNode * node;
	
#define TMP_BUF_SIZE 32
	u16 tmp[TMP_BUF_SIZE];
	u32 cnt;

	for(cnt = 0; cnt < TMP_BUF_SIZE; cnt ++)
	{
		tmp[cnt] = color;
	}
	
	cnt = height*width;
	
	node = bus_lcd_open(lcd->dev.buslcd);

	while(1)
	{
		if(cnt < TMP_BUF_SIZE)
		{
			bus_lcd_write_data(node, (u8 *)tmp, cnt);
			cnt -= cnt;
		}
		else
		{
			bus_lcd_write_data(node, (u8 *)tmp, TMP_BUF_SIZE);
			cnt -= TMP_BUF_SIZE;
		}

		if(cnt <= 0)
			break;
	}
	
	bus_lcd_close(node);

	return 0;

}

/**
 *@brief:      drv_ILI9341_color_fill
 *@details:    ����������
 *@param[in]   u16 sx      
               u16 sy      
               u16 ex      
               u16 ey      
               u16 *color  ÿһ�������ɫ����
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ILI9341_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color)
{

	u16 height,width;
	u32 i,j;
	u16 sc,ec,sp,ep;

	drv_ILI9341_xy2cp(lcd, sx, ex, sy, ey, &sc,&ec,&sp,&ep);
	drv_ILI9341_set_cp_addr(lcd, sc, ec, sp, ep);

	width=(ec+1)-sc;
	height=(ep+1)-sp;

	wjq_log(LOG_DEBUG, "fill width:%d, height:%d\r\n", width, height);
	
	DevLcdBusNode * node;

	node = bus_lcd_open(lcd->dev.buslcd);
	bus_lcd_write_data(node, (u8 *)color, height*width);	
	bus_lcd_close(node);	 
	
	return 0;

} 

s32 drv_ILI9341_prepare_display(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey)
{
	u16 sc,ec,sp,ep;
	
	wjq_log(LOG_DEBUG, "XY:-%d-%d-%d-%d-\r\n", sx, ex, sy, ey);
	drv_ILI9341_xy2cp(lcd, sx, ex, sy, ey, &sc,&ec,&sp,&ep);
	
	wjq_log(LOG_DEBUG, "cp:-%d-%d-%d-%d-\r\n", sc, ec, sp, ep);
	drv_ILI9341_set_cp_addr(lcd, sc, ec, sp, ep);	
	return 0;
}

s32 drv_ILI9341_flush(DevLcdNode *lcd, u16 *color, u32 len)
{
	lcd->busnode = bus_lcd_open(lcd->dev.buslcd);
	bus_lcd_write_data(lcd->busnode, (u8 *)color,  len);	
	bus_lcd_close(lcd->busnode);
	return 0;
} 
#endif

#ifdef TFT_LCD_DRIVER_9341_8BIT

/*

	����һ��TFT LCD��ʹ��ILI9341����IC���豸
	�����������ʹ�õ���SPI����I2C��8λ�ӿڣ�
	���8080 16λ�ӿڣ���Ҫ���⴦��

*/

s32 drv_ILI9341_8_init(DevLcdNode *lcd);
static s32 drv_ILI9341_8_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color);
s32 drv_ILI9341_8_color_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 color);
s32 drv_ILI9341_8_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color);

s32 drv_ILI9341_8_prepare_display(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey);
static void drv_ILI9341_8_scan_dir(DevLcdNode *lcd, u8 dir);
s32 drv_ILI9341_8_flush(DevLcdNode *lcd, u16 *color, u32 len);

_lcd_drv TftLcdILI9341_8_Drv = {
							.id = 0X9342,

							.init = drv_ILI9341_8_init,
							.draw_point = drv_ILI9341_8_drawpoint,
							.color_fill = drv_ILI9341_8_color_fill,
							.fill = drv_ILI9341_8_fill,
							.onoff = drv_ILI9341_display_onoff,
							.prepare_display = drv_ILI9341_8_prepare_display,
							.flush = drv_ILI9341_8_flush,
							.set_dir = drv_ILI9341_8_scan_dir,
							.backlight = drv_ILI9341_lcd_bl
							};

	
/**
 *@brief:      drv_ILI9341_scan_dir
 *@details:    �����Դ�ɨ�跽�� ������Ϊ�����Ƕ�
 *@param[in]   u8 dir  
 *@param[out]  ��
 *@retval:     static
 */
static void drv_ILI9341_8_scan_dir(DevLcdNode *lcd, u8 dir)
{
	u16 regval=0;

	/*���ô���ߵ��ұ߻����ұߵ����*/
	switch(dir)
	{
		case R2L_U2D:
		case R2L_D2U:
		case U2D_R2L:
		case D2U_R2L:
			regval|=(1<<6); 
			break;	 
	}

	/*���ô��ϵ��»��Ǵ��µ���*/
	switch(dir)
	{
		case L2R_D2U:
		case R2L_D2U:
		case D2U_L2R:
		case D2U_R2L:
			regval|=(1<<7); 
			break;	 
	}

	/*
		���������һ��������� Reverse Mode
		�������Ϊ1��LCD�������Ѿ����и��жԵ��ˣ�
		�����Ҫ����ʾ�н��е���
	*/
	switch(dir)
	{
		case U2D_L2R:
		case D2U_L2R:
		case U2D_R2L:
		case D2U_R2L:
			regval|=(1<<5);
			break;	 
	}
	/*
		����������RGB����GBR
		���������õ�ת����
	*/	
	regval|=(1<<3);//0:GBR,1:RGB  ��R61408�෴

	DevLcdBusNode * node;
	node = bus_lcd_open(lcd->dev.buslcd);
	
	bus_lcd_write_cmd(node, (0x36));
	u16 tmp[2];
	tmp[0] = regval;
	bus_lcd_write_data(node, (u8*)tmp, 1);
	bus_lcd_close(node);

}

/**
 *@brief:      drv_ILI9341_set_cp_addr
 *@details:    ���ÿ����������е�ַ��Χ
 *@param[in]   u16 sc  
               u16 ec  
               u16 sp  
               u16 ep  
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ILI9341_8_set_cp_addr(DevLcdNode *lcd, u16 sc, u16 ec, u16 sp, u16 ep)
{

	DevLcdBusNode * node;
	u8 tmp[4];

	node = bus_lcd_open(lcd->dev.buslcd);

	bus_lcd_write_cmd(node, ILI9341_CMD_SETX);
	tmp[0] = (sc>>8);
	tmp[1] = (sc&0XFF);
	tmp[2] = (ec>>8);
	tmp[3] = (ec&0XFF);
	bus_lcd_write_data(node, (u8*)tmp, 4);

	bus_lcd_write_cmd(node, (ILI9341_CMD_SETY));
	tmp[0] = (sp>>8);
	tmp[1] = (sp&0XFF);
	tmp[2] = (ep>>8);
	tmp[3] = (ep&0XFF);
	bus_lcd_write_data(node, (u8*)tmp, 4);

	bus_lcd_write_cmd(node, (ILI9341_CMD_WRAM));
	
	bus_lcd_close(node);
	
	return 0;
}



/**
 *@brief:      drv_ILI9341_init
 *@details:    ��ʼ��FSMC�����Ҷ�ȡILI9341���豸ID
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ILI9341_8_init(DevLcdNode *lcd)
{
	u16 data;
	DevLcdBusNode * node;
	u8 tmp[16];
	
	node = bus_lcd_open(lcd->dev.buslcd);

	bus_lcd_rst(node, 1);
	Delay(50);
	bus_lcd_rst(node, 0);
	Delay(50);
	bus_lcd_rst(node, 1);
	Delay(50);

	bus_lcd_write_cmd(node, (0xd3));
	/*��4���ֽڣ���һ���ֽ���dummy read�� �ڶ��ֽ���0x00�� �����ֽ���93�������ֽ���41*/
	bus_lcd_read_data(node, (u8*)tmp, 4);

	ILI9341_DEBUG(LOG_DEBUG, "read reg:%02x, %02x,%02x,%02x,\r\n", tmp[0], tmp[1], tmp[2], tmp[3]);
	
	data = tmp[2]; 
	data<<=8;
	data |= tmp[3];

	ILI9341_DEBUG(LOG_DEBUG, "read reg:%04x\r\n", data);

	if(data != TftLcdILI9341Drv.id)
	{
		ILI9341_DEBUG(LOG_DEBUG, "lcd drive no 9341\r\n");	
		//bus_lcd_close(node);
		//return -1;
	}

	bus_lcd_write_cmd(node, (0xCF));//Power control B
	tmp[0] = 0x00;
	tmp[1] = 0xC1;
	tmp[2] = 0x30;
	bus_lcd_write_data(node, (u8*)tmp, 3);
	

	bus_lcd_write_cmd(node, (0xED));//Power on sequence control 
	tmp[0] = 0x64;
	tmp[1] = 0x03;
	tmp[2] = 0x12;
	tmp[3] = 0x81;
	bus_lcd_write_data(node, (u8*)tmp, 4);

	bus_lcd_write_cmd(node, (0xE8));//Driver timing control A
	tmp[0] = 0x85;
	tmp[1] = 0x01;
	tmp[2] = 0x7A;
	bus_lcd_write_data(node, (u8*)tmp, 3);

	bus_lcd_write_cmd(node, (0xCB));//Power control 
	tmp[0] = 0x39;
	tmp[1] = 0x2C;
	tmp[2] = 0x00;
	tmp[3] = 0x34;
	tmp[4] = 0x02;
	bus_lcd_write_data(node, (u8*)tmp, 5);

	bus_lcd_write_cmd(node, (0xF7));//Pump ratio control
	tmp[0] = 0x20;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0xEA));//Driver timing control
	tmp[0] = 0x00;
	tmp[1] = 0x00;
	bus_lcd_write_data(node, (u8*)tmp, 2);

	bus_lcd_write_cmd(node, (0xC0));
	tmp[0] = 0x21;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0xC1));
	tmp[0] = 0x11;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0xC5));
	tmp[0] = 0x31;
	tmp[1] = 0x3C;
	bus_lcd_write_data(node, (u8*)tmp, 2);

	bus_lcd_write_cmd(node, (0xC7));
	tmp[0] = 0x9f;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x36));
	tmp[0] = 0x08;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x3A));
	tmp[0] = 0x55;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0xB1));
	tmp[0] = 0x00;
	tmp[1] = 0x1B;
	bus_lcd_write_data(node, (u8*)tmp, 2);

	bus_lcd_write_cmd(node, (0xB6));
	tmp[0] = 0x0A;
	tmp[1] = 0xA2;
	bus_lcd_write_data(node, (u8*)tmp, 2);

	bus_lcd_write_cmd(node, (0xF2));
	tmp[0] = 0x00;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x26));
	tmp[0] = 0x01;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0xE0));
	tmp[0] = 0x0F; tmp[1] = 0x20; tmp[2] = 0x1d; tmp[3] = 0x0b;
	tmp[4] = 0x10; tmp[5] = 0x0a; tmp[6] = 0x49; tmp[7] = 0xa9;
	tmp[8] = 0x3b; tmp[9] = 0x0a; tmp[10] = 0x15; tmp[11] = 0x06;
	tmp[12] = 0x0c; tmp[13] = 0x06; tmp[14] = 0x00;
	bus_lcd_write_data(node, (u8*)tmp, 15);

	bus_lcd_write_cmd(node, (0XE1));
	tmp[0] = 0x00; tmp[1] = 0x1f; tmp[2] = 0x22; tmp[3] = 0x04;
	tmp[4] = 0x0f; tmp[5] = 0x05; tmp[6] = 0x36; tmp[7] = 0x46;
	tmp[8] = 0x46; tmp[9] = 0x05; tmp[10] = 0x0b; tmp[11] = 0x09;
	tmp[12] = 0x33; tmp[13] = 0x39; tmp[14] = 0x0F;
	bus_lcd_write_data(node, (u8*)tmp, 15);	
	
	bus_lcd_write_cmd(node, (0x11));

	bus_lcd_close(node);
	
	Delay(50);
	
	return 0;
}

/**
 *@brief:      drv_ILI9341_drawpoint
 *@details:    ����
 *@param[in]   u16 x      
               u16 y      
               u16 color  
 *@param[out]  ��
 *@retval:     static
 */
static s32 drv_ILI9341_8_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color)
{
	u16 sc,ec,sp,ep;

	drv_ILI9341_xy2cp(lcd, x, x, y, y, &sc,&ec,&sp,&ep);
	drv_ILI9341_8_set_cp_addr(lcd, sc, ec, sp, ep);

	DevLcdBusNode * node;
	node = bus_lcd_open(lcd->dev.buslcd);
	
	u8 tmp[2];
	tmp[0] = color>>8;
	tmp[1] = color&0xff;
	bus_lcd_write_data(node, (u8*)tmp, 2);
	bus_lcd_close(node);
 
	return 0;
}
/**
 *@brief:      drv_ILI9341_color_fill
 *@details:    ��һ�������趨Ϊĳ����ɫ
 *@param[in]   u16 sx     
               u16 sy     
               u16 ex     
               u16 ey     
               u16 color  
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ILI9341_8_color_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 color)
{

	u16 height,width;
	u16 i;
	u16 sc,ec,sp,ep;

	wjq_log(LOG_DEBUG, "%d, %d, %d, %d\r\n", sx, ex, sy, ey);

	drv_ILI9341_xy2cp(lcd, sx, ex, sy, ey, &sc,&ec,&sp,&ep);
	
	wjq_log(LOG_DEBUG, "%d, %d, %d, %d\r\n", sc, ec, sp, ep);
	
	drv_ILI9341_8_set_cp_addr(lcd, sc, ec, sp, ep);

	width=(ec+1)-sc;//�õ����Ŀ�� +1����Ϊ�����0��ʼ
	height=(ep+1)-sp;//�߶�
	
	wjq_log(LOG_DEBUG, "ili9341 width:%d, height:%d\r\n", width, height);

	u8 *tmp;

	tmp = (u8 *)wjq_malloc(width*2);

	for(i = 0; i < width*2;)
	{
		tmp[i++] = color>>8;
		tmp[i++] = color&0xff;
	}
	
	lcd->busnode = bus_lcd_open(lcd->dev.buslcd);

	for(i = 0; i < height;i++)
	{
			bus_lcd_write_data(lcd->busnode, tmp, width*2);
	}
	
	bus_lcd_close(lcd->busnode);

	wjq_free(tmp);

	return 0;

}

/**
 *@brief:      drv_ILI9341_color_fill
 *@details:    ����������
 *@param[in]   u16 sx      
               u16 sy      
               u16 ex      
               u16 ey      
               u16 *color  ÿһ�������ɫ����
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ILI9341_8_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color)
{

	u16 height,width;
	u32 i,j;
	u16 sc,ec,sp,ep;
	u16 *pcc;
	
	drv_ILI9341_xy2cp(lcd, sx, ex, sy, ey, &sc,&ec,&sp,&ep);
	drv_ILI9341_8_set_cp_addr(lcd, sc, ec, sp, ep);

	width=(ec+1)-sc;
	height=(ep+1)-sp;

	wjq_log(LOG_DEBUG, "fill width:%d, height:%d\r\n", width, height);
	
	u8 *tmp;

	tmp = (u8 *)wjq_malloc(width*2);

	lcd->busnode = bus_lcd_open(lcd->dev.buslcd);
	
	pcc = color;
	
	for(i = 0; i < height;i++)
	{
		for(j = 0; j < width*2;)
		{
			tmp[j++] = (*pcc) >> 8;
			tmp[j++] = (*pcc) & 0xff;
			pcc++;
		}
		bus_lcd_write_data(lcd->busnode, tmp, width*2);
	}
	
	bus_lcd_close(lcd->busnode);

	wjq_free(tmp);	 
	return 0;
} 

s32 drv_ILI9341_8_prepare_display(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey)
{
	u16 sc,ec,sp,ep;
	wjq_log(LOG_DEBUG, "XY:-%d-%d-%d-%d-\r\n", sx, ex, sy, ey);
	drv_ILI9341_xy2cp(lcd, sx, ex, sy, ey, &sc,&ec,&sp,&ep);
	
	wjq_log(LOG_DEBUG, "cp:-%d-%d-%d-%d-\r\n", sc, ec, sp, ep);
	drv_ILI9341_8_set_cp_addr(lcd, sc, ec, sp, ep);	
	return 0;
}

s32 drv_ILI9341_8_flush(DevLcdNode *lcd, u16 *color, u32 len)
{
	u8 *tmp;
	u32 i;
	
	DevLcdBusNode * node;
	node = bus_lcd_open(lcd->dev.buslcd);

	/*��ʾ320*240ͼƬ����������ת�������Ҫ10msʱ��*/
	tmp = (u8 *)wjq_malloc(len*2);
	i = 0;
	while(1)
	{
		tmp[i*2] = (*(color+i))>>8;
		tmp[i*2+1] = (*(color+i))&0xff;
		i++;
		if(i>=len)
			break;
	}

	bus_lcd_write_data(lcd->busnode, tmp,  len*2);	
	bus_lcd_close(node);
	
	wjq_free(tmp);
	
	return 0;
} 

#endif


