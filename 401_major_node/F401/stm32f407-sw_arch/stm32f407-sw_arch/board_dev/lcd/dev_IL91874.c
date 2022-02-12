/**
 * @file            dev_IL91874.c
 * @brief           TFT LCD ����оƬIL91874��������
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
	91874 �������� 2.7�����ֽ ��ɫ��
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
#include "dev_IL91874.h"

#define DEV_IL91874_DEBUG

#ifdef DEV_IL91874_DEBUG
#define IL91874_DEBUG	wjq_log 
#else
#define IL91874_DEBUG(a, ...)
#endif

extern void Delay(__IO uint32_t nTime);


/*
	������
	1 91874��ͨ�ţ�ÿ���ֽڶ���Ҫ����CS�������⣬��ʱ���ڱ��������Ȳ��޸�LCD BUS��
	2 91874��һ��BUSY�ܽţ�����ܽţ�Ӧ��������LCD Ӳ���ӿڲ�ġ����ڱ��������Ǻܺ��ʡ�

	3 ����ֽˢ�º���������ܸ�Ϊ��ָ����ʾ���������ʾ���壬refresh������ʽ��
	  �������󣬽�����LCD�������ӿڸ�Ϊ֧������ʽ��
*/

/*

	����ֽ���Դ�ɨ�跽��
	gram[0], ������ʱ���ϽǺ���8���㡣
	������Щcolor fill��fill����Ҫ���ǵġ�

	����������Ե�2.74�� 176*264���ص�
	һ��page����������һ��X�᷽��ĵ㡣
	Ҳ����˵��һ��page�� 176/8 ���ֽڣ�
	LCD�ܹ���264page������(0,0)��page0��1���ֽڵ�bit7��

*/
#define IL91874_PAGE_SIZE ((lcd->dev.width+7)/8)

struct _epaper_drv_data
{
	/*�Դ棬ͨ����̬���룬������Ļ��С����*/
	u8 *bgram;
	u8 *rgram;
	
	/*ˢ������*/
	u16 sx;
	u16 ex;
	u16 sy;
	u16 ey;
	u16 disx;
	u16 disy;

};



#ifdef TFT_LCD_DRIVER_91874

s32 drv_IL91874_init(DevLcdNode *lcd);
static s32 drv_IL91874_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color);
s32 drv_IL91874_color_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 color);
s32 drv_IL91874_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color);
static s32 drv_IL91874_display_onoff(DevLcdNode *lcd, u8 sta);
s32 drv_IL91874_prepare_display(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey);
static void drv_IL91874_scan_dir(DevLcdNode *lcd, u8 dir);
void drv_IL91874_lcd_bl(DevLcdNode *lcd, u8 sta);
s32 drv_IL91874_flush(DevLcdNode *lcd, u16 *color, u32 len);


/*

	����һ��TFT LCD��ʹ��IL91874����IC���豸

*/
_lcd_drv TftLcdIL91874Drv = {
							.id = 0X9187,

							.init = drv_IL91874_init,
							.draw_point = drv_IL91874_drawpoint,
							.color_fill = drv_IL91874_color_fill,
							.fill = drv_IL91874_fill,
							.onoff = drv_IL91874_display_onoff,
							.prepare_display = drv_IL91874_prepare_display,
							.flush = drv_IL91874_flush,
							.set_dir = drv_IL91874_scan_dir,
							.backlight = drv_IL91874_lcd_bl
							};

void SPI_Delay(unsigned char xrate)
{
	unsigned char i;
	while(xrate)
	{
		for(i=0;i<2;i++);
		xrate--;
	}
}


/*
	IL91874, ÿ���ֽڵ�SPIͨ�Ŷ���ҪCS�½���
	Ϊ�˲�Ӱ��LCD BUS����CS�Ĳ������������У�
	����һ���������

	mcu_spi_cs
*/
s32 drv_il91874_write_cmd(DevLcdBusNode *node, u8 cmd)
{
	mcu_spi_cs((DevSpiChNode *)node->basenode, 0);
	bus_lcd_write_cmd(node, cmd);
	mcu_spi_cs((DevSpiChNode *)node->basenode, 1);
}

s32 drv_il91874_write_data(DevLcdBusNode *node, u8 *data, u32 len)
{
	mcu_spi_cs((DevSpiChNode *)node->basenode, 0);
	bus_lcd_write_data(node, data, len);
	mcu_spi_cs((DevSpiChNode *)node->basenode, 1);
}


/*
	īˮ��û�б��⣬��ǰ�ù�Դ
*/
void drv_IL91874_lcd_bl(DevLcdNode *lcd, u8 sta)
{
	DevLcdBusNode * node;
	
	node = bus_lcd_open(lcd->dev.buslcd);
	bus_lcd_bl(node, sta);
	bus_lcd_close(node);

}
	
/**
 *@brief:      drv_IL91874_scan_dir
 *@details:    �����Դ�ɨ�跽�� ������Ϊ�����Ƕ�
 *@param[in]   u8 dir  
 *@param[out]  ��
 *@retval:     static
 */
static void drv_IL91874_scan_dir(DevLcdNode *lcd, u8 dir)
{

}

/**
 *@brief:      drv_IL91874_set_cp_addr
 *@details:    ���ÿ����������е�ַ��Χ
 *@param[in]   u16 sc  
               u16 ec  
               u16 sp  
               u16 ep  
 *@param[out]  ��
 *@retval:     
 */
s32 drv_IL91874_set_cp_addr(DevLcdNode *lcd, u16 sc, u16 ec, u16 sp, u16 ep)
{

}
/**
 *@brief:      drv_IL91874_refresh_gram
 *@details:       ˢ��ָ��������Ļ��
                  �����Ǻ���ģʽ����
 *@param[in]   u16 sc  
               u16 ec  
               u16 sp  
               u16 ep  
 *@param[out]  ��
 *@retval:     static
 */
static s32 drv_IL91874_refresh_gram(DevLcdNode *lcd, u16 sc, u16 ec, u16 sp, u16 ep)
{	
	struct _epaper_drv_data *drvdata; 

	DevLcdBusNode *node;
	u32 cnt;
	u32 gramsize;
	u16 i,j;
	u8 data;
	u16 w,l;
	
	drvdata = (struct _epaper_drv_data *)lcd->pri;
	node = bus_lcd_open(lcd->dev.buslcd);

	wjq_log(LOG_DEBUG, "drv_IL91874_refresh_gram: %d, %d, %d, %d\r\n ", sc, ec, sp, ep);
	
	#if 0 /*�ֲ�ˢ*/
	/* ��SC��w����У����������8�������� */
	sc = sc&0xfff8;
	w = ec-sc+1;
	l = ep-sp+1;
	w = w&0xfff8;
	
	wjq_log(LOG_DEBUG, "drv_IL91874_refresh_gram: %d, %d, %d, %d\r\n ", sc, sp, w, l);
	
	
	bus_lcd_write_cmd(node, (0x14));
	data = sc>>8;
	bus_lcd_write_data(node, &data, 1);
	data = sc&0xff;
	bus_lcd_write_data(node, &data, 1);

	data = sp>>8;
	bus_lcd_write_data(node, &data, 1);
	data = sp&0xff;
	bus_lcd_write_data(node, &data, 1);

	data = w>>8;
	bus_lcd_write_data(node, &data, 1);
	data = w&0xff;
	bus_lcd_write_data(node, &data, 1);
	
	data = l>>8;
	bus_lcd_write_data(node, &data, 1);
	data = l&0xff;
	bus_lcd_write_data(node, &data, 1);


	for(j=0;j < l;j++)
		for(i=0;i < w/8;i++)
			bus_lcd_write_data(node, (u8 *)&drvdata->bgram[j*IL91874_PAGE_SIZE + i], 1);
		
	bus_lcd_write_cmd(node, (0x15));
	data = sc>>8;
	bus_lcd_write_data(node, &data, 1);
	data = sc&0xff;
	bus_lcd_write_data(node, &data, 1);

	data = sp>>8;
	bus_lcd_write_data(node, &data, 1);
	data = sp&0xff;
	bus_lcd_write_data(node, &data, 1);

	data = w>>8;
	bus_lcd_write_data(node, &data, 1);
	data = w&0xff;
	bus_lcd_write_data(node, &data, 1);
	
	data = l>>8;
	bus_lcd_write_data(node, &data, 1);
	data = l&0xff;
	bus_lcd_write_data(node, &data, 1);
	
	for(j=0;j < l;j++)
		for(i=0;i < w/8;i++)
			bus_lcd_write_data(node, (u8 *)&drvdata->rgram[j*IL91874_PAGE_SIZE + i], 1);

		
	wjq_log(LOG_DEBUG, " DISPLAY REFRESH\r\n ");

	bus_lcd_write_cmd(node, (0x16));//DISPLAY REFRESH 	
	data = sc>>8;
	bus_lcd_write_data(node, &data, 1);
	data = sc&0xff;
	bus_lcd_write_data(node, &data, 1);

	data = sp>>8;
	bus_lcd_write_data(node, &data, 1);
	data = sp&0xff;
	bus_lcd_write_data(node, &data, 1);

	data = w>>8;
	bus_lcd_write_data(node, &data, 1);
	data = w&0xff;
	bus_lcd_write_data(node, &data, 1);
	
	data = l>>8;
	bus_lcd_write_data(node, &data, 1);
	data = l&0xff;
	bus_lcd_write_data(node, &data, 1);
	#endif

	#if 1 /*ȫˢ*/
	/*ע�⣬Ҫ��dev�е�w��h����Ϊgram��������������û��ϵ��
	ֻ�Ͷ���һ��*/
	gramsize = lcd->dev.height * IL91874_PAGE_SIZE;
	//wjq_log(LOG_DEBUG, "gram size: %d\r\n ", gramsize);


	drv_il91874_write_cmd(node, (0x10));
	for(cnt = 0; cnt <gramsize; cnt++)
		drv_il91874_write_data(node, (u8 *)&drvdata->bgram[cnt], 1);

	drv_il91874_write_cmd(node, (0x13));
	for(cnt = 0; cnt <gramsize; cnt++)
		drv_il91874_write_data(node, (u8 *)&drvdata->rgram[cnt], 1);
	
	wjq_log(LOG_DEBUG, " DISPLAY REFRESH\r\n ");

	drv_il91874_write_cmd(node, (0x12));//DISPLAY REFRESH 	
	#endif

	Delay(1);//!!!The delay here is necessary, 200uS at least!!!  
	
	unsigned char busy;
	do
	{
		drv_il91874_write_cmd(node, (0x71));
		busy = mcu_io_input_readbit(node->dev.staport, node->dev.stapin);
		busy =!(busy & 0x01);        
	}
	while(busy);  
	wjq_log(LOG_DEBUG, "IL91874 refresh finish\r\n ");
	bus_lcd_close(node);
	
	return 0;
}

/**
 *@brief:      drv_IL91874_display_onoff
 *@details:    ��ʾ��ر�
 *@param[in]   u8 sta  
 *@param[out]  ��
 *@retval:     static
 */
static s32 drv_IL91874_display_onoff(DevLcdNode *lcd, u8 sta)
{
	wjq_log(LOG_DEBUG, " drv_IL91874_display_onoff\r\n ");	
	return 0;
}

/**
 *@brief:      drv_IL91874_init
 *@details:    
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 drv_IL91874_init(DevLcdNode *lcd)
{
	u16 data;
	DevLcdBusNode * node;
	u8 tmp[16];
	u8 testbuf[2];
	
	
  
	node = bus_lcd_open(lcd->dev.buslcd);

	mcu_io_config_in(node->dev.staport, node->dev.stapin);

	bus_lcd_rst(node, 1);
	Delay(50);
	bus_lcd_rst(node, 0);
	Delay(1000);
	bus_lcd_rst(node, 1);
	Delay(1000);

	
	drv_il91874_write_cmd(node, (0x06));//boost soft start
	tmp[0] = 0x07;
	drv_il91874_write_data(node, (u8*)tmp, 1);
	tmp[0] = 0x07;
	drv_il91874_write_data(node, (u8*)tmp, 1);
	tmp[0] = 0x17;
	drv_il91874_write_data(node, (u8*)tmp, 1);
     
	drv_il91874_write_cmd(node, (0x04)); 
	
	unsigned char busy;
	do
	{
		drv_il91874_write_cmd(node, (0x71));
		busy = mcu_io_input_readbit(node->dev.staport, node->dev.stapin);
		busy =!(busy & 0x01);        
	}
	while(busy);   
	Delay(200); 

	drv_il91874_write_cmd(node, (0x00));//panel setting	
	tmp[0] = 0x0f;
	drv_il91874_write_data(node, (u8*)tmp, 1);//LUT from OTP��128x296
	drv_il91874_write_cmd(node, (0x0d));//VCOM to 0V fast

	drv_il91874_write_cmd(node, (0x16));
	tmp[0] = 0x00;
	drv_il91874_write_data(node, (u8*)tmp, 1);//KW-BF   KWR-AF	BWROTP 0f	

	drv_il91874_write_cmd(node, (0xF8));         //boost�趨
	tmp[0] = 0x60;
	drv_il91874_write_data(node, (u8*)tmp, 1);
	tmp[0] = 0xa5;
	drv_il91874_write_data(node, (u8*)tmp, 1);
	
	drv_il91874_write_cmd(node, (0xF8));         //boost�趨
	tmp[0] = 0x73;
	drv_il91874_write_data(node, (u8*)tmp, 1);
	tmp[0] = 0x23;
	drv_il91874_write_data(node, (u8*)tmp, 1);
	
	drv_il91874_write_cmd(node, (0xF8));        //boost�趨
	tmp[0] = 0x7C;
	drv_il91874_write_data(node, (u8*)tmp, 1);
	tmp[0] = 0x00;
	drv_il91874_write_data(node, (u8*)tmp, 1);
	
	bus_lcd_close(node);
	
	Delay(50);

	
	/*�����Դ棬�����ͷ�*/
	lcd->pri = (void *)wjq_malloc(sizeof(struct _epaper_drv_data));
	memset((char*)lcd->pri, 0x00, sizeof(struct _epaper_drv_data));

	struct _epaper_drv_data *p;
	u16 gramsize;

	/*��ɫ����ֽ��Ҫ��������*/
	p = (struct _epaper_drv_data *)lcd->pri;

	gramsize = lcd->dev.height * IL91874_PAGE_SIZE;
	wjq_log(LOG_DEBUG, "gram size: %d\r\n ", gramsize);

	
	p->bgram = (u8 *)wjq_malloc(gramsize);
	p->rgram = (u8 *)wjq_malloc(gramsize);
	
	wjq_log(LOG_DEBUG, "drv_IL91874_init finish\r\n ");

	return 0;
}
/**
 *@brief:      drv_IL91874_xy2cp
 *@details:    ��xy����ת��ΪCP����
 *@param[in]   ��
 *@param[out]  ��
 *@retval:     
 */
s32 drv_IL91874_xy2cp(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey, u16 *sc, u16 *ec, u16 *sp, u16 *ep)
{

	return 0;
}
/**
 *@brief:      drv_IL91874_drawpoint
 *@details:    ����
 *@param[in]   u16 x      
               u16 y      
               u16 color  
 *@param[out]  ��
 *@retval:     static
 */
static s32 drv_IL91874_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color)
{
	wjq_log(LOG_DEBUG, " drv_IL91874_drawpoint------\r\n ");
	return 0;
}
/**
 *@brief:      drv_IL91874_color_fill
 *@details:    ��һ�������趨Ϊĳ����ɫ
 *@param[in]   u16 sx     
               u16 sy     
               u16 ex     
               u16 ey     
               u16 color  
 *@param[out]  ��
 *@retval:     
 */
s32 drv_IL91874_color_fill(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey, u16 color)
{
	u16 i,j;
	u16 xtmp,ytmp;
	u16 page, colum;
	u16 sp, ep, sc, ec;
	
	struct _epaper_drv_data *drvdata;

	wjq_log(LOG_DEBUG, " drv_IL91874_color_fill\r\n ");

	drvdata = (struct _epaper_drv_data *)lcd->pri;

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

	/*������Ķ���page��cloum*/
	if(lcd->dir == H_LCD)
	{
		sp = sy;
		ep = ey;

		sc = sx;
		ec = ex;
			
	}
	else//�����������XY����Դ��ӳ��Ҫ�Ե�
	{
		sp = sx;
		ep = ex;

		ec = (lcd->height - sy);
		sc = (lcd->height - ey);
	}

	for(j=sy;j<=ey;j++)
	{
		//wjq_log(LOG_DEBUG, "\r\n");
		
		for(i=sx;i<=ex;i++)
		{

			if(lcd->dir == H_LCD)
			{
				xtmp = i;
				ytmp = j;
			}
			else//�����������XY����Դ��ӳ��Ҫ�Ե�
			{
				/* ��ͬ���㷨���൱�ڲ�ͬ��ɨ�跽ʽ*/
			
				//xtmp = j;
				//ytmp = lcd->width-i;

				xtmp = lcd->height - 1 - j;
				ytmp = i;
			}

			page = ytmp; //ҳ��ַ
			colum = xtmp/8;//�е�ַ
		

			if(color == RED)
			{
				drvdata->rgram[page*IL91874_PAGE_SIZE + colum] |= (0x80>>(xtmp%8));
				drvdata->bgram[page*IL91874_PAGE_SIZE + colum] &= ~(0x80>>(xtmp%8));
				//uart_printf("*");
			}
			else if(color == BLACK)
			{
				drvdata->bgram[page*IL91874_PAGE_SIZE + colum] |= (0x80>>(xtmp%8));
				drvdata->rgram[page*IL91874_PAGE_SIZE + colum] &= ~(0x80>>(xtmp%8));
				//uart_printf("*");
			}	
			else
			{
				drvdata->bgram[page*IL91874_PAGE_SIZE + colum] &= ~(0x80>>(xtmp%8));
				drvdata->rgram[page*IL91874_PAGE_SIZE + colum] &= ~(0x80>>(xtmp%8));
				//uart_printf("-");
			}
		}
	}

	/*
		ֻˢ����Ҫˢ�µ�����
		���귶Χ�Ǻ���ģʽ
	*/
	drv_IL91874_refresh_gram(lcd, sc, ec, sp, ep);
	wjq_log(LOG_DEBUG, " drv_IL91874_color_fill finish\r\n ");

	return 0;
}

/**
 *@brief:      drv_IL91874_fill
 *@details:    ����������
 *@param[in]   u16 sx      
               u16 sy      
               u16 ex      
               u16 ey      
               u16 *color  ÿһ�������ɫ����
 *@param[out]  ��
 *@retval:     
 */
s32 drv_IL91874_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color)
{
	u16 i,j;
	u16 xtmp,ytmp;
	u16 xlen,ylen;
	u16 page, colum;
	u32 index;
	u16 cdata;
	u16 sp, ep, sc, ec;
	
	struct _epaper_drv_data *drvdata;

	wjq_log(LOG_DEBUG, " drv_IL91874_fill:%d,%d,%d,%d\r\n", sx,ex,sy,ey);
	
	drvdata = (struct _epaper_drv_data *)lcd->pri;

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

	/*������Ķ���page��cloum*/
	if(lcd->dir == H_LCD)
	{
		sp = sy;
		ep = ey;

		sc = sx;
		ec = ex;
			
	}
	else//�����������XY����Դ��ӳ��Ҫ�Ե�
	{
		sp = sx;
		ep = ex;

		ec = (lcd->height - sy);
		sc = (lcd->height - ey);
	}
	
	for(j=sy;j<=ey;j++)
	{
		//uart_printf("\r\n");

		index = (j-sy)*xlen;
		
		for(i=sx;i<=ex;i++)
		{

			if(lcd->dir == H_LCD)
			{
				xtmp = i;
				ytmp = j;
			}
			else
			{
				xtmp = lcd->height - 1 - j;
				ytmp = i;
			}
			
			page = ytmp; //ҳ��ַ
			colum = xtmp/8;//�е�ַ
			
			cdata = *(color+index+i-sx);

			if(cdata == RED)
			{
				drvdata->rgram[page*IL91874_PAGE_SIZE + colum] |= (0x80>>(xtmp%8));
				drvdata->bgram[page*IL91874_PAGE_SIZE + colum] &= ~(0x80>>(xtmp%8));
				//uart_printf("*");
			}
			else if(cdata == BLACK)
			{
				drvdata->bgram[page*IL91874_PAGE_SIZE + colum] |= (0x80>>(xtmp%8));
				drvdata->rgram[page*IL91874_PAGE_SIZE + colum] &= ~(0x80>>(xtmp%8));
				//uart_printf("*");
			}	
			else
			{
				drvdata->bgram[page*IL91874_PAGE_SIZE + colum] &= ~(0x80>>(xtmp%8));
				drvdata->rgram[page*IL91874_PAGE_SIZE + colum] &= ~(0x80>>(xtmp%8));
				//uart_printf("-");
			}
		}
	}

	/*
		ֻˢ����Ҫˢ�µ�����
		���귶Χ�Ǻ���ģʽ
	*/
	drv_IL91874_refresh_gram(lcd, sc,ec,sp,ep);
	//uart_printf("refresh ok\r\n");		
	return 0;
}


s32 drv_IL91874_prepare_display(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey)
{
	wjq_log(LOG_DEBUG, " drv_IL91874_prepare_display\r\n ");
	return 0;
}

s32 drv_IL91874_flush(DevLcdNode *lcd, u16 *color, u32 len)
{
	wjq_log(LOG_DEBUG, " drv_IL91874_flush\r\n ");
	return 0;
} 
#endif


