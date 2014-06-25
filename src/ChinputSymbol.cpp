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

#include <ctype.h>
#include <string.h>
#include <besavager/Strings.h>

#include "ChinputSymbol.h"

static chinput_symbol chinput_corner_gb[] = {
	{' ',		1,	0,	"　"},
	{'!',		1,	0,	"！"},
	{'"',		2,	0,	"“”"},
	{'#',		1,	0,	"＃"},
	{'$',		3,	0,	"￥＄￡"},
	{'%',		2,	0,	"％‰"},
	{'&',		1,	0,	"＆"},
	{'\'',		2,	0,	"‘’"},
	{'(',		3,	0,	"（「『"},
	{')',		3,	0,	"）」』"},
	{'*',		2,	0,	"×＊"},
	{'+',		3,	0,	"＋±∑"},
	{',',		1,	0,	"，"},
	{'-',		1,	0,	"—"},
	{'.',		1,	0,	"。"},
	{'/',		3,	0,	"÷／√"},
	{'0',		1,	0,	"０"},
	{'1',		1,	0,	"１"},
	{'2',		1,	0,	"２"},
	{'3',		1,	0,	"３"},
	{'4',		1,	0,	"４"},
	{'5',		1,	0,	"５"},
	{'6',		1,	0,	"６"},
	{'7',		1,	0,	"７"},
	{'8',		1,	0,	"８"},
	{'9',		1,	0,	"９"},
	{':',		1,	0,	"："},
	{';',		1,	0,	"；"},
	{'<',		5,	0,	"《〈＜≮≤"},
	{'=',		4,	0,	"＝≈≠≌"},
	{'>',		5,	0,	"》〉＞≯≥"},
	{'?',		1,	0,	"？"},
	{'@',		1,	0,	"＠"},
	{'A',		1,	0,	"Ａ"},
	{'B',		1,	0,	"Ｂ"},
	{'C',		1,	0,	"Ｃ"},
	{'D',		1,	0,	"Ｄ"},
	{'E',		1,	0,	"Ｅ"},
	{'F',		1,	0,	"Ｆ"},
	{'G',		1,	0,	"Ｇ"},
	{'H',		1,	0,	"Ｈ"},
	{'I',		1,	0,	"Ｉ"},
	{'J',		1,	0,	"Ｊ"},
	{'K',		1,	0,	"Ｋ"},
	{'L',		1,	0,	"Ｌ"},
	{'M',		1,	0,	"Ｍ"},
	{'N',		1,	0,	"Ｎ"},
	{'O',		1,	0,	"Ｏ"},
	{'P',		1,	0,	"Ｐ"},
	{'Q',		1,	0,	"Ｑ"},
	{'R',		1,	0,	"Ｒ"},
	{'S',		1,	0,	"Ｓ"},
	{'T',		1,	0,	"Ｔ"},
	{'U',		1,	0,	"Ｕ"},
	{'V',		1,	0,	"Ｖ"},
	{'W',		1,	0,	"Ｗ"},
	{'X',		1,	0,	"Ｘ"},
	{'Y',		1,	0,	"Ｙ"},
	{'Z',		1,	0,	"Ｚ"},
	{'[',		5,	0,	"〔［〖【｛"},
	{'\\',		1,	0,	"、"},
	{']',		5,	0,	"〕］〗】｝"},
	{'^',		3,	0,	"＾ˇ…"},
	{'_',		1,	0,	"＿"},
	{'`',		2,	0,	"｀＇"},
	{'a',		1,	0,	"ａ"},
	{'b',		1,	0,	"ｂ"},
	{'c',		1,	0,	"ｃ"},
	{'d',		1,	0,	"ｄ"},
	{'e',		1,	0,	"ｅ"},
	{'f',		1,	0,	"ｆ"},
	{'g',		1,	0,	"ｇ"},
	{'h',		1,	0,	"ｈ"},
	{'i',		1,	0,	"ｉ"},
	{'j',		1,	0,	"ｊ"},
	{'k',		1,	0,	"ｋ"},
	{'l',		1,	0,	"ｌ"},
	{'m',		1,	0,	"ｍ"},
	{'n',		1,	0,	"ｎ"},
	{'o',		1,	0,	"ｏ"},
	{'p',		1,	0,	"ｐ"},
	{'q',		1,	0,	"ｑ"},
	{'r',		1,	0,	"ｒ"},
	{'s',		1,	0,	"ｓ"},
	{'t',		1,	0,	"ｔ"},
	{'u',		1,	0,	"ｕ"},
	{'v',		1,	0,	"ｖ"},
	{'w',		1,	0,	"ｗ"},
	{'x',		1,	0,	"ｘ"},
	{'y',		1,	0,	"ｙ"},
	{'z',		1,	0,	"ｚ"},
	{'{',		1,	0,	"｛"},
	{'|',		3,	0,	"｜‖∥"},
	{'}',		1,	0,	"｝"},
	{'~',		2,	0,	"～￣"},
};

static chinput_symbol chinput_corner_big5[] = {
	{' ',		1,	0,	"　"},
	{'!',		1,	0,	"！"},
	{'"',		2,	0,	"“”"},
	{'#',		1,	0,	"＃"},
	{'$',		1,	0,	"＄"},
	{'%',		1,	0,	"％"},
	{'&',		1,	0,	"＆"},
	{'\'',		2,	0,	"‘’"},
	{'(',		3,	0,	"（「『"},
	{')',		3,	0,	"）」』"},
	{'*',		1,	0,	"＊"},
	{'+',		1,	0,	"＋"},
	{',',		1,	0,	"，"},
	{'-',		1,	0,	"─"},
	{'.',		1,	0,	"。"},
	{'/',		1,	0,	""},
	{'0',		1,	0,	"０"},
	{'1',		1,	0,	"１"},
	{'2',		1,	0,	"２"},
	{'3',		1,	0,	"３"},
	{'4',		1,	0,	"４"},
	{'5',		1,	0,	"５"},
	{'6',		1,	0,	"６"},
	{'7',		1,	0,	"７"},
	{'8',		1,	0,	"８"},
	{'9',		1,	0,	"９"},
	{':',		1,	0,	"："},
	{';',		1,	0,	"﹔"},
	{'<',		2,	0,	"《＜"},
	{'=',		1,	0,	"＝"},
	{'>',		2,	0,	"》＞"},
	{'?',		1,	0,	"？"},
	{'@',		1,	0,	"＠"},
	{'A',		1,	0,	"Ａ"},
	{'B',		1,	0,	"Ｂ"},
	{'C',		1,	0,	"Ｃ"},
	{'D',		1,	0,	"Ｄ"},
	{'E',		1,	0,	"Ｅ"},
	{'F',		1,	0,	"Ｆ"},
	{'G',		1,	0,	"Ｇ"},
	{'H',		1,	0,	"Ｈ"},
	{'I',		1,	0,	"Ｉ"},
	{'J',		1,	0,	"Ｊ"},
	{'K',		1,	0,	"Ｋ"},
	{'L',		1,	0,	"Ｌ"},
	{'M',		1,	0,	"Ｍ"},
	{'N',		1,	0,	"Ｎ"},
	{'O',		1,	0,	"Ｏ"},
	{'P',		1,	0,	"Ｐ"},
	{'Q',		1,	0,	"Ｑ"},
	{'R',		1,	0,	"Ｒ"},
	{'S',		1,	0,	"Ｓ"},
	{'T',		1,	0,	"Ｔ"},
	{'U',		1,	0,	"Ｕ"},
	{'V',		1,	0,	"Ｖ"},
	{'W',		1,	0,	"Ｗ"},
	{'X',		1,	0,	"Ｘ"},
	{'Y',		1,	0,	"Ｙ"},
	{'Z',		1,	0,	"Ｚ"},
	{'[',		3,	0,	"〔﹝【"},
	{'\\',		1,	0,	"、"},
	{']',		3,	0,	"〕﹞】"},
	{'^',		1,	0,	"…"},
	{'_',		1,	0,	""},
	{'`',		1,	0,	"‵"},
	{'a',		1,	0,	"ａ"},
	{'b',		1,	0,	"ｂ"},
	{'c',		1,	0,	"ｃ"},
	{'d',		1,	0,	"ｄ"},
	{'e',		1,	0,	"ｅ"},
	{'f',		1,	0,	"ｆ"},
	{'g',		1,	0,	"ｇ"},
	{'h',		1,	0,	"ｈ"},
	{'i',		1,	0,	"ｉ"},
	{'j',		1,	0,	"ｊ"},
	{'k',		1,	0,	"ｋ"},
	{'l',		1,	0,	"ｌ"},
	{'m',		1,	0,	"ｍ"},
	{'n',		1,	0,	"ｎ"},
	{'o',		1,	0,	"ｏ"},
	{'p',		1,	0,	"ｐ"},
	{'q',		1,	0,	"ｑ"},
	{'r',		1,	0,	"ｒ"},
	{'s',		1,	0,	"ｓ"},
	{'t',		1,	0,	"ｔ"},
	{'u',		1,	0,	"ｕ"},
	{'v',		1,	0,	"ｖ"},
	{'w',		1,	0,	"ｗ"},
	{'x',		1,	0,	"ｘ"},
	{'y',		1,	0,	"ｙ"},
	{'z',		1,	0,	"ｚ"},
	{'{',		1,	0,	"﹛"},
	{'|',		1,	0,	"︱"},
	{'}',		1,	0,	"﹜"},
	{'~',		1,	0,	"∼"},
};


static chinput_symbol chinput_corner_jp[] = {
	{' ',		1,	0,	"　"},
	{'!',		1,	0,	"！"},
	{'"',		2,	0,	"“”"},
	{'#',		1,	0,	"＃"},
	{'$',		1,	0,	"＄"},
	{'%',		1,	0,	"％"},
	{'&',		1,	0,	"＆"},
	{'\'',		2,	0,	"‘’"},
	{'(',		1,	0,	"（"},
	{')',		1,	0,	"）"},
	{'*',		1,	0,	"＊"},
	{'+',		1,	0,	"＋"},
	{',',		1,	0,	"、"},
	{'-',		1,	0,	"－"},
	{'.',		1,	0,	"。"},
	{'/',		1,	0,	"／"},
	{'0',		1,	0,	"０"},
	{'1',		1,	0,	"１"},
	{'2',		1,	0,	"２"},
	{'3',		1,	0,	"３"},
	{'4',		1,	0,	"４"},
	{'5',		1,	0,	"５"},
	{'6',		1,	0,	"６"},
	{'7',		1,	0,	"７"},
	{'8',		1,	0,	"８"},
	{'9',		1,	0,	"９"},
	{':',		1,	0,	"："},
	{';',		1,	0,	"；"},
	{'<',		1,	0,	"＜"},
	{'=',		1,	0,	"＝"},
	{'>',		1,	0,	"＞"},
	{'?',		1,	0,	"？"},
	{'@',		1,	0,	"＠"},
	{'A',		1,	0,	"Ａ"},
	{'B',		1,	0,	"Ｂ"},
	{'C',		1,	0,	"Ｃ"},
	{'D',		1,	0,	"Ｄ"},
	{'E',		1,	0,	"Ｅ"},
	{'F',		1,	0,	"Ｆ"},
	{'G',		1,	0,	"Ｇ"},
	{'H',		1,	0,	"Ｈ"},
	{'I',		1,	0,	"Ｉ"},
	{'J',		1,	0,	"Ｊ"},
	{'K',		1,	0,	"Ｋ"},
	{'L',		1,	0,	"Ｌ"},
	{'M',		1,	0,	"Ｍ"},
	{'N',		1,	0,	"Ｎ"},
	{'O',		1,	0,	"Ｏ"},
	{'P',		1,	0,	"Ｐ"},
	{'Q',		1,	0,	"Ｑ"},
	{'R',		1,	0,	"Ｒ"},
	{'S',		1,	0,	"Ｓ"},
	{'T',		1,	0,	"Ｔ"},
	{'U',		1,	0,	"Ｕ"},
	{'V',		1,	0,	"Ｖ"},
	{'W',		1,	0,	"Ｗ"},
	{'X',		1,	0,	"Ｘ"},
	{'Y',		1,	0,	"Ｙ"},
	{'Z',		1,	0,	"Ｚ"},
	{'[',		2,	0,	"［｢"},
	{'\\',		1,	0,	"＼"},
	{']',		2,	0,	"］｣"},
	{'^',		1,	0,	"＾"},
	{'_',		1,	0,	"＿"},
	{'`',		1,	0,	"｀"},
	{'a',		1,	0,	"ａ"},
	{'b',		1,	0,	"ｂ"},
	{'c',		1,	0,	"ｃ"},
	{'d',		1,	0,	"ｄ"},
	{'e',		1,	0,	"ｅ"},
	{'f',		1,	0,	"ｆ"},
	{'g',		1,	0,	"ｇ"},
	{'h',		1,	0,	"ｈ"},
	{'i',		1,	0,	"ｉ"},
	{'j',		1,	0,	"ｊ"},
	{'k',		1,	0,	"ｋ"},
	{'l',		1,	0,	"ｌ"},
	{'m',		1,	0,	"ｍ"},
	{'n',		1,	0,	"ｎ"},
	{'o',		1,	0,	"ｏ"},
	{'p',		1,	0,	"ｐ"},
	{'q',		1,	0,	"ｑ"},
	{'r',		1,	0,	"ｒ"},
	{'s',		1,	0,	"ｓ"},
	{'t',		1,	0,	"ｔ"},
	{'u',		1,	0,	"ｕ"},
	{'v',		1,	0,	"ｖ"},
	{'w',		1,	0,	"ｗ"},
	{'x',		1,	0,	"ｘ"},
	{'y',		1,	0,	"ｙ"},
	{'z',		1,	0,	"ｚ"},
	{'{',		1,	0,	"｛"},
	{'|',		1,	0,	"｜"},
	{'}',		1,	0,	"｝"},
	{'~',		1,	0,	"～"},
};


static chinput_symbol chinput_corner_ko[] = {
	{' ',		1,	0,	"　"},
	{'!',		1,	0,	"！"},
	{'"',		1,	0,	"＂"},
	{'#',		1,	0,	"＃"},
	{'$',		1,	0,	"＄"},
	{'%',		1,	0,	"％"},
	{'&',		1,	0,	"＆"},
	{'\'',		2,	0,	"‘’"},
	{'(',		1,	0,	"（"},
	{')',		1,	0,	"）"},
	{'*',		1,	0,	"＊"},
	{'+',		1,	0,	"＋"},
	{',',		1,	0,	"，"},
	{'-',		1,	0,	"－"},
	{'.',		1,	0,	"．"},
	{'/',		1,	0,	"／"},
	{'0',		1,	0,	"０"},
	{'1',		1,	0,	"１"},
	{'2',		1,	0,	"２"},
	{'3',		1,	0,	"３"},
	{'4',		1,	0,	"４"},
	{'5',		1,	0,	"５"},
	{'6',		1,	0,	"６"},
	{'7',		1,	0,	"７"},
	{'8',		1,	0,	"８"},
	{'9',		1,	0,	"９"},
	{':',		1,	0,	"："},
	{';',		1,	0,	"；"},
	{'<',		1,	0,	"＜"},
	{'=',		1,	0,	"＝"},
	{'>',		1,	0,	"＞"},
	{'?',		1,	0,	"？"},
	{'@',		1,	0,	"＠"},
	{'A',		1,	0,	"Ａ"},
	{'B',		1,	0,	"Ｂ"},
	{'C',		1,	0,	"Ｃ"},
	{'D',		1,	0,	"Ｄ"},
	{'E',		1,	0,	"Ｅ"},
	{'F',		1,	0,	"Ｆ"},
	{'G',		1,	0,	"Ｇ"},
	{'H',		1,	0,	"Ｈ"},
	{'I',		1,	0,	"Ｉ"},
	{'J',		1,	0,	"Ｊ"},
	{'K',		1,	0,	"Ｋ"},
	{'L',		1,	0,	"Ｌ"},
	{'M',		1,	0,	"Ｍ"},
	{'N',		1,	0,	"Ｎ"},
	{'O',		1,	0,	"Ｏ"},
	{'P',		1,	0,	"Ｐ"},
	{'Q',		1,	0,	"Ｑ"},
	{'R',		1,	0,	"Ｒ"},
	{'S',		1,	0,	"Ｓ"},
	{'T',		1,	0,	"Ｔ"},
	{'U',		1,	0,	"Ｕ"},
	{'V',		1,	0,	"Ｖ"},
	{'W',		1,	0,	"Ｗ"},
	{'X',		1,	0,	"Ｘ"},
	{'Y',		1,	0,	"Ｙ"},
	{'Z',		1,	0,	"Ｚ"},
	{'[',		1,	0,	"［"},
	{'\\',		1,	0,	"￦"},
	{']',		1,	0,	"］"},
	{'^',		1,	0,	"＾"},
	{'_',		1,	0,	"＿"},
	{'`',		1,	0,	"｀"},
	{'a',		1,	0,	"ａ"},
	{'b',		1,	0,	"ｂ"},
	{'c',		1,	0,	"ｃ"},
	{'d',		1,	0,	"ｄ"},
	{'e',		1,	0,	"ｅ"},
	{'f',		1,	0,	"ｆ"},
	{'g',		1,	0,	"ｇ"},
	{'h',		1,	0,	"ｈ"},
	{'i',		1,	0,	"ｉ"},
	{'j',		1,	0,	"ｊ"},
	{'k',		1,	0,	"ｋ"},
	{'l',		1,	0,	"ｌ"},
	{'m',		1,	0,	"ｍ"},
	{'n',		1,	0,	"ｎ"},
	{'o',		1,	0,	"ｏ"},
	{'p',		1,	0,	"ｐ"},
	{'q',		1,	0,	"ｑ"},
	{'r',		1,	0,	"ｒ"},
	{'s',		1,	0,	"ｓ"},
	{'t',		1,	0,	"ｔ"},
	{'u',		1,	0,	"ｕ"},
	{'v',		1,	0,	"ｖ"},
	{'w',		1,	0,	"ｗ"},
	{'x',		1,	0,	"ｘ"},
	{'y',		1,	0,	"ｙ"},
	{'z',		1,	0,	"ｚ"},
	{'{',		1,	0,	"｛"},
	{'|',		1,	0,	"｜"},
	{'}',		1,	0,	"｝"},
	{'~',		1,	0,	"～"},
};


static int nchinput_corner_gb = (sizeof(chinput_corner_gb)/sizeof(chinput_corner_gb[0]));
static int nchinput_corner_big5 = (sizeof(chinput_corner_big5)/sizeof(chinput_corner_big5[0]));
static int nchinput_corner_jp = (sizeof(chinput_corner_jp)/sizeof(chinput_corner_jp[0]));
static int nchinput_corner_ko = (sizeof(chinput_corner_ko)/sizeof(chinput_corner_ko[0]));


inline int in_table(char c, chinput_symbol *chinput_corner, int nchinput_corner)
{
	if(!chinput_corner || nchinput_corner <= 0) return -1;

	int i;

	for(i = 0; i< nchinput_corner; i++)
		if(chinput_corner[i].c == c) return i;

	return -1;
}

inline int in_alnum(char c, chinput_symbol *chinput_corner, int nchinput_corner)
{
	if(isalnum(c))
	{
		int item;
		item = in_table(c, chinput_corner, nchinput_corner);
		return item;
	}
	else
		return -1;
}


inline int in_punct(char c, chinput_symbol *chinput_corner, int nchinput_corner)
{
	if(ispunct(c) || isspace(c))
	{
		int item;
		item = in_table(c, chinput_corner, nchinput_corner);
		return item;
	}
	else
		return -1;
}


inline char* chinput_get_reply_corner_punct(char c, chinput_symbol *chinput_corner, int nchinput_corner)
{
	if(!chinput_corner || nchinput_corner <= 0) return NULL;

	int item;
	item = in_table(c, chinput_corner, nchinput_corner);
	if(item != -1)
	{
		if(chinput_corner[item].num == 1)
			return Sstrdup(chinput_corner[item].s);
		else
		{
			char* tmp_utf8 = NULL;
			const char *tmp1 = chinput_corner[item].s;
			const char *tmp2 = NULL;

			if(chinput_corner[item].cur < 0 ||
			   chinput_corner[item].cur >= chinput_corner[item].num)
			   	chinput_corner[item].cur = 0;

			for(int i = 0; i <= chinput_corner[item].cur; i++)
			{
				uint len;

				tmp2 = __utf8_next_char(tmp1);
												
				if(!*tmp2) len = strlen(tmp1);
				else len = strlen(tmp1) - strlen(tmp2);
												
				if(tmp_utf8) delete[] tmp_utf8;
				tmp_utf8 = Sstrdup(tmp1, len);
				tmp1 += len;

				if(!tmp_utf8) break;
				if(!*tmp2) break;
			}

			chinput_corner[item].cur ++;
			if(chinput_corner[item].cur >= chinput_corner[item].num) chinput_corner[item].cur = 0;
			
			return tmp_utf8;
		}
	}
	else
		return NULL;
}


inline char* chinput_get_reply_corner(char c, chinput_symbol *chinput_corner, int nchinput_corner)
{
	if(!chinput_corner || nchinput_corner <= 0) return NULL;

	int item;
	item = in_alnum(c, chinput_corner, nchinput_corner);
	if(item != -1)
		return Sstrdup(chinput_corner[item].s);
	else
		return NULL;
}


inline char* chinput_get_reply_punct(char c, chinput_symbol *chinput_corner, int nchinput_corner)
{
	if(!chinput_corner || nchinput_corner <= 0) return NULL;

	int item;
	item = in_punct(c, chinput_corner, nchinput_corner);
	if(item != -1)
	{
		if(chinput_corner[item].num == 1)
		{
			return Sstrdup(chinput_corner[item].s);
		}
		else
		{
			char* tmp_utf8 = NULL;
			const char *tmp1 = chinput_corner[item].s;
			const char *tmp2 = NULL;

			if(chinput_corner[item].cur < 0 ||
			   chinput_corner[item].cur >= chinput_corner[item].num)
			   	chinput_corner[item].cur = 0;

			for(int i = 0; i <= chinput_corner[item].cur; i++)
			{
				uint len;

				tmp2 = __utf8_next_char(tmp1);
												
				if(!*tmp2) len = strlen(tmp1);
				else len = strlen(tmp1) - strlen(tmp2);
												
				if(tmp_utf8) delete[] tmp_utf8;
				tmp_utf8 = Sstrdup(tmp1, len);
				tmp1 += len;

				if(!tmp_utf8) break;
				if(!*tmp2) break;
			}

			chinput_corner[item].cur++;
			if(chinput_corner[item].cur >= chinput_corner[item].num) chinput_corner[item].cur = 0;
			
			return tmp_utf8;
		}
	}
	else
		return NULL;
}


char* chinput_get_corner_punct(char c, s_input_encoding encoding, bool corner, bool punct)
{
	if(!(isalnum(c) || ispunct(c) || isspace(c))) return NULL;

	chinput_symbol *chinput_corner = NULL;
	int nchinput_corner = 0;

	switch(encoding)
	{
		case ENCODING_SIMPLIFIED_CHINESE:
			chinput_corner = chinput_corner_gb;
			nchinput_corner = nchinput_corner_gb;
			break;

		case ENCODING_TRADITIONAL_CHINESE:
			chinput_corner = chinput_corner_big5;
			nchinput_corner = nchinput_corner_big5;
			break;

		case ENCODING_JAPANESE:
			chinput_corner = chinput_corner_jp;
			nchinput_corner = nchinput_corner_jp;
			break;

		case ENCODING_KOREAN:
			chinput_corner = chinput_corner_ko;
			nchinput_corner = nchinput_corner_ko;
			break;

		default:
			chinput_corner = NULL;
			nchinput_corner = 0;
	}

	if(!chinput_corner || nchinput_corner <= 0 || (!corner && !punct))
	{
		char *ret = new char[2];
		if(!ret) return NULL;
		
		ret[0] = c;
		ret[1] = 0;

		return ret;
	}

	char *ret = NULL;

	if(corner && punct)
		ret = chinput_get_reply_corner_punct(c, chinput_corner, nchinput_corner);
	else if(corner)
		ret = chinput_get_reply_corner(c, chinput_corner, nchinput_corner);
	else if(punct)
		ret = chinput_get_reply_punct(c, chinput_corner, nchinput_corner);

	return ret;
}


