/**
 * @file            dev_touchscreen.c
 * @brief           ���ߵ������ӿ�+����ADC��ⷽ������
 * @author          wujique
 * @date            2018��4��15�� ������
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2018��4��15�� ������
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

#include "stm32f4xx.h"
#include "wujique_log.h"
#include "mcu_adc.h"
#include "mcu_timer.h"
#include "tslib.h"
#include "dev_touchscreen.h"
#include "dev_xpt2046.h"
#include "wujique_sysconf.h"


#define DEV_TS_DEBUG

#ifdef DEV_TS_DEBUG
#define TS_DEBUG	wjq_log 
#else
#define TS_DEBUG(a, ...)
#endif


s32 dev_ts_adc_init(void);
s32 dev_ts_adc_open(void);

/*  �������ӿڣ�
	�Բ�ͬ�Ĵ�����ⷽ����װΪͳһ�ӿڡ�
	���������ǣ�
		��ͬ�Ĵ�������IC��
		������ADCת����
	��ͬ�ķ�����������д�뻺�塣tslib��ӻ����ȡ���ݡ�
	*/
s32 TpSgd = -2;
#define  DEV_TP_QUE_MAX (250)//���г���, ��Ҫ��С
struct ts_sample DevTpSampleQue[DEV_TP_QUE_MAX];//ɨ��õ��ȶ��ĵ�Ķ���
volatile u16 TpQueWindex = 0;
volatile u16 TpQueRindex = 0;

/**
 *@brief:      dev_touchscreen_init
 *@details:    
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_touchscreen_init(void)
{
	TpSgd = -1;

	#ifdef SYS_USE_TS_ADC_CASE
	return dev_ts_adc_init();
	#endif

	#ifdef SYS_USE_TS_IC_CASE
	return dev_xpt2046_init();
	#endif	
}
/**
 *@brief:      dev_touchscreen_open
 *@details:    �򿪴�����������ADCת������
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_touchscreen_open(void)
{
	/*����ת��*/

	TpSgd = 0;
	
	TpQueWindex = TpQueWindex;
	

	#ifdef SYS_USE_TS_ADC_CASE
	return dev_ts_adc_open();
	#endif

	#ifdef SYS_USE_TS_IC_CASE
	return dev_xpt2046_open();
	#endif	

}

s32 dev_touchscreen_close(void)
{
	TpSgd = -1;
	
	#ifdef SYS_USE_TS_IC_CASE
	return dev_xpt2046_close();
	#endif	
	
}
/**
 *@brief:      dev_touchscreen_read
 *@details:    ������������ԭʼ���㣬Ӧ�ò����tslib����
 *@param[in]   struct ts_sample *samp  
               int nr                  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_touchscreen_read(struct ts_sample *samp, int nr)
{
	int i = 0;

	while(1)
	{
		if(i>=nr)
			break;

		if(TpQueWindex ==  TpQueRindex)
			break;

		samp->pressure = DevTpSampleQue[TpQueRindex].pressure;
		samp->x = DevTpSampleQue[TpQueRindex].x;
		samp->y = DevTpSampleQue[TpQueRindex].y;
		
		TpQueRindex++;
		if(TpQueRindex >= DEV_TP_QUE_MAX)
			TpQueRindex = 0;

		i++;
	}

	return i;
}
/**
 *@brief:      dev_touchscreen_write
 *@details:    ������д�뻺�壬�ײ����
 *@param[in]   struct ts_sample *samp  
               int nr                  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_touchscreen_write(struct ts_sample *samp, int nr)
{
	int index;
	struct ts_sample *p;
	
	index = 0;
	while(1)
	{
		if(index >= nr)
			break;	
		p = samp+index;
		
		DevTpSampleQue[TpQueWindex].pressure = p->pressure;//ѹ��Ҫ�������ô����
		DevTpSampleQue[TpQueWindex].x = p->x;
		DevTpSampleQue[TpQueWindex].y = p->y;
		TpQueWindex++;
		if(TpQueWindex >=  DEV_TP_QUE_MAX)
			TpQueWindex = 0;

		index++;
	}
		
	return index;
}

s32 dev_touchscreen_ioctrl(void)
{


	return 0;
}
/*

	adcת������

*/

#include "mcu_adc.h"

#define DEV_TP_S_PORT GPIOD
#define DEV_TP_S0 	  GPIO_Pin_11	
#define DEV_TP_S1 	  GPIO_Pin_12

#define DEV_TP_S0_L GPIO_ResetBits(DEV_TP_S_PORT, DEV_TP_S0)
#define DEV_TP_S0_H GPIO_SetBits(DEV_TP_S_PORT, DEV_TP_S0)

#define DEV_TP_S1_L GPIO_ResetBits(DEV_TP_S_PORT, DEV_TP_S1)
#define DEV_TP_S1_H GPIO_SetBits(DEV_TP_S_PORT, DEV_TP_S1)


#define DEV_TP_PRESS_SCAN {DEV_TP_S1_H;DEV_TP_S0_L;}
#define DEV_TP_SCAN_X {DEV_TP_S1_L;DEV_TP_S0_L;}
#define DEV_TP_SCAN_Y {DEV_TP_S1_L;DEV_TP_S0_H;}


/*
��������ѹ����ѹ��ֵ��С��PENdown����Ϊ���±ʣ�
����penup��Ϊ����ʣ�����֮�䲻����
���ֵ��ѹ����һ����
���͵�����ѹ����С��0��Ҳ������ʺ�
����ǵ�ѹ��ֵΪ0��ʱ�򣨿��ܴﲻ������
ͨ������ת����Ҳ����0-400��0��ʣ�400����ѹѹ��Ϊ0
*/
#define DEV_TP_PENDOWN_GATE (400)
#define DEV_TP_PENUP_GATE	 (2000)

static u8 TouchScreenStep = 9;
s32 TsAdcGd = -2;

s32 dev_ts_adc_init(void)
{
	#ifdef SYS_USE_TS_ADC_CASE
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin = DEV_TP_S0 | DEV_TP_S1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(DEV_TP_S_PORT, &GPIO_InitStructure);

	DEV_TP_PRESS_SCAN;

	mcu_adc_init();
	mcu_timer7_init();
	TsAdcGd = -1;
	#else
	wjq_log(LOG_INFO, ">>>>ts adc not init!\r\n");
	#endif
	return 0;
}

s32 dev_ts_adc_open(void)
{
	if(TsAdcGd != -1)
		return -1;
	/* wjq bug
	
	Ҫ�嶨ʱ����ADC�жϱ�־	
	*/
	DEV_TP_PRESS_SCAN;
	mcu_adc_start_conv(ADC_Channel_9);
	TsAdcGd = 0;
	return 0;
}

s32 dev_ts_adc_close(void)
{
		return 0;
}
/**
 *@brief:      dev_touchscreen_tr
 *@details:    ���¿�ʼһ�μ������
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
void dev_ts_adc_tr(void)
{
	if(TouchScreenStep	== 3)
	{
		mcu_adc_start_conv(ADC_Channel_8);
	}
	else
	{
		mcu_adc_start_conv(ADC_Channel_9);	
	}
}

/**
 *@brief:      dev_touchscreen_task
 *@details:    ������ADCת������
 *@param[in]   u16 adc_value  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_ts_adc_task(u16 dac_value)
{
	static u16 pre_y, pre_x;
	static u16 sample_x;
	static u8 pendownup = 1;
	struct ts_sample tss;
	
	if(TsAdcGd != 0)
		return -1;
	
	if(TouchScreenStep == 0)//ѹ������һ��ADCת������
	{	
		pre_y = dac_value;
		TouchScreenStep	= 1;
		mcu_adc_start_conv(ADC_Channel_8);
	}
	else if(TouchScreenStep == 1)
	{
		pre_x = dac_value;
		//TS_DEBUG(LOG_DEBUG, "--press :%d %d\r\n", pre_y, pre_x);

		if(pre_x + DEV_TP_PENDOWN_GATE > pre_y)
		{
			TouchScreenStep	= 2;
			DEV_TP_SCAN_X;
			mcu_tim7_start(2, dev_ts_adc_tr, 1);
		}
		else if(pre_x + DEV_TP_PENUP_GATE < pre_y)//ûѹ����������XY����
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
			TouchScreenStep	= 0;

			DEV_TP_PRESS_SCAN;
			//��һ����ʱ������ʱʱ�䵽�˲Ž���ѹ�����
			mcu_tim7_start(100, dev_ts_adc_tr, 1);
		}
		else
		{
			/*���±ʵĹ��ɣ�����*/
			TouchScreenStep	= 0;

			DEV_TP_PRESS_SCAN;

			mcu_tim7_start(20, dev_ts_adc_tr, 1);
		}
	}
	else if(TouchScreenStep == 2)
	{
		sample_x =  dac_value;
		
		TouchScreenStep	= 3;
		DEV_TP_SCAN_Y;
		mcu_tim7_start(2, dev_ts_adc_tr, 1);
	}
	else if(TouchScreenStep == 3)//һ�ֽ�������������ѹ�����
	{
		tss.pressure = DEV_TP_PENDOWN_GATE-(pre_y - pre_x);//ѹ��Ҫ�������ô����
		tss.x = sample_x;
		tss.y = dac_value;
		dev_touchscreen_write(&tss,1);
		//TS_DEBUG(LOG_DEBUG, "tp :%d, %d, %d\r\n", tss.pressure, tss.x, tss.y);
		pendownup = 0;
		
		TouchScreenStep	= 0;
		DEV_TP_PRESS_SCAN;
		mcu_tim7_start(2, dev_ts_adc_tr, 1);
	}
	else//�쳣������ѹ�����
	{
		TouchScreenStep	= 0;
		DEV_TP_PRESS_SCAN;
		mcu_tim7_start(100, dev_ts_adc_tr, 1);
	}

	return 0;
}

/**
 *@brief:      dev_touchscreen_test
 *@details:       ��������������
 *@param[in]  void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_touchscreen_test(void)
{
	/*��ѯ����ʹ���ж� ��MCU_ADC_IRQ����*/
#ifndef MCU_ADC_IRQ
	u16 adc_y_value;
	u16 adc_x_value;
	u16 pre;//ѹ����

	dev_touchscreen_init();
	dev_touchscreen_open();
	
	while(1)
	{
		Delay(1000);
		/*  ���ѹ�� */
		DEV_TP_PRESS_SCAN;
		adc_y_value = mcu_adc_get_conv(ADC_Channel_9);
		adc_x_value = mcu_adc_get_conv(ADC_Channel_8);
		pre = adc_y_value-adc_x_value;
		TS_DEBUG(LOG_FUN,"touch pre:%d, %d, %d\r\n", adc_x_value, adc_y_value, pre);	


		if(adc_x_value + 200 > adc_y_value)//200Ϊ���Է��ţ�ʵ����Ҫ����
		{
			/* ���X��*/
			DEV_TP_SCAN_X;
			adc_x_value = mcu_adc_get_conv(ADC_Channel_9);
			//uart_printf("ADC_Channel_8:%d\r\n", adc_x_value);
			
			/* ���Y��*/
			DEV_TP_SCAN_Y;
			adc_y_value = mcu_adc_get_conv(ADC_Channel_8);
			//uart_printf("ADC_Channel_8:%d\r\n", adc_y_value);

			TS_DEBUG(LOG_FUN,"----------get a touch:%d, %d, %d\r\n", adc_x_value, adc_y_value, pre);

		}

	}
#else//�ж�ģʽִ�д������������
	struct ts_sample s;
	s32 ret;
	u8 pensta = 1;//û�Ӵ�
	
	dev_touchscreen_init();
	dev_touchscreen_open();
	
	while(1)
	{
		ret = dev_touchscreen_read(&s,1);
		if(ret == 1)
		{
			if(s.pressure != 0 && pensta == 1)
			{
				pensta = 0;
				wjq_log(LOG_FUN, "pen down\r\n");
				wjq_log(LOG_FUN, ">%d %d %d-\r\n", s.pressure, s.x, s.y);
			}
			else if(s.pressure == 0 && pensta == 0)
			{	
				pensta = 1;
				wjq_log(LOG_FUN, "\r\n--------pen up--------\r\n");	

			}
		}
	}
#endif

}


