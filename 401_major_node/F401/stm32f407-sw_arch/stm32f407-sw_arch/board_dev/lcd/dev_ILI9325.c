/**
 * @file            dev_ILI9325.c
 * @brief           TFT LCD ����оƬILI9325��������
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

extern void Delay(__IO uint32_t nTime);

/*

	9325

*/
#ifdef TFT_LCD_DRIVER_9325
/*
	9325�ص㣺
	9325��ʹ��page��colum��ʹ��Horizontal��Vertical���������ǵ�ģ�飬H��240�̱ߣ�V��320����
	1 ����ɨ����ʼ��ַ,  Ҳ���� H, V��ַ�� HV��ַ��ɲ���AD
	  AD�Ͱ�λ��ӦH���߰�λ��ӦV
	  
	2 ɨ�贰�ڵ�ַ��
	  9325����HSA/HEA/VSA/VEA
	  ����HҪ���ڵ���4
	  ��00��h �� HSA[7:0]< HEA[7:0] �� ��EF��h. and ��04��h�QHEA-HAS
	  ��000��h �� VSA[8:0]< VEA[8:0] �� ��13F��h.

	3 ������ɨ�跽�������»���������ʱ��H��V���䡣

	4 ����ɨ�跽����ô�䣬ԭ�㶼�����������Ͻǡ�������Ļ��ɨ�跽�����㡣
	���磬����Ϊ���������ң��ϵ���
	dev_tftlcd_setdir( W_LCD, L2R_U2D);
	Ȼ��ˢ��һ�����ݿ�
	TftLcd->color_fill(0, TftLcd->width/4, 0 , TftLcd->height/4, RED);
	������Ļˢ�µ��Ǻ��������Ͻǣ�
	9325ˢ���������½ǣ�
	��������ˢ�����һ�У�Ҳ���ǵ�0�У������Ǵ����ҡ�
	Ȼ����ˢ����һ�У�Ҳ�Ǵ����ҡ�
	�����Ҫ��ɨ�������ΪXY������
	
	
*/
#define ILI9325_CMD_WRAM 0x22
#define ILI9325_CMD_SETV 0x21
#define ILI9325_CMD_SETH 0x20


s32 drv_ILI9325_init(DevLcdNode *lcd);
static s32 drv_ILI9325_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color);
s32 drv_ILI9325_color_fill(DevLcdNode *lcd, u16 sx,u16 sy,u16 ex,u16 ey,u16 color);
s32 drv_ILI9325_fill(DevLcdNode *lcd, u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);
static s32 drv_ILI9325_display_onoff(DevLcdNode *lcd, u8 sta);
s32 drv_ILI9325_prepare_display(DevLcdNode *lcd, u16 sc, u16 ec, u16 sp, u16 ep);
static void drv_ILI9325_scan_dir(DevLcdNode *lcd, u8 dir);
void drv_ILI9325_lcd_bl(DevLcdNode *lcd, u8 sta);
s32 drv_ILI9325_flush(DevLcdNode *lcd, u16 *color, u32 len);

/*

	9325����

*/
_lcd_drv TftLcdILI9325Drv = {
							.id = 0X9325,

							.init = drv_ILI9325_init,
							.draw_point = drv_ILI9325_drawpoint,
							.color_fill = drv_ILI9325_color_fill,
							.fill = drv_ILI9325_fill,
							.prepare_display = drv_ILI9325_prepare_display,
							.flush = drv_ILI9325_flush,
							
							.onoff = drv_ILI9325_display_onoff,
							.set_dir = drv_ILI9325_scan_dir,
							.backlight = drv_ILI9325_lcd_bl
							};
void drv_ILI9325_lcd_bl(DevLcdNode *lcd, u8 sta)
{
	DevLcdBusNode * node;
	
	node = bus_lcd_open(lcd->dev.buslcd);
	bus_lcd_bl(node, sta);
	bus_lcd_close(node);

}	
/**
 *@brief:	   drv_ILI9325_scan_dir
 *@details:    �����Դ�ɨ�跽��
 *@param[in]   u8 dir  
 *@param[out]  ��
 *@retval:	   static OK
 */
static void drv_ILI9325_scan_dir(DevLcdNode *lcd, u8 dir)
{
	u16 regval=0;
	u8 dirreg=0;
	
	switch(dir)
    {
        case L2R_U2D://������,���ϵ���
            regval|=(1<<5)|(1<<4)|(0<<3); 
            break;
        case L2R_D2U://������,���µ���
            regval|=(0<<5)|(1<<4)|(0<<3); 
            break;
        case R2L_U2D://���ҵ���,���ϵ���
            regval|=(1<<5)|(0<<4)|(0<<3);
            break;
        case R2L_D2U://���ҵ���,���µ���
            regval|=(0<<5)|(0<<4)|(0<<3); 
            break;   
        case U2D_L2R://���ϵ���,������
            regval|=(1<<5)|(1<<4)|(1<<3); 
            break;
        case U2D_R2L://���ϵ���,���ҵ���
            regval|=(1<<5)|(0<<4)|(1<<3); 
            break;
        case D2U_L2R://���µ���,������
            regval|=(0<<5)|(1<<4)|(1<<3); 
            break;
        case D2U_R2L://���µ���,���ҵ���
            regval|=(0<<5)|(0<<4)|(1<<3); 
            break;   
    }
	
    dirreg=0X03;
    regval|=1<<12;  

	u16 tmp[16];
	DevLcdBusNode * node;
	
	node = bus_lcd_open(lcd->dev.buslcd);
	
	bus_lcd_write_cmd(node, (dirreg));
	tmp[0] = regval;
	bus_lcd_write_data(node, (u8*)tmp, 1);
	
	bus_lcd_close(node);

}

/**
 *@brief:      drv_ILI9325_set_cp_addr
 *@details:    ���ÿ����������е�ַ��Χ
 *@param[in]   u16 sc  
               u16 ec  
               u16 sp  
               u16 ep  
 *@param[out]  ��
 *@retval:     9325����ɨ��������һ�����ƣ��Ǿ��Ǵ��ڿ�Ȳ���С����
 */
static s32 drv_ILI9325_set_cp_addr(DevLcdNode *lcd, u16 hsa, u16 hea, u16 vsa, u16 vea)
{
	u16 heatmp;
	u16 tmp[2];
	DevLcdBusNode * node;
	
	node = bus_lcd_open(lcd->dev.buslcd);

	/* ����ɨ�贰�� */
	if((hsa+4) > hea)
		heatmp = hsa+4;
	else
		heatmp = hea;
	
	bus_lcd_write_cmd(node, (0x0050));//HSA
	tmp[0] = hsa;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x0051));//HEA
	tmp[0] = heatmp;
	bus_lcd_write_data(node, (u8*)tmp, 1);
	
	bus_lcd_write_cmd(node, (0x0052));//VSA
	tmp[0] = vsa;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x0053));//VEA
	tmp[0] = vea;
	bus_lcd_write_data(node, (u8*)tmp, 1);
	
	/*
		����ɨ����ʼ��ַ��
	*/
	if((lcd->scandir&LR_BIT_MASK) == LR_BIT_MASK)
	{
		bus_lcd_write_cmd(node, (ILI9325_CMD_SETH));
		tmp[0] = hea&0XFF;
		bus_lcd_write_data(node, (u8*)tmp, 1);
	
	}
	else
	{
		bus_lcd_write_cmd(node, (ILI9325_CMD_SETH));
		tmp[0] = hsa&0XFF;
		bus_lcd_write_data(node, (u8*)tmp, 1);
			  
	}

	if((lcd->scandir&UD_BIT_MASK) == UD_BIT_MASK)
	{
		bus_lcd_write_cmd(node, (ILI9325_CMD_SETV));
		tmp[0] = vea&0X1FF;
		bus_lcd_write_data(node, (u8*)tmp, 1);
		
	}
	else
	{
		bus_lcd_write_cmd(node, (ILI9325_CMD_SETV));
		tmp[0] = vsa&0X1FF;
		bus_lcd_write_data(node, (u8*)tmp, 1);
	}
	bus_lcd_write_cmd(node, (ILI9325_CMD_WRAM));
	bus_lcd_close(node);
	return 0;
}

/**
 *@brief:	   drv_ILI9325_display_onoff
 *@details:    ��ʾ��ر�
 *@param[in]   u8 sta  
 *@param[out]  ��
 *@retval:	   static OK
 */
static s32 drv_ILI9325_display_onoff(DevLcdNode *lcd, u8 sta)
{
	u16 tmp[2];
	DevLcdBusNode * node;
	
	node = bus_lcd_open(lcd->dev.buslcd);

	if(sta == 1)
	{
		bus_lcd_write_cmd(node, (0X07));
		tmp[0] = 0x0173;
		bus_lcd_write_data(node, (u8*)tmp, 1);
	}
	else
	{
		bus_lcd_write_cmd(node, (0X07));
		tmp[0] = 0x00;
		bus_lcd_write_data(node, (u8*)tmp, 1);
	}
	bus_lcd_close(node);
	return 0;
}

/**
 *@brief:	   drv_ILI9325_init
 *@details:    ��ʼ��FSMC�����Ҷ�ȡILI9325���豸ID
 *@param[in]   void  
 *@param[out]  ��
 *@retval:	   
 */
s32 drv_ILI9325_init(DevLcdNode *lcd)
{
	u16 data;
	u16 tmp[16];
	DevLcdBusNode * node;
	
	node = bus_lcd_open(lcd->dev.buslcd);

	bus_lcd_rst(node, 1);
	Delay(50);
	bus_lcd_rst(node, 0);
	Delay(50);
	bus_lcd_rst(node, 1);
	Delay(50);
	/*
		��9325��ID
		
	*/
	bus_lcd_write_cmd(node, (0x0000));
	tmp[0] = 0x0001;
	bus_lcd_write_data(node, (u8*)tmp, 1);
	
	bus_lcd_write_cmd(node, (0x0000));
	bus_lcd_read_data(node, (u8*)tmp, 1);
    data = tmp[0]; 

	wjq_log(LOG_DEBUG, "read reg:%04x\r\n", data);
	if(data != TftLcdILI9325Drv.id)
	{
		wjq_log(LOG_DEBUG, "lcd drive no 9325\r\n");	
		bus_lcd_close(node);
		return -1;
	}
	
	bus_lcd_write_cmd(node, (0x00E5));
	tmp[0] = 0x78F0;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x0001));
	tmp[0] = 0x0100;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x0002));
	tmp[0] = 0x0700;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x0003));
	tmp[0] = 0x1030;
	bus_lcd_write_data(node, (u8*)tmp, 1);	

	bus_lcd_write_cmd(node, (0x0004));
	tmp[0] = 0x0000;
	bus_lcd_write_data(node, (u8*)tmp, 1);	

	bus_lcd_write_cmd(node, (0x0008));
	tmp[0] = 0x0202;
	bus_lcd_write_data(node, (u8*)tmp, 1);	

	bus_lcd_write_cmd(node, (0x0009));
	tmp[0] = 0x0000;
	bus_lcd_write_data(node, (u8*)tmp, 1);	

	bus_lcd_write_cmd(node, (0x000A));
	tmp[0] = 0x0000;
	bus_lcd_write_data(node, (u8*)tmp, 1);	

	bus_lcd_write_cmd(node, (0x000C));
	tmp[0] = 0x0000;
	bus_lcd_write_data(node, (u8*)tmp, 1);	

	bus_lcd_write_cmd(node, (0x000D));
	tmp[0] = 0x0000;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x000F));
	tmp[0] = 0x0000;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x0010));
	tmp[0] = 0x0000;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x0011));
	tmp[0] = 0x0007;
	bus_lcd_write_data(node, (u8*)tmp, 1);	

	bus_lcd_write_cmd(node, (0x0012));
	tmp[0] = 0x0000;
	bus_lcd_write_data(node, (u8*)tmp, 1);	

	bus_lcd_write_cmd(node, (0x0013));
	tmp[0] = 0x0000;
	bus_lcd_write_data(node, (u8*)tmp, 1);	

	bus_lcd_write_cmd(node, (0x0007));
	tmp[0] = 0x0000;
	bus_lcd_write_data(node, (u8*)tmp, 1);	

	bus_lcd_write_cmd(node, (0x0010));
	tmp[0] = 0x1690;
	bus_lcd_write_data(node, (u8*)tmp, 1);	

	bus_lcd_write_cmd(node, (0x0011));
	tmp[0] = 0x0227;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x0012));
	tmp[0] = 0x009D;
	bus_lcd_write_data(node, (u8*)tmp, 1);	

	bus_lcd_write_cmd(node, (0x0013));
	tmp[0] = 0x1900;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x0029));
	tmp[0] = 0x0025;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x002B));
	tmp[0] = 0x000D;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x0030));
	tmp[0] = 0x0007;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x0031));
	tmp[0] = 0x0303;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x0032));
	tmp[0] = 0x0003;
	bus_lcd_write_data(node, (u8*)tmp, 1);	

	bus_lcd_write_cmd(node, (0x0035));
	tmp[0] = 0x0206;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x0036));
	tmp[0] = 0x0008;
	bus_lcd_write_data(node, (u8*)tmp, 1);	

	bus_lcd_write_cmd(node, (0x0037));
	tmp[0] = 0x0406;
	bus_lcd_write_data(node, (u8*)tmp, 1);	

	bus_lcd_write_cmd(node, (0x0038));
	tmp[0] = 0x0304;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x0039));
	tmp[0] = 0x0007;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x003C));
	tmp[0] = 0x0602;
	bus_lcd_write_data(node, (u8*)tmp, 1);	

	bus_lcd_write_cmd(node, (0x003D));
	tmp[0] = 0x0008;
	bus_lcd_write_data(node, (u8*)tmp, 1);	

	/*
	Horizontal and Vertical RAM Address Position 219*319
	����ɨ�贰��

	*/
	bus_lcd_write_cmd(node, (0x0050));
	tmp[0] = 0x0000;
	bus_lcd_write_data(node, (u8*)tmp, 1);	

	bus_lcd_write_cmd(node, (0x0051));
	tmp[0] = 0x00EF;
	bus_lcd_write_data(node, (u8*)tmp, 1);	

	bus_lcd_write_cmd(node, (0x0052));
	tmp[0] = 0x0000;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x0053));
	tmp[0] = 0x013F;
	bus_lcd_write_data(node, (u8*)tmp, 1);	

	//-------------------------------------
	bus_lcd_write_cmd(node, (0x0060));
	tmp[0] = 0xA700;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x0061));
	tmp[0] = 0x0001;
	bus_lcd_write_data(node, (u8*)tmp, 1);	

	bus_lcd_write_cmd(node, (0x006A));
	tmp[0] = 0x0000;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x0080));
	tmp[0] = 0x0000;
	bus_lcd_write_data(node, (u8*)tmp, 1);	

	bus_lcd_write_cmd(node, (0x0081));
	tmp[0] = 0x0000;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x0082));
	tmp[0] = 0x0000;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x0083));
	tmp[0] = 0x0000;
	bus_lcd_write_data(node, (u8*)tmp, 1);	

	bus_lcd_write_cmd(node, (0x0084));
	tmp[0] = 0x0000;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x0085));
	tmp[0] = 0x0000;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x0090));
	tmp[0] = 0x0010;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x0092));
	tmp[0] = 0x0600;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x0007));
	tmp[0] = 0x0133;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_write_cmd(node, (0x00));
	tmp[0] = 0x0022;
	bus_lcd_write_data(node, (u8*)tmp, 1);

	bus_lcd_close(node);
	return 0;
}
/**
 *@brief:      drv_ILI9325_xy2cp
 *@details:    ��xy����ת��ΪCP����
 *@param[in]   ��
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ILI9325_xy2cp(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey, u16 *hsa, u16 *hea, u16 *vsa, u16 *vea)
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
		XY�ᣬʵ������������ȡ���ں�����������
		CP�ᣬ�ǿ������Դ棬
		ӳ���ϵȡ����ɨ�跽��
	*/
	/* 
		�������û��ӽǵ�XY���꣬��LCDɨ���CP����Ҫ����һ���Ե�
		���ң�9325�ں�����ҲҪ����ӳ��
		
	*/
	if(lcd->dir == W_LCD)
	{
		*hsa = (lcd->height - ey) - 1;
		*hea = (lcd->height - sy) - 1;
		
		*vsa = sx;
		*vea = ex;
	}
	else
	{
		*hsa = sx;
		*hea = ex;
		*vsa = sy;
		*vea = ey;
	}
	
	return 0;
}
/**
 *@brief:      drv_ILI9325_drawpoint
 *@details:    ����
 *@param[in]   u16 x      
               u16 y      
               u16 color  
 *@param[out]  ��
 *@retval:     static
 */
static s32 drv_ILI9325_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color)
{
	u16 hsa,hea,vsa,vea;

	drv_ILI9325_xy2cp(lcd, x, x, y, y, &hsa,&hea,&vsa,&vea);
	drv_ILI9325_set_cp_addr(lcd, hsa, hea, vsa, vea);
	
	DevLcdBusNode * node;
	node = bus_lcd_open(lcd->dev.buslcd);
	
	u16 tmp[2];
	tmp[0] = color;
	bus_lcd_write_data(node, (u8*)tmp, 1);
	bus_lcd_close(node);
	return 0;
}
/**
 *@brief:      dev_ILI9325_color_fill
 *@details:    ��һ�������趨Ϊĳ����ɫ
 *@param[in]   u16 sx     
               u16 sy     
               u16 ex     
               u16 ey     
               u16 color  
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ILI9325_color_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 color)
{

	u16 height,width;
	u16 i,j;
	u16 hsa,hea,vsa,vea;

	drv_ILI9325_xy2cp(lcd, sx, ex, sy, ey, &hsa,&hea,&vsa,&vea);
	drv_ILI9325_set_cp_addr(lcd, hsa, hea, vsa, vea);

	width = hea - hsa + 1;//�õ����Ŀ��
	height = vea - vsa + 1;//�߶�
	
	//uart_printf("ili9325 width:%d, height:%d\r\n", width, height);
	//GPIO_ResetBits(GPIOG, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2| GPIO_Pin_3);
	
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

	//GPIO_SetBits(GPIOG, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2| GPIO_Pin_3);
	return 0;

}

/**
 *@brief:      dev_ILI9325_color_fill
 *@details:    ����������
 *@param[in]   u16 sx      
               u16 sy      
               u16 ex      
               u16 ey      
               u16 *color  ÿһ�������ɫ����
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ILI9325_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color)
{

	u16 height,width;
	u16 i,j;
	u16 hsa,hea,vsa,vea;

	drv_ILI9325_xy2cp(lcd, sx, ex, sy, ey, &hsa,&hea,&vsa,&vea);
	drv_ILI9325_set_cp_addr(lcd, hsa, hea, vsa, vea);

	width=(hea +1) - hsa ;//�õ����Ŀ�� +1����Ϊ�����0��ʼ
	height=(vea +1) - vsa;//�߶�
	
	//uart_printf("ili9325 width:%d, height:%d\r\n", width, height);
	//GPIO_ResetBits(GPIOG, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2| GPIO_Pin_3);

	DevLcdBusNode * node;

	node = bus_lcd_open(lcd->dev.buslcd);
	bus_lcd_write_data(node, (u8 *)color, height*width);	
	bus_lcd_close(node);

	//GPIO_SetBits(GPIOG, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2| GPIO_Pin_3);
	
	return 0;

} 

s32 drv_ILI9325_prepare_display(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey)
{
	u16 hsa,hea,vsa,vea;

	wjq_log(LOG_DEBUG, "XY:-%d-%d-%d-%d-\r\n", sx, ex, sy, ey);

	drv_ILI9325_xy2cp(lcd, sx, ex, sy, ey, &hsa,&hea,&vsa,&vea);

	wjq_log(LOG_DEBUG, "HV:-%d-%d-%d-%d-\r\n", hsa, hea, vsa, vea);
	
	drv_ILI9325_set_cp_addr(lcd, hsa, hea, vsa, vea);	
	return 0;
}
s32 drv_ILI9325_flush(DevLcdNode *lcd, u16 *color, u32 len)
{
	lcd->busnode = bus_lcd_open(lcd->dev.buslcd);
	bus_lcd_write_data(lcd->busnode, (u8 *)color,  len);	
	bus_lcd_close(lcd->busnode);
	return 0;
} 

#endif


