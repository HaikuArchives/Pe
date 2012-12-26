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

	Created: 09/10/97 15:20:13
*/

#include "pe.h"
#include "CGlossary.h"
#include "PApp.h"
#include <E-mail.h>
#include "Utils.h"
#include "HError.h"

CGlossary *gGlossary;

CGlossary::CGlossary()
{
	try
	{
		if (gPrefsDir.Contains("Glossary", B_FILE_NODE | B_SYMLINK_NODE))
			ParseGlossaryFile();
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* CGlossary::CGlossary */

CGlossary::~CGlossary()
{
} /* CGlossary::~CGlossary */
		
bool CGlossary::IsGlossaryShortcut(int key, int modifiers)
{
	key |= (modifiers & kModifierMask) << 16;

	glossymap::iterator gi;
	gi = fGlossies.find(key);
	return (gi != fGlossies.end());
} /* CGlossary::IsGlossaryShortcut */

void CGlossary::GetGlossary(int key, int modifiers, const char *selection,
	const char *filename, const char *lws, char*& s, int& insert)
{
	string str = fGlossies[key | ((modifiers & kModifierMask) << 16)];
	ReplaceGlossary(str, selection, filename, lws, s, insert);
} /* CGlossary::GetGlossary */

void CGlossary::ReplaceGlossary(const string& glossy, const char *selection,
	const char *filename, const char *lws, char*& s, int& insert)
{
	string str = glossy;
	int is;
	
	const char *base = filename ? strrchr(filename, kDirectorySeparator) : NULL;
	if (base == NULL && filename)
		base = filename;
	else if (base)
		base++;
	
	while ((is = str.find("$file$")) != string::npos)
	{
		str.replace(is, 6, base ? base : "");
	}
	
	while ((is = str.find("$dir$")) != string::npos)
	{
		str.erase(is, 5);
		if (filename)
		{
			const char *t = base ? base : filename + strlen(filename);
			str.insert(str.begin() + is, filename, t);
		}
	}
	
	while ((is = str.find("$file_define$")) != string::npos)
	{
		char dFileName[256];
		int i = 0;
		
		if (filename)
		{
			for (const char *t = base; *t && i < 255; t++, i++)
				dFileName[i] = isalnum(*t) ? toupper(*t) : '_';
		}

		dFileName[i] = 0;
		
		str.replace(is, 13, dFileName);
	}
	
	while ((is = str.find("$selection$")) != string::npos)
	{
		str.replace(is, 11, selection ? selection : "");
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
	
	if ((is = str.find("$insert$")) != string::npos)
	{
		str.erase(is, 8);
		insert = is;
	}
	else
		insert = str.length();

	s = strdup(str.c_str());
} /* CGlossary::ReplaceGlossary */
	
void CGlossary::ParseGlossaryFile()
{
	BFile bFile;
	FailOSErr(bFile.SetTo(&gPrefsDir, "Glossary", B_READ_ONLY));
	int fd = bFile.Dup();
	FILE *file = fdopen(fd, "r");
	
	char s[256];
	
	fgets(s, 255, file);
	
	if (strncmp(s, "### ", 4)) THROW(("Invalid glossary file"));
	
	fgets(s, 255, file);
	while (!feof(file) && strncmp(s, "##", 2) == 0)
	{
		int key = 0;
		char *kp;
		
		kp = strtok(s + 2, " -\n");
		while (kp && strncmp(kp, "##", 2))
		{
			if (strcmp(kp, "control") == 0)
				key |= B_CONTROL_KEY << 16;
			else if (strcmp(kp, "shift") == 0)
				key |= B_SHIFT_KEY << 16;
			else if (strcmp(kp, "command") == 0)
				key |= B_COMMAND_KEY << 16;
			else if (strcmp(kp, "option") == 0)
				key |= B_OPTION_KEY << 16;
			else if (strcmp(kp, "menu") == 0)
				key |= B_MENU_KEY << 16;
			else if (strcmp(kp, "space") == 0 || strcmp(kp, "spacebar") == 0)
				key |= ' ';
			else if (strlen(kp) == 1)
				key |= *kp;
			else if (tolower(*kp) == 'f')
				key |= (strtol(kp + 1, &kp, 10) + 1) | 0x0100;
			else
				THROW(("Incorrect Glossary File, unrecognized keyword %s", kp));

			kp = strtok(NULL, " -\n");
		}
		
		string g;
		
		fgets(s, 255, file);
		while (!feof(file) && strncmp(s, "##", 2))
		{
			g += s;
			fgets(s, 255, file);
		}
		
		g.resize(g.size() - 1);
		
		fGlossies[key] = g;
	}
	
	fclose(file);
	
	close(fd);
} /* CGlossary::ParseGlossaryFile */
