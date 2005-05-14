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

	Created: long ago
*/

#include "pe.h"

#include <regex.h>

#include "PText.h"
#include "PDoc.h"
#include "CFindDialog.h"
#include "PMessages.h"
#include "PApp.h"
#include "PMessageWindow.h"
#include "Utils.h"
#include "HError.h"
#include "HPreferences.h"
#include "PLongAction.h"
#include "MAlert.h"

const unsigned long
	msg_SelectDirectory = 'SelO',
	msg_SelectedDir = 'SelD',
	msg_YASD = 'SYAD',
	msg_ChangedMFKind = 'MFKn',
	msg_GrepPopup = 'GrPp',
	msg_AddGrepPattern = 'addP',
	msg_Collapse = 'clps';

CFindDialog* gFindDialog;

CFindDialog::CFindDialog(BRect frame, const char *name,
		window_type type, int flags, BWindow *owner, BPositionIO& data)
	: HDialog(frame, name, type, flags, owner, data)
{
	gFindDialog = this;
	fCurrentDir = NULL;
	
	char *bi = strdup(getenv("BEINCLUDES"));
	
	int i = 1;
	const char *p = bi;
	while ((p = strchr(p + 1, ';')) != NULL)
		i++;

	fBeIncludes = (char **)malloc(i * sizeof(char *));
	FailNil(fBeIncludes);
	
	p = bi;
	i = 0;
	
	do
	{
		char *ep = strchr(p, ';');
		if (ep) *ep++ = 0;
		
		fBeIncludes[i] = strdup(p);
		FailNil(fBeIncludes[i++]);
		p = ep;
	}
	while (p);
	
	free(bi);
	fBeIncludeCount = i;
	
	fDirPanel = NULL;
	
	memset(&fPatternBuffer, 0, sizeof(regex_t));

	SetOn("case", gPrefs->GetPrefInt("Search Ignore Case", 1));
	SetOn("wrap", gPrefs->GetPrefInt("Search Wrap", 1));
	SetOn("back", gPrefs->GetPrefInt("Search Backwards", 0));
	SetOn("word", gPrefs->GetPrefInt("Search Entire Word", 0));
	SetOn("text", gPrefs->GetPrefInt("Search Text Files Only", 1));
	if (gRxInstalled)
		SetOn("grep", gPrefs->GetPrefInt("Search with Grep", 0));
	else
	{
		SetOn("grep", false);
		SetEnabled("grep", false);
	}

	SetOn("btch", gPrefs->GetPrefInt("Search Batch", 0));
	SetOn("recu", gPrefs->GetPrefInt("Search Recursive", 1));
	
	BMenuField *mf = dynamic_cast<BMenuField*>(FindView("meth"));
	fKind = mf->Menu();
	BMenuItem *item = fKind->ItemAt(gPrefs->GetPrefInt("Search Multikind", 0));
	(item ? item : fKind->ItemAt(0))->SetMarked(true);
	fKind->SetRadioMode(true);
	
	mf = dynamic_cast<BMenuField*>(FindView("sdir"));
	fDirectory = mf->Menu();
	fDirectory->SetRadioMode(true);
	
	fName = dynamic_cast<BMenuField*>(FindView("namp"))->Menu();
	fName->SetRadioMode(true);
	fName->ItemAt(0)->SetMarked(true);

	i = 0;
	while ((p = gPrefs->GetIxPrefString("searchpath", i++)) != NULL)
		AddPathToDirMenu(p, false);

	item = fDirectory->ItemAt(gPrefs->GetPrefInt("Search Whichdir", 0));
	(item ? item : fDirectory->ItemAt(0))->SetMarked(true);

	static_cast<BTextControl*>(FindView("find"))->TextView()->SetWordWrap(true);
	static_cast<BTextControl*>(FindView("repl"))->TextView()->SetWordWrap(true);
	
	mf = dynamic_cast<BMenuField*>(FindView("pats"));
	fGrepPopup = mf->Menu();
	fGrepPopup->SetRadioMode(false);

	SetDefaultButton(dynamic_cast<BButton*>(FindView("bfnd")));
	
	fOpenWindowIndex = -1;
	
	UpdateFields();
//	FillGrepPopup();
} /* CFindDialog::CFindDialog */

bool CFindDialog::QuitRequested()
{
	do
		Hide();
	while (!IsHidden())	;

	gPrefs->SetPrefInt("Search Backwards", IsOn("back"));
	gPrefs->SetPrefInt("Search Entire Word", IsOn("word"));
	gPrefs->SetPrefInt("Search Ignore Case", IsOn("case"));
	gPrefs->SetPrefInt("Search Wrap", IsOn("wrap"));
	gPrefs->SetPrefInt("Search Text Files Only", IsOn("text"));
	gPrefs->SetPrefInt("Search with Grep", IsOn("grep"));
	gPrefs->SetPrefInt("Search Batch", IsOn("btch"));
	gPrefs->SetPrefInt("Search Recursive", IsOn("recu"));
	gPrefs->SetPrefInt("Search Multikind", fKind->IndexOf(fKind->FindMarked()));
	gPrefs->SetPrefInt("Search Whichdir", fDirectory->IndexOf(fDirectory->FindMarked()));

	gPrefs->RemovePref("searchpath");
	
	for (int i = 2; i < fDirectory->CountItems(); i++)
		gPrefs->SetIxPrefString("searchpath", i - 2, fDirectory->ItemAt(i)->Label());

	return CDoc::CountDocs() == 0;
} /* CFindDialog::QuitRequested */

void CFindDialog::DoFind(unsigned long cmd)
{
	if (gRxInstalled && IsOn("grep"))
	{
		int r = rx_regcomp(&fPatternBuffer, GetText("find"), IsOn("case"));
		
		if (r)
		{
			char err[100];
			regerror(r, &fPatternBuffer, err, 100);
			MWarningAlert a(err);
			a.Go();
			return;
		}
	}
	
	if (IsOn("mult"))
	{
		switch (fKind->IndexOf(fKind->FindMarked()))
		{
			case 0:
			{
				PDoc *w = NULL;
				
				MultiReplaceKind all = mrNone;
				
				if (cmd == msg_ReplaceAll)
				{
					switch (MAlert("Replace All on multiple files", "Cancel", "Leave Open", "Save To Disk").Go())
					{
						case 1:	return;
						case 2:	all = mrLeaveOpen; break;
						case 3:	all = mrSaveToDisk; break;
					}
				}
				
				if (!DoMultiFileFind(fDirectory->FindMarked()->Label(),
					IsOn("recu"), true, all, &w))
					beep();
				else if (w)
					w->Show();
				break;

			}
			
			case 1:
				fOpenWindowIndex = -1;
				DoOpenWindows(cmd == msg_ReplaceAll);
				break;
			
			case 2:
				fCurrentIncludeIndex = -1;
				DoIncludesFind();
				break;
			
			case 3:
				break;
		}
	}
	else
	{
		BMessage msg(cmd);
		
		msg.AddString("what", GetText("find"));
		msg.AddString("with", GetText("repl"));
		
		msg.AddBool("wrap", IsOn("wrap"));
		msg.AddBool("case", IsOn("case"));
		msg.AddBool("word", IsOn("word"));
		msg.AddBool("back", IsOn("back"));
		msg.AddBool("grep", IsOn("grep"));

		PDoc *w = PDoc::TopWindow();

		if (w)
			w->PostMessage(&msg, w->TextView(), this);
	}
} /* CFindDialog::DoFind */

void CFindDialog::WindowActivated(bool active)
{
	HDialog::WindowActivated(active);
	UpdateFields();
	FindView("find")->MakeFocus();
} /* CFindDialog::WindowActivated */

void CFindDialog::UpdateFields()
{
	PDoc *w = PDoc::TopWindow();

	if (w && strlen(GetText("find")))
	{
		BMessage query(msg_QueryCanReplace);
		query.AddString("what", GetText("find"));
		query.AddBool("case", IsOn("case"));
		query.AddBool("grep", IsOn("grep"));
		w->PostMessage(&query, w->TextView(), this);
	}
	else
	{
		SetEnabled("brpl", false);
		SetEnabled("br&f", false);
	}

	if (IsOn("mult"))
	{
		int kind = GetValue("meth") - 1;

		SetEnabled("meth", true);
		SetEnabled("sdir", kind == 0);
		SetEnabled("name", kind == 0);
		SetEnabled("namp", kind == 0);
		SetEnabled("text", kind == 0);
		SetEnabled("recu", kind == 0 || kind == 2);
		
		if (kind == 2)
			SetOn("text", true);

		int nmKind =
			fName->IndexOf(fName->FindMarked());
		SetEnabled("name", nmKind != 0);
	}
	else
	{
		SetEnabled("meth", false);
		SetEnabled("sdir", false);
		SetEnabled("text", false);
		SetEnabled("name", false);
		SetEnabled("namp", false);
		SetEnabled("recu", false);
		
		fCurrentDir = NULL;
		while (fDirStack.size())
		{
			closedir(fDirStack.top());
			fDirStack.pop();
			
			char *p = fDirNameStack.top();
			free(p);
			fDirNameStack.pop();
		}

		fCurrentIncludeIndex = -1;
		fOpenWindowIndex = -1;
	}
	
	SetEnabled("ball", !(IsOn("mult") && GetValue("meth") == 3));
} /* CFindDialog::UpdateFields */

void CFindDialog::MessageReceived(BMessage *msg)
{
	try
	{
		long what = msg->what;

		if (msg->WasDropped() && msg->HasRef("refs"))
		{
			entry_ref ref;
			FailOSErr(msg->FindRef("refs", &ref));
			BEntry e(&ref);
			
			if (!e.IsDirectory())
				BEntry(&ref).GetParent(&e); // nasty huh?

			fKind->ItemAt(0)->SetMarked(true);

			SetOn("mult", true);
			BPath path;
			e.GetPath(&path);
			
			AddPathToDirMenu(path.Path(), true);
			UpdateFields();
		}
		else switch (what)
		{
			case msg_Find:
			case msg_Replace:
			case msg_ReplaceAndFind:
			case msg_ReplaceAll:
				DoFind(what);
				break;
	
			case msg_ReplyCanReplace:
			{
				bool canReplace;
				if (msg->FindBool("canreplace", &canReplace) == B_NO_ERROR)
				{
					SetEnabled("brpl", canReplace);
					SetEnabled("br&f", canReplace);
				}
				break;
			}
			
			case msg_EnterSearchString:
			case msg_FindSelection:
			case msg_FindSelectionBackward:
			{
				SetOn("mult", false);
				SetOn("grep", false);
				UpdateFields();
				const char *s;
				if (msg->FindString("string", &s) == B_NO_ERROR)
					SetText("find", s);
				if (what == msg_FindSelection)
					DoFind(msg_Find);
				else if (what == msg_FindSelectionBackward)
					DoFind(msg_FindAgainBackward);
				break;
			}
				
			case msg_EnterReplaceString:
			{
				SetOn("mult", false);
				SetOn("grep", false);
				UpdateFields();
				const char *s;
				if (msg->FindString("string", &s) == B_NO_ERROR)
					SetText("repl", s);
				break;
			}
			
			case msg_FindInNextFile:
			{
				switch (fKind->IndexOf(fKind->FindMarked()))
				{
					case 0:
					{
						PDoc *w = NULL;
						if (!DoMultiFileFind(fDirectory->FindMarked()->Label(),
							IsOn("recu"), false, mrNone, &w))
							beep();
						else if (w)
							w->Show();
						break;
					}
					
					case 1:
						DoOpenWindows(false);
						break;
					
					case 2:
						DoIncludesFind();
						break;
				}
				break;
			}
			case msg_SelectDirectory:
				if (!fDirPanel)
				{
					BMessage msg(msg_SelectedDir);
					fDirPanel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this), NULL,
						B_DIRECTORY_NODE, false, &msg);
				}
				fDirPanel->Show();
				break;
			
			case msg_SelectedDir:
			{
				entry_ref ref;
				if (msg->FindRef("refs", &ref) == B_NO_ERROR)
					AddPathToDirMenu(ref, true);
				break;
			}
			case msg_YASD:
			case msg_ChangedMFKind:
				SetOn("mult", true);
				UpdateFields();
				break;
			
			case msg_GrepPopup:
			{
				long ix;
				FailOSErr(msg->FindInt32("index", &ix));
				
				ix -= 2;
				SetText("find", gPrefs->GetIxPrefString("greppatfind", ix));
				SetText("repl", gPrefs->GetIxPrefString("greppatrepl", ix));
				SetOn("grep", true);
				break;
			}
			
			case msg_PrefsChanged:
				FillGrepPopup();
				break;
			
			case msg_AddGrepPattern:
			{
				int ix = fGrepPopup->CountItems() - 1;
				char n[32];
				sprintf(n, "Grep Pattern %d", ix);
				ix--;
				gPrefs->SetIxPrefString("greppatname", ix, n);
				gPrefs->SetIxPrefString("greppatfind", ix, GetText("find"));
				gPrefs->SetIxPrefString("greppatrepl", ix, GetText("repl"));
				FillGrepPopup();
				
				static_cast<PApp*>(be_app)->PostMessage(msg_AddGrepPattern);
				break;
			}
			
			default:
				HDialog::MessageReceived(msg);
				break;
		}
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* CFindDialog::MessageReceived */

void CFindDialog::AddPathToDirMenu(const char *path, bool select)
{
	int i;

	for (i = 2; i < fDirectory->CountItems(); i++)
	{
		if (!strcmp(fDirectory->ItemAt(i)->Label(), path))
		{
			if (select) fDirectory->ItemAt(i)->SetMarked(true);
			return;
		}
	}

	if (fDirectory->CountItems() == 1)
		fDirectory->AddSeparatorItem();
	
	fDirectory->AddItem(new BMenuItem(path, new BMessage(msg_YASD)));

	if (select)
		fDirectory->ItemAt(fDirectory->CountItems() - 1)->SetMarked(true);
} /* CFindDialog::AddPathToDirMenu */

void CFindDialog::AddPathToDirMenu(entry_ref& ref, bool select)
{
	BEntry e;
	FailOSErr(e.SetTo(&ref));
	BPath p;
	FailOSErr(e.GetPath(&p));
	
	AddPathToDirMenu(p.Path(), select);
} /* CFindDialog::AddPathToDirMenu */

const char* CFindDialog::FindString()
{
	BAutolock lock(this);
	return GetText("find");
} /* CFindDialog::FindString */

const char* CFindDialog::ReplaceString()
{
	BAutolock lock(this);
	return GetText("repl");
} /* CFindDialog::FindString */

const regex_t* CFindDialog::PatternBuffer()
{
	regfree(&fPatternBuffer);
	
	int r = rx_regcomp(&fPatternBuffer, GetText("find"), IsOn("case"));
	if (r != REG_NOERROR)
	{
		char err[100];
		regerror(r, &fPatternBuffer, err, 100);
		THROW((err));
	}
	
	return &fPatternBuffer;
}

void CFindDialog::FillGrepPopup()
{
	while (fGrepPopup->CountItems() > 2)
	{
		BMenuItem *mi = fGrepPopup->RemoveItem(2);
		delete mi;
	}
	
	int i = 0;
	const char *name;
	
	do
	{
		name = gPrefs->GetIxPrefString("greppatname", i++);
		if (name)
			fGrepPopup->AddItem(new BMenuItem(name, new BMessage(msg_GrepPopup)));
	}
	while (name);
} /* CFindDialog::FillGrepPopup */

#pragma mark - Multi file

bool CFindDialog::DoMultiFileFind(const char *dir, bool recursive, bool restart, MultiReplaceKind replace, PDoc **w)
{
	char path[PATH_MAX];
	bool batch = IsOn("btch");
	vector<PMessageItem*> *lst = NULL;
	
	if (batch && replace == mrNone)
		lst = new vector<PMessageItem*>;

	if (restart)
	{
		while (fDirStack.size())
		{
			closedir(fDirStack.top());
			fDirStack.pop();

			free(fDirNameStack.top());
			fDirNameStack.pop();
		}
		fCurrentDir = opendir(dir);
		if (!fCurrentDir) FailOSErr(errno);
		fDirStack.push(fCurrentDir);
		fDirNameStack.push(strdup(dir));
	}

	if (!fCurrentDir) THROW(("Directory not found: %s", dir));
	
	struct dirent *dent;
	struct stat stbuf;
	
	PLongAction la;
	bool cancel = false;
	
	while (fDirStack.size() && ! cancel)
	{
		fCurrentDir = fDirStack.top();
		dir = fDirNameStack.top();
		
		while ((dent = readdir(fCurrentDir)) != NULL && ! cancel)
		{
			strcpy(path, dir);
			strcat(path, "/");
			strcat(path, dent->d_name);
			status_t err = stat(path, &stbuf);
			
			if (err || !(strcmp(dent->d_name, ".") && strcmp(dent->d_name, "..")))
				continue;
			
			if (S_ISREG(stbuf.st_mode))
			{
				entry_ref ref;
				if (GetRefForPath(ref, path) && FindInFile(ref, lst))
				{
					if (replace == mrLeaveOpen)
					{
						PDoc *doc = dynamic_cast<PDoc*>(gApp->OpenWindow(ref));
						if (doc)
						{
							BAutolock lock(doc);
		
							doc->TextView()->Find(msg_ReplaceAll, NULL);
						}
					}
					else if (replace == mrSaveToDisk)
					{
						PDoc *doc = dynamic_cast<PDoc*>(gApp->OpenWindow(ref, false));
						if (doc)
						{
							doc->Lock();
		
							doc->TextView()->Find(msg_ReplaceAll, NULL);
							
							if (doc->IsHidden())
							{
								doc->Save();
								doc->Quit();
							}
							else
								doc->Unlock();
						}
					}
					else if (replace == mrNone && !batch)
					{
						PDoc *doc = dynamic_cast<PDoc*>(gApp->OpenWindow(ref));
						if (doc)
						{
							BAutolock lock(doc);
		
							int offset = 0;
							
							doc->TextView()->FindNext((unsigned char *)GetText("find"),
								offset, IsOn("case"), false, false, false, IsOn("grep"), true);
							return true;
						}
					}
				}
			}
			else if (S_ISDIR(stbuf.st_mode) && recursive)
			{
				DIR *r = opendir(path);
				if (!r) FailOSErr(errno);
				fDirStack.push(r);
				fDirNameStack.push(strdup(path));
				fCurrentDir = NULL;
				break;
			}
			
			cancel = la.Tick();
		}

		if (fCurrentDir)
		{
			closedir(fDirStack.top());
			fDirStack.pop();
			fCurrentDir = NULL;
			
			free(fDirNameStack.top());
			fDirNameStack.pop();
			dir = NULL;
		}
	}
	
// not found... or batch of course!

	if (lst)
	{
		ShowBatch(lst, (BWindow**)w);
		delete lst;
	}
	
	return false;
} /* CFindDialog::DoMultiFileFind */

bool CFindDialog::GetRefForPath(entry_ref& ref, const char *path)
{
	bool result = true;
	FailOSErr(get_ref_for_path(path, &ref));

	if (IsOn("text"))
	{
		BNode n;
		
		FailOSErr(n.SetTo(&ref));

		char mime[64];
		BNodeInfo(&n).GetType(mime);

		if (strncmp(mime, "text/", 5))
			result = false;
	}
	
	int name = fName->IndexOf(fName->FindMarked());
	if (result && name)
	{
		const char *pat = GetText("name");
		char *file = strrchr(path, '/') + 1;
	
		switch (name)
		{
			case 1:	result = strcmp(file + strlen(file) - strlen(pat), pat) == 0; break;
			case 2: result = strncmp(file, pat, strlen(pat)) == 0; break;
			case 3: result = strstr(file, pat) != NULL; break;
		}
	}

	return result;
} /* CFindDialog::GetRefForPath */

bool CFindDialog::FindInFile(const entry_ref& ref, vector<PMessageItem*> *lst)
{
	bool found;
	bool word = IsOn("word");
	const char *what = GetText("find");
	PDoc *doc;

	BEntry e;
	FailOSErr(e.SetTo(&ref));
	BPath p;
	FailOSErr(e.GetPath(&p));
	const char *path = p.Path();
	
	doc = dynamic_cast<PDoc*>(CDoc::FindDoc(ref));

	if (doc)
	{
		BAutolock lock(doc);

		PText *txt = doc->TextView();
		if (gRxInstalled && IsOn("grep"))
			found = BufferContainsEx(txt->Text(), txt->Size(), path,
				&fPatternBuffer, word, lst);
		else
			found = BufferContains(txt->Text(), txt->Size(), path, what, IsOn("case"), word, lst);
	}
	else if (gRxInstalled && IsOn("grep"))
		found = FileContainsEx(path, &fPatternBuffer, word, lst);
	else
		found = FileContains(path, what, IsOn("case"), word, lst);
	
	return found;
} /* CFindDialog::FindInFile */

void CFindDialog::DoIncludesFind()
{
	PDoc *w = NULL;
	
	if (fCurrentIncludeIndex > -1)
	{
		if (fCurrentDir && DoMultiFileFind(fBeIncludes[fCurrentIncludeIndex], false, false, mrNone, &w))
			return;
		else
			fCurrentIncludeIndex++;
	}
	else
		fCurrentIncludeIndex = 0;
		
	do
	{
		if (DoMultiFileFind(fBeIncludes[fCurrentIncludeIndex], false, true, mrNone, &w))
			return;
	}
	while (++fCurrentIncludeIndex < fBeIncludeCount);

	if (w)
		w->Show();
		
	fCurrentIncludeIndex = -1;
	beep();
} /* CFindDialog::DoBeIncludesFind */

void CFindDialog::DoOpenWindows(bool replace)
{
//	if (IsOn("btch"))
//	{
//		int i = be_app->CountWindows();
//		BList *lst = new BList;
//		
//		while (i--)
//		{
//			PDoc *doc = dynamic_cast<PDoc*>(be_app->WindowAt(i));
//			if (doc)
//				FindInFile(*doc->File(), lst);
//		}
//
//		BWindow *w = NULL;
//		ShowBatch(lst, &w);
//	}
//	else
//	{
		if (fOpenWindowIndex == -1)
		{
			fOpenWindows.clear();
	
			int i = be_app->CountWindows();
			
			while (i--)
			{
				PDoc *doc = dynamic_cast<PDoc*>(be_app->WindowAt(i));
				if (doc)
					fOpenWindows.push_back(doc);
			}
			
			fOpenWindowIndex = fOpenWindows.size();
		}
	
		while (--fOpenWindowIndex >= 0)
		{
			PDoc* doc = fOpenWindows[fOpenWindowIndex];

			const entry_ref& file = *doc->File();

			if (doc && FindInFile(file, NULL))
			{
				BAutolock lock(doc);
				
				if (!lock.IsLocked())
					continue;
				doc->Activate();
				if (replace)
					doc->TextView()->Find(msg_ReplaceAll, NULL);
				else
				{
					int offset = 0;
					doc->TextView()->FindNext((unsigned char *)GetText("find"), offset,
						IsOn("case"), false, false, IsOn("word"), IsOn("grep"), true);
					return;
				}
			}
		}
//	}
	
	beep();
} /* CFindDialog::DoOpenWindows */

void CFindDialog::ShowBatch(vector<PMessageItem*> *lst, BWindow** w)
{
	if (lst && lst->size())
	{
		PMessageWindow *m;
		
		if (w && *w)
			m = static_cast<PMessageWindow*>(*w);
		else
			m = new PMessageWindow("Search Results");

		BAutolock lock(m);
		
		if (lock.IsLocked())
		{
			if (w) *w = m;
	
			for (int i = 0; i < lst->size(); i++)
				m->AddMessage(static_cast<PMessageItem*>((*lst)[i]));
	
			if (w && *w)
				m->Show();
		}
	}
} /* CFindDialog::ShowBatch */

#pragma mark - Find


bool gRxInstalled = false;

void initskip(const unsigned char *p, int skip[], bool ignoreCase)
{
	int M = strlen((char *)p), i;
	
	for (i = 0; i < 255; i++)
		skip[i] = M;
	
	if (ignoreCase)
	{
		for (i = 0; i < M; i++)
			skip[toupper(p[i])] = M - i - 1;
	}
	else
	{
		for (i = 0; i < M; i++)
			skip[p[i]] = M - i - 1;
	}
} /* initskip */

int mismatchsearch(const unsigned char *p, const unsigned char *a, int N, int skip[], bool ignoreCase)
{
	ASSERT(p);
	ASSERT(a);
	ASSERT(skip);
	int i, j, t, M = strlen((char *)p);

	if (ignoreCase)
	{
		for (i = M - 1, j = M - 1; j >= 0; i--, j--)
		{
			while (toupper(a[i]) != toupper(p[j]))
			{
				t = skip[toupper(a[i])];
				i += (M - j > t) ? M - j : t;
				if (i >= N)
					return N;
				j = M - 1;
			}
		}
	}
	else
	{
		for (i = M - 1, j = M - 1; j >= 0; i--, j--)
		{
			while (a[i] != p[j])
			{
				t = skip[a[i]];
				i += (M - j > t) ? M - j : t;
				if (i >= N)
					return N;
				j = M - 1;
			}
		}
	}
	return i;
} /* mismatchsearch */

void initskip_b(const unsigned char *p, int skip[], bool ignoreCase)
{
	int M = strlen((char *)p), i;
	
	for (i = 0; i < 255; i++)
		skip[i] = M;
	
	if (ignoreCase)
	{
		for (i = M - 1; i >= 0; i--)
			skip[toupper(p[i])] = i;
	}
	else
	{
		for (i = M - 1; i >= 0; i--)
			skip[p[i]] = i;
	}
} /* initskip_b */

int mismatchsearch_b(const unsigned char *p, const unsigned char *a, int N, int skip[], bool ignoreCase)
{
	ASSERT(p);
	ASSERT(a);
	ASSERT(skip);
	int i, j, t, M = strlen((char *)p);

	if (ignoreCase)
	{
		for (i = N - M, j = 0; j < M; i++, j++)
		{
			while (toupper(a[i]) != toupper(p[j]))
			{
				t = skip[toupper(a[i])];
				i -= (j + 1 > t) ? j + 1 : t;
				if (i < 0)
					return -1;
				j = 0;
			}
		}
	}
	else
	{
		for (i = N - M, j = 0; j < M; i++, j++)
		{
			while (a[i] != p[j])
			{
				t = skip[a[i]];
				i -= (j + 1 > t) ? j + 1 : t;
				if (i < 0)
					return -1;
				j = 0;
			}
		}
	}
	return i - M;
} /* mismatchsearch_b */

int Find(const char *what, const char *buf, int bufSize, bool ignoreCase)
{
	int skip[256];
	int offset = 0;

	initskip((unsigned char *)what, skip, ignoreCase);
	offset = mismatchsearch((unsigned char *)what, (unsigned char *)buf, bufSize, skip, ignoreCase);

	return offset;
} /* Find */

static bool IsWord(const char *buf, int size, int start, int len)
{
	bool result;
	
	result = isalnum(buf[start]);
	if (result) result = isalnum(buf[start + len - 1]);
	if (result) result = (start == 0 || (!isalnum(buf[start - 1]) && buf[start - 1] != '_'));
	if (result) result = (start + len == size || (!isalnum(buf[start + len]) && buf[start + len] != '_'));
	
	return result;
} /* IsWord */

void Offset2Line(const char *buf, int size, int offset, int& line, int& selStart, char** l)
{
	line = 1;
	int i = 0, ls = 0;
	
	while (i < offset)
	{
		if (buf[i] == '\n')
		{
			line++;
			ls = i + 1;
		}
		i++;
	}
	
	selStart = offset - ls + 2;

	i = ls + 1;
	while (buf[i] != '\n' && i < size)
		i++;

	*l = (char *)malloc(i - ls + 2);
	FailNil(*l);
	strncpy(*l, buf + ls, i - ls + 1);
	(*l)[i - ls + 1] = 0;

	for (i = 0; (*l)[i]; i++)
		if (isspace((*l)[i])) (*l)[i] = ' ';
} /* Offset2Line */

bool FileContains(const char *path, const char *what, bool ignoreCase, bool word, vector<PMessageItem*> *lst)
{
	bool result = false;

	FILE *f = fopen(path, "r");
	if (f)
	{
		fseek(f, 0, SEEK_END);
		int size = ftell(f);
		fseek(f, 0, SEEK_SET);
		
		char *buf = (char *)malloc(size + 1);
		if (buf)
		{
			fread(buf, 1, size, f);
			buf[size] = 0;
			result = BufferContains(buf, size, path, what, ignoreCase, word, lst);			
			free(buf);
		}
		fclose(f);
	}

	return result;
} /* FileContains */

bool BufferContains(const char *buf, int size, const char *path, const char *what, bool ignoreCase,
	bool word, vector<PMessageItem*> *lst)
{
	bool result = false;
	int skip[256];
	int offset = 0;

	initskip((unsigned char *)what, skip, ignoreCase);
	
	do
	{
		offset += mismatchsearch((unsigned char *)what, (unsigned char *)buf + offset, size - offset, skip, ignoreCase);
		result = offset < size;
		if (result && word)
			result = IsWord(buf, size, offset + 1, strlen(what));
		if (!result) offset += strlen(what);
	}
	while (!result && offset < size);

	if (result && lst)
	{
		do
		{
			if (!word || IsWord(buf, size, offset + 1, strlen(what)))
			{
				PMessageItem *i = new PMessageItem;
				
				char *l;
				int line, start;
				
				Offset2Line(buf, size, offset, line, start, &l);
				
				i->SetError(l);
				i->SetFile(path);
				i->SetLine(line);
				i->SetSel(start, strlen(what));
				i->SetKind(3);
				
				lst->push_back(i);
				
				free(l);
			}
			
			offset += strlen(what);
			offset += mismatchsearch((unsigned char *)what, (unsigned char *)buf + offset,
				size - offset, skip, ignoreCase);
		}
		while (offset < size);
	}
				
	return result;
} /* BufferContains */

bool FileContainsEx(const char *path, const regex_t *preg, bool word, vector<PMessageItem*> *lst)
{
	bool result = false;

	FILE *f = fopen(path, "r");
	if (f)
	{
		fseek(f, 0, SEEK_END);
		int size = ftell(f);
		fseek(f, 0, SEEK_SET);
		
		char *buf = (char *)malloc(size + 1);
		if (buf)
		{
			fread(buf, 1, size, f);
			buf[size] = 0;
			result = BufferContainsEx(buf, size, path, preg, word, lst);
			free(buf);
		}
		
		fclose(f);
	}

	return result;
} /* FileContainsEx */

bool BufferContainsEx(const char *buf, int size, const char *path, const regex_t *preg, bool word, vector<PMessageItem*> *lst)
{
	int offset, e = 0, r;
	
	regmatch_t match[2];
	
	do
	{
		offset = e;
		r = rx_regexec(preg, buf + offset, size - offset, 1, match, 0);
		offset += match[0].rm_so;
		e = offset + (match[0].rm_eo - match[0].rm_so);
	}
	while (r == 0 && word && !IsWord(buf, size, offset, e));

	if (r == 0 && lst)
	{
		do
		{
			if (!word || IsWord(buf, size, offset, e))
			{
				PMessageItem *i = new PMessageItem;
				
				char *l;
				int line, start;
				
				Offset2Line(buf, size, offset, line, start, &l);
				
				i->SetError(l);
				i->SetFile(path);
				i->SetLine(line);
				i->SetSel(start - 1, e - offset);
				i->SetKind(3);
				
				lst->push_back(i);
				
				free(l);
			}
			
			offset = e;
			r = rx_regexec(preg, buf + offset, size - offset, 1, match, 0);
			offset += match[0].rm_so;
			e = offset + (match[0].rm_eo - match[0].rm_so);
		}
		while (r == 0);
	}
				
	return (r == 0);
} /* BufferContainsEx */

char* rx_replace(regex_t *pb, const char *txt, int size, const char *repl)
{
	regmatch_t matches[10];
	
	int r = rx_regexec(pb, txt, size, 9, matches, 0);
	if (r > 1)
	{
		char err[100];
		regerror(r, pb, err, 100);
		THROW((err));
	}

	vector<char> str;
	int rl = strlen(repl);
	int i = 0;
	
	while (i <= rl)
	{
		switch (repl[i])
		{
			case '&':
			{
				str.insert(str.end(), txt, txt + size);
				break;
			}
			case '\\':
			{
				i++;
				int start = 0, stop = 0;
	
				switch (repl[i])
				{
					case '1': start = matches[1].rm_so; stop = matches[1].rm_eo; break;
					case '2': start = matches[2].rm_so; stop = matches[2].rm_eo; break;
					case '3': start = matches[3].rm_so; stop = matches[3].rm_eo; break;
					case '4': start = matches[4].rm_so; stop = matches[4].rm_eo; break;
					case '5': start = matches[5].rm_so; stop = matches[5].rm_eo; break;
					case '6': start = matches[6].rm_so; stop = matches[6].rm_eo; break;
					case '7': start = matches[7].rm_so; stop = matches[7].rm_eo; break;
					case '8': start = matches[8].rm_so; stop = matches[8].rm_eo; break;
					case '9': start = matches[9].rm_so; stop = matches[9].rm_eo; break;
					case 'n': str.push_back('\n'); break;
					case 'r': str.push_back('\r'); break;
					case '\\': str.push_back('\\'); break;
					case 't': str.push_back('\t'); break;
				}
				
				stop = min(stop, size);
				start = max(0, start);

				if (stop > start)
					str.insert(str.end(), txt + start, txt + stop);
				
				break;
			}
			
			default:
				str.push_back(repl[i]);
		}
			
		i++;
	}
	
	char *b = (char *)malloc(str.size() + 1);
	copy(str.begin(), str.end(), b);
	b[str.size()] = 0;

	return b;
} /* rx_replace */

int rx_regcomp(regex_t *preg, const char *pattern, bool ignoreCase)
{
	char *p = strdup(pattern), *sp = p, *dp = p;
	
	while (*sp)
	{
		if (*sp == '\\')
		{
			switch (sp[1])
			{
				case 'n':	*dp++ = '\n'; sp += 2;	continue;
				case 'r':	*dp++ = '\r'; sp += 2;	continue;
				case 't':	*dp++ = '\t'; sp += 2;	continue;
				case '\\':	*dp++ = '\\'; *dp++ = '\\'; sp += 2; continue;
			}
		}

		*dp++ = *sp++;
	}
	*dp = 0;

	regfree(preg);
	int r = regcomp(preg, p, REG_EXTENDED | REG_NEWLINE | (ignoreCase ? REG_ICASE : 0));
	free(p);
	return r;
} /* int rx_regcomp */

int rx_regexec (const regex_t *preg, const char *string, int len,
	int nmatch, regmatch_t pmatch[], int eflags, bool backward)
{
	int ret;
	struct re_registers regs;
	regex_t private_preg;
	bool want_reg_info = !preg->no_sub && nmatch > 0;
	
	private_preg = *preg;
	  
	private_preg.not_bol = !!(eflags & REG_NOTBOL);
	private_preg.not_eol = !!(eflags & REG_NOTEOL);

	/*	The user has told us exactly how many registers to return
		information about, via `nmatch'.  We have to pass that on to the
		matching routines.  */
	private_preg.regs_allocated = REGS_FIXED;

	if (want_reg_info)
	{
		regs.num_regs = nmatch;
		regs.start = (regoff_t *)malloc(nmatch * sizeof(regoff_t));
		FailNil(regs.start);
		regs.end = (regoff_t *)malloc(nmatch * sizeof(regoff_t));
		FailNil(regs.end);
	}

	/* Perform the searching operation.  */
	if (backward)
		ret = re_search_2 (&private_preg, NULL, 0, string, len,
			len, -len - 1,
			want_reg_info ? &regs : (struct re_registers *) 0, 0);
	else
		ret = re_search (&private_preg, string, len,
			/* start: */ 0, /* range: */ len,
			want_reg_info ? &regs : (struct re_registers *) 0);

	/* Copy the register information to the POSIX structure.  */
	if (want_reg_info)
	{
		if (ret >= 0)
		{
			unsigned r;
			
			for (r = 0; r < nmatch; r++)
			{
				pmatch[r].rm_so = regs.start[r];
				pmatch[r].rm_eo = regs.end[r];
			}
		}

	/* If we needed the temporary register info, free the space now.  */
		free (regs.start);
		free (regs.end);
	}
	
	/* We want zero return to mean success, unlike `re_search'.  */
	return ret >= 0 ? (int) REG_NOERROR : (int) REG_NOMATCH;
} /* rx_regexec */
