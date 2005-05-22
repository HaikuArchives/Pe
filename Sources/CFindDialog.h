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

	Created: 02-06-02
*/

#ifndef CFINDDIALOG_H
#define CFINDDIALOG_H

#include <stack>
#include <regex.h>
#include "HDialog.h"

class PMessageItem;
class PDoc;

const unsigned long
	msg_QueryCanReplace = 'CRpl',
	msg_ReplyCanReplace = 'RpRp';

class CFindDialog : public HDialog
{
	friend class CDirDropArea;
  public:
	enum { sResID = 8 };
		
	CFindDialog(BRect frame, const char *name,
		window_type type, int flags, BWindow *owner, BPositionIO* data);

	virtual void DoFind(unsigned long cmd);
	virtual void MessageReceived(BMessage *msg);
	virtual void UpdateFields();

	const char* FindString();
	const char* ReplaceString();
	bool Backward();
	bool IgnoreCase();
	bool Wrap();
	bool Word();
	bool Grep();
	bool IsInMultiFileState() const;
	const regex_t *PatternBuffer();
		
  private:
	virtual bool QuitRequested();

	virtual void WindowActivated(bool active);

	enum MultiReplaceKind { mrNone, mrLeaveOpen, mrSaveToDisk };

	void UpdateSearchDirMenu();
	void AddPathToDirMenu(const char *path, bool select = false, bool addToPrefs = false);
	void AddPathToDirMenu(entry_ref& ref, bool select = false, bool addToPrefs = false);

	bool DoMultiFileFind(const char *dir, bool recursive, bool restart, MultiReplaceKind replace, PDoc **doc);
	bool FindInFile(const entry_ref& ref, vector<PMessageItem*> *lst);
	bool GetRefForPath(entry_ref& ref, const char *path);
	void DoIncludesFind();
	void DoOpenWindows(bool replace);

	void ShowBatch(vector<PMessageItem*> *lst, BWindow **w);
	
	void FillGrepPopup();

	BMenu *fKind, *fDirectory, *fName, *fGrepPopup;
	char **fBeIncludes;
	int fBeIncludeCount;
	stack<char*> fDirNameStack;
	stack<DIR*> fDirStack;
	DIR *fCurrentDir;
	BFilePanel *fDirPanel;

	vector<PDoc*> fOpenWindows;
	int fOpenWindowIndex;
	int fCurrentIncludeIndex;
	regex_t fPatternBuffer;
	long padding[4];
};

extern CFindDialog* gFindDialog;

inline bool CFindDialog::Backward()
{
	BAutolock lock(this);
	return IsOn("back");
}

inline bool CFindDialog::IgnoreCase()
{
	BAutolock lock(this);
	return IsOn("case");
}

inline bool CFindDialog::Wrap()
{
	BAutolock lock(this);
	return IsOn("wrap");
}

inline bool CFindDialog::Word()
{
	BAutolock lock(this);
	return IsOn("word");
}

inline bool CFindDialog::Grep()
{
	BAutolock lock(this);
	return IsOn("grep");
}

inline bool CFindDialog::IsInMultiFileState() const
{
	return fCurrentDir != NULL || fOpenWindowIndex >= 0;
}

#pragma mark - Find

extern bool gRxInstalled;

void initskip(const unsigned char *p, int skip[], bool ignoreCase);
void initskip_b(const unsigned char *p, int skip[], bool ignoreCase);
int mismatchsearch(const unsigned char *p, const unsigned char *a, int N, int skip[], bool ignoreCase);
int mismatchsearch_b(const unsigned char *p, const unsigned char *a, int N, int skip[], bool ignoreCase);

int Find(const char *what, const char *buf, int bufSize, bool ignoreCase);
bool FileContains(const char *path, const char *what, bool ignoreCase, bool word, vector<PMessageItem*> *lst = NULL);
void Offset2Line(const char *buf, int size, int offset, int& line, int& selStart, char** l);

bool BufferContains(const char *buf, int size, const char *path, const char *what, bool ignoreCase, bool word, vector<PMessageItem*> *lst = NULL);

bool FileContainsEx(const char *path, const regex_t *preg, bool word, vector<PMessageItem*> *lst = NULL);
bool BufferContainsEx(const char *buf, int size, const char *path, const regex_t *preg, bool word, vector<PMessageItem*> *lst = NULL);

int rx_regcomp(regex_t *pb, const char *pattern, bool ignoreCase);
char *rx_replace(regex_t *pb, const char *txt, int size, const char *repl);
int rx_regexec(const regex_t *preg, const char *string, int len,
	int nmatch, regmatch_t pmatch[], int eflags, bool backward = false);

#endif // CFINDDIALOG_H
