/**
 * @file                wujique_log.c
 * @brief           ������Ϣ����
 * @author          wujique
 * @date            2018��4��12�� ������
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2018��4��12�� ������
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
#include <stdarg.h>
#include <stdio.h>

#include "stm32f4xx.h"
#include "mcu_uart.h"
#include "wujique_log.h"

LOG_L LogLevel = LOG_DEBUG;//ϵͳ������Ϣ�ȼ�
/*
ʹ�ô������������Ϣ
*/
s8 string[256];//������Ϣ���壬���������Ϣһ�β����Դ���256

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_SendData(USART3, (uint8_t) ch);

    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    return ch;
}

extern int vsprintf(char * s, const char * format, __va_list arg);
/**
 *@brief:      uart_printf
 *@details:    �Ӵ��ڸ�ʽ�����������Ϣ
 *@param[in]   s8 *fmt  
               ...      
 *@param[out]  ��
 *@retval:     
 */
static void uart_printf(s8 *fmt,...)
{
    s32 length = 0;
    va_list ap;

    s8 *pt;
    
    va_start(ap,fmt);
    vsprintf((char *)string,(const char *)fmt,ap);
    pt = &string[0];
    while(*pt!='\0')
    {
        length++;
        pt++;
    }
    
    mcu_uart_write(PC_PORT, (u8*)&string[0], length);  //д����
    
    va_end(ap);
}

void wjq_log(LOG_L l, s8 *fmt,...)
{
	if(l > LogLevel)
		return;

	s32 length = 0;
    va_list ap;

    s8 *pt;
    
    va_start(ap,fmt);
    vsprintf((char *)string,(const char *)fmt,ap);
    pt = &string[0];
    while(*pt!='\0')
    {
        length++;
        pt++;
    }
    
    mcu_uart_write(PC_PORT, (u8*)&string[0], length);  //д����
    
    va_end(ap);
}
/**
 *@brief:      PrintFormat
 *@details:    ��ʽ�����BUF�е�����
 *@param[in]   u8 *wbuf  
               s32 wlen  
 *@param[out]  ��
 *@retval:     
 */
void PrintFormat(u8 *wbuf, s32 wlen)
{   
    s32 i;
    for(i=0; i<wlen; i++)
    {
        if((0 == (i&0x0f)))//&&(0 != i))
        {
            uart_printf("\r\n");
        }
        uart_printf("%02x ", wbuf[i]);
    }
    uart_printf("\r\n");
}

void cmd_uart_printf(s8 *fmt,...)
{
    s32 length = 0;
    va_list ap;

    s8 *pt;
    
    va_start(ap,fmt);
    vsprintf((char *)string,(const char *)fmt,ap);
    pt = &string[0];
    while(*pt!='\0')
    {
        length++;
        pt++;
    }
    
    mcu_uart_write(PC_PORT, (u8*)&string[0], length);  //д����
    
    va_end(ap);
}

