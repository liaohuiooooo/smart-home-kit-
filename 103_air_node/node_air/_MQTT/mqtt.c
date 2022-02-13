#include "mqtt.h"
#include "ESP8266.h"



extern uint8_t Rx_String[];
extern uint8_t Tx_String[];
extern uint8_t Rx_Once[];
extern uint32_t Rx_Counter;

//连接成功服务器回应 20 02 00 00
//客户端主动断开连接 e0 00

const uint8_t parket_connetAck[] = {0x20,0x02,0x00,0x00};
const uint8_t parket_disconnet[] = {0xe0,0x00};
const uint8_t parket_heart[] = {0xc0,0x00};
const uint8_t parket_heart_reply[] = {0xc0,0x00};
const uint8_t parket_subAck[] = {0x90,0x03};

volatile uint16_t MQTT_TxLen;

//MQTT发送数据
void MQTT_SendBuf(uint8_t *buf,uint16_t len)
{
	ESP8266_ATSendBuf(buf,len);
}	

//发送心跳包
void MQTT_SentHeart(void)
{
	MQTT_SendBuf((uint8_t *)parket_heart,sizeof(parket_heart));
}

//MQTT无条件断开
void MQTT_Disconnect()
{
	MQTT_SendBuf((uint8_t *)parket_disconnet,sizeof(parket_disconnet));
}

//MQTT初始化
void MQTT_Init(uint8_t *prx,uint16_t rxlen,uint8_t *ptx,uint16_t txlen)
{
	memset(Tx_String,0,Str_Len);  
	Clear_Rx(Rx_String);  
	
	//无条件先主动断开
	MQTT_Disconnect();HAL_Delay(100);
	MQTT_Disconnect();HAL_Delay(100);
}


//MQTT连接服务器的打包函数
uint8_t MQTT_Connect(char *ClientID,char *Username,char *Password)
{
	int ClientIDLen = strlen(ClientID);
	int UsernameLen = strlen(Username);
	int PasswordLen = strlen(Password);
	int DataLen;
	MQTT_TxLen=0;
	//可变报头+Payload  每个字段包含两个字节的长度标识
  DataLen = 10 + (ClientIDLen+2) + (UsernameLen+2) + (PasswordLen+2);
	
	//固定报头
	//控制报文类型
  Tx_String[MQTT_TxLen++] = 0x10;		//MQTT Message Type CONNECT
	//剩余长度(不包括固定头部)
	do
	{
		uint8_t encodedByte = DataLen % 128;
		DataLen = DataLen / 128;
		// if there are more data to encode, set the top bit of this byte
		if ( DataLen > 0 )
			encodedByte = encodedByte | 128;
		Tx_String[MQTT_TxLen++] = encodedByte;
	}while ( DataLen > 0 );
    	
	//可变报头
	//协议名
	Tx_String[MQTT_TxLen++] = 0;        		// Protocol Name Length MSB    
	Tx_String[MQTT_TxLen++] = 4;        		// Protocol Name Length LSB    
	Tx_String[MQTT_TxLen++] = 'M';        	// ASCII Code for M    
	Tx_String[MQTT_TxLen++] = 'Q';        	// ASCII Code for Q    
	Tx_String[MQTT_TxLen++] = 'T';        	// ASCII Code for T    
	Tx_String[MQTT_TxLen++] = 'T';        	// ASCII Code for T    
	//协议级别
	Tx_String[MQTT_TxLen++] = 4;        		// MQTT Protocol version = 4    
	//连接标志
	Tx_String[MQTT_TxLen++] = 0xc2;        	// conn flags 
	Tx_String[MQTT_TxLen++] = 0;        		// Keep-alive Time Length MSB    
	Tx_String[MQTT_TxLen++] = 60;        	// Keep-alive Time Length LSB  60S心跳包  

	Tx_String[MQTT_TxLen++] = BYTE1(ClientIDLen);// Client ID length MSB    
	Tx_String[MQTT_TxLen++] = BYTE0(ClientIDLen);// Client ID length LSB  	
	memcpy(&Tx_String[MQTT_TxLen],ClientID,ClientIDLen);
	MQTT_TxLen += ClientIDLen;
	
	if(UsernameLen > 0)
	{   
		Tx_String[MQTT_TxLen++] = BYTE1(UsernameLen);		//username length MSB    
		Tx_String[MQTT_TxLen++] = BYTE0(UsernameLen);    	//username length LSB    
		memcpy(&Tx_String[MQTT_TxLen],Username,UsernameLen);
		MQTT_TxLen += UsernameLen;
	}
	
	if(PasswordLen > 0)
	{    
		Tx_String[MQTT_TxLen++] = BYTE1(PasswordLen);		//password length MSB    
		Tx_String[MQTT_TxLen++] = BYTE0(PasswordLen);    	//password length LSB  
		memcpy(&Tx_String[MQTT_TxLen],Password,PasswordLen);
		MQTT_TxLen += PasswordLen; 
	}    
	
	uint8_t cnt=2;
	uint8_t wait;
	while(cnt--)
	{
		Clear_Rx(Rx_String);
		MQTT_SendBuf(Tx_String,MQTT_TxLen);
		wait=30;//等待3s时间
		while(wait--)
		{
			//CONNECT
			if(Rx_String[0]==parket_connetAck[0] && Rx_String[1]==parket_connetAck[1]) //连接成功			   
			{
				return 1;//连接成功
			}
			HAL_Delay(100);			
		}
	}
	return 0;
}


//MQTT订阅/取消订阅数据打包函数
//topic       主题 
//qos         消息等级 
//whether     订阅/取消订阅请求包
uint8_t MQTT_SubscribeTopic(char *topic,uint8_t qos,uint8_t whether)
{    
	MQTT_TxLen=0;
	int topiclen = strlen(topic);
	
	int DataLen = 2 + (topiclen+2) + (whether?1:0);//可变报头的长度（2字节）加上有效载荷的长度
	//固定报头
	//控制报文类型
	if(whether) Tx_String[MQTT_TxLen++] = 0x82; //消息类型和标志订阅
	else	Tx_String[MQTT_TxLen++] = 0xA2;    //取消订阅

	//剩余长度
	do
	{
		uint8_t encodedByte = DataLen % 128;
		DataLen = DataLen / 128;
		// if there are more data to encode, set the top bit of this byte
		if ( DataLen > 0 )
			encodedByte = encodedByte | 128;
		Tx_String[MQTT_TxLen++] = encodedByte;
	}while ( DataLen > 0 );	
	
	//可变报头
	Tx_String[MQTT_TxLen++] = 0;				//消息标识符 MSB
	Tx_String[MQTT_TxLen++] = 0x01;           //消息标识符 LSB
	//有效载荷
	Tx_String[MQTT_TxLen++] = BYTE1(topiclen);//主题长度 MSB
	Tx_String[MQTT_TxLen++] = BYTE0(topiclen);//主题长度 LSB   
	memcpy(&Tx_String[MQTT_TxLen],topic,topiclen);
	MQTT_TxLen += topiclen;
	if(whether)
	{
		Tx_String[MQTT_TxLen++] = qos;//QoS级别
	}
	
	uint8_t cnt=2;
	uint8_t wait;
	while(cnt--)
	{
		Clear_Rx(Rx_String);
		MQTT_SendBuf(Tx_String,MQTT_TxLen);
		wait=30;//等待3s时间
		while(wait--)
		{
			if(Rx_String[0]==parket_subAck[0] && Rx_String[1]==parket_subAck[1]) //订阅成功			   
			{
				return 1;//订阅成功
			}
			HAL_Delay(100);			
		}
	}
	if(cnt) return 1;	//订阅成功
	return 0;
}

//MQTT发布数据打包函数
//topic   主题 
//message 消息
//qos     消息等级 
uint8_t MQTT_PublishData(char *topic, char *message, uint8_t qos)
{  
	int topicLength = strlen(topic);    
	int messageLength = strlen(message);     
	static uint16_t id=0;
	int DataLen;
	MQTT_TxLen=0;
	//鏈夋晥杞借嵎鐨勯暱搴﹁繖鏍疯?＄畻锛氱敤鍥哄畾鎶ュご涓?鐨勫墿浣欓暱搴﹀瓧娈电殑鍊煎噺鍘诲彲鍙樻姤澶寸殑闀垮害
	//QOS涓?0鏃舵病鏈夋爣璇嗙??
	//鏁版嵁闀垮害             涓婚?樺??   鎶ユ枃鏍囪瘑绗?   鏈夋晥杞借嵎
	if(qos)	DataLen = (2+topicLength) + 2 + messageLength;       
	else	DataLen = (2+topicLength) + messageLength;   

    //鍥哄畾鎶ュご
	//鎺у埗鎶ユ枃绫诲瀷
	Tx_String[MQTT_TxLen++] = 0x30;    // MQTT Message Type PUBLISH  

	//鍓╀綑闀垮害
	do
	{
		uint8_t encodedByte = DataLen % 128;
		DataLen = DataLen / 128;
		// if there are more data to encode, set the top bit of this byte
		if ( DataLen > 0 )
			encodedByte = encodedByte | 128;
		Tx_String[MQTT_TxLen++] = encodedByte;
	}while ( DataLen > 0 );	
	
	Tx_String[MQTT_TxLen++] = BYTE1(topicLength);//涓婚?橀暱搴?MSB
	Tx_String[MQTT_TxLen++] = BYTE0(topicLength);//涓婚?橀暱搴?LSB 
	memcpy(&Tx_String[MQTT_TxLen],topic,topicLength);//鎷疯礉涓婚??
	MQTT_TxLen += topicLength;
        
	//鎶ユ枃鏍囪瘑绗?
	if(qos)
	{
			Tx_String[MQTT_TxLen++] = BYTE1(id);
			Tx_String[MQTT_TxLen++] = BYTE0(id);
			id++;
	}
	memcpy(&Tx_String[MQTT_TxLen],message,messageLength);
  MQTT_TxLen += messageLength;
        
	MQTT_SendBuf(Tx_String,MQTT_TxLen);
  return MQTT_TxLen;
}
void MQTT_CONNECTING(void)
{
	if(ESP8266_ConnectServer("TCP",MQTT_BROKERADDRESS,1883)!=0)
	{
		user_debug("TCP/IP 成功连接服务器.");
	}
	else 
	{
		user_debug("TCP/IP 无法连接MQTT服务器.");
		return;
	}
	
	if(MQTT_Connect(MQTT_CLIENTID, MQTT_USARNAME, MQTT_PASSWD) != 0)
	{
		user_debug("成功连接MQTT服务器");
	}
	else
	{
		user_debug("无法连接MQTT服务器!");
		return;
	}

	if(MQTT_SubscribeTopic(MQTT_SUBSCRIBE_TOPIC,0,1) != 0)
	{
		user_debug("MQTT Topic 订阅成功");
		HAL_TIM_Base_Start_IT(&htim2);
	}
	else {
		user_debug("MQTT subscribe");
	}

}
