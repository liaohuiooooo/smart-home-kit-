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
#define MQTT_BROKERADDRESS "liligaogao.xyz"	    //�����Ʒ�����-MQTT����-mosquitto
#define MQTT_CLIENTID "103-Air"	    //�ͻ��˺�
#define MQTT_USARNAME "stm"				        //user_name
#define MQTT_PASSWD "232323"				//password
#define	MQTT_PUBLISH_TOPIC 			    "AIR"
#define MQTT_SUBSCRIBE_TOPIC			"test"



 

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

