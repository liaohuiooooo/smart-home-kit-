/**
 * @file            soundplay.h
 * @brief           �������Ŷ��ⶨ��
 * @author          wujique
 * @date            2018��1��7�� ������
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2018��1��7�� ������
 *   ��    ��:         wujique
 *   �޸�����:   �����ļ�
*/
#ifndef _SOUNDPLAY_H_
#define _SOUNDPLAY_H_

typedef enum {
  SOUND_IDLE =0,
  SOUND_BUSY,
  SOUND_PLAY,
  SOUND_PAUSE
}
SOUND_State;

typedef enum{
	SOUND_DEV_NULL = 0,	
	SOUND_DEV_1CH,
	SOUND_DEV_2CH
}SOUND_DEV_TYPE;


extern int fun_sound_play(char *name, char *dev);
extern s32 fun_sound_stop(void);
extern void fun_sound_task(void);
extern s32 fun_sound_pause(void);
extern s32 fun_sound_resume(void);
extern s32 fun_sound_setvol(u8 vol);
extern s32 fun_sound_rec(char *name);
extern s32 fun_rec_stop(void);
extern void fun_rec_task(void);

#endif
