/*	$Id: ChtmlPopup.cpp 6 2002-06-05 14:39:23Z maarten $
	
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

	Created: 04/19/98 19:30:25
*/

/* TODO:
	- AddInclude() for include, include_once, require and require_once
	- Support for x='attribute'
	- PHP class support
*/
#include "CLanguageAddOn.h"
#include <string>

const long kMaxNameSize = 256;

const char *skip(const char *txt, int ch);
const char *skip_nc(const char *txt, int ch);
const char *Anchor(const char *txt, CLanguageProxy& ao);
const char *Heading(const char *txt, CLanguageProxy& ao);
const char *JavaScript(const char *txt, CLanguageProxy& ao);
const char *PhpScript(const char *txt, CLanguageProxy& ao);

const char *skip(const char *txt, int ch)
{
	while (*txt)
	{
		if (*txt == ch)
		{
			txt++;
			break;
		}
		txt++;
	}

	return txt;
} /* skip */

const char *skip_nc(const char *txt, int ch)
{
	while (*txt)
	{
		if (*txt == ch)
		{
			txt++;
			break;
		}
		else if (*txt == '"')
			txt = skip(txt + 1, '"');
		else
			txt++;
	}

	return txt;
} /* skip_nc */

const char *block(const char *txt, int open)
{
	int close = 0;
	switch (open)
	{
		case '(':
			close = ')';
			break;
		case '{':
			close = '}';
			break;
		default:
			assert(false);
	}
	
	while (*txt)
	{
		if (*txt == open)
			txt = block(txt + 1, open);
		else if (*txt == close)
			return txt + 1;
		else if (*txt == '"')
		{
			while (*++txt)
			{
				if (*txt == '\\')
					++txt;
				else if (*txt == '"')
					break;
			}
			++txt;
		}
		else
			++txt;
	}
	
	return txt;
}

inline const char *skip_white(const char *txt)
{
	while (isspace(*txt))
		txt++;
	return txt;
} /* skip_white */

bool get_attribute(const char *&ptr, const char *&namBeg, int& namLen, const char *&valBeg, int& valLen)
{
	const char *namEnd=0, *valEnd=0;

	namBeg = valBeg = 0;
	ptr = skip_white(ptr);
	if (*ptr != 0 && *ptr != '>')
	{
		// Search for attribue name
		namBeg = namEnd = ptr;
		// Gracefully also accept non alphanumeric characters
		while (*namEnd != 0 && *namEnd != '=' && !isspace(*namEnd))
			namEnd++;
		ptr = skip_white(namEnd);
		// Search for attribue value
		if (*ptr == '=') {
			ptr = skip_white(ptr+1);
			if (*ptr == '"' || *ptr == '\'')
			{
				// Value is enclosed
				const char *encl = ptr;
				valBeg = valEnd = (ptr + 1);
				while (*valEnd != 0 && *valEnd != *encl)
					valEnd++;
				ptr = valEnd+1;
			}
			else
			{
				// Value is not enclosed
				valBeg = valEnd = ptr;
				while (isalnum(*valEnd) || *valEnd == '_')
					valEnd++;
				ptr = valEnd;
			}
		}
		//
		namLen = namEnd - namBeg;
		valLen = valEnd - valBeg;
		return true;
	} else {
		return false;
	}
}

void skip_attributes(const char *&ptr)
{
	const char *nam, *val;
	int namLen, valLen, len;

	// Skip tagname
	if (*(++ptr) != 0 && *ptr == '/')
		ptr++;
	while (*ptr != 0 && isalnum(*ptr))
		ptr++;
	// Skip attributes
	while (get_attribute(ptr, nam, namLen, val, valLen)) ;
	// skip ">"
	ptr++;
}

const char *Anchor(const char *txt, CLanguageProxy& ao)
{
	const char *nam, *val;
	int namLen, valLen, len;

	while (get_attribute(txt, nam, namLen, val, valLen))
	{
		if (valLen > 0)
		{
			if (strncasecmp(nam, "HREF", 4) == 0)
			{
				if (*val != '#')
				{
					BString label("HREF:  "), include(val, valLen);
					label << include;
					ao.AddInclude(label.String(), include.String());
				}
			}
			else if (strncasecmp(nam, "NAME", 4) == 0)
			{
				BString label("A:  "), function(val, valLen);
				label << function;
				ao.AddFunction(label.String(), function.String(), txt - ao.Text());
			}
		}

	};
	return txt;
} /* Anchor */

const char *Heading(const char *txt, CLanguageProxy& ao)
{
	char endTag[6];
	bool wasSpace = false;
	const char *ptr = txt;
	BString label("H"), heading;

	label << (*txt - '0') << ":  ";
	skip_attributes(ptr);
	sprintf(endTag, "</h%c>", *txt);
	txt = ptr;
	while (*ptr != 0) {
		if (*ptr == '<') {
			if (strncasecmp(ptr, endTag, 5) == 0)
			{
				break;
			}
			skip_attributes(ptr);
		} else {
			if (isspace(*ptr))
			{
				if (!wasSpace)
					label << *ptr;
				wasSpace = true;
			}
			else
			{
				label << *ptr;
				wasSpace = false;
			}
			ptr++;
		}
	}
	if (label.Length() > 100)
	{
		label.Truncate(100);
		label << "…";
	}
	heading.SetTo(txt, ptr-txt);
	ao.AddFunction(label.String(), heading.String(), txt - ao.Text());

	return ptr;
} /* Heading */

const char *JavaScript(const char *txt, CLanguageProxy& ao)
{
	txt = skip_nc(txt, '>');
	
	while (*txt)
	{
		switch (*txt)
		{
			case '/':
				if (*(txt + 1) == '/')
					txt = skip(txt, '\n') - 1;
				break;
			case '"':
			{
				while (*++txt)
				{
					if (*txt == '\\')
						txt++;
					else if (*txt == '"')
						break;
				}
				break;
			}
			case '\'':
			{
				while (*++txt)
				{
					if (*txt == '\\')
						txt++;
					else if (*txt == '\'')
						break;
				}
				break;
			}
			case '{':
			case '(':
				txt = block(txt + 1, *txt);
				break;
			case '<':
				if (strncasecmp(txt, "</script", 8) == 0)
					return txt + 9;
				break;
			default:
				if (strncasecmp(txt, "function", 8) == 0)
				{
					txt += 8;
					while (isspace(*txt)) txt++;
					
					int offset = txt - ao.Text();
					
					char name[kMaxNameSize];
					char *n = name;
					while ((isalnum(*txt) || *txt == '.' || *txt == '_') &&
							(n - name) < kMaxNameSize - 1)
					{
						*n++ = *txt++;
					}
					*n = 0;
					
					char label[kMaxNameSize + 3];
					strcpy(label, "JS:  ");
					strcat(label, name);

					ao.AddFunction(label, name, offset);
				}
				break;
		}
		txt++;
	}
	
	return txt;
} // JavaScript

const char *PhpScript(const char *txt, CLanguageProxy& ao)
{
	while (*txt)
	{
		switch (*txt)
		{
			case '/':
				if (*(txt + 1) == '/')
					txt = skip(txt + 2, '\n') - 1;
				else if (*(txt + 1) == '*')
				{
					++txt;
					
					while ( *++txt &&
							!(*txt == '?' && *(txt + 1) == '>') &&
							!(*txt == '*' && *(txt + 1) == '/'))
						;
					++txt;
				}
				break;
			case '"':
			{
				while (*++txt)
				{
					if (*txt == '\\')
						txt++;
					else if (*txt == '"')
						break;
				}
				break;
			}
			case '\'':
			{
				while (*++txt)
				{
					if (*txt == '\\')
						txt++;
					else if (*txt == '\'')
						break;
				}
				break;
			}
			case '{':
			case '(':
				txt = block(txt + 1, *txt);
				break;
			case '?':
				if (*++txt == '>')
					return txt + 1;
				break;
			default:
				if (strncasecmp(txt, "function", 8) == 0)
				{
					txt += 8;
					while (isspace(*txt)) txt++;
					
					int offset = txt - ao.Text();
					
					char name[kMaxNameSize];
					char *n = name;
					while ((isalnum(*txt) || *txt == '_') && (n - name) < kMaxNameSize)
						*n++ = *txt++;
					*n = 0;
					
					char label[kMaxNameSize + 6];
					strcpy(label, "PHP:  ");
					strcat(label, name);

					ao.AddFunction(label, name, offset);
				}
				break;
		}
		txt++;
	}
	
	return txt;
} // PhpScript

void ScanForFunctions(CLanguageProxy& proxy)
{
	const char *text = proxy.Text(), *max = text + proxy.Size();
	if (*max != 0)
		return;

	if (strncasecmp(text, "<!--:javascript", 15) == 0)
		text = JavaScript(text, proxy);
	else if (strncasecmp(text, "<!--:php", 8) == 0)
		text = PhpScript(text + 11, proxy);

	while (text < max)
	{
		text = skip(text, '<');
		text = skip_white(text);
		
		switch (toupper(*text))
		{
			case 0:
				return;
			case '?':
				text = PhpScript(text, proxy);
				break;
			case 'A':
				if (isspace(*++text))
					text = Anchor(text, proxy);
				else
					text = skip_nc(text, '>');
				break;
			case 'L':
				if (strncasecmp(text, "LINK", 4) == 0)
					text = Anchor(text + 4, proxy);
				else
					text = skip_nc(text, '>');
				break;
			case 'H':
				if (*++text >= '1' && *text <= '6')
				{
					text = Heading(text, proxy);
				}
				else
					text = skip_nc(text, '>');
				break;
			case 'S':
				if (strncasecmp(text, "SCRIPT", 6) == 0)
					text = JavaScript(text, proxy);
				break;
			default:
				text = skip_nc(text + 1, '>');
				break;
		}
	}
} /* ScanForFunctions */
