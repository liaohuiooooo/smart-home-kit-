#include "mqx.h"

float * AdcGetVal(void)
{
    user_debug("进入ADC1函数");
    static float adc_value[3];
    uint8_t i = 0;


    for(i=0; i<2; i++)
    {
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 0xffff);
        adc_value[i] = HAL_ADC_GetValue(&hadc1);
        adc_value[i] = adc_value[i] * 100.0 / 4096.0;
    }
    HAL_ADC_Stop(&hadc1);
    return adc_value;
}

void showMQX(float *arr)
{
    char mqtt_message[30];
    printf("ADC7: %.3f%%\t 通道8: %.3f%%\t\n",arr[0],arr[1]);

    sprintf(mqtt_message,"{\"MQ3\":%.3f, \"MQ7\":%.3f}",arr[0],arr[1]);
	MQTT_PublishData(MQTT_PUBLISH_TOPIC,mqtt_message,0);
}

