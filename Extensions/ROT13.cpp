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
*/

#include "PeAddOn.h"

#if __INTEL__
	extern "C" _EXPORT long perform_edit(MTextAddOn *addon);
#else
#	pragma export on
	extern "C"
		long perform_edit(MTextAddOn *addon);
#	pragma export reset
#endif

long perform_edit(MTextAddOn *addon)
{
	long result = B_NO_ERROR;
	
	long s, e, l;
	unsigned char *r, *t;
	
	addon->GetSelection(&s, &e);
	
	if (s == e)
	{
		s = 0;
		e = addon->TextLength();
		addon->Select(s, e);
	}
	
	l = e - s;
	
	r = (unsigned char *)malloc(l);

	if (r)
	{
		memcpy(r, addon->Text() + s, l);
		
		for (t = r; t < r + l; t++)
		{
			if (*t >= 'a' && *t <= 'z')
			{
				*t += 13;
				if (*t > 'z') *t -= 26;
			}
			else if (*t >= 'A' && *t <= 'Z')
			{
				*t += 13;
				if (*t > 'Z') *t -= 26;
			}
		}
		
		addon->Delete();
		addon->Insert((char *)r, l);
		
		free(r);
	}
	else
		result = B_ERROR;
	
	return result;
} /* perform_edit */






