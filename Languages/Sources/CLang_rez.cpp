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

_EXPORT const char kLanguageName[] = "Rez";
_EXPORT const char kLanguageExtensions[] = "r;rez";
_EXPORT const char kLanguageCommentStart[] = "//";
_EXPORT const char kLanguageCommentEnd[] = "";
_EXPORT const char kLanguageKeywordFile[] = "keywords.rez";

enum {
	START, IDENT, OTHER, COMMENT, LCOMMENT, STRING,
	CHAR_CONST, LEAVE, PRAGMA1, PRAGMA2, PRAGMA3,
	FUNCTION
};

#define GETCHAR			(c = (i++ < size) ? text[i - 1] : 0)

_EXPORT void ColorLine(CLanguageProxy& proxy, int& state)
{
	const char *text = proxy.Text();
	int size = proxy.Size();
	int i = 0, s = 0, kws, cc_cnt, esc = 0;
	char c;
	bool leave = false;
	
	if (state == COMMENT || state == LCOMMENT)
		proxy.SetColor(0, kLCommentColor);
	else
		proxy.SetColor(0, kLTextColor);
	
	if (size <= 0)
		return;
	
	while (!leave)
	{
		GETCHAR;
		
		switch (state) {
			case START:
				if (c == '#')
				{
					kws = proxy.Move((int)(unsigned char)c, 1);
					state = PRAGMA1;
				}
				else if (isalpha(c) || c == '_')
				{
					kws = proxy.Move((int)(unsigned char)tolower(c), 1);
					state = IDENT;
				}
				else if (c == '/' && text[i] == '*')
					state = COMMENT;
				else if (c == '/' && text[i] == '/')
					state = LCOMMENT;
				else if (c == '"')
					state = STRING;
				else if (c == '\'')
				{
					state = CHAR_CONST;
					cc_cnt = 0;
				}
				else if (c == '$' && text[i] == '$')
				{
					GETCHAR;
					kws = 1;
					state = FUNCTION;
				}
				else if (c == '\n' || c == 0)
					leave = true;
					
				if (leave || (state != START && s < i))
				{
					proxy.SetColor(s, kLTextColor);
					s = i - 1;
				}
				break;
			
			case COMMENT:
				if ((s == 0 || i > s + 1) && c == '*' && text[i] == '/')
				{
					proxy.SetColor(s, kLCommentColor);
					s = i + 1;
					state = START;
				}
				else if (c == 0 || c == '\n')
				{
					proxy.SetColor(s, kLCommentColor);
					leave = true;
				}
				break;

			case LCOMMENT:
				proxy.SetColor(s, kLCommentColor);
				leave = true;
				if (text[size - 1] == '\n')
					state = START;
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
//							case 2:	proxy.SetColor(s, kLUser1); break;
							case 3:	proxy.SetColor(s, kLUser2); break;
							case 4:	proxy.SetColor(s, kLUser3); break;
							case 5:	proxy.SetColor(s, kLUser4); break;
//							default:	ASSERT(false);
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
					kws = proxy.Move((int)(unsigned char)tolower(c), kws);
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
					proxy.SetColor(s, kLTextColor);
					s = --i;
					state = START;
				}	
				break;
			
			case PRAGMA2:
				if (!islower(c))
				{
					int kwc;

					if (i > s + 2 && (kwc = proxy.IsKeyWord(kws)) != 0)
					{
						switch (kwc)
						{
							case 1:	proxy.SetColor(s, kLKeyWordColor); break;
//							case 2:	proxy.SetColor(s, kLUser1); break;
							case 3:	proxy.SetColor(s, kLUser2); break;
							case 4:	proxy.SetColor(s, kLUser3); break;
							case 5:	proxy.SetColor(s, kLUser4); break;
//							default:	ASSERT(false);
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
			
			case STRING:
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
			
			case CHAR_CONST:
				if (c == '\t' || c == '\n' || c == 0)	// don't like this
				{
					proxy.SetColor(s, kLTextColor);
					s = i;
					state = START;
				}
				else if (c == '\'' && !esc)
				{
					if (cc_cnt != 1 && cc_cnt != 2 && cc_cnt != 4)
					{
						proxy.SetColor(s, kLTextColor);
						s = --i;
						state = START;
					}
					else
					{
						proxy.SetColor(s, kLCharConstColor);
						s = i;
						state = START;
					}
				}
				else
				{
					if (!esc) cc_cnt++;
					esc = !esc && (c == '\\');
				}
				break;
			
			case FUNCTION:
				if (!isalnum(c))
				{
					int kwc;

					if (i > s + 1 && (kwc = proxy.IsKeyWord(kws)) == 2)
						proxy.SetColor(s - 1, kLUser1);
					else
						proxy.SetColor(s - 1, kLTextColor);
					
					s = --i;
					state = START;
				}
				else if (kws)
					kws = proxy.Move((int)(unsigned char)tolower(c), kws);
				break;
			
			default:	// error condition, gracefully leave the loop
				leave = true;
				break;
		}
	}
} /* ColorLine */

