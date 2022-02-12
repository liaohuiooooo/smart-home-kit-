#include <stdarg.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "wujique_log.h"
#include "main.h"
#include "wujique_sysconf.h"

/*

	8266 �ײ���������������ڵ�Ӳ��ͨ��

	ģ��ʹ�ð��ſ�8266ģ��ESP-01/01S
	VCC/GND
	TXD/TXD
	EN  ʹ�ܣ��ߵ�ƽ��Ч -----------PF5
	RST ��λ���͵�ƽ��Ч      ----------PF4
	IO0 ����ʱ���ͣ���������ģʽ---------PA11
	IO2 ������				------------PA12
*/	
#define DEV_8266_UART MCU_UART_1

s32 ESP8266Gd = -2;

s32 dev_8266_open(void)
{
	
	return 0;
}

s32 dev_8266_close(void)
{
	return 0;
}
/**
 *@brief:      dev_8266_init
 *@details:       ��ʼ��8266�õ���IO
 *@param[in]  void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_8266_init(void)
{
	#ifdef SYS_USE_EXUART
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5; //GPIOG8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //�ٶ� 100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOF, &GPIO_InitStructure); //��ʼ�� PG8	

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12; //GPIOG8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //�ٶ� 100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ�� PG8
	
	GPIO_SetBits(GPIOA, GPIO_Pin_11|GPIO_Pin_12);
	GPIO_SetBits(GPIOF, GPIO_Pin_4);
	GPIO_ResetBits(GPIOF, GPIO_Pin_5);
	Delay(100);
	//��ʼ������Ϊ����ģʽ
	GPIO_ResetBits(GPIOF, GPIO_Pin_4);
	Delay(100);
	GPIO_SetBits(GPIOF, GPIO_Pin_4);
	GPIO_SetBits(GPIOF, GPIO_Pin_5);
	
	mcu_uart_open(DEV_8266_UART);
	mcu_uart_set_baud(DEV_8266_UART, 115200);
	#else
	wjq_log(LOG_INFO, ">------esp8266 no init!\r\n");
	#endif
	
	return 0;
}


s32 dev_8266_read(u8 *buf, s32 len)
{
	s32 res;
	
	res = mcu_uart_read(DEV_8266_UART, buf, len);	
	
	return res;
}

s32 dev_8266_write(u8 *buf, s32 len)
{
	s32 res;
	
	res = mcu_uart_write(DEV_8266_UART, buf, len);

	return res;
}

/**
 *@brief:      dev_8266_test
 *@details:    ���Ը�8266��ͨ��
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_8266_test(void)
{
	u8 buf[32];
	u16 len;
	u32 timeout = 0;

	dev_8266_open();

	while(1)
	{
		Delay(500);
		dev_8266_write("at\r\n", 4);
		timeout = 0;
		while(1)
		{
			Delay(50);
			memset(buf, 0, sizeof(buf));
			len = dev_8266_read(buf, sizeof(buf));
			if(len != 0)
			{
				wjq_log(LOG_FUN, "%s", buf);
			}
			
			timeout++;
			if(timeout >= 100)
			{
				wjq_log(LOG_FUN, "timeout---\r\n");
				break;
			}
		}
	}
}


