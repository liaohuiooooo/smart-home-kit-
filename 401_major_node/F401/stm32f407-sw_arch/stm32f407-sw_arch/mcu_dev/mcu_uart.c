/**
 * @file            mcu_uart.c
 * @brief           ��������
 * @author          wujique
 * @date            2017��10��24�� ���ڶ�
 * @version         ����
 * @par             
 * @par History:
 * 1.��    ��:      2017��10��24�� ���ڶ�
 *   ��    ��:     
 *   �޸�����:        �����ļ�
		��Ȩ˵����
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
#include "stm32f4xx_usart.h"
#include "mcu_uart.h"
#include "wujique_log.h"
#include "alloc.h"


#define MCU_UART1_BUF_SIZE       1024
#define MCU_UART2_BUF_SIZE       1024
/*������Ϣ����*/
#define MCU_UART3_BUF_SIZE       256

#if 0
/*���ڻ��� ��ʱͨ���������飬������Ϊ��̬����*/
u8 McuUart1Buf[MCU_UART1_BUF_SIZE];
u8 McuUart2Buf[MCU_UART2_BUF_SIZE];
u8 McuUart3Buf[MCU_UART3_BUF_SIZE];
#endif
u8 McuUartLogBuf[MCU_UART3_BUF_SIZE];

/*
@bref�������豸
*/
typedef struct  
{	
	/* Ӳ�����*/
	/*
		STM IO ������Ҫ̫�����ݣ�����ֱ���ڴ����ж��壬 �����ú궨��
		����Ǹ���һ���豸����������FLASH ���Ϳ������豸�����ж���һ�����ĸ�SPI�Ķ��塣
	*/

	USART_TypeDef* USARTx;

	/*RAM���*/
	s32 gd;	//�豸��� С�ڵ���0��Ϊδ���豸
	
	u16 size;// buf ��С
	u8 *Buf;//����ָ��
	u16 Head;//ͷ
	u16 End;//β
	u8  OverFg;//�����־	
}_strMcuUart; 

static _strMcuUart McuUart[MCU_UART_MAX];
/*------------------------------------------------------*/
/**
 *@brief:      mcu_uart_init
 *@details:       ��ʼ�������豸
 *@param[in]  void  
 *@param[out]  ��
 *@retval:     
 */
s32 mcu_uart_init(void)
{
	u8 i;

	for(i = 0; i<MCU_UART_MAX; i++)
	{
		McuUart[i].gd = -1;	
	}

	return 0;	
}

/**
 *@brief:      mcu_uart_open
 *@details:    �򿪴��ڣ�ʵ�ʾ��ǳ�ʼ������
 *@param[in]   s32 comport  
 *@param[out]  ��
 *@retval:     
 */
s32 mcu_uart_open(McuUartNum comport)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
	if(comport >= MCU_UART_MAX)
		return -1;

	if(McuUart[comport].gd >=0)
		return -1;

	if(comport == MCU_UART_1)
	{
		McuUart[MCU_UART_1].USARTx = USART1;
		McuUart[MCU_UART_1].End = 0;
		McuUart[MCU_UART_1].Head = 0;
		McuUart[MCU_UART_1].OverFg = 0;
		McuUart[MCU_UART_1].size = MCU_UART1_BUF_SIZE;
		McuUart[MCU_UART_1].gd = 0;
		McuUart[MCU_UART_1].Buf = (u8 *)wjq_malloc(MCU_UART1_BUF_SIZE);

		// ��GPIOʱ��
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		// �򿪴���ʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

		GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);

		GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

		// TXD ---- PA9
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //����
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		// RXD ---- PA10
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //����
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		//USART ��ʼ������
		USART_InitStructure.USART_BaudRate = 115200;//������;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ 8 λ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�շ�
		USART_Init(USART1, &USART_InitStructure); //��ʼ������
		USART_Cmd(USART1, ENABLE); //ʹ�ܴ���
		USART_ClearFlag(USART1, USART_FLAG_TC);

		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
		//Usart1NVIC ����
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//��ռ���ȼ�
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      //��Ӧ���ȼ�
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

	}
	else if(comport == MCU_UART_2)
	{
		McuUart[MCU_UART_2].USARTx = USART2;
		McuUart[MCU_UART_2].End = 0;
		McuUart[MCU_UART_2].Head = 0;
		McuUart[MCU_UART_2].OverFg = 0;
		McuUart[MCU_UART_2].size = MCU_UART2_BUF_SIZE;
		McuUart[MCU_UART_2].gd = 0;
		
		McuUart[MCU_UART_2].Buf = (u8 *)wjq_malloc(MCU_UART2_BUF_SIZE);
		
		// ��GPIOʱ��
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		// �򿪴���ʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

		GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART3);//GPIOB10 ����Ϊ USART3

		GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART3);//GPIOB11 ����Ϊ USART3

		// TXD ---- PA2
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //����
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		// RXD ---- PA3
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //����
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		//USART ��ʼ������
		USART_InitStructure.USART_BaudRate = 115200;//������;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ 8 λ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�շ�
		USART_Init(USART2, &USART_InitStructure); //��ʼ������
		USART_Cmd(USART2, ENABLE); //ʹ�ܴ���
		USART_ClearFlag(USART2, USART_FLAG_TC);

		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
		//Usart2 NVIC ����
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//��ռ���ȼ�
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      //��Ӧ���ȼ�
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

	}
	else if(comport == MCU_UART_3)
	{
		McuUart[MCU_UART_3].USARTx = USART3;
		McuUart[MCU_UART_3].End = 0;
		McuUart[MCU_UART_3].Head = 0;
		McuUart[MCU_UART_3].OverFg = 0;
		McuUart[MCU_UART_3].size = MCU_UART3_BUF_SIZE;
		McuUart[MCU_UART_3].gd = 0;
		McuUart[MCU_UART_3].Buf = McuUartLogBuf;//(u8 *)wjq_malloc(MCU_UART3_BUF_SIZE);
		// ��GPIOʱ��
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		// �򿪴���ʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

		GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);//GPIOB10 ����Ϊ USART3

		GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);//GPIOB11 ����Ϊ USART3

		// TXD ---- PB10
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //����
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		// RXD ---- PB11
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //����
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		//USART ��ʼ������
		USART_InitStructure.USART_BaudRate = 115200;//������;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ 8 λ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�շ�
		USART_Init(USART3, &USART_InitStructure); //��ʼ������
		USART_Cmd(USART3, ENABLE); //ʹ�ܴ���
		USART_ClearFlag(USART3, USART_FLAG_TC);

		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�
		//Usart3 NVIC ����
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//��ռ���ȼ�
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      //��Ӧ���ȼ�
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
	else
	{
		/* ���ںŲ�֧��*/
		return -1;
	}
    return (0);
}
/**
 *@brief:      mcu_uart_close
 *@details:    �رմ���
 *@param[in]   s32 comport  
 *@param[out]  ��
 *@retval:     
 */
s32 mcu_uart_close (McuUartNum comport)
{
	if(comport >= MCU_UART_MAX)
		return -1;

	if(McuUart[comport].gd < 0)
		return 0;

	if(comport == MCU_UART_1)
	{
    	USART_Cmd(USART1, DISABLE);
    	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);
    }
    else if(comport == MCU_UART_2)
	{
    	USART_Cmd(USART2, DISABLE);
    	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);
    }
	else if(comport == MCU_UART_3)
	{
    	USART_Cmd(USART3, DISABLE);
    	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, DISABLE);
    } 
	else
		return -1;
	
	wjq_free(McuUart[comport].Buf);
	
	McuUart[comport].gd = -1;
	
    return(0);
}
/**
 *@brief:      mcu_dev_uart_tcflush
 *@details:    �崮�ڽ��ջ���
 *@param[in]   s32 comport  
 *@param[out]  ��
 *@retval:     
 */
s32 mcu_uart_tcflush(McuUartNum comport)
{ 
	if(comport >= MCU_UART_MAX)
		return -1;

	if(McuUart[comport].gd < 0)
		return -1;
	
	McuUart[comport].Head = McuUart[comport].End; 
    return 0;
}
/**
 *@brief:      mcu_dev_uart_set_baud
 *@details:    ���ô��ڲ�����
 *@param[in]   s32 comport   
               s32 baud      
               s32 databits  
               s32 parity    
               s32 stopbits  
               s32 flowctl   
 *@param[out]  ��
 *@retval:     
 */
s32 mcu_uart_set_baud (McuUartNum comport, s32 baud)
{
    USART_InitTypeDef USART_InitStructure;

	if(comport >= MCU_UART_MAX)
		return -1;
    if(McuUart[comport].gd < 0)
		return -1;
    
    USART_Cmd(McuUart[comport].USARTx, DISABLE);
    USART_InitStructure.USART_BaudRate = baud; 
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(McuUart[comport].USARTx, &USART_InitStructure);
	
    mcu_uart_tcflush(comport);
    USART_Cmd(McuUart[comport].USARTx, ENABLE); 

    return 0;
}
/**
 *@brief:      mcu_uart_read
 *@details:    ���������ݣ��������ݣ�
 *@param[in]   s32 comport  
               u8 *buf      
               s32 len      
 *@param[out]  ��
 *@retval:     
 */
s32 mcu_uart_read (McuUartNum comport, u8 *buf, s32 len)
{
    s32 i;
    
    if(len <= 0) return(-1);
    if(buf == NULL) return(-1);
	if(comport >= MCU_UART_MAX)
		return -1;
	
	if(McuUart[comport].gd < 0)
		return -1;
	
    i = 0;

    //uart_printf("rec index:%d, %d\r\n", UartHead3, rec_end3);
    while(McuUart[comport].Head != McuUart[comport].End)
    {
        *buf = *(McuUart[comport].Buf + McuUart[comport].Head);
		McuUart[comport].Head++;
        if(McuUart[comport].Head >= McuUart[comport].size) 
            McuUart[comport].Head = 0;

        buf ++;
        i ++;
        if(i >= len)
        {
            break;
        }
  }
  return (i);
}
/**
 *@brief:      mcu_dev_uart_write
 *@details:    д��������(��������)
 *@param[in]   s32 comport  
               u8 *buf      
               s32 len      
 *@param[out]  ��
 *@retval:     
 */
s32 mcu_uart_write (McuUartNum comport, u8 *buf, s32 len)
{
    u32 t;
    u16 ch;
  
    if (len <= 0) 
        return(-1);
        
    if(buf == NULL) 
        return(-1);
	
	if(comport >= MCU_UART_MAX)
		return -1;

	if(McuUart[comport].gd < 0)
		return -1;
	
    while(len != 0)
    {
        // �ȴ����ڷ������
        t = 0;
        while(USART_GetFlagStatus(McuUart[comport].USARTx, USART_FLAG_TXE) == RESET)
        {
            if(t++ >= 0x1000000)
                return(-1);
        }  
        ch = (u16)(*buf & 0xff);
        USART_SendData(McuUart[comport].USARTx, ch);
        buf++;
        len--;
    }
    
    return(0);
}
/**
 *@brief:      mcu_uart3_IRQhandler
 *@details:    �����жϴ�����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
void mcu_uart3_IRQhandler(void)
{
    unsigned short ch;

    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
        if(USART_GetITStatus(USART3, USART_IT_ORE) != RESET)
        {
            // �������ݽ����ַ��ж�ʧ
            //wjq_log(LOG_DEBUG, "1");
            ch = USART_ReceiveData(USART3);
            USART_GetITStatus(USART3, USART_IT_ORE); // ���ORE���

            McuUart[MCU_UART_3].OverFg = 1;
        }
        else
        {
            ch = USART_ReceiveData(USART3);
            //uart_printf("%02x", ch);
            *(McuUart[MCU_UART_3].Buf+McuUart[MCU_UART_3].End) = (unsigned char)(ch&0xff);
			McuUart[MCU_UART_3].End++;
			if(McuUart[MCU_UART_3].End >= McuUart[MCU_UART_3].size)
                McuUart[MCU_UART_3].End = 0;
                
            if(McuUart[MCU_UART_3].End == McuUart[MCU_UART_3].Head)       // ���ڽ��ջ��������
                McuUart[MCU_UART_3].Head = 1;
        }
    }
    
    if(USART_GetITStatus(USART3, USART_IT_FE) != RESET)
    {
        /* Clear the USART3 Frame error pending bit */
        USART_ClearITPendingBit(USART3, USART_IT_FE);
        USART_ReceiveData(USART3);

    }
#if 0
    /* If the USART3 detects a parity error */
    if(USART_GetITStatus(USART3, USART_IT_PE) != RESET)
    {
        while(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET)
        {
        }
        /* Clear the USART3 Parity error pending bit */
        USART_ClearITPendingBit(USART3, USART_IT_PE);
        USART_ReceiveData(USART3);
    }
    /* If a Overrun error is signaled by the card */
    if(USART_GetITStatus(USART3, USART_IT_ORE) != RESET)
    {
        /* Clear the USART3 Frame error pending bit */
        USART_ClearITPendingBit(USART3, USART_IT_ORE);
        USART_ReceiveData(USART3);
    }
    /* If a Noise error is signaled by the card */
    if(USART_GetITStatus(USART3, USART_IT_NE) != RESET)
    {
        /* Clear the USART3 Frame error pending bit */
        USART_ClearITPendingBit(USART3, USART_IT_NE);
        USART_ReceiveData(USART3);
    }
#endif    
}
/**
 *@brief:      mcu_uart2_IRQhandler
 *@details:    �����жϴ�����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
void mcu_uart2_IRQhandler(void)
{
	unsigned short ch;

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		if(USART_GetITStatus(USART2, USART_IT_ORE) != RESET)
		{
			// �������ݽ����ַ��ж�ʧ
			//uart_printf("1");
			ch = USART_ReceiveData(USART2);
			USART_GetITStatus(USART2, USART_IT_ORE); // ���ORE���

			McuUart[MCU_UART_2].OverFg = 1;
		}
		else
		{
			ch = USART_ReceiveData(USART2);
			//uart_printf("%02x", ch);
			*(McuUart[MCU_UART_2].Buf+McuUart[MCU_UART_2].End) = (unsigned char)(ch&0xff);
			McuUart[MCU_UART_2].End++;
			if(McuUart[MCU_UART_2].End >= McuUart[MCU_UART_2].size)
				McuUart[MCU_UART_2].End = 0;
				
			if(McuUart[MCU_UART_2].End == McuUart[MCU_UART_2].Head) 	  // ���ڽ��ջ��������
				McuUart[MCU_UART_2].Head = 1;
		}
	}
	
	if(USART_GetITStatus(USART2, USART_IT_FE) != RESET)
	{
		/* Clear the USART3 Frame error pending bit */
		USART_ClearITPendingBit(USART2, USART_IT_FE);
		USART_ReceiveData(USART2);

	}
#if 0
	/* If the USART3 detects a parity error */
	if(USART_GetITStatus(USART3, USART_IT_PE) != RESET)
	{
		while(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET)
		{
		}
		/* Clear the USART3 Parity error pending bit */
		USART_ClearITPendingBit(USART3, USART_IT_PE);
		USART_ReceiveData(USART3);
	}
	/* If a Overrun error is signaled by the card */
	if(USART_GetITStatus(USART3, USART_IT_ORE) != RESET)
	{
		/* Clear the USART3 Frame error pending bit */
		USART_ClearITPendingBit(USART3, USART_IT_ORE);
		USART_ReceiveData(USART3);
	}
	/* If a Noise error is signaled by the card */
	if(USART_GetITStatus(USART3, USART_IT_NE) != RESET)
	{
		/* Clear the USART3 Frame error pending bit */
		USART_ClearITPendingBit(USART3, USART_IT_NE);
		USART_ReceiveData(USART3);
	}
#endif    
}


/**
 *@brief:      mcu_uart1_IRQhandler
 *@details:    �����жϴ�����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
void mcu_uart1_IRQhandler(void)
{
	unsigned short ch;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		if(USART_GetITStatus(USART1, USART_IT_ORE) != RESET)
		{
			// �������ݽ����ַ��ж�ʧ
			//uart_printf("1");
			ch = USART_ReceiveData(USART1);
			USART_GetITStatus(USART1, USART_IT_ORE); // ���ORE���

			McuUart[MCU_UART_1].OverFg = 1;
		}
		else
		{
			ch = USART_ReceiveData(USART1);
			//uart_printf("%02x", ch);
			*(McuUart[MCU_UART_1].Buf+McuUart[MCU_UART_1].End) = (unsigned char)(ch&0xff);
			McuUart[MCU_UART_1].End++;
			if(McuUart[MCU_UART_1].End >= McuUart[MCU_UART_1].size)
				McuUart[MCU_UART_1].End = 0;
				
			if(McuUart[MCU_UART_1].End == McuUart[MCU_UART_1].Head) 	  // ���ڽ��ջ��������
				McuUart[MCU_UART_1].Head = 1;
		}
	}
	
	if(USART_GetITStatus(USART1, USART_IT_FE) != RESET)
	{
		/* Clear the USART3 Frame error pending bit */
		USART_ClearITPendingBit(USART1, USART_IT_FE);
		USART_ReceiveData(USART1);

	}
}

/**
 *@brief:      mcu_dev_uart_test
 *@details:    ���ڲ���
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
void mcu_uart_test(void)
{
    u8 buf[12];
    s32 len;
    s32 res;
    
    len =  mcu_uart_read (PC_PORT, buf, 10);
    wjq_log(LOG_FUN, "mcu_dev_uart_read :%d\r\n", len);
    res = mcu_uart_write(PC_PORT, buf, len);
    wjq_log(LOG_FUN, "mcu_dev_uart_write res: %d\r\n", res);
    wjq_log(LOG_FUN, "%s,%s,%d,%s\r\n", __FUNCTION__,__FILE__,__LINE__,__DATE__);
    
}

