/**
 * @file            mcu_timer.c
 * @brief           CPUƬ�϶�ʱ������
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

#define MCU_TIME_DEBUG

#ifdef MCU_TIME_DEBUG
#define TIME_DEBUG	wjq_log 
#else
#define TIME_DEBUG(a, ...)
#endif


void mcu_tim5_test(void);


#define TestTim TIM5
/*
    ��ʱ��ʱ��Ϊ84M,
    Tout=((SYSTEM_CLK_PERIOD)*(SYSTEM_CLK_PRESCALER))/Ft us.

	Ԥ��Ƶ,8400��ʱ�ӲŴ���һ�ζ�ʱ������ 
	��ôһ����ʱ��������ʱ�����(1/84M)*8400 = 100us	  
*/
#define SYSTEM_CLK_PRESCALER    8400                  
#define SYSTEM_CLK_PERIOD       10000//��ʱ����


void mcu_tim5_test(void);
/**
 *@brief:      mcu_timer_init
 *@details:    ��ʱ����ʼ��
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 mcu_timer_init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    //�򿪶�ʱ��ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    //��λ��ʱ��
    TIM_Cmd(TestTim, DISABLE);
    TIM_SetCounter(TestTim, 0);
    
    //�趨TIM5�ж����ȼ�
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;	
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;      //��Ӧ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
    TIM_TimeBaseInitStruct.TIM_Period = SYSTEM_CLK_PERIOD - 1;  //����
    TIM_TimeBaseInitStruct.TIM_Prescaler = SYSTEM_CLK_PRESCALER-1;//��Ƶ
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 1;
    TIM_TimeBaseInit(TestTim, &TIM_TimeBaseInitStruct);
    
    TIM_ITConfig(TestTim, TIM_IT_Update, ENABLE);//�򿪶�ʱ���ж�
    
    TIM_Cmd(TestTim, ENABLE);//ʹ�ܶ�ʱ��(����)
		
		return 0;
}  
/**
 *@brief:      mcu_tim5_IRQhandler
 *@details:    ��ʱ���жϴ�����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
void mcu_tim5_IRQhandler(void)
{
    if(TIM_GetITStatus(TIM5, TIM_FLAG_Update) == SET)
    {                                       
        TIM_ClearFlag(TIM5, TIM_FLAG_Update);

        mcu_tim5_test();
 
    }
}
/**
 *@brief:      mcu_tim5_test
 *@details:    ��ʱ������
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
void mcu_tim5_test(void)
{
    static u8 i = 0;

    i++;
    if(1 == i)
    {
        TIME_DEBUG(LOG_DEBUG, "tim int 1\r\n");    
    }
    else if(2 == i)
    {
        TIME_DEBUG(LOG_DEBUG, "tim int 2\r\n");
    }
    else if(3 == i)
    {
        TIME_DEBUG(LOG_DEBUG, "tim int 3\r\n");
    }
    else
    {
        TIME_DEBUG(LOG_DEBUG, "tim int 4\r\n");
        i = 0;   
    }
}

/*

		�������ö�ʱ��4 PWM����

*/


/**
 *@brief:      mcu_tim4_pwm_init
 *@details:    ��ʼ����ʱ��4PWM���ܣ�Ĭ�����õ���CH2
 *@param[in]   u32 arr  ��ʱ����  
               u32 psc  ʱ��Ԥ��Ƶ
 *@param[out]  ��
 *@retval:     
 */
void mcu_tim4_pwm_init(u32 arr,u32 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);//---TIM4 ʱ��ʹ��

    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_Prescaler = psc - 1; //---��ʱ����Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //---���ϼ���ģʽ
    TIM_TimeBaseStructure.TIM_Period= arr - 1; //---�Զ���װ��ֵ
    TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);//---��ʼ����ʱ�� 4
    
    //----��ʼ�� TIM4 PWM ģʽ
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //---PWM ����ģʽ 1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //---�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //---������Ե�

	/*Ĭ�����õ���ͨ��2*/
    TIM_OC2Init(TIM4, &TIM_OCInitStructure); //---��ʼ������ TIM4
    TIM_SetCompare2(TIM4, arr/2);//---ռ�ձ�50%
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable); //---ʹ��Ԥװ�ؼĴ���
    TIM_ARRPreloadConfig(TIM4,ENABLE);

}

/*


	��ʱ������

*/
/**
 *@brief:      mcu_timer_cap_init
 *@details:    ��ʼ����ʱ�����񣬲�ʹ���ж�
 *@param[in]   u32 arr  
               u16 psc  
 *@param[out]  ��
 *@retval:     
 */
void mcu_timer_cap_init(u32 arr,u16 psc)
{

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM2_ICInitStructure;

	//��ʼ�� TIM2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // ʱ��ʹ��
	TIM_TimeBaseStructure.TIM_Period = arr; //�趨�������Զ���װֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //Ԥ��Ƶ�� 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM ���ϼ���
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); // ��ʼ����ʱ�� 2

	//��ʼ��ͨ�� 4
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_4; //ѡ������� IC4 ӳ�䵽 TIM2
	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling; //�½��ز���
	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; //���������Ƶ,����Ƶ
	TIM2_ICInitStructure.TIM_ICFilter = 0x00;//���������˲��� ���˲�
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);//��ʼ�� TIM2 IC4

	TIM_ClearITPendingBit(TIM2, TIM_IT_CC4|TIM_IT_Update); //����жϱ�־
	TIM_SetCounter(TIM2,0); 

	TIM_Cmd(TIM2,ENABLE); //ʹ�ܶ�ʱ�� 2
}
/**
 *@brief:      mcu_timer_get_cap
 *@details:    ��ѯ��ȡ��ʱȥ����ֵ
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     ����ֵ����ʱ�򷵻����ֵ	
 */
u32 mcu_timer_get_cap(void)
{ 

	while(TIM_GetFlagStatus(TIM2, TIM_IT_CC4) == RESET)//�ȴ�����������
	{
		if(TIM_GetCounter(TIM2) > 0xffffffff-1000)
			return TIM_GetCounter(TIM2);//��ʱ��,ֱ�ӷ��� CNT ��ֵ
	}
	return TIM_GetCapture4(TIM2);
}

/*

	��ʱ��3����DAC�������Ŷ�ʱ�������ݵ�DAC

*/

#define DacTim TIM3
#define TIM3_CLK_PRESCALER    84 //Ԥ��Ƶ,84��ʱ�ӲŴ���һ�ζ�ʱ������ 
                                    //һ����ʱ��������ʱ�����(1/84M)*84 = 1us                       
#define TIM3_CLK_PERIOD       125//��ʱ���� 125us

/**
 *@brief:      mcu_timer_init
 *@details:    ��ʱ����ʼ��
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
void mcu_tim3_init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

    //�򿪶�ʱ��ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    //��λ��ʱ��
    TIM_Cmd(DacTim, DISABLE);
    TIM_SetCounter(DacTim, 0);
    
    //�趨TIM7�ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;      //��Ӧ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
    TIM_TimeBaseInitStruct.TIM_Period = TIM3_CLK_PERIOD - 1;  //����
    TIM_TimeBaseInitStruct.TIM_Prescaler = TIM3_CLK_PRESCALER-1;//��Ƶ
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 1;
    TIM_TimeBaseInit(DacTim, &TIM_TimeBaseInitStruct);
    
    TIM_ITConfig(DacTim, TIM_IT_Update, ENABLE);//�򿪶�ʱ���ж�

}  

s32 mcu_tim3_start(void)
{
	 TIM_Cmd(DacTim, ENABLE);//ʹ�ܶ�ʱ��(����)	
	return 0;
}

s32 mcu_tim3_stop(void)
{
	TIM_Cmd(DacTim, DISABLE);//ֹͣ��ʱ��	
	return 0;
}

extern s32 dev_dacsound_timerinit(void);
/**
 *@brief:      mcu_tim6_IRQhandler
 *@details:    ��ʱ���жϴ�����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
void mcu_tim3_IRQhandler(void)
{
    if(TIM_GetITStatus(DacTim, TIM_FLAG_Update) == SET)
    {                                       
        TIM_ClearFlag(DacTim, TIM_FLAG_Update);

		dev_dacsound_timerinit();

    }
}

/*

	ʹ���ڲ�ADCת����ⴥ������
	�ö�ʱ������ȷ��ʱ

*/
void (*Tim7Callback)(void);
u8 Tim7Type = 1;//1��ִ��һ�Σ����ظ���0�ظ�

#define TpTim TIM7
#define TIM7_CLK_PRESCALER    840 //Ԥ��Ƶ,840��ʱ�ӲŴ���һ�ζ�ʱ������ 
                                    //һ����ʱ��������ʱ�����(1/84M)*840 = 10us                       
#define TIM7_CLK_PERIOD       100//��ʱ���� 1����

/**
 *@brief:      mcu_timer_init
 *@details:    ��ʱ����ʼ��
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
void mcu_timer7_init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    //�򿪶�ʱ��ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
    //��λ��ʱ��
    TIM_Cmd(TpTim, DISABLE);
    TIM_SetCounter(TpTim, 0);
    
    //�趨TIM7�ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;	
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;      //��Ӧ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
    TIM_ITConfig(TpTim, TIM_IT_Update, ENABLE);//�򿪶�ʱ���ж�
    
    //TIM_Cmd(TpTim, ENABLE);//ʹ�ܶ�ʱ��(����)
	Tim7Callback = NULL;
}  

s32 mcu_tim7_start(u32 Delay_10us, void (*callback)(void), u8 type)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

	Tim7Callback = callback;
	Tim7Type = type;
	//��λ��ʱ��
    TIM_Cmd(TpTim, DISABLE);
    TIM_SetCounter(TpTim, 0);

	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
    TIM_TimeBaseInitStruct.TIM_Period = Delay_10us - 1;  //����
    TIM_TimeBaseInitStruct.TIM_Prescaler = TIM7_CLK_PRESCALER-1;//��Ƶ
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 1;
    TIM_TimeBaseInit(TpTim, &TIM_TimeBaseInitStruct);

	TIM_ClearFlag(TpTim, TIM_FLAG_Update);
	
	TIM_ITConfig(TpTim, TIM_IT_Update, ENABLE);//�򿪶�ʱ���ж�

	TIM_Cmd(TpTim, ENABLE);//ʹ�ܶ�ʱ��(����)	
	
	return 0;
}
/**
 *@brief:      mcu_tim6_IRQhandler
 *@details:    ��ʱ���жϴ�����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
void mcu_tim7_IRQhandler(void)
{
    if(TIM_GetITStatus(TpTim, TIM_FLAG_Update) == SET)
    {                                       
        TIM_ClearFlag(TpTim, TIM_FLAG_Update);
		if(Tim7Type == 1)
			TIM_Cmd(TpTim, DISABLE);//ֹͣ��ʱ��
			
		Tim7Callback();
		
    }
}

