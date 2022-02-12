/**
 * @file                dev_key.c
 * @brief           ����ɨ������
 * @author          wujique
 * @date            2018��2��1�� ������
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2018��2��1�� ������
 *   ��    ��:         wujique
 *   �޸�����:   �����ļ�
 ��Ȩ˵����
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
#include "dev_key.h"

//#define DEV_KEY_DEBUG

#ifdef DEV_KEY_DEBUG
#define KEY_DEBUG	wjq_log 
#else
#define KEY_DEBUG(a, ...)
#endif

extern void Delay(__IO uint32_t nTime);

/*��������*/
#define KEY_BUF_SIZE (12)
u8 KeyBuf[KEY_BUF_SIZE];
u8 KeyR =0;
u8 KeyW = 0;
/*�����豸��*/
s32 KeyGd = -1;
/*  ������������ʱ����ڴ�������scan keyִ�м��*/
#define DEV_KEY_DEBOUNCE	(10)
/**
 *@brief:      dev_key_init
 *@details:    ��ʼ������
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//ʹ�� GPIOA ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//��Ӧ���� PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ�� GPIOA0	
	return 0;
}
/**
 *@brief:      dev_key_open
 *@details:    �򿪰����豸
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_key_open(void)
{
	KeyGd = 0;
	return 0;
}
/**
 *@brief:      dev_key_close
 *@details:    �رհ���
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_key_close(void)
{
	KeyGd = -1;
	return 0;
}

/**
 *@brief:      dev_key_scan
 *@details:    ɨ�谴��
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_key_scan(void)
{
	volatile u8 sta;//�ֲ�����������ջ�ռ䣬���뺯��ʱʹ�ã��˳����ͷš�
	static u8 new_sta = Bit_SET;
	static u8 old_sta = Bit_SET;
	u8 key_value;
	static u8 cnt = 0;

	if(KeyGd != 0)
		return -1;

	/*������״̬*/
	sta = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
	/*
		�жϸ��ϴζ���״̬�ǲ���һ����
		ԭ���ǣ���֤�������̵�״̬������һ���ġ�
		�����׿������󰴼�״̬���ٱ仯��
		����������ǲ�Ҫ��Ϊ���а�����
	*/
	if((sta != new_sta))
	{
		cnt = 0;
		new_sta = sta;
	}
	/*
		���ϴεõ���ֵ��״̬�Ƚϣ�
		�����һ����˵�������б仯
	*/
	if(sta != old_sta)
	{
		cnt++;

		if(cnt >= DEV_KEY_DEBOUNCE)
		{
			/*���������ﵽ��ɨ�赽һ�������仯*/
			cnt = 0;
			key_value = DEV_KEY_PRESS;

			/*�ж����ɿ����ǰ���*/
			if(sta == Bit_RESET)
			{
				KEY_DEBUG(LOG_DEBUG, "key press!\r\n");
			}
			else
			{
				key_value += DEV_KEY_PR_MASK;
				KEY_DEBUG(LOG_DEBUG, "key rel!\r\n");
			}
			/*��ֵд�뻷�λ���*/
			KeyBuf[KeyW] = key_value;
			KeyW++;
			if(KeyW>= KEY_BUF_SIZE)
			{
				KeyW = 0;
			}
			/*����״̬*/
			old_sta = new_sta;
		}
	}
	return 0;
}
/**
 *@brief:      dev_key_read
 *@details:    ������
 *@param[in]   u8 *key  �������ָ��	
               u8 len   ������   
 *@param[out]  ��
 *@retval:     ������������	
 */
s32 dev_key_read(u8 *key, u8 len)
{
	u8 i =0;
	
	if(KeyGd != 0)
		return -1;
	
	while(1)
	{
		if(KeyR != KeyW)
		{
			*(key+i) = KeyBuf[KeyR];
			KeyR++;
			if(KeyR >= KEY_BUF_SIZE)
				KeyR = 0;
		}
		else
		{
			break;
		}
		
		i++;
		if(i>= len)
			break;
	}

	return i;
}

/*


	�����ã��Ժ�Ҫ�ƶ������Գ���SDK

*/
s32 dev_key_waitkey(void)
{
	s32 res;
	u8 key;

	while(1)
	{
		res = dev_key_read(&key, 1);

		if(res == 1)
		{
			if(key == DEV_KEY_PRESS)	
			{

				break;
			}
		}
	}

	return 0;
}



