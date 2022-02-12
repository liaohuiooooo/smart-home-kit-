/**
 * @file            dev_htu21d.c
 * @brief           
 * @author          test
 * @date            2017��10��31�� ���ڶ�
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:      2018��5��31�� ���ڶ�
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
#include "dev_htu21d.h"

#define DEV_HTU21D_I2CBUS "VI2C1"
/* HTU21D i2c��ַ�����������λ��дλ */
#define HTU21D_I2C_ADDR   0X40
/*
��ν���������֣�������ת�������У�HTU21D����סSCK��
��ô���I2C�Ͳ��ܿ��������豸�ˡ�
���ͨ���ò�����ģʽ��ͨ����ѯ��ת�������
*/
#define HTU21D_CMD_TTM_H 	0XE3//���¶ȣ���������ģʽ
#define HTU21D_CMD_THM_H 	0XE5//��ʪ�ȣ���������ģʽ
#define HTU21D_CMD_TTM_NH 	0XF3//���¶ȣ�����������
#define HTU21D_CMD_THM_NH 	0XF5//��ʪ�Ȳ�����
#define HTU21D_CMD_WREG 	0XE6//д�û��Ĵ���
#define HTU21D_CMD_RREG 	0XE7//���û��Ĵ���
#define HTU21D_CMD_RESET 	0XFE//��λ
/**
 *@brief:     
 *@details:   
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
s32 dev_htu21d_open(void)
{
		return 0;
}
/**
 *@brief:     
 *@details:   
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
s32 dev_htu21d_init(void)
{
	DevI2cNode *dev;
	u8 tmp[2];

	tmp[0] = HTU21D_CMD_RESET;
	
	dev = mcu_i2c_open(DEV_HTU21D_I2CBUS);
    mcu_i2c_transfer(dev, HTU21D_I2C_ADDR, MCU_I2C_MODE_W, tmp, 1);
	mcu_i2c_close(dev);
	return 0;	
}
/**
 *@brief:     
 *@details:   
 *@param[in]    
 *@param[out]  
 *@retval:     
 */

s32 dev_htu21d_colse(void)
{
	return 0;
}
/**
 *@brief:     
 *@details:   
 *@param[in]    
 *@param[out]  
 *@retval:     
 */

s32 dev_htu21d_read(u8 type)
{
	DevI2cNode *dev;
	u8 tmp[3];
	s32 res;
	u32 data;
	s32 temp;

	if(type == HTU21D_READ_TEMP)
		tmp[0] = HTU21D_CMD_TTM_NH;
	else if(type == HTU21D_READ_HUMI)
		tmp[0] = HTU21D_CMD_THM_NH;
	else
		return -1;
	
	dev = mcu_i2c_open(DEV_HTU21D_I2CBUS);
	wjq_log(LOG_INFO, "HTU21D_CMD_TTM_NH\r\n");	
    mcu_i2c_transfer(dev, HTU21D_I2C_ADDR, MCU_I2C_MODE_W, tmp, 1);

	wjq_log(LOG_INFO, "read data \r\n");
	while(1)
	{
		Delay(5);
		res = mcu_i2c_transfer(dev, HTU21D_I2C_ADDR, MCU_I2C_MODE_R, tmp, 3);
		if(res == 0)
			break;		
	}
	mcu_i2c_close(dev);
	/**/
	wjq_log(LOG_INFO, "-%02x %02x %02x\r\n", tmp[0], tmp[1], tmp[2]);
	//����CRCУ��
	
	//������ʪ��
	data = tmp[0]*256+(tmp[1]&0xfc);
	
	if((tmp[1] & 0x02) == 0x00)//�¶Ȳ���
	{
		temp = ((17572*data)>>16)-4685;
		wjq_log(LOG_INFO, "data: %d, temp:%d\r\n", data, temp);
	}
	else//ʪ�Ȳ���
	{
		temp = ((125*data)>>16)-6;
		wjq_log(LOG_INFO, "data: %d, humi:%d\r\n", data, temp);	
	}
	
	return temp;
}



