#include "esp8266.h"

//UART4 发送和接收数组
//定义在main.c文件中

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



//向ESP8266发送定长数据
void ESP8266_ATSendBuf(uint8_t* buf,uint16_t len)
{
	//每次发送前将接收串口接收总数置0,为了接收
	Clear_Rx(Rx_String);
	//定长发送
	HAL_UART_Transmit(&huart2,(uint8_t *)buf,len,0xFFFF);
}

//向ESP8266发送字符串
void ESP8266_ATSendString(char* str)
{
	memset(Rx_String,0, Str_Len);
	
	//每次发送前将接收串口接收总数置0,为了接收
	Rx_Counter = 0;	
	
	uint8_t k=0;
	do 
	{
	  HAL_UART_Transmit(&huart2,(uint8_t *)(str + k) ,1,1000);
	  k++;
	} while(*(str + k)!='\0');
}

//退出透传
void ESP8266_ExitUnvarnishedTrans(void)
{
	ESP8266_ATSendString("+++");delay_ms(50);
	ESP8266_ATSendString("+++");delay_ms(50);	
}

//查找字符串中是否包含另一个字符串
//存在返回1
uint8_t FindStr(char* dest,char* src,uint16_t retry_us)
{
	

	while(strstr(dest,src)==0 && retry_us)//等待串口接收完毕或超时退出
	{		
		delay_ms(1);
		retry_us--;
	}

	if(retry_us) return 1;                       

	return 0; 
}

/**
 * 功能：检查ESP8266是否正常
 * 参数：None
 * 返回值：ESP8266返回状态
 *        非0 ESP8266正常
 *        0 ESP8266有问题  
 */
uint8_t ESP8266_Check(void)
{
	uint8_t check_cnt=5;
	while(check_cnt--)
	{
		Clear_Rx(Rx_String);	 						//清空接收缓冲
		ESP8266_ATSendString("AT\r\n");
		delay_ms(200);
		if(strstr((char*)Rx_String,"OK"))
		{
			user_debug("ESP01s 已发现。");
			Clear_Rx(Rx_String);
			return 1;
		}
		delay_ms(200);
	}
	user_debug("ESP01s 未发现。 ");
	return 0;
}

/**
 * 功能：初始化ESP8266
 * 参数：None
 * 返回值：初始化结果，非0为初始化成功,0为失败
 */
uint8_t ESP8266_Init(void)
{
	//清空发送和接收数组
	memset(Rx_String,0,Str_Len);
	memset(Tx_String,0,Str_Len);
	Rx_Counter = 0;
	
	ESP8266_ExitUnvarnishedTrans();		//退出透传
	ESP8266_ATSendString("AT+RST\r\n");
	ESP8266_ATSendString("AT+RST\r\n");
	delay_ms(500);
	if(!ESP8266_Check())              //使用AT指令检查ESP8266是否存在
	{
		user_debug("ESP01s 与 MCU 通信不正常.");//这里遇到了问题是，ESPCHECH无法检测到UART4_IT数据，通过修改NVIC的中断级别，便可解决；
		return 0;
	}
	else{
		user_debug("ESP01s 与 MCU 通信正常.");
	}
	
	Clear_Rx(Rx_String);   //清空接收缓冲
//	ESP8266_ATSendString("ATE0\r\n");     	//关闭回显
//	delay_ms(1000);
//	if(!FindStr((char*)Rx_String,"OK",1500))  //设置不成功
//	{
//		user_debug("关闭回显失败");
//		return 0;      
//	}
//	user_debug("关闭回显成功");
	return 1;                         //设置成功
}

/**
 * 功能：恢复出厂设置
 * 参数：None
 * 返回值：None
 * 说明:此时ESP8266中的用户设置将全部丢失回复成出厂状态
 */
void ESP8266_Restore(void)
{
	ESP8266_ExitUnvarnishedTrans();          	//退出透传
	delay_ms(500);
	ESP8266_ATSendString("AT+RESTORE\r\n");		//恢复出厂 	
}

/**
 * 功能：连接热点
 * 参数：
 *         ssid:热点名
 *         pwd:热点密码
 * 返回值：
 *         连接结果,非0连接成功,0连接失败
 * 说明： 
 *         失败的原因有以下几种(UART通信和ESP8266正常情况下)
 *         1. WIFI名和密码不正确
 *         2. 路由器连接设备太多,未能给ESP8266分配IP
 */
uint8_t ESP8266_ConnectAP(char* ssid,char* pswd)
{
	uint8_t cnt=5;
	while(cnt--)
	{
		Clear_Rx(Rx_String);   
		ESP8266_ATSendString("AT+CWMODE_CUR=1\r\n");              //设置为STATION模式	
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
		memset(Tx_String,0,Str_Len);//清空接收缓冲
		sprintf((char*)Tx_String,"AT+CWJAP_CUR=\"%s\",\"%s\"\r\n",ssid,pswd);//连接目标AP
		ESP8266_ATSendString((char*)Tx_String);	
		if(FindStr((char*)Rx_String,"OK",5000))                      //连接成功且分配到IP
		{
			return 1;
		}
	}
	return 0;
}

//开启透传模式
static uint8_t ESP8266_OpenTransmission(void)
{
	//设置透传模式
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
 * 功能：使用指定协议(TCP/UDP)连接到服务器
 * 参数：
 *         mode:协议类型 "TCP","UDP"
 *         ip:目标服务器IP
 *         port:目标是服务器端口号
 * 返回值：
 *         连接结果,非0连接成功,0连接失败
 * 说明： 
 *         失败的原因有以下几种(UART通信和ESP8266正常情况下)
 *         1. 远程服务器IP和端口号有误
 *         2. 未连接AP
 *         3. 服务器端禁止添加(一般不会发生)
 */
uint8_t ESP8266_ConnectServer(char* mode,char* ip,uint16_t port)
{
	uint8_t cnt;
   
	ESP8266_ExitUnvarnishedTrans();                   //多次连接需退出透传
	delay_ms(500);
	user_debug("退出透传");
	//连接服务器
	cnt=2;
	while(cnt--)
	{
		memset(Rx_String,0,Str_Len);//清空发送缓冲
		memset(Tx_String,0,Str_Len);//清空接收缓冲
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
	
	//设置透传模式
	if(ESP8266_OpenTransmission()==0) return 0;
	user_debug("进入透传模式");
	//开启发送状态
	cnt=2;
	while(cnt--)
	{
		memset(Rx_String,0,Str_Len); //清空接收缓冲
		Rx_Counter = 0;
		ESP8266_ATSendString("AT+CIPSEND\r\n");//开始处于透传发送状态
		delay_ms(500);
		if(FindStr((char*)Rx_String,">",200)!=0)
		{
			return 1;
		}
	}
	return 0;
}

/**
 * 功能：主动和服务器断开连接
 * 参数：None
 * 返回值：
 *         连接结果,非0断开成功,0断开失败
 */
uint8_t DisconnectServer(void)
{
	uint8_t cnt;
	
	ESP8266_ExitUnvarnishedTrans();	//退出透传
	delay_ms(500);
	
	while(cnt--)
	{
		memset(Rx_String,0,Str_Len); //清空接收缓冲  
		Rx_Counter = 0;
		ESP8266_ATSendString("AT+CIPCLOSE\r\n");//关闭链接

		if(FindStr((char*)Rx_String,"CLOSED",200)!=0)//操作成功,和服务器成功断开
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
	//初始化
	if(ESP8266_Init())
	{
		user_debug("ESP8266初始化中...\r\n");
		status++;
	}

	//连接热点
	if(status==1)
	{
		if(ESP8266_ConnectAP(WIFI_NAME,WIFI_PASSWD))
		{
			user_debug("ESP826 已经连接WiFi！\r\n");
			status++;
		}
	}
	return status;
}
