/*
 *  tslib/src/ts_read.c
 *
 *  Copyright (C) 2001 Russell King.
 *
 * This file is placed under the LGPL.  Please see the file
 * COPYING for more details.
 *
 * $Id: ts_read.c,v 1.4 2004/07/21 19:12:59 dlowder Exp $
 *
 * Read raw pressure, x, y, and timestamp from a touchscreen device.
 */
#include "tslib-private.h"
/* This array is used to prevent segfaults and memory overwrites
 * that can occur if multiple events are returned from ts_read_raw
 * for each event returned by ts_read
 */
/* We found this was not needed, and have gone back to the
 * original implementation
 */

// static struct ts_sample ts_read_private_samples[1024];

int ts_read(struct tsdev *ts, struct ts_sample *samp, int nr)
{
	int result;

	result = ts->list->ops->read(ts->list, samp, nr);

	return result;

}

