#ifndef __ESP8266_AT_H
#define __ESP8266_AT_H

#include "stm32f1xx_hal.h"
#include "main.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"

#include "bsp_timer.h"
#include "bsp_uart.h"

//此处根据自己的wifi作调整
#define WIFI_NAME "heartcare"
#define WIFI_PASSWD "Heartcare"

#define Str_Len 100


extern uint8_t ESP8266_Init(void);
extern void ESP8266_Restore(void);

extern void ESP8266_ATSendBuf(uint8_t* buf,uint16_t len);		//向ESP8266发送指定长度数据
extern void ESP8266_ATSendString(char* str);								//向ESP8266模块发送字符串
extern void ESP8266_ExitUnvarnishedTrans(void);							//ESP8266退出透传模式
extern uint8_t ESP8266_ConnectAP(char* ssid,char* pswd);		//ESP8266连接热点
extern uint8_t ESP8266_ConnectServer(char* mode,char* ip,uint16_t port);	//使用指定协议(TCP/UDP)连接到服务器
uint8_t ESP8266_Check(void);
void Clear_Rx(uint8_t *String);
void Clear_Tx(uint8_t *String);


uint8_t ESP01S_CONNECT(void);


extern uint8_t Rx_String[];
extern uint8_t Tx_String[];
extern uint8_t Rx_Once[];
extern uint32_t Rx_Counter;

#endif
