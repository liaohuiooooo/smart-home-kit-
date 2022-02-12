/**
 * @file            mcu_adc.c
 * @brief           adc����
 * @author          wujique
 * @date            2017��12��8�� ������
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2017��12��8�� ������
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

//#define MCU_ADC_DEBUG_G

#ifdef MCU_ADC_DEBUG_G
#define MCU_ADC_DEBUG	wjq_log 
#else
#define MCU_ADC_DEBUG(a, ...)
#endif

extern s32 dev_ts_adc_task(u16 dac_value);

/**
 *@brief:      mcu_adc_init
 *@details:    adc��ʼ�������ڵ��败�������
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     static
 */
void mcu_adc_init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//---ģ��ģʽ
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//---��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);//---��ʼ�� GPIO

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);//ʹ��ADC ʱ��

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = 	ADC_TwoSamplingDelay_20Cycles;//���������׶�֮����ӳ� 5 ��ʱ��
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA ʧ��
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;//Ԥ��Ƶ 6��Ƶ��
	ADC_CommonInit(&ADC_CommonInitStructure);//��ʼ��

	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12 λģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//��ɨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//������ת��
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//��ֹ������⣬ʹ���������
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;//��ֵ�Ǵ���Դ�������Ѿ���ֹ��������˱�ֵ������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���
	ADC_InitStructure.ADC_NbrOfConversion = 1;//1 ��ת���ڹ���������, Ҳ����˵һ��ת��һ��ͨ��
	ADC_Init(ADC2, &ADC_InitStructure);//ADC ��ʼ��

	#ifdef MCU_ADC_IRQ
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;	
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;      //��Ӧ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	ADC_ITConfig(ADC2,  ADC_IT_EOC,  ENABLE);//��ADC EOC�ж�
	ADC_ClearFlag(ADC2, ADC_FLAG_EOC);
	#endif
	
	ADC_Cmd(ADC2, ENABLE);	
}
/**
 *@brief:      mcu_adc_get_conv
 *@details:    ��ѯģʽ����ADCת��
 *@param[in]   u8 ch  
 *@param[out]  ��
 *@retval:     
 */
u16 mcu_adc_get_conv(u8 ch)
{
	u16 adcvalue;
	FlagStatus ret;
	
	//����ָ�� ADC �Ĺ�����ͨ����һ�����У�����ʱ��
	MCU_ADC_DEBUG(LOG_DEBUG, "str--");
	ADC_ClearFlag(ADC2, ADC_FLAG_OVR);

	ADC_RegularChannelConfig(ADC2, ch, 1, ADC_SampleTime_480Cycles ); 
	ADC_SoftwareStartConv(ADC2); 	//ʹ��ָ���� ADC �����ת����������
	
	while(1)//�ȴ�ת������
	{
		ret = ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC);
		if(ret == SET)
		{
			MCU_ADC_DEBUG(LOG_DEBUG, "ADC_FLAG_EOC\r\n");
			break;
		}
		ret = ADC_GetFlagStatus(ADC2, ADC_FLAG_AWD);
		if(ret == SET)
		{
			MCU_ADC_DEBUG(LOG_DEBUG, "ADC_FLAG_AWD\r\n");

		}
		ret = ADC_GetFlagStatus(ADC2, ADC_FLAG_JEOC);
		if(ret == SET)
		{
			MCU_ADC_DEBUG(LOG_DEBUG, "ADC_FLAG_JEOC\r\n");

		}
		ret = ADC_GetFlagStatus(ADC2, ADC_FLAG_JSTRT);
		if(ret == SET)
		{
			MCU_ADC_DEBUG(LOG_DEBUG, "ADC_FLAG_JSTRT\r\n");

		}

		ret = ADC_GetFlagStatus(ADC2, ADC_FLAG_STRT);
		if(ret == SET)
		{
			MCU_ADC_DEBUG(LOG_DEBUG, "ADC_FLAG_STRT\r\n");

		}

		ret = ADC_GetFlagStatus(ADC2, ADC_FLAG_OVR);
		if(ret == SET)
		{
			MCU_ADC_DEBUG(LOG_DEBUG, "ADC_FLAG_OVR\r\n");

		}
	}
	adcvalue = ADC_GetConversionValue(ADC2);
	return adcvalue;
}

/**
 *@brief:      mcu_adc_start_conv
 *@details:    ����ADCת��
 *@param[in]   u8 ch  
 *@param[out]  ��
 *@retval:     
 */
s32 mcu_adc_start_conv(u8 ch)
{	
	ADC_RegularChannelConfig(ADC2, ch, 1, ADC_SampleTime_480Cycles ); 
	ADC_SoftwareStartConv(ADC2); 	//ʹ��ָ���� ADC �����ת����������
	return 0;
}
/**
 *@brief:      mcu_adc_IRQhandler
 *@details:    ADC�жϷ������
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
void mcu_adc_IRQhandler(void)
{
	volatile u16 adc_value;
	FlagStatus ret;
	ITStatus itret;
	
	itret = ADC_GetITStatus(ADC2, ADC_IT_EOC);
	if( itret == SET)
    {    

		ret = ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC);
		if(ret == SET)
		{
			//uart_printf("ADC_FLAG_EOC t\r\n");
			adc_value = ADC_GetConversionValue(ADC2);
			//MCU_ADC_DEBUG(LOG_DEBUG, "%d ", adc_value);
			
			dev_ts_adc_task(adc_value);

		}
		
		ret = ADC_GetFlagStatus(ADC2, ADC_FLAG_AWD);
		if(ret == SET)
		{
			MCU_ADC_DEBUG(LOG_DEBUG, "ADC_FLAG_AWD t\r\n");

		}
		ret = ADC_GetFlagStatus(ADC2, ADC_FLAG_JEOC);
		if(ret == SET)
		{
			MCU_ADC_DEBUG(LOG_DEBUG, "ADC_FLAG_JEOC t\r\n");

		}
		ret = ADC_GetFlagStatus(ADC2, ADC_FLAG_JSTRT);
		if(ret == SET)
		{
			MCU_ADC_DEBUG(LOG_DEBUG, "ADC_FLAG_JSTRT t\r\n");

		}

		ret = ADC_GetFlagStatus(ADC2, ADC_FLAG_STRT);
		if(ret == SET)
		{
			//uart_printf("ADC_FLAG_STRT t\r\n");

		}

		ret = ADC_GetFlagStatus(ADC2, ADC_FLAG_OVR);
		if(ret == SET)
		{
			MCU_ADC_DEBUG(LOG_DEBUG, "ADC_FLAG_OVR t\r\n");
			ADC_ClearFlag(ADC2, ADC_FLAG_OVR);
		}
		
		ADC_ClearITPendingBit(ADC2, ADC_IT_EOC);
    }	
}

/*

	adc ���ԣ�ֻҪ����ͨ�����������ɡ�
	ֵ�Ƿ�׼ȷ���ڴ������������

*/
extern void Delay(__IO uint32_t nTime);

s32 mcu_adc_test(void)
{
	mcu_adc_init();
	
#ifndef MCU_ADC_IRQ/*��ѯģʽ*/
	u16 adc_value;

	wjq_log(LOG_FUN, "mcu_adc_test check\r\n");

	while(1)
	{
		adc_value = mcu_adc_get_conv(ADC_Channel_8);
	
		wjq_log(LOG_FUN, "ADC_Channel_8:%d\r\n", adc_value);
		Delay(1000);
		
		adc_value = mcu_adc_get_conv(ADC_Channel_9);
		wjq_log(LOG_FUN, "ADC_Channel_9:%d\r\n", adc_value);
		Delay(1000);
	}
#else/*�ж�ģʽ*/
	wjq_log(LOG_FUN, "mcu_adc_test int\r\n");

	while(1)
	{		
		wjq_log(LOG_FUN, "r ");
		mcu_adc_start_conv(ADC_Channel_8);
		Delay(1000);
		wjq_log(LOG_FUN, "d ");
		mcu_adc_start_conv(ADC_Channel_9);
		Delay(1000);
	}

#endif
}



/*

	����CPU�¶�	

*/
void  mcu_adc_temprate_init(void)
{	 
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);//ʹ��ADC1ʱ��

	//�ȳ�ʼ��IO��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;// ����
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��  

	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	//ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//��λ����	 

	ADC_TempSensorVrefintCmd(ENABLE);//ʹ���ڲ��¶ȴ�����

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMAʧ��
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; //ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz
	ADC_CommonInit(&ADC_CommonInitStructure);

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12λģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//��ɨ��ģʽ	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//��ֹ������⣬ʹ���������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���	
	ADC_InitStructure.ADC_NbrOfConversion = 1;//1��ת���ڹ��������� Ҳ����ֻת����������1 
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_480Cycles );	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_480Cycles );		
	ADC_Cmd(ADC1, ENABLE);//����ADת����	 	

}				  

u16 mcu_tempreate_get_adc_value(void)	 
{
	ADC_ClearFlag(ADC2, ADC_FLAG_OVR);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_480Cycles );				
	ADC_SoftwareStartConv(ADC1);		//ʹ��ָ����ADC1�����ת����������	
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������
	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}

u32 mcu_tempreate_get_tempreate(void)
{
	u32 adcx;

	adcx=mcu_tempreate_get_adc_value();	
	adcx = adcx*4125/128-27900;			
	//wjq_log(LOG_DEBUG, "%d-\r\n", adcx);
	
	return adcx;
}


