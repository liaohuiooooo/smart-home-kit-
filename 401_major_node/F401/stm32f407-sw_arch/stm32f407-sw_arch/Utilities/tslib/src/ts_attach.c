/*
 *  tslib/src/ts_attach.c
 *
 *  Copyright (C) 2001 Russell King.
 *
 * This file is placed under the LGPL.  Please see the file
 * COPYING for more details.
 *
 * $Id: ts_attach.c,v 1.3 2004/10/19 22:01:27 dlowder Exp $
 *
 * Attach a filter to a touchscreen device.
 */
#include <stdlib.h>
#include "tslib-private.h"

/*
	将module插入ts链表，插入方式是从头插入。
*/
int __ts_attach(struct tsdev *ts, struct tslib_module_info *info)
{
	info->dev = ts;
	info->next = ts->list;
	ts->list = info;

	return 0;
}

int __ts_attach_raw(struct tsdev *ts, struct tslib_module_info *info)
{
	struct tslib_module_info *next, *prev, *prev_list = ts->list_raw;
	info->dev = ts;
	info->next = prev_list;
	ts->list_raw = info;

	/* 
	 * ensure the last item in the normal list now points to the
	 * top of the raw list.
	 */

	if (ts->list == NULL || ts->list == prev_list) 
	{ /* main list is empty, ensure it points here */
		ts->list = info;
		return 0;
	}

	for(next = ts->list, prev=next;
	    next != NULL && next != prev_list;
	    next = prev->next, prev = next);

	prev->next = info;
	return 0;
}

