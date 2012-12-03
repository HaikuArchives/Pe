/*	Xml_Language.cpp
	
	Copyright 1996, 1997, 1998, 2002
	        Hekkelman Programmatuur B.V.  All rights reserved.       
	Copyright 2012
	        Przemysław Buczkowski <przemub@yahoo.pl> All rights reserved.
		
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

	Created: 3rd December 2012 by Przemysław Buczkowski
*/

/*
 * This file provides syntax-highlighting for XML files.
 * This file is derived from Html_Language.cpp created by Maarten Hekkelman.
 */

#include "CLanguageAddOn.h"
#include "HColorUtils.h"

extern "C" {
_EXPORT const char  kLanguageName[] = "XML";
_EXPORT const char  kLanguageExtensions[] = "xml";
_EXPORT const char  kLanguageCommentStart[] = "<!";
_EXPORT const char  kLanguageCommentEnd[] = ">";
_EXPORT const char  kLanguageKeywordFile[] = "keywords.xml";
_EXPORT const int16 kInterfaceVersion = 2;
}

enum {
	START,
	TAG,
	TAGSTRING1,
	TAGSTRING2,
	TAGKEYWORD,
	SPECIAL,
	COMMENT
};

#define GETCHAR			(c = (i++ < size) ? text[i - 1] : 0)

_EXPORT void ColorLine(CLanguageProxy& proxy, int& state)
{
	const char *text = proxy.Text();
	int size = proxy.Size();
	char c;
	int i = 0, s = 0, kws = 0;
	bool leave = false;
	
	proxy.SetColor(0, kColorText);
	
	if (size <= 0)
		return;
	
	while (!leave)
	{
		GETCHAR;
		
		switch (state) {
			case START:
				if (c == '<')
					state = TAG;
				else if (c == '&')
					state = SPECIAL;
				else if (c == 0 || c == '\n')
					leave = true;
					
				if ((leave || state != START) && s < i)
				{
					proxy.SetColor(s, kColorText);
					s = i - 1;
				}
				break;
				
			case TAG:
				switch (c)
				{
					case 0:
					case '\n':
						proxy.SetColor(s, kColorTag);
						leave = true;
						break;
					case '>':
						proxy.SetColor(s, kColorTag);
						s = i;
						proxy.SetColor(s, kColorText);
						state = START;
						break;
					case '"':
						proxy.SetColor(s, kColorTag);
						s = i - 1;
						state = TAGSTRING1;
						break;
					case '\'':
						proxy.SetColor(s, kColorTag);
						s = i - 1;
						state = TAGSTRING2;
						break;
					case '!':
						if (i == s + 2)
						{
							proxy.SetColor(s, kColorTag);
							state = COMMENT;
						}
					default:
						if (isalpha(c))
						{
							proxy.SetColor(s, kColorTag);
							s = i - 1;
							kws = proxy.Move(tolower(c), 1);
							state = TAGKEYWORD;
						}
						break;
				}
				break;
			
			case TAGSTRING1:
				if (c == '"')
				{
					proxy.SetColor(s, kColorString2);
					s = i;
					state = TAG;
				}
				else if (c == '\n' || c == 0)
				{
					proxy.SetColor(s, kColorString2);
					leave = true;
				}
				break;
			
			case TAGSTRING2:
				if (c == '\'')
				{
					proxy.SetColor(s, kColorString2);
					s = i;
					state = TAG;
				}
				else if (c == '\n' || c == 0)
				{
					proxy.SetColor(s, kColorString2);
					leave = true;
				}
				break;
			
			case TAGKEYWORD:
				if (! isalnum(c))
				{
					proxy.SetColor(s, kColorKeyword1);
					s = --i;
					state = TAG;
				}
				else if (kws)
					kws = proxy.Move(tolower(c), kws);
				break;
			
			case SPECIAL:
				if (c == 0 || c == '\n')
				{
					proxy.SetColor(s, kColorText);
					state = START;
					leave = true;
				}
				else if (c == ';')
				{
					proxy.SetColor(s, kColorCharConst);
					s = i;
					state = START;
				}
				else if (isspace(c))
					state = START;
				break;
				
			case COMMENT:
				if (c == 0 || c == '\n')
				{
					proxy.SetColor(s, kColorComment1);
					leave = true;
				}
				else if (c == '>')
				{
					proxy.SetColor(s, kColorComment1);
					s = i;
					proxy.SetColor(s, kColorText);
					state = START;
					break;
				}
				break;
			// error condition, gracefully leave the loop
			default:
				leave = true;
				break;
		}
	}
} /* ColorLine */

const unsigned char kWordWrapTable[] =
	{
		0x84, 0x85, 0x80, 0x00, 0x83, 0x83,
		0x00, 0x02, 0x80, 0x00, 0x00, 0x00,
		0x00, 0x86, 0x00, 0x00, 0x83, 0x00,
		0x00, 0x86, 0x00, 0x00, 0x00, 0x00,
		0x84, 0x85, 0x80, 0x00, 0x83, 0x83,
		0x84, 0x86, 0x00, 0x00, 0x83, 0x83
	};

int FindNextWord(const CLanguageProxy& proxy)
{
	int mark = 0, i = 0;
	int unicode, state, len;
	
	state = 1;
	
	while (state && i < proxy.Size())
	{
		proxy.CharInfo(proxy.Text() + i, unicode, len);
		
		int cl = 0;
		
		if (unicode == '\n')
			cl = 3;
		else if (proxy.isspace_uc(unicode))
			cl = 2;
		else if (proxy.isalnum_uc(unicode))
			cl = 4;
		else
			switch (unicode)
			{
				case 160:
				case 8199:
				case 8209:
					cl = 1;
					break;
				case '&':
				case '*':
				case '+':
				case '-':
				case '=':
				case '\\':
				case '^':
				case '|':
					cl = 5;
					break;
				default:
					cl = 4;
			}
		
		unsigned char t = kWordWrapTable[(state - 1) * 6 + cl];

		state = t & 0x7f;

		if (t & 0x80)
			mark = i + len - 1;

		i += len;
	}

	return mark;
} /* FindNextWord */
