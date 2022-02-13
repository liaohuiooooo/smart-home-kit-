#include "mqtt.h"
#include "ESP8266.h"



extern uint8_t Rx_String[];
extern uint8_t Tx_String[];
extern uint8_t Rx_Once[];
extern uint32_t Rx_Counter;

//���ӳɹ���������Ӧ 20 02 00 00
//�ͻ��������Ͽ����� e0 00

const uint8_t parket_connetAck[] = {0x20,0x02,0x00,0x00};
const uint8_t parket_disconnet[] = {0xe0,0x00};
const uint8_t parket_heart[] = {0xc0,0x00};
const uint8_t parket_heart_reply[] = {0xc0,0x00};
const uint8_t parket_subAck[] = {0x90,0x03};

volatile uint16_t MQTT_TxLen;

//MQTT��������
void MQTT_SendBuf(uint8_t *buf,uint16_t len)
{
	ESP8266_ATSendBuf(buf,len);
}	

//����������
void MQTT_SentHeart(void)
{
	MQTT_SendBuf((uint8_t *)parket_heart,sizeof(parket_heart));
}

//MQTT�������Ͽ�
void MQTT_Disconnect()
{
	MQTT_SendBuf((uint8_t *)parket_disconnet,sizeof(parket_disconnet));
}

//MQTT��ʼ��
void MQTT_Init(uint8_t *prx,uint16_t rxlen,uint8_t *ptx,uint16_t txlen)
{
	memset(Tx_String,0,Str_Len);  
	Clear_Rx(Rx_String);  
	
	//�������������Ͽ�
	MQTT_Disconnect();HAL_Delay(100);
	MQTT_Disconnect();HAL_Delay(100);
}


//MQTT���ӷ������Ĵ������
uint8_t MQTT_Connect(char *ClientID,char *Username,char *Password)
{
	int ClientIDLen = strlen(ClientID);
	int UsernameLen = strlen(Username);
	int PasswordLen = strlen(Password);
	int DataLen;
	MQTT_TxLen=0;
	//�ɱ䱨ͷ+Payload  ÿ���ֶΰ��������ֽڵĳ��ȱ�ʶ
  DataLen = 10 + (ClientIDLen+2) + (UsernameLen+2) + (PasswordLen+2);
	
	//�̶���ͷ
	//���Ʊ�������
  Tx_String[MQTT_TxLen++] = 0x10;		//MQTT Message Type CONNECT
	//ʣ�೤��(�������̶�ͷ��)
	do
	{
		uint8_t encodedByte = DataLen % 128;
		DataLen = DataLen / 128;
		// if there are more data to encode, set the top bit of this byte
		if ( DataLen > 0 )
			encodedByte = encodedByte | 128;
		Tx_String[MQTT_TxLen++] = encodedByte;
	}while ( DataLen > 0 );
    	
	//�ɱ䱨ͷ
	//Э����
	Tx_String[MQTT_TxLen++] = 0;        		// Protocol Name Length MSB    
	Tx_String[MQTT_TxLen++] = 4;        		// Protocol Name Length LSB    
	Tx_String[MQTT_TxLen++] = 'M';        	// ASCII Code for M    
	Tx_String[MQTT_TxLen++] = 'Q';        	// ASCII Code for Q    
	Tx_String[MQTT_TxLen++] = 'T';        	// ASCII Code for T    
	Tx_String[MQTT_TxLen++] = 'T';        	// ASCII Code for T    
	//Э�鼶��
	Tx_String[MQTT_TxLen++] = 4;        		// MQTT Protocol version = 4    
	//���ӱ�־
	Tx_String[MQTT_TxLen++] = 0xc2;        	// conn flags 
	Tx_String[MQTT_TxLen++] = 0;        		// Keep-alive Time Length MSB    
	Tx_String[MQTT_TxLen++] = 60;        	// Keep-alive Time Length LSB  60S������  

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
		wait=30;//�ȴ�3sʱ��
		while(wait--)
		{
			//CONNECT
			if(Rx_String[0]==parket_connetAck[0] && Rx_String[1]==parket_connetAck[1]) //���ӳɹ�			   
			{
				return 1;//���ӳɹ�
			}
			HAL_Delay(100);			
		}
	}
	return 0;
}


//MQTT����/ȡ���������ݴ������
//topic       ���� 
//qos         ��Ϣ�ȼ� 
//whether     ����/ȡ�����������
uint8_t MQTT_SubscribeTopic(char *topic,uint8_t qos,uint8_t whether)
{    
	MQTT_TxLen=0;
	int topiclen = strlen(topic);
	
	int DataLen = 2 + (topiclen+2) + (whether?1:0);//�ɱ䱨ͷ�ĳ��ȣ�2�ֽڣ�������Ч�غɵĳ���
	//�̶���ͷ
	//���Ʊ�������
	if(whether) Tx_String[MQTT_TxLen++] = 0x82; //��Ϣ���ͺͱ�־����
	else	Tx_String[MQTT_TxLen++] = 0xA2;    //ȡ������

	//ʣ�೤��
	do
	{
		uint8_t encodedByte = DataLen % 128;
		DataLen = DataLen / 128;
		// if there are more data to encode, set the top bit of this byte
		if ( DataLen > 0 )
			encodedByte = encodedByte | 128;
		Tx_String[MQTT_TxLen++] = encodedByte;
	}while ( DataLen > 0 );	
	
	//�ɱ䱨ͷ
	Tx_String[MQTT_TxLen++] = 0;				//��Ϣ��ʶ�� MSB
	Tx_String[MQTT_TxLen++] = 0x01;           //��Ϣ��ʶ�� LSB
	//��Ч�غ�
	Tx_String[MQTT_TxLen++] = BYTE1(topiclen);//���ⳤ�� MSB
	Tx_String[MQTT_TxLen++] = BYTE0(topiclen);//���ⳤ�� LSB   
	memcpy(&Tx_String[MQTT_TxLen],topic,topiclen);
	MQTT_TxLen += topiclen;
	if(whether)
	{
		Tx_String[MQTT_TxLen++] = qos;//QoS����
	}
	
	uint8_t cnt=2;
	uint8_t wait;
	while(cnt--)
	{
		Clear_Rx(Rx_String);
		MQTT_SendBuf(Tx_String,MQTT_TxLen);
		wait=30;//�ȴ�3sʱ��
		while(wait--)
		{
			if(Rx_String[0]==parket_subAck[0] && Rx_String[1]==parket_subAck[1]) //���ĳɹ�			   
			{
				return 1;//���ĳɹ�
			}
			HAL_Delay(100);			
		}
	}
	if(cnt) return 1;	//���ĳɹ�
	return 0;
}

//MQTT�������ݴ������
//topic   ���� 
//message ��Ϣ
//qos     ��Ϣ�ȼ� 
uint8_t MQTT_PublishData(char *topic, char *message, uint8_t qos)
{  
	int topicLength = strlen(topic);    
	int messageLength = strlen(message);     
	static uint16_t id=0;
	int DataLen;
	MQTT_TxLen=0;
	//有效载荷的长度这样�?�算：用固定报头�?的剩余长度字段的值减去可变报头的长度
	//QOS�?0时没有标识�??
	//数据长度             主�?��??   报文标识�?   有效载荷
	if(qos)	DataLen = (2+topicLength) + 2 + messageLength;       
	else	DataLen = (2+topicLength) + messageLength;   

    //固定报头
	//控制报文类型
	Tx_String[MQTT_TxLen++] = 0x30;    // MQTT Message Type PUBLISH  

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
	
	Tx_String[MQTT_TxLen++] = BYTE1(topicLength);//主�?�长�?MSB
	Tx_String[MQTT_TxLen++] = BYTE0(topicLength);//主�?�长�?LSB 
	memcpy(&Tx_String[MQTT_TxLen],topic,topicLength);//拷贝主�??
	MQTT_TxLen += topicLength;
        
	//报文标识�?
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
		user_debug("TCP/IP �ɹ����ӷ�����.");
	}
	else 
	{
		user_debug("TCP/IP �޷�����MQTT������.");
		return;
	}
	
	if(MQTT_Connect(MQTT_CLIENTID, MQTT_USARNAME, MQTT_PASSWD) != 0)
	{
		user_debug("�ɹ�����MQTT������");
	}
	else
	{
		user_debug("�޷�����MQTT������!");
		return;
	}

	if(MQTT_SubscribeTopic(MQTT_SUBSCRIBE_TOPIC,0,1) != 0)
	{
		user_debug("MQTT Topic ���ĳɹ�");
		HAL_TIM_Base_Start_IT(&htim2);
	}
	else {
		user_debug("MQTT subscribe");
	}

}
