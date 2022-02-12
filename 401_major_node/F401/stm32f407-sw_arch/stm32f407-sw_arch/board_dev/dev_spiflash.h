#ifndef __DEV_SPIFLASH_H_
#define __DEV_SPIFLASH_H_

/*SPI FLASH ��Ϣ*/
typedef struct
{
	char *name;
	u32 JID;
	u32 MID;
	/*���������������С����Ϣ*/
	u32 sectornum;//�ܿ���
	u32 sectorsize;//���С
	u32 structure;//������
	
}_strSpiFlash;


/*SPI FLASH�豸����*/
typedef struct
{
	char *name;//�豸����
	char *spich;//����������SPIͨ��
	_strSpiFlash *pra;//�豸��Ϣ
}DevSpiFlash;

typedef struct
{
	/**/
	s32 gd;
	/*�豸��Ϣ*/
	
	DevSpiFlash dev;
	
	/*spi ͨ���ڵ�*/
	DevSpiChNode *spichnode;
	
	struct list_head list;
}DevSpiFlashNode;

extern s32 dev_spiflash_sector_erase(DevSpiFlashNode *node, u32 sector);
extern s32 dev_spiflash_sector_read(DevSpiFlashNode *node, u32 sector, u8 *dst);	
extern s32 dev_spiflash_sector_write(DevSpiFlashNode *node, u32 sector, u8 *src);
extern DevSpiFlashNode *dev_spiflash_open(char* name);
extern s32 dev_spiflash_close(DevSpiFlashNode *node);

extern s32 dev_spiflash_test(void);

#endif

