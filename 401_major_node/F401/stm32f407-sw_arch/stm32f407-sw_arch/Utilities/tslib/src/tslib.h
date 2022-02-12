#ifndef _TSLIB_H_
#define _TSLIB_H_
/*
 *  tslib/src/tslib.h
 *
 *  Copyright (C) 2001 Russell King.
 *
 * This file is placed under the LGPL.
 *
 * $Id: tslib.h,v 1.4 2005/02/26 01:47:23 kergoth Exp $
 *
 * Touch screen library interface definitions.
 */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <stdarg.h>
#include "wujique_log.h"


#ifndef ts_error
#define ts_error wjq_log
#endif
/*---------------*/
struct tsdev;

struct ts_sample //������һ������
{
	int		x;
	int		y;
	unsigned int	pressure;
	//struct timeval	tv;//ʱ�䣬��ֲ��STM32ƽ̨��Ӧ�ò���Ҫ
};

/*
	ʹ�����У׼������5��LCD��������������ݴ��룬�����7��У׼����
	ԭ������У׼�ļ��ģ���ֲ��ȫ������tslibģ��
*/
typedef struct {
	int x[5], xfb[5]; //x,y�Ǵ�������xfb��yfb�Ƕ�Ӧ��LCD����ֵ
	int y[5], yfb[5];
	unsigned int a[7];	//У׼�õ���7������
} calibration;


/*
 * Close the touchscreen device, free all resources.
 */
extern int ts_close(struct tsdev *);

/*
 * Configure the touchscreen device.
 */
extern int ts_config(struct tsdev *);

/*
 * Change this hook to point to your custom error handling function.
 */
extern int (*ts_error_fn)(const char *fmt, va_list ap);

/*
 * Returns the file descriptor in use for the touchscreen device.
 */
extern int ts_fd(struct tsdev *);

/*
 * Load a filter/scaling module
 */
extern int ts_load_module(struct tsdev *, const char *mod, const char *params);

/*
 * Open the touchscreen device.
 */
extern struct tsdev *ts_open(const char *dev_name, int nonblock);

/*
 * Return a scaled touchscreen sample.
 */
extern int ts_read(struct tsdev *, struct ts_sample *, int);

/*
 * Returns a raw, unscaled sample from the touchscreen.
 */
extern int ts_read_raw(struct tsdev *, struct ts_sample *, int);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _TSLIB_H_ */
