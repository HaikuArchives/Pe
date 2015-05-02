/*
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
*/

#include "CLanguageAddOn.h"
#include "HColorUtils.h"


extern "C" {
	_EXPORT const char	kLanguageName[]			= "Rust";
	_EXPORT const char	kLanguageExtensions[]	= "rs";
	_EXPORT const char	kLanguageCommentStart[]	= "//";
	_EXPORT const char	kLanguageCommentEnd[]	= "";
	_EXPORT const char	kLanguageKeywordFile[]	= "keywords.rs";
	_EXPORT const int16	kInterfaceVersion		= 2;
}


enum {
	START		= 0x00,
	IDENTIFIER,
	COMMENT,			// "/* */"	Block Comments
	LCOMMENT,			// "//"		Line Comments
	STRING,				// Double Qouted strings: "Some\tString"
	CHAR_CONST,
	NUMERIC,
	OPERATOR,
	SYMBOL,
	SPECIAL_TOKEN		// Special Token Sequence
};


static inline bool
isOperator(char c)
{
	switch (c) {
		case '-': return true;
		case '*': return true;
		case '!': return true;
		case '+': return true;
		case '/': return true;
		case '%': return true;
		case '&': return true;
		case '|': return true;
		case '^': return true;
		case '<': return true;
		case '>': return true;
		default: return false;
	}
}


static inline bool
isSymbol(char c)
{
	switch (c) {
		case '(': return true;
		case ')': return true;
		case ',': return true;
		case ':': return true;
		case ';': return true;
		case '[': return true;
		case ']': return true;
		case '{': return true;
		case '}': return true;
		default: return false;
	}
}



_EXPORT void
ColorLine(CLanguageProxy& proxy, int32& state)
{
	int32 size = proxy.Size();

	if (size <= 0)
		return;

	if (state == COMMENT) {
		proxy.SetColor(0, kColorComment1);
	} else {
		proxy.SetColor(0, kColorText);
	}

	const char* text = proxy.Text();

	int32 i = 0, s = 0, kws = 0, cc_cnt = 0, esc = 0;
	char c;

	bool leave = false;
	bool floating_point = false;
	bool hex_num = false;
	bool oct_num = false;
	bool bin_num = false;
	bool special_token_string = false;

	while (!leave)
	{
		c = (i++ < size) ? text[i - 1] : 0;

		switch (state) {
			case START:
				s = i - 1;
				proxy.SetColor(s, kColorText);

				if (isalpha(c) || c == '_') {
					kws = proxy.Move(c, 1);
					state = IDENTIFIER;
				}
				else if (c == '/' && text[i] == '*') {
					i++;
					state = COMMENT;
				}
				else if (c == '/' && text[i] == '/') {
					i++;
					state = LCOMMENT;
				}
				else if (c == '"') {
					state = STRING;
				}
				else if (c == '\'') {
					state = CHAR_CONST;
					cc_cnt = 0;
				}
				else if (isdigit(c)) {
					state = NUMERIC;
				}
				else if (isOperator(c)) {
					state = OPERATOR;
				}
				else if (isSymbol(c)) {
					state = SYMBOL;
				}
				else if (c == '#' && (text[i] == '[' || text[i] == '!')) {
					state = SPECIAL_TOKEN;
				} else if (c == '\n' || c == 0) {
					leave = true;
				}
			break;

			case COMMENT:
				if ((s == 0 || i > s + 1) && c == '*' && text[i] == '/') {
					proxy.SetColor(s, kColorComment1);
					i++;
					state = START;
				} else if (c == 0 || c == '\n') {
					proxy.SetColor(s, kColorComment1);
					leave = true;
				}
			break;

			case LCOMMENT:
				proxy.SetColor(s, kColorComment1);
				leave = true;
				if (text[size - 1] == '\n')
					state = START;
			break;
 
			case IDENTIFIER:
				if (!isalnum(c) && c != '_') {
					int32 kwc;

					if (i > s + 1 && (kwc = proxy.IsKeyword(kws)) != 0) {
						switch (kwc) {
							case 1:	proxy.SetColor(s, kColorKeyword1); break;
							case 2:	proxy.SetColor(s, kColorUserSet1); break;
							case 3:	proxy.SetColor(s, kColorUserSet2); break;
							case 4:	proxy.SetColor(s, kColorUserSet3); break;
							case 5:	proxy.SetColor(s, kColorUserSet4); break;
						}
					} else {
						proxy.SetColor(s, kColorText);
					}

					i--;
					state = START;
				} else if (kws) {
					kws = proxy.Move((int)(unsigned char) c, kws);
				}
			break;

			case SPECIAL_TOKEN:
				if (c == '\n' || c == 0) {
					proxy.SetColor(s, kColorPreprocessor1);
					state = START;
					leave = true;
				} else if (c == '"' && !special_token_string) {
					proxy.SetColor(s, kColorPreprocessor1);
					s = i - 1;
					special_token_string = true;
				} else if (c == '"' && special_token_string) {
					proxy.SetColor(s, kColorString1);
					s = i - 1;
					special_token_string = false;
				}
			break;

			case STRING:
				if (c == '"' && !esc) {
					proxy.SetColor(s, kColorString1);
					state = START;
				} else if (c == '\n' || c == 0) {
					if (text[i - 2] == '\\' && text[i - 3] != '\\') {
						proxy.SetColor(s, kColorString1);
					} else {
						proxy.SetColor(s, kColorText);
						state = START;
					}
					leave = true;
				}
				else
					esc = !esc && (c == '\\');
			break;

			case CHAR_CONST:
				if (c == '\t' || c == '\n' || c == 0 ||
					(c == '\'' && !esc && (cc_cnt == 0 || cc_cnt > 8)))
				{
					// invalid char constant - either invalid char or too short/long
					proxy.SetColor(s, kColorText);
					state = START;
				} else if (c == '\'' && !esc) {
					proxy.SetColor(s, kColorCharConst);
					state = START;
				} else {
					if (!esc)
						cc_cnt++;

					esc = !esc && (c == '\\');
				}
			break;

			case NUMERIC:
				proxy.SetColor(s, kColorNumber1);
				if (!(isdigit(text[i - 1]) || (hex_num && isxdigit(text[i - 1]))))
				{
					if (text[i - 1] == '.' && !floating_point && !hex_num && !oct_num && !bin_num)
						floating_point = true;
					else if (text[i - 1] == 'x' && !floating_point && !hex_num && !oct_num && !bin_num)
						hex_num = true;
					else if (text[i - 1] == 'o' && !floating_point && !hex_num && !oct_num && !bin_num)
						oct_num = true;
					else if (text[i - 1] == 'b' && !floating_point && !hex_num && !oct_num && !bin_num)
						bin_num = true;
					else if (text[i - 1] == '_')
						break;
					else {
						i--;
						hex_num = false;
						oct_num = false;
						bin_num = false;
						state = START;
					}
				}
			break;

			case OPERATOR:
				proxy.SetColor(s, kColorOperator1);
				if (!isOperator(text[i - 1])) {
					i--;
					state = START;
				}
			break;

			case SYMBOL:
				proxy.SetColor(s, kColorSeparator1);
				if (!isSymbol(text[i - 1])) {
					i--;
					state = START;
				}
			break;

			default:
				leave = true;
			break;
		}
	}
}
