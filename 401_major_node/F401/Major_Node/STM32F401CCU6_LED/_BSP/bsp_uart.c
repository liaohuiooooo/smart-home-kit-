#include "bsp_uart.h"
//UART_HandleTypeDef huart6;


/**
 * @brief printf函数重定向
 * @param int ch, FILE *F
 * @arg 要发送的数据ch
 * @retval 0
 */
int fputc(int ch, FILE *F){
    uint8_t temp[1]={ch};
    HAL_UART_Transmit(&huart1, temp, 1, 2);
    return 0;
}

/**
 * @brief 重定向c库函数scanf到串口USARTx，重写向后可使用scanf、getchar等函数
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
 * @brief  DEBUG_USART6 GPIO 配置,工作模式配置。115200 8-N-1
 * @param  无
 * @retval 无
 * @note   初始化Uart与MCU硬件无关的东西。最终实例化到具体芯片，使用韩式HAL_UART_MspInit()函数,
 *         使用UART6串口之前，要初始化。
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
//     /*初始化结构体 */
//    HAL_UART_Init(&huart6);

//     /*使能串口接收断 */
//    __HAL_UART_ENABLE_IT(&huart6, UART_IT_RXNE);
//}



/**
 * @brief UART6 MSP 初始化
 * @param huart: UART handle
 * @retval 无
 * @note HAL_UART_MspInit函数被HAL_UART_Init()
 * 
 */
// void HAL_UART_MspInit(UART_HandleTypeDef *huart)
// {
//     GPIO_InitTypeDef GPIO_InitStruct;
//     DEBUG_USART6_CLK_ENABLE();
//     DEBUG_USART_RX_GPIO_CLK_ENABLE();
//     DEBUG_USART_TX_GPIO_CLK_ENABLE();

//     /* 配置Tx引脚为复用功能 */
//     GPIO_InitStruct.Pin = DEBUG_USART_TX_PIN;
//     GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;//复用推挽输出
//     GPIO_InitStruct.Pull = GPIO_PULLUP;
//     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//     GPIO_InitStruct.Alternate = DEBUG_USART_TX_AF;
//     HAL_GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStruct);

//     /*配置Rx引脚为复用功能*/
//     GPIO_InitStruct.Pin = DEBUG_USART_RX_PIN;
//     GPIO_InitStruct.Alternate = DEBUG_USART_RX_AF;
//     HAL_GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStruct);

//     HAL_NVIC_SetPriority(DEBUG_USART6_IRQ, 0,1);
//     HAL_NVIC_EnableIRQ(DEBUG_USART6_IRQ);
// }


/**
 * @brief 字符发送
 * @param str 
 * @note HAL_UART_Transmit阻塞发送，直到遇到空字符才停止发送
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




