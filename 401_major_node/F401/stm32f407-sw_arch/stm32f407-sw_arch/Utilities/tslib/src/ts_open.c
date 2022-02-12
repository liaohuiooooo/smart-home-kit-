/*
 *  tslib/src/ts_open.c
 *
 *  Copyright (C) 2001 Russell King.
 *
 * This file is placed under the LGPL.  Please see the file
 * COPYING for more details.
 *
 * $Id: ts_open.c,v 1.4 2004/07/21 19:12:59 dlowder Exp $
 *
 * Open a touchscreen device.
 */
#include <stdlib.h>
#include <string.h>
#include "tslib-private.h"

struct tsdev Stm32TslibDev;//ԭ����ͨ��malloc���룬��ֲ��ʱ���޸�Ϊֱ�Ӷ���һ���豸��
/**
 *@brief:      ts_open
 *@details:    ��һ��TS�豸
 *@param[in]   const char *name  
               int nonblock      
 *@param[out]  ��
 *@retval:     struct

 ��linux�����£���ʵ��������һ��tsdev����
 */
struct tsdev *ts_open(const char *name, int nonblock)
{
	struct tsdev *ts;

	ts = &Stm32TslibDev;

	memset(ts, 0, sizeof(struct tsdev));
	
	ts->fd = 0;//���豸���صľ��
	
	return ts;

}


