#ifndef __ESP8266_AT_H
#define __ESP8266_AT_H

#include "stm32f1xx_hal.h"
#include "main.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"

#include "bsp_timer.h"
#include "bsp_uart.h"

//�˴������Լ���wifi������
#define WIFI_NAME "heartcare"
#define WIFI_PASSWD "Heartcare"

#define Str_Len 100


extern uint8_t ESP8266_Init(void);
extern void ESP8266_Restore(void);

extern void ESP8266_ATSendBuf(uint8_t* buf,uint16_t len);		//��ESP8266����ָ����������
extern void ESP8266_ATSendString(char* str);								//��ESP8266ģ�鷢���ַ���
extern void ESP8266_ExitUnvarnishedTrans(void);							//ESP8266�˳�͸��ģʽ
extern uint8_t ESP8266_ConnectAP(char* ssid,char* pswd);		//ESP8266�����ȵ�
extern uint8_t ESP8266_ConnectServer(char* mode,char* ip,uint16_t port);	//ʹ��ָ��Э��(TCP/UDP)���ӵ�������
uint8_t ESP8266_Check(void);
void Clear_Rx(uint8_t *String);
void Clear_Tx(uint8_t *String);


uint8_t ESP01S_CONNECT(void);


extern uint8_t Rx_String[];
extern uint8_t Tx_String[];
extern uint8_t Rx_Once[];
extern uint32_t Rx_Counter;

#endif
