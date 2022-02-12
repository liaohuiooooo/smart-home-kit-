/**
********************************************************
*
* @file      Hal_temp_hum.c
* @author    Gizwtis
* @version   V03010100
* @date      2016-07-05
*
* @brief     机智云.只为智能硬件而生
*            Gizwits Smart Cloud  for Smart Products
*            链接|增值?|开放|中立|安全|自有|自由|生态
*            www.gizwits.com
*
*********************************************************/
#include "DHT11.h"
#include <string.h>
#include "tim.h"

thTypedef_t temphumTypedef;
#define Delayms TIM1_Delay_ms
//在里面添加自定义的延时函数  定时器1  微秒级延时
void delayUs(uint16_t us)
{
	TIM1_Delay_us(us);
}



void DHT11_IO_OUT(void)
{
		GPIO_InitTypeDef myGPIO_InitStruct;
		myGPIO_InitStruct.Pin = DHT11_IO_Pin;
		myGPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		myGPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(DHT11_IO_GPIO_Port, &myGPIO_InitStruct);
}

void DHT11_IO_IN(void)
{
		GPIO_InitTypeDef myGPIO_InitStruct;
		myGPIO_InitStruct.Pin = DHT11_IO_Pin;
		myGPIO_InitStruct.Pull = GPIO_PULLUP;
		myGPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		myGPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(DHT11_IO_GPIO_Port, &myGPIO_InitStruct);
}
//Reset DHT11
static void dht11Rst(void)
{
    DHT11_IO_OUT(); 			            //SET OUTPUT
    DHT11_DQ_OUT_0; 			            //GPIOA.0=0
    Delayms(22);    			          //Pull down Least 18ms
    DHT11_DQ_OUT_1; 			            //GPIOA.0=1
    delayUs(30);     			            //Pull up 20~40us
}

static uint8_t dht11Check(void)
{
    uint8_t retry=0;
    DHT11_IO_IN();                                              //SET INPUT
    while (DHT11_DQ_IN && (retry<100))                          //DHT11 Pull down 40~80us
    {
        retry++;
        delayUs(1);
    }

    if(retry >= 100)
    {
        return 1;
    }
    else
    {
        retry=0;
    }

    while (!DHT11_DQ_IN&& (retry < 100))		    //DHT11 Pull up 40~80us
    {
        retry++;
        delayUs(1);
    }

    if(retry >= 100)
    {
        return 1;	                        //check error
    }        

    return 0;
}

static uint8_t dht11ReadBit(void)
{
    uint8_t retry=0;
    while(DHT11_DQ_IN && (retry<100))                           //wait become Low level
    {
        retry++;
        delayUs(1);
    }

    retry = 0;
    while(!DHT11_DQ_IN && (retry < 100))		    //wait become High level
    {
        retry++;
        delayUs(1);
    }

    delayUs(30);//wait 40us

    if(DHT11_DQ_IN)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static uint8_t dht11ReadByte(void)
{
    uint8_t i,dat;
    dat=0;
    for (i=0; i<8; i++)
    {
        dat<<=1;
        dat |= dht11ReadBit();
    }

    return dat;
}

static uint8_t dht11ReadData(uint8_t *temperature, uint8_t *humidity)
{
    uint8_t buf[5];
    uint8_t i;
    dht11Rst(); 
    if(0 == dht11Check()) 
    {
        for(i=0; i<5; i++)
        {
            buf[i] = dht11ReadByte();
        }
        if(buf[4] == (buf[0]+buf[1]+buf[2]+buf[3]))
        {
            *humidity = buf[0];
            *temperature = buf[2];
        }
    }
    else
    {
        return 1;
    }

    return 0;
}

uint8_t dht11Read(uint8_t *temperature, uint8_t *humidity)
{
    uint8_t curTem = 0, curHum = 0;
    uint16_t temMeans = 0, humMeans = 0;
    uint8_t curI = 0;
    uint8_t ret = 0; 

    ret = dht11ReadData(&curTem, &curHum);

    if(1 != ret) 
    {
        //Cycle store ten times stronghold
        if(MEAN_NUM > temphumTypedef.curI)
        {
            temphumTypedef.thBufs[temphumTypedef.curI][0] = curTem;
            temphumTypedef.thBufs[temphumTypedef.curI][1] = curHum;

            temphumTypedef.curI++;
        }
        else
        {
            temphumTypedef.curI = 0;

            temphumTypedef.thBufs[temphumTypedef.curI][0] = curTem;
            temphumTypedef.thBufs[temphumTypedef.curI][1] = curHum;

            temphumTypedef.curI++;
        }
    }
    else
    {
        return (1); 
    }
    
    if(MEAN_NUM <= temphumTypedef.curI) 
    {
        temphumTypedef.thAmount = MEAN_NUM;
    }

    if(0 == temphumTypedef.thAmount) 
    {
        //Calculate Before ten the mean
        for(curI = 0; curI < temphumTypedef.curI; curI++)
        {
            temMeans += temphumTypedef.thBufs[curI][0];
            humMeans += temphumTypedef.thBufs[curI][1];
        }

        temMeans = temMeans / temphumTypedef.curI;
        humMeans = humMeans / temphumTypedef.curI; 
        
        *temperature = temMeans;
        *humidity = humMeans;
    }
    else if(MEAN_NUM == temphumTypedef.thAmount) 
    {
        //Calculate After ten times the mean
        for(curI = 0; curI < temphumTypedef.thAmount; curI++) 
        {
            temMeans += temphumTypedef.thBufs[curI][0];
            humMeans += temphumTypedef.thBufs[curI][1];
        }

        temMeans = temMeans / temphumTypedef.thAmount; 
        humMeans = humMeans / temphumTypedef.thAmount; 
        
        *temperature = (uint8_t)temMeans; 
        *humidity = (uint8_t)humMeans; 
    }

    return (0);
}

uint8_t dht11Init(void)
{
    /* Migrate your driver code */


    dht11Rst(); 
    
    memset((uint8_t *)&temphumTypedef, 0, sizeof(thTypedef_t)); 
    
    //printf("dh11Init \r\n"); 
    
    return dht11Check(); 
}



void Temperture_Measure(void)
{
	uint8_t temperature=0;
	uint8_t humidity=0;
	uint8_t get_times;
	char mqtt_message[300];

	// 获取温湿度信息并用串口打印,获取十次，直到成功跳出
	for(get_times=0;get_times<10;get_times++)
	{
		if(!dht11Read(&temperature, &humidity))//Read DHT11 Value
		{
			user_main_debug("temperture :%d \n humidity:%d \n",temperature,humidity);
			break;
		}
	}
	sprintf(mqtt_message,
	"{\"temp\":%.1f, \"hum\":%.lf}",
	(float)temperature,(float)humidity
	);
	

	MQTT_PublishData(MQTT_PUBLISH_TOPIC,mqtt_message,0);
	user_main_debug("Published!");

}

