/*
 *  tslib/src/ts_config.c
 *
 *  Copyright (C) 2001 Russell King.
 *
 * This file is placed under the LGPL.  Please see the file
 * COPYING for more details.
 *
 * $Id: ts_config.c,v 1.5 2004/10/19 22:01:27 dlowder Exp $
 *
 * Read the configuration and load the appropriate drivers.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tslib-private.h"


/* Maximum line size is BUF_SIZE - 2 
 * -1 for fgets and -1 to test end of line
 */
#define BUF_SIZE 32

/*

	��TSLIBԴ���etcĿ¼���棬����һ��ts.conf���ļ���
	ts_config�������Ǹ�������ļ�������tsҪ�õ�ģ��(���)���������˲���Ĳ���

	module_raw input
	module pthres pmin=1
	module variance delta=30
	module dejitter delta=100
	module linear
	��ֲ��ʱ�����ǽ����conf������һ����ά����
*/
#define TsConfLine 4
/* ��������module��ʱ��������Ĭ��ֵ*/
const char TsConf[TsConfLine][BUF_SIZE]={
						{"module_raw stm32ts\n"},
						//{"module pthres pmin=1\n"},
						{"module variance delta=30\n"},
						{"module dejitter delta=100\n"},
						{"module linear\n"},
						
					};

/*
	��ΪSTM32 ���ܽ�һ��������һ��ģ�鶯̬���أ�û�������Ŀ�ܣ�
	���Խ�ģ���ʼ��������ģ�����ֶ����һ���б��������Խ�һ��ģ����ص���ʼ�б�
*/					
struct _ts_mod_init
{
	char *name;
	struct tslib_module_info *(*init)(struct tsdev *dev, const char *params);
};

extern struct tslib_module_info *mod_pthres_init(struct tsdev *dev, const char *params);
extern struct tslib_module_info *mod_variance_init(struct tsdev *dev, const char *params);
extern struct tslib_module_info *mod_dejitter_init(struct tsdev *dev, const char *params);
extern struct tslib_module_info *mod_linear_init(struct tsdev *dev, const char *params);
extern struct tslib_module_info *mod_stm32_init(struct tsdev *dev, const char *params);

const struct _ts_mod_init TsModInit[TsConfLine]=
{
	{"stm32ts", mod_stm32_init},
	{"linear", mod_linear_init},
	//{"pthres", mod_pthres_init},
	{"variance", mod_variance_init},
	{"dejitter", mod_dejitter_init},
	
};

int ts_stm32_gei_init(char *name)
{
	int i = 0;
	
	while(i < TsConfLine)
	{
		if(0 == strcmp(TsModInit[i].name, name))
		{
			return (int)TsModInit[i].init;
		}
		i++;
	}
	return -1;
}
/*
 * Get next token from string *stringp, where tokens are possibly-empty
 * strings separated by characters from delim.
 *
 * Writes NULs into the string at *stringp to end tokens.
 * delim need not remain constant from call to call.
 * On return, *stringp points past the last NUL written (if there might
 * be further tokens), or is NULL (if there are definitely no moretokens).
 *
 * If *stringp is NULL, strsep returns NULL.

���������linux����еģ���ֲ��STM32��ʱ��ŵ�����
 
 */
char *strsep(char **stringp, const char *delim)
{
    char *s;
    const char *spanp;
    int c, sc;
    char *tok;
    if ((s = *stringp)== NULL)
        return (NULL);
    for (tok = s;;) {
        c = *s++;
        spanp = delim;
        do {
            if ((sc =*spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *stringp = s;
                return (tok);
            }
        } while (sc != 0);
    }
    /* NOTREACHED */
}


int ts_config(struct tsdev *ts)
{
	char buf[BUF_SIZE], *p;
	int line = 0;
	int ret = 0;

	char *conffile = "TsConf";

	buf[BUF_SIZE - 2] = '\0';

	line = 0;
	while (line<TsConfLine) 
	{
		char *e;
		char *tok;
		char *module_name;

		/*���������ļ����ַ���*/
		memcpy(buf, &TsConf[line][0], BUF_SIZE);

		p = &buf[0];
		line++;

		/* Chomp */
		e = strchr(p, '\n');
		if (e) 
		{
			*e = '\0';
		}

		/* Did we read a whole line? */
		if (buf[BUF_SIZE - 2] != '\0') 
		{
			wjq_log(LOG_INFO, "%s: line %d too long\n", conffile, line);
			break;
		}

		tok = (char *)strsep(&p, " \t");
		
		/* Ignore comments or blank lines.
		 * Note: strsep modifies p (see man strsep)
		 */

		if (p==NULL || *tok == '#')
			continue;

		/*
			Search for the option. 
			ģ�������֣����ط�ʽ��ͬ
		*/
		if (strcmp(tok, "module") == 0) 
		{
			module_name = (char *)strsep(&p, " \t");
			ret = ts_load_module(ts, module_name, p);
		}
		else if (strcmp(tok, "module_raw") == 0) 
		{
			module_name = (char *)strsep(&p, " \t");
			ret = ts_load_module_raw(ts, module_name, p);
		} 
		else 
		{
			wjq_log(LOG_INFO, "%s: Unrecognised option %s:%d:%s\n", conffile, line, tok);
			break;
		}
		
		if (ret != 0) 
		{
			wjq_log(LOG_INFO, "Couldnt load module %s\n", module_name);
			break;
		}
	}

	if (ts->list_raw == NULL) //����Ҫ��һ��rawģ��
	{
		wjq_log(LOG_INFO, "No raw modules loaded.\n");
		ret = -1;
	}

	return ret;
}

