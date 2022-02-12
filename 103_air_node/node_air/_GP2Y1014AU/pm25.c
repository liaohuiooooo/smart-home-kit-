#include "pm25.h"

uint32_t getADC2(void)
{
	float value = 0;
	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, 0xffff);
	value = HAL_ADC_GetValue(&hadc2);
	HAL_ADC_Stop(&hadc2);
	return value;
    
}

uint16_t GetGP2Y(void)
{
    uint32_t ADC_VAL = 0;
    int dustVal = 0;
    float Voltage = 0;

    PLED_ON;
	delay_us(320);
	ADC_VAL = getADC2();
	PLED_OFF;
	delay_us(9680);
	Voltage = 3.3f * ADC_VAL / 4096.f * 2; //获得AO输出口的电压值
	
	dustVal = (0.17*Voltage-0.1)*1000;  //乘以1000单位换成ug/m3//
	
	printf("\r\n电压值:%fV\n",Voltage);

	printf("\r\n灰尘浓度:%d ug/m3\n",dustVal);
	
	return dustVal;

}

