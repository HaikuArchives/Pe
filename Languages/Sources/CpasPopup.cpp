/*	$Id$
	
	Copyright 1996, 1997, 1998, 2002
	        Hekkelman Programmatuur B.V.  All rights reserved.
	        
	 Copyright 2003 Kelvin W Sherlock
	
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

	Created: 03/05/2003
*/

#include "CLanguageAddOn.h"

const int kMaxNameSize = 256;

static bool prototype = true;	// while true, methods are just declarations.
static bool protoadded = false;	// true just after the first proto was added.

const char *comment(const char *text)
{
	do
	{
		while (isspace (*text))
			text++;
			
		if (*text == '{') 
		{
			while (*text && *text != '}') text++;
			if (*text) text++;
		}
		else if (*text == '(' && text[1] == '*')
		{
			text += 2;	
			while (*text && *text != '*' && text[1] != ')') text++;
			if (*text) text += 2;
		}
		else if (*text == '/' && text[1] == '/')
		{
			while (*text && *text != '\n') text++;
			if (*text) text++;
		}
		else break;
	}
	while (*text);

	return text;
}

const char *parseFunction(const char *text, CLanguageProxy& proxy)
{
	char nameBuf[kMaxNameSize], *name = nameBuf;
	int size = 0;
	int offset = text - proxy.Text();
	
	text = comment(text);
	
	while (isalpha(*text) || *text == '.' || *text == '_')
	{
			if (size < kMaxNameSize - 1)
			{
				*name++ = *text++;
				size++;
			}
			else text++;
	}
	*name = 0;

	if (size)
	{
		if (proxy.Prototypes() && prototype)
		{
		 	proxy.AddFunction(nameBuf, nameBuf, offset, true);
		 	protoadded = true;
		}
		else if (!prototype)
	 		proxy.AddFunction(nameBuf, nameBuf, offset, false);
	}

	return text;
}

//------------------------------------------------------------------------------

void ScanForFunctions(CLanguageProxy& proxy)
{
	const char *text = proxy.Text(), *max = text + proxy.Size();

	if (*max != 0) return;

	while (text < max)
	{
		text = comment(text);
		
		switch (*text++)
		{
			case 'i':
			case 'I':
				if (!strncasecmp(text, "mplementation", 13))
				{
					prototype = false;
					if (proxy.Prototypes() && protoadded)
						proxy.AddSeparator();
				}
			break;

			case 'f':
			case 'F':
				if (!strncasecmp(text, "unction", 7))
					text = parseFunction(text + 7, proxy);
			break;

			case 'p':
			case 'P':
				if (!strncasecmp(text, "rocedure", 8))
					text = parseFunction(text + 8, proxy);
			break;
		}
	}
}

//------------------------------------------------------------------------------
