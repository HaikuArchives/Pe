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

#include "pe.h"
#include "PCmd.h"
#include "PText.h"
#include "PMessages.h"
#include "Utils.h"
#include "HError.h"
#include "PDoc.h"
#include "CFindDialog.h"
#include "PLongAction.h"
#include "PApp.h"
#include "PErrorWindow.h"

PCmd::PCmd(const char *str, PText *txt)
{
	fStr = strdup(str);
	fText = txt;
	fTextDirty = fText->IsDirty();
	fUndoable = true;
} /* PCmd::PCmd */

PCmd::~PCmd() {
	free(fStr);
} /* PCmd::~PCmd */
		
void PCmd::Redo() {
	Do();
} /* PCmd::Redo */

void PCmd::Update() {
	fText->RedrawDirtyLines();
	BMessage msg(msg_SmoothUpdate);
	BMessenger(fText).SendMessage(&msg, (BHandler*)NULL, 1000);
} /* PCmd::Update */

void PCmd::Redraw()
{
	fText->Draw(fText->Bounds());
} /* PCmd::Redraw */

void PCmd::SetDirty(bool dirty)
{
	fTextDirty = dirty;
} /* PCmd::SetDirty */

#pragma mark - Copy

PCutCmd::PCutCmd(PText *txt, int append)
	: PCmd("Cut", txt)
{
	fSavedTxt = NULL;
	fAnchor = txt->Anchor();
	fCaret = txt->Caret();
	fAppend = append;
} /* PCutCmd::PCutCmd */

PCutCmd::~PCutCmd()
{
	if (fSavedTxt) free(fSavedTxt);
} /* PCutCmd::PCutCmd */

void PCutCmd::Do()
{
	fText->Select(fAnchor, fCaret, true, false);
	fWhere = min(fAnchor, fCaret);
	fText->GetSelectedText(fSavedTxt);
	fText->Cut(fAppend);
	Update();
} /* PCutCmd::Do */

void PCutCmd::Undo()
{
	if (fSavedTxt && strlen(fSavedTxt))
	{
		fText->Insert(fSavedTxt, strlen(fSavedTxt), fWhere);
		fText->Select(fWhere, fWhere + strlen(fSavedTxt), true, false);

		free(fSavedTxt);
		fSavedTxt = NULL;
	}
	else
		fText->SetCaret(fWhere);
	Update();
} /* PCutCmd::Undo */

#pragma mark -

PClearCmd::PClearCmd(PText *txt)
	: PCmd("Clear", txt)
{
	fSavedTxt = NULL;
	fAnchor = txt->Anchor();
	fCaret = txt->Caret();
} /* PClearCmd::PClearCmd */

PClearCmd::~PClearCmd()
{
	if (fSavedTxt) free(fSavedTxt);
} /* PClearCmd::PClearCmd */

void PClearCmd::Do()
{
	fText->Select(fAnchor, fCaret, true, false);
	fWhere = min(fAnchor, fCaret);
	fText->GetSelectedText(fSavedTxt);
	fText->Clear();
	Update();
} /* PClearCmd::Do */

void PClearCmd::Undo()
{
	if (fSavedTxt)
	{
		fText->Insert(fSavedTxt, strlen(fSavedTxt), fWhere);
		fText->Select(fWhere, fWhere + strlen(fSavedTxt), true, false);

		free(fSavedTxt);
		fSavedTxt = NULL;
	}
	else
		fText->SetCaret(fWhere);
	Update();
} /* PClearCmd::Undo */

#pragma mark -

PPasteCmd::PPasteCmd(PText *txt)
	: PCmd("Paste", txt)
{
	fSavedTxt = NULL;
} /* PPasteCmd::PPasteCmd */

PPasteCmd::~PPasteCmd()
{
	if (fSavedTxt) free(fSavedTxt);
	if (fPasted) free(fPasted);
} /* PPasteCmd::PPasteCmd */

void PPasteCmd::Do()
{
	fText->SetBlockSelect(false);
	fWhere = min(fText->Anchor(), fText->Caret());
	fText->GetSelectedText(fSavedTxt);

	fText->Paste();
	fTo = fText->Caret();

	int cnt = fTo - fWhere;
	fPasted = (char *)malloc(cnt + 1);
	FailNil(fPasted);
	fText->TextBuffer().Copy(fPasted, fWhere, cnt);
	fPasted[cnt] = 0;
	
	Update();
} /* PPasteCmd::Do */

void PPasteCmd::Undo()
{
	if (fPasted && strlen(fPasted))
		fText->Delete(fWhere, fTo);

	if (fSavedTxt && strlen(fSavedTxt))
	{
		fText->Insert(fSavedTxt, strlen(fSavedTxt), fWhere);
		fText->Select(fWhere, fWhere + strlen(fSavedTxt), true, false);
	}
	else
		fText->SetCaret(fWhere);

	Update();
} /* PPasteCmd::Undo */

void PPasteCmd::Redo()
{
	if (fSavedTxt && strlen(fSavedTxt))
		fText->Delete(fWhere, fWhere + strlen(fSavedTxt));

	if (fPasted && strlen(fPasted))
	{
		fText->Insert(fPasted, strlen(fPasted), fWhere);
		fText->Select(fWhere, fTo, true, false);
	}
	else
		fText->SetCaret(fWhere);

	Update();
} /* PPasteCmd::Redo */

#pragma mark --- Typing Command ---
PTypingCmd::PTypingCmd(PText *txt)
	: PCmd("Typing", txt)
{
} /* PTypingCmd::PTypingCmd */

PTypingCmd::~PTypingCmd()
{
	free(fDeleted);
} /* PTypingCmd::~PTypingCmd */

void PTypingCmd::Do()
{
	fDeletedIndx = min(fText->Caret(), fText->Anchor());
	fDeletedLen = abs(fText->Caret() - fText->Anchor());
		
	if (fDeletedIndx + fDeletedLen >= fText->TextBuffer().Size())
		fDeletedLen = max(fText->TextBuffer().Size() - fDeletedIndx, 0);
		
	fDeleted = (char *)malloc(fDeletedLen);
		
	if (fDeletedLen)
		fText->TextBuffer().Copy(fDeleted, fDeletedIndx, fDeletedLen);
		
	fInsertedLen = 0;
} /* PTypingCmd::Do */

void PTypingCmd::Undo()
{
	char *pt = NULL;

	if (fText->Anchor() != fText->Caret())
	{
		int lf, lt;
		
		lf = fText->Offset2Line(min(fText->Anchor(), fText->Caret()));
		lt = fText->Offset2Line(max(fText->Anchor(), fText->Caret()));
		fText->TouchLines(lf, lt);
	}

	if (fInsertedLen > 0)
	{
		pt = (char *)malloc(fInsertedLen + 1);
		FailNil(pt);
		fText->TextBuffer().Copy(pt, fDeletedIndx, fInsertedLen);
	}
	
	if (fInsertedLen)
		fText->Delete(fDeletedIndx, fDeletedIndx + fInsertedLen);

	if (fDeletedLen)
	{
		fText->Insert(fDeleted, fDeletedLen, fDeletedIndx);
		fText->Select(fDeletedIndx, fDeletedIndx + fDeletedLen, false, false);
	}
	else
		fText->SetCaret(fDeletedIndx);

	free(fDeleted);
	fDeleted = pt;
	int t = fInsertedLen;
	fInsertedLen = fDeletedLen;
	fDeletedLen = t;
	
	Update();
} /* PTypingCmd::Undo */

void PTypingCmd::Redo()
{
	Undo();
} /* PTextCmd::Redo */

//#pragma mark -
//#pragma mark --- Font & Tabs Command ---
//PFontTabsCmd::PFontTabsCmd(PText *txt, BMessage *msg)
//	: PCmd("Font Change", txt)
//{
//	const char *s;
//	int32 i;
//	float f;
//	bool b;
//	
//	if (msg->FindString("family", &s) == B_NO_ERROR) strcpy(fNewFamily, s);
//	if (msg->FindString("style", &s) == B_NO_ERROR) strcpy(fNewStyle, s);
//	if (msg->FindFloat("size", &f) == B_NO_ERROR) fNewSize = f;
//	if (msg->FindInt32("tabs", &i) == B_NO_ERROR) fNewTabs = i;
//	if (msg->FindBool("syntaxcol", &b) == B_NO_ERROR) fSyntaxColoring = b;
//	if (msg->FindBool("show invisibles", &b) == B_NO_ERROR) fShowInvisibles = b;
//	if (msg->FindBool("show tabs", &b) == B_NO_ERROR) fShowTabs = b;
//	if (msg->FindInt32("encoding", &i) == B_NO_ERROR) fEncoding = i;
//	if (msg->FindInt32("line breaks", &i) == B_NO_ERROR) fNewLB = i;
//} /* PFontTabsCmd::PFontTabsCmd */
//
//void PFontTabsCmd::Do()
//{
//	font_family ff;
//	font_style fs;
//	
//	fText->fFont.GetFamilyAndStyle(&ff, &fs);
//	fText->fFont.SetFamilyAndStyle(fNewFamily, fNewStyle);
//	strcpy(fNewFamily, ff);
//	strcpy(fNewStyle, fs);
//	
//	float fsize = fText->fFont.Size();
//	fText->fFont.SetSize(fNewSize);
//	fNewSize = fsize;
//	
//	swap(fNewTabs, fText->fTabStops);
//	swap(fSyntaxColoring, fText->fSyntaxColoring);
//	swap(fShowInvisibles, fText->fShowInvisibles);
//	swap(fNewLB, fText->fLineEndType);
//	
//	bool b = fText->Doc()->Status()->ShowsTabs();
//	if (b != fShowTabs)
//	{
//		fText->ShowTabStops(fShowTabs);
//		fShowTabs = b;
//	}
//	
//	int e = fText->Encoding();
//	fText->SetEncoding(fEncoding);
//	fEncoding = e;
//	
//	fText->ReInit();
//	fText->Invalidate();
//} /* PFontTabsCmd::Do */
//
//
//void PFontTabsCmd::Undo()
//{
//	Do();
//} /* PFontTabsCmd::Undo */
//
///////////////////////////////////////////////////////////////////////////////
//
// PDropCmd
//
#pragma mark -
#pragma mark --- Drop Command ---

PDropCmd::PDropCmd(PText *txt, const char *data, ssize_t dataLen, int srcLoc, int dstLoc)
	: PCmd("Drop", txt)
{
	fData = (char *)malloc(fDataLen = dataLen);
	FailNil(fData);
	memcpy(fData, data, dataLen);
	
	char *s, *d;
	s = d = fData;
	
	while (s < fData + dataLen)
	{
		if (*s == '\r')
		{
			*d++ = '\n';
			if (*++s == '\n')
				s++;
		}
		else
			*d++ = *s++;
	}
	
	fSrcLoc = srcLoc;
	fDstLoc = dstLoc;
} /* PDropCmd::PDropCmd */

PDropCmd::~PDropCmd()
{
	free(fData);
} /* PDropCmd::~PDropCmd */

void PDropCmd::Do()
{
	if (fSrcLoc >= 0)
		fText->Delete(fSrcLoc, fSrcLoc + fDataLen);
	
	if (fSrcLoc >= 0 && fSrcLoc < fDstLoc)
	{
		fText->Insert(fData, fDataLen, fDstLoc - fDataLen);
		fText->Select(fDstLoc - fDataLen, fDstLoc, false, false);
	}
	else
	{
		fText->Insert(fData, fDataLen, fDstLoc);
		fText->Select(fDstLoc, fDstLoc + fDataLen, false, false);
	}
	
	Redraw();
} /* PDropCmd::Do */

void PDropCmd::Undo()
{
	if (fSrcLoc >= 0 && fSrcLoc < fDstLoc)
		fText->Delete(fDstLoc - fDataLen, fDstLoc);
	else
		fText->Delete(fDstLoc, fDstLoc + fDataLen);

	if (fSrcLoc >= 0)
	{
		fText->Insert(fData, fDataLen, fSrcLoc);
		fText->Select(fSrcLoc, fSrcLoc + fDataLen, false, false);
		Redraw();
	}
	else
	{
		Update();
		fText->ChangeSelection(fDstLoc, fDstLoc);
	}
} /* PDropCmd::Undo */

ExtAction::ExtAction()
{
	aText = NULL;
	aOffset = 0;
	aType = 3;
} /* ExtAction::ExtAction */

ExtAction::ExtAction(int type, int offset, char *text)
	: aType(type), aOffset(offset), aText(text)
{
} /* ExtAction::ExtAction */

ExtAction::~ExtAction()
{
} /* ExtAction::~ExtAction */

PExtCmd::PExtCmd(PText *txt, const char *ext)
	: PCmd(ext, txt)
{
	fAnchor = fText->Anchor();
	fCaret = fText->Caret();
} /* PExtCmd::PExtCmd */

PExtCmd::~PExtCmd()
{
	list<ExtAction>::iterator ai;
	
	for (ai = fActions.begin(); ai != fActions.end(); ai++)
		free((*ai).aText);
} /* PExtCmd::~PExtCmd */

void PExtCmd::Do()
{
	// that's already done
} /* PExtCmd::Do */

void PExtCmd::Undo()
{
	int anchor = fText->Anchor();
	int caret = fText->Caret();
	
	list<ExtAction>::reverse_iterator ai;
	
	ai = fActions.rbegin();
	
	while (ai != fActions.rend())
	{
		if ((*ai).aType == eaInsert)
			fText->Delete((*ai).aOffset, (*ai).aOffset + strlen((*ai).aText));
		else
			fText->Insert((*ai).aText, strlen((*ai).aText), (*ai).aOffset);
		
		ai++;
	}

	fText->Select(fAnchor, fCaret, true, false);
	swap(anchor, fAnchor);
	swap(caret, fCaret);
	
	Update();
} /* PExtCmd::Undo */

void PExtCmd::Redo()
{
	int anchor = fText->Anchor();
	int caret = fText->Caret();
	
	list<ExtAction>::iterator ai;
	
	ai = fActions.begin();
	
	while (ai != fActions.end())
	{
		if ((*ai).aType == eaDelete)
			fText->Delete((*ai).aOffset, (*ai).aOffset + strlen((*ai).aText));
		else
			fText->Insert((*ai).aText, strlen((*ai).aText), (*ai).aOffset);
		
		ai++;
	}
	
	fText->Select(fAnchor, fCaret, true, false);
	swap(anchor, fAnchor);
	swap(caret, fCaret);
	
	Update();
} /* PExtCmd::Redo */


PReplaceCmd::PReplaceCmd(PText *txt, int offset, int size, bool findNext, bool backward)
	: PCmd("Replace", txt)
{
	fOffset = offset;
	fSize = size;
	fFindNext = findNext;

	fWhat = (char *)malloc(size + 1);
	FailNil(fWhat);
	txt->TextBuffer().Copy(fWhat, offset, size);
	fWhat[size] = 0;

	fWrap = gFindDialog->Wrap();
	fIgnoreCase = gFindDialog->IgnoreCase();
	fBackward = backward;
	fEntireWord = gFindDialog->Word();
	fGrep = gFindDialog->Grep();
	
	if (gRxInstalled && fGrep)
	{
		const char *t = gFindDialog->FindString();
		fWith = rx_replace(const_cast<regex_t*>(gFindDialog->PatternBuffer()),
			fWhat, size, gFindDialog->ReplaceString());
		fExpr = strdup(t);
	}
	else
	{
		fWith = strdup(gFindDialog->ReplaceString());
		fExpr = NULL;
	}
} /* PReplaceCmd::PReplaceCmd */

PReplaceCmd::~PReplaceCmd()
{
	if (fWhat) free(fWhat);
	if (fWith) free(fWith);
	if (fExpr) free(fExpr);
} /* PReplaceCmd::~PReplaceCmd */

void PReplaceCmd::Do()
{
	fText->Delete(fOffset, fOffset + fSize);
	if (*fWith)
		fText->Insert(fWith, strlen(fWith), fOffset);

	int offset = fOffset;
	if (!fBackward)
		offset += strlen(fWith);
	fText->SetCaret(offset);

	Update();
	
	if (fFindNext)
	{
		if (gFindDialog->IsInMultiFileState())
		{
			if (!fText->FindNext((unsigned char *)(fGrep ? fExpr : fWhat), offset, fIgnoreCase,
				false, false, fEntireWord, fGrep, true))
				gFindDialog->PostMessage(msg_FindInNextFile);
		}
		else
			fText->FindNext((unsigned char *)(fGrep ? fExpr : fWhat), offset, fIgnoreCase,
				fWrap, fBackward, fEntireWord, fGrep, true);
	}
} /* PReplaceCmd::Do */

void PReplaceCmd::Undo()
{
	if (fWith && *fWith) fText->Delete(fOffset, fOffset + strlen(fWith));
	fText->Insert(fWhat, strlen(fWhat), fOffset);
	Update();
	fText->Select(fOffset, fOffset + strlen(fWhat), true, false);
} /* PReplaceCmd::Undo */

PReplaceAllCmd::PReplaceAllCmd(PText *txt)
	: PCmd("Replace All", txt)
{
	fFind = strdup(gFindDialog->FindString());
	fRepl = strdup(gFindDialog->ReplaceString());
	fIgnoreCase = gFindDialog->IgnoreCase();
	fEntireWord = gFindDialog->Word();
	fGrep = gFindDialog->Grep();
	
	char buf[PATH_MAX];
	FailOSErr(fBackingStore.SetTo(tmpnam(buf)));
} /* PReplaceAllCmd::PReplaceAllCmd */

PReplaceAllCmd::~PReplaceAllCmd()
{
	if (fFind) free(fFind);
	if (fRepl) free(fRepl);
	
	remove(fBackingStore.Path());
} /* PReplaceAllCmd::~PReplaceAllCmd */

void PReplaceAllCmd::Do()
{
	BDirectory d;
	BFile f;

	FailOSErr(d.CreateFile(fBackingStore.Path(), &f));

	CheckedWrite(f, fText->Text(), fText->Size());
	
	fText->SetCaret(0);

	int a, c;
	
	if (gRxInstalled && fGrep)
	{
		regex_t pb;
		memset(&pb, 0, sizeof(pb));

		int r = rx_regcomp(&pb, fFind, fIgnoreCase);
		if (r > 1)
		{
			char err[100];
			regerror(r, &pb, err, 100);
			THROW((err));
		}
		
		a = c = 0;
		regmatch_t match[2];
		
		PLongAction la(fText);
		
		while (rx_regexec(&pb, fText->Text() + a, fText->Size() - a, 1, match, 0) == REG_NOERROR)
		{
			a += match[0].rm_so;
			c = a + (match[0].rm_eo - match[0].rm_so);
			
			fText->Select(a, c, true, false);
			
			char *r = rx_replace(&pb, fText->Text() + a, c - a + 1, fRepl);
			fText->Delete(a, c);
			fText->Insert(r, strlen(r), a);

			a += strlen(r);
			free(r);
			
			if (la.Tick())
				break;
		}
		
		regfree(&pb);
	}
	else
	{
		int fl, rl, offset = 0;
		fl = strlen(fFind);
		rl = strlen(fRepl);
		
		PLongAction la(fText);
		
		while (fText->FindNext((unsigned char *)fFind, offset, fIgnoreCase,
			false, false, fEntireWord, fGrep, false))
		{
			fText->Delete(offset, offset + fl);
			if (rl) fText->Insert(fRepl, rl, offset);
			offset += rl;
			
			if (la.Tick())
				break;
		}

		a = offset;
		c = a + rl;
	}
	
	Update();
	fText->Select(a, c, true, false);
} /* PReplaceAllCmd::Do */

void PReplaceAllCmd::Undo()
{
	fText->Select(0, 0, true, false);

	BFile f;
	FailOSErr(f.SetTo(fBackingStore.Path(), B_READ_WRITE));
	
	size_t size = f.Seek(0, SEEK_END);
	f.Seek(0, SEEK_SET);
	
	char *t = (char *)malloc(size);
	FailNil(t);
	CheckedRead(f, t, size);
	
	f.Seek(0, SEEK_SET);
	FailOSErr(f.SetSize(0));
	CheckedWrite(f, fText->Text(), fText->Size());

	fText->Delete(0, fText->Size());
	fText->Insert(t, size, 0);
	free(t);
	
	Update();
} /* PReplaceAllCmd::Undo */

void PReplaceAllCmd::Redo()
{
	Undo();
} /* PReplaceAllCmd::Redo */

const int
	kBufferSize = 1024;

PScriptCmd::PScriptCmd(PText *txt, const char *script)
	: PCmd("Script", txt)
{
	char buf[PATH_MAX];
	
	BPath p;
	BEntry e;
	FailOSErr(gAppDir.FindEntry("Extensions", &e, true));
	FailOSErr(e.GetPath(&p));
	
	strcpy(buf, p.Path());
	strcat(buf, "/");
	strcat(buf, script);

	fScript = strdup(buf);
	fOldText = NULL;
} /* PScriptCmd::PScriptCmd */

PScriptCmd::~PScriptCmd()
{
	if (fScript) free(fScript);
	if (fOldText) free(fOldText);
} /* PScriptCmd::~PScriptCmd */

void PScriptCmd::Do()
{
	fOldTextSize = abs(fText->Anchor() - fText->Caret());
	if (fOldTextSize)
	{
		fAnchor = fCaret = min(fText->Anchor(), fText->Caret());
		fOldText = (char *)malloc(fOldTextSize);
		FailNil(fOldText);
		fText->TextBuffer().Copy(fOldText, fAnchor, fOldTextSize);
	}
	else
		fCaret = fAnchor = fText->Caret();

	int pi[2], po[2], pe[3], pid, err;
	char e[PATH_MAX + 10], *cwd;
	
	BPath p;
	BEntry(fText->Doc()->File()).GetPath(&p);
	cwd = strdup(p.Path());
	sprintf(e, "PE_CUR_FILE=%s", cwd);
	char *t = strrchr(cwd, '/');
	if (t) *t = 0;
	
	sem_id wait_for_me = create_sem(0, "Wait for me!");
	
	try
	{
		FailOSErr(pipe(pi));
		FailOSErr(pipe(po));
		FailOSErr(pipe(pe));
		
		pid = fork();
			
		if (pid == 0)		// child
		{
			fPid = pid;
			setpgid(0, 0);		// Make this a new process group
			
			fflush(stdout);
			close(STDOUT_FILENO);
			err = dup(po[1]);		// What will happen if this fails???
			if (err < 0) perror("duplicating stdout"); //THROW((err));

			fflush(stderr);
			close(STDERR_FILENO);
			err = dup(pe[1]);
			if (err < 0) perror("duplicating stderr"); //THROW((err));
			
			close(STDIN_FILENO);
			err = dup(pi[0]);
			if (err < 0) perror("duplicating stdin");
			
			close(pi[0]);
			close(pi[1]);
			close(po[0]);
			close(po[1]);
			close(pe[0]);
			close(pe[1]);
			
			chdir(cwd);
			
			char *args[2];
			args[0] = fScript;
			args[1] = NULL;
			
			int i = 0;
			while (environ[i]) i++;
			
			char **env = (char **)malloc((i + 2) * sizeof(char*));
			
			memcpy(env, environ, i * sizeof(char *));
			env[i++] = e;
			env[i] = 0;
			
			release_sem(wait_for_me);
			
			if (execve(args[0], args, env))
			{
				beep();
				exit(1);
			}
		}
		else if (pid >= 0)	// parent
		{
			acquire_sem(wait_for_me);
			
			close(pi[0]);
			close(po[1]);
		
			fFD = pi[1];
			resume_thread(spawn_thread(Piper, "Piper", B_NORMAL_PRIORITY, this));
			
			new PErrorWindow(pe, cwd);
			
			if (fOldTextSize)
				fText->Delete(fAnchor, fAnchor + fOldTextSize);

			char buf[kBufferSize];
			int flags;
	
			fcntl(po[0], F_GETFL, &flags);
			fcntl(po[0], F_SETFL, flags | O_NONBLOCK);
			
			int rr;
			
			PLongAction act;
			
			while ((rr = read(po[0], buf, kBufferSize)) != 0 && ! act.Tick())
			{
				if (rr > 0)
				{
					fText->Insert(buf, rr, fCaret);
					fCaret += rr;
				}
				else if (errno == EAGAIN)
					snooze(100000);
				else
					break;
			}

			close(po[0]);
		}
		else
			FailOSErr(pid);
	}
	catch (HErr& e)
	{
		e.DoError();
	}

	delete_sem(wait_for_me);
	
	fText->Select(fAnchor, fCaret, true, false);
	Update();
} /* PScriptCmd::Do */

void PScriptCmd::Undo()
{
	size_t size = fCaret - fAnchor;
	char *t = NULL;
	
	if (size)
	{
		t = (char *)malloc(size);
		FailNil(t);
		fText->TextBuffer().Copy(t, fAnchor, size);
		fText->Delete(fAnchor, fCaret);
	}
	
	if (fOldTextSize)
		fText->Insert(fOldText, fOldTextSize, fAnchor);

	fCaret = fAnchor + fOldTextSize;
	fOldTextSize = size;
	if (fOldText) free(fOldText);
	fOldText = t;
	
	fText->Select(fAnchor, fCaret, true, false);
	Update();
} /* PScriptCmd::Undo */

void PScriptCmd::Redo()
{
	Undo();
} /* PScriptCmd::Redo */

long PScriptCmd::Piper(void *data)
{
	PScriptCmd *cmd = static_cast<PScriptCmd*>(data);
	
	write(cmd->fFD, cmd->fOldText, cmd->fOldTextSize);
	close(cmd->fFD);

	return 0;
} /* PScriptCmd::Piper */

#pragma mark -
#pragma mark --- Shift Command ---

PShiftCmd::PShiftCmd(const char *name, PText *txt)
	: PCmd(name, txt)
{
	fFrom = min(fText->Caret(), fText->Anchor());
	fTo = max(fText->Caret(), fText->Anchor());
	int fromLine = fText->Offset2Line(fFrom);
	int toLine = fText->Offset2Line(fTo);
	// our first line may be a soft wrapped one: find the preceding hard wrap
	while (fromLine > 0 && fText->SoftStart(fromLine))
		fromLine--;
	// find the hard wrap of the last line
	if (fromLine == toLine || fText->LineStart(toLine) < fTo)
		toLine++;
	while (toLine < fText->LineCount() && fText->SoftStart(toLine))
		toLine++;
	// now get the real offsets
	fFrom = fText->LineStart(fromLine);
	fTo = (toLine < fText->LineCount() ? fText->LineStart(toLine)
									   : fText->Size());
} /* PShiftCmd::PShiftCmd */

int PShiftCmd::FirstLine() const
{
	return fText->Offset2Line(fFrom);
} /* PShiftCmd::FirstLine */

int PShiftCmd::LastLine() const
{
	if (fTo == fText->Size())
	{
		// If the text's last line is empty, it shall only be included if
		// the selection is empty.
		int lineCount = fText->LineCount();
		if (fFrom < fTo && fTo == fText->LineStart(lineCount - 1))
			return lineCount - 1;
		return lineCount;
	}
	return fText->Offset2Line(fTo);
} /* PShiftCmd::LastLine */

PShiftLeftCmd::PShiftLeftCmd(PText *txt)
	: PShiftCmd("Shift Left", txt)
{
	// count the involved hard lines
	int firstLine = FirstLine();
	int lastLine = LastLine();
	fLineCount = 0;
	for (int i = firstLine; i < lastLine; i++)
	{
		if (!fText->SoftStart(i))
			fLineCount++;
	}
	// Get the first character of each hard line. If it's not a tab or a space,
	// we mark it 0.
	fFirstChars = new(nothrow) char[fLineCount];
	FailNil(fFirstChars);
	fNoOp = true;
	int hardIndex = 0;
	for (int i = firstLine; i < lastLine; i++)
	{
		if (!fText->SoftStart(i)) {
			char c = fText->TextBuffer()[fText->LineStart(i)];
			if (c == '\t' || c == ' ')
			{
				fFirstChars[hardIndex] = c;
				fNoOp = false;
			}
			else
				fFirstChars[hardIndex] = 0;
			hardIndex++;
		}
	}
} /* PShiftLeftCmd::PShiftLeftCmd */

PShiftLeftCmd::~PShiftLeftCmd()
{
	delete[] fFirstChars;
} /* PShiftLeftCmd::~PShiftLeftCmd */

void PShiftLeftCmd::Do()
{
	if (fNoOp)
		return;
	// remove the tabs/spaces
	int lineCount = fText->LineCount();
	int firstLine = FirstLine();
	int hardIndex = fLineCount - 1;
	for (int i = LastLine() - 1; i >= firstLine; i--)
	{
		if (!fText->SoftStart(i))
		{
			if (fFirstChars[hardIndex])
			{
				int ix = fText->LineStart(i);
				fText->Delete(ix, ix + 1);
				fTo--;
			}
			hardIndex--;
		}
	}
	// select and update the affected lines
	fText->Select(fFrom, fTo, true, false);
	if (fText->LineCount() != lineCount)
		Redraw();
	else
		Update();
} /* PShiftLeftCmd::Do */

void PShiftLeftCmd::Undo()
{
	if (fNoOp)
		return;
	// insert the tabs/spaces
	int lineCount = fText->LineCount();
	int firstLine = FirstLine();
	int hardIndex = fLineCount - 1;
	for (int i = LastLine() - 1; i >= firstLine; i--)
	{
		if (!fText->SoftStart(i))
		{
			if (fFirstChars[hardIndex])
			{
				fText->Insert(fFirstChars + hardIndex, 1, fText->LineStart(i));
				fTo++;
			}
			hardIndex--;
		}
	}
	// select and update the affected lines
	fText->Select(fFrom, fTo, true, false);
	if (fText->LineCount() != lineCount)
		Redraw();
	else
		Update();
} /* PShiftLeftCmd::Undo */


bool PShiftLeftCmd::IsNoOp()
{
	return fNoOp;
} /* PShiftLeftCmd::IsNoOp */


PShiftRightCmd::PShiftRightCmd(PText *txt)
	: PShiftCmd("Shift Right", txt)
{
} /* PShiftRightCmd::PShiftRightCmd */

void PShiftRightCmd::Do()
{
	// insert the tabs
	int lineCount = fText->LineCount();
	int firstLine = FirstLine();
	for (int i = LastLine() - 1; i >= firstLine; i--)
	{
		if (!fText->SoftStart(i))
		{
			fText->Insert("\t", 1, fText->LineStart(i));
			fTo++;
		}
	}
	// select and update the affected lines
	fText->Select(fFrom, fTo, true, false);
	if (fText->LineCount() != lineCount)
		Redraw();
	else
		Update();
} /* PShiftRightCmd::Do */

void PShiftRightCmd::Undo()
{
	// remove the tabs
	int lineCount = fText->LineCount();
	int firstLine = FirstLine();
	for (int i = LastLine() - 1; i >= firstLine; i--)
	{
		if (!fText->SoftStart(i))
		{
			int ix = fText->LineStart(i);
			fText->Delete(ix, ix + 1);
			fTo--;
		}
	}
	// select and update the affected lines
	fText->Select(fFrom, fTo, true, false);
	if (fText->LineCount() != lineCount)
		Redraw();
	else
		Update();
} /* PShiftRightCmd::Undo */

#pragma mark -
#pragma mark --- Twiddle Command ---

PTwiddleCmd::PTwiddleCmd(PText *txt)
	: PCmd("Twiddle", txt)
{
	fFrom = min(fText->Caret(), fText->Anchor());
	fTo = max(fText->Caret(), fText->Anchor());
	
	if (fFrom == fTo)
	{
		int line;
		line = fText->Offset2Line(fFrom);
		
		if (fFrom == fText->LineStart(line))
		{
			fTo += txt->TextBuffer().CharLen(fTo);
			fTo += txt->TextBuffer().CharLen(fTo);
		}
		else if (fFrom == fText->Size() || fFrom == fText->LineStart(line + 1) - 1)
		{
			fFrom -= txt->TextBuffer().PrevCharLen(fFrom);
			fFrom -= txt->TextBuffer().PrevCharLen(fFrom);
		}
		else
		{
			fTo += txt->TextBuffer().CharLen(fTo);
			fFrom -= txt->TextBuffer().PrevCharLen(fFrom);
		}
	}
	
	if (fText->TextBuffer()[fFrom] == '\n' ||
		fText->TextBuffer()[fTo - 1] == '\n' ||
		fTo > fText->Size() || fFrom < 0)
	{
		THROW((0));
	}
} /* PTwiddleCmd::PTwiddleCmd */

void PTwiddleCmd::Do()
{
	char *s;
	int size = fTo - fFrom;
	
	fText->ChangeSelection(fFrom, fTo);
	fText->GetSelectedText(s);

// reverse text, keep in mind that we might have long chars in between
	char *t = (char *)alloca(size + 1);
	int x = size;

	while (*s)
	{
		int l = mcharlen(s);
		x -= l;
		memcpy(t + x, s, l);
		s += l;
	}

	fText->Delete(fFrom, fTo);
	fText->Insert(t, size, fFrom);

	Redraw();
} /* PTwiddleCmd::Do */

void PTwiddleCmd::Undo()
{
	Do();
} /* PTwiddleCmd::Undo */

#pragma mark -
#pragma mark --- Comment Command ---

PCommentCmd::PCommentCmd(PText *txt, bool comment,
			const char *before, const char *after)
	: PCmd(comment ? "Comment" : "Uncomment", txt)
{
	fComment = comment;
	
	fFrom = min(fText->Caret(), fText->Anchor());
	fTo = max(fText->Caret(), fText->Anchor());
	
	if (strlen(before) > 7 || strlen(after) > 7) THROW(("comment strings too long"));
	
	strcpy(fBefore, before);
	strcpy(fAfter, after);
	
	if (fTo == fFrom || fTo > fText->LineStart(fText->Offset2Line(fTo)))
	{
		int lt = fText->Offset2Line(fTo);
		if (lt < fText->LineCount() - 1)
			fTo = fText->LineStart(lt + 1);
		else
			fTo = fText->Size();
	}
} /* PCommentCmd::PCommentCmd */

void PCommentCmd::Do()
{
	int i, lf, c;
	
	c = fText->LineCount();
	lf = fText->Offset2Line(fFrom);

	for (i = lf; i < fText->Offset2Line(fTo); i++)
	{
		if (! fText->SoftStart(i))
		{
			if (fComment)
				CommentLine(i);
			else if (UncommentLine(i))
				fLines.push_back(i);
		}
	}
	
	fText->Select(fText->LineStart(lf), fText->LineStart(fText->Offset2Line(fTo)), true, false);
	fFrom = min(fText->Caret(), fText->Anchor());
	fTo = max(fText->Caret(), fText->Anchor());

	if (fText->LineCount() != c)	
		Redraw();
	else
		Update();
} /* PCommentCmd::Do */

void PCommentCmd::Undo()
{
	int i, lf, c;

	c = fText->LineCount();
	lf = fText->Offset2Line(fFrom);
	
	if (fComment)
	{
		for (i = lf; i < fText->Offset2Line(fTo); i++)
			if (! fText->SoftStart(i))
				UncommentLine(i);
	}
	else
	{
		vector<int>::iterator ii;
		
		for (ii = fLines.begin(); ii != fLines.end(); ii++)
			CommentLine(*ii);
		
		fLines.erase(fLines.begin(), fLines.end());
	}
	
	fText->Select(fText->LineStart(lf), fText->LineStart(fText->Offset2Line(fTo)), true, false);
	fFrom = min(fText->Caret(), fText->Anchor());
	fTo = max(fText->Caret(), fText->Anchor());
	
	if (fText->LineCount() != c)	
		Redraw();
	else
		Update();
} /* PCommentCmd::Undo */

void PCommentCmd::CommentLine(int line)
{
	int ix = fText->LineStart(line);

	fText->Insert(fBefore, strlen(fBefore), ix);
	
	ix = fText->LineCount() > line ? fText->LineStart(line + 1) - 1 : fText->Size();
	fText->Insert(fAfter, strlen(fAfter), ix);
	
	fTo += strlen(fBefore) + strlen(fAfter);
} /* PCommentCmd::CommentLine */

bool PCommentCmd::UncommentLine(int line)
{
	char b[8];
	int j = strlen(fBefore);
	int ix = fText->LineStart(line);
	
	b[j] = 0;
	while (j--)
		b[j] = fText->TextBuffer()[ix + j];

	if (strcmp(b, fBefore) == 0)
	{
		fText->Delete(ix, ix + strlen(fBefore));

		if (strlen(fAfter))
		{
			ix = line < fText->LineCount() - 1 ? fText->LineStart(line + 1) : fText->Size();
			j = strlen(fAfter);
			ix -= j + 1;
			
			b[j] = 0;
			while (j--)
				b[j] = fText->TextBuffer()[ix + j];
	
			if (strcmp(b, fAfter) == 0)
				fText->Delete(ix, ix + strlen(fAfter));
		}

		fTo -= strlen(fBefore) + strlen(fAfter);

		return true;
	}
	else
		return false;
} /* PCommentCmd::UncommentLine */

#pragma mark -
#pragma mark --- Wrap Command ---

PWrapCmd::PWrapCmd(PText *txt)
	: PCmd("Wrap", txt)
{
	fCaret = txt->Caret();
	fAnchor = txt->Anchor();
	fSavedText = NULL;
} /* PWrapCmd::PWrapCmd */

PWrapCmd::~PWrapCmd()
{
	if (fSavedText) free(fSavedText);
} /* PWrapCmd::PWrapCmd */

void PWrapCmd::Do()
{
	fText->Select(fAnchor, fCaret, true, false);
	fText->GetSelectedText(fSavedText);
	fText->InsertLinebreaks();
	fNewLen = abs(fText->Caret() - fText->Anchor());
	Redraw();
} /* PWrapCmd::Do */

void PWrapCmd::Undo()
{
	int a = min(fAnchor, fCaret);
	fText->Delete(a, a + fNewLen);
	if (fSavedText)
	{
		fText->Insert(fSavedText, strlen(fSavedText), a);
		free(fSavedText);
		fSavedText = NULL;
	}
	fText->Select(fAnchor, fCaret, true, false);
	
	Redraw();
} /* PWrapCmd::Undo */

#pragma mark -
#pragma mark --- Unwrap Command ---

PUnwrapCmd::PUnwrapCmd(PText *txt)
	: PCmd("Unwrap", txt)
{
	fCaret = txt->Caret();
	fAnchor = txt->Anchor();
	
	if (fCaret == fAnchor)
	{
		fAnchor = 0;
		fCaret = fText->Size() - 1;
	}
	
	fSavedText = NULL;
} /* PUnwrapCmd::PUnwrapCmd */

PUnwrapCmd::~PUnwrapCmd()
{
	if (fSavedText) free(fSavedText);
} /* PUnwrapCmd::~PUnwrapCmd */

void PUnwrapCmd::Do()
{
	fText->Select(fAnchor, fCaret, true, false);
	fText->GetSelectedText(fSavedText);
	fText->RemoveLinebreaks();
	fNewLen = abs(fText->Caret() - fText->Anchor());
	Redraw();
} /* PUnwrapCmd::Do */

void PUnwrapCmd::Undo()
{
	int a = min(fAnchor, fCaret);
	fText->Delete(a, a + fNewLen);
	if (fSavedText)
	{
		fText->Insert(fSavedText, strlen(fSavedText), a);
		free(fSavedText);
		fSavedText = NULL;
	}
	fText->Select(fAnchor, fCaret, true, false);
	
	Redraw();
} /* PUnwrapCmd::Undo */

#pragma mark -
#pragma mark --- Justify Command ---

PJustifyCmd::PJustifyCmd(PText *txt)
	: PCmd("Justify", txt)
{
} /* PJustifyCmd::PJustifyCmd */

PJustifyCmd::~PJustifyCmd()
{
	if (fSavedText) free(fSavedText);
} /* PJustifyCmd::~PJustifyCmd */

void PJustifyCmd::Do()
{
	if (fText->Caret() == fText->Anchor())
		fText->SelectParagraph();
	
	fAnchor = min(fText->Anchor(), fText->Caret());
	fOldLen = max(fText->Anchor(), fText->Caret()) - fAnchor;
	
	fText->GetSelectedText(fSavedText);
	
	char *t = (char *)malloc(strlen(fSavedText) + 1);
	FailNil(t);
	
	char *p, *q;
	
	p = fSavedText;
	q = t;
	
	while (*p)
	{
		if (isspace(*p))
		{
			while (isspace(*++p))
				;
			*q++ = ' ';
		}
		else
			*q++ = *p++;
	}
	
	*q = 0;
	fNewLen = strlen(t);
	
	fText->Delete(fAnchor, fAnchor + fOldLen);
	fText->Insert(t, fNewLen, fAnchor);
	fText->Select(fAnchor, fAnchor + fNewLen, true, false);
	fText->RewrapLines(fAnchor, fAnchor + fNewLen, true);
	Update();
	fText->InsertLinebreaks();
	fNewLen = abs(fText->Anchor() - fText->Caret());
} /* PJustifyCmd::Do */

void PJustifyCmd::Undo()
{
	char *t;
	
	fText->Select(fAnchor, fAnchor + fNewLen, true, false);
	fText->GetSelectedText(t);
	
	fText->Delete(fAnchor, fAnchor + fNewLen);
	fText->Insert(fSavedText, fOldLen, fAnchor);
	
	swap(fSavedText, t);
	free(t);
	swap(fOldLen, fNewLen);
	
	fText->Select(fAnchor, fAnchor + fNewLen, true, false);
	
	Update();
} /* PJustifyCmd::Undo */

void PJustifyCmd::Redo()
{
	Undo();
} /* PJustifyCmd::Redo */

#pragma mark -
#pragma mark --- Change Case Command ---

PChangeCaseCmd::PChangeCaseCmd(PText *txt, int newCase)
	: PCmd("Change Case", txt)
{
	fText->GetSelectedText(fSavedText);
	
	if (fSavedText)
	{
		fAnchor = min(fText->Anchor(), fText->Caret());
		fCaret = max(fText->Anchor(), fText->Caret());
	}
	else
	{
		fAnchor = 0;
		fCaret = fText->Size();
		fSavedText = strdup(fText->Text());
	}
	
	ChangeCase(newCase);
} /* PChangeCaseCmd::PChangeCaseCmd */

PChangeCaseCmd::~PChangeCaseCmd()
{
	free(fSavedText);
} /* PChangeCaseCmd::~PChangeCaseCmd */

void PChangeCaseCmd::Do()
{
	char *p;
	
	fText->Select(fAnchor, fCaret, true, false);
	fText->GetSelectedText(p);
	fText->Delete(fAnchor, fCaret);
	fText->Insert(fSavedText, fCaret - fAnchor, fAnchor);
	
	free(fSavedText);
	fSavedText = p;
		
	Redraw();
} /* PChangeCaseCmd::Do */

void PChangeCaseCmd::Undo()
{
	Do();
} /* PChangeCaseCmd::Undo */

void PChangeCaseCmd::ChangeCase(int newCase)
{
	char *p = fSavedText;
	int state = 0;
	
	switch (newCase)
	{
		case 1:
			while (*p)
			{
				if (isalpha(*p))
					*p = tolower(*p);
				p++;
			}
			break;
		case 2:
			while (*p)
			{
				if (isalpha(*p))
					*p = toupper(*p);
				p++;
			}
			break;
		case 3:
			while (*p)
			{
				if (isalnum(*p) && state == 0)
				{
					*p = toupper(*p);
					state = 1;
				}
				else if (isalnum(*p) && state == 1)
					*p = tolower(*p);
				else if (!isalnum(*p) && state == 1)
					state = 0;
				p++;
			}
			break;
		case 4:
			while (*p)
			{
				if (isalnum(*p) && state == 0)
				{
					*p = toupper(*p);
					state = 1;
				}
//				else if (isalnum(*p) && state == 1)
//					*p = tolower(*p);
				else if (*p == '.' && state == 1)
					state = 0;
				p++;
			}
			break;
		case 5:
			while (*p)
			{
				if (isalnum(*p) && state == 0)
				{
					*p = toupper(*p);
					state = 1;
				}
//				else if (isalnum(*p) && state == 1)
//					*p = tolower(*p);
				else if (*p == '\n' && state == 1)
					state = 0;
				p++;
			}
			break;
	}
} /* PChangeCaseCmd::ChangeCase */

#pragma mark -
#pragma mark --- Change Encoding Command ---

PEncodingCmd::PEncodingCmd(PText *txt, int from, int to)
	: PCmd("Change Encoding", txt)
{
	fSourceEncoding = from;
	fDestEncoding = to;
	
	fAnchor = txt->Anchor();
	fCaret = txt->Caret();
	
	if (fAnchor == fCaret)
	{
		fAnchor = 0;
		fCaret = txt->Size();
	}
	
	fSaved = (char *)malloc(fCaret - fAnchor);
	fSrcLen = fCaret - fAnchor;
	
	FailNil(fSaved);
	txt->TextBuffer().Copy(fSaved, fAnchor, fSrcLen);
	
	fPrevEncoding = txt->Encoding();
} /* PEncodingCmd::PEncodingCmd */

PEncodingCmd::~PEncodingCmd()
{
	if (fSaved) free(fSaved);
} /* PEncodingCmd::~PEncodingCmd */

void PEncodingCmd::Do()
{
	if (fSourceEncoding >= 0 && fDestEncoding >= 0)
	{
		long tl = 3 * fSrcLen;
		char *p1 = (char *)malloc(tl);
		FailNil(p1);
		int32 state = 0;
		
		FailOSErr(convert_to_utf8(fSourceEncoding, fSaved, &fSrcLen, p1, &tl, &state));
		
		fDstLen = 2 * tl;
		char *p2 = (char *)malloc(fDstLen);
		FailNil(p2);
		
		FailOSErr(convert_from_utf8(fDestEncoding, p1, &tl, p2, &fDstLen, &state));
		
		free(p1);
		
		fCaret = fAnchor + fSrcLen;
		fText->Delete(fAnchor, fCaret);
		fText->Insert(p2, fDstLen, fAnchor);
		
		free(p2);
		
		fText->SetEncoding(fDestEncoding + 1);
		if (fText->Caret() != fText->Anchor())
			fText->SetCaret(fAnchor + fDstLen);
	}
	else if (fSourceEncoding >= 0)
	{
		fDstLen = 3 * fSrcLen;
		
		char *p = (char *)malloc(fDstLen);
		FailNil(p);
		
		int32 state = 0;

		FailOSErr(convert_to_utf8(fSourceEncoding, fSaved, &fSrcLen,
			p, &fDstLen, &state));

		fCaret = fAnchor + fSrcLen;
		fText->Delete(fAnchor, fCaret);
		fText->Insert(p, fDstLen, fAnchor);
		free(p);
		
		fText->SetEncoding(0);
		if (fText->Caret() != fText->Anchor())
			fText->SetCaret(fAnchor + fDstLen);
	}
	else if (fDestEncoding >= 0)
	{
		fDstLen = fSrcLen * 2;
		
		char *p = (char *)malloc(fDstLen);
		FailNil(p);
		
		int32 state = 0;
		
		FailOSErr(convert_from_utf8(fDestEncoding, fSaved, &fSrcLen,
			p, &fDstLen, &state));

		fCaret = fAnchor + fSrcLen;
		fText->Delete(fAnchor, fCaret);
		fText->Insert(p, fDstLen, fAnchor);
		free(p);
		
		fText->SetEncoding(fDestEncoding + 1);
		if (fText->Caret() != fText->Anchor())
			fText->SetCaret(fAnchor + fDstLen);
	}

	Update();
} /* PEncodingCmd::Do */

void PEncodingCmd::Undo()
{
	fText->Delete(fAnchor, fAnchor + fDstLen);
	fText->Insert(fSaved, fSrcLen, fAnchor);
	fText->SetEncoding(fSourceEncoding + 1);
	fText->Select(fAnchor, fCaret, true, false);
	
	Update();
} /* PEncodingCmd::Undo */

