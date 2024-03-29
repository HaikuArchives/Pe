/*	$Id: CperlFuncPopup.l 98 2004-03-17 16:42:21Z tappe $
	
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


   Put together in 2006, by Oscar Lesta, from "CperlFuncPopup.l" and some bits
    of "CtexFuncPopup.l". Don't ask me how this work, it just does it! :-)

  Caveats/Notes:

    - This will look for lout's data is installed under the first of
      `findpaths B_FIND_PATH_DATA_DIRECTORY` + "/lout" that exists, namely;

		/boot/home/config/non-packaged/data/lout
		/boot/home/config/data/lout
		/boot/system/non-packaged/data/lout
		/boot/system/data/lout

      *but*

      if the enviromental variable "$LOUTLIB" is found, it's value is used
      instead (make sure it is correct!).

    - Assumes that *your* files end with .lt (lout's files) and .ld (lout's
      databases).

    - Assumes that chapters, sections, subsections, etc. are in the form:
      @Section
        @Title { A Title }
      ie. @Title must *immediately* follow @Chapter, @Section, etc.

    - Untitled chapters/sections/overheads/etc handling needs improvements.
*/


%{
#include "CLanguageAddOn.h"

#include <PathFinder.h>
#include <StringList.h>

static const int kMaxNameLength = 70;

static const char *gBuf = NULL;
static int gIndx;
static int gBufSize;
static bool inString;

#undef YY_DECL
#define YY_DECL	static int yylex(CLanguageProxy& proxy)
#undef YY_INPUT
#define YY_INPUT(tok,result,max)	result = GetNextChar(tok, max)
#define YY_SKIP_YYWRAP
#undef yywrap
#define yywrap()	1
#undef ECHO
#define ECHO

static int GetNextChar(char* tok, int max);
static void AddInclude(CLanguageProxy& proxy, const char ext[]);
static void AddSysDataBase(CLanguageProxy& proxy);
static void AddSysInclude(CLanguageProxy& proxy);
static void AddSection(CLanguageProxy& proxy, int level);
%}

ws				[ \n\t]*

%x string

%%
											int string_caller = 0;
\"											string_caller = INITIAL; BEGIN(string); inString = true;
<string>\"									BEGIN(string_caller); inString = false;
"#"[^\n]*\n									// comment line
\@DataBase{ws}\@Reference{ws}\{[^\n]+\}		AddInclude(proxy, ".ld");
\@SysDataBase{ws}\@Reference{ws}\{[^\n]+\}	AddSysDataBase(proxy);
\@Include{ws}\{[^\n]+\}						AddInclude(proxy, ".lt");
\@SysInclude{ws}\{[^\n]+\}					AddSysInclude(proxy);
\@Overhead{ws}\@Title{ws}\{[^\n]+\}			AddSection(proxy, 0);	// for Slides
\@Chapter{ws}\@PartTitle{ws}\{[^\n]+\}		AddSection(proxy, 0);
\@Chapter{ws}\@PartNumber{ws}\{[^\n]+\}		AddSection(proxy, 0);
\@Chapter{ws}\@Title{ws}\{[^\n]+\}			AddSection(proxy, 1);
\@Section{ws}\@Title{ws}\{[^\n]+\}			AddSection(proxy, 2);
\@SubSection{ws}\@Title{ws}\{[^\n]+\}		AddSection(proxy, 3);
\@SubSubSection{ws}\@Title{ws}\{[^\n]+\}	AddSection(proxy, 4);
<<EOF>>										BEGIN(INITIAL); return 0;
%%


_EXPORT void ScanForFunctions(CLanguageProxy& proxy)
{
	static sem_id fps = create_sem(1, "func popup semaphore");

	if (acquire_sem(fps) == B_NO_ERROR) {
		gBuf = proxy.Text();
		gBufSize = proxy.Size();
		gIndx = 0;
		inString = false;

		yyrestart(NULL);
		yylex(proxy);

		release_sem(fps);
	}
}


int GetNextChar(char* tok, int max)
{
	int c = gBuf[gIndx++];

	if (inString)
		;
	else if (c == '#') {
		int result;
		int space = 0;

		do {
			c = gBuf[gIndx++];
			space++;
		}
		while (gIndx < gBufSize && c != '\n');

		result = max = std::min(max, space);

		while (max--)
			*tok++ = ' ';

		return result;
	}

	return (gIndx < gBufSize) ? (tok[0] = c, 1) : YY_NULL;
}


char* trimstring(char str[])
{
	char *start, *end;

	start = str;
	while (isspace(*start))					// skip leading whites...
		start++;

	if (*start == '\0')						// are we at string's end yet?
		return start;

	end = start + strlen(start) - 1;
	while ((end > start) && isspace(*end))	// skip trailing whites...
		end--;

	*++end = '\0';

	return start;
}


static bool PathExists(const BPath& path)
{
	BEntry entry(path.Path(), true);
	return entry.Exists();
}


static void AddInclude(CLanguageProxy& proxy, const char ext[])
{
	char *s, *i = yytext;

	s = strrchr(i, '}');
	if (!s) return;
	*s = 0;
	s = strrchr(i, '{');
	if (!s) return;

	s++;
	s = trimstring(s);

	BString nameNoExt(s);
	BPath path(nameNoExt);

	path.Append(ext);

	// Also try without the extension (useful when working with Lout's own doc files).
	if (!PathExists(path))
		path.SetTo(nameNoExt);

	if (PathExists(path))
		proxy.AddInclude(s, path.Path(), false);
}


static status_t GetLoutDirectory(const char* subDir, BPath& path)
{
	static const char* kLoutDir = getenv("LOUTLIB");

	if (kLoutDir != NULL) {
		path.SetTo(kLoutDir);
		path.Append(subDir);
	}

	if (kLoutDir == NULL || !PathExists(path)) {
		BStringList paths;
		status_t status = BPathFinder::FindPaths(B_FIND_PATH_DATA_DIRECTORY,
			"lout", B_FIND_PATH_EXISTING_ONLY, paths);
		if (status != B_OK)
			return status;

		path.SetTo(paths.StringAt(0));
		path.Append(subDir);

		if (!PathExists(path))
			return B_ENTRY_NOT_FOUND;
	}

	return B_OK;
}


static void AddSysDataBase(CLanguageProxy& proxy)
{
	char *s, *i = yytext;

	s = strrchr(i, '}');
	if (!s) return;
	*s = 0;
	s = strrchr(i, '{');
	if (!s) return;

	s++;

	s = trimstring(s);

	BPath path;
	if (GetLoutDirectory("data", path) != B_OK)
		return;

	BString name = s;
	name << ".ld";
	path.Append(name.String());

	if (PathExists(path))
		proxy.AddInclude(s, path.Path(), true);
}


static void AddSysInclude(CLanguageProxy& proxy)
{
	char *s, *i = yytext;

	s = strrchr(i, '}');
	if (!s) return;
	*s = 0;
	s = strrchr(i, '{');
	if (!s) return;

	s++;

	s = trimstring(s);

	BPath path;
	if (GetLoutDirectory("include", path) != B_OK)
		return;

	path.Append(s);

	if (PathExists(path))
		proxy.AddInclude(s, path.Path(), true);
}


static void AddSection(CLanguageProxy& proxy, int level)
{
	static const char* kUnnamed = "<unnamed>";
	static const int kUnnamedLen = strlen(kUnnamed);

	char *s, *i = yytext;

	s = strrchr(i, '}');
	if (!s) return;
	*s = 0;
	s = strrchr(i, '{');
	if (!s) return;

	s++;

	s = trimstring(s);
	char* tmp;

	if (*s && s[0] != ' ') {
		tmp = (char*) calloc(1, strlen(s) + level * 2 + 1);	// level * 2 spaces
		if (!tmp)
			return;
		while (level--)
			strcat(tmp, "  ");				// 2 spaces for each level deep
		strncat(tmp, s, kMaxNameLength);	// trucate if title is too long.
	}
	else {
		tmp = (char*) calloc(1, kUnnamedLen + level * 2 + 1);	// level * 2 spaces
		if (!tmp)
			return;
		while (level--)
			strcat(tmp, "  ");		// 2 spaces for each level deep
		strcat(tmp, kUnnamed);
	}
	proxy.AddFunction(tmp, s, 0);
	free(tmp);
}
