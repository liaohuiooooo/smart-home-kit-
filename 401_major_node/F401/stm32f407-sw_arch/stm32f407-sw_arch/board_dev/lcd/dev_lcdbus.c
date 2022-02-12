/**
 * @file            dev_lcdbus.c
 * @brief           �Ը���LCD�ӿڷ�װ
 * @author          wujique
 * @date            2018��4��18�� ������
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2018��4��18�� ������
 *   ��    ��:         wujique
 *   �޸�����:   �����ļ�
	1 Դ����ݼ�ȸ���������С�
	2 �������ڵ�������ҵ��;�����׿��������۳��⣩��������Ȩ��
	3 �ݼ�ȸ�����Ҳ��Դ��빦�����κα�֤����ʹ�������в��ԣ�����Ը���
	4 �������޸�Դ�벢�ַ���������ֱ�����۱���������������뱣��WUJIQUE��Ȩ˵����
	5 �緢��BUG�����Ż�����ӭ�������¡�����ϵ��code@wujique.com
	6 ʹ�ñ�Դ�����൱����ͬ����Ȩ˵����
	7 ���ַ����Ȩ��������ϵ��code@wujique.com
	8 һ�н���Ȩ���ݼ�ȸ���������С�
*/
#include <stdarg.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "main.h"
#include "wujique_log.h"
#include "list.h"
#include "mcu_spi.h"
#include "mcu_i2c.h"
#include "dev_lcdbus.h"


/*
	д�Ĵ���Ҫ����
	*LcdReg = LCD_Reg; //д��Ҫд�ļĴ������
	*LcdData = LCD_RegValue; //д������ 
*/

volatile u16 *LcdReg = (u16*)0x6C000000;
volatile u16 *LcdData = (u16*)0x6C010000;

/*
	һ��LCD�ӿ�
	����ͨ�ŵĽӿ�
	��������������ͨ�Žӿڵ��ź�
*/

/*LCD �����豸�ڵ���ڵ�*/
struct list_head DevBusLcdRoot = {&DevBusLcdRoot, &DevBusLcdRoot};	
/**
 *@brief:      bus_lcd_IO_init
 *@details:    ��ʼ��lcd�ӿ�IO
 *@param[in]   const DevLcdBus *dev  
 *@param[out]  ��
 *@retval:     
 */
static void bus_lcd_IO_init(const DevLcdBus *dev) 
{

	if(dev->type == LCD_BUS_I2C)
		return;

	/* ��ʼ���ܽ� */
	mcu_io_config_out(dev->A0port,dev->A0pin);
	mcu_io_output_setbit(dev->A0port,dev->A0pin);

	mcu_io_config_out(dev->rstport,dev->rstpin);
	mcu_io_output_setbit(dev->rstport,dev->rstpin);
	
	mcu_io_config_out(dev->blport,dev->blpin);
	mcu_io_output_setbit(dev->blport,dev->blpin);

}
/**
 *@brief:     
 *@details:   
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
s32 bus_lcd_bl(DevLcdBusNode *node, u8 sta)
{
	if(sta ==1)
	{
		mcu_io_output_setbit(node->dev.blport, node->dev.blpin);
	}
	else
	{
		mcu_io_output_resetbit(node->dev.blport, node->dev.blpin);	
	}
	return 0;
}
/**
 *@brief:     
 *@details:   
 *@param[in]    
 *@param[out]  
 *@retval:     
 */

s32 bus_lcd_rst(DevLcdBusNode *node, u8 sta)
{
	if(sta ==1)
	{
		mcu_io_output_setbit(node->dev.rstport, node->dev.rstpin);
	}
	else
	{
		mcu_io_output_resetbit(node->dev.rstport, node->dev.rstpin);	
	}
	return 0;
}
/**
 *@brief:     
 *@details:   
 *@param[in]    
 *@param[out]  
 *@retval:     
 */

DevLcdBusNode *bus_lcd_open(char *name)
{
	/*���豸*/
	DevLcdBusNode *node;
	struct list_head *listp;

	//wjq_log(LOG_INFO, "lcd bus name:%s!\r\n", name);
	
	listp = DevBusLcdRoot.next;
	node = NULL;
	
	while(1)
	{
		if(listp == &DevBusLcdRoot)
			break;

		node = list_entry(listp, DevLcdBusNode, list);
		//wjq_log(LOG_INFO, "lcd bus name:%s!\r\n", node->dev.name);
		
		if(strcmp(name, node->dev.name) == 0)
		{
			break;
		}
		else
		{
			node = NULL;
		}
		
		listp = listp->next;
	}

	if(node != NULL)
	{
		if(node->gd == 0)
		{
			wjq_log(LOG_INFO, "lcd bus open err:using!\r\n");
			node = NULL;
		}
		else
		{
			
			if(node->dev.type == LCD_BUS_SPI)
			{
				node->basenode = (void *)mcu_spi_open(node->dev.basebus, SPI_MODE_3, SPI_BaudRatePrescaler_2);

			}
			else if(node->dev.type == LCD_BUS_I2C)
			{
				node->basenode = mcu_i2c_open(node->dev.basebus);
			}
			else if(node->dev.type == LCD_BUS_8080)
			{
				/*8080���⴦��*/
				node->basenode = (void *)1;
			}
			
			if(node->basenode == NULL)
			{
				wjq_log(LOG_INFO, "lcd bus open base bus err!\r\n");	
				node =  NULL;
			}
			else
			{
				node->gd = 0;

			}
		}
	}
	else
	{
		wjq_log(LOG_INFO, "lcd bus open err:%s!\r\n", name);
	}

	return node;
}
/**
 *@brief:     
 *@details:   
 *@param[in]    
 *@param[out]  
 *@retval:     
 */

s32 bus_lcd_close(DevLcdBusNode *node)
{
	if(node->gd != 0)
		return -1;
	
	if(node->dev.type == LCD_BUS_SPI)
	{
		mcu_spi_close((DevSpiChNode *)node->basenode);
		
	}
	else if(node->dev.type == LCD_BUS_I2C)
	{
		mcu_i2c_close((DevI2cNode *)node->basenode);	
	}
	else if(node->dev.type == LCD_BUS_8080)
	{
		/*8080���⴦��*/
		node->basenode = NULL;
	}
	
	node->gd = -1;
	
	return 0;
}


/**
 *@brief:     
 *@details:   
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
s32 bus_lcd_write_data(DevLcdBusNode *node, u8 *data, u32 len)
{
	/*ֱ�Ӷ���256�ֽڣ�������BUG��Ҫ����len��̬����*/
	u8 tmp[256];
	u32 i;

	switch(node->dev.type)
	{
		case LCD_BUS_SPI:
		{
			mcu_spi_transfer((DevSpiChNode *)node->basenode,  data, NULL, len);
		}
		break;
		case LCD_BUS_I2C:
		{
			tmp[0] = 0x40;
			memcpy(&tmp[1], data, len);
			mcu_i2c_transfer((DevI2cNode *)node->basenode, 0x3C, MCU_I2C_MODE_W, tmp, len+1);
		}
		break;
		case LCD_BUS_8080:			
		{
			u16 *p;
			p = (u16 *)data;
			for(i=0; i<len; i++)
			{
				*LcdData = *(p+i);	
			}
		}
		break;
		default:
			break;
	}
	
	return 0;
}
#if 0//FSMC����DMA
#define FSMC_LCD_ADDRESS      0x6C010000

void bus_lcd_fsmc_dma_init(u32 MemoryAddr,	u32 len)
{
	DMA_InitTypeDef  DMA_InitStructure;
	
	/* DMA2 Stream1 Configuration */
	DMA_DeInit(DMA2_Stream0);
	while (DMA_GetCmdStatus(DMA2_Stream0) != DISABLE)
  	{
  		//wjq_log(LOG_DEBUG, "W-");
  	}
	
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;	
	DMA_InitStructure.DMA_PeripheralBaseAddr = MemoryAddr;   
	DMA_InitStructure.DMA_Memory0BaseAddr = FSMC_LCD_ADDRESS;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;
	DMA_InitStructure.DMA_BufferSize = len*2;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	/* DMA2 IRQ channel Configuration */
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);
	
	DMA_Cmd(DMA2_Stream0, ENABLE); 

	while ((DMA_GetCmdStatus(DMA2_Stream0) != ENABLE))
  	{
  		//wjq_log(LOG_DEBUG, "E-");
  	}
}


/**
 *@brief:     ��������ʽ��������
 *@details:   
 *@param[in]    
 *@param[out]  
 *@retval:     
 			����8080���������ͣ���ˢͼ�������������ݲ����á�
 */
s32 bus_lcd_flush_data(DevLcdBusNode *node, u8 *data, u32 len)
{
	/*ֱ�Ӷ���256�ֽڣ�������BUG��Ҫ����len��̬����*/
	u8 tmp[256];
	u32 i;

	switch(node->dev.type)
	{
		case LCD_BUS_SPI:
		{
			mcu_spi_transfer((DevSpiChNode *)node->basenode,  data, NULL, len);
		}
		break;
		
		case LCD_BUS_I2C:
		{
			tmp[0] = 0x40;
			memcpy(&tmp[1], data, len);
			mcu_i2c_transfer((DevI2cNode *)node->basenode, 0x3C, MCU_I2C_MODE_W, tmp, len+1);
		}
		break;
		
		case LCD_BUS_8080:			
		{
			bus_lcd_fsmc_dma_init((u32)data, len);	
			
			while(1)
			{
				//wjq_log(LOG_DEBUG, "W");
				if(SET == DMA_GetFlagStatus(DMA2_Stream0, DMA_FLAG_TCIF0))
				{
					DMA_ClearFlag(DMA2_Stream0, DMA_FLAG_TCIF0);
					break;
				}
			}
		}
		break;
		
		default:
			break;
	}
	
	return 0;
}
/**
 *@brief:  �ȴ�flush�������   
 *@details:   
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
s32 bus_lcd_flush_wait(DevLcdBusNode *node)
{
	switch(node->dev.type)
	{
		case LCD_BUS_SPI:
		{

		}
		break;
		case LCD_BUS_I2C:
		{

		}
		break;
		case LCD_BUS_8080:			
		{
			if(DMA_GetCmdStatus(DMA2_Stream0) == DISABLE)
			{
				DMA_ClearFlag(DMA2_Stream0, DMA_FLAG_TCIF0);
				return 0;
			}
			while(1)
			{
				//wjq_log(LOG_DEBUG, "W");
				if(SET == DMA_GetFlagStatus(DMA2_Stream0, DMA_FLAG_TCIF0))
				{
					DMA_ClearFlag(DMA2_Stream0, DMA_FLAG_TCIF0);
					break;
				}
			}			
		}
		break;
		default:
			break;
	}
	
	return 0;
}
#endif
/**
 *@brief:     
 *@details:   
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
s32 bus_lcd_read_data(DevLcdBusNode *node, u8 *data, u32 len)
{
	u32 i;
	
	switch(node->dev.type)
	{
		case LCD_BUS_SPI:
		{	
			mcu_spi_transfer((DevSpiChNode *)node->basenode,  NULL, data, len);
		}
		break;
		
		case LCD_BUS_I2C:
		{	

		}
		break;
		
		case LCD_BUS_8080:
		{
			u16 *p;
			p = (u16 *)data;
			
			for(i=0; i<len; i++)
			{
				*(p+i) = *LcdData;	
			}
		}
		break;
		default:
			break;
	}
	return 0;	
}
/**
 *@brief:     
 *@details:   
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
s32 bus_lcd_write_cmd(DevLcdBusNode *node, u8 cmd)
{
	u8 tmp[2];

	switch(node->dev.type)
	{
		case LCD_BUS_SPI:
		{	
			mcu_io_output_resetbit(node->dev.A0port, node->dev.A0pin);
			tmp[0] = cmd;
			mcu_spi_transfer((DevSpiChNode *)node->basenode,  &tmp[0], NULL, 1);
			mcu_io_output_setbit(node->dev.A0port, node->dev.A0pin);
		}
		break;
		
		case LCD_BUS_I2C:
		{	
			tmp[0] = 0x00;
			tmp[1] = cmd;
			
			mcu_i2c_transfer((DevI2cNode *)node->basenode, 0x3C, MCU_I2C_MODE_W, tmp, 2);
		}
		break;
		
		case LCD_BUS_8080:
		{
			*LcdReg = cmd;	
		}
		break;
		default:
			break;
	}
	return 0;
}
/**
 *@brief:      dev_lcdbus_register
 *@details:    ע��LCD�ӿ�
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_lcdbus_register(const DevLcdBus *dev)
{
	struct list_head *listp;
	DevLcdBusNode *p;

	wjq_log(LOG_INFO, "[register] lcd bus :%s, base on:%s!\r\n", dev->name, dev->basebus);

	/*
		��Ҫ��ѯ��ǰ����ֹ����
	*/
	listp = DevBusLcdRoot.next;
	while(1)
	{
		if(listp == &DevBusLcdRoot)
			break;

		p = list_entry(listp, DevLcdBusNode, list);

		if(strcmp(dev->name, p->dev.name) == 0)
		{
			wjq_log(LOG_INFO, "bus lcd dev name err!\r\n");
			return -1;
		}
		
		listp = listp->next;
	}

	/* 
		����һ���ڵ�ռ� 
		
	*/
	p = (DevLcdBusNode *)wjq_malloc(sizeof(DevLcdBusNode));
	list_add(&(p->list), &DevBusLcdRoot);
	/*�����豸��Ϣ*/
	memcpy((u8 *)&p->dev, (u8 *)dev, sizeof(DevLcdBus));
	p->gd = -1;

	/*��ʼ��*/
	bus_lcd_IO_init(dev);

	if(dev->type == LCD_BUS_8080)
	{
		//��ʼFSMC
		mcu_fsmc_lcd_Init();
	}
	return 0;
}





