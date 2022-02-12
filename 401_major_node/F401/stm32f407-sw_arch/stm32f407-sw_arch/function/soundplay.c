/**
 * @file            soundplay.c
 * @brief           �������Ź���
 * @author          wujique
 * @date            2018��1��6�� ������
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2018��1��6�� ������
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
/*

	�����������ǣ��ṩ�ӿڣ�����ָ��·���µ������ļ���ͨ��ָ�����豸���

	��ע��
		1 ��ʱֻ��16BIT WAV�ļ�����
		2 ������ͨ�������������˫����

*/
#include "stm32f4xx.h"
#include "ff.h"
#include "stm32f4xx_spi.h"
#include "dev_wm8978.h"
#include "mcu_i2s.h"
#include "soundplay.h"
#include "wujique_log.h"
#include "alloc.h"
#include "dev_dacsound.h"

#define FUN_SOUND_DEBUG

#ifdef FUN_SOUND_DEBUG
#define SOUND_DEBUG	wjq_log 
#else
#define SOUND_DEBUG(a, ...)
#endif



/*wav �ļ��ṹ*/
typedef struct _TWavHeader 
{          
	/*RIFF��*/
    int rId;    //��־����RIFF��  0x46464952
    int rLen;   //���ݴ�С,��������ͷ�Ĵ�С����Ƶ�ļ��Ĵ�С   (�ļ��ܴ�С-8)      
    int wId;    //��ʽ���ͣ�"WAVE"��   0x45564157
    
    /*Format Chunk*/
    int fId;    //"fmt " ��һ���ո� 0X20746D66     
    int fLen;   //Sizeof(WAVEFORMATEX)          
    short wFormatTag;       //�����ʽ������ 1 WAVE_FORMAT_PCM��WAVEFORMAT_ADPCM��         
    short nChannels;        //��������������Ϊ1��˫����Ϊ2         
    int nSamplesPerSec;   //����Ƶ��         
    int nAvgBytesPerSec;  //ÿ���������         
    short nBlockAlign;      //�����          
    short wBitsPerSample;   //WAVE�ļ��Ĳ�����С         
    int dId;              //"data"     �п�����FACT��     
    int wSampleLength;    //��Ƶ���ݵĴ�С 
    /*�������������һ��fact �飬��ѹ���йأ����û�У�����data��*/
}TWavHeader;


TWavHeader *wav_header;	
FIL SoundFile;//�����ļ�

/*
����SD�������֣�ֻҪ2*4K����
����U���е����֣�ȴҪ2*8K
*/
#define I2S_DMA_BUFF_SIZE1   (1024*4)
#define DAC_SOUND_BUFF_SIZE2 (1024*1)

u32 SoundBufSize = I2S_DMA_BUFF_SIZE1;//����Ƶ��С���Ϳ�Сһ�㻺�塣

volatile u8 SoundBufIndex=0xff;//˫����������ȡֵ0��1��������ֵ0XFF
u16 *SoundBufP[2];

volatile SOUND_State SoundSta = SOUND_IDLE;
u32 playlen;
SOUND_DEV_TYPE SoundDevType = SOUND_DEV_NULL;

/*------------------------------------------*/

s32 fun_sound_stop(void);
/**
 *@brief:      fun_sound_set_free_buf
 *@details:    ���ÿ��л�������
 			   ��������ṩ��I2S����DAC SOUNDģ�����
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
/*

	����������ʹ��WM8978����Ҫ��������
	���˫����

*/
static s32 fun_sound_deal_1ch_data(u8 *p)
{
	u8 ch1,ch2;
	u16 shift;
	u16 i;
	
	for(i=SoundBufSize;i>0;)
	{
		i--;
		//uart_printf("%d-",i);
		ch1 = *(p+i);
		i--;
		ch2 = *(p+i);
		
		shift = i*2;
		
		*(p+shift) = ch2;	
		*(p+shift+1) = ch1;
		*(p+shift+2) = ch2;	
		*(p+shift+3) = ch1;

	}
	
	return 0;
}

/**
 *@brief:      fun_sound_play
 *@details:    ͨ��ָ���豸����ָ������
 *@param[in]   char *name  
               char *dev   
 *@param[out]  ��
 *@retval:     
 */
int fun_sound_play(char *name, char *dev)
{
	FRESULT res;
	unsigned int len;

	SoundSta = SOUND_BUSY;
	/*
		���ļ��Ƿ���Ҫ�رգ�
		ͬʱ�򿪺ܶ��ļ��·�����ڴ�й©��
	*/
	res = f_open(&SoundFile, name, FA_READ);
	if(res != FR_OK)
	{
		SOUND_DEBUG(LOG_DEBUG, "sound open file err:%d\r\n", res);
		SoundSta = SOUND_IDLE;
		return -1;
	}

	SOUND_DEBUG(LOG_DEBUG, "sound open file ok\r\n");

	wav_header = (TWavHeader *)wjq_malloc(sizeof(TWavHeader));
	if(wav_header == 0)
	{
		SOUND_DEBUG(LOG_DEBUG, "sound malloc err!\r\n");
		SoundSta = SOUND_IDLE;
		return -1;
	}
	SOUND_DEBUG(LOG_DEBUG, "sound malloc ok\r\n");

	res = f_read(&SoundFile, (void *)wav_header, sizeof(TWavHeader), &len);
	if(res != FR_OK)
	{
		SOUND_DEBUG(LOG_DEBUG, "sound read err\r\n");
		SoundSta = SOUND_IDLE;
		return -1;
	}

	SOUND_DEBUG(LOG_DEBUG, "sound read ok\r\n");
	if(len != sizeof(TWavHeader))
	{
		SOUND_DEBUG(LOG_DEBUG, "read wav header err %d\r\n", len);
		SoundSta = SOUND_IDLE;
		return -1;
	}
	
	SOUND_DEBUG(LOG_DEBUG, "---%x\r\n", wav_header->rId);
	SOUND_DEBUG(LOG_DEBUG, "---%x\r\n", wav_header->rLen);
	SOUND_DEBUG(LOG_DEBUG, "---%x\r\n", wav_header->wId);//����WAVE(0X45564157)����˵����wave��ʽ
	SOUND_DEBUG(LOG_DEBUG, "---%x\r\n", wav_header->fId);
	SOUND_DEBUG(LOG_DEBUG, "---%x\r\n", wav_header->fLen);
	SOUND_DEBUG(LOG_DEBUG, "---wave ��ʽ %x\r\n", wav_header->wFormatTag);
	SOUND_DEBUG(LOG_DEBUG, "---����      %x\r\n", wav_header->nChannels);
	SOUND_DEBUG(LOG_DEBUG, "---����Ƶ��  %d\r\n", wav_header->nSamplesPerSec);
	SOUND_DEBUG(LOG_DEBUG, "---ÿ�������� %d\r\n", wav_header->nAvgBytesPerSec);
	SOUND_DEBUG(LOG_DEBUG, "---�����ֽ��� %d\r\n", wav_header->nBlockAlign);
	SOUND_DEBUG(LOG_DEBUG, "---λ�� :    %d bit\r\n", wav_header->wBitsPerSample);
	SOUND_DEBUG(LOG_DEBUG, "---data =    %x\r\n", wav_header->dId);
	SOUND_DEBUG(LOG_DEBUG, "---���ݳ���: %x\r\n", wav_header->wSampleLength);

	if(wav_header->nSamplesPerSec <= I2S_AudioFreq_16k)
	{
		SoundBufSize = DAC_SOUND_BUFF_SIZE2;
	}
	else
	{
		SoundBufSize = I2S_DMA_BUFF_SIZE1;
	}
	/*
	
	*/
	SoundBufP[0] = (u16 *)wjq_malloc(SoundBufSize*2); 
	SoundBufP[1] = (u16 *)wjq_malloc(SoundBufSize*2); 
	
	SOUND_DEBUG(LOG_DEBUG, "%08x, %08x\r\n", SoundBufP[0], SoundBufP[1]);
	if(SoundBufP[0] == NULL)
	{

		SOUND_DEBUG(LOG_DEBUG, "sound malloc err\r\n");
		SoundSta = SOUND_IDLE;
		return -1;
	}

	if(SoundBufP[1] == NULL )
	{
		wjq_free(SoundBufP[0]);
		SoundSta = SOUND_IDLE;
		return -1;
	}
		

	/*�����ļ��������ò���Ƶ�ʸ������ʽ*/
	u8 format;
	if(wav_header->wBitsPerSample == 16)
	{
		format =	WM8978_I2S_Data_16b; 	
	}
	else if(wav_header->wBitsPerSample == 24)
	{
		format =	WM8978_I2S_Data_24b; 	
	}
	else if(wav_header->wBitsPerSample == 32)
	{
		format =	WM8978_I2S_Data_32b; 	
	}

	/*��ָ���豸*/
	if(0 == strcmp(dev, "wm8978"))
	{

	}

	playlen = 0;

	u32 rlen;

	/*��Դ���������豸˫�����������ݸ���һ�ݵ�����һ������*/
	if((wav_header->nChannels == 1) && (SoundDevType == SOUND_DEV_2CH))
	{
		rlen = SoundBufSize;
		f_read(&SoundFile, (void *)SoundBufP[0], rlen, &len);
		fun_sound_deal_1ch_data((u8*)SoundBufP[0]);
		f_read(&SoundFile, (void *)SoundBufP[1], rlen, &len);
		fun_sound_deal_1ch_data((u8*)SoundBufP[1]);

	}
	else
	{
		rlen = SoundBufSize*2;
		f_read(&SoundFile, (void *)SoundBufP[0], rlen, &len);
		f_read(&SoundFile, (void *)SoundBufP[1], rlen, &len);
	}
	
	playlen += rlen*2;

	if(0 == strcmp(dev, "wm8978"))
	{
		dev_wm8978_open();
		dev_wm8978_inout(WM8978_INPUT_DAC|WM8978_INPUT_AUX|WM8978_INPUT_ADC,
					WM8978_OUTPUT_PHONE|WM8978_OUTPUT_SPK);
		dev_wm8978_dataformat(wav_header->nSamplesPerSec, WM8978_I2S_Phillips, format);
		mcu_i2s_dma_init(SoundBufP[0], SoundBufP[1], SoundBufSize);
		SoundDevType = SOUND_DEV_2CH;
		dev_wm8978_transfer(1);//����I2S����
	}
	else if(0 == strcmp(dev, "dacsound"))
	{
		dev_dacsound_open();
		dev_dacsound_dataformat(wav_header->nSamplesPerSec, WM8978_I2S_Phillips, format);
		dev_dacsound_setbuf(SoundBufP[0], SoundBufP[1], SoundBufSize);
		SoundDevType = SOUND_DEV_1CH;
		dev_dacsound_transfer(1);
	}
	

	SoundSta = SOUND_PLAY;
	
	return 0;
}

/**
 *@brief:      fun_sound_task
 *@details:    ����������ѯ����ִ�м��������̫�ã�-
               ������������������Ҳ���Ƕ���
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
void fun_sound_task(void)
{
	FRESULT res;
	unsigned int len;
	volatile s32 buf_index = 0;
	int rlen;
	u16 i;

	if(SoundSta == SOUND_BUSY
		|| SoundSta == SOUND_IDLE)
		return;
	
	buf_index = fun_sound_get_buff_index();
	if(0xff != buf_index)
	{
		if(SoundSta == SOUND_PAUSE)//��ͣ
		{
			for(i=0;i<SoundBufSize;i++)
			{
				*(SoundBufP[buf_index]+i)= 0x0000;
			}	
		}
		else
		{

			if((wav_header->nChannels == 1) && (SoundDevType == SOUND_DEV_2CH))
			{
				rlen = SoundBufSize;
				res = f_read(&SoundFile, (void *)SoundBufP[buf_index], rlen, &len);
				fun_sound_deal_1ch_data((u8*)SoundBufP[buf_index]);
			}
			else
			{
				rlen = SoundBufSize*2;
				res = f_read(&SoundFile, (void *)SoundBufP[buf_index], rlen, &len);
			}
			
			//memset(SoundBufP[buf_index], 0, SoundRecBufSize*2);
			
			playlen += len;


			/*
				u����BUG����ʱ����������ݳ��Ȳ���
				�Ƚ������������Ѿ����ŵĳ��ȸ���Դ���ȱȽϡ�
			*/
			if(len < rlen)
			{
				SOUND_DEBUG(LOG_DEBUG, "play finish %d, playlen:%x\r\n", len, playlen);
				fun_sound_stop();
				
			}	
		}
			
	}

}
/**
 *@brief:      fun_sound_get_sta
 *@details:    ��ѯ���ֲ���״̬
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
SOUND_State fun_sound_get_sta(void)
{
	return SoundSta;

}
/**
 *@brief:      fun_sound_stop
 *@details:    ֹͣ���ֲ���
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 fun_sound_stop(void)
{
	u32 res;
	
	if(SoundSta == SOUND_PLAY
		|| SoundSta == SOUND_PAUSE)	
	{
		if(SoundDevType == SOUND_DEV_2CH)
		{
			dev_wm8978_transfer(0);
		}
		else if(SoundDevType == SOUND_DEV_1CH)
		{
			dev_dacsound_transfer(0);
			dev_dacsound_close();
		}
		
		wjq_free(SoundBufP[0]);
		wjq_free(SoundBufP[1]);
		res = f_close(&SoundFile);
		SOUND_DEBUG(LOG_DEBUG, "f_close:%d\r\n", res);
		wjq_free(wav_header);
		SoundSta = SOUND_IDLE;	
	}
	return 0;
}
/**
 *@brief:      fun_sound_pause
 *@details:    ��ͣ����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 fun_sound_pause(void)
{
	if(SoundSta == SOUND_PLAY)
	{
		SoundSta = SOUND_PAUSE;
	}
	return 0;
}
/**
 *@brief:      fun_sound_resume
 *@details:    �ָ�����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 fun_sound_resume(void)
{
	if(SoundSta == SOUND_PAUSE)
	{
		SoundSta = SOUND_PLAY;
	}
	return 0;
}
/**
 *@brief:      fun_sound_setvol
 *@details:    ��������
 *@param[in]   u8 vol  
 *@param[out]  ��
 *@retval:     
 */
s32 fun_sound_setvol(u8 vol)
{
	return 0;
}

/**
 *@brief:      fun_sound_test
 *@details:    ���Բ���
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
void fun_sound_test(void)
{
	SOUND_DEBUG(LOG_DEBUG, "play sound\r\n");
	fun_sound_play("1:/mono_16bit_8k.wav", "dacsound");		

}
/*
������Դ����
mono_16bit_8k.wav
mono_16bit_44k.wav
stereo_16bit_32k.wav
ʮ�ͺ��.wav

*/

/*

	ͨ��I2S����WM8978¼��
	������һ����ͨ��˫����DMA¼����
	��Ҫע���һ���ǣ�¼������I2S_EXIT��������ʱ�ӣ���ҪI2S���ܲ���ͨ��ʱ�ӡ�
	Ҳ����˵Ҫ��������¼����
	����¼��ʱ��ҲҪ����I2S���ţ�����ֻ����һ���ֽڵ�DMA���壬�Ա�I2S����ͨ��ʱ�ӣ�
	
*/
/*---¼������������Ҫ��࣬����ụ�࿨��----*/
u32 SoundRecBufSize;
u16 *SoundRecBufP[2];
static u8 SoundRecBufIndex=0xff;//˫����������ȡֵ0��1��������ֵ0XFF
TWavHeader *recwav_header;	
FIL SoundRecFile;//�����ļ�
u32 RecWavSize = 0;
u16 RecPlayTmp[8];
s32 RecSta = 0;

/*
	¼��Ƶ�ʣ�������ǲ�����¼��һ������
	��Ҫ�ۺϿ����������¼��Ƶ��
*/
#define SOUND_REC_FRE 32000
/**
 *@brief:      fun_rec_set_free_buf
 *@details:    ����¼��������������I2S exit�ж���ʹ��
 *@param[in]   u8 index  
 *@param[out]  ��
 *@retval:     
 */
s32 fun_rec_set_free_buf(u8 index)
{
	SoundRecBufIndex = index;
	return 0;
}
/**
 *@brief:      fun_rec_get_buff_index
 *@details:    ��ѯ¼�����Ļ��壬����Ҫ����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     static
 */
static s32 fun_rec_get_buff_index(void)
{
	s32 res;

	res = SoundRecBufIndex;
	SoundRecBufIndex = 0xff;
	return res;
}
/**
 *@brief:      fun_sound_rec
 *@details:    ����¼��
 *@param[in]   char *name  
 *@param[out]  ��
 *@retval:     
 */
s32 fun_sound_rec(char *name)
{
	FRESULT fres;
	u32 len;

	SOUND_DEBUG(LOG_DEBUG, "sound rec\r\n");
	RecWavSize = 0;
	SoundRecBufSize = SoundBufSize;

	/*  ����WAV�ļ� */
	fres=f_open(&SoundRecFile,(const TCHAR*)name, FA_CREATE_ALWAYS | FA_WRITE); 
	if(fres != FR_OK)			//�ļ�����ʧ��
	{
		SOUND_DEBUG(LOG_DEBUG, "create rec file err!\r\n");
		return -1;
	}

	recwav_header = (TWavHeader *)wjq_malloc(sizeof(TWavHeader));
	if(recwav_header == NULL)
	{
		SOUND_DEBUG(LOG_DEBUG, "rec malloc err!\r\n");
		return -1;	
	}
	
	recwav_header->rId=0X46464952;
	recwav_header->rLen = 0;//¼����������
	recwav_header->wId = 0X45564157;//wave
	recwav_header->fId=0X20746D66;
	recwav_header->fLen = 16;
	recwav_header->wFormatTag = 0X01;
	recwav_header->nChannels = 2;
	recwav_header->nSamplesPerSec = SOUND_REC_FRE;//�������Ƶ����Ҫ���⴦����ʱ������
	recwav_header->nAvgBytesPerSec = (recwav_header->nSamplesPerSec)*(recwav_header->nChannels)*(16/8);
	recwav_header->nBlockAlign = recwav_header->nChannels*(16/8);
	recwav_header->wBitsPerSample = 16;
	recwav_header->dId = 0X61746164;
	recwav_header->wSampleLength = 0;

	fres=f_write(&SoundRecFile,(const void*)recwav_header,sizeof(TWavHeader), &len);
	if((fres!= FR_OK)
		|| (len != sizeof(TWavHeader)))
	{

		SOUND_DEBUG(LOG_DEBUG, "rec write err!\r\n");
		wjq_free(recwav_header);
		return -1;		
	}
	else
	{
		SOUND_DEBUG(LOG_DEBUG, "create rec wav ok!\r\n");
	}

	/*  ����¼��     */
	SoundRecBufP[0] = (u16 *)wjq_malloc(SoundRecBufSize*2); 
	SoundRecBufP[1] = (u16 *)wjq_malloc(SoundRecBufSize*2); 
	
	SOUND_DEBUG(LOG_DEBUG, "%08x, %08x\r\n", SoundRecBufP[0], SoundRecBufP[1]);
	if(SoundRecBufP[0] == NULL)
	{

		SOUND_DEBUG(LOG_DEBUG, "sound malloc err\r\n");
		return -1;
	}

	if(SoundRecBufP[1] == NULL )
	{
		wjq_free(SoundRecBufP[0]);
		return -1;
	}
	
	dev_wm8978_open();	
	dev_wm8978_inout(WM8978_INPUT_DAC|WM8978_INPUT_AUX|WM8978_INPUT_ADC|WM8978_INPUT_LMIC|WM8978_INPUT_RMIC,
					WM8978_OUTPUT_PHONE);
	dev_wm8978_dataformat(SOUND_REC_FRE, WM8978_I2S_Phillips, WM8978_I2S_Data_16b);
	mcu_i2s_dma_init(RecPlayTmp, RecPlayTmp, 1);

	mcu_i2sext_dma_init(SoundRecBufP[0], SoundRecBufP[1], SoundRecBufSize);
	mcu_i2sext_dma_start();
	
	RecSta = 1;
	
	dev_wm8978_transfer(1);//����I2S����
	

	SOUND_DEBUG(LOG_DEBUG, "rec--------------------\r\n");
	
	return 0;
}
/**
 *@brief:      fun_rec_stop
 *@details:    ֹͣ¼��
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 fun_rec_stop(void)
{
	u32 len;

	RecSta = 0;
	
	dev_wm8978_transfer(0);
	mcu_i2sext_dma_stop();
	
	recwav_header->rLen = RecWavSize+36;
	recwav_header->wSampleLength = RecWavSize;
	f_lseek(&SoundRecFile,0);
	
	f_write(&SoundRecFile,(const void*)recwav_header,sizeof(TWavHeader),&len);//д��ͷ����
	f_close(&SoundRecFile);

	wjq_free(SoundRecBufP[0]);
	wjq_free(SoundRecBufP[1]);
	wjq_free(recwav_header);
	return 0;
}
/**
 *@brief:      fun_rec_task
 *@details:    ¼���߳�
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
void fun_rec_task(void)
{
	int buf_index = 0;
	u32 len;
	FRESULT fres;

	if(RecSta == 0)
		return;
	
	buf_index = fun_rec_get_buff_index();
	if(0xff != buf_index)
	{
		//uart_printf("rec buf full:%d!\r\n", buf_index);
		RecWavSize += SoundRecBufSize*2;
	
		fres = f_write(&SoundRecFile,(const void*)SoundRecBufP[buf_index], 2*SoundRecBufSize, &len);
		if(fres != FR_OK)
		{
			SOUND_DEBUG(LOG_DEBUG, "write err\r\n");
		}
		
		if(len!= 2*SoundRecBufSize)
		{
			SOUND_DEBUG(LOG_DEBUG, "len err\r\n");
		}
		
	}
}

/**
 *@brief:      fun_rec_test
 *@details:    ��ʼ¼��
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
void fun_rec_test(void)
{
	fun_sound_rec("1:/rec9.wav");
}

void fun_play_rec_test(void)
{
	fun_sound_play("1:/rec9.wav", "wm8978");	
}

