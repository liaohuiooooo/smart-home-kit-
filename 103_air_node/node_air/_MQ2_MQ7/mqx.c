#include "mqx.h"

float * AdcGetVal(void)
{
    user_debug("����ADC1����");
    static float adc_value[3];
    uint8_t i = 0;


    for(i=0; i<3; i++)
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
    printf("ADC1 ͨ��7: %.3f%% ͨ��8: %.3f%% ͨ��9: %.3f%%\n",\
    arr[0],arr[1],arr[2]);
}

