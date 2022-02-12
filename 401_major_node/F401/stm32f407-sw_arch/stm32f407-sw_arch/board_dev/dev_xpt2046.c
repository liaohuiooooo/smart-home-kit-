/**
 * @file            dev_xpt2046.c
 * @brief           ���败��������оƬxpt2046
 * @author          wujique
 * @date            2018��3��21�� ������
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2018��3��21�� ������
 *   ��    ��:         wujique
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

#include "stm32f4xx.h"
#include "wujique_log.h"
#include "tslib.h"

#include "mcu_spi.h"
#include "dev_touchscreen.h"
#include "mcu_timer.h"
#include "wujique_sysconf.h"

extern s32 dev_touchscreen_write(struct ts_sample *samp, int nr);

/*
	���������壺
	bit7��S��������־���̶�Ϊ1
	bit6-bit4��A2/A1/A0,ͨ��ѡ�񣬲��ģʽ�����˲�һ��������ֻ˵���
		001����Y��
		011����Z1
		100����Z2
		101����X��
	bit3 ��MODE,1 =8λADC��0 =12λADC
	bit2��SEF/DFR,����ģʽ���ǲ��ģʽ��XY���ѹ�������ò�֣�����ò�֣�����ֻ���õ���
	bit1-bit0��PD1/PD0, ����ģʽ 11�����Ǵ��ڹ���״̬��00���ڱ任֮�����͹���

	����ѹ��ʱ��Z2��ѹֵ����Z1�������ߵĲС����ֵ������Ϊ�ʰ���
*/
#define XPT2046_CMD_X 0xd0
#define XPT2046_CMD_Y 0X90
#define XPT2046_CMD_Z1 0xb0
#define XPT2046_CMD_Z2 0xc0

/*
��������ѹ����ѹ��ֵ��С��PENdown����Ϊ���±ʣ�
����penup��Ϊ����ʣ�����֮�䲻����
���ֵ��ѹ����һ����
���͵�����ѹ����С��0��Ҳ������ʺ�
����ǵ�ѹ��ֵΪ0��ʱ�򣨿��ܴﲻ������
ͨ������ת����Ҳ����0-400��0��ʣ�400����ѹѹ��Ϊ0

*/
#define DEV_XPT2046_PENDOWN_GATE (3500)
#define DEV_XPT2046_PENUP_GATE	 (3700)


s32 DevXpt2046Gd = -2;
DevSpiChNode *Xpt2046SpiCHNode;

void dev_xpt2046_task(void);

/**
 *@brief:      dev_xpt2046_init
 *@details:    ��ʼ��XPT2046
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_xpt2046_init(void)
{
	#ifdef SYS_USE_TS_IC_CASE
	DevXpt2046Gd = -1;
	
	wjq_log(LOG_INFO, ">-----------xpt2046 init!\r\n");
	mcu_timer7_init();
	#else
	wjq_log(LOG_INFO, ">-----------xpt2046 not init!\r\n");

	#endif

	return 0;

}
/**
 *@brief:      dev_xpt2046_open
 *@details:    ��xpt2046
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_xpt2046_open(void)
{

	if(DevXpt2046Gd != -1)
		return -1;

	wjq_log(LOG_INFO, ">--------------xpt2046 open!\r\n");

	mcu_tim7_start(100, dev_xpt2046_task, 0);
	DevXpt2046Gd = 0;
	return 0;
}
/**
 *@brief:      dev_xpt2046_close
 *@details:    �ر�XPT2046
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_xpt2046_close(void)
{

	if(DevXpt2046Gd != 0)
		return -1;

	DevXpt2046Gd = -1;
	return 0;
}
/**
 *@brief:      dev_xpt2046_task
 *@details:    xpt2046�������
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
void dev_xpt2046_task(void)
{

	static u16 pre_y, pre_x;
	static u16 sample_y, sample_x;

	static u8 pendownup = 1;
	struct ts_sample tss;
	
	u8 stmp[4];
	u8 rtmp[4];
	
	if(DevXpt2046Gd != 0)
		return;
	/*

		�������̷��Ĳ�
		��Z1,Z2�����ڼ���ѹ��
		��X,Y�ᣬ���ڼ�������

		1 ʹ���˿���16CLK������������100us����, ����SPIͨ�Ź���50us��
		�����ԣ��м䲻��Ҫ��ʱ��
		2 û��ʹ���±��жϣ�ͨ��ѹ���ж��Ƿ��±ʡ������е��ɻ������ϽӴ�����Ӧ�ú�С�ģ�
		 ��ADC��������������XPT2046�������о��Ӵ�����Ƚϴ󣬲�֪��������û�����ԡ�

		3 ����CLK������Ҳ�����ڶ����һ���ֽڵ�ʱ��ͬʱ������һ��ת�����
		  һ��Ҫ��һ���ֽڷ���0X00���ڶ����ֽڷ�����������һ���ֽڲ���00����������BIT7��1��
		  оƬ����������ת�����������ĵ�ѹֵ�Ͷ������ˡ�

		4 ʵ�⣬����Ҫ��ʱ��������SPIʱ�ӽϿ죬��ע����ʱ�ȴ�ת��������
		5 �����ϻ����Խ�ʡһ���ֽڵķ���ʱ�䣬�������Ż���
	*/
	
	/*------------------------*/
	GPIO_ResetBits(GPIOG, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2| GPIO_Pin_3);
	
	Xpt2046SpiCHNode = mcu_spi_open(XPT2046_SPI, SPI_MODE_0, XPT2046_SPI_PRE);
	//Xpt2046SpiCHNode = NULL;
	if(Xpt2046SpiCHNode == NULL)
		return;
	
	stmp[0] = XPT2046_CMD_Z2;
	mcu_spi_transfer(Xpt2046SpiCHNode, stmp, NULL, 1);
	//vspi_delay(100);
	stmp[0] = 0x00;
	stmp[1] = XPT2046_CMD_Z1;
	mcu_spi_transfer(Xpt2046SpiCHNode, stmp, rtmp, 2);
	//wjq_log(LOG_DEBUG, "%d, %d- ", rtmp[0], rtmp[1]);
	
	pre_y = ((u16)(rtmp[0]&0x7f)<<5) + (rtmp[1]>>3);
	/*------------------------*/
	//vspi_delay(100);
	stmp[0] = 0x00;
	stmp[1] = XPT2046_CMD_X;
	mcu_spi_transfer(Xpt2046SpiCHNode, stmp, rtmp, 2);
	pre_x = ((u16)(rtmp[0]&0x7f)<<5) + (rtmp[1]>>3);
	/*------------------------*/
	//vspi_delay(100);
	stmp[0] = 0x00;
	stmp[1] = XPT2046_CMD_Y;
	mcu_spi_transfer(Xpt2046SpiCHNode, stmp, rtmp, 2);
	sample_x = ((u16)(rtmp[0]&0x7f)<<5) + (rtmp[1]>>3);
	/*------------------------*/
	//vspi_delay(100);
	stmp[0] = 0x00;
	stmp[1] = 0X00;
	mcu_spi_transfer(Xpt2046SpiCHNode, stmp, rtmp, 2);
	sample_y = ((u16)(rtmp[0]&0x7f)<<5) + (rtmp[1]>>3);
	
	mcu_spi_close(Xpt2046SpiCHNode);
	GPIO_SetBits(GPIOG, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2| GPIO_Pin_3);
	/*
		��ѹ��
		���㷨
		ʵ�ʣ�
		R��������=Rx���*��Xλ��/4096��*(Z2/Z1-1)
	*/
	if(pre_x +  DEV_XPT2046_PENDOWN_GATE > pre_y)
	{
		/*��ѹ��*/
		tss.pressure = 200;//DEV_XPT2046_PENDOWN_GATE - rpress;
		tss.x = sample_x;
		tss.y = sample_y;
		dev_touchscreen_write(&tss,1);
		//uart_printf("%d,%d,%d\r\n", tss.pressure, tss.x, tss.y);
		pendownup = 0;
	}
	else if(pre_x + DEV_XPT2046_PENUP_GATE < pre_y)//ûѹ����������XY����
	{
		/* ���ֻ����һ�㻺��*/
		if(pendownup == 0)
		{
			pendownup = 1;
			tss.pressure = 0;//ѹ��Ҫ�������ô����
			tss.x = 0xffff;
			tss.y = 0xffff;
			dev_touchscreen_write(&tss,1);
		}

	}
	else
	{
		//uart_printf("--press :%d %d\r\n", pre_y, pre_x);
		/*���±ʵĹ��ɣ�����*/

	}

}


