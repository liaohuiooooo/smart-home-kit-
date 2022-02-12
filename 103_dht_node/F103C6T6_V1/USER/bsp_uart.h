#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "usart.h"
#include "string.h"
#include "stdio.h"

#define Str_Len 100

#define USER_MAIN_DEBUG

#ifdef USER_MAIN_DEBUG
	#define user_main_printf(format, ...) printf( format "\r\n",##__VA_ARGS__)
	#define user_main_info(format, ...) printf("¡¾main¡¿info:" format "\r\n",##__VA_ARGS__)
	#define user_main_debug(format, ...) printf("¡¾main¡¿debug:" format "\r\n",##__VA_ARGS__)
	#define user_main_error(format, ...) printf("¡¾main¡¿error:" format "\r\n",##__VA_ARGS__)
#else
	#define user_main_printf(format, ...)
	#define user_main_info(format, ...)
	#define user_main_debug(format, ...)
	#define user_main_error(format, ...)
#endif

#endif /*__BSP_UART_H__*/

