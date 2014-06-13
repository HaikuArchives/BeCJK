/*
###                 Copyright (C) 1999-2000 TurboLinux, Inc. 
###                        All Rights Reserved
### Distributed under the terms of the GNU General Public License (GPL)
###
###
### Authors:     TurboLinux Chinese Development Team:
###              Justin Yu   <justiny@turbolinux.com.cn>
###              Sean Chen   <seanc@turbolinux.com.cn>
###              Daniel Fang <danf@turbolinux.com.cn>
###
### Modified by Anthony Lee <anthonylee@eLong.com> 2002
*/

#ifndef __CHINPUT_SYMBOL_H__
#define __CHINPUT_SYMBOL_H__

#include <SupportDefs.h>

#include "BeCJKInputModule.h"

typedef struct chinput_symbol{
	char	c;
	int		num;
	int 	cur;
	char	*s;
} chinput_symbol;


// must free by delete[]
char* chinput_get_corner_punct(char c, s_input_encoding encoding, bool corner, bool punct);

#endif /* __CHINPUT_SYMBOL_H__ */
