#ifndef __ES8266_MQTT_H
#define __ES8266_MQTT_H

#include "stm32f1xx_hal.h"
#include "main.h"
#include "esp8266.h"



#define BYTE0(dwTemp)       (*( char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))
	
//�˴��Ƿ������ĵ�½����
#define MQTT_BROKERADDRESS "liligaogao.xyz"	//�����Ʒ�����-MQTT����-mosquitto
#define MQTT_CLIENTID "STM32-ESP8266-ARM"	//�ͻ��˺�
#define MQTT_USARNAME "stm"				//user_name
#define MQTT_PASSWD "232323"				//password
#define	MQTT_PUBLISH_TOPIC 			"test"
#define MQTT_SUBSCRIBE_TOPIC			"test"



#define LOOPTIME 100 	//��������ѭ����ʱʱ�䣺100ms
#define COUNTER_LEDBLINK			(300/LOOPTIME)		//LED������˸ʱ�䣺300ms
#define COUNTER_RUNINFOSEND		(5000/LOOPTIME)		//���д�����ʾ��5s
#define COUNTER_MQTTHEART     (5000/LOOPTIME)		//MQTT������������5s
#define COUNTER_STATUSREPORT	(3000/LOOPTIME)		//״̬�ϴ���3s
#define COUNTER_SUB_TIME		(1000/LOOPTIME)		//	ÿ��һ�����һ������

//MQTT���ӷ�����
extern uint8_t MQTT_Connect(char *ClientID,char *Username,char *Password);
//MQTT��Ϣ����
extern uint8_t MQTT_SubscribeTopic(char *topic,uint8_t qos,uint8_t whether);
//MQTT��Ϣ����
extern uint8_t MQTT_PublishData(char *topic, char *message, uint8_t qos);
//MQTT����������
extern void MQTT_SentHeart(void);
void MQTT_CONNECTING(void);
#endif

