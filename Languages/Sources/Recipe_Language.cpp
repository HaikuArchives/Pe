/*	$Id$

	Copyright 1996, 1997, 1998, 2002
	        Hekkelman Programmatuur B.V.  All rights reserved.
	Copyright 2013
			Alvin Tan <tan.dejun.alvin@dhs.sg>. All rights reserved.

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

	Created: 21th November 2013 by Alvin
*/

/*
 * This file provides syntax-highlighting for Recipe files.
 * This file is derived from Shell_Language.cpp created by Maarten Hekkelman.
*/

#include "CLanguageAddOn.h"
#include "HColorUtils.h"

const char kLanguageName[] = "Recipe";
const char kLanguageExtensions[] = "recipe";
const char kLanguageCommentStart[] = "#";
const char kLanguageCommentEnd[] = "";
const char kLanguageKeywordFile[] = "keywords.recipe";
const int16 kInterfaceVersion = 2;

enum {
	START, IDENT, LCOMMENT, STRING1, STRING2
};

#define GETCHAR			(c = (i++ < size) ? text[i - 1] : 0)

_EXPORT void ColorLine(CLanguageProxy& proxy, int32& state)
{
	const char *text = proxy.Text();
	int32 size = proxy.Size();
	int32 i = 0, s = 0, kws = 0, esc = 0;
	char c = 0;
	bool leave = false;

	if (state == LCOMMENT)
		proxy.SetColor(0, kColorComment1);
	else
		proxy.SetColor(0, kColorText);

	if (size <= 0)
		return;

	while (!leave)
	{
		char b = c;
		GETCHAR;

		switch (state) {
			case START:
				if (c == '#' && (!b || isspace(b)))
					state = LCOMMENT;
				else if (isalpha(c) || c == '_')
				{
					kws = proxy.Move(c, 1);
					state = IDENT;
				}
				else if (c == '"')
					state = STRING1;
				else if (c == '\'')
					state = STRING2;
				else if (c == '\n' || c == 0)
					leave = true;

				if (leave || (state != START && s < i))
				{
					proxy.SetColor(s, kColorText);
					s = i - 1;
				}
				break;

			case LCOMMENT:
				proxy.SetColor(s, kColorComment1);
				leave = true;
				if (text[size - 1] == '\n')
					state = START;
				break;

			case IDENT:
				if (!isalnum(c) && c != '_')
				{
					int kwc;

					if (i >= s + 1 && (kwc = proxy.IsKeyword(kws)) != 0)
					{
						switch (kwc)
						{
							case 1:	proxy.SetColor(s, kColorKeyword1); break;
							case 2:	proxy.SetColor(s, kColorUserSet1); break;
							case 3:	proxy.SetColor(s, kColorUserSet2); break;
							case 4:	proxy.SetColor(s, kColorUserSet3); break;
							case 5:	proxy.SetColor(s, kColorUserSet4); break;
//							default:ASSERT(false);
						}
					}
					else
					{
						proxy.SetColor(s, kColorText);
					}

					s = --i;
					state = START;
				}
				else if (kws)
					kws = proxy.Move((int)(unsigned char)c, kws);
				break;

			case STRING1:
				if (c == '"' && !esc)
				{
					proxy.SetColor(s, kColorString1);
					s = i;
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

					s = size;
					leave = true;
				}
				else
					esc = !esc && (c == '\\');
				break;

			case STRING2:
				if (c == '\'' && !esc)
				{
					proxy.SetColor(s, kColorString1);
					s = i;
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

					s = size;
					leave = true;
				}
				else
					esc = !esc && (c == '\\');
				break;

			default:	// error condition, gracefully leave the loop
				leave = true;
				break;
		}
	}
} /* CshellIntf::ColorLine */
