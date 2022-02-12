#include "bsp_uart.h"
//UART_HandleTypeDef huart6;


/**
 * @brief printf�����ض���
 * @param int ch, FILE *F
 * @arg Ҫ���͵�����ch
 * @retval 0
 */
int fputc(int ch, FILE *F){
    uint8_t temp[1]={ch};
    HAL_UART_Transmit(&huart1, temp, 1, 2);
    return 0;
}

/**
 * @brief �ض���c�⺯��scanf������USARTx����д����ʹ��scanf��getchar�Ⱥ���
 * 
 * @param F 
 * @return int 
 */
int fgetc(FILE *F){
    int ch;
    while(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) == RESET);
    HAL_UART_Receive(&huart1, (uint8_t*)&ch, 1, 0xffff);
    return (ch);
}

 /**
 * @brief  DEBUG_USART6 GPIO ����,����ģʽ���á�115200 8-N-1
 * @param  ��
 * @retval ��
 * @note   ��ʼ��Uart��MCUӲ���޹صĶ���������ʵ����������оƬ��ʹ�ú�ʽHAL_UART_MspInit()����,
 *         ʹ��UART6����֮ǰ��Ҫ��ʼ����
 */
//void DEBUG6_Config(void)
//{
//    huart6.Instance = DEBUG_USART6;
//    huart6.Init.BaudRate = DEBUG_USART6_BAUDRATE;
//    huart6.Init.WordLength = UART_WORDLENGTH_8B;
//    huart6.Init.StopBits = UART_STOPBITS_1;
//    huart6.Init.Parity = UART_PARITY_NONE;
//    huart6.Init.Mode = UART_MODE_TX_RX;
//    huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//    huart6.Init.OverSampling = UART_OVERSAMPLING_16;
//   
//     /*��ʼ���ṹ�� */
//    HAL_UART_Init(&huart6);

//     /*ʹ�ܴ��ڽ��ն� */
//    __HAL_UART_ENABLE_IT(&huart6, UART_IT_RXNE);
//}



/**
 * @brief UART6 MSP ��ʼ��
 * @param huart: UART handle
 * @retval ��
 * @note HAL_UART_MspInit������HAL_UART_Init()
 * 
 */
// void HAL_UART_MspInit(UART_HandleTypeDef *huart)
// {
//     GPIO_InitTypeDef GPIO_InitStruct;
//     DEBUG_USART6_CLK_ENABLE();
//     DEBUG_USART_RX_GPIO_CLK_ENABLE();
//     DEBUG_USART_TX_GPIO_CLK_ENABLE();

//     /* ����Tx����Ϊ���ù��� */
//     GPIO_InitStruct.Pin = DEBUG_USART_TX_PIN;
//     GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;//�����������
//     GPIO_InitStruct.Pull = GPIO_PULLUP;
//     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//     GPIO_InitStruct.Alternate = DEBUG_USART_TX_AF;
//     HAL_GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStruct);

//     /*����Rx����Ϊ���ù���*/
//     GPIO_InitStruct.Pin = DEBUG_USART_RX_PIN;
//     GPIO_InitStruct.Alternate = DEBUG_USART_RX_AF;
//     HAL_GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStruct);

//     HAL_NVIC_SetPriority(DEBUG_USART6_IRQ, 0,1);
//     HAL_NVIC_EnableIRQ(DEBUG_USART6_IRQ);
// }


/**
 * @brief �ַ�����
 * @param str 
 * @note HAL_UART_Transmit�������ͣ�ֱ���������ַ���ֹͣ����
 */
//void usart6_sendstring(uint8_t *str)
//{
//    uint8_t k = 0;
//    do{
//        HAL_UART_Transmit(&huart6,(uint8_t*)(str + k), 1, 2);
//        k++;
//    }while(*(str+k)!= '\0');
//}


//void DEBUG_USART6_IRQHandler(void)
//{
//    uint8_t ch;
//    if(__HAL_UART_GET_FLAG(&huart6, UART_FLAG_RXNE) != RESET)
//    {
//        ch = (uint16_t)READ_REG(huart6.Instance->DR);
//        WRITE_REG(huart6.Instance->DR, ch);
//    }
//}




