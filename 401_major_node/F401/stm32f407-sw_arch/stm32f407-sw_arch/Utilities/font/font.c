/**
 * @file            font.c
 * @brief           �ֿ����
 * @author          wujique
 * @date            2018��3��2�� ������
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2018��3��2�� ������
 *   ��    ��:       �ݼ�ȸ������
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
#include "stm32f4xx.h"
#include "wujique_log.h"
#include "font.h"
#include "ff.h"

/*
	��õķ�����ʵ�����ֿ��Ծ٣�Ҳ�������ֿ��ͷ�������ֿ���Ϣ��
	��ʱ������ô�����ˡ�
*/

/*

	�����ֿ�ֻʹ�ö�����ֵ����ֿ����������ɵĵ����ֿ⣬
	ʹ��ģʽ�����ɣ�Ҳ�����ݿ⡣

*/
/*����*/
FIL FontFileST1616;
FIL FontFileST1212;

/*˼Դ*/
FIL FontFileSY1616;
FIL FontFileSY1212;

struct fbcon_font_desc font_songti_16x16 = {
	HZ_1616_IDX,
	"1:/songti1616.DZK",//������SD��
	16,
	16,
	(char *)&FontFileST1616,
	0,
	32
};
	
struct fbcon_font_desc font_songti_12x12 = {
	HZ_1212_IDX,
	"1:/songti1212.DZK",
	12,
	12,
	(char *)&FontFileST1212,
	0,
	24
};

struct fbcon_font_desc font_siyuan_16x16 = {
	HZ_1616_IDX,
	"1:/shscn1616.DZK",//������SD��
	16,
	16,
	(char *)&FontFileSY1616,
	0,
	32
};
	
struct fbcon_font_desc font_siyuan_12x12 = {
	HZ_1212_IDX,
	"1:/shscn1212.DZK",
	12,
	12,
	(char *)&FontFileSY1212,
	0,
	24
};

const struct fbcon_font_desc *FontList[FONT_LIST_MAX]=
	{
		&font_songti_16x16,
		&font_songti_12x12,
		&font_siyuan_16x16,
		&font_siyuan_12x12
	};

/*

	asc�ַ�����ʹ�ú�⡣
	ASC�ֿ�������Դ�����С�
*/
const struct fbcon_font_desc *FontAscList[FONT_LIST_MAX]=
		{
			&font_vga_8x16,
			&font_vga_6x12,
			&font_vga_8x16,
			&font_vga_6x12
		};

s32 FontInit = -1;		
/**
 *@brief:      font_check_hzfont
 *@details:    ����ֿ⣬��Ҫ�Ǻ��ֿ�
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 font_check_hzfont(void)
{
	u8 i;
	FRESULT res;
	FIL* fp;
	
	i = 0;
	while(1)
	{
		if(i>= FONT_LIST_MAX)
			break;
		
		fp = (FIL*)(FontList[i]->data);		
		res = f_open(fp, FontList[i]->name, FA_READ);

		
		if(res != FR_OK)
		{
			wjq_log(LOG_INFO, "font open file:%s, err:%d!\r\n", FontList[i]->name, res);
		}
		else
		{
			wjq_log(LOG_INFO, "font open file :%s ok!\r\n", FontList[i]->name);	
		}
		
		i++;
	}

	FontInit = 0;
	return 0;
}

/**
 *@brief:      font_get_hz
 *@details:    ��ȡ���ֵ���
 *@param[in]   FontType type  
               char *ch       
               char *buf      
 *@param[out]  ��
 *@retval:     
 */
s32 font_get_hz(FontType type, u8 *ch, u8 *buf)
{
	FRESULT res;
	unsigned int len;
	u32 addr;
	u8 hcode,lcode;

	FIL* fontp;

	if(type >= FONT_LIST_MAX)
		return -1;

	if(FontInit == -1)
		font_check_hzfont();
	
	hcode = *ch;
	lcode = *(ch+1);
	
	if((hcode < 0x81) 
		|| (hcode > 0xfe)
		)
	{
		//uart_printf("no china hz\r\n");
		return -1;
	}

	//uart_printf("hz code:%02x, %02x\r\n", hcode, lcode);

	addr = (hcode-0x81)*190;
	if(lcode<0x7f)
	{
		addr = addr+lcode-0x40;	
	}
	else
	{
		addr = addr+lcode-0x41;	
	}
	addr = addr*FontList[type]->size;
	//uart_printf("adr:%08x\r\n", addr);
	
	fontp = (FIL*)FontList[type]->data;
	if(fontp->fs == 0)
	{
		//uart_printf("no font\r\n");
		return -1;
	}
	
	res = f_lseek(fontp, addr);
	if(res != FR_OK)
	{
		return 0;
	}

	res = f_read(fontp, (void *)buf, FontList[type]->size, &len);
	if((res != FR_OK) || (len!= FontList[type]->size))
	{
		//uart_printf("font read err\r\n");
		return -1;
	}
	
	return 0;
	
}
/**
 *@brief:      font_get_asc
 *@details:    ��ȡASC�ַ���������
 *@param[in]   FontType type  
               char *ch       
               char *buf      
 *@param[out]  ��
 *@retval:     
 */
s32 font_get_asc(FontType type, u8 *ch, u8 *buf)
{
	u8* fp;
	
	if(*ch >= 0x80)
		return -1;

	if(type > FONT_LIST_MAX)
		return -1;

	fp = (u8*)FontAscList[type]->data + (*ch)*FontAscList[type]->size;
	//wjq_log(LOG_DEBUG, "dot data\r\n");
	//PrintFormat(fp, 16);
	
	memcpy(buf, fp, FontAscList[type]->size);

	return 0;
}
/**
 *@brief:      font_get_hw
 *@details:    ��ȡ���峤��
 *@param[in]   FontType type  
               u8 *h       
               u8 *w      
 *@param[out]  ��
 *@retval:     
 			���ص��ǵ����ַ�����Ҳ���Ƕ�Ӧ��ASC��ȣ������������ַ����
 */

s32 font_get_hw(FontType type, u8 *h, u8 *w)
{

	if(type >= FONT_LIST_MAX)
		return -1;

		*w = FontAscList[type]->width;
		*h = FontAscList[type]->height;

	return 0;
}

