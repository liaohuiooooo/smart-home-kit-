/**
 * @file            dev_touchkey.c
 * @brief           ����������������
 * @author          wujique
 * @date            2017��11��7�� ���ڶ�
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2017��11��7�� ���ڶ�
 *   ��    ��:      �ݼ�ȸ������
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
/*
	���ݴ�����������
	1 IO������ߵ�ƽ���Դ���ͭ����硣
	2 ���ö�ʱ�����벶��
	3 ��ѯ���õ�����ֵ��
	4 ������ֵ������ʷֵһ�������������õ�����״̬�����д��������봥��������������
		
*/

#include "stm32f4xx.h"
#include "wujique_log.h"
#include "mcu_timer.h"
#include "dev_touchkey.h"

//#define DEV_TOUCHKEY_DEBUG

#ifdef DEV_TOUCHKEY_DEBUG
#define TOUCHKEY_DEBUG	wjq_log 
#else
#define TOUCHKEY_DEBUG(a, ...)
#endif


#define DEV_TOUCHKEY_GATE (50)//ȷ��״̬�仯������ֵ������Ӳ�����ܵ��ڱ����������������ȼ��ɡ�
#define DEV_TOUCHKEY_DATA_NUM (50)//һ���ȶ�״̬��Ҫ��ʱ��������������ͨ���޸�������ڴ���ɨ��ʱ��
static u16 TouchKeyLastCap = 0;//���һ���ȶ���CAPƽ��ֵ

#define DEV_TOUCHKEY_BUF_SIZE (16)
static u8 TouchKeyBuf[DEV_TOUCHKEY_BUF_SIZE];//��������������������õ�ʱ���ͽ�ʱ��ŵ����������
static u8 TouchKeyWrite = 0;
static u8 TouchKeyRead = 0;

s32 TouchKeyGd = -2;

/**
 *@brief:      dev_touchkey_resetpad
 *@details:    ��λ��������������ߵ�ƽ��磩
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
static s32 dev_touchkey_resetpad(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_SetBits(GPIOA,GPIO_Pin_3);	

	return 0;
}
/**
 *@brief:      dev_touchkey_iocap
 *@details:       IO����Ϊ��ʱ��������
 *@param[in]  void  
 *@param[out]  ��
 *@retval:     static
 */
static s32 dev_touchkey_iocap(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //ʹ�� PORTAʱ��
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_TIM2); //PA3 ����λ��ʱ�� 2
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //GPIOA5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//�ٶ� 100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//����������
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ�� PA3

	return 0;

}

s32 dev_touchkey_init(void)
{
	TouchKeyGd = -1;
	return 0;
}

s32 dev_touchkey_open(void)
{
	if(TouchKeyGd != -1)
		return -1;
	TouchKeyGd = 0;
	return 0;
}

s32 dev_touchkey_close(void)
{
	TouchKeyGd = -1;
	return 0;
}
/**
 *@brief:      dev_touchkey_read
 *@details:    ���豸����ȡ�����¼�
 *@param[in]   u8 *buf    
               u32 count  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_touchkey_read(u8 *buf, u32 count)
{
	u32 cnt = 0;

	if(TouchKeyGd != 0)
		return -1;
	
	while(1)
	{
		if(TouchKeyWrite ==  TouchKeyRead)
			break;

		if(cnt >= count)
			break;
		
		*(buf+cnt) = TouchKeyBuf[TouchKeyRead++];
		if(TouchKeyRead >= DEV_TOUCHKEY_BUF_SIZE)
			TouchKeyRead = 0;

		cnt++;
	}
	
	return cnt;

}

s32 dev_touchkey_write(void)
{
	return 0;
}

/**
 *@brief:      dev_touchkey_scan
 *@details:    ɨ�败�������������
 *@param[in]   u32  
 *@param[out]  ��
 *@retval:                  	
 */
static s32 dev_touchkey_scan(u32 cap)
{
	static u16 average = 0;//ƽ��ֵ
	static u8 cap_cnt = 0;//��Ч�������
	static u8 last_dire = DEV_TOUCHKEY_IDLE;//��һ��ֵ�ķ���1Ϊ���,������2Ϊ��С���ɿ�
	static u8 last_chg = NULL;
	
	TOUCHKEY_DEBUG(LOG_DEBUG, "--%08x-%04x-", cap, TouchKeyLastCap);
	if(cap > TouchKeyLastCap + DEV_TOUCHKEY_GATE)
	{
		/*����һ�α仯��ƽ��ֵ�Ƚϣ� �󣬽���*/
		if(last_dire != DEV_TOUCHKEY_TOUCH)
		{
			cap_cnt = 0;
			average = 0;
			last_dire = DEV_TOUCHKEY_TOUCH;
		}

		cap_cnt++;
		average = average + cap;
	}
	else if(cap < TouchKeyLastCap - DEV_TOUCHKEY_GATE)
	{
		if(last_dire != DEV_TOUCHKEY_RELEASE)
		{
			cap_cnt = 0;
			average = 0;
			last_dire = DEV_TOUCHKEY_RELEASE;
		}	
		cap_cnt++;
		average = average + cap;

	}
	else
	{

		cap_cnt = 0;
		average = 0;
		last_dire = DEV_TOUCHKEY_IDLE;
	}

	if(cap_cnt >= DEV_TOUCHKEY_DATA_NUM)
	{

		if(TouchKeyLastCap == 0)
		{
			TOUCHKEY_DEBUG(LOG_DEBUG, "\r\n-------------------init\r\n");	
		}
		else
		{
			TOUCHKEY_DEBUG(LOG_DEBUG, "\r\n-------------------chg\r\n");
			if(last_chg != last_dire)//��ֹ�ظ��ϱ�
			{
				TOUCHKEY_DEBUG(LOG_DEBUG, "\r\n--------report\r\n");
				TouchKeyBuf[TouchKeyWrite++] = last_dire;
				if(TouchKeyWrite >= DEV_TOUCHKEY_BUF_SIZE)
				TouchKeyWrite = 0;
			}
			
			last_chg = last_dire;
			
		}
		/*�����µ�ƽ��ֵ*/
		TouchKeyLastCap = average/DEV_TOUCHKEY_DATA_NUM;
		cap_cnt = 0;
		average = 0;
	}
	return 0;
}
/**
 *@brief:      dev_touchkey_task
 *@details:    ���������̣߳���פ����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_touchkey_task(void)
{
	volatile u32 i = 0;
	u32 cap;

	if(TouchKeyGd != 0)
		return -1;
	//IO���1���Ե��ݳ��
	dev_touchkey_resetpad();
	//��ʱһ�㣬���
	for(i=0;i++;i<0x12345);
	//��IO������Ϊ��ʱȥ���벶��ͨ��
	dev_touchkey_iocap();
	//����ʱ���������Ԥ��Ƶ8��һ����ʱ��������100ns���� �����ֵҪͨ�����ԣ�
	mcu_timer_cap_init(0xffffffff, 8);
	cap = mcu_timer_get_cap();
	TOUCHKEY_DEBUG(LOG_DEBUG, "\r\n%08x---", cap);

	dev_touchkey_scan(cap);
	
	return 0;
}

/**
 *@brief:      dev_touchkey_test
 *@details:    �����������Գ���
 *@param[in]   ��
 *@param[out]  ��
 *@retval:     
 */
s32 dev_touchkey_test(void)
{
	u8 tmp;
	s32 res;

	//dev_touchkey_open();
	
	res = dev_touchkey_read(&tmp, 1);
	if(1 == res)
	{
		if(tmp == DEV_TOUCHKEY_TOUCH)
		{
			wjq_log(LOG_FUN, "touch key test get a touch event!\r\n");
		}
		else if(tmp == DEV_TOUCHKEY_RELEASE)
		{
			wjq_log(LOG_FUN, "touch key test get a release event!\r\n");
		}
	}
	return 0;
	
}


