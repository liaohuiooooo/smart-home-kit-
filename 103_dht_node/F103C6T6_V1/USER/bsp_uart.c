#include "bsp_uart.h"


uint32_t Rx_Counter;

uint8_t Rx_String[Str_Len];
uint8_t Tx_String[Str_Len];
uint8_t Rx_Once[Str_Len];



// 重定向printf函数
int fputc(int ch,FILE *f)
{
    uint8_t temp[1]={ch};
    HAL_USART_Transmit(&husart1,temp,1,2);
	return 0;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart2 )
	{
		Rx_String[Rx_Counter++] = Rx_Once[0];
		HAL_UART_Receive_IT(&huart2, Rx_Once, 1);
		
	}
		
}

