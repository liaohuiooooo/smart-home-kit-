/**
 * @file            dev_tea5767.c
 * @brief           ������оƬ tea5765 ����
 * @author          test
 * @date            2017��10��31�� ���ڶ�
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:      2017��10��31�� ���ڶ�
 *   ��    ��:         �ݼ�ȸ������
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
#include "mcu_i2c.h"

extern void Delay(__IO uint32_t nTime);



#define DEV_TEA5767_DEBUG

#ifdef DEV_TEA5767_DEBUG
#define TEA5767_DEBUG	wjq_log 
#else
#define TEA5767_DEBUG(a, ...)
#endif

#define DEV_TEA5767_I2CBUS "VI2C1"
#define DEV_TEA5767_I2CC_ADDR 0x60//110 0000b, 7λ��ַģʽ����I2C�����л��������

#define TEA5767_MAX_FREQ 108000
#define TEA5767_MIN_FREQ 87500

unsigned int Tea5767MaxPll=0x339b;    //108MHzʱ��pll,
unsigned int Tea5767MinPll=0x299d;    //87.5MHzʱ��pll.

unsigned int Tea5767Fre;
unsigned int Tea5767Pll;

//tea5767����I2Cͨ�ŵ�ʱ��ÿ�ζ��ǽ�5���ֽ�һ���������
u8 tea5767_readbuf[5];
//��ʼ��Ҫд��TEA5767�����ݣ���<TEA5767HN�͹���������������������>
u8 tea5767_writebuf[5]={0x2a,0xb6,0x51,0x11,0x40};
u8 tea5767_initbuf[5]={0xaa,0xb6,0x51,0x11,0x40};//�������ϵ��ʼ��


static s32 dev_tea5767_readreg(u8* data)
{
	DevI2cNode *dev;
	dev = mcu_i2c_open(DEV_TEA5767_I2CBUS);
	mcu_i2c_transfer(dev, DEV_TEA5767_I2CC_ADDR, MCU_I2C_MODE_R, data, 5);
	mcu_i2c_close(dev);	
	return 0;
}
static s32 dev_tea5767_writereg(u8* data)
{
	DevI2cNode *dev;

	dev = mcu_i2c_open(DEV_TEA5767_I2CBUS);
    mcu_i2c_transfer(dev, DEV_TEA5767_I2CC_ADDR, MCU_I2C_MODE_W, data, 5);
	mcu_i2c_close(dev);
	return 0;	
}

/**
 *@brief:      dev_tea5767_fre2pll
 *@details:    ��Ƶ��ֵתΪPLL
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
static u32 dev_tea5767_fre2pll(u8 hlsi, u32 fre)
{
    if (hlsi)
    {
        //return (((fre+225)*4000)/32768);
		return (u32)((float)((fre+225))/(float)8.192);
    }
	else
	{
        //return (((fre-225)*4000)/32768);
		return (u32)((float)((fre-225))/(float)8.192);
	}
}
/**
 *@brief:      dev_tea5767_pll2fre
 *@details:    ��PLLת��ΪƵ��
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
static u32 dev_tea5767_pll2fre(u8 hlsi, u32 pll)
{  
    if (hlsi)
        return (pll*8192/1000 - 225);
    else
        return (pll*8192/1000 + 225);

}
/**
 *@brief:      dev_tea5767_getfre
 *@details:    �Ӷ�ȡ����״ֵ̬�м���FMƵ��
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
static void dev_tea5767_getfre(void)
{
    unsigned char temp_l,temp_h;
    
    Tea5767Pll=0;
	
    temp_l =tea5767_readbuf[1];
    temp_h =tea5767_readbuf[0]&0x3f;
	
    Tea5767Pll=temp_h*256 + temp_l;
    
    Tea5767Fre = dev_tea5767_pll2fre(1, Tea5767Pll);
}

/**
 *@brief:      dev_tea5767_setfre
 *@details:    ֱ������tea5767��ָ��Ƶ��
 *@param[in]   unsigned long fre  
 *@param[out]  ��
 *@retval:     
 */
void dev_tea5767_setfre(unsigned long fre)
{

    if(fre>TEA5767_MAX_FREQ)
        return;
    if(fre<TEA5767_MIN_FREQ)  
        return;

    Tea5767Fre = fre;            
    Tea5767Pll = dev_tea5767_fre2pll(tea5767_writebuf[2]&0x10, Tea5767Fre);
    
    tea5767_writebuf[0]=Tea5767Pll/256;
    tea5767_writebuf[1]=Tea5767Pll%256;

	dev_tea5767_writereg(tea5767_writebuf);
}

/**
 *@brief:      dev_tea5767_set_ssl
 *@details:    ��������ֹͣ����
 *@param[in]   u8 level  0 ����������1 �ͼ���2 �м���3 �߼���
 *@param[out]  ��
 *@retval:     
 */
s32 dev_tea5767_set_ssl(u8 level)
{
	u8 tmp;
	
	if(level > 3)
		return -1;
	
	tmp = tea5767_writebuf[2];
	tea5767_writebuf[2] = ((tmp&0x9f)|(level<<5));
	TEA5767_DEBUG(LOG_DEBUG, "---%02x\r\n", tea5767_writebuf[2]);
	
	return 0;
}
/**
 *@brief:      dev_tea5767_auto_search
 *@details:    оƬ�Զ���̨
 *@param[in]   u8 mode  mode=1,Ƶ��������̨; mode=0:Ƶ�ʼ�С��̨
 *@param[out]  ��
 *@retval:     
 */
s32 dev_tea5767_auto_search(u8 mode)
{
	u8 if_counter = 0;
	u32 flag = 0;
	u8 adc;

	dev_tea5767_readreg(tea5767_readbuf);
	dev_tea5767_getfre();

    if(mode)
    {
        tea5767_writebuf[2] |=0x80;

    }
    else
    {
        tea5767_writebuf[2] &=0x7f; 
    }          
	
	while(1)
	{
		flag = 0;
		
		if(mode)
    	{
        	Tea5767Fre += 20;

    	}
    	else
    	{
        	Tea5767Fre -= 20;
    	} 
		TEA5767_DEBUG(LOG_DEBUG, "Tea5767Fre:%d\r\n", Tea5767Fre);
	    Tea5767Pll = dev_tea5767_fre2pll(1, Tea5767Fre);

		tea5767_writebuf[0]=Tea5767Pll/256+0x40;//��0x40�ǽ�SM��Ϊ1 Ϊ�Զ�����ģʽ
		tea5767_writebuf[1]=Tea5767Pll%256;   

		dev_tea5767_writereg(tea5767_writebuf);
	    while(1)     
	    {
	        dev_tea5767_readreg(tea5767_readbuf);
	        if(0x80 == (tea5767_readbuf[0]&0x80))//��̨�ɹ���־
	        {
				dev_tea5767_getfre();
				if_counter = tea5767_readbuf[2]&0x7f;
				if((0x31 < if_counter) && (0x3e > if_counter))
				{         
					adc = (tea5767_readbuf[3]>>4);
					TEA5767_DEBUG(LOG_DEBUG, "fre:%d, adc:%d\r\n", Tea5767Fre, adc);
					//�Ƚ�leve ��ƽȷ���Ƿ��յ�̨ ʵ�ʲ���ʹ�ô��� ����©̨
					if( adc >= 8)
					{
						TEA5767_DEBUG(LOG_DEBUG, "fre: \r\n", Tea5767Fre);
						return 0;
					}
				}
	            flag = 1;
	        }

			if(0x60 == (tea5767_readbuf[0]&0x60))//����Ƶ�ʼ���
			{
				TEA5767_DEBUG(LOG_DEBUG, "tea5767  search fail!\r\n");
				
				if(mode)
					Tea5767Fre = TEA5767_MIN_FREQ;
				else
					Tea5767Fre = TEA5767_MAX_FREQ;
				
				flag = 1;
			}

			if(flag == 1)
			{
				break;
			}
	    }  

	}	
} 
/**
 *@brief:      dev_tea5767_search
 *@details:    ������̨ ����100KHZ
 *@param[in]   u8 mode  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_tea5767_search(u8 mode)
{
	u8 adc;

	dev_tea5767_readreg(tea5767_readbuf);

	dev_tea5767_getfre();
    TEA5767_DEBUG(LOG_DEBUG, "Tea5767Fre:%d\r\n", Tea5767Fre);

	if(mode)
	{
	    Tea5767Fre += 100;
		if(Tea5767Fre > TEA5767_MAX_FREQ)
			Tea5767Fre = TEA5767_MIN_FREQ;
	}
	else
	{
	    Tea5767Fre -= 100;
		if(Tea5767Fre < TEA5767_MIN_FREQ)
			Tea5767Fre = TEA5767_MAX_FREQ;
	}
	
	while(1)
	{
		
		dev_tea5767_setfre(Tea5767Fre);
		
		Delay(2);
		dev_tea5767_readreg(tea5767_readbuf);
		adc = (tea5767_readbuf[3]>>4);
		
		if( adc >=7)
		{
			TEA5767_DEBUG(LOG_DEBUG, "fre:%d, adc:%d\r\n", Tea5767Fre, adc);
			return 0;
		}

		if(mode)
		{
		    Tea5767Fre += 20;
			if(Tea5767Fre > TEA5767_MAX_FREQ)
				Tea5767Fre = TEA5767_MIN_FREQ;
		}
		else
		{
		    Tea5767Fre -= 20;
			if(Tea5767Fre < TEA5767_MIN_FREQ)
				Tea5767Fre = TEA5767_MAX_FREQ;
		}
	}

}
/**
 *@brief:      dev_tea5767_init
 *@details:    ��ʼ��TEA5767�豸
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_tea5767_init(void)
{

	dev_tea5767_writereg(tea5767_initbuf);
	return 0;
}
/**
 *@brief:      dev_tea5767_open
 *@details:    ��5767�豸
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_tea5767_open(void)
{

	dev_tea5767_writereg(tea5767_writebuf);
	return 0;
}
/**
 *@brief:      dev_tea5767_close
 *@details:    �ر�5767�豸
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_tea5767_close(void)
{
	dev_tea5767_writereg(tea5767_initbuf);
	return 0;
}

/**
 *@brief:      dev_tea5767_test
 *@details:       ����TEA5767�豸
 *@param[in]  void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_tea5767_test(void)
{
    wjq_log(LOG_INFO, "\r\n------tea5767 test--------\r\n");

    dev_tea5767_open();
	dev_tea5767_setfre(89800);
	while(1)
	{
		Delay(5000);
		dev_tea5767_auto_search(1);
	}
    //dev_tea5767_close();
    
    //return 0;
}


