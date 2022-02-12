/**
 * @file            emenu.c
 * @brief           ���ײ˵�ģ��
 * @author          wujique
 * @date            2018��3��10�� ������
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2018��3��10�� ������
 *   ��    ��:         wujique�ݼ�ȸ������
 *   �޸�����:   �����ļ�
*/
#include "stm32f4xx.h"
#include "wujique_log.h"
#include "emenu.h"
#include "dev_lcd.h"
#include "font.h"
#include "dev_keypad.h"

extern u16 PenColor;
extern u16 BackColor;


/*��������*/
#define EMENU_KEYU_0	'0'
#define EMENU_KEYU_1	'1'
#define EMENU_KEYU_2	'2'
#define EMENU_KEYU_3	'3'
#define EMENU_KEYU_4	'4'
#define EMENU_KEYU_5	'5'
#define EMENU_KEYU_6	'6'
#define EMENU_KEYU_7	'7'
#define EMENU_KEYU_8	'8'
#define EMENU_KEYU_9	'9'
#define EMENU_KEYU_ESC	0x1b
#define EMENU_KEYU_F	0x70
#define EMENU_KEYU_DEL	0x2e
#define EMENU_KEYU_ENTER	0x0d
#define EMENU_KEYU_STAR 0x2a
#define EMENU_KEYU_HASH 0x23
#define EMENU_KEYU_UP	EMENU_KEYU_STAR
#define EMENU_KEYU_DOWM EMENU_KEYU_HASH

/*����ת����*/
const u8 emunu_key_chg[17]=
{
	0, EMENU_KEYU_1,EMENU_KEYU_2,EMENU_KEYU_3,EMENU_KEYU_F,
	EMENU_KEYU_4,EMENU_KEYU_5,EMENU_KEYU_6,EMENU_KEYU_DEL,
	EMENU_KEYU_7,EMENU_KEYU_8,EMENU_KEYU_9,EMENU_KEYU_ESC,
	EMENU_KEYU_STAR,EMENU_KEYU_0,EMENU_KEYU_HASH,EMENU_KEYU_ENTER
};
	
u8 emenu_get_key(void)
{
	u8 key;
	s32 res;
	
	res = dev_keypad_read(&key, 1);
	if(res == 1)
	{
		//wjq_log(LOG_DEBUG,"key:%02x\r\n", key);
		
		if((key & KEYPAD_PR_MASK)!= KEYPAD_PR_MASK  )
		{
			return emunu_key_chg[key];
		}
	}
	return 0;

}

/**
 *@brief:      emenu_check_list
 *@details:    ���˵��б�
 *@param[in]   MENU *p  
               u16 len  
 *@param[out]  ��
 *@retval:     
 */
s32 emenu_check_list(MENU *list, u16 len)
{
	u16 i = 0;
	s32 err = 0;;
	MenuLel now_lel = 0;//��ǰ�˵��ȼ�
	MENU *p = list;
	
	while(1)
	{
		if(i>= len)
			return err;

		/*�˵��ȼ���������*/
		if((p->l != now_lel)
			&&(p->l != (now_lel-1))
			&&(p->l != (now_lel+1))
			)
		{
			wjq_log(LOG_DEBUG,"level err!\r\n");
			return err;
		}
		/*���ܲ˵���������Ϊ��*/
		if(p->type == MENU_TYPE_FUN)
		{
		
		}

		p++;
		i++;
	}
	 
}
/**
 * @brief  �˵��������
*/

typedef struct _strMenuCtrl
{
	MENU *fa;	//���˵���Ҳ���ǵ�ǰ��ʾ�Ĳ˵�
	MENU *dis;	//��ʾ��ʼ
	MENU *sel;	//��ǰѡ�еĲ˵�
	u8 d;		//��¼dis��SEL��ͬ��ƫ�ƣ�����ÿ��ȥ��
	
	u8 lang;	//����ָ��ƫ����
	
	/*
		LCD��������
	*/
	u16 lcdh;
	u16 lcdw;
	
	FontType font;
	u8 fonth;
	u8 fontw;

	/*
		��ʾ����
	*/
	u8 spaced;//�м��
	u8 line;//һ������ʾ������
} MenuCtrl;

MenuCtrl menu_ctrl;

/*
	��¼dis��sel��d���Ա㷵��
*/
MENU *emenu_stack[2*MENU_L_MAX];
u8 d_rec[MENU_L_MAX];

MENU *emenu_find_fa(MENU *fp)
{
	MENU *p;
	p = fp;

	/*���ڵ�û����*/
	if(p->l == MENU_L_0)
			return NULL;
	
	while(1)
	{
		p--;
		if(p->l == ((menu_ctrl.fa->l)-1))
		{
			/*�ҵ����ڵ�*/
			return p;	
		}
	}
}

MENU *emunu_find_next(MENU *fp, u8 mode)
{
	MENU *p;
	p = fp;

	while(1)
	{
		if(mode == 1)
			p++;
		else
			p--;
		
		/*�˵��ȼ��ȸ��ڵ��һ������1��*/
		if(p->l == ((menu_ctrl.fa->l)+1))
		{
			return p;	
		}
		else if(p->l == (menu_ctrl.fa->l))
		{
			/*�����˵��ȼ�һ�£�˵��û�Ӳ˵���*/	
			return 0;
		}
		else if(p->l == MENU_L_0)
		{
			wjq_log(LOG_DEBUG,"menu end\r\n");
			return 0;
		}
		else
		{
			
		}

	}
}
/**
 *@brief:    
 *@details:   
 *@param[in]    
 *@param[out] 
 *@retval:     
 */
s32 emenu_display(DevLcdNode *lcd)
{
	MENU *p;
	u16 disy = 1;
	u16 disx;
	u8 discol = 0;
	char disbuf[32];
	u8 menu_num = 0;
	u16 lang;

	/*���㲻ͬ������ʾ���ݵ�ƫ��*/
	lang = menu_ctrl.lang*MENU_LANG_BUF_SIZE;
	
	wjq_log(LOG_DEBUG,"emenu display:%s\r\n", menu_ctrl.fa->cha + lang);
	
	dev_lcd_color_fill(lcd, 1, menu_ctrl.lcdw, 1, menu_ctrl.lcdh, BackColor);
	
	/*���о�����ʾ���˵�*/
	disx = (menu_ctrl.lcdw - strlen(menu_ctrl.fa->cha)*menu_ctrl.fontw)/2;//������ʾ
	dev_lcd_put_string(lcd, menu_ctrl.font, disx, disy, menu_ctrl.fa->cha + lang, PenColor);
	
	/* ��ʾ�Ӳ˵�*/
	switch(menu_ctrl.fa->type)
	{
		case MENU_TYPE_LIST:
			disy += menu_ctrl.fonth + menu_ctrl.spaced;//��Щ����Ҫ����ѡ�������ѡ��

			/* 
				�Ӹ��ڵ��һ���ڵ㿪ʼ�������Ա�ͳ���ӽڵ�����
				�ڲ˵�ǰ����ʾ���
			*/
			
			p =  menu_ctrl.fa +1;
			while(1)
			{
				/*�˵��ȼ��ȸ��ڵ��һ������1��*/
				if(p->l == ((menu_ctrl.fa->l)+1))
				{
					menu_num++;
					/*��ʼ��ʾ��֮ǰ�Ĳ˵�����ʾ*/
					if(p>=menu_ctrl.dis)
					{
						if(disy + menu_ctrl.fonth-1 > menu_ctrl.lcdh)//��ʾ12���ַ�����1����ʼ��ֻҪ��dot11�Ϳ�����ʾ���ˡ�
						{
							wjq_log(LOG_DEBUG,"over the lcd\r\n");
							break;
						}
						
						memset(disbuf,0,sizeof(disbuf));
						/*�˵���ѡ�У�����Ч��*/
						if(p == menu_ctrl.sel)
						{
							//uart_printf("sel menu\r\n");	
							sprintf(disbuf, ">%d.", menu_num);
						}
						else
						{
							sprintf(disbuf, "%d.", menu_num);	
						}
						strcat(disbuf, p->cha + lang);
						
						dev_lcd_put_string(lcd, menu_ctrl.font, 1, disy, disbuf, PenColor);

						disy += (menu_ctrl.fonth + menu_ctrl.spaced);//ÿ�м��
					}
				}
				else if(p->l == (menu_ctrl.fa->l))
				{
					/*�����˵��ȼ�һ�£�˵��û�Ӳ˵���*/	
					break;
				}
				else if(p->l == MENU_L_0)
				{
					/* ĩβ�ڵ�*/
					wjq_log(LOG_DEBUG, "menu end\r\n");
					break;
				}
				else
				{
					/*�ȵ�ǰ���ڵ��������˵������˵�*/
				}
				p++;
			}
			break;
			
		case MENU_TYPE_KEY_2COL:
			u8 col2_dis_num;
			/*
				���ּ�ֻ��0-9�����˫�в˵������ʾ8���˵�һҳ����1-8����ѡ��
			*/
			col2_dis_num = (menu_ctrl.line-1)*2;
			if(col2_dis_num > 8)
				col2_dis_num = 8;
			
			p = menu_ctrl.dis;//ֱ�Ӵ���ʾ�ڵ㿪ʼ����������ͳ���ӽڵ�����
			
			disy += menu_ctrl.fonth + menu_ctrl.spaced;
		
			menu_ctrl.d = 0;
			while(1)
			{

				if(p->l == ((menu_ctrl.fa->l)+1))
				{
					menu_num++;
					
					if(menu_num > col2_dis_num)
						break;
					/*������ʾλ��*/
					if(menu_num  == (col2_dis_num/2+1))
					{
						disy = menu_ctrl.fonth + menu_ctrl.spaced + 1;
						discol++;
						if(discol>=2)
						{
							wjq_log(LOG_DEBUG,"over the lcd\r\n");
							break;
						}
					}

					menu_ctrl.sel = p;//selָ����ʾ�����һ���˵�
					menu_ctrl.d++;
					
					memset(disbuf,0,sizeof(disbuf));
					sprintf(disbuf, "%d.", menu_num);	
					strcat(disbuf, p->cha + lang);
					
					dev_lcd_put_string(lcd, menu_ctrl.font, menu_ctrl.lcdw/2*discol+1, disy, disbuf, PenColor);

					disy += (menu_ctrl.fonth + menu_ctrl.spaced);
				}
				else if(p->l == (menu_ctrl.fa->l))
				{
					/*�����˵��ȼ�һ�£�˵��û�Ӳ˵���*/	
					break;
				}
				else if(p->l == MENU_L_0)
				{
					/* ĩβ�ڵ�*/
					wjq_log(LOG_DEBUG,"menu end\r\n");
					break;
				}
				else
				{
					/*�ȵ�ǰ���ڵ��������˵������˵�*/
				}
				p++;
			}
			break;
			
		case MENU_TYPE_B:
			break;
		case MENU_TYPE_KEY_1COL:
			break;
		
		default:
			break;
	}
	return 0;
}
/**
 *@brief:      emenu_deal_key_list
 *@details:    �б�˵���������
 *@param[in]   u8 key  
 *@param[out]  ��
 *@retval:     
 */
s32 emenu_deal_key_list(u8 key)
{
	MENU *menup;
	
	switch(key)
	{
	
		case EMENU_KEYU_DOWM://down
			menup = emunu_find_next(menu_ctrl.sel, 1);
			if(menup!=0)
			{
				menu_ctrl.sel = menup;	
				wjq_log(LOG_DEBUG,"%");
				if(menu_ctrl.d>= menu_ctrl.line-2)//���һ������ʾ5�У���������ʾ���ݣ�4�в˵������Ϊ3�����-2
				{
					menup = emunu_find_next(menu_ctrl.dis, 1);
					if(menup!=0)
					{
						menu_ctrl.dis = menup;	
					}
				}
				else
				{
					menu_ctrl.d++;
				}
			}
			else
			{
				/*�Ѿ������һ���˵�����ͷ��ʼ��ʾ��һ���˵�	*/
				menu_ctrl.sel = menu_ctrl.fa+1;
				menu_ctrl.dis = menu_ctrl.sel;
				menu_ctrl.d = 0;
			}
			break;
			
		case EMENU_KEYU_UP://up
			menup = emunu_find_next(menu_ctrl.sel, 0);
			
			if(menup!=0)
			{
				menu_ctrl.sel = menup;	
				wjq_log(LOG_DEBUG,"%");
				if(menu_ctrl.d == 0)
				{
						menu_ctrl.dis = menu_ctrl.sel;	
				}
				else
				{
					menu_ctrl.d--;
				}
			}
			else
			{
				/*��һ���ڵ㣬��ô�������һ���ڵ�*/
				while(1)
				{
					menup = emunu_find_next(menu_ctrl.sel, 1);
					if(menup!=0)
					{
						menu_ctrl.sel = menup;	

						if(menu_ctrl.d>= menu_ctrl.line-2)
						{
							menup = emunu_find_next(menu_ctrl.dis, 1);
							if(menup!=0)
							{
								menu_ctrl.dis = menup;	
							}
						}
						else
						{
							/*��ʾ�˵����䣬ֻ���ƶ���ѡ��*/
							menu_ctrl.d++;
						}
						}
						else
						{
							/*�Ѿ������һ���˵�����ͷ��ʼ��ʾ��һ���˵�	*/
							break;
						}
				}
			}
			break;
			
		case EMENU_KEYU_ENTER://enter
			/*��֤�Ӳ˵���Ч*/
			if(menu_ctrl.sel->l == (menu_ctrl.fa->l)+1)
			{
				if(menu_ctrl.sel->type == MENU_TYPE_FUN)
				{
					if(menu_ctrl.sel->fun != NULL)
						/*ִ����ʵ���Ժ���*/
						menu_ctrl.sel->fun();
				}
				else
				{
					wjq_log(LOG_DEBUG, "sel:%s\r\n", menu_ctrl.sel->cha);
					wjq_log(LOG_DEBUG, "dis:%s\r\n", menu_ctrl.dis->cha);
					
					emenu_stack[(menu_ctrl.fa->l)*2] = menu_ctrl.sel;
					emenu_stack[(menu_ctrl.fa->l)*2+1] = menu_ctrl.dis;
					
					d_rec[menu_ctrl.fa->l] = menu_ctrl.d;
					/* �����¼��˵� */
					menu_ctrl.fa = 	menu_ctrl.sel;
					menu_ctrl.sel = menu_ctrl.fa+1;
					menu_ctrl.dis = menu_ctrl.sel;
					menu_ctrl.d = 0;
				}
			}
			break;
			
		case EMENU_KEYU_ESC://ESC
			menup = emenu_find_fa(menu_ctrl.fa);
			if(menup != NULL)
			{
				menu_ctrl.fa = 	menup;
				
				menu_ctrl.sel = emenu_stack[(menu_ctrl.fa->l)*2];
				menu_ctrl.dis = emenu_stack[(menu_ctrl.fa->l)*2+1];
				menu_ctrl.d = d_rec[menu_ctrl.fa->l];

				wjq_log(LOG_DEBUG, "sel:%s\r\n", menu_ctrl.sel->cha);
				wjq_log(LOG_DEBUG, "dis:%s\r\n", menu_ctrl.dis->cha);

			}
			break;
		default:
			break;
	}
	return 0;
}

s32 emenu_deal_key_2col(u8 key)
{
	MENU *menup;
	u8 index;
	
	switch(key)
	{
	
		case EMENU_KEYU_DOWM://down
			menup = emunu_find_next(menu_ctrl.sel, 1);
			if(menup != 0)
			{
				menu_ctrl.dis = menup;	
			}
			break;
			
		case EMENU_KEYU_UP://up
			wjq_log(LOG_DEBUG, "2COL UP\r\n");
			u8 i = 0;
			while(1)
			{
				menup = emunu_find_next(menu_ctrl.dis, 0);
				if(menup != 0)
				{
					menu_ctrl.dis = menup;
				}
				else
					break;
				
				u8 col2_dis_num;
				/*
					���ּ�ֻ��0-9�����˫�в˵������ʾ8���˵�һҳ����1-8����ѡ��
				*/
				col2_dis_num = (menu_ctrl.line-1)*2;
				if(col2_dis_num > 8)
					col2_dis_num = 8;
				
				i++;
				if( i>= col2_dis_num)
					break;
			}
			break;
			
		case EMENU_KEYU_ESC://ESC
			menup = emenu_find_fa(menu_ctrl.fa);
			if(menup != NULL)
			{
				menu_ctrl.fa = 	menup;
				
				menu_ctrl.sel = emenu_stack[(menu_ctrl.fa->l)*2];
				menu_ctrl.dis = emenu_stack[(menu_ctrl.fa->l)*2+1];
				menu_ctrl.d = d_rec[menu_ctrl.fa->l];

				wjq_log(LOG_DEBUG, "sel:%s\r\n", menu_ctrl.sel->cha);
				wjq_log(LOG_DEBUG, "dis:%s\r\n", menu_ctrl.dis->cha);
			}
			break;

		case EMENU_KEYU_1:
		case EMENU_KEYU_2:
		case EMENU_KEYU_3:
		case EMENU_KEYU_4:
		case EMENU_KEYU_5:
		case EMENU_KEYU_6:
		case EMENU_KEYU_7:
		case EMENU_KEYU_8:
		case EMENU_KEYU_9:	
			index = key-'0';
			wjq_log(LOG_DEBUG, "index:%d\r\n", index);
			if(index <= menu_ctrl.d)
			{
				menup = menu_ctrl.dis;
				while(1)
				{
					index--;
					if(index == 0)
						break;
					
					menup = emunu_find_next(menup, 1);
					if(menup == 0)
						return -1;
				}
				
				if(menup->l == (menu_ctrl.fa->l)+1)
				{
					if(menup->type == MENU_TYPE_FUN)
					{
						if(menup->fun != NULL)
							/*ִ����ʵ���Ժ���*/
							menup->fun();
					}
					else
					{
						wjq_log(LOG_DEBUG, "sel:%s\r\n", menu_ctrl.sel->cha);
						wjq_log(LOG_DEBUG, "dis:%s\r\n", menu_ctrl.dis->cha);
						
						emenu_stack[(menu_ctrl.fa->l)*2] = menu_ctrl.sel;
						emenu_stack[(menu_ctrl.fa->l)*2+1] = menu_ctrl.dis;
						d_rec[menu_ctrl.fa->l] = menu_ctrl.d;
						
						/* �����¼��˵� */
						menu_ctrl.fa = 	menup;
						menu_ctrl.sel = menu_ctrl.fa+1;
						menu_ctrl.dis = menu_ctrl.sel;
						menu_ctrl.d = 0;
					}
				}	
				
			}
			else
			{
				wjq_log(LOG_DEBUG, "out list\r\n");
			}
			break;
		
		default:
			break;
	}

	
	return 0;
}


/**
 *@brief:      emenu_run
 *@details:    ����һ���˵�
 *@param[in]   MENU *p  �˵���ָ��  
               u16 len  �˵�����
 *@param[out]  ��
 *@retval:     
 */
s32 emenu_run(DevLcdNode *lcd, MENU *p, u16 len, FontType font, u8 spaced)
{
	u8 disflag = 1;
	

	#if 0
	MENU *menup = p;
	u16 i;
	uart_printf("menu len:%d\r\n", len);
	for(i=0; i< len; i++)
	{
		uart_printf("%s\r\n", menup->cha);
		menup++;
	}
	#endif
	
	menu_ctrl.lcdw = lcd->width;
	menu_ctrl.lcdh = lcd->height;
	menu_ctrl.font = font;
	font_get_hw(font, &(menu_ctrl.fonth), &(menu_ctrl.fontw));
	
	menu_ctrl.spaced = spaced;
	
	menu_ctrl.lang = MENU_LANG_CHA;//����ѡ��
	
	menu_ctrl.fa = p;//��ʼ������ʾ���˵���Ҳ����ΨһMENU_L_0�˵�
	menu_ctrl.sel = p+1;//�ڶ����˵��϶��ǵ�һ��MENU_L_1
	menu_ctrl.dis = menu_ctrl.sel;
	menu_ctrl.d = 0;
	menu_ctrl.line = (menu_ctrl.lcdh+1)/(menu_ctrl.fonth + menu_ctrl.spaced);//+1����Ϊ���һ�п��Բ�Ҫ��࣬
	
	wjq_log(LOG_DEBUG, "line:%d\r\n", menu_ctrl.line);
	
	while(1)
	{
		if(1==disflag)
		{
			emenu_display(lcd);
			disflag = 0;
		}
		
		u8 key;

		key = emenu_get_key();
		
		if(key != 0)
		{
			//wjq_log(LOG_DEBUG, "KEY:%02x\r\n", key);
			
			if(menu_ctrl.fa->type == MENU_TYPE_LIST)
			{
				emenu_deal_key_list(key);
			}
			else if(menu_ctrl.fa->type == MENU_TYPE_KEY_2COL)
			{
				emenu_deal_key_2col(key);
			}
			//wjq_log(LOG_DEBUG,"get a key:%02x\r\n", key);
			disflag = 1;

		}
	}
		
}

