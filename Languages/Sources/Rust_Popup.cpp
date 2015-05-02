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


const int kMaxNameSize = 256;


void
ScanForFunctions(CLanguageProxy& proxy)
{
	int offset = 0;
	bool parsed = false;

	char nameBuffer[kMaxNameSize];
	char* name = nameBuffer;

	const char* text = proxy.Text();
	const char* max = text + proxy.Size();

	if (*max != 0)
		return;

	while (text < max) {
		while (isspace(*text)) {
			text++;
		}

		if (*text++ == 'p') {
			if (*text == 'u') {
				text++;

				if (*text == 'b') {
					text++;

					while (isspace(*text)) {
						text++;
					}
				}
			} else if (*text++ == 'r' && *text++ == 'i' && *text++ == 'v') {
				while (isspace(*text)) {
					text++;
				}
			}
		}

		if (*text++ == 'f' && *text++ == 'n') {
			while (isspace(*text)) {
				text++;
			}

			parsed = false;

			while (*text != '\n') {
				if (!parsed) {
					offset = text - proxy.Text();
					
					while (isalpha(*text) || *text == '_') {
						*name++ = *text++;
						parsed = true;
					}
				}

				text++;
			}

			if (parsed) {
				*name++ = 0;
				proxy.AddFunction(nameBuffer, nameBuffer, offset, false);
				name = nameBuffer;
				*name = 0;
			}
		}

		while (*text != '\n' && *text != 0) {
			text++;
		}
	}
}
