#include "esp8266.h"

//UART4 ���ͺͽ�������
//������main.c�ļ���

uint32_t Rx_Counter;

uint8_t Rx_String[Str_Len];
uint8_t Tx_String[Str_Len];
uint8_t Rx_Once[Str_Len];





void Clear_Rx(uint8_t *String)
{
	Rx_Counter = 0;
	memset(String,0,Str_Len);
}

void Clear_Tx(uint8_t *String)
{
	memset(String,0,Str_Len);
}



//��ESP8266���Ͷ�������
void ESP8266_ATSendBuf(uint8_t* buf,uint16_t len)
{
	//ÿ�η���ǰ�����մ��ڽ���������0,Ϊ�˽���
	Clear_Rx(Rx_String);
	//��������
	HAL_UART_Transmit(&huart2,(uint8_t *)buf,len,0xFFFF);
}

//��ESP8266�����ַ���
void ESP8266_ATSendString(char* str)
{
	memset(Rx_String,0, Str_Len);
	
	//ÿ�η���ǰ�����մ��ڽ���������0,Ϊ�˽���
	Rx_Counter = 0;	
	
	uint8_t k=0;
	do 
	{
	  HAL_UART_Transmit(&huart2,(uint8_t *)(str + k) ,1,1000);
	  k++;
	} while(*(str + k)!='\0');
}

//�˳�͸��
void ESP8266_ExitUnvarnishedTrans(void)
{
	ESP8266_ATSendString("+++");delay_ms(50);
	ESP8266_ATSendString("+++");delay_ms(50);	
}

//�����ַ������Ƿ������һ���ַ���
//���ڷ���1
uint8_t FindStr(char* dest,char* src,uint16_t retry_us)
{
	

	while(strstr(dest,src)==0 && retry_us)//�ȴ����ڽ�����ϻ�ʱ�˳�
	{		
		delay_ms(1);
		retry_us--;
	}

	if(retry_us) return 1;                       

	return 0; 
}

/**
 * ���ܣ����ESP8266�Ƿ�����
 * ������None
 * ����ֵ��ESP8266����״̬
 *        ��0 ESP8266����
 *        0 ESP8266������  
 */
uint8_t ESP8266_Check(void)
{
	uint8_t check_cnt=5;
	while(check_cnt--)
	{
		Clear_Rx(Rx_String);	 						//��ս��ջ���
		ESP8266_ATSendString("AT\r\n");
		delay_ms(200);
		if(strstr((char*)Rx_String,"OK"))
		{
			user_debug("ESP01s �ѷ��֡�");
			Clear_Rx(Rx_String);
			return 1;
		}
		delay_ms(200);
	}
	user_debug("ESP01s δ���֡� ");
	return 0;
}

/**
 * ���ܣ���ʼ��ESP8266
 * ������None
 * ����ֵ����ʼ���������0Ϊ��ʼ���ɹ�,0Ϊʧ��
 */
uint8_t ESP8266_Init(void)
{
	//��շ��ͺͽ�������
	memset(Rx_String,0,Str_Len);
	memset(Tx_String,0,Str_Len);
	Rx_Counter = 0;
	
	ESP8266_ExitUnvarnishedTrans();		//�˳�͸��
	ESP8266_ATSendString("AT+RST\r\n");
	ESP8266_ATSendString("AT+RST\r\n");
	delay_ms(500);
	if(!ESP8266_Check())              //ʹ��ATָ����ESP8266�Ƿ����
	{
		user_debug("ESP01s �� MCU ͨ�Ų�����.");//���������������ǣ�ESPCHECH�޷���⵽UART4_IT���ݣ�ͨ���޸�NVIC���жϼ��𣬱�ɽ����
		return 0;
	}
	else{
		user_debug("ESP01s �� MCU ͨ������.");
	}
	
	Clear_Rx(Rx_String);   //��ս��ջ���
//	ESP8266_ATSendString("ATE0\r\n");     	//�رջ���
//	delay_ms(1000);
//	if(!FindStr((char*)Rx_String,"OK",1500))  //���ò��ɹ�
//	{
//		user_debug("�رջ���ʧ��");
//		return 0;      
//	}
//	user_debug("�رջ��Գɹ�");
	return 1;                         //���óɹ�
}

/**
 * ���ܣ��ָ���������
 * ������None
 * ����ֵ��None
 * ˵��:��ʱESP8266�е��û����ý�ȫ����ʧ�ظ��ɳ���״̬
 */
void ESP8266_Restore(void)
{
	ESP8266_ExitUnvarnishedTrans();          	//�˳�͸��
	delay_ms(500);
	ESP8266_ATSendString("AT+RESTORE\r\n");		//�ָ����� 	
}

/**
 * ���ܣ������ȵ�
 * ������
 *         ssid:�ȵ���
 *         pwd:�ȵ�����
 * ����ֵ��
 *         ���ӽ��,��0���ӳɹ�,0����ʧ��
 * ˵���� 
 *         ʧ�ܵ�ԭ�������¼���(UARTͨ�ź�ESP8266���������)
 *         1. WIFI�������벻��ȷ
 *         2. ·���������豸̫��,δ�ܸ�ESP8266����IP
 */
uint8_t ESP8266_ConnectAP(char* ssid,char* pswd)
{
	uint8_t cnt=5;
	while(cnt--)
	{
		Clear_Rx(Rx_String);   
		ESP8266_ATSendString("AT+CWMODE_CUR=1\r\n");              //����ΪSTATIONģʽ	
		if(FindStr((char*)Rx_String,"OK",200) != 0)
		{
			break;
		}             		
	}
	if(cnt == 0)
		return 0;

	cnt=2;
	while(cnt--)
	{                    
		Clear_Rx(Rx_String);
		memset(Tx_String,0,Str_Len);//��ս��ջ���
		sprintf((char*)Tx_String,"AT+CWJAP_CUR=\"%s\",\"%s\"\r\n",ssid,pswd);//����Ŀ��AP
		ESP8266_ATSendString((char*)Tx_String);	
		if(FindStr((char*)Rx_String,"OK",5000))                      //���ӳɹ��ҷ��䵽IP
		{
			return 1;
		}
	}
	return 0;
}

//����͸��ģʽ
static uint8_t ESP8266_OpenTransmission(void)
{
	//����͸��ģʽ
	uint8_t cnt=2;
	while(cnt--)
	{
		memset(Rx_String,0,Str_Len);   
		Rx_Counter = 0;
		ESP8266_ATSendString("AT+CIPMODE=1\r\n");  
		if(FindStr((char*)Rx_String,"OK",500)!=0)
		{	
			return 1;
		}
	}
	return 0;
}

/**
 * ���ܣ�ʹ��ָ��Э��(TCP/UDP)���ӵ�������
 * ������
 *         mode:Э������ "TCP","UDP"
 *         ip:Ŀ�������IP
 *         port:Ŀ���Ƿ������˿ں�
 * ����ֵ��
 *         ���ӽ��,��0���ӳɹ�,0����ʧ��
 * ˵���� 
 *         ʧ�ܵ�ԭ�������¼���(UARTͨ�ź�ESP8266���������)
 *         1. Զ�̷�����IP�Ͷ˿ں�����
 *         2. δ����AP
 *         3. �������˽�ֹ���(һ�㲻�ᷢ��)
 */
uint8_t ESP8266_ConnectServer(char* mode,char* ip,uint16_t port)
{
	uint8_t cnt;
   
	ESP8266_ExitUnvarnishedTrans();                   //����������˳�͸��
	delay_ms(500);
	user_debug("�˳�͸��");
	//���ӷ�����
	cnt=2;
	while(cnt--)
	{
		memset(Rx_String,0,Str_Len);//��շ��ͻ���
		memset(Tx_String,0,Str_Len);//��ս��ջ���
		Rx_Counter = 0;
		sprintf((char*)Tx_String,"AT+CIPSTART=\"%s\",\"%s\",%d\r\n",mode,ip,port);
		ESP8266_ATSendString((char*)Tx_String);
		delay_ms(500);
		if(FindStr((char*)Rx_String,"CONNECT",5000) !=0 )
		{
			break;
		}
	}
	if(cnt == 0) 
		return 0;
	
	//����͸��ģʽ
	if(ESP8266_OpenTransmission()==0) return 0;
	user_debug("����͸��ģʽ");
	//��������״̬
	cnt=2;
	while(cnt--)
	{
		memset(Rx_String,0,Str_Len); //��ս��ջ���
		Rx_Counter = 0;
		ESP8266_ATSendString("AT+CIPSEND\r\n");//��ʼ����͸������״̬
		delay_ms(500);
		if(FindStr((char*)Rx_String,">",200)!=0)
		{
			return 1;
		}
	}
	return 0;
}

/**
 * ���ܣ������ͷ������Ͽ�����
 * ������None
 * ����ֵ��
 *         ���ӽ��,��0�Ͽ��ɹ�,0�Ͽ�ʧ��
 */
uint8_t DisconnectServer(void)
{
	uint8_t cnt;
	
	ESP8266_ExitUnvarnishedTrans();	//�˳�͸��
	delay_ms(500);
	
	while(cnt--)
	{
		memset(Rx_String,0,Str_Len); //��ս��ջ���  
		Rx_Counter = 0;
		ESP8266_ATSendString("AT+CIPCLOSE\r\n");//�ر�����

		if(FindStr((char*)Rx_String,"CLOSED",200)!=0)//�����ɹ�,�ͷ������ɹ��Ͽ�
		{
			break;
		}
	}
	if(cnt) return 1;
	return 0;
}


uint8_t ESP01S_CONNECT(void)
{
	uint8_t status=0;
	//��ʼ��
	if(ESP8266_Init())
	{
		user_debug("ESP8266��ʼ����...\r\n");
		status++;
	}

	//�����ȵ�
	if(status==1)
	{
		if(ESP8266_ConnectAP(WIFI_NAME,WIFI_PASSWD))
		{
			user_debug("ESP826 �Ѿ�����WiFi��\r\n");
			status++;
		}
	}
	return status;
}
