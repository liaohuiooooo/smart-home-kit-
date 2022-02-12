/*
 *  tslib/plugins/linear.c
 *
 *  Copyright (C) 2001 Russell King.
 *
 * This file is placed under the LGPL.  Please see the file
 * COPYING for more details.
 *
 * $Id: linear.c,v 1.10 2005/02/26 01:47:23 kergoth Exp $
 *
 * Linearly scale touchscreen values
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "tslib.h"
#include "tslib-filter.h"


extern calibration cal;
/*
	ͨ��������ᷢ�֣�ÿһ��module�Ľṹ�壬
	���Ȼ����һ��ts��׼module��
	Ȼ���Ǳ�ģ��Ĳ���
*/
struct tslib_linear 
{
	struct tslib_module_info module;

	int	swap_xy;

// Linear scaling and offset parameters for pressure
	int	p_offset;
	int	p_mult;
	int	p_div;

// Linear scaling and offset parameters for x,y (can include rotation)
	int	a[7];
};
/*

	���������������������ʱ��Ҫ��
	
*/
#define NR_VARS (sizeof(linear_vars) / sizeof(linear_vars[0]))

struct tslib_linear Stm32TsLinear;

/*

	�����sampֻ�Ǹ����㻺�壬���治��Ҫ���ݣ�
	����ͨ��info�����read��������һ��ģ���ȡ

	nrֻ��ϣ����������ݣ�ʵ�ʴ�����٣����п��ܶ�������
*/
/**
 *@brief:      linear_read
 *@details:    У׼
 *@param[in]   struct tslib_module_info *info  
               struct ts_sample *samp          
               int nr                          
 *@param[out]  ��
 *@retval:     static
 */
static int linear_read(struct tslib_module_info *info, struct ts_sample *samp, int nr)
{
	struct tslib_linear *lin = (struct tslib_linear *)info;
	int ret;
	int xtemp,ytemp;

	// ����һ���ȡ����
	ret = info->next->ops->read(info->next, samp, nr);
	//����ret������
	if (ret >= 0) 
	{
		int nr;//��������һ��nr����?�б�ҪҲ��nr���������

		for (nr = 0; nr < ret; nr++, samp++) 
		{
			xtemp = samp->x; ytemp = samp->y;

			samp->x = 	( lin->a[2] +
					lin->a[0]*xtemp + 
					lin->a[1]*ytemp ) / lin->a[6];
			
			samp->y =	( lin->a[5] +
					lin->a[3]*xtemp +
					lin->a[4]*ytemp ) / lin->a[6];
			
			samp->pressure = ((samp->pressure + lin->p_offset)
						 * lin->p_mult) / lin->p_div;

			/*XY��Ե�*/
			if (lin->swap_xy) 
			{
				int tmp = samp->x;
				samp->x = samp->y;
				samp->y = tmp;
			}
		}
	}

	return ret;
}

static int linear_fini(struct tslib_module_info *info)
{
	return 0;
}

static const struct tslib_ops linear_ops =
{
	.read	= linear_read,
	.fini	= linear_fini,
};

/*

	linearû�в����������ã�������1

*/
static int linear_xyswap(struct tslib_module_info *inf, char *str, void *data)
{
	struct tslib_linear *lin = (struct tslib_linear *)inf;

	lin->swap_xy = 1;
	return 0;
}

/*

	linear ģ��Ĳ���

*/
static const struct tslib_vars linear_vars[] =
{
	{ "xyswap",	(void *)1, linear_xyswap }
};

/*
	��ֲ��stm32ƽ̨ʱ����mod_init������Ϊmod_linear_init��
	��Ƭ��Ƕ��ʽƽ̨������ͬ������ʱû�л��ƴ���

*/
struct tslib_module_info *mod_linear_init(struct tsdev *dev, const char *params)
{

	struct tslib_linear *lin;
	
	wjq_log(LOG_DEBUG, "mod_linear_init\r\n");

	lin = &Stm32TsLinear;//stm32ֱ�Ӷ���һ���ṹ�壬��ֵָ���lin

	lin->module.ops = &linear_ops;//������ʵ���ǰѱ�ģ���������غ�����ֵ��lin�ṹ��

	/*  �����ĸ����ݲ����޸ģ�  */
	lin->p_offset = 0;
	lin->p_mult   = 1;
	lin->p_div    = 1;
	lin->swap_xy  = 0;

	/*
	 �������ݾ��ǻ�ȡϵͳ��У׼����
	 */
	lin->a[0] = cal.a[0];
	lin->a[1] = cal.a[1];
	lin->a[2] = cal.a[2];
	lin->a[3] = cal.a[3];
	lin->a[4] = cal.a[4];
	lin->a[5] = cal.a[5];
	lin->a[6] = cal.a[6];
		
	/*
	 * Parse the parameters. ��������Ĳ���
	 */
	if (tslib_parse_vars(&lin->module, linear_vars, NR_VARS, params)) 
	{	
		return NULL;
	}
	
	wjq_log(LOG_DEBUG, "mod linear init ok\r\n");
	
	return &lin->module;
}

