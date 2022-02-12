
#ifndef __EASY_MENU_H__
#define __EASY_MENU_H__


typedef enum{
	MENU_TYPE_NULL = 0,
/*
���ܲ˵������ܲ˵�û���Ӳ˵�����ִ�еĺ�������ʵ�ʹ���
*/
	MENU_TYPE_FUN,
/*
//<�б�˵�> �˵�����ʾ�ڶ������¼��˵�ͨ���б���ʽ��ʾ���·�
ͨ�����°���ѡ���Ӳ˵�, ͨ��ȷ�ϼ�ѡ�������һ���˵�
*/
	MENU_TYPE_LIST,
/*
//<�춥�˵�> �����˵���ͳ�Ʋ˵���ʾ�ڶ�����ͨ������ѡ�񱾼��˵���
�¼��˵�ͨ���б���ʽ��ʾ���·���ͨ������ѡ���Ӳ˵�
ֻ�е�һ���˵�������Ϊ�춥�˵����춥�˵�����һ����һ�����б�˵�
*/
	MENU_TYPE_B,
/*
//<�����˵�> �����˵�����ʾ�ڶ������Ӳ˵�����ʽ��ʾ���·���ͨ������ѡ�����˵�
  �������ڷ�ҳ�Ӳ˵� 
  MENU_TYPE_KEY_1COL ���� MENU_TYPE_KEY_2COL˫��
  ˫�в˵�ͨ�����ְ���ѡ��ͨ�����ּ���ֻ��10��������һ��ֻʹ��1-8
*/
	MENU_TYPE_KEY_1COL,
	MENU_TYPE_KEY_2COL
}MenuType;

typedef enum{
	MENU_L_0 = 0,
	MENU_L_1,
	MENU_L_2,
	MENU_L_3,
	MENU_L_MAX
}MenuLel;

typedef enum{
	MENU_LANG_CHA = 0,
	MENU_LANG_ENG,
}MenuLang;

#define MENU_LANG_BUF_SIZE 16
/**
 * @brief  �˵�����
*/
typedef struct _strMenu
{
    MenuLel l;     ///<�˵��ȼ�
    char cha[MENU_LANG_BUF_SIZE];   ///����
    char eng[MENU_LANG_BUF_SIZE];   ///Ӣ��
    MenuType type;  ///
    s32 (*fun)(void);  ///���Ժ���

} MENU;

#include "dev_lcd.h"
extern s32 emenu_run(DevLcdNode *lcd, MENU *p, u16 len, FontType font, u8 spaced);


#endif

