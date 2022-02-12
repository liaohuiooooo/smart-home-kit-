/**
 * @file            dev_rs485.c
 * @brief           485ͨ������
 * @author          wujique
 * @date            2017��12��8�� ������
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2017��12��8�� ������
 *   ��    ��:         wujique
 *   �޸�����:   		�����ļ�
 					485�������ڴ�������
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
#include <stdarg.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "mcu_uart.h"
#include "wujique_log.h"
#include "wujique_sysconf.h"


/*rs485ʹ�ô���1*/
#define DEV_RS485_UART MCU_UART_1
s32 RS485Gd = -2;
/**
 *@brief:      dev_rs485_init
 *@details:    ��ʼ��485�豸
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_rs485_init(void)
{
	#ifdef SYS_USE_RS485
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE); //ʹ�� PGʱ��
	//PG8 ��������� 485 ģʽ����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //GPIOG8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //�ٶ� 100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOG, &GPIO_InitStructure); //��ʼ�� PG8	

	//��ʼ������Ϊ����ģʽ
	GPIO_ResetBits(GPIOG, GPIO_Pin_8);

	RS485Gd = -1;
	#else
	wjq_log(LOG_INFO, ">---------------RS485 IS NO ININT!\r\n");
	#endif
	return 0;
}
/**
 *@brief:      dev_rs485_open
 *@details:    ��RS485�豸
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_rs485_open(void)
{
	if(RS485Gd!= -1)
		return -1;
	
	mcu_uart_open(DEV_RS485_UART);
	mcu_uart_set_baud(DEV_RS485_UART, 9600);	
	RS485Gd = 0;
	return 0;
}
/**
 *@brief:      dev_rs485_close
 *@details:    �ر�RS485�豸
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_rs485_close(void)
{
	if(RS485Gd!= 0)
		return -1;
	
	mcu_uart_close(DEV_RS485_UART);
	return 0;
}

/**
 *@brief:      dev_rs485_read
 *@details:    RS485��������
 *@param[in]   u8 *buf  
               s32 len  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_rs485_read(u8 *buf, s32 len)
{
	s32 res;
	
	if(RS485Gd!= 0)
		return -1;
	res = mcu_uart_read(DEV_RS485_UART, buf, len);	
	
	return res;
}
/**
 *@brief:      dev_rs485_write
 *@details:    rs485��������
 *@param[in]   u8 *buf  
               s32 len  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_rs485_write(u8 *buf, s32 len)
{
	s32 res;
	
	if(RS485Gd!= 0)
		return -1;
	GPIO_SetBits(GPIOG, GPIO_Pin_8);//����Ϊ����ģʽ
	res = mcu_uart_write(DEV_RS485_UART, buf, len);
	GPIO_ResetBits(GPIOG, GPIO_Pin_8);//���ͽ���������Ϊ����ģʽ

	return res;
}
/**
 *@brief:      dev_rs485_ioctl
 *@details:    IOCTL
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_rs485_ioctl(void)
{
	if(RS485Gd!= 0)
		return -1;

		return 0;
}

extern void Delay(__IO uint32_t nTime);

/**
 *@brief:      dev_rs485_test
 *@details:    RS485���Գ���
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_rs485_test(u8 mode)
{
	u8 buf[20];
	u8 len;
	s32 res;
	
	dev_rs485_open();

	if(mode == 1)// ���Ͷ˲���
	{
		while(1)
		{
			Delay(1000);
			res = dev_rs485_write("rs485 test\r\n", 13);
			wjq_log(LOG_FUN, "dev rs485 write:%d\r\n", res);
		}
	}
	else//���ն˲���
	{
		while(1)
		{
			Delay(20);
			len = dev_rs485_read(buf, sizeof(buf));
			if(len > 0)
			{
				buf[len] = 0;
				wjq_log(LOG_FUN, "%s", buf);
				memset(buf, 0, sizeof(buf));
			}
		}
	}

}

/*------ end file --------*/


