/**
 * @file            dev_rf24l01.c
 * @brief           RF24L01����
 * @author          wujique
 * @date            2018��3��12�� ����һ
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2018��3��12�� ����һ
 *   ��    ��:         wujique
 *   �޸�����:   �����ļ�
*/
#include "stm32f4xx.h"
#include "mcu_spi.h"
#include "wujique_log.h"
#include "wujique_sysconf.h"

/*
	�ӷ���
	RF24L01����������SPI3�ϣ�Ҳ���Խӵ������IO��ģ��SPI
	�ӿڸ�OLEDһ��
	�ӿڣ�
	��SPI���⣬����IRQ��CE�ܽ�
	IRQ���͵�ƽ�жϣ������ж�ԭ����Ҫ��ѯ�Ĵ���
	CE������RF�Ƿ������ߵ�ƽ���������ͽ��ն���Ҫ���ߣ�����оƬʱ���͹ص�RF��
	���ܣ�
	0 ֻ��һ������ͨ��
	1 ��6������ͨ����6��ͨ��Ƶ��һ������ַ�������ò�ͬ��ͨ��0/ͨ��1��ַ40BIT������ͨ��ǰ32BIT��ͨ��1��ͬ����8bit��һ����
	2 ����ģʽ��ͨ��0����ȷ��Ӧ�����ͨ��0��ַҪ���ø�����Ŀ�ĵ�ַһ����
	3 оƬ���Զ�Ӧ���ط����Զ�Ӧ�����ʱ���ط������ǿɱ�̵ġ�
	4 ÿ�ο�ʼSPIͨ�ţ�оƬ���᷵��״̬��

	��������NRF24L01��ʹ�ã�
	1 ����ͨ��0Ĭ����ΪACK����ͨ�������ṩ��APPʹ�á�
	2 �ṩ1-5ͨ����Ϊ����ͨ����APP�����õ�ַ��
	3 �ṩ��д�ӿڸ�APP�����ӿڶ����ǻ��壬д��ֱ�Ӳ������͡�
*/

#define RF24L01_SPI "SPI3_CH3"

#define RF24L01_CE_PORT	GPIOG
#define RF24L01_CE_PIN	GPIO_Pin_7
#define RF24L01_IRQ_PORT GPIO_Pin_4
#define RF24L01_IRQ_PIN GPIOG


#define NRF_READ_REG 0x00 	//�����üĴ���,��5λΪ�Ĵ�����ַ
#define NRF_WRITE_REG 0x20 	//д���üĴ���,��5λΪ�Ĵ�����ַ
#define NRF_RD_RX_PLOAD 0x61 //��RX��Ч����,1~32�ֽ�
#define NRF_WR_TX_PLOAD 0xA0 //дTX��Ч����,1~32�ֽ�
#define NRF_FLUSH_TX 0xE1 	//���TX FIFO�Ĵ���.����ģʽ����
#define NRF_FLUSH_RX 0xE2 	//���RX FIFO�Ĵ���.����ģʽ����
#define NRF_REUSE_TX_PL 0xE3 //����ʹ����һ������,CEΪ��,���ݰ������Ϸ���.
#define NRF_NOP 0xFF //�ղ���,����������״̬�Ĵ���
/*
	�Ĵ���
*/
#define NRF_REG_CONFIG 0x00 //���üĴ�����ַ
#define NRF_REG_EN_AA 0x01 //ʹ���Զ�Ӧ����
#define NRF_REG_EN_RXADDR 0x02 //���յ�ַ����
#define NRF_REG_SETUP_AW 0x03 //���õ�ַ���(��������ͨ��)
#define NRF_REG_SETUP_RETR 0x04 //�����Զ��ط�
#define NRF_REG_RF_CH 0x05 //RFͨ��, ����ͨ��Ƶ��
#define NRF_REG_RF_SETUP 0x06 //RF�Ĵ���
#define NRF_REG_STATUS 0x07 //״̬�Ĵ���
#define NRF_REG_OBSERVE_TX 0x08 // ���ͼ��Ĵ���
#define NRF_REG_CD 0x09 // �ز����Ĵ���

#define NRF_REG_RX_ADDR_P0 0x0A // ����ͨ��0���յ�ַ 5�ֽ�
#define NRF_REG_RX_ADDR_P1 0x0B // ����ͨ��1���յ�ַ 5�ֽ�
#define NRF_REG_RX_ADDR_P2 0x0C // ����ͨ��2���յ�ַ 1�ֽڣ������ֽ���P1һ��
#define NRF_REG_RX_ADDR_P3 0x0D // ����ͨ��3���յ�ַ 1�ֽڣ������ֽ���P1һ��
#define NRF_REG_RX_ADDR_P4 0x0E // ����ͨ��4���յ�ַ 1�ֽڣ������ֽ���P1һ��
#define NRF_REG_RX_ADDR_P5 0x0F // ����ͨ��5���յ�ַ 1�ֽڣ������ֽ���P1һ��

#define NRF_REG_TX_ADDR 0x10 // ���͵�ַ�Ĵ���
/*
   ͨ����ѯ��5���Ĵ���������֪�����ν��յ���������
*/
#define NRF_REG_RX_PW_P0 0x11 // ��������ͨ��0��Ч���ݿ��(1~32�ֽ�)
#define NRF_REG_RX_PW_P1 0x12 // ��������ͨ��1��Ч���ݿ��(1~32�ֽ�)
#define NRF_REG_RX_PW_P2 0x13 // ��������ͨ��2��Ч���ݿ��(1~32�ֽ�)
#define NRF_REG_RX_PW_P3 0x14 // ��������ͨ��3��Ч���ݿ��(1~32�ֽ�)
#define NRF_REG_RX_PW_P4 0x15 // ��������ͨ��4��Ч���ݿ��(1~32�ֽ�)
#define NRF_REG_RX_PW_P5 0x16 // ��������ͨ��5��Ч���ݿ��(1~32�ֽ�)

#define NRF_REG_FIFO_STATUS 0x17 // FIFO״̬�Ĵ���

/*
	״̬��־λ
*/
#define NRF_STA_BIT_TX_FULL 0x01//���ͻ�����
#define NRF_STA_BIT_RX_P_NO 0x0e//�յ�����ͨ�� 000-101: Data Pipe Number��110: Not Used��111: RX FIFO Empty
#define NRF_STA_BIT_MAX_RT  0x10//�ﵽ����ط���������д1�壬�����޷�����ͨ��
#define NRF_STA_BIT_TX_DS   0x20//���ͳɹ������ʹ���Զ�Ӧ���յ�Ӧ����ɹ�
#define NRF_STA_BIT_RX_DR   0x40//�յ�����

#define TX_PLOAD_WIDTH 32//ÿ����෢��32���ֽ�
#define RX_PLOAD_WIDTH 32
#define CHANAL	40

#define TX_ADR_WIDTH 5

#define RX_ADR_WIDTH 5

u8 TX_ADDRESS[TX_ADR_WIDTH] = {0x02,0x02,0x02,0x02,0x02};

u8 RX_ADDRESS_0[RX_ADR_WIDTH] = {0x02,0x02,0x02,0x02,0x02};

u8 RX_ADDRESS_1[RX_ADR_WIDTH] = {0x02,0xc2,0xc2,0xc2,0xc2};
u8 RX_ADDRESS_2[1] = {0x03};
u8 RX_ADDRESS_3[1] = {0x02};
u8 RX_ADDRESS_4[1] = {0x02};
u8 RX_ADDRESS_5[1] = {0x02};


/**
 *@brief:      dev_nrf24l01_init
 *@details:    ��ʼ��NRF24
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_nrf24l01_init(void)
{
    
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
 
	/* ce */
	GPIO_InitStructure.GPIO_Pin = RF24L01_CE_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(RF24L01_CE_PORT, &GPIO_InitStructure);

	/* IRQ */
	GPIO_InitStructure.GPIO_Pin = RF24L01_IRQ_PORT;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(RF24L01_IRQ_PIN, &GPIO_InitStructure);

	GPIO_ResetBits(RF24L01_CE_PORT, RF24L01_CE_PIN);
	return 0;
}

/**
 *@brief:      dev_nrf24l01_setce
 *@details:    ����CE��ƽ���ߵ�ƽRF����
 *@param[in]   u8 sta  
 *@param[out]  ��
 *@retval:     static
 */
static s32 dev_nrf24l01_setce(u8 sta)
{
	if(sta == 1)
	{
		GPIO_SetBits(RF24L01_CE_PORT, RF24L01_CE_PIN);	
	}
	else
	{
		GPIO_ResetBits(RF24L01_CE_PORT, RF24L01_CE_PIN);	
	}
	
	return 0;
}
/**
 *@brief:      dev_nrf24l01_irqsta
 *@details:    ��IRQ�ܽ��ж�״̬
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     static
 */
static s32 dev_nrf24l01_irqsta(void)
{
	u8 sta;
	sta = GPIO_ReadInputDataBit(RF24L01_IRQ_PIN, RF24L01_IRQ_PORT);	
	if(sta == Bit_SET)
		return 1;
	else
		return 0;
}


static u8 dev_nrf24l01_write_reg(u8 reg,u8 data)
{
	u8 cmd[2];
	s32 res;
	
	cmd[0] = reg;
	cmd[1] = data;
	
	//mcu_spi_cs(RF24L01_SPI, 0);
	//res = mcu_spi_transfer(RF24L01_SPI, &cmd[0], NULL, 2);
	//mcu_spi_cs(RF24L01_SPI, 1);
	
    return 0;
}

static s32 dev_nrf24l01_read_reg(u8 reg, u8 *data)
{
  	s32 res;

	
	//mcu_spi_cs(RF24L01_SPI, 0);
    //res = mcu_spi_transfer(RF24L01_SPI, &reg, NULL, 1);
    if(res != -1)
    {
	//	res = mcu_spi_transfer(RF24L01_SPI, NULL, data, 1);

    }
	//mcu_spi_cs(RF24L01_SPI, 1);
	
    return res;
}

static s32 dev_nrf24l01_write_buf(u8 reg, u8 *pBuf, u8 len)
{
	s32 res;
	u8 tmp = reg;

	//mcu_spi_cs(RF24L01_SPI, 0);
	//res = mcu_spi_transfer(RF24L01_SPI, &tmp, NULL, 1);
    if(res != -1)
    {
	//	res = mcu_spi_transfer(RF24L01_SPI, pBuf, NULL, len);
    }
	
	//mcu_spi_cs(RF24L01_SPI, 1);
	return res;
}

static u8 dev_nrf24l01_read_buf(u8 reg,u8 *pBuf, u8 len)
{

	s32 res;
	u8 tmp;
	tmp = reg;

	//mcu_spi_cs(RF24L01_SPI, 0);
    //res = mcu_spi_transfer(RF24L01_SPI, &tmp, NULL, 1);
    if(res != -1)
    {
    //	res = mcu_spi_transfer(RF24L01_SPI, NULL, pBuf, len);
    }
	//mcu_spi_cs(RF24L01_SPI, 1);
	
    return 0;   
}

static s32 dev_nrf24l01_openspi(void)
{
	 s32 res;

	//res = mcu_spi_open(RF24L01_SPI, SPI_MODE_0, SPI_BaudRatePrescaler_32);
	if(res == -1)
	{
		wjq_log(LOG_FUN, "rf open spi err\r\n");
		return -1;
	}	
	
	return 0;
}

static s32 dev_nrf24l01_closespi(void)
{
	//mcu_spi_close(RF24L01_SPI);	
	return 0;
}
/**
 *@brief:      NRF_Tx_Dat
 *@details:       �������ݣ�����ģʽ
 *@param[in]  u8 *txbuf  
               u16 len    
 *@param[out]  ��
 *@retval:     
 */
u8 NRF_Tx_Dat(u8 *txbuf, u16 len)  
{  
    u8 state;    

	
	wjq_log(LOG_FUN, "%s\r\n", __FUNCTION__);

	dev_nrf24l01_openspi();

	/*ceΪ�ͣ��������ģʽ1*/
	 dev_nrf24l01_setce(0);

	/*д���ݵ�TX BUF ��� 32���ֽ�*/ 
	dev_nrf24l01_write_buf(NRF_WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH); 

		 /*CEΪ�ߣ�txb�ǿգ��������ݰ� */
	    dev_nrf24l01_setce(1);
		 
		 /*�ȴ���������ж� */ 
	    while(0 != dev_nrf24l01_irqsta()); 
		
		wjq_log(LOG_FUN, "NRF send data ok\r\n");
	    dev_nrf24l01_read_reg(NRF_REG_STATUS, &state); /*��ȡ״̬�Ĵ�����ֵ */   
	    dev_nrf24l01_write_reg(NRF_WRITE_REG+NRF_REG_STATUS, state); /*���TX_DS��MAX_RT�жϱ�־*/      
	    dev_nrf24l01_write_reg(NRF_FLUSH_TX, NRF_NOP);    //���TX FIFO�Ĵ���   

	wjq_log(LOG_FUN, "NRF send data finish\r\n");
	
	dev_nrf24l01_closespi();
	/*�ж��ж�����*/      
    if(state&NRF_STA_BIT_MAX_RT)   //�ﵽ����ط�����  
        return NRF_STA_BIT_MAX_RT;   
  
    else if(state&NRF_STA_BIT_TX_DS)//�������  
        return NRF_STA_BIT_TX_DS;  
    else  
        return ERROR;                 //����ԭ����ʧ��  
}   

s32 NRF_Rx_Dat(u8 *rxb)  
{  
    u8 state;  
	s32 res;
	
	dev_nrf24l01_openspi();
    dev_nrf24l01_setce(1);   //�������״̬  
     /*�ȴ������ж�*/  
    while(dev_nrf24l01_irqsta()!=0);  
	
    dev_nrf24l01_setce(0);    //�������״̬                  
    dev_nrf24l01_read_reg(NRF_REG_STATUS, &state);    /*��ȡstatus�Ĵ�����ֵ*/    
    if(state&NRF_STA_BIT_RX_DR)  /*�ж��Ƿ���յ�����*/  //���յ�����  
    {
    	wjq_log(LOG_FUN, "nrf receive data\r\n");
		
        dev_nrf24l01_write_reg(NRF_WRITE_REG+NRF_REG_STATUS,state);/* ����жϱ�־*/   
        dev_nrf24l01_read_buf(NRF_RD_RX_PLOAD, rxb, RX_PLOAD_WIDTH);//��ȡ����  
        
        PrintFormat(rxb, RX_PLOAD_WIDTH);
		
//      SPI_NRF_WriteReg(FLUSH_RX,NOP);          //���RX FIFO�Ĵ���  
        res = NRF_STA_BIT_RX_DR;   
    }else      
        res =  ERROR;                    //û�յ��κ�����  

	dev_nrf24l01_closespi();

	return res;
}  

extern void Delay(__IO uint32_t nTime);


void NRF_TX_Mode(u8 *addr, u8 rfch)  
{    
   dev_nrf24l01_openspi();
	
   dev_nrf24l01_setce(0);  
   /*дTX�ڵ��ַ;  */
   dev_nrf24l01_write_buf(NRF_WRITE_REG+NRF_REG_TX_ADDR, addr, TX_ADR_WIDTH); 
   /*//����TX�ڵ�0��ַ, �����͵�ַһ������ҪΪ��ʹ��ACK;   */
   dev_nrf24l01_write_buf(NRF_WRITE_REG+NRF_REG_RX_ADDR_P0, addr, RX_ADR_WIDTH);
   
   dev_nrf24l01_write_reg(NRF_WRITE_REG+NRF_REG_EN_AA,0x01);     //ʹ��ͨ��0���Զ�Ӧ��;      
   dev_nrf24l01_write_reg(NRF_WRITE_REG+NRF_REG_EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ;    
   /*//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��;  */
   dev_nrf24l01_write_reg(NRF_WRITE_REG+NRF_REG_SETUP_RETR,0x1a);
   /**/
   dev_nrf24l01_write_reg(NRF_WRITE_REG+NRF_REG_RF_CH,  rfch);   //����RFͨ��ΪCHANAL;  
   dev_nrf24l01_write_reg(NRF_WRITE_REG+NRF_REG_RF_SETUP, 0x07);  //����TX�������,0db����,1Mbps,���������濪��;     
   dev_nrf24l01_write_reg(NRF_WRITE_REG+NRF_REG_CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�;  

	dev_nrf24l01_closespi();
/*CE���ߣ����뷢��ģʽ*/   
   dev_nrf24l01_setce(1);  
   Delay(100); //CEҪ����һ��ʱ��Ž��뷢��ģʽ  
}  

void NRF_RX_Mode(u8 rfch)  
{  
   dev_nrf24l01_openspi();
   dev_nrf24l01_setce(0);      
   dev_nrf24l01_write_buf(NRF_WRITE_REG+NRF_REG_RX_ADDR_P0, RX_ADDRESS_0, RX_ADR_WIDTH);//дRX�ڵ��ַ  
   dev_nrf24l01_write_reg(NRF_WRITE_REG+NRF_REG_EN_AA,0x01);    //ʹ��ͨ��0���Զ�Ӧ��      
   dev_nrf24l01_write_reg(NRF_WRITE_REG+NRF_REG_EN_RXADDR,0x01);//ʹ��ͨ��0�Ľ��յ�ַ      
   dev_nrf24l01_write_reg(NRF_WRITE_REG+NRF_REG_RF_CH, rfch);      //����RFͨ��Ƶ��      
   dev_nrf24l01_write_reg(NRF_WRITE_REG+NRF_REG_RX_PW_P0, RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ��        
   dev_nrf24l01_write_reg(NRF_WRITE_REG+NRF_REG_RF_SETUP,0x07); //����TX�������,0db����,1Mbps,���������濪��     
   dev_nrf24l01_write_reg(NRF_WRITE_REG+NRF_REG_CONFIG, 0x0f);  //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ  

   dev_nrf24l01_closespi();

   dev_nrf24l01_setce(1); /*CE���ߣ��������ģʽ*/   
   Delay(100); //CE����һ��ʱ��  
}     

/**
 *@brief:      dev_nrf24l01_check
 *@details:    ���ģ���Ƿ�����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_nrf24l01_check(void)  
{  
    u8 buf[5]={0xC2,0xC2,0xC2,0xC2,0xC2};  
    u8 buf1[5];  
 
	
	dev_nrf24l01_openspi();
    dev_nrf24l01_write_buf(NRF_WRITE_REG + NRF_REG_TX_ADDR, buf, 5); /*д��5���ֽڵĵ�ַ.  */    
    dev_nrf24l01_read_buf(NRF_REG_TX_ADDR, buf1, 5); /*����д��ĵ�ַ */ 
	dev_nrf24l01_closespi();
	
	wjq_log(LOG_FUN, "\r\n-------------\r\n");
	PrintFormat(buf, sizeof(buf));
	wjq_log(LOG_FUN, "\r\n-------------\r\n");
	PrintFormat(buf1, sizeof(buf1));
	wjq_log(LOG_FUN, "\r\n-------------\r\n");
	
	if(0 == memcmp(buf,buf1, sizeof(buf)))  
    {
    	wjq_log(LOG_FUN, "nrf24l01 ok\r\n");
    	return 0;
    }
	else
	{
		wjq_log(LOG_FUN, "nrf24l01 err\r\n");
		return -1;
	}
}  
/**
 *@brief:      dev_nrf24l01_task
 *@details:    ��פ���񣬿��Կ����߳�
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_nrf24l01_task(void)
{
	return 0;
}
  

s32 dev_nrf24l01_open(void)
{
	return 0;
}
s32 dev_nrf24l01_close(void)
{
	/*�����������ģʽ*/
	dev_nrf24l01_setce(0);
	return 0;
}
/**
 *@brief:      dev_nrf24l01_read
 *@details:    ������
 *@param[in]   ��
 *@param[out]  ��
 *@retval:     
 */
s32 dev_nrf24l01_read(u8 *buf, u16 len)
{
		return 0;
}
/**
 *@brief:      dev_nrf24l01_write
 *@details:       д���ݣ���������
 *@param[in]       ��
 *@param[out]  ��
 *@retval:     
 */
s32 dev_nrf24l01_write(u8* addr, u8 *buf, u16 len)
{
	
	/* ���뷢��ģʽ */
	NRF_TX_Mode(addr, CHANAL);
	
	/* �������� */
	NRF_Tx_Dat(buf,len);
	/* ���ؽ���ģʽ */
	NRF_RX_Mode(CHANAL);
	
	return 0;
}


s32 dev_nrf24l01_test(void)
{
	s32 res;
	u8 buf[32];
	u8 src[256];
	u16 i;
	
	for(i = 0; i<sizeof(src);i++)
	{
		src[i] = i;
	}

	
	dev_nrf24l01_init();
	res = dev_nrf24l01_check();
	if(res != 0)
	{
		wjq_log(LOG_FUN, "nrf 24l01 init err!\r\n");
		while(1);
	}
	/*�������ģʽ*/
	NRF_RX_Mode(CHANAL);
	while(1)
	{
		#if 1
		wjq_log(LOG_FUN, "nrf write test\r\n");
		dev_nrf24l01_write(TX_ADDRESS ,src, sizeof(src));	
		Delay(1000);
		#else
		NRF_Rx_Dat(buf);
		#endif
	}
}

