#ifndef _DEV_LCD_H_
#define _DEV_LCD_H_

#include "dev_lcdbus.h"
#include "font.h"

typedef struct _strDevLcdNode DevLcdNode;
/*
	LCD��������
*/
typedef struct  
{	
	u16 id;
	
	s32 (*init)(DevLcdNode *lcd);
	
	s32 (*draw_point)(DevLcdNode *lcd, u16 x, u16 y, u16 color);
	s32 (*color_fill)(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey, u16 color);
	s32 (*fill)(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color);
	
	s32 (*prepare_display)(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey);
	s32 (*flush)(DevLcdNode *lcd, u16 *color, u32 len);
	
	s32 (*onoff)(DevLcdNode *lcd, u8 sta);
	void (*set_dir)(DevLcdNode *lcd, u8 scan_dir);
	void (*backlight)(DevLcdNode *lcd, u8 sta);
}_lcd_drv; 


/*
	�豸����
	�������ط�ʽ����
	Ҳ����˵����һʲôID���豸����ʲô�ط�
	���綨��һ��COG LCD������SPI3��
	��ʲô������LCD���������ʲô��ͨ��IDƥ��
	ͬһ�����͵�LCD��������ͬ��ֻ�����ش�С��һ������δ���
	��������һ�������ṹ�壬����һ����ID��һ����
*/
typedef struct
{
	char name[DEV_NAME_SIZE]; //�豸����
	
	char buslcd[DEV_NAME_SIZE]; //��������LCD������
	
	u16 id;

	u16 width;	//LCD ���   ����
	u16 height;	//LCD �߶�    ����
}DevLcd;
/*
	��ʼ����ʱ�������豸�����壬
	����ƥ������������������ʼ��������
	�򿪵�ʱ��ֻ�ǻ�ȡ��һ��ָ��
*/
struct _strDevLcdNode
{
	s32 gd;//����������Ƿ���Դ�
	
	DevLcd	dev;

	/* LCD���� */
	_lcd_drv *drv;

	/*������Ҫ�ı���*/
	u8  dir;	//���������������ƣ�0��������1��������	
	u8  scandir;//ɨ�跽��
	u16 width;	//LCD ��� 
	u16 height;	//LCD �߶�

	void *pri;//˽�����ݣ��ڰ�����OLED���ڳ�ʼ����ʱ��Ὺ���Դ�

	DevLcdBusNode *busnode;

	struct list_head list;
};


#define H_LCD 0//����
#define W_LCD 1//����

//ɨ�跽����
//BIT 0 ��ʶLR��1 R-L��0 L-R
//BIT 1 ��ʶUD��1 D-U��0 U-D
//BIT 2 ��ʶLR/UD��1 DU-LR��0 LR-DU
#define LR_BIT_MASK 0X01
#define UD_BIT_MASK 0X02
#define LRUD_BIT_MASK 0X04

#define L2R_U2D  (0) //������,���ϵ���
#define L2R_D2U  (0 + UD_BIT_MASK)//������,���µ���
#define R2L_U2D  (0 + LR_BIT_MASK) //���ҵ���,���ϵ���
#define R2L_D2U  (0 + UD_BIT_MASK + LR_BIT_MASK) //���ҵ���,���µ���

#define U2D_L2R  (LRUD_BIT_MASK)//���ϵ���,������
#define U2D_R2L  (LRUD_BIT_MASK + LR_BIT_MASK) //���ϵ���,���ҵ���
#define D2U_L2R  (LRUD_BIT_MASK + UD_BIT_MASK) //���µ���,������
#define D2U_R2L  (LRUD_BIT_MASK + UD_BIT_MASK+ LR_BIT_MASK) //���µ���,���ҵ���	 

//������ɫ
/*
	���ںڰ���
	WHITE���ǲ���ʾ�����
	BLACK������ʾ
*/
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  

#define BLUE         	 0x001F  
#define GREEN         	 0x07E0
#define RED           	 0xF800

#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define MAGENTA       	 0xF81F
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //��ɫ
#define BRRED 			 0XFC07 //�غ�ɫ
#define GRAY  			 0X8430 //��ɫ
#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
#define LIGHTGREEN     	 0X841F //ǳ��ɫ
#define LIGHTGRAY      0XEF5B //ǳ��ɫ(PANNEL)
#define LGRAY 			 0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ
#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)

extern s32 dev_lcd_register(const DevLcd *dev);
extern DevLcdNode *dev_lcd_open(char *name);
extern s32 dev_lcd_close(DevLcdNode *node);
extern s32 dev_lcd_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color);
extern s32 dev_lcd_prepare_display(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey);
extern s32 dev_lcd_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color);
extern s32 dev_lcd_color_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 color);
extern s32 dev_lcd_backlight(DevLcdNode *lcd, u8 sta);
extern s32 dev_lcd_display_onoff(DevLcdNode *lcd, u8 sta);
extern s32 dev_lcd_setdir(DevLcdNode *node, u8 dir, u8 scan_dir);

extern s32 dev_lcd_put_string(DevLcdNode *lcd, FontType font, int x, int y, char *s, unsigned colidx);

extern void put_string_center(DevLcdNode *lcd, int x, int y, char *s, unsigned colidx);
extern s32 dev_lcd_setdir(DevLcdNode *lcd, u8 dir, u8 scan_dir);

#endif

