#ifndef __DEV_KEYPAD_H__
#define __DEV_KEYPAD_H__

/*��ֵ�б�ʶͨ�ϵ�λ*/
#define KEYPAD_PR_MASK (0x80)


extern s32 dev_keypad_init(void);
extern s32 dev_keypad_open(void);
extern s32 dev_keypad_read(u8 *key, u8 len);
extern s32 dev_keypad_scan(void);

#endif
