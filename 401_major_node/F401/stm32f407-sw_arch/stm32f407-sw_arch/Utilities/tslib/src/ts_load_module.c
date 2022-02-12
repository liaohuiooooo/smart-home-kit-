/*
 *  tslib/src/ts_load_module.c
 *
 *  Copyright (C) 2001 Russell King.
 *
 * This file is placed under the LGPL.  Please see the file
 * COPYING for more details.
 *
 * $Id: ts_load_module.c,v 1.4 2004/10/19 22:01:27 dlowder Exp $
 *
 * Close a touchscreen device.
 */
#include <stdlib.h>
#include <string.h>

#include "tslib-private.h"

extern int ts_stm32_gei_init(char *name);

/**
 *@brief:      __ts_load_module
 *@details:    �������ֺͲ�������һ��ģ����ӵ�ts�豸
 *@param[in]   struct tsdev *ts    
               const char *module  
               const char *params  
               int raw             
 *@param[out]  ��
 *@retval:     
 */
int __ts_load_module(struct tsdev *ts, const char *module, const char *params, int raw)
{
	struct tslib_module_info * (*init)(struct tsdev *, const char *);
	struct tslib_module_info *info;

	void *handle;
	int ret;
	/*
		��module
		����ģ��ĳ�ʼ������ָ��
	*/
	init = (struct tslib_module_info * (*)(struct tsdev *, const char *))ts_stm32_gei_init((char *)module);
	if (!init) 
	{
		return -1;
	}
	/*
		ִ��mod_init����������ֵ��info
		���ص���ʵ�Ƕ�Ӧ��module�ṹ���һ����Ա��ָ��
		���磬dejitterģ�� struct tslib_dejitter ��һ�� module
	*/
	info = init(ts, params);
	if (!info) 
	{
		wjq_log(LOG_DEBUG, "__ts_load_module init params err\r\n");
		return -1;
	}
	/* ��ģ��ľ����ֵ��ģ�����涨���info�ṹ��handle���*/
	info->handle = handle;

	/*attach����ʵ���ǽ�module��ӵ�tsdev��������*/
	if (raw) 
	{
		ret = __ts_attach_raw(ts, info);
	} 
	else 
	{
		ret = __ts_attach(ts, info);
	}

	/*���ʧ�ܣ��ͷ�module*/
	if (ret) 
	{
		info->ops->fini(info);

	}

	return ret;
}

int ts_load_module(struct tsdev *ts, const char *module, const char *params)
{
	return __ts_load_module(ts, module, params, 0);
}

int ts_load_module_raw(struct tsdev *ts, const char *module, const char *params)
{
	return __ts_load_module(ts, module, params, 1);
}
