/**
 * @file            wujique_sysconf.c
 * @brief           ϵͳ����
 * @author          wujique
 * @date            2018��4��9�� ����һ
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2018��4��9�� ����һ
 *   ��    ��:         wujique
 *   �޸�����:   �����ļ�
*/
#include "stm32f4xx.h"
#include "wujique_sysconf.h"
#include "mcu_i2c.h"
#include "mcu_spi.h"

#include "dev_spiflash.h"
#include "dev_lcdbus.h"

#include "dev_lcd.h"

/*

	���ļ���������ϵͳ����Щ�豸
	1.��ײ㣬����STM32��ƣ���������CPU��Ҫ��ֲ��

*/
/*-------------------------------
	I2C������
-------------------------------*/
/*
	IO��ģ���I2C1
	WM8978��TEA5767�������ӿڵ�I2Cʹ��
*/
const DevI2c DevVi2c1={
		.name = "VI2C1",
		
		.sclport = MCU_PORT_D,
		.sclpin = GPIO_Pin_6,

		.sdaport = MCU_PORT_D,
		.sdapin = GPIO_Pin_7,
		};
	
		
#if 0
/*
	����IO��ģ��I2C���;��󰴼���ģ��SPI��ͻ
*/
const DevI2c DevVi2c2={
		.name = "VI2C2",
		
		.sclport = MCU_PORT_F,
		.sclpin = GPIO_Pin_11,

		.sdaport = MCU_PORT_F,
		.sdapin = GPIO_Pin_10,
		};	
#endif
/*----------------------
	IO��ģ��SPI������
------------------------*/
/*
	VSPI1�����İ��ϵ�LCD�ӿ��е�4��IOģ��SPI��
	����XPT2046�������������ɶ���д��
*/					
const DevSpi DevVSpi1IO={
		.name = "VSPI1",
		.type = DEV_SPI_V,
		
		/*clk*/
		.clkport = MCU_PORT_B,
		.clkpin = GPIO_Pin_0,
		/*mosi*/
		.mosiport = MCU_PORT_D,
		.mosipin = GPIO_Pin_11,
		/*miso*/
		.misoport = MCU_PORT_D,
		.misopin = GPIO_Pin_12,
	};

#if 1
/*
	ģ��SPI����miso
	���ڲ���SPI������
*/
const DevSpi DevVspi3IO={
		.name = "VSPI3",
		.type = DEV_SPI_V,
		
		/*clk*/
		.clkport = MCU_PORT_G,
		.clkpin = GPIO_Pin_6,
		
		/*mosi*/
		.mosiport = MCU_PORT_F,
		.mosipin = GPIO_Pin_2,

		/*miso*/
		.misoport = NULL,
		.misopin = NULL,

	};
#endif

#if 1
/*  
	�����ӿ�ģ��VSPI2�� �������̣�ģ��I2C2��ͻ  
*/			
const DevSpi DevVspi2IO={
		.name = "VSPI2",
		.type = DEV_SPI_V,
		
		/*clk*/
		.clkport = MCU_PORT_F,
		.clkpin = GPIO_Pin_11,
		
		/*mosi*/
		.mosiport = MCU_PORT_F,
		.mosipin = GPIO_Pin_10,

		/*miso*/
		.misoport = MCU_PORT_F,
		.misopin = GPIO_Pin_9,

	};
#endif
/*
	Ӳ��SPI��������SPI3
	SPI������ʱ֧��SPI3��
	���������������������޸�mcu_spi.c�е�Ӳ��SPI��������ʼ������
*/
const DevSpi DevSpi3IO={
		.name = "SPI3",
		.type = DEV_SPI_H,
		
		/*clk*/
		.clkport = MCU_PORT_B,
		.clkpin = GPIO_Pin_3,
		
		/*mosi*/
		.mosiport = MCU_PORT_B,
		.mosipin = GPIO_Pin_5,

		/*miso*/
		.misoport = MCU_PORT_B,
		.misopin = GPIO_Pin_4,
	};

/*------------------------ 
	SPIͨ��
-------------------------*/
/*
	FLASH��
*/
const DevSpiCh DevSpi3CH1={
		.name = "SPI3_CH1",
		.spi = "SPI3",
		
		.csport = MCU_PORT_B,
		.cspin = GPIO_Pin_14,
		
	};		
/*
	FLASH��
*/
const DevSpiCh DevSpi3CH2={
		.name = "SPI3_CH2",
		.spi = "SPI3",
		
		.csport = MCU_PORT_G,
		.cspin = GPIO_Pin_15,
		
	};
/*
	�����ӿڵ�SPI���ɽ�COG��OLED��SPI TFT��RF24L01
*/			
const DevSpiCh DevSpi3CH3={
		.name = "SPI3_CH3",
		.spi = "SPI3",
		
		.csport = MCU_PORT_G,
		.cspin = GPIO_Pin_6,
		
	};
		
#if 0		
/*
	�����ӿڵ�SPI, 
	��DevSpi3CH3����ͬ��IO��
	��Ҫ�ǽ�SPI�ӿڵ�LCD,
	ͬʱ��������XPT2046��
	��ͨ�����ǿ���XPT2046�ġ�

	����ʵ���ϣ�SPI�ӿڵ�TFT 2.7��LCD̫���ˣ�
	ֻ���������Գ����ܡ�
*/
const DevSpiCh DevSpi3CH4={
		.name = "SPI3_CH4",
		.spi = "SPI3",
		
		.csport = MCU_PORT_F,
		.cspin = GPIO_Pin_2,
		
	};
#endif

#if 1
/*
	ģ��SPIͨ������CS
	��������û��CS��LCD��Ļ��
	VSPI3����ʵ�������ӿ�SPI����������IOģ��ġ�
*/	
const DevSpiCh DevVSpi3CH1={
		.name = "VSPI3_CH1",
		.spi = "VSPI3",
		
		.csport = NULL,
		.cspin = NULL,
		
	};
#endif

/* 
	LCD�����еĴ������ӿ�, IOģ��SPI
*/
const DevSpiCh DevVSpi1CH1={
		.name = "VSPI1_CH1",
		.spi = "VSPI1",
		
		.csport = MCU_PORT_B,
		.cspin = GPIO_Pin_1,
		
	};
#if 0		
/* 
	SPI������������������ͬ�Ŀ�����
*/		
const DevSpiCh DevVSpi1CH2={
		.name = "VSPI1_CH2",
		.spi = "VSPI1",
		
		.csport = MCU_PORT_D,
		.cspin = GPIO_Pin_14,
	};
#endif

#if 0
/*
	����IO VSPIͨ��
*/
const DevSpiCh DevVSpi2CH1={
		.name = "VSPI2_CH1",
		.spi = "VSPI2",
		
		.csport = MCU_PORT_F,
		.cspin = GPIO_Pin_12,
		
	};
#endif	

/*-------------------------------------
	LCDӲ���ӿ����߶���
	LCDӲ���ӿڰ���һ������ͨ�Žӿ�basebus��A0�ܽţ���λ�ܽţ�����ܽš�
---------------------------------------*/
#if 1
/*
	����LCD�ӿڣ�ʹ��������SPI����
	�����ӿ��е�SPI�ӿ�
*/
const DevLcdBus BusLcdSpi3={
	.name = "BusLcdSpi3",
	.type = LCD_BUS_SPI,
	.basebus = "SPI3_CH3",

	.A0port = MCU_PORT_G,
	.A0pin = GPIO_Pin_4,

	.rstport = MCU_PORT_G,
	.rstpin = GPIO_Pin_7,

	.blport = MCU_PORT_G,
	.blpin = GPIO_Pin_9,

	.staport = MCU_PORT_F, 
	.stapin = GPIO_Pin_2,
};
#else
/*
	������û��CS��MISO��1.33��LCD��
*/
const DevLcdBus BusLcdVSpi3={
	.name = "BusLcdVSpi3",
	.type = LCD_BUS_SPI,
	.basebus = "VSPI3_CH1",

	.A0port = MCU_PORT_G,
	.A0pin = GPIO_Pin_4,

	.rstport = MCU_PORT_G,
	.rstpin = GPIO_Pin_7,

	.blport = MCU_PORT_G,
	.blpin = GPIO_Pin_9,
};

#endif 
/*
	I2C�ӿڵ�LCD����
*/
const DevLcdBus BusLcdI2C1={
	.name = "BusLcdI2C1",
	.type = LCD_BUS_I2C,
	.basebus = "VI2C1",

	/*I2C�ӿڵ�LCD���ߣ�����Ҫ����IO*/

};
/*
	8080�ӿڵ�LCD����
*/	
const DevLcdBus BusLcd8080={
	.name = "BusLcd8080",
	.type = LCD_BUS_8080,
	.basebus = "8080",//��ʹ���ã�8080����ֱ��Ƕ����LCD BUS������

	/* 8080 ����A0�ţ�����ȥ */
	.A0port = MCU_PORT_B,
	.A0pin = GPIO_Pin_15,

	.rstport = MCU_PORT_A,
	.rstpin = GPIO_Pin_15,
	
	.blport = MCU_PORT_B,
	.blpin = GPIO_Pin_15,

};


#if 0
/* ģ��SPI2������IO���ɽ�SPI�ӿڵ��� */
const DevLcdBus BusLcdVSpi2CH1={
	.name = "BusLcdVSpi2CH1",
	.type = LCD_BUS_SPI,
	.basebus = "VSPI2_CH1",

	.A0port = MCU_PORT_F,
	.A0pin = GPIO_Pin_8,

	.rstport = MCU_PORT_F,
	.rstpin = GPIO_Pin_13,

	.blport = MCU_PORT_F,
	.blpin = GPIO_Pin_14,

	.staport = MCU_PORT_F, 
	.stapin = GPIO_Pin_15,
};
#endif

#if 0
/*
	
*/
const DevLcdBus BusLcdVSpi1CH2={
	.name = "BusLcdVSpi1CH2",
	.type = LCD_BUS_SPI,
	.basebus = "VSPI1_CH2",

	.A0port = MCU_PORT_D,
	.A0pin = GPIO_Pin_15,

	.rstport = MCU_PORT_A,
	.rstpin = GPIO_Pin_15,

	.blport = MCU_PORT_B,
	.blpin = GPIO_Pin_15,
};
#endif
/*----------------------------
	FLASH�豸����
-----------------------------*/
const DevSpiFlash DevSpiFlashCore={
	/*��һ������board_spiflash��SPI FLASH����DEV_SPI_3_2�ϣ��ͺ�δ֪*/
	"board_spiflash", 
	"SPI3_CH2", 
	NULL
};

const DevSpiFlash DevSpiFlashBoard={
	"core_spiflash",  
	"SPI3_CH1", 
	NULL
};

/*--------------------------------
	lcd�豸������
	ָ��ϵͳ�ж��ٸ�LCD�豸�������ĸ�LCD�����ϡ�
---------------------------------*/

/*I2C�ӿڵ� OLED*/

const DevLcd DevLcdOled1={
	.name = "i2coledlcd",  
	.buslcd = "BusLcdI2C1",  
	.id = 0X1315, 
	.width = 64, 
	.height = 128
	};

/*SPI�ӿڵ� OLED*/
/*
DevLcd DevLcdSpiOled	=	{
	.name = "spioledlcd", 	
	.buslcd = "BusLcdSpi3", 	
	.id = 0X1315, 
	.width = 64, 
	.height = 128};
*/
/*SPI�ӿڵ� COG LCD*/
	/*
const DevLcd DevLcdCOG1	=	{
	.name = "spicoglcd", 
	//.buslcd = "BusLcdVSpi2CH1", 
	.buslcd = "BusLcdSpi3",
	.id = 0X7565, 
	.width = 64, 
	.height = 128};
*/	
/*fsmc�ӿڵ� tft lcd*/
const DevLcd DevLcdtTFT	=	{"tftlcd", 		"BusLcd8080", 	NULL, 240, 320};
//const DevLcd DevLcdtTFT	=	{"tftlcd", 		"BusLcd8080", 	0x9325, 240, 320};
//const DevLcd DevLcdtTFT	=	{"tftlcd", 		"BusLcd8080", 	0x9341, 240, 320};
/*1408, 4.0���IPS��Ļ*/
//const DevLcd DevLcdtTFT	=	{"tftlcd", 		"BusLcd8080", 	0x1408, 480, 800};
/*SPI�ӿڵ� tft lcd*/
//const DevLcd DevLcdtTFT	=	{"spitftlcd", 		"BusLcdSpi3", 	0x9342, 240, 320};
//const DevLcd DevLcdtTFT	=	{"spitftlcd", 		"BusLcdVSpi1CH2", 	0x9342, 240, 320};
/*1.44�� �о�԰*/
//const DevLcd DevSpiLcdtTFT	=	{"spitftlcd",   "BusLcdSpi3", 	0x7735, 128, 128};

/* 1.3�磬IPS���о�԰��ֻ��SCL&SDA��SPI�ӿ�LCD*/
//const DevLcd DevLcdVSPITFT =	{"vspitftlcd",		"BusLcdVSpi3",	0x7789, 240, 240};

/* spi �ӿڵ���ɫīˮ�� */
//const DevLcd DevLcdSPIEPaper =	{"spiE-Paper",		"BusLcdSpi3",	0x9187, 176, 264};
/* spi �ӿ� �ڰ�īˮ�� 1.54�� GDEH154D27*/
//const DevLcd DevLcdSPIEPaper =	{"spiE-Paper",		"BusLcdSpi3",	0x3820, 200, 200};

/*
	ϵͳ�豸ע��
	ͨ���������ֲ㼶��������ϵ��
	�������Կ���ʵ�ֿɼ��ַ���ʾ���豸��
*/
s32 sys_dev_register(void)
{
	/*ע��I2C����*/
	mcu_i2c_register(&DevVi2c1);
			dev_lcdbus_register(&BusLcdI2C1);
					dev_lcd_register(&DevLcdOled1);
					
	#ifdef SYS_USE_VI2C2
	//mcu_i2c_register(&DevVi2c2);
	#endif
	
	/*ӲSPI3�����������İ�͵װ��FLASH�������ӿڵ�SPI��*/
	mcu_spi_register(&DevSpi3IO);
			mcu_spich_register(&DevSpi3CH1);
					dev_spiflash_register(&DevSpiFlashBoard);
			mcu_spich_register(&DevSpi3CH2);
					dev_spiflash_register(&DevSpiFlashCore);
			mcu_spich_register(&DevSpi3CH3);
					//dev_lcdbus_register(&BusLcdSpi3);
						//	dev_lcd_register(&DevLcdSPIEPaper);
							//dev_lcd_register(&DevLcdCOG1);
			//mcu_spich_register(&DevSpi3CH4);
	
	#ifdef SYS_USE_VSPI1
	mcu_spi_register(&DevVSpi1IO);
			mcu_spich_register(&DevVSpi1CH1);//8080�ӿڵĴ�����
			//mcu_spich_register(&DevVSpi1CH2);
					//dev_lcdbus_register(&BusLcdVSpi1CH2);
	#endif
	
	#ifdef SYS_USE_VSPI2
	mcu_spi_register(&DevVspi2IO);
			mcu_spich_register(&DevVSpi2CH1);
					dev_lcdbus_register(&BusLcdVSpi2CH1);
							dev_lcd_register(&DevLcdSpiOled);
	#endif
	
	/*����1.33��IPS��Ļ����*/
	#if 0
	mcu_spi_register(&DevVspi3IO);
			mcu_spich_register(&DevVSpi3CH1);
					dev_lcdbus_register(&BusLcdVSpi3);
							dev_lcd_register(&DevLcdVSPITFT);
	#endif
	dev_lcdbus_register(&BusLcd8080);
			dev_lcd_register(&DevLcdtTFT);
	
	return 0;
}

/*

Ӳ���������ñ��뷨������LINUX���豸��
ʵ��Ӧ���У��޸Ľӿ�IO�Ŀ�����Ӧ�ò���
�޸�����Ŀ����Խϴ�
������ݶ��LCD��������ʱ�򣬻����LCD��
�ܶ�LCD�޷��Զ�ʶ�����ԣ�����Ӳ�������ļ���á�

��ʽ��
[�㼶]����:����
	{
		����
	}

���磺	
{
	[0]cpu:stm32f407
		{}
		[1]VI2C:VI2C1
			{	
				
			}
			[2]LCDBUS:BusLcdI2C1
				{
					
				}
				[3]LCD:i2coledlcd
					{
						
					}

}


*/

