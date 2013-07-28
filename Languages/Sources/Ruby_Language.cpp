/*	$Id$

	Copyright 1996, 1997, 1998, 2002
	        Hekkelman Programmatuur B.V.  All rights reserved.
	Copyright 2012
	        Przemysław Buczkowski <przemub@yahoo.pl>. All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
	1. Redistributions of source code must retain the above copyright notice,
	   this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice,
	   this list of conditions and the following disclaimer in the documentation
	   and/or other materials provided with the distribution.
	3. All advertising materials mentioning features or use of this software
	   must display the following acknowledgement:

	    This product includes software developed by Hekkelman Programmatuur B.V.

	4. The name of Hekkelman Programmatuur B.V. may not be used to endorse or
	   promote products derived from this software without specific prior
	   written permission.

	THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
	FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
	AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
	OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Created: 14th December 2012 by Przemysław Buczkowski

*/

/*
 * This file provides syntax-highlighting for Ruby files.
 * This file is derived from Cpp_Language.cpp created by Maarten Hekkelman
 * and extended by Guy Haviv <mul_m7m@bezeqint.net>.
*/

#include <stack>
#include "CLanguageAddOn.h"
#include "HColorUtils.h"

extern "C" {
_EXPORT const char kLanguageName[] = "Ruby";
_EXPORT const char kLanguageExtensions[] = "rb;rbw";
_EXPORT const char kLanguageCommentStart[] = "#";
_EXPORT const char kLanguageCommentEnd[] = "";
_EXPORT const char kLanguageKeywordFile[] = "keywords.rb";
_EXPORT const int16 kInterfaceVersion = 2;
}

enum {
	START				= 0x00,
	IDENT				= 0x01,
	OTHER				= 0x02,
	COMMENT				= 0x03,
	VAR_INSTANCE		= 0x04,
	STRING				= 0x05,
	MULTI_STRING		= 0x06,
	NUMERIC				= 0x07,
	OPERATOR			= 0x08,
	SYMBOL				= 0x09,
	LEAVE				= 0x0a,
	PRAGMA2				= 0x0b,
	PRAGMA1				= 0x0c,
	INCL1				= 0x0d,
	INCL2				= 0x0e,
	INCL3				= 0x0f,

	STATE_MASK			= 0x1f,
	STATE_SHIFT			= 5
};

#define GETCHAR			(c = (i++ < size) ? text[i - 1] : 0)

bool isOperator(char c)
{
	if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '=' ||
		c == '>' ||	c == '<' || c == '&' || c == '|' || c == '!' || c == '.' ||
		c == '~' || c == '^' || c == '?' || c == ':')
		return true;

	return false;
}

bool isSymbol(char c)
{
	if (c=='{' || c=='}' || c=='(' || c==')' || c=='[' || c==']' || c==',' ||  c==';')
		return true;

	return false;
}

bool isNumeric(char c)
{
	if (c>='0' && c<='9')
		return true;

	return false;
}

bool isHexNum(char c)
{
	if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
		return true;

	return false;
}

static inline bool strings_equal(const char* string1, const char* string2,
	int len1, int len2)
{
	return (len1 == len2 && strncmp(string1, string2, len1) == 0);
}

bool find_comment_start(const char *text, int len, int& offset)
{
	for (int i = 0; i < len; i++)
	{
		if (text[i] == '#')
		{
			if (i + 1 < len)
			{
				return false;
			}
		}
	}
	return false;
}

_EXPORT void ColorLine(CLanguageProxy& proxy, int32& state)
{
	const char *text = proxy.Text();
	int32 size = proxy.Size();
	int32 i = 0, s = 0, kws = 0, esc = 0;
	char c;
	bool leave = false;
	// floating point flag, true when the NUMERIC: label finds a . inside a number, and checks to make sure that a number with two '.' is invalid. (and not highlighted as numeric)
	bool floating_point = false;
	// same flag, only for hex numbers. allows proper highlighting only for 1 x per number. (0x21 is ok. 0x023x31 is not. will look wierd.)
	bool hex_num = false;
	int32 ifZeroCounter = state >> STATE_SHIFT;
	state = state & STATE_MASK;
	if (state == COMMENT)
		proxy.SetColor(0, kColorComment1);
	else
		proxy.SetColor(0, kColorText);

	if (size <= 0)
		return;

	while (!leave)
	{
		GETCHAR;

		switch (state) {
			case START:
				s = i - 1;
				proxy.SetColor(s, kColorText);
				if (isalpha(c) || c == '_')
				{
					kws = proxy.Move(c, 1);
					state = IDENT;
				}
				else if (c == '#')
				{
					i++;
					state = COMMENT;
				}
				else if (c == '"' || c == '\'')
					state = STRING;
				else if (c == '[' && text[i] == '[') {
					i++;
					state = MULTI_STRING;
				}
				else if (c == '@') {
					state = VAR_INSTANCE;
				}
				// m7m: here are the 3 magic IFs.
				else if (isNumeric(c))
				{
					state = NUMERIC;
				}
				else if (isOperator(c))
				{
					state = OPERATOR;
				}
				else if (isSymbol(c))
				{
					state = SYMBOL;
				}
				else if (c == '\n' || c == 0)
					leave = true;
				break;
			case COMMENT:
				proxy.SetColor(s, kColorComment1);
				leave = true;
				if (text[size - 1] == '\n')
					state = START;
				break;

			case IDENT:
				if (!isalnum(c) && c != '_')
				{
					int kwc;

					if (i > s + 1 && (kwc = proxy.IsKeyword(kws)) != 0)
					{
						switch (kwc)
						{
							case 1:	proxy.SetColor(s, kColorKeyword1); break;
							case 2:	proxy.SetColor(s, kColorUserSet1); break;
							case 3:	proxy.SetColor(s, kColorUserSet2); break;
							case 4:	proxy.SetColor(s, kColorUserSet3); break;
							case 5:	proxy.SetColor(s, kColorUserSet4); break;
						}
					}
					else
						proxy.SetColor(s, kColorText);

					i--;
					state = START;
				}
				else if (kws)
					kws = proxy.Move((int)(unsigned char)c, kws);
				break;

			case PRAGMA1:
				if (c == ' ' || c == '\t')
					;
				else if (islower(c))
				{
					kws = proxy.Move((int)(unsigned char)c, kws);
					state = PRAGMA2;
				}
				else
				{
					proxy.SetColor(s, kColorText);
					i--;
					state = START;
				}
				break;

			case PRAGMA2:
				if (!islower(c))
				{
					int kwc;

					if (i > s + 2 && (kwc = proxy.IsKeyword(kws)) != 0)
					{
						switch (kwc)
						{
							case 1:	proxy.SetColor(s, kColorKeyword1); break;
							case 2:	proxy.SetColor(s, kColorUserSet1); break;
							case 3:	proxy.SetColor(s, kColorUserSet2); break;
							case 4:	proxy.SetColor(s, kColorUserSet3); break;
							case 5:	proxy.SetColor(s, kColorUserSet4); break;
						}
					}
					else
					{
						proxy.SetColor(s, kColorText);
					}
				}
				else if (kws)
					kws = proxy.Move((int)(unsigned char)c, kws);
				break;

			case INCL1:
				if (c == '"')
					state = INCL2;
				else if (c == '<')
					state = INCL3;
				else if (c != ' ' && c != '\t')
				{
					state = START;
					i--;
				}
				break;

			case INCL2:
				if (c == '"')
				{
					proxy.SetColor(s, kColorString1);
					state = START;
				}
				else if (c == '\n' || c == 0)
				{
					proxy.SetColor(s, kColorText);
					leave = true;
					state = START;
				}
				break;

			case INCL3:
				if (c == '>')
				{
					proxy.SetColor(s, kColorString1);
					state = START;
				}
				else if (c == '\n' || c == 0)
				{
					proxy.SetColor(s, kColorText);
					leave = true;
					state = START;
				}
				break;

			case STRING:
				if ((c == '"' || c == '\'') && !esc)
				{
					proxy.SetColor(s, kColorString1);
					state = START;
				}
				else if (c == '\n' || c == 0)
				{
					if (text[i - 2] == '\\' && text[i - 3] != '\\')
					{
						proxy.SetColor(s, kColorString1);
					}
					else
					{
						proxy.SetColor(s, kColorText);
						state = START;
					}
					leave = true;
				}
				else
					esc = !esc && (c == '\\');
				break;

			case MULTI_STRING:
				if ((s == 0 || i > s + 1) && c == ']' && text[i] == ']')
				{
					proxy.SetColor(s, kColorString2);
					i++;
					state = START;
				}
				else if (c == 0 || c == '\n')
				{
					proxy.SetColor(s, kColorString2);
					leave = true;
				}
				break;

			case VAR_INSTANCE:
				proxy.SetColor(s, kColorUserSet4);
				if(isOperator(text[i]) || isSymbol(text[i]) || c==' ')
					state = START;
				break;

			case NUMERIC:
			{
				proxy.SetColor(s, kColorNumber1);
				if (isNumeric(text[i-1]) || (hex_num && isHexNum(text[i - 1])))
					;
				else
					if (text[i-1]=='.' && c!='.' && floating_point==false && hex_num==false)
						floating_point = true;
					else if (text[i-1]=='x' && hex_num==false && floating_point==false)
						hex_num = true;
					else
					{
						i--;
						hex_num = false;
						state = START;
					}
			}
			break;

			case OPERATOR:
			{
				proxy.SetColor(s, kColorOperator1);
				if (isOperator(text[i-1]))
					;
				else
				{
					i--;
					state = START;
				}
			}
			break;

			case SYMBOL:
			{
				proxy.SetColor(s, kColorSeparator1);
				if (isSymbol(text[i-1]))
					;
				else
				{
					i--;
					state = START;
				}
			}
			break;

			default:	// error condition, gracefully leave the loop
				leave = true;
				break;
		}
	}
	state |= ifZeroCounter << STATE_SHIFT;
} /* ColorLine */
