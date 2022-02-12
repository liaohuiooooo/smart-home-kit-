
#include "stm32f4xx.h"
#include "wujique_log.h"
#include "emenu.h"
#include "dev_lcd.h"
#include "dev_keypad.h"
#include "font.h"
#include "mcu_uart.h"

DevLcdNode *emenulcd;

s32 test_wait_key(u8 key)
{
	while(1)
	{
		u8 keyvalue;
		s32 res;
		
		res = dev_keypad_read(&keyvalue, 1);
		if(res == 1)
		{
			if(key == 0)
				break;
			else if(keyvalue == key)
				break;	
		}
	}	
	return 0;
}

s32 emenu_test_fun(char *s)

{
	wjq_log(LOG_DEBUG, "test:%s", s);
	
	dev_lcd_color_fill(emenulcd, 1, 1000, 1, 1000, WHITE);
	/*���о�����ʾ���˵�*/
	dev_lcd_put_string(emenulcd, FONT_SONGTI_1212, 1, 32, s, BLACK);
	
	test_wait_key(0);
	
	return 0;
}	

s32 test_test(void)
{
	emenu_test_fun((char *)__FUNCTION__);
	return 0;
}


const MENU EMenuListTest[]=
{
	MENU_L_0,//�˵��ȼ�
	"���Գ���",//����
	"test",	//Ӣ��
	MENU_TYPE_LIST,//�˵�����
	NULL,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��

		MENU_L_1,//�˵��ȼ�
		"LCD",//����
		"LCD",	//Ӣ��
		MENU_TYPE_LIST,//�˵�����
		NULL,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��
			MENU_L_2,//�˵��ȼ�
			"VSPI OLED",//����
			"VSPI OLED",	//Ӣ��
			MENU_TYPE_FUN,//�˵�����
			test_test,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��

			MENU_L_2,//�˵��ȼ�
			"I2C OLED",//����
			"I2C OLED",	//Ӣ��
			MENU_TYPE_FUN,//�˵�����
			test_test,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��

			MENU_L_2,//�˵��ȼ�
			"SPI COG",//����
			"SPI COG",	//Ӣ��
			MENU_TYPE_FUN,//�˵�����
			test_test,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��

			MENU_L_2,//�˵��ȼ�
			"tft",//����
			"tft",	//Ӣ��
			MENU_TYPE_FUN,//�˵�����
			test_test,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��
	
			MENU_L_2,//�˵��ȼ�
			"ͼƬ����",//����
			"test BMP",	//Ӣ��
			MENU_TYPE_FUN,//�˵�����
			test_test,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��
			
			MENU_L_2,//�˵��ȼ�
			"�ֿ����",//����
			"test Font",	//Ӣ��
			MENU_TYPE_FUN,//�˵�����
			test_test,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��

		MENU_L_1,//�˵��ȼ�
		"����",//����
		"sound",	//Ӣ��
		MENU_TYPE_LIST,//�˵�����
		NULL,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��
			MENU_L_2,//�˵��ȼ�
			"������",//����
			"buzzer",	//Ӣ��
			MENU_TYPE_FUN,//�˵�����
			test_test,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��

			MENU_L_2,//�˵��ȼ�
			"DAC����",//����
			"DAC music",	//Ӣ��
			MENU_TYPE_FUN,//�˵�����
			test_test,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��

			MENU_L_2,//�˵��ȼ�
			"����",//����
			"FM",	//Ӣ��
			MENU_TYPE_FUN,//�˵�����
			test_test,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��

			MENU_L_2,//�˵��ȼ�
			"I2S����",//����
			"I2S Music",	//Ӣ��
			MENU_TYPE_FUN,//�˵�����
			test_test,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��

			MENU_L_2,//�˵��ȼ�
			"¼��",//����
			"rec",	//Ӣ��
			MENU_TYPE_FUN,//�˵�����
			test_test,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��

		MENU_L_1,//�˵��ȼ�
		"������",//����
		"tp",	//Ӣ��
		MENU_TYPE_LIST,//�˵�����
		NULL,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��
			
			MENU_L_2,//�˵��ȼ�
			"У׼",//����
			"calibrate",	//Ӣ��
			MENU_TYPE_FUN,//�˵�����
			test_test,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��

			MENU_L_2,//�˵��ȼ�
			"����",//����
			"test",	//Ӣ��
			MENU_TYPE_FUN,//�˵�����
			test_test,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��
			
		MENU_L_1,//�˵��ȼ�
		"����",//����
		"KEY",	//Ӣ��
		MENU_TYPE_FUN,//�˵�����
		test_test,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��

		MENU_L_1,//�˵��ȼ�
		"����",//����
		"camera",	//Ӣ��
		MENU_TYPE_FUN,//�˵�����
		test_test,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��

		MENU_L_1,//�˵��ȼ�
		"485",//����
		"485",	//Ӣ��
		MENU_TYPE_LIST,//�˵�����
		NULL,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��
			MENU_L_2,//�˵��ȼ�
			"485 ����",//����
			"485 rec",	//Ӣ��
			MENU_TYPE_FUN,//�˵�����
			test_test,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��
			MENU_L_2,//�˵��ȼ�
			"485 ����",//����
			"485 snd",	//Ӣ��
			MENU_TYPE_FUN,//�˵�����
			test_test,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��
			
		MENU_L_1,//�˵��ȼ�
		"CAN",//����
		"CAN",	//Ӣ��
		MENU_TYPE_LIST,//�˵�����
		NULL,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��
			MENU_L_2,//�˵��ȼ�
			"CAN ����",//����
			"CAN rec",	//Ӣ��
			MENU_TYPE_FUN,//�˵�����
			test_test,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��
			
			MENU_L_2,//�˵��ȼ�
			"CAN ����",//����
			"CAN snd",	//Ӣ��
			MENU_TYPE_FUN,//�˵�����
			test_test,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��
			
		MENU_L_1,//�˵��ȼ�
		"SPI FLASH",//����
		"SPI FLASH",	//Ӣ��
		MENU_TYPE_LIST,//�˵�����
		NULL,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��
			MENU_L_2,//�˵��ȼ�
			"���İ�FLASH",//����
			"core FLASH",	//Ӣ��
			MENU_TYPE_FUN,//�˵�����
			test_test,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��
			
			MENU_L_2,//�˵��ȼ�
			"�װ� FLASH",//����
			"board FLASH",	//Ӣ��
			MENU_TYPE_FUN,//�˵�����
			test_test,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��
		
		MENU_L_1,//�˵��ȼ�
		"����",//����
		"uart",	//Ӣ��
		MENU_TYPE_FUN,//�˵�����
		test_test,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��

		MENU_L_1,//�˵��ȼ�
		"����",//����
		"eth",	//Ӣ��
		MENU_TYPE_LIST,//�˵�����
		NULL,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��

		MENU_L_1,//�˵��ȼ�
		"OTG",//����
		"OTG",	//Ӣ��
		MENU_TYPE_LIST,//�˵�����
		NULL,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��
			MENU_L_2,//�˵��ȼ�
			"HOST",//����
			"HOST",	//Ӣ��
			MENU_TYPE_LIST,//�˵�����
			NULL,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��
			MENU_L_2,//�˵��ȼ�
			"Device",//����
			"Device",	//Ӣ��
			MENU_TYPE_LIST,//�˵�����
			NULL,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��

		/*�����ӿڲ���*/
		MENU_L_1,//�˵��ȼ�
		"RF24L01",//����
		"RF24L01",	//Ӣ��
		MENU_TYPE_LIST,//�˵�����
		NULL,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��

		MENU_L_1,//�˵��ȼ�
		"MPU6050",//����
		"MPU6050",	//Ӣ��
		MENU_TYPE_LIST,//�˵�����
		NULL,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��

		/*���Բ��⣬�������Գ����Ҫʹ�þ��󰴼�*/
		MENU_L_1,//�˵��ȼ�
		"���󰴼�",//����
		"keypad",	//Ӣ��
		MENU_TYPE_LIST,//�˵�����
		NULL,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��
		
		/*�ô�������8266ģ��*/
		MENU_L_1,//�˵��ȼ�
		"wifi",//����
		"wifi",	//Ӣ��
		MENU_TYPE_LIST,//�˵�����
		NULL,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��
		
	/*���Ĳ˵��ǽ����˵���������*/			
	MENU_L_0,//�˵��ȼ�
	"END",//����
	"END",	//Ӣ��
	MENU_TYPE_NULL,//�˵�����
	NULL,//�˵����������ܲ˵��Ż�ִ�У����Ӳ˵��Ĳ���ִ��
};
	



s32 emenu_test(DevLcdNode *lcd)
{
	emenulcd = lcd;
	if(emenulcd == NULL)
	{
		wjq_log(LOG_DEBUG, "open lcd err\r\n");
	}
	emenu_run(emenulcd, (MENU *)&EMenuListTest[0], sizeof(EMenuListTest)/sizeof(MENU), FONT_SONGTI_1212, 1);	
	
	return 0;
}


