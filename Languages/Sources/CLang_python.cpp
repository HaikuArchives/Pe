/*	$Id$
	
	Copyright 1996, 1997, 1998, 2002
	        Hekkelman Programmatuur B.V.  All rights reserved.
	
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

	Created: 12/07/97 22:01:11 by Maarten Hekkelman
*/

#include "CLanguageAddOn.h"

_EXPORT const char kLanguageName[] = "Python";
_EXPORT const char kLanguageExtensions[] = "py";
_EXPORT const char kLanguageCommentStart[] = "#";
_EXPORT const char kLanguageCommentEnd[] = "";
_EXPORT const char kLanguageKeywordFile[] = "keywords.py";

enum {
	START, IDENT, OTHER, COMMENT, LCOMMENT, STRING1, STRING2, STRING3,
	CHAR_CONST, LEAVE, PRAGMA1, PRAGMA2, PRAGMA3
};

#define GETCHAR			(c = (i++ < size) ? text[i - 1] : 0)

_EXPORT void ColorLine(CLanguageProxy& proxy, int& state)
{
	const char *text = proxy.Text();
	int size = proxy.Size();
	int i = 0, s = 0, kws, esc = 0;
	char c;
	bool leave = false;
	
	switch (state)
	{
		case COMMENT:		proxy.SetColor(0, kLCommentColor);	break;
		case LCOMMENT:		proxy.SetColor(0, kLCommentColor);	break;
		case STRING3:			proxy.SetColor(0, kLStringColor);		break;
		default:					proxy.SetColor(0, kLTextColor);			break;
	}
	
	if (size <= 0)
		return;
	
	while (!leave)
	{
		GETCHAR;
		
		switch (state) {
			case START:
				if (c == '#')
					state = LCOMMENT;
				else if (isalpha(c) || c == '_')
				{
					kws = proxy.Move(c, 1);
					state = IDENT;
				}
				else if (c == '"')
				{
					if (text[i] == '"' && text[i + 1] == '"')
						state = COMMENT;
					else
						state = STRING1;
				}
				else if (c == '\'')
				{
					if (text[i] == '\'' && text[i + 1] == '\'')
						state = STRING3;
					else
						state = STRING2;
				}
				else if (c == '\n' || c == 0)
					leave = true;
					
				if (leave || (state != START && s < i))
				{
					proxy.SetColor(s, kLTextColor);
					s = i - 1;
				}
				break;
			
			case IDENT:
				if (!isalnum(c) && c != '_')
				{
					int kwc;

					if (i > s + 1 && (kwc = proxy.IsKeyWord(kws)) != 0)
					{
						switch (kwc)
						{
							case 1:	proxy.SetColor(s, kLKeyWordColor); break;
							case 2:	proxy.SetColor(s, kLUser1); break;
							case 3:	proxy.SetColor(s, kLUser2); break;
							case 4:	proxy.SetColor(s, kLUser3); break;
							case 5:	proxy.SetColor(s, kLUser4); break;
//							default:ASSERT(false);
						}
					}
					else
					{
						proxy.SetColor(s, kLTextColor);
					}
					
					s = --i;
					state = START;
				}
				else if (kws)
					kws = proxy.Move((int)(unsigned char)c, kws);
				break;
			
			case LCOMMENT:
				proxy.SetColor(s, kLCommentColor);
				leave = true;
				if (text[size - 1] == '\n')
					state = START;
				break;
			
			case STRING1:
				if (c == '"' && !esc)
				{
					proxy.SetColor(s, kLStringColor);
					s = i;
					state = START;
				}
				else if (c == '\n' || c == 0)
				{
					if (text[i - 2] == '\\' && text[i - 3] != '\\')
					{
						proxy.SetColor(s, kLStringColor);
					}
					else
					{
						proxy.SetColor(s, kLTextColor);
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
					proxy.SetColor(s, kLStringColor);
					s = i;
					state = START;
				}
				else if (c == '\n' || c == 0)
				{
					if (text[i - 2] == '\\' && text[i - 3] != '\\')
					{
						proxy.SetColor(s, kLStringColor);
					}
					else
					{
						proxy.SetColor(s, kLTextColor);
						state = START;
					}
					
					s = size;
					leave = true;
				}
				else
					esc = !esc && (c == '\\');
				break;
			
			case STRING3:
				if (c == '\'' && text[i] == '\'' && text[i + 1] == '\'' && !esc)
				{
					proxy.SetColor(s, kLStringColor);
					s = i + 2;
					state = START;
				}
				else if (c == '\n' || c == 0)
				{
					if (text[i - 2] == '\\' && text[i - 3] != '\\')
					{
						proxy.SetColor(s, kLStringColor);
					}
					else
					{
						proxy.SetColor(s, kLTextColor);
						state = START;
					}
					
					s = size;
					leave = true;
				}
				else
					esc = !esc && (c == '\\');
				break;
			
			case COMMENT:
				if (c == '"' && text[i] == '"' && text[i + 1] == '"')
				{
					proxy.SetColor(s, kLCommentColor);
					s = i + 2;
					state = START;
				}
				else if (c == '\n' || c == 0)
				{
					proxy.SetColor(s, kLCommentColor);
					leave = true;
				}
				break;
			
			default:	// error condition, gracefully leave the loop
				leave = true;
				break;
		}
	}
} /* ColorLine */
