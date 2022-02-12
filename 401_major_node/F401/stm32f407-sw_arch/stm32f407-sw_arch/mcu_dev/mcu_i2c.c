/**
 * @file            mcu_i2c.c
 * @brief           IOģ��I2C
 * @author          test
 * @date            2017��10��30�� ����һ
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:      2017��10��30�� ����һ
 *   ��    ��:      �ݼ�ȸ������
 *   �޸�����:      �����ļ�
    	1 Դ����ݼ�ȸ���������С�
		2 �������ڵ�������ҵ��;�����׿��������۳��⣩��������Ȩ��
		3 �ݼ�ȸ�����Ҳ��Դ��빦�����κα�֤����ʹ�������в��ԣ�����Ը���
		4 �������޸�Դ�벢�ַ���������ֱ�����۱���������������뱣��WUJIQUE��Ȩ˵����
		5 �緢��BUG�����Ż�����ӭ�������¡�����ϵ��code@wujique.com
		6 ʹ�ñ�Դ�����൱����ͬ����Ȩ˵����
		7 ���ַ����Ȩ��������ϵ��code@wujique.com
		8 һ�н���Ȩ���ݼ�ȸ���������С�
*/

#include "stm32f4xx.h"
#include "wujique_log.h"
#include "list.h"
#include "mcu_i2c.h"
#include "alloc.h"

#define MCU_I2C_DEBUG

#ifdef MCU_I2C_DEBUG
#define I2C_DEBUG	wjq_log 
#else
#define I2C_DEBUG(a, ...)
#endif


#define MCU_I2C_TIMEOUT 250


/**
 *@brief:      mcu_i2c_delay
 *@details:    I2C�ź���ʱ����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     static
 */
static void mcu_i2c_delay(void)
{
    //Delay(1);//��ʱ��I2Cʱ��
    volatile u32 i = 5;

    for(;i>0;i--);
}

/**
 *@brief:      mcu_i2c_sda_input
 *@details:    ��I2C sda IO����Ϊ����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
void mcu_i2c_sda_input(DevI2c *dev)
{
	mcu_io_config_in(dev->sdaport, dev->sdapin);
}
/**
 *@brief:      mcu_i2c_sda_output
 *@details:       ��I2C sda IO����Ϊ���
 *@param[in]  void  
 *@param[out]  ��
 *@retval:     
 */
void mcu_i2c_sda_output(DevI2c *dev)
{
	mcu_io_config_out(dev->sdaport, dev->sdapin);
}
/**
 *@brief:      mcu_i2c_readsda
 *@details:    ��SDA����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     static
 */
static s32 mcu_i2c_readsda(DevI2c *dev)
{

    if(Bit_SET == mcu_io_input_readbit(dev->sdaport, dev->sdapin))
        return 1;
    else
        return 0;
}
/**
 *@brief:      mcu_i2c_sda
 *@details:       SDA����ߵ͵�ƽ
 *@param[in]  u8 sta  
 *@param[out]  ��
 *@retval:     static
 */
static void mcu_i2c_sda(DevI2c *dev, u8 sta)
{

    if(sta == 1)
    {
		mcu_io_output_setbit(dev->sdaport, dev->sdapin);
    }
    else if(sta == 0)
    {
    	mcu_io_output_resetbit(dev->sdaport, dev->sdapin);
    }

}

/**
 *@brief:      mcu_i2c_scl
 *@details:    ʱ��SCL����ߵ͵�ƽ
 *@param[in]   u8 sta  
 *@param[out]  ��
 *@retval:     static
 */
static void mcu_i2c_scl(DevI2c *dev, u8 sta)
{

	if(sta == 1)
    {
		mcu_io_output_setbit(dev->sclport, dev->sclpin);
    }
    else if(sta == 0)
    {
    	mcu_io_output_resetbit(dev->sclport, dev->sclpin);
    }
}
/**
 *@brief:      mcu_i2c_start
 *@details:    ����startʱ��
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     static
 */
static void mcu_i2c_start(DevI2c *dev)
{
    mcu_i2c_sda_output(dev);
    
    mcu_i2c_sda(dev, 1);  
    mcu_i2c_scl(dev, 1);

    mcu_i2c_delay();
    mcu_i2c_sda(dev, 0);

    mcu_i2c_delay();
    mcu_i2c_scl(dev, 0);
}
/**
 *@brief:      mcu_i2c_stop
 *@details:    ����I2C STOPʱ��
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     static
 */
static void mcu_i2c_stop(DevI2c *dev)
{
    mcu_i2c_sda_output(dev);

    mcu_i2c_scl(dev, 0);
    mcu_i2c_sda(dev, 0);   
    mcu_i2c_delay();
    
    mcu_i2c_scl(dev, 1);
    mcu_i2c_delay();
    
    mcu_i2c_sda(dev, 1);
    mcu_i2c_delay();
}

/**
 *@brief:      mcu_i2c_wait_ack
 *@details:       �ȴ�ACK�ź�
 *@param[in]  void  
 *@param[out]  ��
 *@retval:     static
 */
static s32 mcu_i2c_wait_ack(DevI2c *dev)
{
    u8 time_out = 0;
    
    //sdaת����
    mcu_i2c_sda_input(dev);
    mcu_i2c_sda(dev, 1);
    mcu_i2c_delay();
    
    mcu_i2c_scl(dev, 1);
    mcu_i2c_delay();
    
    while(1)
    {
        time_out++;
        if(time_out > MCU_I2C_TIMEOUT)
        {
            mcu_i2c_stop(dev);
            //wjq_log(LOG_ERR, "i2c:wait ack time out!\r\n");
            return 1;
        }

        if(0 == mcu_i2c_readsda(dev))
        {   
            break;
        }
    }
    
    mcu_i2c_scl(dev, 0);
    
    return 0;
}
/**
 *@brief:      mcu_i2c_ack
 *@details:       ����ACK�ź�
 *@param[in]  void  
 *@param[out]  ��
 *@retval:     static
 */
static void mcu_i2c_ack(DevI2c *dev)
{
    mcu_i2c_scl(dev, 0);
    mcu_i2c_sda_output(dev);
    
    mcu_i2c_sda(dev, 0);
    mcu_i2c_delay();
    
    mcu_i2c_scl(dev, 1);
    mcu_i2c_delay();
    
    mcu_i2c_scl(dev, 0);
}
/**
 *@brief:      mcu_i2c_writebyte
 *@details:       I2C����дһ���ֽ�����
 *@param[in]  u8 data  
 *@param[out]  ��
 *@retval:     static
 */
static s32 mcu_i2c_writebyte(DevI2c *dev, u8 data)
{
    u8 i = 0;

    mcu_i2c_sda_output(dev);

    mcu_i2c_scl(dev, 0);
    
    while(i<8)
    {
    
        if((0x80 & data) == 0x80)
        {
            mcu_i2c_sda(dev, 1);   
        }
        else
        {
            mcu_i2c_sda(dev, 0);
        }
        
        mcu_i2c_delay();

        mcu_i2c_scl(dev, 1);
        mcu_i2c_delay();
        
        mcu_i2c_scl(dev, 0);
        mcu_i2c_delay();
        
        data = data <<1;
        i++;
    }
		return 0;
}
/**
 *@brief:      mcu_i2c_readbyte
 *@details:       I2C���߶�һ���ֽ�����
 *@param[in]  void  
 *@param[out]  ��
 *@retval:     static
 */
static u8 mcu_i2c_readbyte(DevI2c *dev)
{
    u8 i = 0;
    u8 data = 0;

    mcu_i2c_sda_input(dev);
    
    while(i<8)
    {
        mcu_i2c_scl(dev, 0);
        mcu_i2c_delay();
        
        mcu_i2c_scl(dev, 1);

        data = (data <<1);

        if(1 == mcu_i2c_readsda(dev))
        {
            data =data|0x01;    
        }
        else
        {
            data = data & 0xfe;
        }

        mcu_i2c_delay();
        
        i++;
    }

    return data;
}

/**
 *@brief:      mcu_i2c_transfer
 *@details:    �м������¿�ʼλ�Ĵ�������
 *@param[in]   DevI2cNode * node  I2C�ڵ�
 			   u8 addr   7λ��ַ
               u8 rw    0 д��1 ��    
               u8* data  
               s32 datalen �������ݳ���
 *@param[out]  ��
 *@retval:     
 */
s32 mcu_i2c_transfer(DevI2cNode *node, u8 addr, u8 rw, u8* data, s32 datalen)
{
    s32 i;
    u8 ch;
	DevI2c *dev;
	s32 res;
	
    #if 0//����IO���Ƿ���ͨ
    while(1)
    {
        uart_printf("test \r\n");
        mcu_i2c_scl(1);
        mcu_i2c_sda(1); 
        Delay(5);
        mcu_i2c_scl(0);
        mcu_i2c_sda(0); 
        Delay(5);
    }   
    #endif

	if(node == NULL)
		return -1;

	dev = &node->dev;
	
	//I2C_DEBUG(LOG_DEBUG, "i2c trf %s\r\n", dev->name);
    //������ʼ
    mcu_i2c_start(dev);
    //���͵�ַ+��д��־
    //����ADDR
    if(rw == MCU_I2C_MODE_W)
    {
        addr = ((addr<<1)&0xfe);
        //uart_printf("write\r\n");
    }
    else
    {
        addr = ((addr<<1)|0x01);
        //uart_printf("read\r\n");
    }
    
    mcu_i2c_writebyte(dev, addr);
	
    res = mcu_i2c_wait_ack(dev);
	if(res == 1)
		return 1;
	
    i = 0;

    //���ݴ���
    if(rw == MCU_I2C_MODE_W)//д
    {
	    while(i < datalen)
	 	{
            ch = *(data+i);
            mcu_i2c_writebyte(dev, ch);
            res = mcu_i2c_wait_ack(dev);
			if(res == 1)
				return 1;
			
			i++;
	    }
    }
    else if(rw == MCU_I2C_MODE_R)//��
    {
       	while(i < datalen)
	 	{
            ch = mcu_i2c_readbyte(dev);  
            mcu_i2c_ack(dev);
            *(data+i) = ch;
			i++;
	    }
    }

    //���ͽ���
    mcu_i2c_stop(dev);
    return 0;
}


/*

	I2Cģ��ά��һ��������¼�е�ǰ��ʼ����I2C������

*/
struct list_head DevI2cGdRoot = {&DevI2cGdRoot, &DevI2cGdRoot};


/**
 *@brief:      mcu_i2c_register
 *@details:    ��ʼ��I2C�ӿڣ� �൱��ע��һ��I2C�豸
 *@param[in]   const DevI2c * dev I2C�豸��Ϣ
 *@param[out]  ��
 *@retval:     
 */
s32 mcu_i2c_register(const DevI2c * dev)
{
    
	struct list_head *listp;
	DevI2cNode *p;
	
	wjq_log(LOG_INFO, "[register] i2c:%s!\r\n", dev->name);

	/*
		��Ҫ��ѯ��ǰI2C����������ֹ����
	*/
	listp = DevI2cGdRoot.next;
	while(1)
	{
		if(listp == &DevI2cGdRoot)
			break;

		p = list_entry(listp, DevI2cNode, list);
		//wjq_log(LOG_INFO, "i2c dev name:%s!\r\n", p->dev.name);
		
		if(strcmp(dev->name, p->dev.name) == 0)
		{
			wjq_log(LOG_INFO, "i2c dev name err!\r\n");
			return -1;
		}
		
		listp = listp->next;
	}

	/* 
		����һ���ڵ�ռ� 
		
	*/
	p = (DevI2cNode *)wjq_malloc(sizeof(DevI2cNode));
	list_add(&(p->list), &DevI2cGdRoot);
	
	/*
		��ʼ���豸�ڵ�
	*/
	memcpy((u8 *)&p->dev, (u8 *)dev, sizeof(DevI2c));
	p->gd = -1;

	/*��ʼ��IO��״̬*/
	mcu_io_config_out(dev->sclport, dev->sclpin);
	mcu_io_config_out(dev->sdaport, dev->sdapin);

	mcu_io_output_setbit(dev->sdaport, dev->sdapin);
	mcu_io_output_setbit(dev->sclport, dev->sclpin); 

	return 0;
}
/**
 *@brief:      mcu_i2c_open
 *@details:    �������ִ�һ��i2c�ӿ�
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     �����豸�ڵ�
 */
DevI2cNode *mcu_i2c_open(char *name)
{

	DevI2cNode *node;
	struct list_head *listp;
	
	//I2C_DEBUG(LOG_INFO, "i2c open:%s!\r\n", name);

	listp = DevI2cGdRoot.next;
	node = NULL;
	
	while(1)
	{
		if(listp == &DevI2cGdRoot)
			break;

		node = list_entry(listp, DevI2cNode, list);
		//I2C_DEBUG(LOG_INFO, "i2c dev name:%s!\r\n", node->dev.name);
		
		if(strcmp(name, node->dev.name) == 0)
		{
			//I2C_DEBUG(LOG_INFO, "i2c dev open ok!\r\n");
			break;
		}
		
		listp = listp->next;
	}

	if(node != NULL)
	{
		if(node->gd == 0)
		{
			node = NULL;
		}
		else
		{
			node->gd = 0;
		}
	}
	return node;
}
/**
 *@brief:      mcu_i2c_close
 *@details:    �ر�I2C�ڵ�
 *@param[in]   DevI2cNode *node 
 *@param[out]  ��
 *@retval:     -1 �ر�ʧ�ܣ�0 �رճɹ�
 */
s32 mcu_i2c_close(DevI2cNode *node)
{
	if(node == NULL)
		return -1;

	if(node->gd != 0)
		return -1;

	node->gd = -1; 

	return 0;
}


#if 0

void mcu_i2c_example(void)
{
	DevI2cNode *node;
	
	node = mcu_i2c_open("VI2C1");
	if(node == NULL)
	{
		wjq_log(LOG_DEBUG, "open VI2C1 err!\r\n");
		while(1);
	}
	
	u8 data[16];
	mcu_i2c_transfer(node, 0x70, MCU_I2C_MODE_W, data, 8);
	mcu_i2c_transfer(node, 0x70, MCU_I2C_MODE_R, data, 8);

	mcu_i2c_close(node);
}

#endif



/*
	����ͷʹ��SCCB�ӿڣ���ʵ����I2C
	��ST�ٷ�����ͷ��ֲ��ʹ�õ���Ӳ��I2C
*/
#define CAREMA_SCCB_I2C 	I2C2 //ʹ��I2C2������
#define SCCB_TIMEOUT_MAX    10000
	
void SCCB_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef  I2C_InitStruct;	  
	/****** Configures the I2C1 used for OV9655 camera module configuration *****/
	/* I2C1 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	
	/* GPIOB clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); 
	
	/* Connect I2C2 pins to AF4 */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource0, GPIO_AF_I2C2);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource1, GPIO_AF_I2C2);
	
	/* Configure I2C2 GPIOs */	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	/* Configure I2C2 */
	/* I2C DeInit */
	I2C_DeInit(CAREMA_SCCB_I2C);
	  
	///* Enable the I2C peripheral */
	//I2C_Cmd(CAREMA_SCCB_I2C, ENABLE);
	
	/* Set the I2C structure parameters */
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_OwnAddress1 = 0xFE;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_ClockSpeed = 10000;
	
	/* Initialize the I2C peripheral w/ selected parameters */
	I2C_Init(CAREMA_SCCB_I2C, &I2C_InitStruct);
	
	I2C_Cmd(CAREMA_SCCB_I2C, ENABLE);
	
	I2C_AcknowledgeConfig(CAREMA_SCCB_I2C, ENABLE);

}
	
uint8_t bus_sccb_writereg(uint8_t DeviceAddr, uint16_t Addr, uint8_t Data)
{
	
	uint32_t timeout = SCCB_TIMEOUT_MAX;
	
	/* Generate the Start Condition */
	I2C_GenerateSTART(CAREMA_SCCB_I2C, ENABLE);
	
	/* Test on I2C1 EV5 and clear it */
	timeout = SCCB_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(CAREMA_SCCB_I2C, I2C_EVENT_MASTER_MODE_SELECT))
	{
	  /* If the timeout delay is exceeded, exit with error code */
	  if ((timeout--) == 0) return 0xFF;
	}
	 
	/* Send DCMI selected device slave Address for write */
	I2C_Send7bitAddress(CAREMA_SCCB_I2C, DeviceAddr, I2C_Direction_Transmitter);
	
	/* Test on I2C1 EV6 and clear it */
	timeout = SCCB_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(CAREMA_SCCB_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
	  /* If the timeout delay is exceeded, exit with error code */
	  if ((timeout--) == 0) return 0xFF;
	}
	
	/* Send I2C1 location address LSB */
	I2C_SendData(CAREMA_SCCB_I2C, (uint8_t)(Addr));
	
	/* Test on I2C1 EV8 and clear it */
	timeout = SCCB_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(CAREMA_SCCB_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
	  /* If the timeout delay is exceeded, exit with error code */
	  if ((timeout--) == 0) return 0xFF;
	}
	
	/* Send Data */
	I2C_SendData(CAREMA_SCCB_I2C, Data);
	
	/* Test on I2C1 EV8 and clear it */
	timeout = SCCB_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(CAREMA_SCCB_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
	  /* If the timeout delay is exceeded, exit with error code */
	  if ((timeout--) == 0) return 0xFF;
	}  
	
	/* Send I2C1 STOP Condition */
	I2C_GenerateSTOP(CAREMA_SCCB_I2C, ENABLE);
	
	/* If operation is OK, return 0 */
	return 0;

}
	
uint8_t bus_sccb_readreg(uint8_t DeviceAddr, uint16_t Addr)
{
  uint32_t timeout = SCCB_TIMEOUT_MAX;
  uint8_t Data = 0;

  /* Generate the Start Condition */
  I2C_GenerateSTART(CAREMA_SCCB_I2C, ENABLE);

  /* Test on I2C1 EV5 and clear it */
  timeout = SCCB_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAREMA_SCCB_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
	/* If the timeout delay is exceeded, exit with error code */
	if ((timeout--) == 0) return 0xFF;
  }
  
  /* Send DCMI selected device slave Address for write */
  I2C_Send7bitAddress(CAREMA_SCCB_I2C, DeviceAddr, I2C_Direction_Transmitter);
 
  /* Test on I2C1 EV6 and clear it */
  timeout = SCCB_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAREMA_SCCB_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
	/* If the timeout delay is exceeded, exit with error code */
	if ((timeout--) == 0) return 0xFF;
  }

  /* Send I2C1 location address LSB */
  I2C_SendData(CAREMA_SCCB_I2C, (uint8_t)(Addr));

  /* Test on I2C1 EV8 and clear it */
  timeout = SCCB_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAREMA_SCCB_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
	/* If the timeout delay is exceeded, exit with error code */
	if ((timeout--) == 0) return 0xFF;
  } 
  
  /* Clear AF flag if arised */
  CAREMA_SCCB_I2C->SR1 |= (uint16_t)0x0400;

  /* Generate the Start Condition */
  I2C_GenerateSTART(CAREMA_SCCB_I2C, ENABLE);
  
  /* Test on I2C1 EV6 and clear it */
  timeout = SCCB_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAREMA_SCCB_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
	/* If the timeout delay is exceeded, exit with error code */
	if ((timeout--) == 0) return 0xFF;
  } 
  
  /* Send DCMI selected device slave Address for write */
  I2C_Send7bitAddress(CAREMA_SCCB_I2C, DeviceAddr, I2C_Direction_Receiver);

  /* Test on I2C1 EV6 and clear it */
  timeout = SCCB_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAREMA_SCCB_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
  {
	/* If the timeout delay is exceeded, exit with error code */
	if ((timeout--) == 0) return 0xFF;
  }
 
  /* Prepare an NACK for the next data received */
  I2C_AcknowledgeConfig(CAREMA_SCCB_I2C, DISABLE);

  /* Test on I2C1 EV7 and clear it */
  timeout = SCCB_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAREMA_SCCB_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))
  {
	/* If the timeout delay is exceeded, exit with error code */
	if ((timeout--) == 0) return 0xFF;
  }

  /* Prepare Stop after receiving data */
  I2C_GenerateSTOP(CAREMA_SCCB_I2C, ENABLE);

  /* Receive the Data */
  Data = I2C_ReceiveData(CAREMA_SCCB_I2C);

  /* return the read data */
  return Data;
}
	

