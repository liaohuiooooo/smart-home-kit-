/**
 * @file            dev_buzzer.c
 * @brief           PWM����������
 * @author          test
 * @date            2017��10��25�� ������
 * @version         ����
 * @par             
 * @par History:
 * 1.��    ��:      2017��10��25�� ������
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
#include "mcu_timer.h"

/*
    ��ʱ��ʱ��Ϊ84M,
    Tout=((SYSTEM_CLK_PERIOD)*(SYSTEM_CLK_PRESCALER))/Ft us.
*/
#define BUZZER_CLK_PRESCALER    84 //Ԥ��Ƶ,84��ʱ�Ӵ���һ�ζ�ʱ������ 
                                    //һ����ʱ��������ʱ�����(1/84M)*84 = 1us                       
#define BUZZER_CLK_PERIOD       250//��ʱ���ڣ�250*1=250us, Ƶ��������4K������������ӦƵ�ʡ�

/**
 *@brief:      dev_buzzer_init
 *@details:    ��ʼ��������
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_buzzer_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); //---ʹ�� GPIOD ʱ��
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_TIM4); //---�ܽŸ���Ϊ TIM4����
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //---���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //---�ٶ� 50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //---���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //---����
    GPIO_Init(GPIOD,&GPIO_InitStructure);
	
    mcu_tim4_pwm_init(BUZZER_CLK_PERIOD,BUZZER_CLK_PRESCALER);
	
	return 0;
}
/**
 *@brief:      dev_buzzer_open
 *@details:    �򿪷�����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_buzzer_open(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); //---ʹ�� GPIOD ʱ��
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_TIM4); //---�ܽŸ���Ϊ TIM4����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //---���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //---�ٶ� 50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //---���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //---����
    GPIO_Init(GPIOD,&GPIO_InitStructure);
	
    TIM_Cmd(TIM4, ENABLE); //---ʹ�� TIM4 
	
	return 0;
}
/**
 *@brief:      dev_buzzer_close
 *@details:    �رշ�����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_buzzer_close(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
    TIM_Cmd(TIM4, DISABLE); //---�رն�ʱ�� TIM4 

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); //---ʹ�� GPIOD ʱ��

	/*�رշ�����ʱ��Ҫ��IO��Ϊ��ͨIO����������͵�ƽ���������������ɴ����*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //---���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //---�ٶ� 50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //---���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //---����
    GPIO_Init(GPIOD,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOD, GPIO_Pin_13);
	
	return 0;
}
/**
 *@brief:      dev_buzzer_test
 *@details:    ���Է�����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
void dev_buzzer_test(void)
{
    
}

