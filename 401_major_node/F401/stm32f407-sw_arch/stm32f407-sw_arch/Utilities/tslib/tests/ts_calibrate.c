/*
 *  tslib/tests/ts_calibrate.c
 *
 *  Copyright (C) 2001 Russell King.
 *
 * This file is placed under the GPL.  Please see the file
 * COPYING for more details.
 *
 * $Id: ts_calibrate.c,v 1.8 2004/10/19 22:01:27 dlowder Exp $
 *
 * Basic test program for touchscreen library.

	ԭ����У������ǻ���linux LCD��ʾ�����ģ�
	��STM32�ϣ�û��LCD����ʾ�ܹ���
	��˱�У������޸Ľϴ�

 */
#include <stdio.h>
#include "tslib.h"
#include <stdarg.h>
#include "stm32f4xx.h"
#include "wujique_log.h"
#include "dev_lcd.h"

extern void put_cross(DevLcdNode *lcd, int x, int y, unsigned colidx);
extern void put_string_center(DevLcdNode *lcd, int x, int y, char *s, unsigned colidx);

extern int getxy(struct tsdev *ts, int *x, int *y);
/*

	����У׼���㣬����������Ǻ����㷨

	1.4Դ��������㷨�����⣬��������������һ���㷨��
	�ο���Tslib�д�����У׼ԭ����ʵ��.pdf��

*/
int perform_calibration(calibration *cal) 
{
    int j;
    float n, x, y, x2, y2, xy, z, zx, zy;
    float det, det1, det2, det3;
    float scaling = 65536.0;
    
    n = x = y = x2 = y2 = xy = 0;
    for (j = 0; j < 5; j++)
    {
        n += 1.0;
        x += (float)cal->x[j];
        y += (float)cal->y[j];
        x2 += (float)(cal->x[j] * cal->x[j]);
        y2 += (float)(cal->y[j] * cal->y[j]);
        xy += (float)(cal->x[j] * cal->y[j]);
    }
    
    det = n * (x2*y2 - xy*xy) + x * (xy*y - x*y2) + y * (x*xy - y*x2);
    if (det < 0.1 && det > -0.1)
    {
    	wjq_log(LOG_DEBUG, "ts_calibrate: determinant is too small -- %f\n",det);
        return -1;
    }
    
    z = zx = zy = 0;
    for (j = 0; j < 5; j++)
    {
        z += (float)cal->xfb[j];
        zx += (float)(cal->xfb[j] * cal->x[j]);
        zy += (float)(cal->xfb[j] * cal->y[j]);
    }
    
    det1 = n * (zx*y2 - xy*zy) + z * (xy*y - x*y2) + y * (x*zy - y*zx);
    det2 = n * (x2*zy - zx*xy) + x * (zx*y - x*zy) + z * (x*xy - y*x2);
    det3 = z * (x2*y2 - xy*xy) + x * (xy*zy - zx*y2) + y * (zx*xy - zy*x2);
    cal->a[0] = (int)((det1 / det) * scaling);
    cal->a[1] = (int)((det2 / det) * scaling);
    cal->a[2] = (int)((det3 / det) * scaling);

    z = zx = zy = 0;
    for (j = 0; j < 5; j++)
    {
        z += (float)cal->yfb[j];
        zx += (float)(cal->yfb[j] * cal->x[j]);
        zy += (float)(cal->yfb[j] * cal->y[j]);
    }
    
    det1 = n * (zx*y2 - xy*zy) + z * (xy*y - x*y2) + y * (x*zy - y*zx);
    det2 = n * (x2*zy - zx*xy) + x * (zx*y - x*zy) + z * (x*xy - y*x2);
    det3 = z * (x2*y2 - xy*xy) + x * (xy*zy - zx*y2) + y * (zx*xy - zy*x2);
    cal->a[3] = (int)((det1 / det) * scaling);
    cal->a[4] = (int)((det2 / det) * scaling);
    cal->a[5] = (int)((det3 / det) * scaling);
    cal->a[6] = (int)scaling;

	wjq_log(LOG_DEBUG, "calibration ok!\r\n");
    return 0;
}

static void get_sample (struct tsdev *ts, calibration *cal,
			int index, int x, int y, char *name)
{
	//static int last_x = -1, last_y;

	getxy (ts, &cal->x [index], &cal->y [index]);

	//last_x = 
	//last_y = 
	cal->xfb [index] = x;
	cal->yfb [index] = y;
	
	wjq_log(LOG_DEBUG, "\r\n %s : X = %4d Y = %4d\r\n", name, cal->x [index], cal->y [index]);
}
/*

	У׼���̣���LCD��ʾ���
	ʹ������У׼��

*/
calibration cal;//���ԣ�ֱ�Ӷ���һ��cal���ڱ���У׼����


struct tsdev *ts_open_module(void)
{
	struct tsdev *ts;
	char *tsdevice = NULL;
	unsigned int i;

	wjq_log(LOG_DEBUG, "env Calibration constants: ");
	for (i = 0; i < 7; i++) 
		wjq_log(LOG_DEBUG, "%d ", cal.a [i]);
	wjq_log(LOG_DEBUG, "\n");
	
	ts = ts_open(tsdevice,0);

	if (!ts) 
	{
		return NULL;
	}
	if (ts_config(ts)) 
	{
		return NULL;
	}

	return ts;
}

int ts_calibrate(DevLcdNode *lcd)
{
	struct tsdev *ts;
	char *tsdevice = NULL;
	int i;

	wjq_log(LOG_DEBUG, " main ts calibrate!\r\n");

	ts = ts_open(tsdevice,0);
	if (!ts) 
	{
		return -1;
	}

	if (ts_config(ts)) 
	{
		return -1;
	}

	/*���濪ʼ����У׼*/
	/*ˢLCD����*/

	/*  ��ʾ��ʾ��     */
	int xres = 240;
	int yres = 320;

	put_string_center (lcd, xres / 2, yres / 4,
			   "TSLIB calibration utility", RED);
	put_string_center (lcd,  xres / 2, yres / 4 + 20,
			   "Touch crosshair to calibrate", RED);

	wjq_log(LOG_DEBUG, "xres = %d, yres = %d\n", xres, yres);
	
	//----У׼���̣���ȡ5���������-------
	put_cross(lcd,  50, 50, RED);
	get_sample (ts, &cal, 0, 50,        50,        "Top left");
	wjq_log(LOG_DEBUG, "-----------------------Top left finish\r\n");

	put_cross(lcd,  xres - 50, 50, RED);
	get_sample (ts, &cal, 1, xres - 50, 50,        "Top right");
	wjq_log(LOG_DEBUG, "-----------------------Top right finish\r\n");

	put_cross(lcd,  xres - 50, yres - 50, RED);
	get_sample (ts, &cal, 2, xres - 50, yres - 50, "Bot right");
	wjq_log(LOG_DEBUG, "-----------------------Bot right finish\r\n");

	put_cross(lcd,  50,  yres - 50, RED);
	get_sample (ts, &cal, 3, 50,        yres - 50, "Bot left");
	wjq_log(LOG_DEBUG, "-----------------------Bot left finish\r\n");

	put_cross(lcd,  xres / 2, yres / 2, RED);
	get_sample (ts, &cal, 4, xres / 2,  yres / 2,  "Center");
	wjq_log(LOG_DEBUG, "-----------------------Center\r\n");

	if (0 == perform_calibration (&cal)) 
	{
		//У׼��õ�������
		wjq_log(LOG_DEBUG, "Calibration constants: ");
		for (i = 0; i < 7; i++) 
			wjq_log(LOG_DEBUG, "%d ", cal.a [i]);
		wjq_log(LOG_DEBUG, "\n");

	} 
	else 
	{
		wjq_log(LOG_DEBUG, "Calibration failed.\n");
		i = -1;
	}

	return i;
}

#if 0
extern s32 dev_lcd_drawpoint(u16 x, u16 y, u16 color);

s32 ts_calibrate_test(void)
{
	struct tsdev *ts;

	ts = ts_open_module();
	
	while(1)
	{
		struct ts_sample samp;
		int ret;
		
		ret = ts_read(ts, &samp, 1);
		if(ret == 1)
		{
			//uart_printf("pre:%d, x:%d, y:%d\r\n", samp.pressure, samp.x, samp.y);
			if(samp.pressure !=0 )
			{
				//uart_printf("pre:%d, x:%d, y:%d\r\n", samp.pressure, samp.x, samp.y);
				dev_lcd_drawpoint(samp.x, samp.y, 0xF800);	
			}
		}
		
	}	
}
#endif

