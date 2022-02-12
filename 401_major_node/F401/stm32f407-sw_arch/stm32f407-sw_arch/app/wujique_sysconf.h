/**
 * @file                wujique_sysconf.h
 * @brief           ϵͳ����
 * @author          wujique
 * @date            2018��3��30�� ������
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2018��3��30�� ������
 *   ��    ��:         wujique
 *   �޸�����:   �����ļ�
*/
#ifndef __WUJIQUE_SYSCONF__
#define __WUJIQUE_SYSCONF__

/* 

	�ú������Щ�����μӱ���

*/
#define TFT_LCD_DRIVER_COG12864
#define TFT_LCD_DRIVER_SSD1615
#define TFT_LCD_DRIVER_9341
#define TFT_LCD_DRIVER_9341_8BIT
#define TFT_LCD_DRIVER_9325
#define TFT_LCD_DRIVER_7735
#define TFT_LCD_DRIVER_7789
#define TFT_LCD_DRIVER_91874
#define TFT_LCD_DRIVER_3820



/*
	ѡ��������ⷽ��
*/

#define SYS_USE_TS_IC_CASE		1
//#define SYS_USE_TS_ADC_CASE 	1

#ifdef SYS_USE_TS_IC_CASE
/*
	xpt 2046ʹ�� ģ��SPI1_ch1
	��Ƶ����Ϊ0
*/
#define XPT2046_SPI "VSPI1_CH1"
#define XPT2046_SPI_PRE	0

/*
	���ʹ��Ӳ��SPI3_CH4,
	��Ƶ��������̫��
*/
//#define XPT2046_SPI "SPI3_CH4"
//#define XPT2046_SPI_PRE	SPI_BaudRatePrescaler_8

#endif

/*
	RS485�����������豸���á�
*/
//#define SYS_USE_RS485	1
//#define SYS_USE_EXUART	1
#define SYS_USE_USB	1
#define SYS_USE_CAMERA  1

/* 
	�������ӿڽ��й��ܶ���  
*/

/*
	VSPI2 VI2C2 KEYPAD ,3��ֻ����һ��
*/
//#define SYS_USE_VSPI2 1
//#define SYS_USE_VI2C2	1
#define SYS_USE_KEYPAD	1


#if (defined(SYS_USE_EXUART) && defined(SYS_USE_RS485))
 #error "please not define SYS_USE_EXUART & SYS_USE_RS485 sametime!(in wujique_sysconf.h file)"
#endif

#if (defined(SYS_USE_EXUART) && defined(SYS_USE_USB))
 #error "please not define SYS_USE_EXUART & SYS_USE_USB sametime!(in wujique_sysconf.h file)"
#endif

#if (defined(SYS_USE_EXUART) && defined(SYS_USE_CAMERA))
 #error "please not define SYS_USE_EXUART & SYS_USE_CAMERA sametime!(in wujique_sysconf.h file)"
#endif

#if (defined(SYS_USE_RS485) && defined(SYS_USE_USB))
 #error "please not define SYS_USE_RS485 & SYS_USE_USB sametime!(in wujique_sysconf.h file)"
#endif
#if (defined(SYS_USE_RS485) && defined(SYS_USE_CAMERA))
 #error "please not define SYS_USE_RS485 & SYS_USE_CAMERA sametime!(in wujique_sysconf.h file)"
#endif

/*
	����SPI2��ʹ������IO�������󰴼���ͻ����I2C2Ҳ����
*/
#if (defined(SYS_USE_VSPI2) && defined(SYS_USE_KEYPAD))
 #error "please not define SYS_USE_VSPI2 & SYS_USE_KEYPAD sametime!(in wujique_sysconf.h file)"
#endif
#if (defined(SYS_USE_VI2C2) && defined(SYS_USE_KEYPAD))
 #error "please not define SYS_USE_VI2C2 & SYS_USE_KEYPAD sametime!(in wujique_sysconf.h file)"
#endif
#if (defined(SYS_USE_VSPI2) && defined(SYS_USE_VI2C2))
 #error "please not define SYS_USE_VSPI2 & SYS_USE_VI2C2 sametime!(in wujique_sysconf.h file)"
#endif


/*��������ⷽ����xpt2046 ������ADC����ֻ��ѡһ��*/
#if defined(SYS_USE_TS_IC_CASE) && defined(SYS_USE_TS_ADC_CASE)
 #error "please just select one touch device!(in wujique_sysconf.h file)"
#endif

/*xpt2046��������VSPI1*/
#ifdef SYS_USE_TS_IC_CASE
	#define SYS_USE_VSPI1	1
#endif

/*
�Ƿ�ʹ��RTOS
*/
#define SYS_USE_RTOS

/* 
�ж����ȼ�ͳһ����
*/
#define NVIC_PRE(x) NVIC_PRE_PRI_##x
#define NVIC_SUB(x) NVIC_SUB_PRI_##x
/*
#define NVIC_PRE_PRI_
#define NVIC_SUB_PRI_
*/
/*
	USB �ж�
	
*/
#define NVIC_PRE_PRI_OTG_FS_IRQn	1
#define NVIC_SUB_PRI_OTG_FS_IRQn	1

#if 0
#define NVIC_PRE_PRI_OTG_HS_EP1_OUT_IRQn	1
#define NVIC_SUB_PRI_OTG_HS_EP1_OUT_IRQn	2
#define NVIC_PRE_PRI_OTG_HS_EP1_IN_IRQn 1
#define NVIC_SUB_PRI_OTG_HS_EP1_IN_IRQn 1
#endif
/*
	SDIO_IRQn 0 0
*/
#define NVIC_PRE_PRI_SDIO_IRQn	0
#define NVIC_SUB_PRI_SDIO_IRQn	0
/*
SD_SDIO_DMA_IRQn 0 1
*/
#define NVIC_PRE_PRI_SD_SDIO_DMA_IRQn	1
#define NVIC_SUB_PRI_SD_SDIO_DMA_IRQn	0


#define NVIC_PRE_PRI_USART1_IRQn	3
#define NVIC_SUB_PRI_USART1_IRQn	3

#define NVIC_PRE_PRI_USART2_IRQn	3
#define NVIC_SUB_PRI_USART2_IRQn	3

#define NVIC_PRE_PRI_USART3_IRQn	3
#define NVIC_SUB_PRI_USART3_IRQn	3

#define NVIC_PRE_PRI_TIM2_IRQn		1
#define NVIC_SUB_PRI_TIM2_IRQn  	1

#define NVIC_PRE_PRI_ADC_IRQn	1
#define NVIC_SUB_PRI_ADC_IRQn	1

#define NVIC_PRE_PRI_CAN1_RX0_IRQn	2
#define NVIC_SUB_PRI_CAN1_RX0_IRQn	0
/*
i2s ����
DMA1_Stream4_IRQn 0 0
*/
#define NVIC_PRE_PRI_DMA1_Stream4_IRQn	0
#define NVIC_SUB_PRI_DMA1_Stream4_IRQn 	0

/*
i2s ¼��
DMA1_Stream3_IRQn	0 0
*/
#define NVIC_PRE_PRI_DMA1_Stream3_IRQn	0
#define NVIC_SUB_PRI_DMA1_Stream3_IRQn	0

/*
���׵δ�ʱ��
TIM5_IRQn 2 2
*/
#define NVIC_PRE_PRI_TIM5_IRQn	2
#define NVIC_SUB_PRI_TIM5_IRQn	2

/*
	dac sound
	TIM3_IRQn 0 0
*/
#define NVIC_PRE_PRI_TIM3_IRQn	0
#define NVIC_SUB_PRI_TIM3_IRQn	0

/* 
adc ��ʽ���������̿���
TIM7_IRQn 1 1
*/
#define NVIC_PRE_PRI_TIM7_IRQn	1
#define NVIC_SUB_PRI_TIM7_IRQn	1

/*
���ڼ��
EXTI15_10_IRQn 1 1
*/
#define NVIC_PRE_PRI_EXTI15_10_IRQn	1
#define NVIC_SUB_PRI_EXTI15_10_IRQn	1

#include "alloc.h"

#include "mcu_uart.h"
#include "mcu_adc.h"
#include "mcu_can.h"
#include "mcu_dac.h"
#include "mcu_i2c.h"
#include "mcu_spi.h"
#include "mcu_timer.h"
#include "mcu_rtc.h"

#include "dev_buzzer.h"
#include "dev_key.h"
#include "dev_keypad.h"
#include "dev_dacsound.h"
#include "dev_spiflash.h"
#include "dev_tea5767.h"
#include "dev_wm8978.h"
#include "dev_lcd.h"
#include "dev_touchkey.h"
#include "dev_rs485.h"
#include "dev_esp8266.h"
#include "dev_touchkey.h"
#include "dev_touchscreen.h"
#include "soundplay.h"

#endif


