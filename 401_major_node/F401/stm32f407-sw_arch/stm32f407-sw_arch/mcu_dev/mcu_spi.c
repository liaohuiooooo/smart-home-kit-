/**
 * @file            mcu_spi.c
 * @brief           Ƭ��SPI����������
 * @author          test
 * @date            2017��10��26�� ������
 * @version         ����
 * @par             
 * @par History:
 * 1.��    ��:      2017��10��26�� ������
 *   ��    ��:      test
 *   �޸�����:      �����ļ�
		��Ȩ˵����
		1 Դ����ݼ�ȸ���������С�
		2 �������ڵ�������ҵ��;�����׿��������۳��⣩��������Ȩ��
		3 �ݼ�ȸ�����Ҳ��Դ��빦�����κα�֤����ʹ�������в��ԣ�����Ը���
		4 �������޸�Դ�벢�ַ���������ֱ�����۱�������������ұ�����Ȩ˵����
		5 �緢��BUG�����Ż�����ӭ�������¡�����ϵ��code@wujique.com
		6 ʹ�ñ�Դ�����൱����ͬ����Ȩ˵����
		7 ���ַ����Ȩ��������ϵ��code@wujique.com
		8 һ�н���Ȩ���ݼ�ȸ���������С�
*/
#include "stm32f4xx.h"
#include "wujique_log.h"
#include "list.h"
#include "mcu_spi.h"
#include "wujique_sysconf.h"

#define MCU_SPI_DEBUG

#ifdef MCU_SPI_DEBUG
#define SPI_DEBUG	wjq_log 
#else
#define SPI_DEBUG(a, ...)
#endif

/*

	���ļ���Ҫ�������¹��ܣ�
	1 Ӳ��SPI
	2 IOģ��SPI
	3 SPI����������
	4 SPIͨ������ͨ�����ڿ�����
	5 ����ͳһ�ӿ�

*/


#define MCU_SPI_WAIT_TIMEOUT 0x40000

/*
	��λ���ã�һ������ģʽ
*/
typedef struct
{
	u16 CPOL;
	u16 CPHA;	
}_strSpiModeSet;

const _strSpiModeSet SpiModeSet[SPI_MODE_MAX]=
	{
		{SPI_CPOL_Low, SPI_CPHA_1Edge},
		{SPI_CPOL_Low, SPI_CPHA_2Edge},
		{SPI_CPOL_High, SPI_CPHA_1Edge},
		{SPI_CPOL_High, SPI_CPHA_2Edge}
	};

extern const GPIO_TypeDef *Stm32PortList[16];



/**
 *@brief:      mcu_spi_init
 *@details:    ��ʼ��SPI����������ʱ֧��SPI3
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
static s32 mcu_hspi_init(const DevSpi *dev)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStruct;
	uint8_t GPIO_AF;
	uint32_t RCC_CLK;
	uint16_t pinsource;
	
	/*����IO��*/
    GPIO_InitStructure.GPIO_Pin = dev->clkpin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//---���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//---�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//---100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//---����
    GPIO_Init((GPIO_TypeDef *)Stm32PortList[dev->clkport], &GPIO_InitStructure);//---��ʼ��

	GPIO_InitStructure.GPIO_Pin = dev->misopin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//---���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//---�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//---100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//---����
    GPIO_Init((GPIO_TypeDef *)Stm32PortList[dev->misoport], &GPIO_InitStructure);//---��ʼ��

	GPIO_InitStructure.GPIO_Pin = dev->mosipin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//---���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//---�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//---100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//---����
    GPIO_Init((GPIO_TypeDef *)Stm32PortList[dev->mosiport], &GPIO_InitStructure);//---��ʼ��


    //�������Ÿ���ӳ��
    if(strcmp(dev->name, "SPI3") == 0)
    {
		GPIO_AF = GPIO_AF_SPI3;
		RCC_CLK = RCC_APB1Periph_SPI3;
    }
	
	pinsource = log2(dev->clkpin);
    GPIO_PinAFConfig((GPIO_TypeDef *)Stm32PortList[dev->clkport], pinsource,  GPIO_AF); //����
    pinsource = log2(dev->misopin);
    GPIO_PinAFConfig((GPIO_TypeDef *)Stm32PortList[dev->misoport], pinsource, GPIO_AF); //����
    pinsource = log2(dev->mosipin);
    GPIO_PinAFConfig((GPIO_TypeDef *)Stm32PortList[dev->mosiport], pinsource, GPIO_AF); //����

    RCC_APB1PeriphClockCmd(RCC_CLK, ENABLE);// ---ʹ��ʱ��
   
    return 0;
}

/**
 *@brief:      mcu_spi_open
 *@details:       ��SPI
 *@param[in]   SPI_DEV dev  ��SPI��
               u8 mode      ģʽ
               u16 pre      Ԥ��Ƶ
 *@param[out]  ��
 *@retval:     
 */
static s32 mcu_hspi_open(DevSpiNode *node, SPI_MODE mode, u16 pre)
{
	SPI_InitTypeDef SPI_InitStruct;
	SPI_TypeDef* SPIC;
	
	if(node->gd != -1)
	{
		//SPI_DEBUG(LOG_DEBUG, "spi dev busy\r\n");
		return -1;
	}
	
	if(mode >= SPI_MODE_MAX)
		return -1;

	if(strcmp(node->dev.name, "SPI3") == 0)
    {
		SPIC = SPI3;
    }

	SPI_I2S_DeInit(SPIC);
	SPI_Cmd(SPIC, DISABLE); 
	
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//---˫��˫��ȫ˫��
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;//---��ģʽ
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//---8bit֡�ṹ
    SPI_InitStruct.SPI_CPOL = SpiModeSet[mode].CPOL;
    SPI_InitStruct.SPI_CPHA = SpiModeSet[mode].CPHA;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft; //---SPI_NSS_Hard; Ƭѡ��Ӳ������SPI������������
    SPI_InitStruct.SPI_BaudRatePrescaler = pre;  //---Ԥ��Ƶ
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//---���ݴ���� MSB λ��ʼ
    SPI_InitStruct.SPI_CRCPolynomial = 7;//---CRC ֵ����Ķ���ʽ

    SPI_Init(SPIC, &SPI_InitStruct);

	SPI_Cmd(SPIC, ENABLE);
	
	node->gd = 0;
		
    return 0;
}
/**
 *@brief:      mcu_spi_close
 *@details:    �ر�SPI ������
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
static s32 mcu_hspi_close(DevSpiNode *node)
{
    SPI_TypeDef* SPIC;
	
	if(node->gd != 0)
		return -1;
	
	if(strcmp(node->dev.name, "SPI3") == 0)
    {
		SPIC = SPI3;
    }
	
	SPI_Cmd(SPIC, DISABLE);
	node->gd = -1;
    return 0;
}
/**
 *@brief:      mcu_spi_transfer
 *@details:    SPI ����
 *@param[in]   u8 *snd  
               u8 *rsv  
               s32 len  
 *@param[out]  ��
 *@retval:     
 */
static s32 mcu_hspi_transfer(DevSpiNode *node, u8 *snd, u8 *rsv, s32 len)
{
    s32 i = 0;
    s32 pos = 0;
    u32 time_out = 0;
    u16 ch;
	SPI_TypeDef* SPIC;
	
	if(node == NULL)
		return -1;

	if(node->gd != 0)
	{
		SPI_DEBUG(LOG_DEBUG, "spi dev no open\r\n");
		return -1;
	}
	
    if( ((snd == NULL) && (rsv == NULL)) || (len < 0) )
    {
        return -1;
    }
	
    if(strcmp(node->dev.name, "SPI3") == 0)
    {
		SPIC = SPI3;
    }
    /* æ�ȴ� */
    time_out = 0;
    while(SPI_I2S_GetFlagStatus(SPIC, SPI_I2S_FLAG_BSY) == SET)
    {
        if(time_out++ > MCU_SPI_WAIT_TIMEOUT)
        {
            return(-1);
        }
    }

    /* ���SPI�������ݣ���ֹ�����ϴδ������������� */
    time_out = 0;
    while(SPI_I2S_GetFlagStatus(SPIC, SPI_I2S_FLAG_RXNE) == SET)
    {
        SPI_I2S_ReceiveData(SPIC);
        if(time_out++ > 2)
        {
            return(-1);
        }
    }

    /* ��ʼ���� */
    for(i=0; i < len; )
    {
        // д����
        if(snd == NULL)/*����ָ��ΪNULL��˵�������Ƕ����� */
        {
            SPI_I2S_SendData(SPIC, 0xff);
        }
        else
        {
            ch = (u16)snd[i];
            SPI_I2S_SendData(SPIC, ch);
        }
        i++;
        
        // �ȴ����ս���
        time_out = 0;
        while(SPI_I2S_GetFlagStatus(SPIC, SPI_I2S_FLAG_RXNE) == RESET)
        {
            time_out++;
            if(time_out > MCU_SPI_WAIT_TIMEOUT)
            {
                return -1;
            }    
        }
        // ������
        if(rsv == NULL)/* ����ָ��Ϊ�գ������ݺ��� */
        {
            SPI_I2S_ReceiveData(SPIC);
        }
        else
        {
            ch = SPI_I2S_ReceiveData(SPIC);
            rsv[pos] = (u8)ch;
        } 
        pos++;

    }

    return i;
}

/**
 *@brief:      mcu_vspi_init
 *@details:    ��ʼ������SPI
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
static s32 mcu_vspi_init(const DevSpi *dev)
{

	wjq_log(LOG_DEBUG, "vspi init:%s\r\n", dev->name);

	mcu_io_config_out(dev->clkport, dev->clkpin);
	mcu_io_output_setbit(dev->clkport,dev->clkpin);

	mcu_io_config_out(dev->mosiport, dev->mosipin);
	mcu_io_output_setbit(dev->mosiport, dev->mosipin);


	mcu_io_config_in(dev->misoport, dev->misopin);
	mcu_io_output_setbit(dev->misoport, dev->misopin);
	
	return 0;
}


/**
 *@brief:      vspi_delay
 *@details:    ����SPIʱ����ʱ
 *@param[in]   u32 delay  
 *@param[out]  ��
 *@retval:     
 */
void vspi_delay(u32 delay)
{
	volatile u32 i=delay;

	while(i>0)
	{
		i--;	
	}

}

u32 VspiDelay = 0;

/**
 *@brief:      mcu_vspi_open
 *@details:    ������SPI
 *@param[in]   SPI_DEV dev    
               SPI_MODE mode  
               u16 pre        
 *@param[out]  ��
 *@retval:     
 */
static s32 mcu_vspi_open(DevSpiNode *node, SPI_MODE mode, u16 pre)
{

	if(node == NULL)
		return -1;
	
	if(node->gd != -1)
	{
		//SPI_DEBUG(LOG_DEBUG, "vspi dev busy\r\n");
		return -1;
	}
	
	//SPI_DEBUG(LOG_DEBUG, "vo-");
	
	node->clk = pre;
	node->gd = 0;
		
    return 0;
}
/**
 *@brief:      mcu_vspi_close
 *@details:    �ر�����SPI
 *@param[in]   SPI_DEV dev  
 *@param[out]  ��
 *@retval:     
 */
static s32 mcu_vspi_close(DevSpiNode *node)
{
	if(node->gd != 0)
		return -1;
	//SPI_DEBUG(LOG_DEBUG, "vc-");
	node->gd = -1;
	
    return 0;
}
/**
 *@brief:      mcu_vspi_transfer
 *@details:       ����SPIͨ��
 *@param[in]   SPI_DEV dev  
               u8 *snd      
               u8 *rsv      
               s32 len      
 *@param[out]  ��
 *@retval:     

 		node->clk = 0, CLKʱ��1.5M 2018.06.02
 */
static s32 mcu_vspi_transfer(DevSpiNode *node, u8 *snd, u8 *rsv, s32 len)
{
	u8 i;
	u8 data;
	s32 slen;
	u8 misosta;

	volatile u16 delay;
	
	DevSpi *dev;
	
	if(node == NULL)
	{
		SPI_DEBUG(LOG_DEBUG, "vspi dev err\r\n");
		return -1;
	}

	if(node->gd != 0)
	{
		SPI_DEBUG(LOG_DEBUG, "vspi dev no open\r\n");
		return -1;
	}
	
    if( ((snd == NULL) && (rsv == NULL)) || (len < 0) )
    {
        return -1;
    }

	dev = &(node->dev);

	slen = 0;

	while(1)
	{
		if(slen >= len)
			break;

		if(snd == NULL)
			data = 0xff;
		else
			data = *(snd+slen);
		
		for(i=0; i<8; i++)
		{
			mcu_io_output_resetbit(dev->clkport, dev->clkpin);

			delay = node->clk;
			while(delay>0)
			{
				delay--;	
			}
			
			if(data&0x80)
				mcu_io_output_setbit(dev->mosiport, dev->mosipin);
			else
				mcu_io_output_resetbit(dev->mosiport, dev->mosipin);
			
			delay = node->clk;
			while(delay>0)
			{
				delay--;	
			}
			
			data<<=1;
			mcu_io_output_setbit(dev->clkport, dev->clkpin);
			
			delay = node->clk;
			while(delay>0)
			{
				delay--;	
			}
			
			misosta = mcu_io_input_readbit(dev->misoport, dev->misopin);
			if(misosta == Bit_SET)
			{
				data |=0x01;
			}
			else
			{
				data &=0xfe;
			}
			
			delay = node->clk;
			while(delay>0)
			{
				delay--;	
			}
			
		}

		if(rsv != NULL)
			*(rsv+slen) = data;
		
		slen++;
	}

	return slen;
}

/*


	����SPIͳһ����ӿ�


*/
struct list_head DevSpiRoot = {&DevSpiRoot, &DevSpiRoot};
/**
 *@brief:      mcu_spi_register
 *@details:    ע��SPI�������豸
 *@param[in]   DevSpi *dev      
 *@param[out]  ��
 *@retval:     
 */
s32 mcu_spi_register(const DevSpi *dev)
{

	struct list_head *listp;
	DevSpiNode *p;
	
	wjq_log(LOG_INFO, "[register] spi :%s!\r\n", dev->name);

	/*
		��Ҫ��ѯ��ǰ����ֹ����
	*/
	listp = DevSpiRoot.next;
	while(1)
	{
		if(listp == &DevSpiRoot)
			break;

		p = list_entry(listp, DevSpiNode, list);

		if(strcmp(dev->name, p->dev.name) == 0)
		{
			wjq_log(LOG_INFO, "spi dev name err!\r\n");
			return -1;
		}
		
		listp = listp->next;
	}

	/* 
		����һ���ڵ�ռ� 
		
	*/
	p = (DevSpiNode *)wjq_malloc(sizeof(DevSpiNode));
	list_add(&(p->list), &DevSpiRoot);

	memcpy((u8 *)&p->dev, (u8 *)dev, sizeof(DevSpi));
	

	/*��ʼ��*/
	if(dev->type == DEV_SPI_V)
		mcu_vspi_init(dev);
	else if(dev->type == DEV_SPI_H)
		mcu_hspi_init(dev);
	
	p->gd = -1;
	
	return 0;
}

struct list_head DevSpiChRoot = {&DevSpiChRoot, &DevSpiChRoot};
/**
 *@brief:      mcu_spich_register
 *@details:    ע��SPIͨ��
 *@param[in]   DevSpiCh *dev     
 *@param[out]  ��
 *@retval:     
 */
s32 mcu_spich_register(const DevSpiCh *dev)
{
	struct list_head *listp;
	DevSpiChNode *p;
	DevSpiNode *p_spi;
	
	wjq_log(LOG_INFO, "[register] spi ch :%s!\r\n", dev->name);

	/*
		��Ҫ��ѯ��ǰ����ֹ����
	*/
	listp = DevSpiChRoot.next;
	while(1)
	{
		if(listp == &DevSpiChRoot)
			break;

		p = list_entry(listp, DevSpiChNode, list);
		
		if(strcmp(dev->name, p->dev.name) == 0)
		{
			wjq_log(LOG_INFO, ">--------------------spi ch dev name err!\r\n");
			return -1;
		}
		
		listp = listp->next;
	}

	/* Ѱ��SPI������*/
	listp = DevSpiRoot.next;
	while(1)
	{
		if(listp == &DevSpiRoot)
		{
			wjq_log(LOG_INFO, ">---------------------spi ch reg err:no spi!\r\n");
			return -1;
		}


		p_spi = list_entry(listp, DevSpiNode, list);

		if(strcmp(dev->spi, p_spi->dev.name) == 0)
		{
			//wjq_log(LOG_INFO, "spi ch find spi!\r\n");
			break;
		}
		
		listp = listp->next;
	}
	/* 
		����һ���ڵ�ռ� 
		
	*/
	p = (DevSpiChNode *)wjq_malloc(sizeof(DevSpiChNode));
	list_add(&(p->list), &DevSpiChRoot);
	memcpy((u8 *)&p->dev, (u8 *)dev, sizeof(DevSpiCh));
	p->gd = -1;
	p->spi = p_spi;

	/* ��ʼ���ܽ� */
	mcu_io_config_out(dev->csport,dev->cspin);
	mcu_io_output_setbit(dev->csport,dev->cspin);

	return 0;
}


/**
 *@brief:      mcu_spi_open
 *@details:    ��SPIͨ��
 *@param[in]   DevSpiChNode * node
               u8 mode      ģʽ
               u16 pre      Ԥ��Ƶ
 *@param[out]  ��
 *@retval:     
 			   ��һ��SPI����F407�ϴ��Ҫ2us
 */
DevSpiChNode *mcu_spi_open(char *name, SPI_MODE mode, u16 pre)
{

	s32 res;
	DevSpiChNode *node;
	struct list_head *listp;
	
	//SPI_DEBUG(LOG_INFO, "spi ch open:%s!\r\n", name);

	listp = DevSpiChRoot.next;
	node = NULL;
	
	while(1)
	{
		if(listp == &DevSpiChRoot)
			break;

		node = list_entry(listp, DevSpiChNode, list);
		//SPI_DEBUG(LOG_INFO, "spi ch name%s!\r\n", node->dev.name);
		
		if(strcmp(name, node->dev.name) == 0)
		{
			//SPI_DEBUG(LOG_INFO, "spi ch dev get ok!\r\n");
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
			//SPI_DEBUG(LOG_INFO, "spi ch open err:using!\r\n");
			node = NULL;
		}
		else
		{
			/*��SPI������*/
			if(node->spi->dev.type == DEV_SPI_H)
			{
				res = mcu_hspi_open(node->spi, mode, pre);	
			}
			else if(node->spi->dev.type == DEV_SPI_V)
			{
				res = mcu_vspi_open(node->spi, mode, pre);	
			}

			if(res == 0)
			{
				node->gd = 0;
				//SPI_DEBUG(LOG_INFO, "spi dev open ok: %s!\r\n", name);
				mcu_io_output_resetbit(node->dev.csport, node->dev.cspin);
			}
			else
			{
				//SPI_DEBUG(LOG_INFO, "spi dev open err!\r\n");
				node = NULL;
			}
		}
	}
	else
	{
		SPI_DEBUG(LOG_INFO, ">-------spi ch no exist!\r\n");	
	}
	
	return node;
}


/**
 *@brief:      mcu_spi_close
 *@details:    �ر�SPI ͨ��
 *@param[in]   DevSpiChNode * node  
 *@param[out]  ��
 *@retval:     
 */
s32 mcu_spi_close(DevSpiChNode * node)
{
	if(node->spi->dev.type == DEV_SPI_H)
	{
		mcu_hspi_close(node->spi);
	}
	else
		mcu_vspi_close(node->spi);
	
	/*����CS*/
	mcu_io_output_setbit(node->dev.csport, node->dev.cspin);
	node->gd = -1;
 
	return 0;
}
/**
 *@brief:      mcu_spi_transfer
 *@details:    SPI ����
 *@param[in]   DevSpiChNode * node
 			   u8 *snd  
               u8 *rsv  
               s32 len  
 *@param[out]  ��
 *@retval:     
 */
s32 mcu_spi_transfer(DevSpiChNode * node, u8 *snd, u8 *rsv, s32 len)
{
	if(node == NULL)
		return -1;

	if(node->spi->dev.type == DEV_SPI_H)
		return mcu_hspi_transfer(node->spi, snd, rsv, len);
	else if(node->spi->dev.type == DEV_SPI_V)	
		return mcu_vspi_transfer(node->spi, snd, rsv, len);
	else
	{
		SPI_DEBUG(LOG_DEBUG, "spi dev type err\r\n");	
	}
}
/**
 *@brief:      mcu_spi_cs
 *@details:    �ٿض�ӦSPI��CS
 *@param[in]   DevSpiChNode * node  
               u8 sta   1 �ߵ�ƽ��0 �͵�ƽ     
 *@param[out]  ��
 *@retval:     
 */
s32 mcu_spi_cs(DevSpiChNode * node, u8 sta)
{
	switch(sta)
	{
		case 1:
			mcu_io_output_setbit(node->dev.csport, node->dev.cspin);
			break;
			
		case 0:
			mcu_io_output_resetbit(node->dev.csport, node->dev.cspin);
			break;
			
		default:
			return -1;

	}

	return 0;
}

#if 0

void spi_example(void)
{
	DevSpiChNode *spichnode;
	u8 src[16];
	u8 rsv[16];
	
	/*��SPIͨ��*/
	spichnode = mcu_spi_open("VSPI1_CH1", SPI_MODE_1, 4);
	if(spichnode == NULL)
	{
		while(1);
	}
	/*��10������*/
	mcu_spi_transfer(spichnode, NULL, rsv, 10);
	/*д10������*/
	mcu_spi_transfer(spichnode, src, NULL, 10);
	/*��д10������*/
	mcu_spi_transfer(spichnode, src, rsv, 10);

	mcu_spi_close(spichnode);
}

#endif

