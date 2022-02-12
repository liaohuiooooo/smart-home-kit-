/**
 * @file            mcu_dac.c
 * @brief           STM32Ƭ��DAC����
 * @author          test
 * @date            2017��11��1�� ������
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2017��11��1�� ������
 *   ��    ��:      �ݼ�ȸ������
 *   �޸�����:      �����ļ�
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

//#define MCU_DAC_DEBUG

#ifdef MCU_DAC_DEBUG
#define MCU_DAC_DEBUG	wjq_log 
#else
#define MCU_DAC_DEBUG(a, ...)
#endif



s32 mcu_dac_init(void)
{
	return 0;
}
/**
 *@brief:      mcu_dac_open
 *@details:    ��DAC������
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 mcu_dac_open(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitType;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//----ʹ�� PA ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);//----ʹ�� DAC ʱ��
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//---ģ��ģʽ
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//---����
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);//---��ʼ�� GPIO
    
    DAC_InitType.DAC_Trigger=DAC_Trigger_None;  //---��ʹ�ô������� TEN1=0
    DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;   //---��ʹ�ò��η���
    DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;
    DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Enable ;        //---�������ر�
    //DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_4095; //����������
	DAC_Init(DAC_Channel_2,&DAC_InitType); //---��ʼ�� DAC ͨ�� 2    

	DAC_Cmd(DAC_Channel_2, ENABLE); //---ʹ�� DAC ͨ�� 2
    DAC_SetChannel2Data(DAC_Align_12b_R, 0); //---12 λ�Ҷ������ݸ�ʽ   ���0 
		
		return 0;
}
/**
 *@brief:      mcu_dac_output
 *@details:    ����DAC���ֵ
 *@param[in]   u16 vol�� ��ѹ����λMV��0-Vref  
 *@param[out]  ��
 *@retval:     
 */
s32 mcu_dac_output_vol(u16 vol)
{

    u32 temp;
    
    temp = (0xfff*vol)/3300;

    MCU_DAC_DEBUG(LOG_DEBUG, "\r\n---test dac data:%d-----\r\n", temp);
    
    DAC_SetChannel2Data(DAC_Align_12b_R, temp);//12 λ�Ҷ������ݸ�ʽ
		return 0;
}
/**
 *@brief:      mcu_dac_output
 *@details:    ��һ����ֵ��ΪDACֵ���
 *@param[in]   u16 data  
 *@param[out]  ��
 *@retval:     
 */
void mcu_dac_output(u16 data)
{
    DAC_SetChannel2Data(DAC_Align_12b_R, data);//12 λ�Ҷ������ݸ�ʽ
}


/**
 *@brief:      mcu_dac_test
 *@details:    DAC���Գ���
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 mcu_dac_test(void)
{
    wjq_log(LOG_INFO, "\r\n---test dac!-----\r\n");
    
    mcu_dac_open();
    mcu_dac_output_vol(1500);//1.5v
    while(1);
}

