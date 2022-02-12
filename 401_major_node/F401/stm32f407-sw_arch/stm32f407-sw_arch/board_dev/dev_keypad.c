/**
 * @file            dev_keypad.c
 * @brief           ���󰴼�ɨ��
 * @author          wujique
 * @date            2018��3��10�� ������
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2018��3��10�� ������
 *   ��    ��:         �ݼ�ȸ������
 *   �޸�����:   �����ļ�

		��Ȩ˵����
		1 Դ����ݼ�ȸ���������С�
		2 �������ڵ�������ҵ��;�����׿��������۳��⣩��������Ȩ��
		3 �ݼ�ȸ�����Ҳ��Դ��빦�����κα�֤����ʹ�������в��ԣ�����Ը���
		4 �������޸�Դ�벢�ַ���������ֱ�����۱�������������ұ�����Ȩ˵����
		5 �緢��BUG�����Ż�����ӭ�������¡�����ϵ��code@wujique.com
		6 ʹ�ñ�Դ�����൱����ͬ����Ȩ˵����
		7 ���ַ����Ȩ��������ϵ��code@wujique.com
*/
#include "stm32f4xx.h"
#include "wujique_log.h"
#include "wujique_sysconf.h"
#include "dev_keypad.h"

//#define DEV_KEYPAD_DEBUG

#ifdef DEV_KEYPAD_DEBUG
#define KEYPAD_DEBUG	wjq_log 
#else
#define KEYPAD_DEBUG(a, ...)
#endif

/*

	ԭ��
	ɨ�����ֻ�ṩ��λ�����ṩ����
	����������APP�����飬�����̰���������Ҳ��APP�����顣
	4*4���󰴼�����λ���壬���Ͻ�Ϊ1�������ң����ϵ��¡�

	ɨ�跽����
	���������0������IO��״̬��

	��ֵ�ο����̣�ɨ�����䵽����������ǰ���λ��ͨ����Ϣ��

*/
#define KEY_PAD_COL_NUM (4)//4��
#define KEY_PAD_ROW_NUM (4)//4��
/*�ް�������ʱ����IO״̬��ͬʱҲ��ʶ��״̬��Щλ��Ч*/
#define KEYPAD_INIT_STA_MASK (0X0F)
/*
����ʱ��=��������*scan����ִ�м��*KEY_PAD_ROW_NUM
��ͬ�����������ʱ�䣺
�𶯰�����10ms
���У�40ms-50ms
��е����������100ms
*/
#define KEY_PAD_DEC_TIME (3)//����ɨ�����
/*
	ɨ�����ʹ�õı���
*/
struct _strKeyPadCtrl
{
	u8 dec;//��������
	u8 oldsta;//��һ�ε��ȶ�״̬
	u8 newsta;
};
struct _strKeyPadCtrl KeyPadCtrl[KEY_PAD_ROW_NUM];


/*
	�������壬ɨ�赽�İ���λ�ü�ֵ��ֻ˵��λ�õ�ͨ�ϣ�
	���λΪ0��ʶͨ�����£���Ϊ1�����ʶ�Ͽ���
*/
#define KEYPAD_BUFF_SIZE (32)
static u8 KeyPadBuffW = 0;//д����
static u8 KeyPadBuffR = 0;//������
static u8 KeyPadBuff[KEYPAD_BUFF_SIZE];

static s32 DevKeypadGd = -2;//�豸����

/**
 *@brief:      dev_keypad_init
 *@details:    ��ʼ�����󰴼�IO��
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_keypad_init(void)
{
	/*
	c:PF8-PF11   ����������
	r:PF12-PF15  �������
	*/
	#ifdef SYS_USE_KEYPAD
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
 
	/* r */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	/* c */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	GPIO_SetBits(GPIOF, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);

	u8 i;
	for(i = 0; i< KEY_PAD_ROW_NUM; i++)
	{
		KeyPadCtrl[i].dec = 0;
		KeyPadCtrl[i].oldsta = KEYPAD_INIT_STA_MASK;
		KeyPadCtrl[i].newsta = KEYPAD_INIT_STA_MASK;
	}
	
	DevKeypadGd = -1;
	#else
	wjq_log(LOG_INFO, ">>>>>>keypad not init!\r\n");
	#endif
	
	return 0;
}

/**
 *@brief:      dev_keypad_open
 *@details:    �򿪰�����ֻ�д򿪰�����ɨ��Ż����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_keypad_open(void)
{
	if(DevKeypadGd == -1)
	{
		DevKeypadGd	 = 0;
	}
	else
		return -1;
	
	return 0;
}
/**
 *@brief:      dev_keypad_read
 *@details:    ����ֵ
 *@param[in]   u8 *key  
               u8 len   ��������ֵ����   
 *@param[out]  ��
 *@retval:     ���ض�����ֵ��
 */
s32 dev_keypad_read(u8 *key, u8 len)
{
	u8 i =0;
	
	if(DevKeypadGd != 0)
		return -1;

	while(1)
	{
		if(KeyPadBuffR != KeyPadBuffW)
		{
			*(key+i) = KeyPadBuff[KeyPadBuffR];
			KeyPadBuffR++;
			if(KeyPadBuffR >= KEYPAD_BUFF_SIZE)
				KeyPadBuffR = 0;
		}
		else
			break;

		i++;
		if(i>= len)
			break;
	}

	return i;
}

/**
 *@brief:      dev_keypad_scan
 *@details:    ����ɨ�裬�ڶ�ʱ�����������ж�ʱִ��
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_keypad_scan(void)
{
	u16 ColSta;
	u8 chgbit;
	static u8 scanrow = 0;
	u8 keyvalue;
	
	if(DevKeypadGd != 0)
		return -1;
	
	/*�������״̬�������������IO����ƴ������IO*/
	ColSta = GPIO_ReadInputData(GPIOF);
	ColSta = (ColSta>>8)&KEYPAD_INIT_STA_MASK;

	/*��¼��״̬����״̬�����������ȶ����������¼���*/
	if(ColSta != KeyPadCtrl[scanrow].newsta)
	{
		KeyPadCtrl[scanrow].newsta = ColSta;
		KeyPadCtrl[scanrow].dec = 0;
	}

	/*����״̬���״̬�б仯������ɨ���ж�*/
	if(ColSta != KeyPadCtrl[scanrow].oldsta)
	{
		KEYPAD_DEBUG(LOG_DEBUG, " chg--");
		KeyPadCtrl[scanrow].dec++;
		if(KeyPadCtrl[scanrow].dec >= KEY_PAD_DEC_TIME)//���ڷ�������
		{
			/*ȷ���б仯*/
			KeyPadCtrl[scanrow].dec = 0;
			/*�¾ɶԱȣ��ҳ��仯λ*/
			chgbit = KeyPadCtrl[scanrow].oldsta^KeyPadCtrl[scanrow].newsta;
			KEYPAD_DEBUG(LOG_DEBUG, "row:%d, chage bit:%02x\r\n",scanrow,chgbit);

			/*���ݱ仯��λ������仯�İ���λ��*/
			u8 i;
			for(i=0;i<KEY_PAD_COL_NUM;i++)
			{
				if((chgbit & (0x01<<i))!=0)
				{
					keyvalue = 	scanrow*KEY_PAD_COL_NUM+i;
					/*���ͨ�ϣ������ɿ�����־*/
					if((KeyPadCtrl[scanrow].newsta&(0x01<<i)) == 0)
					{
						KEYPAD_DEBUG(LOG_DEBUG, "press\r\n");
					}
					else
					{
						KEYPAD_DEBUG(LOG_DEBUG, "rel\r\n");
						keyvalue += KEYPAD_PR_MASK;
					}
					/**/
					KeyPadBuff[KeyPadBuffW] =keyvalue+1;//+1��������1��ʼ������0��ʼ
					KeyPadBuffW++;
					if(KeyPadBuffW>=KEYPAD_BUFF_SIZE)
						KeyPadBuffW = 0;
				}
			}
			
			KeyPadCtrl[scanrow].oldsta = KeyPadCtrl[scanrow].newsta;
			
		}
	}

	/*����һ�е�IO���0*/
	scanrow++;
	if(scanrow >= KEY_PAD_ROW_NUM)
		scanrow = 0;
	
	GPIO_SetBits(GPIOF, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
	
	switch(scanrow)
	{
		case 0:
			GPIO_ResetBits(GPIOF, GPIO_Pin_12);
			break;
		case 1:
			GPIO_ResetBits(GPIOF, GPIO_Pin_13);
			break;
		case 2:
			GPIO_ResetBits(GPIOF, GPIO_Pin_14);
			break;
		case 3:
			GPIO_ResetBits(GPIOF, GPIO_Pin_15);
			break;
	}
	return 0;
}
/**
 *@brief:      dev_keypad_test
 *@details:    ����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_keypad_test(void)
{
	u8 key;
	s32 res;
	res = dev_keypad_read(&key, 1);
	if(res == 1)
	{
		wjq_log(LOG_FUN, "get a key:%02x\r\n", key);
	}
	return 0;
}



