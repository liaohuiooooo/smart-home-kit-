/**
 * @file            dev_ILI9341.c
 * @brief           TFT LCD ����оƬST7789��������
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

/* 
	1.3�磬IPS���о�԰��ֻ��SCL&SDA��SPI�ӿ�LCD
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

extern void Delay(__IO uint32_t nTime);


#ifdef TFT_LCD_DRIVER_7789
#define ST7789_CMD_WRAM 0x2c
#define ST7789_CMD_SETX 0x2a
#define ST7789_CMD_SETY 0x2b

s32 drv_ST7789_init(DevLcdNode *lcd);
static s32 drv_ST7789_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color);
s32 drv_ST7789_color_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 color);
s32 drv_ST7789_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color);
static s32 drv_ST7789_display_onoff(DevLcdNode *lcd, u8 sta);
s32 drv_ST7789_prepare_display(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey);
static void drv_ST7789_scan_dir(DevLcdNode *lcd, u8 dir);
void drv_ST7789_lcd_bl(DevLcdNode *lcd, u8 sta);
s32 drv_ST7789_flush(DevLcdNode *lcd, u16 *color, u32 len);

_lcd_drv TftLcdST7789_Drv = {
							.id = 0X7789,

							.init = drv_ST7789_init,
							.draw_point = drv_ST7789_drawpoint,
							.color_fill = drv_ST7789_color_fill,
							.fill = drv_ST7789_fill,
							.onoff = drv_ST7789_display_onoff,
							.prepare_display = drv_ST7789_prepare_display,
							.flush = drv_ST7789_flush,
							.set_dir = drv_ST7789_scan_dir,
							.backlight = drv_ST7789_lcd_bl
							};

void drv_ST7789_lcd_bl(DevLcdNode *lcd, u8 sta)
{
	DevLcdBusNode * node;
	
	node = bus_lcd_open(lcd->dev.buslcd);
	bus_lcd_bl(node, sta);
	bus_lcd_close(node);

}		
/**
 *@brief:      drv_ST7789_scan_dir
 *@details:    �����Դ�ɨ�跽�� ������Ϊ�����Ƕ�
 *@param[in]   u8 dir  
 *@param[out]  ��
 *@retval:     static
 */
static void drv_ST7789_scan_dir(DevLcdNode *lcd, u8 dir)
{
	u16 regval=0;
#if 0
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

#endif

}

/**
 *@brief:      drv_ST7789_set_cp_addr
 *@details:    ���ÿ����������е�ַ��Χ
 *@param[in]   u16 sc  
               u16 ec  
               u16 sp  
               u16 ep  
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ST7789_set_cp_addr(DevLcdNode *lcd, u16 sc, u16 ec, u16 sp, u16 ep)
{

	DevLcdBusNode * node;
	u8 tmp[4];

	node = bus_lcd_open(lcd->dev.buslcd);

	bus_lcd_write_cmd(node, ST7789_CMD_SETX);
	tmp[0] = (0x00);
	tmp[1] = (sc);
	tmp[2] = (0X00);
	tmp[3] = (ec);
	bus_lcd_write_data(node, (u8*)tmp, 4);

	bus_lcd_write_cmd(node, (ST7789_CMD_SETY));
	tmp[0] = (0);
	tmp[1] = (sp);
	tmp[2] = (0);
	tmp[3] = (ep);
	bus_lcd_write_data(node, (u8*)tmp, 4);

	bus_lcd_write_cmd(node, (ST7789_CMD_WRAM));
	
	bus_lcd_close(node);
	
	return 0;
}

/**
 *@brief:      drv_ST7789_display_onoff
 *@details:    ��ʾ��ر�
 *@param[in]   u8 sta  
 *@param[out]  ��
 *@retval:     static
 */
static s32 drv_ST7789_display_onoff(DevLcdNode *lcd, u8 sta)
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
 *@brief:      drv_ST7789_init
 *@details:    ��ʼ��FSMC�����Ҷ�ȡILI9341���豸ID
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ST7789_init(DevLcdNode *lcd)
{
	u16 data;
	DevLcdBusNode * node;
	u8 tmp[16];
	
	node = bus_lcd_open(lcd->dev.buslcd);

	bus_lcd_rst(node, 1);
	Delay(50);
	bus_lcd_rst(node, 0);
	Delay(100);
	bus_lcd_rst(node, 1);
	Delay(50);

	bus_lcd_write_cmd(node, (0x36));
	tmp[0] = 0x00;
	bus_lcd_write_data(node, (u8*)tmp, 1);
	
	bus_lcd_write_cmd(node, (0x3A));
	tmp[0] = 0x05;
	bus_lcd_write_data(node, (u8*)tmp, 1);
	
	bus_lcd_write_cmd(node, (0xB2));
	tmp[0] = 0x0C;
	tmp[1] = 0x0C;
	tmp[2] = 0x00;
	tmp[3] = 0x33;
	tmp[4] = 0x33;
	bus_lcd_write_data(node, (u8*)tmp, 5);
	
	bus_lcd_write_cmd(node, (0xB7));
	tmp[0] = 0x35;
	bus_lcd_write_data(node, (u8*)tmp, 1);
	
	bus_lcd_write_cmd(node, (0xBB));
	tmp[0] = 0x19;
	bus_lcd_write_data(node, (u8*)tmp, 1);
	
	bus_lcd_write_cmd(node, (0xC0));
	tmp[0] = 0x2C;
	bus_lcd_write_data(node, (u8*)tmp, 1);
	
	bus_lcd_write_cmd(node, (0xC2));
	tmp[0] = 0x01;
	bus_lcd_write_data(node, (u8*)tmp, 1);
	 
	bus_lcd_write_cmd(node, (0xC3));
	tmp[0] = 0x12;
	bus_lcd_write_data(node, (u8*)tmp, 1);
	
	bus_lcd_write_cmd(node, (0xC4));
	tmp[0] = 0x20;
	bus_lcd_write_data(node, (u8*)tmp, 1);
	  
	bus_lcd_write_cmd(node, (0xC6));
	tmp[0] = 0x0F;
	bus_lcd_write_data(node, (u8*)tmp, 1);
	
	bus_lcd_write_cmd(node, (0xD0));
	tmp[0] = 0xA4;
	tmp[1] = 0xA1;
	bus_lcd_write_data(node, (u8*)tmp, 2);
	
	bus_lcd_write_cmd(node, (0xE0));
	tmp[0] = 0xD0;
	tmp[1] = 0x04;
	tmp[2] = 0x0D;
	tmp[3] = 0x11;

	tmp[4] = 0x13;
	tmp[5] = 0x2B;
	tmp[6] = 0x3F;
	tmp[7] = 0x54;

	tmp[8] = 0x4C;
	tmp[9] = 0x18;
	tmp[10] = 0x0D;
	tmp[11] = 0x0B;

	tmp[12] = 0x1F;
	tmp[13] = 0x23;
	
	bus_lcd_write_data(node, (u8*)tmp, 14);	
	
	bus_lcd_write_cmd(node, (0xE1));
	tmp[0] = 0xD0;
	tmp[1] = 0x04;
	tmp[2] = 0x0C;
	tmp[3] = 0x11;

	tmp[4] = 0x13;
	tmp[5] = 0x2C;
	tmp[6] = 0x3F;
	tmp[7] = 0x44;

	tmp[8] = 0x51;
	tmp[9] = 0x2F;
	tmp[10] = 0x1F;
	tmp[11] = 0x1F;

	tmp[12] = 0x20;
	tmp[13] = 0x23;
	bus_lcd_write_data(node, (u8*)tmp, 14);	
	bus_lcd_write_cmd(node, (0x21)); 
	
	bus_lcd_write_cmd(node, (0x11)); 
	Delay (50); 
	//Display on	 
	bus_lcd_write_cmd(node, (0x29)); 

	bus_lcd_close(node);
	
	Delay(50);
	
	return 0;
}
/**
 *@brief:      drv_ST7789_xy2cp
 *@details:    ��xy����ת��ΪCP����
 *@param[in]   ��
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ST7789_xy2cp(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey, u16 *sc, u16 *ec, u16 *sp, u16 *ep)
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
 *@brief:      drv_ST7789_drawpoint
 *@details:    ����
 *@param[in]   u16 x      
               u16 y      
               u16 color  
 *@param[out]  ��
 *@retval:     static
 */
static s32 drv_ST7789_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color)
{
	u16 sc,ec,sp,ep;

	drv_ST7789_xy2cp(lcd, x, x, y, y, &sc,&ec,&sp,&ep);
	drv_ST7789_set_cp_addr(lcd, sc, ec, sp, ep);

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
 *@brief:      drv_ST7789_color_fill
 *@details:    ��һ�������趨Ϊĳ����ɫ
 *@param[in]   u16 sx     
               u16 sy     
               u16 ex     
               u16 ey     
               u16 color  
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ST7789_color_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 color)
{

	u16 height,width;
	u16 i;
	u16 sc,ec,sp,ep;

	wjq_log(LOG_DEBUG, "%d, %d, %d, %d\r\n", sx, ex, sy, ey);

	drv_ST7789_xy2cp(lcd, sx, ex, sy, ey, &sc,&ec,&sp,&ep);
	
	wjq_log(LOG_DEBUG, "%d, %d, %d, %d\r\n", sc, ec, sp, ep);
	
	drv_ST7789_set_cp_addr(lcd, sc, ec, sp, ep);

	width=(ec+1)-sc;//�õ����Ŀ�� +1����Ϊ�����0��ʼ
	height=(ep+1)-sp;//�߶�
	
	wjq_log(LOG_DEBUG, "ST7789 width:%d, height:%d\r\n", width, height);

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
 *@brief:      drv_ST7789_fill
 *@details:    ����������
 *@param[in]   u16 sx      
               u16 sy      
               u16 ex      
               u16 ey      
               u16 *color  ÿһ�������ɫ����
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ST7789_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color)
{

	u16 height,width;
	u32 i,j;
	u16 sc,ec,sp,ep;
	u16 *pcc;
	
	drv_ST7789_xy2cp(lcd, sx, ex, sy, ey, &sc,&ec,&sp,&ep);
	drv_ST7789_set_cp_addr(lcd, sc, ec, sp, ep);

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

s32 drv_ST7789_prepare_display(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey)
{
	u16 sc,ec,sp,ep;
	wjq_log(LOG_DEBUG, "XY:-%d-%d-%d-%d-\r\n", sx, ex, sy, ey);
	drv_ST7789_xy2cp(lcd, sx, ex, sy, ey, &sc,&ec,&sp,&ep);
	
	wjq_log(LOG_DEBUG, "cp:-%d-%d-%d-%d-\r\n", sc, ec, sp, ep);
	drv_ST7789_set_cp_addr(lcd, sc, ec, sp, ep);	
	return 0;
}

s32 drv_ST7789_flush(DevLcdNode *lcd, u16 *color, u32 len)
{
	u8 *tmp;
	u32 i;
	
	DevLcdBusNode * node;
	node = bus_lcd_open(lcd->dev.buslcd);

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

