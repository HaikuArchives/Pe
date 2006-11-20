/*	$Id: HtmlJs_Language.cpp 453 2006-11-19 02:22:01Z hoern $
	
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

#include <cctype>
#include <cstring>
#include <stack>
#include "CLanguageAddOn.h"
#include "HColorUtils.h"

using namespace std;

enum Language {
	lang_HTML,
	lang_JavaScript,
	lang_PHP
};

//template <class I1, class I2>
//inline int strncasecmp(I1 a, I2 b, int l)
//{
//	int d = 0;
//
//	while (l-- > 0 && d == 0)
//		d = tolower(*a++) - tolower(*b++);
//	
//	return d;
//}

_EXPORT const char kLanguageName[] = "HTML-JS-PHP";
_EXPORT const char kLanguageExtensions[] = "html;htm;php";
_EXPORT const char kLanguageCommentStart[] = "<!";
_EXPORT const char kLanguageCommentEnd[] = ">";
_EXPORT const char kLanguageKeywordFile[] = "keywords.html-js-php";

//	--- Available Colors ---
//	kColorText
//	kColorKeyword1,				kColorKeyword2,
//	kColorComment1,				kColorComment2,
//	kColorString1,				kColorString2,
//	kColorCharConst,
//	kColorTag,
//	kColorIdentifierSystem,	kColorIdentifierUser,
//	kColorNumber1,				kColorNumber2,
//	kColorPreprocessor1,		kColorPreprocessor2,
//	kColorError1,				kColorError2,
//	kColorOperator1,			kColorOperator2,
//	kColorSeparator1,			kColorSeparator2,
//	kColorUserSet1, kColorUserSet2, kColorUserSet3, kColorUserSet4,

// Translate from pepper-colors to pe-colors
enum {
	kLngColorHtmlKeyword	= kColorKeyword2,
	kLngColorHtmlAttribute	= kColorKeyword2,
	kLngColorJsKeyword		= kColorKeyword1,
	kLngColorPhpKeyword		= kColorKeyword1,
	kLngColorSpecialChar	= kColorUserSet4,
	kLngColorTagString		= kColorString1,
	kLngColorPhpFunction	= kColorUserSet4
};

enum {
	START = 0,
		TAGSTART,
		TAGSTARTKEYWORD,
		TAGENDKEYWORD,
		TAG,
		TAGSTRING1,
		TAGSTRING2,
		TAGATTRIBUTE,
		SPECIAL,
		COMMENT_DTD,
		COMMENT,
	JAVASCRIPT,
		JS_COMMENT,
		JS_LCOMMENT,
		JS_IDENTIFIER,
		JS_STRING1,
		JS_STRING2,
	PHP_SCRIPT,
		PHP_IDENTIFIER,
		PHP_COMMENT,
		PHP_LCOMMENT,
		PHP_STRING1,
		PHP_STRING2
};

enum {
	kKwHtmlTag			= 2,
	kKwHtmlAttribute	= 3,
	kKwJsLanguage		= 4,
	kKwJsConstants		= 5,	// TODO
	kKwPhpLanguage		= 6,
	kKwPhpFunctions		= 7,
	kKwPhpConstants		= 8,	// TODO
	kKwUserset1			= 9,
	kKwUserset2			= 10,
	kKwUserset3			= 11,
	kKwUserset4			= 12
};



#pragma mark Debug-Functions

#define DEBUG_PHP 0

#if DEBUG_PHP

void DEB_PrintState(const int state, const char c, const char *sub="")
{
	const char* name = NULL;
	switch (state)
	{
		case START:				name = "START"; break;
		case TAGSTART:			name = "TAGSTART"; break;
		case TAGSTARTKEYWORD:	name = "TAGSTARTKEYWORD"; break;
		case TAGENDKEYWORD:		name = "TAGENDKEYWORD"; break;
		case TAG:				name = "TAG"; break;
		case TAGSTRING1:		name = "TAGSTRING1"; break;
		case TAGSTRING2:		name = "TAGSTRING2"; break;
		case TAGATTRIBUTE:		name = "TAGATTRIBUTE"; break;
		case SPECIAL:			name = "SPECIAL"; break;
		case COMMENT_DTD:		name = "COMMENT_DTD"; break;
		case COMMENT:			name = "COMMENT"; break;
		case JAVASCRIPT:		name = "JAVASCRIPT"; break;
		case JS_COMMENT:		name = "JS_COMMENT"; break;
		case JS_LCOMMENT:		name = "JS_LCOMMENT"; break;
		case JS_IDENTIFIER:		name = "JS_IDENTIFIER"; break;
		case JS_STRING1:		name = "JS_STRING1"; break;
		case JS_STRING2:		name = "JS_STRING2"; break;
		case PHP_SCRIPT:		name = "PHP_SCRIPT"; break;
		case PHP_IDENTIFIER:	name = "PHP_IDENTIFIER"; break;
		case PHP_COMMENT:		name = "PHP_COMMENT"; break;
		case PHP_LCOMMENT:		name = "PHP_LCOMMENT"; break;
		case PHP_STRING1:		name = "PHP_STRING1"; break;
		case PHP_STRING2:		name = "PHP_STRING2"; break;
	}
	if (name)
	{
		printf("### %s(%s)", name, sub);
		printf(isprint(c) ? " [%c]" : " [0x%02X]", c);
		printf("\n");
	}
}

void DEB_PrintSetKw(const int state, int keyword)
{
	const char* name = NULL;
	switch (state)
	{
		case START:				name = "START"; break;
		case TAGSTART:			name = "TAGSTART"; break;
		case TAGSTARTKEYWORD:	name = "TAGSTARTKEYWORD"; break;
		case TAGENDKEYWORD:		name = "TAGENDKEYWORD"; break;
		case TAG:				name = "TAG"; break;
		case TAGSTRING1:		name = "TAGSTRING1"; break;
		case TAGSTRING2:		name = "TAGSTRING2"; break;
		case TAGATTRIBUTE:		name = "TAGATTRIBUTE"; break;
		case SPECIAL:			name = "SPECIAL"; break;
		case COMMENT_DTD:		name = "COMMENT_DTD"; break;
		case COMMENT:			name = "COMMENT"; break;
		case JAVASCRIPT:		name = "JAVASCRIPT"; break;
		case JS_COMMENT:		name = "JS_COMMENT"; break;
		case JS_LCOMMENT:		name = "JS_LCOMMENT"; break;
		case JS_IDENTIFIER:		name = "JS_IDENTIFIER"; break;
		case JS_STRING1:		name = "JS_STRING1"; break;
		case JS_STRING2:		name = "JS_STRING2"; break;
		case PHP_SCRIPT:		name = "PHP_SCRIPT"; break;
		case PHP_IDENTIFIER:	name = "PHP_IDENTIFIER"; break;
		case PHP_COMMENT:		name = "PHP_COMMENT"; break;
		case PHP_LCOMMENT:		name = "PHP_LCOMMENT"; break;
		case PHP_STRING1:		name = "PHP_STRING1"; break;
		case PHP_STRING2:		name = "PHP_STRING2"; break;
	}
	const char* kwname = NULL;
	switch (keyword)
	{
		case kKwHtmlTag:		kwname = "HtmlTag"; break;
		case kKwHtmlAttribute:	kwname = "HtmlAttribute"; break;
		case kKwJsLanguage:		kwname = "JsLanguage"; break;
		case kKwJsConstants:	kwname = "JsConstants"; break;
		case kKwPhpLanguage:	kwname = "PhpLanguage"; break;
		case kKwPhpFunctions:	kwname = "PhpFunctions"; break;
		case kKwPhpConstants:	kwname = "PhpConstants"; break;
		case kKwUserset1:		kwname = "Userset1"; break;
		case kKwUserset2:		kwname = "Userset2"; break;
		case kKwUserset3:		kwname = "Userset3"; break;
		case kKwUserset4:		kwname = "Userset4"; break;
	}
	//
	if (name)
	{
		if (kwname)
		{
			printf("    IsKeyword: <%s>\n", kwname);
		}
		else
		{
			printf("    IsKeyword: <%i>\n", keyword);
		}
	}
}

const char* DEB_StateName(int state)
{
	switch (state)
	{
		case START:				return "START";
		case TAGSTART:			return "TAGSTART";
		case TAGSTARTKEYWORD:	return "TAGSTARTKEYWORD";
		case TAGENDKEYWORD:		return "TAGENDKEYWORD";
		case TAG:				return "TAG";
		case TAGSTRING1:		return "TAGSTRING1";
		case TAGSTRING2:		return "TAGSTRING2";
		case TAGATTRIBUTE:		return "TAGATTRIBUTE";
		case SPECIAL:			return "SPECIAL";
		case COMMENT_DTD:		return "COMMENT_DTD";
		case COMMENT:			return "COMMENT";
		case JAVASCRIPT:		return "JAVASCRIPT";
		case JS_COMMENT:		return "JS_COMMENT";
		case JS_LCOMMENT:		return "JS_LCOMMENT";
		case JS_IDENTIFIER:		return "JS_IDENTIFIER";
		case JS_STRING1:		return "JS_STRING1";
		case JS_STRING2:		return "JS_STRING2";
		case PHP_SCRIPT:		return "PHP_SCRIPT";
		case PHP_IDENTIFIER:	return "PHP_IDENTIFIER";
		case PHP_COMMENT:		return "PHP_COMMENT";
		case PHP_LCOMMENT:		return "PHP_LCOMMENT";
		case PHP_STRING1:		return "PHP_STRING1";
		case PHP_STRING2:		return "PHP_STRING2";
		default:				return "UNKOWN";
	}
}

#else

	#define DEB_PrintState
	#define DEB_StateName
	#define DEB_PrintSetKw

#endif


#pragma mark ColorLine


#define GETCHAR  (c = (i++ < size) ? text[i - 1] : 0)

_EXPORT void ColorLine(CLanguageProxy& proxy, int& state)
{
	const char *text = proxy.Text();
	int size = proxy.Size();
	char c;
	int i = 0, bo = 0, s = 0, kws = 0, forceState = START;
	int color, kwc;
	bool leave = false, esc = false, script = false, percent = false;
	
	proxy.SetColor(0, kColorText);
	
	if (size <= 0)
		return;

	script = (state >= JAVASCRIPT);
	percent = false;

#if DEBUG_PHP
	printf("#############################################################################\nColorLine(%s:%s)\n%s\n", DEB_StateName(state), script ? "IsScript" : "NoScript", text);
#endif

	while (!leave)
	{
		GETCHAR;
		
		switch (state)
		{
			case START:
				DEB_PrintState(state, c);
				if (c == '<')
					state = TAGSTART;
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
			
			case TAGSTART:
				DEB_PrintState(state, c);
				if (c == '/')
					state = TAGENDKEYWORD;
				else if (c == '!')
				{
					state = COMMENT_DTD;
					forceState = START;
				}
				else if (c == '?' || c == '%')
				{
					proxy.SetColor(s, kColorTag);
					s = i;
					
					if (strncasecmp(text + s, "php", 3) == 0)
					{
						proxy.SetColor(s, kLngColorHtmlKeyword);
						s = (i += 3);
					}
					
					state = PHP_SCRIPT;
					percent = (c == '%');
				}
				else if (isalpha(c))
				{
					proxy.SetColor(s, kColorTag);
					s = i - 1;
					kws = proxy.Move(tolower(c), 1);
					state = TAGSTARTKEYWORD;
				}
				else if (c == 0 || c == '\n')
				{
					proxy.SetColor(s, kColorTag);
					leave = true;
				}
				else if (!isspace(c))
				{
					--i;
					state = TAG;
				}
				break;
			
			case TAGENDKEYWORD:
				DEB_PrintState(state, c);
				if (isalpha(c))
				{
					proxy.SetColor(s, kColorTag);
					s = i - 1;
					kws = proxy.Move(tolower(c), 1);
					state = TAGSTARTKEYWORD;
				}
				else if (c == 0 || c == '\n')
				{
					proxy.SetColor(s, kColorTag);
					leave = true;
				}
				else if (!isspace(c))
				{
					--i;
					state = TAG;
				}
				break;
			
			case TAGSTARTKEYWORD:
				DEB_PrintState(state, c);
				if (!isalnum(c) && c != '-')
				{
					switch (kwc = proxy.IsKeyword(kws))
					{
						case kKwHtmlTag:  color = kColorKeyword1; break;
						case kKwUserset1: color = kColorUserSet1; break;
						case kKwUserset2: color = kColorUserSet2; break;
						case kKwUserset3: color = kColorUserSet3; break;
						case kKwUserset4: color = kColorUserSet4; break;
						default:          color = kColorTag;      break;
					}
					proxy.SetColor(s, color);
					DEB_PrintSetKw(state, kwc);

					if (strncasecmp(text + s, "script", 6) == 0 && text[s - 1] != '/')
						script = true;

					state = TAG;
					s = --i;
				}
				else if (kws)
					kws = proxy.Move(tolower(c), kws);
				break;
			
			case TAG:
				DEB_PrintState(state, c);
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
						if (script)
							state = JAVASCRIPT;
						else
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
							state = COMMENT_DTD;
							forceState = START;
						}
						break;
					default:
						if (isalpha(c))
						{
							proxy.SetColor(s, kColorTag);
							s = i - 1;
							kws = proxy.Move(tolower(c), 1);
							state = TAGATTRIBUTE;
						}
						break;
				}
				break;
			
			case TAGSTRING1:
				DEB_PrintState(state, c);
				if (c == '"')
				{
					proxy.SetColor(s, kLngColorTagString);
					s = i;
					state = TAG;
				}
				else if (c == '\n' || c == 0)
				{
					proxy.SetColor(s, kLngColorTagString);
					leave = true;
				}
				break;
			
			case TAGSTRING2:
				DEB_PrintState(state, c);
				if (c == '\'')
				{
					proxy.SetColor(s, kLngColorTagString);
					s = i;
					state = TAG;
				}
				else if (c == '\n' || c == 0)
				{
					proxy.SetColor(s, kLngColorTagString);
					leave = true;
				}
				break;
			
			case TAGATTRIBUTE:
				DEB_PrintState(state, c);
				if (!isalnum(c) && c != '-')
				{
					switch (kwc = proxy.IsKeyword(kws))
					{
						case kKwHtmlAttribute: color = kLngColorHtmlAttribute; break;
						case kKwUserset1:      color = kColorUserSet1;         break;
						case kKwUserset2:      color = kColorUserSet2;         break;
						case kKwUserset3:      color = kColorUserSet3;         break;
						case kKwUserset4:      color = kColorUserSet4;         break;
						default:               color = kColorText;             break;
					}
					proxy.SetColor(s, color);
					DEB_PrintSetKw(state, kwc);

					s = --i;
					state = TAG;
				}
				else if (kws)
					kws = proxy.Move(tolower(c), kws);
				break;
			
			case SPECIAL:
				DEB_PrintState(state, c);
				if (c == 0 || c == '\n')
				{
					proxy.SetColor(s, kColorText);
					state = START;
					leave = true;
				}
				else if (c == ';')
				{
					proxy.SetColor(s, kLngColorSpecialChar);
					s = i;
					state = START;
				}
				else if (isspace(c))
					state = START;
				break;
			
			case COMMENT_DTD:
				DEB_PrintState(state, c);
				if (c == '-' && text[i] == '-' && i == s + 3 && text[i - 2] == '!' && text[i - 3] == '<')
				{
					proxy.SetColor(s, kColorTag);
					s = i - 1;
					state = COMMENT;
				}
				else if (c == '>')
				{
					proxy.SetColor(s, kColorTag);
					s = i;
					state = forceState;
				}
				else if (c == 0 || c == '\n')
				{
					proxy.SetColor(s, kColorTag);
					leave = true;
				}
				break;
				
			case COMMENT:
				DEB_PrintState(state, c);
				if (s == 0 && c == ':')
				{
					if (strncasecmp(text + i, "javascript", 10) == 0)
						forceState = JAVASCRIPT;
					else if (strncasecmp(text + i, "php", 3) == 0)
						forceState = PHP_SCRIPT;
				}
				else if (c == '-' && text[i] == '-')
				{
					proxy.SetColor(s, kColorComment1);
					s = ++i;
					state = COMMENT_DTD;
				}
				else if (c == 0 || c == '\n')
				{
					proxy.SetColor(s, kColorComment1);
					leave = true;
				}
				break;

			case JAVASCRIPT:
				DEB_PrintState(state, c);
				script = false;

				if (c == '<' && strncasecmp(text + i, "!--", 3) == 0)
				{
					proxy.SetColor(s, kColorTag);
					proxy.SetColor(i + 1, kColorComment1);
					i += 3;
					s = i;
				}
				else if (c == '<' && text[i] == '/')
				{
					if (strncasecmp(text + i + 1, "script", 6) == 0)
					{
						++i;
						state = TAGSTART;
					}
				}
				else if (c == '/' && text[i] == '*')
					state = JS_COMMENT;
				else if (c == '/' && text[i] == '/')
					state = JS_LCOMMENT;
				else if (isalpha(c))
				{
					kws = proxy.Move(tolower(c), 1);
					state = JS_IDENTIFIER;
				}
				else if (c == '\'')
					state = JS_STRING1;
				else if (c == '"')
					state = JS_STRING2;
				else if (c == 0 || c == '\n')
					leave = true;
					
				if ((leave || state != JAVASCRIPT) && s < i)
				{
					proxy.SetColor(s, kColorText);
					s = i - 1;
				}
				break;
				
			case JS_COMMENT:
				DEB_PrintState(state, c);
				if ((s == 0 || i > s + 1) && c == '*' && text[i] == '/')
				{
					proxy.SetColor(s, kColorComment1);
					s = i + 1;
					state = JAVASCRIPT;
				}
				else if (c == 0 || c == '\n')
				{
					proxy.SetColor(s, kColorComment1);
					leave = true;
				}
				break;

			case JS_LCOMMENT:
				DEB_PrintState(state, c);
				if (c == '-' && text[i] == '-')
				{
					proxy.SetColor(s, kColorComment1);
					s = i + 1;
					state = TAG;
				}
				else if (c == '\n' || c == 0)
				{
					proxy.SetColor(s, kColorComment1);
					state = JAVASCRIPT;
					leave = true;
				}
				break;

			case JS_IDENTIFIER:
				DEB_PrintState(state, c);
				if (!isalnum(c) && c != '_')
				{
					if (i > s + 1 && (kwc = proxy.IsKeyword(kws)) != 0)
					{
						switch (kwc)
						{
							case kKwJsLanguage:  color = kLngColorJsKeyword; break;
							case kKwUserset1:    color = kColorUserSet1; break;
							case kKwUserset2:    color = kColorUserSet2; break;
							case kKwUserset3:    color = kColorUserSet3; break;
							case kKwUserset4:    color = kColorUserSet4; break;
							default:             color = kColorText; break;
						}
						proxy.SetColor(s, color);
						DEB_PrintSetKw(state, kwc);
					}
					else
					{
						proxy.SetColor(s, kColorText);
					}
					
					s = --i;
					state = JAVASCRIPT;
				}
				else if (kws)
					kws = proxy.Move((int)(unsigned char)c, kws);
				break;
			
			case JS_STRING1:
			case JS_STRING2:
				DEB_PrintState(state, c);
				if (!esc &&
					((state == JS_STRING1 && c == '\'') ||
					(state == JS_STRING2 && c == '"')))
				{
					proxy.SetColor(s, kColorString1);
					s = i;
					state = JAVASCRIPT;
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
						state = JAVASCRIPT;
					}
					
					s = size;
					leave = true;
				}
				else
					esc = !esc && (c == '\\');
				break;

			case PHP_SCRIPT:
				DEB_PrintState(state, c);
				if (((c == '?' && !percent) || (c == '%' && percent)) && text[i] == '>')
				{
					proxy.SetColor(s, kColorTag);
					s = ++i;
					state = START;
				}
				else if (isalpha(c))
				{
					kws = proxy.Move(tolower(c), 1);
					state = PHP_IDENTIFIER;
				}
				else if (c == '"')
					state = PHP_STRING1;
				else if (c == '\'')
					state = PHP_STRING2;
				else if (c == '#' || (c == '/' && text[i] == '/'))
					state = PHP_LCOMMENT;
				else if (c == '/' && text[i] == '*')
					state = PHP_COMMENT;
				else if (c == 0 || c == '\n')
					leave = true;

				if ((leave || state != START) && s < i)
				{
					proxy.SetColor(s, kColorText);
					s = i - 1;
				}
				break;

			case PHP_IDENTIFIER:
				if (!isalnum(c) && c != '_')
				{
					if (i > s + 1 && (kwc = proxy.IsKeyword(kws)) != 0)
					{
						DEB_PrintState(state, c, "KWD");
						switch (kwc)
						{
							case kKwPhpLanguage:   color = kColorKeyword1; break;
							case kKwPhpFunctions:  color = kColorKeyword2; break;
							case kKwUserset1:      color = kColorUserSet1; break;
							case kKwUserset2:      color = kColorUserSet2; break;
							case kKwUserset3:      color = kColorUserSet3; break;
							case kKwUserset4:      color = kColorUserSet4; break;
							default:               color = kColorText;     break;
						}
						proxy.SetColor(s, color);
						DEB_PrintSetKw(state, kwc);
					}
					else
					{
						DEB_PrintState(state, c, "TXT");
						proxy.SetColor(s, kColorText);
					}
					
					s = --i;
					state = PHP_SCRIPT;
				}
				else if (kws)
					DEB_PrintState(state, c, "MOV");
					kws = proxy.Move(tolower(c), kws);
				break;

			case PHP_STRING1:
				DEB_PrintState(state, c);
				if (!esc && c == '"')
				{
					proxy.SetColor(s, kColorString1);
					s = i;
					state = PHP_SCRIPT;
				}
				else if (c == '\n' || c == 0)
				{
					proxy.SetColor(s, kColorString1);
					s = size;
					leave = true;
				}
				else
					esc = !esc && (c == '\\');
				break;

			case PHP_STRING2:
				DEB_PrintState(state, c);
				if (!esc && c == '\'')
				{
					proxy.SetColor(s, kColorString2);
					s = i;
					state = PHP_SCRIPT;
				}
				else if (c == '\n' || c == 0)
				{
					proxy.SetColor(s, kColorString2);
					s = size;
					leave = true;
				}
				else
					esc = !esc && (c == '\\');
				break;

			case PHP_COMMENT:
				DEB_PrintState(state, c);
				if ((s == 0 || i > s + 1) && c == '*' && text[i] == '/')
				{
					proxy.SetColor(s, kColorComment2);
					s = i + 1;
					state = PHP_SCRIPT;
				}
				else if (c == 0 || c == '\n')
				{
					proxy.SetColor(s, kColorComment2);
					leave = true;
				}
				else if (((percent && c == '%') || (!percent && c == '?')) &&
					text[i] == '>')
				{
					proxy.SetColor(s, kColorComment2);
					s = i - 1;
					proxy.SetColor(s, kColorTag);
					s = ++i;
					state = START;
				}
				break;

			case PHP_LCOMMENT:
				DEB_PrintState(state, c);
				if (c == 0 || c == '\n')
				{
					proxy.SetColor(s, kColorComment1);
					state = PHP_SCRIPT;
					leave = true;
				}
				else if (((percent && c == '%') || (!percent && c == '?')) &&
					text[i] == '>')
				{
					proxy.SetColor(s, kColorComment1);
					s = i - 1;
					proxy.SetColor(s, kColorTag);
					s = ++i;
					state = START;
				}
				break;

			default:	// error condition, gracefully leave the loop
				leave = true;
				break;
		}
	}
	
#if DEBUG_PHP
	printf("--> (%s:%s)\n", DEB_StateName(state), script ? "IsScript" : "NoScript");
#endif

//	if (script)
//		state |= 0x8000;
//	
//	if (percent)
//		state |= 0x4000;
} /* ColorLine */


#pragma mark FindNextWord


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
	
	while (state > 0 && i < proxy.Size())
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
//				case '/':
//				case '<':
				case '=':
//				case '>':
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


#pragma mark Balance


Language get_language_for_offset(const CLanguageProxy& proxy, int inOffset, int& outLangStart)
{
	const char *text = proxy.Text();
	int size = proxy.Size(), forceState = START;
	
	if (inOffset > size)
		inOffset = size;
	
	char c = 0;
	int i = 0, bo = 0, s = 0;
	bool leave = false, esc = false, script = false, percent = false;
	int state = 0;
	
	while (i < inOffset)
	{
		GETCHAR;
		
		switch (state)
		{
			case START:
				if (c == '<')
					state = TAGSTART;
				else if (c == '&')
					state = SPECIAL;
				else if (c == 0 || c == '\n')
					leave = true;
					
				if ((leave || state != START) && s < i)
					s = i - 1;
				break;
			
			case TAGSTART:
				if (c == '/')
					state = TAGENDKEYWORD;
				else if (c == '!')
				{
					state = COMMENT_DTD;
					forceState = START;
				}
				else if (c == '?' || c == '%')
				{
					s = i;
					outLangStart = s;
					
					if (strncasecmp(text + s, "php", 3) == 0)
						s = (i += 3);
					
					state = PHP_SCRIPT;
					percent = (c == '%');
				}
				else if (isalpha(c))
				{
					s = i - 1;
					state = TAGSTARTKEYWORD;
				}
				else if (c == 0 || c == '\n')
				{
					leave = true;
				}
				else if (!isspace(c))
				{
					--i;
					state = TAG;
				}
				break;
			
			case TAGENDKEYWORD:
				if (isalpha(c))
				{
					s = i - 1;
					state = TAGSTARTKEYWORD;
				}
				else if (c == 0 || c == '\n')
				{
					leave = true;
				}
				else if (!isspace(c))
				{
					--i;
					state = TAG;
				}
				break;
			
			case TAGSTARTKEYWORD:
				if (!isalnum(c) && c != '-')
				{
					if (strncasecmp(text + s, "script", 6) == 0 && text[s - 1] != '/')
						script = true;

					state = TAG;
					s = --i;
				}
				break;
			
			case TAG:
				switch (c)
				{
					case 0:
					case '\n':
						leave = true;
						break;
					case '>':
						s = i;
						if (script)
						{
							outLangStart = s;
							state = JAVASCRIPT;
						}
						else
							state = START;
						break;
					case '"':
						s = i - 1;
						state = TAGSTRING1;
						break;
					case '\'':
						s = i - 1;
						state = TAGSTRING2;
						break;
					case '!':
						if (i == s + 2)
						{
							state = COMMENT_DTD;
							forceState = START;
						}
						break;
					default:
						if (isalpha(c))
						{
							s = i - 1;
							state = TAGATTRIBUTE;
						}
						break;
				}
				break;
			
			case TAGSTRING1:
				if (c == '"')
				{
					s = i;
					state = TAG;
				}
				else if (c == '\n' || c == 0)
					leave = true;
				break;
			
			case TAGSTRING2:
				if (c == '\'')
				{
					s = i;
					state = TAG;
				}
				else if (c == '\n' || c == 0)
					leave = true;
				break;
			
			case TAGATTRIBUTE:
				if (!isalnum(c) && c != '-')
				{
					s = --i;
					state = TAG;
				}
				break;
			
			case SPECIAL:
				if (c == 0 || c == '\n')
				{
					state = START;
					leave = true;
				}
				else if (c == ';')
				{
					s = i;
					state = START;
				}
				else if (isspace(c))
					state = START;
				break;
			
			case COMMENT_DTD:
				if (c == '-' && text[i] == '-' && i == s + 3 && text[i - 2] == '!' && text[i - 3] == '<')
				{
					s = i - 1;
					forceState = START;
					state = COMMENT;
				}
				else if (c == '>')
				{
					s = i;
					state = forceState;
				}
				else if (c == 0 || c == '\n')
				{
					leave = true;
				}
				break;
				
			case COMMENT:
				if (c == ':')
				{
					if (strncasecmp(text, "JavaScript", 10) == 0)
						forceState = JAVASCRIPT;
					else if (strncasecmp(text, "PHP", 3) == 0)
						forceState = PHP_SCRIPT;
				}
				else if (c == '-' && text[i] == '-')
				{
					s = ++i;
					state = COMMENT_DTD;
				}
				else if (c == 0 || c == '\n')
				{
					leave = true;
				}
				break;

			case JAVASCRIPT:
				script = false;

				if (c == '<' && strncasecmp(text + i, "!--", 3) == 0)
				{
					i += 3;
					s = i;
				}
				else if (c == '<' && text[i] == '/')
				{
					if (strncasecmp(text + i + 1, "script", 6) == 0)
					{
						++i;
						state = TAGSTART;
						outLangStart = i - 1;
					}
				}
				else if (c == '/' && text[i] == '*')
					state = JS_COMMENT;
				else if (c == '/' && text[i] == '/')
					state = JS_LCOMMENT;
				else if (isalpha(c))
					state = JS_IDENTIFIER;
				else if (c == '\'')
					state = JS_STRING1;
				else if (c == '"')
					state = JS_STRING2;
				else if (c == 0 || c == '\n')
					leave = true;
					
				if ((leave || state != JAVASCRIPT) && s < i)
					s = i - 1;
				break;
				
			case JS_COMMENT:
				if ((s == 0 || i > s + 1) && c == '*' && text[i] == '/')
				{
					s = i + 1;
					state = JAVASCRIPT;
				}
				else if (c == 0 || c == '\n')
					leave = true;
				break;

			case JS_LCOMMENT:
				if (c == '-' && text[i] == '-')
				{
					s = i + 1;
					state = TAG;
					outLangStart = s;
				}
				else if (c == '\n' || c == 0)
				{
					state = JAVASCRIPT;
					leave = true;
				}
				break;

			case JS_IDENTIFIER:
				if (!isalnum(c) && c != '_')
				{
					s = --i;
					state = JAVASCRIPT;
				}
				break;
			
			case JS_STRING1:
			case JS_STRING2:
				if (!esc &&
					((state == JS_STRING1 && c == '\'') ||
					(state == JS_STRING2 && c == '"')))
				{
					s = i;
					state = JAVASCRIPT;
				}
				else if (c == '\n' || c == 0)
				{
					if (!(text[i - 2] == '\\' && text[i - 3] != '\\'))
						state = JAVASCRIPT;
					
					s = size;
					leave = true;
				}
				else
					esc = !esc && (c == '\\');
				break;
			
			case PHP_SCRIPT:
				if (((c == '?' && !percent) || (c == '%' && percent)) &&
					text[i] == '>')
				{
					s = ++i;
					state = START;
					outLangStart = s;
				}
				else if (isalpha(c))
				{
					state = PHP_IDENTIFIER;
				}
				else if (c == '"')
					state = PHP_STRING1;
				else if (c == '\'')
					state = PHP_STRING2;
				else if (c == '#' || (c == '/' && text[i] == '/'))
					state = PHP_LCOMMENT;
				else if (c == '/' && text[i] == '*')
					state = PHP_COMMENT;
				else if (c == 0 || c == '\n')
					leave = true;

				if ((leave || state != START) && s < i)
					s = i - 1;
				break;

			case PHP_IDENTIFIER:
				if (!isalnum(c) && c != '_')
				{
					s = --i;
					state = PHP_SCRIPT;
				}
				break;

			case PHP_STRING1:
				if (!esc && c == '"')
				{
					s = i;
					state = PHP_SCRIPT;
				}
				else if (c == '\n' || c == 0)
				{
					s = size;
					leave = true;
				}
				else
					esc = !esc && (c == '\\');
				break;

			case PHP_STRING2:
				if (!esc && c == '\'')
				{
					s = i;
					state = PHP_SCRIPT;
				}
				else if (c == '\n' || c == 0)
				{
					s = size;
					leave = true;
				}
				else
					esc = !esc && (c == '\\');
				break;

			case PHP_COMMENT:
				if ((s == 0 || i > s + 1) && c == '*' && text[i] == '/')
				{
					s = i + 1;
					state = PHP_SCRIPT;
				}
				else if (c == 0 || c == '\n')
				{
					leave = true;
				}
				else if (((percent && c == '%') || (!percent && c == '?')) &&
					text[i] == '>')
				{
					s = i - 1;
					s = ++i;
					state = START;
					outLangStart = s;
				}
				break;

			case PHP_LCOMMENT:
				if (c == 0 || c == '\n')
				{
					state = PHP_SCRIPT;
					leave = true;
				}
				else if (((percent && c == '%') || (!percent && c == '?')) &&
					text[i] == '>')
				{
					s = i - 1;
					s = ++i;
					state = START;
					outLangStart = s;
				}
				break;

			default:	// error condition, gracefully leave the loop
				leave = true;
				break;
		}
		
		if (leave)
		{
			while (c != '\n' && c != 0 && i < inOffset)
				GETCHAR;
			
			s = i;
			leave = false;
		}
	}
	
	if (state >= JAVASCRIPT && state <= JS_STRING2)
		return lang_JavaScript;
	else if (state >= PHP_SCRIPT && state <= PHP_STRING2)
		return lang_PHP;
	else
		return lang_HTML;
} /* get_language_for_offset */

const char *skip_in(const char *txt)
{
	if (*txt == '!' && txt[1] == '-' && txt[2] == '-')
	{
		txt += 3;
		
		while (*txt && ! (txt[0] == '-' && txt[1] != '-'))
			txt++;
	}

	while (*txt && *txt != '>')
		txt++;
	
	return txt;
} // skip_in

const char *skip_out(const char *txt)
{
	while (*txt && *txt != '<')
		txt++;
	
	return txt;
} // skip_out

static const char *skip(const char *txt)
{
	while (*txt)
	{
		switch (*txt)
		{
			case '\'':
				while (*++txt)
				{
					if (*txt == '\'')
						break;
					if (*txt == '\\' && txt[1])
						txt++;
				}
				break;
			
			case '"':
				while (*++txt)
				{
					if (*txt == '"')
						break;
					if (*txt == '\\' && txt[1])
						txt++;
				}
				break;
				
			case '/':
				if (txt[1] == '*')
				{
					txt += 2;
					while (*txt && !(*txt == '*' && txt[1] == '/'))
						txt++;
				}
				else if (txt[1] == '/')
				{
					txt += 2;
					while (*txt && *txt != '\n')
						txt++;
				}
				break;
			
			case '{':
			case '[':
			case '(':
			case ')':
			case ']':
			case '}':
				return txt;
		}
		txt++;
	}
	
	return txt;
} // skip

_EXPORT bool Balance(CLanguageProxy& proxy, int& start, int& end)
{
	int langStart;
	Language lang = get_language_for_offset(proxy, start, langStart);
	const char *txt = proxy.Text();
	int size = proxy.Size();
	
	if (lang == lang_HTML)
	{
		bool in = false;
		
		if (start < 0 || start > end || end > size)
			return false;
		
		const char *st = txt + start;
		
		while (txt < st)
		{
			const char *t = in ? skip_in(txt + 1) : skip_out(txt + 1);
			in = !in;
			
			if (*t && st < t)
			{
				start = txt - proxy.Text() + 1;
				end = t - proxy.Text();
				return true;
			}
	
			txt = t;
		}
	}
	else
	{
		if (start < 0 || start > end || end > size)
			return false;
		
		const char *et(txt + end);
		const char *st(txt + langStart);
		
		stack<int> bls, sbls, pls;
		
		while (*txt && txt < et)
		{
			switch (*txt)
			{
				case '{':	bls.push(txt - st);				break;
				case '[':	sbls.push(txt - st);			break;
				case '(':	pls.push(txt - st);				break;
				case '}':	if (!bls.empty()) bls.pop();	break;
				case ']':	if (!sbls.empty()) sbls.pop();	break;
				case ')':	if (!pls.empty()) pls.pop();	break;
			}
			txt = skip(txt + 1);
		}
		
		char ec = 0, oc = 0;
		stack<int> *s = 0;
		
		int db, dsb, dp;
		
		db = bls.empty() ? -1 : start - bls.top();
		dsb = sbls.empty() ? -1 : start - sbls.top();
		dp = pls.empty() ? -1 : start - pls.top();
		
		if (db < 0 && dsb < 0 && dp < 0)
			return false;
		
		if (db >= 0 && (dsb < 0 || db < dsb) && (dp < 0 || db < dp))
		{
			oc = '{';
			ec = '}';
			s = &bls;
		}
		
		if (dsb >= 0 && (db < 0 || dsb < db) && (dp < 0 || dsb < dp))
		{
			oc= '[';
			ec = ']';
			s = &sbls;
		}
		
		if (dp >= 0 && (dsb < 0 || dp < dsb) && (db < 0 || dp < db))
		{
			oc = '(';
			ec = ')';
			s = &pls;
		}
		
		if (ec)
		{
			int l = 1;
			
			while (*txt)
			{
				if (*txt == ec)
				{
					if (--l == 0)
					{
						start = s->top() + 1 + langStart;
						end = txt - st + langStart;
						return true;
					}
					if (!s->empty()) s->pop();
				}
				else if (*txt == oc)
				{
					l++;
					s->push(0);
				}
	
				txt = skip(txt + 1);
			}
		}
	}
	
	return false;
} /* Balance */
