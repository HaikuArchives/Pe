/*
 * Copyright 2024, Team Pe. All rights reserved.
 *
 * Same license as for most of Pe applies (BSD-style), as this is based on
 * pretty old code that keeps getting re-hashed over and over.
 *
 * Author(s):
 *		 Oscar Lesta <oscar.lesta@gmail.com>
 */

#include "CLanguageAddOn.h"
#include "HColorUtils.h"

const char kLanguageName[] = "CMake";
const char kLanguageExtensions[] = "cmake";
const char kLanguageCommentStart[] = "#";
const char kLanguageCommentEnd[] = "";
const char kLanguageKeywordFile[] = "keywords.cmake";
const int16 kInterfaceVersion = 2;

enum {
	START,
	IDENTIFIER,
	COMMENT,
	STRING,
	SYMBOL,
	NUMBER
};

static inline bool
isSymbol(char c)
{
	switch (c) {
		case '$':
			return true;
		case ':':
			return true;
		case '(':
			return true;
		case ')':
			return true;
		case '{':
			return true;
		case '}':
			return true;
		default:
			return false;
	}
}

_EXPORT void
ColorLine(CLanguageProxy& proxy, int32& state)
{
	int32 size = proxy.Size();

	if (size <= 0)
		return;

	if (state == COMMENT)
		proxy.SetColor(0, kColorComment1);
	else
		proxy.SetColor(0, kColorText);

	const char* text = proxy.Text();

	int32 i = 0, s = 0, kws = 0, esc = 0;
	char c;

	bool leave = false;

	while (!leave)
	{
		c = (i++ < size) ? text[i - 1] : 0;

		switch (state) {
			case START:
				s = i - 1;
				proxy.SetColor(s, kColorText);

				if (isalpha(c)) {
					kws = proxy.Move(c, 1);
					state = IDENTIFIER;
				}
				else if (c == '#')
					state = COMMENT;
				else if (c == '"')
					state = STRING;
				else if (isdigit(c))
					state = NUMBER;
				else if (isSymbol(c))
					state = SYMBOL;
				else if (c == '\n' || c == 0)
					leave = true;
			break;

			case COMMENT:
				proxy.SetColor(s, kColorComment1);
				leave = true;
				if (text[size - 1] == '\n')
					state = START;
			break;

			case IDENTIFIER:
				if (!isalnum(c) && c != '_') {
					int kwc;

					if (i > s + 1 && (kwc = proxy.IsKeyword(kws)) != 0) {
						switch (kwc) {
							case 1:	proxy.SetColor(s, kColorKeyword1); break;
							case 2:	proxy.SetColor(s, kColorUserSet1); break;
							case 3:	proxy.SetColor(s, kColorUserSet2); break;
							case 4:	proxy.SetColor(s, kColorUserSet3); break;
							case 5:	proxy.SetColor(s, kColorUserSet4); break;
						}
					}
					else {
						// kColorIdentifierUser would be good, but gets over used.
						proxy.SetColor(s, kColorText);
					}
					s = --i;
					state = START;
				}
				else if (kws)
					kws = proxy.Move(c, kws);
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

			case SYMBOL:
				proxy.SetColor(s, kColorOperator1);
				if (!isSymbol(text[i - 1])) {
					i--;
					state = START;
				}
			break;

			case NUMBER:
				proxy.SetColor(s, kColorNumber1);
				if (!(isdigit(text[i - 1]))) {
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
