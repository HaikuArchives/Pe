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

	Created: 09/15/97 02:33:13
*/

#include "PeAddOn.h"
#include <string>
#include <E-mail.h>

#if __INTEL__
extern "C" _EXPORT long perform_edit(MTextAddOn *addon);
#else
#pragma export on
extern "C" {
long perform_edit(MTextAddOn *addon);
}
#pragma export reset
#endif

const char *skip(const char *txt, int ch);
const char *skip_nc(const char *txt, int ch);
//const char *skip_white(const char *txt);
const char *DoInclude(MTextAddOn& ao, const char *incl, const char *start, const char *end);
void Parse(MTextAddOn& ao);
void ReplaceGlossary(string& text, const char *filename, const char *lws);

long perform_edit(MTextAddOn *addon)
{
	long result = B_NO_ERROR;
	
	Parse(*addon);
	
	return B_NO_ERROR;
} /* perform_edit */

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

inline const char *skip_white(const char *txt)
{
	while (isspace(*txt))
		txt++;
	return txt;
} /* skip_white */

void Parse(MTextAddOn& ao)
{
	const char *inclStart, *inclEnd;
	const char *txt;
	char path[PATH_MAX];

	txt = skip(ao.Text(), '<');

	while (*txt)
	{
		if (txt[0] == '!' && txt[1] == '-' && txt[2] == '-')
		{
				// we have a comment
			do
			{
				txt = skip_white(txt + 3);
				
				if (*txt++ != '#')
					break;
				
				if (strncmp(txt, "include", 7))
					break;
				
				txt = skip_white(txt + 7);
				
				if (*txt++ != '"')
					break;
				
				int l = 0;
				while (l < PATH_MAX && *txt && *txt != '"')
					path[l++] = *txt++;
				
				if (*txt != '"')
					break;
				
				path[l] = 0;

				do		txt = skip_nc(txt + 1, '-');
				while (*txt && *txt != '-');
				
				if (*txt == 0)
					break;
				
				txt = skip_nc(txt + 1, '>');
				
				inclStart = txt;
				
				do
				{
					do		txt = skip(txt, '<');
					while (*txt && ! (txt[0] == '!' && txt[1] == '-' && txt[2] == '-'));
					
					if (*txt == 0)
						break;

					inclEnd = txt - 1;
					
					txt = skip_white(txt + 3);
					
					if (strncmp(txt, "end", 3) == 0)
					{
						txt = skip_white(txt + 3);
						if (strncmp(txt, "include", 7) == 0)
							break;
					}
				}
				while (*txt);
				
				if (*txt == 0)
					break;
				
				txt = DoInclude(ao, path, inclStart, inclEnd);
			}
			while (false);
		}

		txt = skip_nc(txt, '>');
		txt = skip(txt, '<');
	}
} /* Parse */

void ReplaceGlossary(string& str, const char *filename, const char *lws)
{
	int is;
	
	while ((is = str.find("$file$")) != string::npos)
	{
		str.replace(is, 6, filename ? filename : "");
	}
	
	while ((is = str.find("$date$")) != string::npos)
	{
		char now[64];
		time_t t;
		time(&t);
		strftime(now, 63, "%x", localtime(&t));
		
		str.replace(is, 6, now);
	}

	while ((is = str.find("$time$")) != string::npos)
	{
		char now[64];
		time_t t;
		time(&t);
		strftime(now, 63, "%X", localtime(&t));
		
		str.replace(is, 6, now);
	}
	
	while ((is = str.find("$name$")) != string::npos && count_pop_accounts())
	{
		mail_pop_account pop;
		if (get_pop_account(&pop, 0)) break;
		
		str.replace(is, 6, pop.real_name);
	}
	
	is = 0;
	while ((is = str.find('\n', is)) != string::npos)
	{
		str.insert(is + 1, lws);
		is += strlen(lws) + 1;
	}
} /* ReplaceGlossary */

const char *DoInclude(MTextAddOn& ao, const char *incl, const char *start, const char *end)
{
	status_t err = B_NO_ERROR;

	do
	{
		BEntry e;
		entry_ref ref;

		if (ao.GetRef(ref) == B_OK)
		{
			BDirectory d;
			if ((err = BEntry(&ref).GetParent(&d)) != B_OK) break;
			if ((err = d.FindEntry(incl, &e, true)) != B_OK) break;
		}
		else
		{
			if ((err = get_ref_for_path(incl, &ref)) != B_OK) break;
			if ((err = e.SetTo(&ref)) != B_OK) break;
		}
		
		int tStart, tEnd;
		
		tStart = start - ao.Text();
		tEnd = tStart + (end - start);
		ao.Select(tStart, tEnd);
		ao.Delete();

		string str;
		
		BFile file;
		if ((err = file.SetTo(&e, B_READ_ONLY)) != B_OK) break;

		size_t size = file.Seek(0, SEEK_END);
		file.Seek(0, SEEK_SET);
		
		char *t = (char *)malloc(size + 3);
		if (t == NULL)
		{
			err = B_ERROR;
			break;
		}
		
		file.Read(t + 1, size);
		t[0] = '\n';
		
		if (t[size] == '\n')
			t[size + 1] = 0;
		else
		{
			t[size + 1] = '\n';
			t[size + 2] = 0;
		}
		
		const char *txtp = start, *txt = ao.Text();
		char lws[256], *lw = lws;
		
		while (--txtp >= txt && *txtp != '\n')
			;
		while (isspace(*++txtp))
			*lw++ = *txtp;
		*lw = 0;
		
		str = t;
		
		BPath p;
		if ((err = e.GetPath(&p)) != B_OK) break;
		
		ReplaceGlossary(str, p.Path(), lws);
		ao.Insert(str.c_str());
		
		end = ao.Text() + tStart + str.length();
	}
	while (false);
	
	if (err)
	{
		char s[PATH_MAX];
		sprintf(s, "Error including %s: %s", incl, strerror(err));
		beep();
		MWarningAlert(s).Go();
	}
	
	return end;
} /* DoInclude */
