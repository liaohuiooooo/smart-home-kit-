#ifndef __ES8266_MQTT_H
#define __ES8266_MQTT_H

#include "stm32f1xx_hal.h"
#include "main.h"
#include "esp8266.h"



#define BYTE0(dwTemp)       (*( char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))
	
//此处是服务器的登陆配置
#define MQTT_BROKERADDRESS "liligaogao.xyz"	    //阿里云服务器-MQTT服务-mosquitto
#define MQTT_CLIENTID "103-Air"	    //客户端号
#define MQTT_USARNAME "stm"				        //user_name
#define MQTT_PASSWD "232323"				//password
#define	MQTT_PUBLISH_TOPIC 			    "AIR"
#define MQTT_SUBSCRIBE_TOPIC			"test"



 

//MQTT连接服务器
extern uint8_t MQTT_Connect(char *ClientID,char *Username,char *Password);
//MQTT消息订阅
extern uint8_t MQTT_SubscribeTopic(char *topic,uint8_t qos,uint8_t whether);
//MQTT消息发布
extern uint8_t MQTT_PublishData(char *topic, char *message, uint8_t qos);
//MQTT发送心跳包
extern void MQTT_SentHeart(void);
void MQTT_CONNECTING(void);
#endif

