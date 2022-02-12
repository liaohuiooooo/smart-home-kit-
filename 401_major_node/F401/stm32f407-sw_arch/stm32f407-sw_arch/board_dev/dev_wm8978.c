/**
 * @file            dev_wm8978.c
 * @brief           wm8978����
 * @author          wujique
 * @date            2017��11��16�� ������
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2017��11��16�� ������
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
#include "stm32f4xx.h"

#include "wujique_log.h"

#include "mcu_i2c.h"
#include "mcu_i2s.h"
#include "dev_wm8978.h"  

/*
	wm8978ͨ��I2C�ӿڿ��ƣ�����ֻ��д�����ܶ�
	�����ڴ��¼д��WM8978�Ĵ�����ֵ

*/
#define DEV_WM8978_I2CBUS "VI2C1"
#define WM8978_SLAVE_ADDRESS    0x1A

static u16 WM8978RegVaule[] = {
	0X0000,0X0000,0X0000,0X0000, 0X0050,0X0000,0X0140,0X0000,
	0X0000,0X0000,0X0000,0X00FF, 0X00FF,0X0000,0X0100,0X00FF,
	0X00FF,0X0000,0X012C,0X002C, 0X002C,0X002C,0X002C,0X0000,
	0X0032,0X0000,0X0000,0X0000, 0X0000,0X0000,0X0000,0X0000,
	0X0038,0X000B,0X0032,0X0000, 0X0008,0X000C,0X0093,0X00E9,
	0X0000,0X0000,0X0000,0X0000, 0X0003,0X0010,0X0010,0X0100,
	0X0100,0X0002,0X0001,0X0001, 0X0039,0X0039,0X0039,0X0039,
	0X0001,0X0001
};

/**
 *@brief:      dev_wm8978_writereg
 *@details:    дWM8978�Ĵ���
 *@param[in]   u8 reg     �Ĵ���
               u16 vaule  ��ֵ���;�λΪ��Чֵ
 *@param[out]  ��
 *@retval:     
 */
s32 dev_wm8978_writereg(u8 reg, u16 vaule)
{
	DevI2cNode *dev;
	
	s32 ret = -1;
	u8 data[2];

	data[0] = (reg<<1) | ((vaule>>8)&0x01);
	data[1] = vaule & 0xff;

	dev = mcu_i2c_open(DEV_WM8978_I2CBUS);
	ret = mcu_i2c_transfer(dev, WM8978_SLAVE_ADDRESS, MCU_I2C_MODE_W, data, 2);
	mcu_i2c_close(dev);
	
	if(ret == 0)
	{
		WM8978RegVaule[reg] = vaule;
	}
	
	return ret;	
}
/**
 *@brief:      dev_wm8978_readreg
 *@details:    ���ص�ǰWM8978�Ĵ����趨ֵ
 *@param[in]   u8 addr    
               u16 *data  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_wm8978_readreg(u8 addr, u16 *data)
{
	*data = WM8978RegVaule[addr];
	return 0;	
}
/**
 *@brief:      dev_wm8978_set_dataformat
 *@details:    ����I2S��ʽ
 *@param[in]   u8 Standard  
               u8 Format    
 *@param[out]  ��
 *@retval:     
 */
s32 dev_wm8978_set_dataformat(u8 Standard, u8 Format)
{
	u16 RegValue = 0;
	RegValue  = Standard<<3;	
	RegValue |= Format<<5;
	dev_wm8978_writereg(4, RegValue);	// ����IIS�ӿ�Ϊ I2S Phillips ��ʽ�����ݳ���16λ	
	return 0;
}

/**
 *@brief:      dev_wm8978_set_phone_vol
 *@details:    ���ö�������
 *@param[in]   u8 volume  0-63
 *@param[out]  ��
 *@retval:     
 */
s32 dev_wm8978_set_phone_vol(u8 volume)
{
	volume = volume & 0x3F;	

	dev_wm8978_writereg(52, volume);
	dev_wm8978_writereg(53, volume | 0x100);

	return 0;
}

/**
 *@brief:      dev_wm8978_set_spk_vol
 *@details:    ������������
 *@param[in]   u8 volume  0-63
 *@param[out]  ��
 *@retval:     
 */
s32 dev_wm8978_set_spk_vol(u8 volume)
{
	volume = volume & 0x3F;	

	dev_wm8978_writereg(54, volume);
	dev_wm8978_writereg(55, volume | 0x100);
	return 0;
}

/**
 *@brief:      dev_wm8978_set_mic_gain
 *@details:    ������˷�����
 *@param[in]   u8 gain   0-63
 *@param[out]  ��
 *@retval:     
 */
s32 dev_wm8978_set_mic_gain(u8 gain)
{
	gain = gain & 0x3F;

	dev_wm8978_writereg(45, gain);
	dev_wm8978_writereg(46, gain | (1 << 8));

	return 0;
}
/**
 *@brief:      dev_wm8978_set_line_gain
 *@details:    ����������������
 *@param[in]   u8 gain  0-7
 *@param[out]  ��
 *@retval:     
 */
s32 dev_wm8978_set_line_gain(u8 gain)
{
	u16 RegValue;

	gain = gain & 0x07;

	dev_wm8978_readreg(47, &RegValue);
	RegValue &= 0x8F;						
	RegValue |= (gain << 4);			
	dev_wm8978_writereg(47, RegValue);	

	dev_wm8978_readreg(48, &RegValue);  
	RegValue &= 0x8F;					
	RegValue |= (gain << 4);           
	dev_wm8978_writereg(48, RegValue);	

	return 0;
}
/**
 *@brief:      dev_wm8978_set_aux_gain
 *@details:    ����AUX��������
 *@param[in]   u8 gain  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_wm8978_set_aux_gain(u8 gain)
{
	u16 RegValue;

	gain&=0X07;
	dev_wm8978_readreg(47, &RegValue);
	RegValue&=~(7<<0);			 
 	dev_wm8978_writereg(47,RegValue|gain<<0);

	dev_wm8978_readreg(48, &RegValue);
	RegValue&=~(7<<0);			
 	dev_wm8978_writereg(48,RegValue|gain<<0);
	return 0;
}
/**
 *@brief:      dev_wm8978_inout
 *@details:    ����WM8978�������ͨ��
 *@param[in]   u16 In   
               u16 Out  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_wm8978_inout(u8 In, u8 Out)
{
	u16 RegValue;

	if ((In == WM8978_INPUT_NULL) && (Out == WM8978_OUTPUT_NULL))
	{
		return 0;
	}

	RegValue = (1 << 3) | (3 << 0);
	
	if (Out & WM8978_OUTPUT_LINE) 
	{
		RegValue |= ((1 << 7) | (1 << 6));
	}
	
	if ((In & WM8978_INPUT_LMIC) || (In & WM8978_INPUT_RMIC))
	{
		RegValue |= (1 << 4);
	}
	dev_wm8978_writereg(1, RegValue);

	RegValue = 0;
	if (Out & WM8978_OUTPUT_PHONE)	{
		RegValue |= (1 << 7);
		RegValue |= (1 << 8);
	}

	if (In & WM8978_INPUT_LMIC){
		RegValue |= ((1 << 4) | (1 << 2));
	}
	if (In & WM8978_INPUT_RMIC){
		RegValue |= ((1 << 5) | (1 << 3));
	}
	
	if (In & WM8978_INPUT_LINE){
		RegValue |= ((1 << 4) | (1 << 5));
	}
	if (In & WM8978_INPUT_RMIC){
		RegValue |= ((1 << 5) | (1 << 3));
	}
	if (In & WM8978_INPUT_ADC){
		RegValue |= ((1 << 1) | (1 << 0));
	}
	dev_wm8978_writereg(2, RegValue);

	RegValue = 0;
	if (Out & WM8978_OUTPUT_LINE){
		RegValue |= ((1 << 8) | (1 << 7));
	}
	if (Out & WM8978_OUTPUT_SPK){
		RegValue |= ((1 << 6) | (1 << 5));
	}
	if (Out != WM8978_OUTPUT_NULL){
		RegValue |= ((1 << 3) | (1 << 2));
	}
	
	if (In & WM8978_INPUT_DAC){
		RegValue |= ((1 << 1) | (1 << 0));
	}
	dev_wm8978_writereg(3, RegValue);

	RegValue = 0 << 8;
	if (In & WM8978_INPUT_LINE){
		RegValue |= ((1 << 6) | (1 << 2));
	}
	if (In & WM8978_INPUT_RMIC){
		RegValue |= ((1 << 5) | (1 << 4));
	}
	if (In & WM8978_INPUT_LMIC){
		RegValue |= ((1 << 1) | (1 << 0));
	}
	dev_wm8978_writereg(44, RegValue);	

	if (In & WM8978_INPUT_ADC){
		RegValue = (1 << 3) | (0 << 8) | (4 << 0);/* ��ֹADC��ͨ�˲���, ���ý���Ƶ�� */
	}
	else{
		RegValue = 0;
	}
	dev_wm8978_writereg(14, RegValue);	

	if (In & WM8978_INPUT_ADC)
	{
		RegValue = (0 << 7);
		dev_wm8978_writereg(27, RegValue);	
		RegValue = 0;
		dev_wm8978_writereg(28, RegValue);	
		dev_wm8978_writereg(29, RegValue);	
		dev_wm8978_writereg(30, RegValue);	
	}

	RegValue = 0;		/* ��ֹ�Զ�������� */
	dev_wm8978_writereg(32, RegValue);
	dev_wm8978_writereg(33, RegValue);
	dev_wm8978_writereg(34, RegValue);


	RegValue = (3 << 1) | (7 << 0);		/* ��ֹ�Զ�������� */
	dev_wm8978_writereg(35, RegValue);

	RegValue = 0;
	if ((In & WM8978_INPUT_LMIC) || (In & WM8978_INPUT_RMIC))
	{
		RegValue |= (1 << 8);	/* MIC����ȡ+20dB */
	}
	if (In & WM8978_INPUT_AUX)
	{
		RegValue |= (3 << 0);	/* Aux����̶�ȡ3���û��������е��� */
	}
	if (In & WM8978_INPUT_LINE)
	{
		RegValue |= (3 << 4);	/* Line����̶�ȡ3���û��������е��� */
	}
	dev_wm8978_writereg(47, RegValue);	/* д����������������ƼĴ��� */
	dev_wm8978_writereg(48, RegValue);	/* д����������������ƼĴ��� */

	RegValue = 0xFF;
	dev_wm8978_writereg(15, RegValue);	/* ѡ��0dB���Ȼ��������� */
	RegValue = 0x1FF;
	dev_wm8978_writereg(16, RegValue);	/* ͬ�������������� */

	RegValue = 0;
	if (Out & WM8978_OUTPUT_SPK)
	{
		RegValue |= (1 << 4);	/* ROUT2 ����, �������������� */
	}
	if (In & WM8978_INPUT_AUX)
	{
		RegValue |= ((7 << 1) | (1 << 0));
	}
	dev_wm8978_writereg(43, RegValue);

	RegValue = 0;
	if (In & WM8978_INPUT_DAC){
		RegValue |= ((1 << 6) | (1 << 5));
	}
	if (Out & WM8978_OUTPUT_SPK){
		RegValue |=  ((1 << 2) | (1 << 1));	/* SPK 1.5x����,  �ȱ���ʹ�� */
	}
	if (Out & WM8978_OUTPUT_LINE){
		RegValue |=  ((1 << 4) | (1 << 3));	/* BOOT3  BOOT4  1.5x���� */
	}
	dev_wm8978_writereg(49, RegValue);

	RegValue = 0;
	if (In & WM8978_INPUT_AUX){
		RegValue |= ((7 << 6) | (1 << 5));
	}
	if ((In & WM8978_INPUT_LINE) 
		|| (In & WM8978_INPUT_LMIC) 
		|| (In & WM8978_INPUT_RMIC)){
		RegValue |= ((7 << 2) | (1 << 1));
	}
	if (In & WM8978_INPUT_DAC){
		RegValue |= (1 << 0);
	}
	dev_wm8978_writereg(50, RegValue);
	dev_wm8978_writereg(51, RegValue);

	RegValue = 0;
	if (Out & WM8978_OUTPUT_LINE){
		RegValue |= (1 << 3);
	}
	dev_wm8978_writereg(56, RegValue);

	RegValue = 0;
	if (Out & WM8978_OUTPUT_LINE){
		RegValue |= ((1 << 4) |  (1 << 1));
	}
	dev_wm8978_writereg(57, RegValue);

	if (In & WM8978_INPUT_DAC){
		dev_wm8978_writereg(11, 255);
		dev_wm8978_writereg(12, 255 | 0x100);
	}
	else{
		dev_wm8978_writereg(11, 0);
		dev_wm8978_writereg(12, 0 | 0x100);
	}

	if (In & WM8978_INPUT_DAC){
		dev_wm8978_writereg(10, 0);
	}
	
	return 0;
}

/**
 *@brief:      dev_wm8978_init
 *@details:    ��ʼ��wm8978����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
static s32 dev_wm8978_setting_init(void)
{
	s32 ret = -1;

	ret = dev_wm8978_writereg(0,0x00);	// ��λWM8978
	if(ret == -1)		// ��λʧ��
		return ret;
	
	dev_wm8978_writereg(1,0x1B);	
	
	dev_wm8978_writereg(2,0x1B0);
	dev_wm8978_writereg(3, 0x000C);	// ʹ�������������
	dev_wm8978_writereg(6, 0x0000);	// �ɴ������ṩʱ���ź�
	dev_wm8978_writereg(43, 0x0010);	// ����ROUT2����,����������������
	dev_wm8978_writereg(49, 0x0006);	// ������ 1.5x ����, �����ȱ���

	dev_wm8978_inout(WM8978_INPUT_NULL,
						WM8978_OUTPUT_NULL);

	dev_wm8978_set_mic_gain(45);
	dev_wm8978_set_phone_vol(40);
	dev_wm8978_set_spk_vol(62);
	dev_wm8978_set_aux_gain(7);
	return ret;
}
/**
 *@brief:      dev_wm8978_init
 *@details:    ��ʼ��WM8978оƬ
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_wm8978_init(void)
{
	mcu_i2s_init();//��ʼ��I2S�ӿ�
	dev_wm8978_setting_init();//����WM8978��ʼ��״̬
	return 0;
}
/**
 *@brief:      dev_wm8978_open
 *@details:       ��WM8978������Ĭ���������ͨ��
 *@param[in]  void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_wm8978_open(void)
{
	//dev_wm8978_inout(WM8978_INPUT_DAC|WM8978_INPUT_AUX|WM8978_INPUT_ADC|WM8978_INPUT_LMIC|WM8978_INPUT_RMIC,
					//WM8978_OUTPUT_PHONE|WM8978_OUTPUT_SPK);
	return 0;
}
/**
 *@brief:      dev_wm8978_dataformat
 *@details:    ����WM8978��I2S�����������ݸ�ʽ
 *@param[in]   u32 Freq     
               u8 Standard  
               u8 Format    
 *@param[out]  ��
 *@retval:     
 */
s32 dev_wm8978_dataformat(u32 Freq, u8 Standard, u8 Format)
{
	u16 standard;
	u16 dataformat;
	
	dev_wm8978_set_dataformat(Standard, Format);

	if(Standard == WM8978_I2S_LSB)
	{
		standard = 	I2S_Standard_LSB;
	}
	else if(Standard == WM8978_I2S_MSB)
	{
		standard = 	I2S_Standard_MSB;
	}
	else if(Standard == WM8978_I2S_Phillips)
	{
		standard = 	I2S_Standard_Phillips;
	}
	else if(Standard == WM8978_I2S_PCM)
	{
		standard = 	I2S_Standard_PCMLong;
	}
	else
	{
		standard = 	I2S_Standard_Phillips;	
	}

	if(Format == WM8978_I2S_Data_16b)
	{
		dataformat = 	I2S_DataFormat_16b;
	}
	else if(Format == WM8978_I2S_Data_20b)
	{
		dataformat = 	I2S_DataFormat_16bextended;
	}
	else if(Format == WM8978_I2S_Data_24b)
	{
		dataformat = 	I2S_DataFormat_24b;
	}
	else if(Format == WM8978_I2S_Data_32b)
	{
		dataformat = 	I2S_DataFormat_32b;
	}
	
	mcu_i2s_config(Freq, standard, dataformat);
	mcu_i2sext_config(Freq, standard, dataformat);
	
	return 0;
}
/**
 *@brief:      dev_wm8978_transfer
 *@details:    ������ֹͣI2S���ݴ���
 *@param[in]   u8 onoff  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_wm8978_transfer(u8 onoff)
{
	if(onoff == 1)
	{
		mcu_i2s_dma_start();
	}
	else
	{
		mcu_i2s_dma_stop();	
	}
	return 0;
}

#if 0
/*   
	��������ʹ�ã�������Ϊ��̬���� 
*/
volatile u8 SoundBufIndex=0xff;//˫����������ȡֵ0��1��������ֵ0XFF

#define I2S_DMA_BUFF_SIZE (4096*4)
uint16_t I2sDmaBuf[2][I2S_DMA_BUFF_SIZE];

extern const u8 AUDIO_SAMPLE[291632];
/**
 *@brief:      fun_sound_set_free_buf
 *@details:    ���ÿ��л�������
 		
 *@param[in]   u8 *index  
 *@param[out]  ��
 *@retval:     
 */
s32 fun_sound_set_free_buf(u8 index)
{
	SoundBufIndex = index;
	return 0;
}
/**
 *@brief:      fun_sound_get_buff_index
 *@details:    ��ѯ��ǰ��Ҫ����BUF
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
static s32 fun_sound_get_buff_index(void)
{
	s32 res;

	res = SoundBufIndex;
	SoundBufIndex = 0xff;
	return res;
}

/**
 *@brief:      dev_wm8978_test
 *@details:    WM8978���Գ��򣬲�����Ƕ��WAV����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_wm8978_test(void)
{
	u32 sample_index=0;
	u32 buf_index = 0;
	u32 i;
	
	wjq_log(LOG_FUN, "test wm8978\r\n");
	
	dev_wm8978_dataformat(I2S_AudioFreq_8k, WM8978_I2S_Phillips, WM8978_I2S_Data_16b);
	
	mcu_i2s_dma_init(&I2sDmaBuf[0][0], &I2sDmaBuf[1][0], I2S_DMA_BUFF_SIZE);

	sample_index = 0;
	for(i = 0; i<I2S_DMA_BUFF_SIZE; i++)
	{
		I2sDmaBuf[0][i]	= AUDIO_SAMPLE[sample_index++];
		I2sDmaBuf[0][i] += (AUDIO_SAMPLE[sample_index++]<<8);
	}
	
	for(i = 0; i<I2S_DMA_BUFF_SIZE; i++)
	{
		I2sDmaBuf[1][i]	= AUDIO_SAMPLE[sample_index++];
		I2sDmaBuf[1][i] += (AUDIO_SAMPLE[sample_index++]<<8);
	}
	
	dev_wm8978_transfer(1);
	
	while(1)
	{
		buf_index = fun_sound_get_buff_index();
		if(0xff != buf_index)
		{
			for(i = 0; i<I2S_DMA_BUFF_SIZE; i++)
			{
				I2sDmaBuf[buf_index][i]	= AUDIO_SAMPLE[sample_index++];
				I2sDmaBuf[buf_index][i] += (AUDIO_SAMPLE[sample_index++]<<8);
			}
			
			if((sample_index + I2S_DMA_BUFF_SIZE) > (291632))
			{
				wjq_log(LOG_FUN, "play finish\r\n");
				break;
			}
		}

	}
	dev_wm8978_transfer(0);
	return 0;

}
#endif


