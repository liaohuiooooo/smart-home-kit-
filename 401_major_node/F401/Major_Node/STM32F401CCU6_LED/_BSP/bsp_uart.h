#ifndef __BSP_UART_H
#define __BSP_UART_H

#include "stm32f4xx.h"
#include "usart.h"
#include "bsp_led.h"
#include "stdio.h"
#include "string.h"


  //UART6 引脚定义 波特率
  /*******************************************************/
  #define DEBUG_USART6_BAUDRATE 115200

  #define DEBUG_USART6                             USART6
  #define DEBUG_USART6_CLK_ENABLE()                __USART6_CLK_ENABLE()

  #define RCC_PERIPHCLK_UART6                     RCC_PERIPHCLK_USART6
  #define RCC_UART6CLKSOURCE_SYSCLK               RCC_USART6CLKSOURCE_SYSCLK

  #define DEBUG_USART_RX_GPIO_PORT                GPIOC
  #define DEBUG_USART_RX_GPIO_CLK_ENABLE()        __GPIOC_CLK_ENABLE()
  #define DEBUG_USART_RX_PIN                      GPIO_PIN_7
  #define DEBUG_USART_RX_AF                       GPIO_AF8_USART6

  #define DEBUG_USART_TX_GPIO_PORT                GPIOC
  #define DEBUG_USART_TX_GPIO_CLK_ENABLE()        __GPIOC_CLK_ENABLE()
  #define DEBUG_USART_TX_PIN                      GPIO_PIN_6
  #define DEBUG_USART_TX_AF                       GPIO_AF8_USART6

  #define DEBUG_USART6_IRQHandler                  USART6_IRQHandler
  #define DEBUG_USART6_IRQ                         USART6_IRQn
  /************************************************************/







//USER_DEBUG_USART1
#define USERS_DEBUG

#ifdef USERS_DEBUG
    #define user_debug(format, ...) 		printf("[DEBUG]:\t" format "\r\n",##__VA_ARGS__)
#else
    #define user_main_debug(format, ...)
#endif




void DEBUG6_Config(void);
void usart6_sendstring(uint8_t *str);


#endif /*__BSP_UART_H*/
