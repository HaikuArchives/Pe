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

	Created: 10/25/97 08:56:58 by Maarten Hekkelman
*/

#include "pe.h"
#include "PDoc.h"
#include "PText.h"
#include "PApp.h"
#include "PMessages.h"
#include "PStatus.h"
#include "Utils.h"
#include "PAbout.h"
#include "PSplitter.h"
#include "PScrollBar.h"
#include "COpenSelection.h"
#include "CMenuItem.h"
#include "PToolBar.h"
#include "CFtpStream.h"
#include "CUrlOpener.h"
#include "PKeyDownFilter.h"
#include <fs_attr.h>
#include "HButtonBar.h"
#include "HPreferences.h"
#include "MTextAddOn.h"
#include "PCmd.h"
#include "CGoToLine.h"
#include "CInfoDialog.h"
#include "CConvertDialog.h"
#include "CDiffWindow.h"
#include "CFtpDialog.h"
#include "CKeyMapper.h"
#include "Scripting.h"
#include "MScripting.h"
#include "PGroupWindow.h"
#include "CLineObject.h"
#include "CCharObject.h"
#include "HDefines.h"
#include "MAlert.h"

static long sDocCount = 0;

const float
	kStatusWidth = 80;

PDoc::PDoc(const entry_ref *doc, bool show)
	: BWindow(NextPosition(), "Untitled", B_DOCUMENT_WINDOW, 0,
		1 << current_workspace())
	, CDoc("text/plain", this, doc)
{
	fShortcut = atomic_add(&sDocCount, 1);
	fIsWorksheet = false;
	fWindowMenuLength = -1;
	fToolBar = NULL;
	
	InitWindow(doc ? doc->name : "Untitled");

	if (IsReadOnly())
		fButtonBar->SetOn(msg_ReadOnly, true);

	if (doc)
	{
		Read();

		BPath p;
		BEntry e(doc);
		
		e.GetPath(&p);
		fStatus->SetPath(p.Path());
		if (show)
			AddRecent(p.Path());

		if (gPrefs->GetPrefInt("fullpath", 1))
			SetTitle(p.Path());
		else
			SetTitle(doc->name);
	}
	else
		fLastSaved = 0;
	
	fButtonBar->SetEnabled(msg_Save, false);
	
	if (show)
		Show();
} /* PDoc::PDoc */

PDoc::PDoc(URLData& url)
	: BWindow(NextPosition(), "Untitled", B_DOCUMENT_WINDOW, 0,
		1 << current_workspace())
	, CDoc(this, url)
{
	fShortcut = atomic_add(&sDocCount, 1);
	fIsWorksheet = false;
	fWindowMenuLength = -1;
	fToolBar = NULL;
	
	InitWindow("Untitled");

	Read();
	
	char title[128];
	if (strlen(url.Path()))
		sprintf(title, "ftp://%s/%s/%s", url.Server(), url.Path(), url.File());
	else
		sprintf(title, "ftp://%s/%s", url.Server(), url.File());
	SetTitle(title);
	
	AddRecent(title);

	fText->SetLanguage(url.File());
	
	fButtonBar->SetEnabled(msg_Save, false);
	
	Show();
} /* PDoc::PDoc */

PDoc::~PDoc()
{
	vector<HDialog*>::iterator i;
	for (i = fDialogs.begin(); i != fDialogs.end(); i++)
	{
		(*i)->Lock();
		(*i)->Quit();
	}
} /* PDoc::~PDoc */

bool PDoc::QuitRequested()
{
	bool result = true;

	if (fIsWorksheet)
	{
		Save();
	}
	else if (fText->IsDirty())
	{
		SetDirty(true);
		result = CDoc::QuitRequested();
	}

	return result;
} /* PDoc::QuitRequested */

void PDoc::InitWindow(const char *name)
{
	SetSizeLimits(100, 100000, 100, 100000);

	BRect b(Bounds()), r;
	float mbarHeight;
	bool showIde = gPrefs->GetPrefInt("ide menu", 1);
	
	r = b;
	fMBar = HResources::GetMenuBar(r, 0);
	AddChild(fMBar);
	
	mbarHeight = fMBar->Bounds().bottom;
	if (showIde && ++mbarHeight < 19)
		mbarHeight = 19;
	
	b.top += mbarHeight + 1;
	
	BMenu *file = fMBar->SubmenuAt(0);
	int i = 0;
	while (i < file->CountItems())
	{
		if (file->ItemAt(i)->Submenu() != NULL)
			fRecent = file->ItemAt(i)->Submenu();
		i++;
	}
	
	if (!fRecent) THROW(("Resources damaged?"));

	if (showIde)
	{
		BMenu *menu = HResources::GetMenu(7);
		BMenuItem *ideMenuItem = new CMenuItem(menu);
		fMBar->AddItem(ideMenuItem);
	}

	r = b;
	
	r.bottom = r.top + kToolBarHeight;

	fToolBar = new PToolBar(r, "ToolBar");
	AddChild(fToolBar);
	
	r.OffsetTo(0, 0);
	r.bottom -= 2;
	fToolBar->AddChild(fButtonBar = new HButtonBar(r, "ButtonBar", 0));
	
	r.left = fButtonBar->Frame().right + 4;
	fToolBar->AddChild(fError = new BStringView(r, "error", "", B_FOLLOW_LEFT_RIGHT));
	fError->SetViewColor(kViewColor);
	fError->SetFont(be_plain_font);
	fError->SetHighColor(kBlack);

	b.top += kToolBarHeight;

	BScrollBar *bars[4] = { NULL, NULL, NULL, NULL };
	
	r.Set(b.right - B_V_SCROLL_BAR_WIDTH + 1, b.top + kSplitterHeight,
		b.right + 1, b.bottom + 1 - B_H_SCROLL_BAR_HEIGHT);
	bars[1] = new PScrollBar(r, "v scrollbar 2", NULL, 0, 100, B_VERTICAL);
	AddChild(bars[1]);

	r.top -= kSplitterHeight;
	bars[0] = new PScrollBar(r, "v scrollbar 1", NULL, 0, 100, B_VERTICAL);
	AddChild(bars[0]);
	bars[0]->SetResizingMode(B_FOLLOW_RIGHT | B_FOLLOW_TOP);
	bars[0]->Hide();
	bars[0]->ResizeTo(r.Width(), 0);

	r.Set(b.left + kStatusWidth + 1, b.bottom - B_H_SCROLL_BAR_HEIGHT + 1,
		b.right + 1 - B_V_SCROLL_BAR_WIDTH, b.bottom + 1);
	bars[2] = new BScrollBar(r, "h scrollbar", NULL, 0, 100000, B_HORIZONTAL);
	AddChild(bars[2]);
	
	r = b;
	r.right -= B_V_SCROLL_BAR_WIDTH;
	r.bottom -= B_H_SCROLL_BAR_HEIGHT;

	fText = new PText(r, fTextBuffer, bars, name);
	AddChild(fText);
	fText->MakeFocus(true);
	
	fButtonBar->SetTarget(fText);

	r = b;
	r.left = r.right - B_V_SCROLL_BAR_WIDTH + 1;
	r.bottom = r.top + kSplitterHeight;
	AddChild(new PSplitter(r, fText));
	
	r = b;
	r.right = r.left + kStatusWidth;
	r.top = r.bottom - B_H_SCROLL_BAR_HEIGHT + 1;
	fStatus = new PStatus(r, fText);
	AddChild(fStatus);

	fText->SetStatus(fStatus);

	BMenu *m;
	BuildExtensionsMenu(m = fMBar->SubmenuAt(4));
	if (m->CountItems() == 0)
	{
		fMBar->RemoveItem(m);
		delete m;
		fWindows = fMBar->SubmenuAt(4);
	}
	else
		fWindows = fMBar->SubmenuAt(5);

	fMBar->SubmenuAt(1)->SetTargetForItems(fText);
	fMBar->SubmenuAt(2)->SetTargetForItems(fText);
	fMBar->SubmenuAt(3)->SetTargetForItems(fText);
	fWindows->SetTargetForItems(be_app);

	fMBar->FindItem(msg_Info)->SetTarget(this);
	fMBar->FindItem(msg_Preferences)->SetTarget(this);
	fMBar->FindItem(msg_ChangeWD)->SetTarget(fText);
	fMBar->FindItem(msg_ChangeCaseLower)->SetTarget(fText);
	fMBar->FindItem(msg_ChangeCaseUpper)->SetTarget(fText);
	fMBar->FindItem(msg_ChangeCaseCap)->SetTarget(fText);
	fMBar->FindItem(msg_ChangeCaseLine)->SetTarget(fText);
	fMBar->FindItem(msg_ChangeCaseSent)->SetTarget(fText);

	fMBar->FindItem(msg_Quit)->SetTarget(be_app);
	fMBar->FindItem(msg_New)->SetTarget(be_app);
	fMBar->FindItem(msg_NewProject)->SetTarget(be_app);
	fMBar->FindItem(msg_Open)->SetTarget(be_app);
	fMBar->FindItem(msg_Help)->SetTarget(be_app);
	fMBar->FindItem(msg_Preferences)->SetTarget(be_app);

	fText->AddFilter(new PKeyDownFilter());

	ResetMenuShortcuts();
} /* PDoc::InitWindow */

void PDoc::SaveOnServer(URLData& url)
{
	if (fFile) delete fFile;
	fFile = NULL;
	fURL = new URLData(url);

	Save();

	char title[128];
	if (strlen(url.Path()))
		sprintf(title, "ftp://%s/%s/%s", url.Server(), url.Path(), url.File());
	else
		sprintf(title, "ftp://%s/%s", url.Server(), url.File());
	SetTitle(title);
} /* PDoc::SaveOnServer */

void PDoc::ReadData(BPositionIO& file)
{
	size_t s;
	char *t;
	
	s = file.Seek(0, SEEK_END);
	file.Seek(0, SEEK_SET);
	
	t = (char *)malloc(s);
	FailNil(t);
	CheckedRead(file, t, s);
	
	fText->SetText(t, s);
} /* PDoc::ReadData */

void PDoc::ReadAttr(BFile& file)
{
	FailOSErr(file.GetModificationTime(&fLastSaved));
	
	char *p = NULL;
	try
	{
		attr_info ai;
		if (file.GetAttrInfo("pe-info", &ai) == B_NO_ERROR)
		{
			p = (char *)malloc(ai.size);
			FailNil(p);
			
			FailIOErr(file.ReadAttr("pe-info", ai.type, 0, p, ai.size));
			
			BMessage msg;
//			FailOSErr(msg.Unflatten(p));
			if (msg.Unflatten(p) != B_OK) return;

			BRect f;
			if (gRestorePosition &&
				msg.FindRect("windowposition", &f) == B_NO_ERROR)
			{
				{
					BScreen s;
					f = f & s.Frame();
				}
				if (f.IsValid() && f.Width() > 100 && f.Height() > 100)
				{
					MoveTo(f.left, f.top);
					ResizeTo(f.Width(), f.Height());
					fText->AdjustScrollBars();
				}
			}

			fText->SetSettings(msg);
		}
		else if (file.GetAttrInfo("FontPrefs", &ai) == B_NO_ERROR)
		{
			p = (char *)malloc(ai.size);
			FailNil(p);
			
			FailIOErr(file.ReadAttr("FontPrefs", ai.type, 0, p, ai.size));
			
			BMemoryIO mem(p, ai.size);

			BRect f;
			mem >> f;
			
			if (gRestorePosition)
			{
				{
					BScreen s;
					f = f & s.Frame();
				}
	
				if (f.IsValid() && f.Width() > 100 && f.Height() > 100)
				{
					MoveTo(f.left, f.top);
					ResizeTo(f.Width(), f.Height());
				}
			}

			fText->SetSettingsMW(mem);
		}
// no settings data found, see if it is a Mac file and adjust encoding:
		else if (fText->LineEndType() == leCR)
			fText->SetEncoding(B_MACINTOSH_ROMAN);
	}
	catch (HErr& e) {}
	if (p) free(p);
} /* PDoc::ReadAttr */

void PDoc::WriteData(BPositionIO& file)
{
	FailOSErr(file.Seek(0, SEEK_SET));
	
	size_t size;
	const char *text;
	PTextBuffer buf;
	
	if (fURL)
	{
		buf = fText->TextBuffer();
		buf.TranslateFromLF(leCRLF, fText->LineCount());
		size = buf.Size();
		text = buf.Buffer();
	}
	else if (fText->LineEndType() == leLF)
	{
		size = fText->Size();
		text = fText->Text();
	}
	else
	{
		buf = fText->TextBuffer();
		buf.TranslateFromLF(fText->LineEndType(), fText->LineCount());
		size = buf.Size();
		text = buf.Buffer();
	}
	
	CheckedWrite(file, text, size);
	if (gPrefs->GetPrefInt("nl at eof", 1) &&
		text[size - 1] != '\n')
	{
		CheckedWrite(file, "\n", 1);
	}

	time(&fLastSaved);
} /* PDoc::WriteData */

void PDoc::WriteAttr(BFile& file)
{
	char *p = NULL;

	try
	{
		switch (gSavedState)
		{
			case 0:
			{
				BMessage msg;
				fText->GetSettings(msg);
				msg.AddRect("windowposition", Frame());
		
				ssize_t s = msg.FlattenedSize();
				p = (char *)malloc(s);
				FailNil(p);
				FailOSErr(msg.Flatten(p, s));
				FailIOErr(file.WriteAttr("pe-info", 'info', 0, p, s));
				break;
			}
			case 1:
			{
				BMallocIO data;
				
				data << Frame();
				fText->GetSettingsMW(data);
				
				FailIOErr(file.WriteAttr("FontPrefs", 'type', 0, data.Buffer(), data.BufferLength()));
				break;
			}
		}
	}
	catch (...) {}
	
	if (p) free(p);
} /* PDoc::WriteAttr */

void PDoc::SaveRequested(entry_ref& directory, const char *name)
{
	if (fSavePanel)
	{
		BWindow *w = fSavePanel->Window();
		BAutolock lock(w);
		
		if (lock.IsLocked())
		{
			BMenu *m = static_cast<BMenuField*>(w->FindView("mime"))->Menu();
			FailNil(m);
			BMenuItem *i = m->FindMarked();
			FailNil(i);
			
			gPrefs->SetPrefInt("LastSavedMimeType", m->IndexOf(i));
			fMimeType = i->Label();
		}
	}
	
	BFile old;
	bool oldExists;

	if (fFile)
		oldExists = old.SetTo(fFile, B_READ_ONLY) == B_OK;	// could fail
	else
		oldExists = false;
	
	CDoc::SaveRequested(directory, name);
	
	if (fFile)
	{
		if (oldExists && gPrefs->GetPrefInt("save attr", 1))
		{
			BFile file;
			FailOSErr(file.SetTo(fFile, B_READ_WRITE));

			mode_t perm;
			FailOSErr(file.GetPermissions(&perm));

			CopyAttributes(old, file);
			
			file.SetPermissions(perm);
		}
	
		fText->SetLanguage(name);

		BEntry e;
		FailOSErr(e.SetTo(fFile));
		BPath p;
		FailOSErr(e.GetPath(&p));
		fStatus->SetPath(p.Path());

		if (gPrefs->GetPrefInt("fullpath", 1))
			SetTitle(p.Path());
		else
			SetTitle(name);
	}
	else
	{
		SetTitle("Untitled");
		fStatus->SetPath("Not saved");
	}	
} /* PDoc::SaveRequested */

void PDoc::SaveACopy()
{
	if (!fSavePanel)
		fSavePanel = new BFilePanel(B_SAVE_PANEL);
	
	FailNil(fSavePanel);

	BWindow *w = fSavePanel->Window();
	w->Lock();
	
	char s[256];
	sprintf(s, "Save a copy of %s as:", fFile ? fFile->name : Title());
	
	w->SetTitle(s);
	fSavePanel->SetSaveText(fFile ? fFile->name : Title());
	
	if (fFile)
	{
		BEntry e(fFile), p;
		e.GetParent(&p);
		fSavePanel->SetPanelDirectory(&p);
	}
	else
		fSavePanel->SetPanelDirectory(&gCWD);

	fSavePanel->SetMessage(new BMessage(msg_DoSaveACopy));
	fSavePanel->SetTarget(this);

	w->Unlock();

	if (!fSavePanel->IsShowing())
		fSavePanel->Show();
	else
		fSavePanel->Window()->Activate();
} /* PDoc::SaveACopy */

void PDoc::DoSaveACopy(entry_ref& directory, const char *name)
{
	try
	{
		BDirectory dir(&directory);

		BEntry e(&dir, name);
		FailOSErr(e.InitCheck());
		if (e.Exists())
			e.Remove();
		
		BFile file;
		
		FailOSErr(dir.CreateFile(name, &file, true));
		WriteData(file);
		FailOSErr(file.Sync());
		BNodeInfo(&file).SetType("text/plain");

		if (fSavePanel)
		{
			delete fSavePanel;
			fSavePanel = NULL;
		}
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* PDoc::DoSaveACopy */

void PDoc::Revert() 
{
	char title[256];
	sprintf(title, "Revert to the last saved version of %s?", Title());
	
	MInfoAlert a(title, "Cancel", "OK");
	if (a == 2)
	{
		if (fFile)
		{
			BFile file;
			FailOSErr(file.SetTo(fFile, B_READ_ONLY));
			ReadData(file);
		}
		else if (fURL)
		{
			CFtpStream ftp(*fURL, true);
			ReadData(ftp);
		}
		else
			THROW(("No file?!?!?!?"));
		fText->SetDirty(false);
	}
} /* PDoc::Revert */

void PDoc::WindowActivated(bool active)
{
	BWindow::WindowActivated(active);

	if (active)
	{
		sfDocList.remove(this);
		sfDocList.push_front(this);
	}

	try
	{
		if (active && gPrefs->GetPrefInt("verify", 1))
		{
			if (fFile)
			{
				BFile file;
				FailOSErr(file.SetTo(fFile, B_READ_ONLY));
				
				time_t t;
				FailOSErr(file.GetModificationTime(&t));
				if (fLastSaved && t > fLastSaved + 1)
				{
					char s[PATH_MAX + 20];
					sprintf(s, "File %s was modified, reload it?", fFile->name);
					MInfoAlert a(s, "Reload", "Cancel");
					
					if (a.Go() == 1)
						Read();
				}
				
				fLastSaved = t;
			}
		}
	}
	catch (HErr& e)
	{
		delete fFile;
		fFile = NULL;

		MWarningAlert a("This file seems to have disappeared!");
		a.Go();
	}
} /* PDoc::WindowActivated */

void PDoc::OpenInclude(const char *incl)
{
	char *bi = NULL;
	bool found = false;

	try
	{
		BEntry e;
		entry_ref doc;
		BDirectory d;

		if (strncmp(incl, "ftp://", 6) == 0)
		{
			CUrlOpener *uo = new CUrlOpener(incl);
			uo->Run();
			return;
		}
		
		if (fURL && gPrefs->GetPrefInt("parent"))
		{
			URLData url(*fURL);
			url += incl;
			if (url.IsValid())
			{
				CUrlOpener *uo = new CUrlOpener(url);
				uo->Run();
				return;
			}
		}
		
		if (fFile && gPrefs->GetPrefInt("parent"))
		{
			FailOSErr(e.SetTo(fFile));
			FailOSErr(e.GetParent(&d));
			
			if (d.Contains(incl, B_FILE_NODE | B_SYMLINK_NODE))
			{
				FailOSErr(d.FindEntry(incl, &e, true));
				if (!e.IsFile()) THROW((0));
		
				FailOSErr(e.GetRef(&doc));
				found = true;
			}
		}
		
		if (!found && gPrefs->GetPrefInt("beincludes"))
		{
			bi = strdup(getenv("BEINCLUDES"));
			char *ip = bi;
			char *p = ip;
			
			while (p && !found)
			{
				char *pe = strchr(p, ';');
				if (pe) *pe = 0;
				
				FailOSErr(d.SetTo(p));
				if (d.Contains(incl, B_FILE_NODE | B_SYMLINK_NODE))
				{
					FailOSErr(d.FindEntry(incl, &e, true));
					if (!e.IsFile()) THROW((0));
					FailOSErr(e.GetRef(&doc));
					found = true;
				}
				
				p = (pe && pe[1]) ? pe + 1 : NULL;
			}
		}
		
		if (! found && fText->GetCWD())
		{
			FailOSErr(d.SetTo(fText->GetCWD()));
			
			if (d.Contains(incl, B_FILE_NODE | B_SYMLINK_NODE))
			{
				FailOSErr(d.FindEntry(incl, &e, true));
				if (!e.IsFile()) THROW((0));
		
				FailOSErr(e.GetRef(&doc));
				found = true;
			}
		}
		
		if (!found)
		{
			const char *p;
			int i = 0;

			while ((p = gPrefs->GetIxPrefString("includepath", i++))!= NULL && !found)
			{
				if (e.SetTo(p) != B_OK || !e.Exists())
					continue;

				FailOSErr(d.SetTo(p));
				if (d.Contains(incl, B_FILE_NODE | B_SYMLINK_NODE))
				{
					FailOSErr(d.FindEntry(incl, &e, true));
					if (!e.IsFile()) THROW((0));
					FailOSErr(e.GetRef(&doc));
					found = true;
				}
			}
		}
		
		if (found)
			gApp->OpenWindow(doc);
		else
			beep();
	}
	catch (HErr& e)
	{
		beep();
	}
	
	if (bi) free(bi);
} /* PDoc::OpenInclude */

void PDoc::OpenSelection()
{
	COpenSelection *d;

	try
	{
		char *s;
		fText->GetSelectedText(s, true);
		int line1 = -1;
		int line2 = -1;

		if (!s) {
			// try to find a filename around the cursor
			int current = fText->Caret();
			int front = current;
			int frontSpace = 0;
			int back = current;
			int backSpace = 0;
			int end = fText->Size();
			const char * text = fText->Text();
			
			// push forward until (a) end of file (b) end of line
			//                    (c) double quote (d) greater-than sign
			while ((front < end) && (text[front] != '\n')
					&& (text[front] != '"') && (text[front] != '>')) {
				if ((text[front] == ' ') || (text[front] == '\t')) {
					frontSpace = (frontSpace ? frontSpace : front);
				}
				front++;
			}
			if ((front >= end) || (text[front] == '\n'))  {
				// fall back to the first space if one exists
				front = (frontSpace ? frontSpace : front);
			}
			
			// push back until (a) start of file (b) end of line
			//                 (d) double quote (e) less-than sign
			if (back > 0) back--;
			while ((back >= 0) && (text[back] != '\n') 
					&& (text[back] != '"') && (text[back] != '<')) {
				if ((text[back] == ' ') || (text[back] == '\t')) {
					backSpace = (backSpace ? backSpace : back);
				}
				back--;
			}
			if ((back < 0) || (text[back] == '\n')) {
				// fall back to the first space if one exists
				back = (backSpace ? backSpace : back);
			}
			
			if ((back >= 0) && (front < end)) {
				if ((text[back] == '"') && (text[front] != '"') ||
					(text[back] != '"') && (text[front] == '"')) {
					front = (frontSpace ? frontSpace : front);
					back = (backSpace ? backSpace : back);
				}
				if ((text[back] == '<') && (text[front] != '>') ||
					(text[back] != '<') && (text[front] == '>')) {
					front = (frontSpace ? frontSpace : front);
					back = (backSpace ? backSpace : back);
				}
			}
						
			int filenameFront = front;
			// check to ensure matching double quote or angle brackets
			// otherwise fall back to whitespace marker if possible
			if ((back+1 < end) && (front <= end)) {
				// should look for colon delimiters here for line zooming
				int colon1 = back+1;
				while ((colon1 < front) && (text[colon1] != ':')) colon1++;
				if (colon1 < front) {
					filenameFront = colon1;
					char * firstNumberEnd = (char*)&(text[front]);
					line1 = strtol(&text[colon1+1],&firstNumberEnd,10);
					if ((line1 == 0) && (text[colon1+1] != '0')) {
						// no number found, mark the line to zero
						line1 = -1;
						// move the front back to behind the colon
						front = colon1;
					} else if ((firstNumberEnd != &(text[front]))
					           && (*firstNumberEnd == ':')) {
						// there's another colon, so check for second digit
						firstNumberEnd++;
						char * secondNumberEnd = (char*)&(text[front]);
						line2 = strtol(firstNumberEnd,&secondNumberEnd,10);
						if ((line2 == 0) && (*secondNumberEnd != '0')) {
							// no number found, mark the line to zero
							line2 = -1;
							// move the front back to behind the colon
							firstNumberEnd--;
							front -= &(text[front]) - firstNumberEnd;
						}
					}
				}
			}
			if (front > back+1) {
				// found something, grab the filename portion
				fText->Select(back+1,filenameFront,true,false);
				fText->GetSelectedText(s, true);
				// after we get the text, reselect the whole string
				fText->Select(back+1,front,true,false);
			}
		}
		
		if (s)
			OpenInclude(s);
		else
			GetDialog(d);
	}
	catch (HErr& e)
	{
		beep();
	}
} /* PDoc::OpenSelection */

BRect PDoc::NextPosition(bool inc)
{
	BRect sr, r;
	{
		BScreen screen;
		sr = screen.Frame();
	}
	
	static int cnt = -1;

	if (inc)
		cnt++;
	
	BFont textFont;
	gPrefs->InitTextFont(&textFont);
	r.top = 25 + (cnt % 8) * 20;
	r.left = 40 - (cnt % 8) * 4 + (cnt / 8) * 8;
	r.right = min((double)sr.right - 100,
		r.left + 80 * textFont.StringWidth(" ") + B_V_SCROLL_BAR_WIDTH + 3);
	float lh;
	font_height fh;
	be_fixed_font->GetHeight(&fh);
	lh = fh.ascent + fh.descent + fh.leading;
	r.bottom = min(sr.bottom - 100, 50 * lh);
	
	return r;
} /* PDoc::NextPosition */

void PDoc::MakeWorksheet()
{
	fIsWorksheet = true;
} /* PDoc::MakeWorksheet */

bool PDoc::IsSourceFile()
{
	char *e = strrchr(Title(), '.');
	if (e)
	{
		e++;
		if (strcmp(e, "c") && strcmp(e, "C") && strcmp(e, "cc")
		    && strcmp(e, "cpp") && strcmp(e, "cxx") && strcmp(e, "cp"))
			return false;
		return true;
	}
	return false;
} /* PDoc::IsSourceFile */

bool PDoc::IsHeaderFile()
{
	char *e = strrchr(Title(), '.');
	if (e)
	{
		e++;
		if (strcmp(e, "h") && strcmp(e, "H") && strcmp(e, "hh")
		    && strcmp(e, "hpp") && strcmp(e, "hxx") && strcmp(e, "hp"))
			return false;
		return true;
	}
	return false;
} /* PDoc::IsHeaderFile */

void PDoc::OpenPartner()
{
	try
	{
		if (IDEOpenSourceHeader(*fFile))
			return;
	
		BEntry e;
		entry_ref doc;
		FailOSErr(e.SetTo(fFile));
		
		BDirectory d;
		FailOSErr(e.GetParent(&d));
		
		if (IsSourceFile())
		{
			char *t = strdup(Title());
			FailNil(t);
			char *x = strrchr(t, '.');
			
			if (strcpy(x, ".h"), d.Contains(t, B_FILE_NODE | B_SYMLINK_NODE))
				FailOSErr(d.FindEntry(t, &e, true));
			
			else if (strcpy(x, ".H"), d.Contains(t, B_FILE_NODE | B_SYMLINK_NODE))
				FailOSErr(d.FindEntry(t, &e, true));
			
			else if (strcpy(x, ".hh"), d.Contains(t, B_FILE_NODE | B_SYMLINK_NODE))
				FailOSErr(d.FindEntry(t, &e, true));
			
			else if (strcpy(x, ".hpp"), d.Contains(t, B_FILE_NODE | B_SYMLINK_NODE))
				FailOSErr(d.FindEntry(t, &e, true));
			
			free(t);
		}
		else if (IsHeaderFile())
		{
			char *t = (char *)malloc(B_FILE_NAME_LENGTH);
			FailNil(t);
			strcpy(t, Title());
			char *x = strrchr(t, '.');
			
			if (strcpy(x, ".c"), d.Contains(t, B_FILE_NODE | B_SYMLINK_NODE))
				FailOSErr(d.FindEntry(t, &e, true));
			
			else if (strcpy(x, ".C"), d.Contains(t, B_FILE_NODE | B_SYMLINK_NODE))
				FailOSErr(d.FindEntry(t, &e, true));
			
			else if (strcpy(x, ".cp"), d.Contains(t, B_FILE_NODE | B_SYMLINK_NODE))
				FailOSErr(d.FindEntry(t, &e, true));
			
			else if (strcpy(x, ".cpp"), d.Contains(t, B_FILE_NODE | B_SYMLINK_NODE))
				FailOSErr(d.FindEntry(t, &e, true));
			
			else if (strcpy(x, ".cc"), d.Contains(t, B_FILE_NODE | B_SYMLINK_NODE))
				FailOSErr(d.FindEntry(t, &e, true));
			
			free(t);
		}

		if (!e.IsFile()) THROW((0));
		FailOSErr(e.GetRef(&doc));
		
		gApp->OpenWindow(doc);
	}
	catch (HErr& e)
	{
		beep();
	}
} /* PDoc::OpenPartner */

void PDoc::SetDirty(bool dirty)
{
	CDoc::SetDirty(dirty);
	fButtonBar->SetEnabled(msg_Save, dirty);
} /* PDoc::SetDirty */

void PDoc::CreateFilePanel()
{
	CDoc::CreateFilePanel();
	
	BWindow *w = fSavePanel->Window();
	BAutolock lock(w);
	
	if (lock.IsLocked())
	{
		BView *vw = w->ChildAt(0);
		FailNilMsg(vw, "Error building FilePanel");
		
		BMenu *m = HResources::GetMenu(20, true);
		FailNilMsg(m, "Error building FilePanel");
		m->SetFont(be_plain_font);
		m->SetRadioMode(true);
		
		BView *v = vw->FindView("text view");
		FailNilMsg(v, "Error building FilePanel");
		BRect r = v->Frame();
		v->ResizeTo(r.Width() - 50, r.Height());
		
		r.left = r.right - 45;
		r.right = r.left + 100;
		r.top += (r.Height() - 20) / 2;
		
		BMenuField *mf = new BMenuField(r, "mime", "Type:",
			m, B_FOLLOW_BOTTOM | B_FOLLOW_LEFT);
		FailNilMsg(mf, "Error building FilePanel");
		vw->AddChild(mf);
		mf->SetDivider(be_plain_font->StringWidth("Type:") + 2);
		
		int i = 0;
		const char *p;
		
		while ((p = gPrefs->GetIxPrefString("mimetype", i++)) != NULL)
			m->AddItem(new BMenuItem(p, NULL));
		
		BMenuItem *item = m->FindItem(fMimeType.c_str());
		if (item)
			item->SetMarked(true);
		else
		{
			m->AddItem(item = new BMenuItem(fMimeType.c_str(), NULL));
			item->SetMarked(true);
		}
	}
} /* PDoc::CreateFilePanel */

void PDoc::NameAFile(char *name)
{
	strncpy(name, Title(), B_FILE_NAME_LENGTH - 1);
	name[B_FILE_NAME_LENGTH - 1] = 0;
} /* PDoc::NameAFile */

PDoc* PDoc::TopWindow()
{
	PDoc *doc = NULL;
	
	doclist::iterator di;
	
	for (di = sfDocList.begin(); di != sfDocList.end() && doc == NULL; di++)
		doc = dynamic_cast<PDoc*>((*di));
	
	return doc;
} /* PDoc::TopWindow */

PDoc* PDoc::GetWorksheet()
{
	PDoc *doc = NULL;
	
	doclist::iterator di;
	
	for (di = sfDocList.begin();
		di != sfDocList.end() && (doc == NULL || ! doc->IsWorksheet());
		di++)
	{
		doc = dynamic_cast<PDoc*>((*di));
	}
	
	return doc;
} // PDoc::GetWorksheet

void PDoc::SetError(const char *err, rgb_color c)
{
	fError->SetText(err);
	fError->SetHighColor(c);
} // PDoc::SetError

#pragma mark - Addons

static vector<perform_edit_func> sExtensions;
static vector<char*> sExtensionNames;
static int sExtensionCount = 0;

static void LoadAddOnsFromPath(const char *path)
{
	char plug[PATH_MAX];
	DIR *dir = opendir(path);

	if (!dir)
		return;
	
	struct dirent *dent;
	struct stat stbuf;

	while ((dent = readdir(dir)) != NULL)
	{
		strcpy(plug, path);
		strcat(plug, dent->d_name);
		status_t err = stat(plug, &stbuf);
		if (!err && S_ISREG(stbuf.st_mode) &&
			strcmp(dent->d_name, ".") && strcmp(dent->d_name, ".."))
		{
			image_id next;
			perform_edit_func f;

			next = load_add_on(plug);
			if (next > B_ERROR &&
				get_image_symbol(next, "perform_edit", B_SYMBOL_TYPE_TEXT, (void**)&f) == B_NO_ERROR)
			{
				char *n = strdup(dent->d_name);

				sExtensionCount++;
				sExtensions.push_back(f);
				sExtensionNames.push_back(n);
			}
			else if (stbuf.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
			{
				// an executable shell scrip perhaps...
				
				sExtensionCount++;
				sExtensions.push_back(NULL);
				sExtensionNames.push_back(strdup(dent->d_name));
			}
		}
	}
} /* LoadAddOnsFromPath */

void PDoc::LoadAddOns()
{
	char path[PATH_MAX];
	
	BPath p;
	BEntry e;
	gAppDir.GetEntry(&e);
	e.GetPath(&p);
	strcpy(path, p.Path());

	strcat(path, "/Extensions/");
	
	LoadAddOnsFromPath(path);
	
	char *bt = getenv("BETOOLS");
	if (gPrefs->GetPrefInt("mw plugins", 1) && bt)
	{
		strcpy(path, bt);
		char *e = strrchr(path, '/');
		if (e)
		{
			strcpy(e, "/plugins/Editor_add_ons/");
			LoadAddOnsFromPath(path);
		}
	}
} /* PDoc::LoadAddOns */

void PDoc::BuildExtensionsMenu(BMenu *addOnMenu)
{
	for (int i = 0; i < sExtensionCount; i++)
	{
		if (! (gPrefs->GetPrefInt("skiphtmlext", 1) && strncasecmp(sExtensionNames[i], "HTML", 4) == 0))
			addOnMenu->AddItem(new BMenuItem(sExtensionNames[i], new BMessage(msg_PerformExtension)));
	}
} /* PDoc::BuildExtensionsMenu */

void PDoc::PerformExtension(int nr)
{
	if (sExtensions[nr] != NULL)
	{
		MTextAddOn intf(*fText, sExtensionNames[nr]);

		UpdateIfNeeded();

		(*sExtensions[nr])(&intf);
	}
	else if (modifiers() & B_OPTION_KEY)
	{
		char path[PATH_MAX];
		
		BPath p;
		BEntry e;
		gAppDir.GetEntry(&e);
		e.GetPath(&p);
		strcpy(path, p.Path());
	
		strcat(path, "/Extensions/");
		strcat(path, sExtensionNames[nr]);
		
		entry_ref ref;
		if (get_ref_for_path(path, &ref) == B_OK)
			gApp->OpenWindow(ref);
	}
	else
		fText->RegisterCommand(new PScriptCmd(fText, sExtensionNames[nr]));
} /* PDoc::PerformExtension */

void PDoc::PerformExtension(const char *ext)
{
	for (int i = 0; i < sExtensionCount; i++)
	{
		if (strcmp(sExtensionNames[i], ext) == 0)
		{
			PerformExtension(i);
			return;
		}
	}
	THROW(("Extension %s not found!", ext));
} /* PDoc::PerformExtension */

#pragma mark - Commands

void PDoc::MessageReceived(BMessage *msg)
{
	PApp *pa = (PApp *)be_app;
	unsigned long what = msg->what;

	try
	{
		switch (what)
		{
			case msg_CloseAll:
			{
				doclist lst = sfDocList;
				doclist::reverse_iterator di;
				
				for (di = lst.rbegin(); di != lst.rend(); di++)
				{
					PDoc *doc = dynamic_cast<PDoc*>(*di);
					
					if (doc && ! doc->IsWorksheet() && doc->Lock())
					{
						if (doc->QuitRequested())
							doc->Quit();
						else
						{
							doc->Unlock();
							break;
						}
					}
				}
				break;
			}
			
			case msg_Close:
				PostMessage(B_QUIT_REQUESTED);
				break;

			case msg_SaveAll:
				PostToAll(msg_Save, false);
				break;
			
			case msg_Save:
				if (IsDirty())
				{
					Save();
					fText->ResetUndo();
				}
				break;
			
			case msg_SaveAs:
				CDoc::SaveAs();
				break;
			
			case msg_Revert:
				Revert();
				break;
			
			case msg_OpenSelected:
				OpenSelection();
				break;
			
			case msg_OpenRecent:
				if (msg->HasRef("refs"))
					gApp->RefsReceived(msg);
				else if (msg->HasString("url"))
				{
					const char *url;
					FailOSErr(msg->FindString("url", &url));
					CUrlOpener *uo = new CUrlOpener(url);
					uo->Run();
				}
				break;

			case msg_SelectWindow:
			{
				BWindow *w;
				FailOSErr(msg->FindPointer("window", (void**)&w));
				if (w) 
				{
					if (gPrefs->GetPrefInt("window to workspace", 1))
						w->SetWorkspaces(1 << current_workspace());

					w->Activate(true);
				}
				break;
			}
			
			case B_SAVE_REQUESTED:
			{
				entry_ref dir;
				const char *name;
				
				FailOSErr(msg->FindRef("directory", &dir));
				FailOSErr(msg->FindString("name", &name));
				SaveRequested(dir, name);
				break;
			}
			
			case msg_SaveACopy:
				SaveACopy();
				break;
			
			case msg_DoSaveACopy:
			{
				entry_ref dir;
				const char *name;
				
				FailOSErr(msg->FindRef("directory", &dir));
				FailOSErr(msg->FindString("name", &name));
				DoSaveACopy(dir, name);
				break;
			}
			
			case msg_FindCmd:
			{
				int c = 1 << current_workspace();
				
				pa->FindDialog()->Show();
				
				if (gPrefs->GetPrefInt("window to workspace", 1))
					pa->FindDialog()->SetWorkspaces(c);
				
				pa->FindDialog()->Activate(true);
				break;
			}
			
			case msg_OpenInclude:
			{
				const char *f;
				FailOSErr(msg->FindString("include", &f));
				OpenInclude(f);
				break;
			}
			
			case msg_PrefsChanged:
				fText->SetViewColor(gPrefs->GetPrefColor("low color", kViewColor));
				fText->Invalidate();
				
				fStatus->Draw(fStatus->Bounds());
				ResetMenuShortcuts();
				break;
			
			case msg_SwitchHeaderSource:
				OpenPartner();
				break;
			
			case msg_PerformExtension:
			{
				BMenuItem *sender;
				FailOSErr(msg->FindPointer("source", (void**)&sender));
				PerformExtension(sender->Label());
				break;
			}

			case msg_HTMLExtension:
			{
				const char *ext;
				FailOSErr(msg->FindString("ext", &ext));
				PerformExtension(ext);
				break;
			}
			
			case msg_About:
				new PAboutBox();
				break;
			
			case msg_ShowInBrowser:
			{
				if (fText->IsDirty() || File() == NULL)
				{
					MAlert *a;
					
					if (fURL)
						a  = new MInfoAlert("In order to display this page in a browser you need to "
						"save this document on a local disk first.\n\n"
						"Save changes to this document first?", "Save", "Cancel");
					else
						a = new MInfoAlert("Save changes to this document first?", "Save", "Cancel");
					
					if (a->Go() == 1)
					{
						if (fURL)
							delete fURL;
						fURL = NULL;
						Save();
					}
				}
				
				if (!fText->IsDirty() && File())
					gApp->DisplayInBrowser(*File());
				break;
			}
			
			case msg_AddDialog:
			{
				HDialog *dlog;
				bool modal = false;
				FailOSErr(msg->FindPointer("dialog", (void**)&dlog));
				AddDialog(dlog, modal);
				break;
			}
			
			case msg_RemoveDialog:
			{
				HDialog *dlog;
				FailOSErr(msg->FindPointer("dialog", (void**)&dlog));
				RemoveDialog(dlog);
				break;
			}
			
			case msg_GoToLine:
			{
				CGoToLine *d;
				GetDialog(d);
				break;
			}
			
			case msg_Info:
			{
				CInfoDialog *d;
				GetDialog(d);
				break;
			}
			
			case msg_ConvertEncoding:
			{
				CConvertDialog *d;
				GetDialog(d);
				break;
			}
			
			case msg_IDE_Add:
				IDEAddFile();
				break;
				
			case msg_IDE_Remove:
				IDERemoveFile();
				break;
			
			case msg_IDE_Make:
				IDEMake();
				break;
			
			case msg_IDE_BringToFront:
				IDEBringToFront();
				break;
			
			case msg_IDE_Project2Group:
				IDEProject2Group();
				break;
			
			case msg_New:
			case msg_Open:
			case msg_NewProject:
			case msg_Quit:
			case msg_Help:
			case msg_Preferences:
				be_app->PostMessage(msg);
				break;
			
			case msg_ReadOnly:
				try
					{ SetReadOnly(! IsReadOnly());}
				catch (HErr& e)
					{ e.DoError(); }
				fButtonBar->SetOn(msg_ReadOnly, IsReadOnly());
				break;
			
			case msg_BtnOpen:
			{
				BPoint p;
				bool b;
				
				FailOSErr(msg->FindPoint("where", &p));
				FailOSErr(msg->FindBool("showalways", &b));
				
				ShowRecentMenu(p, b);
				break;
			}
			
			case msg_HTMLNew:
			{
				entry_ref doc;
				BEntry e;
				
				FailOSErr(gPrefsDir.FindEntry("Template.html", &e, true));
				FailOSErr(e.GetRef(&doc));
				
				PDoc *hDoc = NULL;
				
				{
					BAutolock lock(be_app);
					if (lock.IsLocked())
						hDoc = dynamic_cast<PDoc*>(pa->OpenWindow(doc, false));
				}
				
				if (hDoc)
				{
					BAutolock lock2(hDoc);
					
					if (lock2.IsLocked())
					{
						delete hDoc->fFile;
						hDoc->fFile = NULL;
	
						hDoc->SetTitle("Untitled");
						hDoc->fReadOnly = false;
						hDoc->fButtonBar->SetOn(msg_ReadOnly, false);

						BRect r = NextPosition(false);
						hDoc->MoveTo(r.left, r.top);
						hDoc->ResizeTo(r.Width(), r.Height());
	
						hDoc->Show();
					}
				}
				break;
			}
			
			case msg_FindDifferences:
			{
				BRect r = NextPosition();
				r.right = r.left + 400;
				r.bottom = r.top + 150;
				
				new CDiffWindow(r, "Differences");
				break;
			}
			
			case msg_FtpOpen:
			{
				CFtpDialog *ftpo = DialogCreator<CFtpDialog>::CreateDialog(NULL);
				ftpo->Show();
				break;
			}
			
			case msg_DoFtpSave:
			{
				URLData *url;
				FailOSErr(msg->FindPointer("url", (void**)&url));
				SaveOnServer(*url);
				delete url;
				break;
			}
			
			case msg_FtpSave:
			{
				CFtpDialog *ftps = DialogCreator<CFtpDialog>::CreateDialog(this);
				ftps->MakeItSave(Title());
				ftps->Show();
				break;
			}

			default:
			{
				if ((msg->what & 0xffffff00) == 0x65787400)
				{
					int nr = msg->what - 'ext0';
					BMenuItem *item = fMBar->SubmenuAt(4)->ItemAt(nr);
					
					if (item)
						PerformExtension(item->Label());
				}
				else
					BWindow::MessageReceived(msg);
			}
		}
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* PDoc::MessageReceived */

void PDoc::MenusBeginning()
{
	int i;

	if (fWindowMenuLength == -1)
		fWindowMenuLength = fWindows->CountItems();
	else
		for (i = fWindows->CountItems() - 1; i >= fWindowMenuLength; i--)
		{
			delete fWindows->RemoveItem(i);
		}

	UpdateShortCuts();

	doclist::iterator di;
	
	for (di = sfDocList.begin(); di != sfDocList.end(); di++)
	{
		PDoc *doc = dynamic_cast<PDoc*>(*di);

		if (doc && doc->IsWorksheet())
			continue;

		BWindow *w = dynamic_cast<BWindow*>(*di);
		BMessage *msg = new BMessage(msg_SelectWindow);
		msg->AddPointer("window", w);
		fWindows->AddItem(new BMenuItem(w->Title(), msg,
			(doc && doc->fShortcut < 10) ? '0' + doc->fShortcut : 0));
	}
	
	i = fRecent->CountItems() - 1;
	while (i >= 0)
		delete fRecent->RemoveItem(i--);

	float w;

	{
		w = BScreen().Frame().Width() / 3;
	}

	int cnt = sfTenLastDocs.size();
	char **p = (char **)malloc(cnt * sizeof(char *));	
	const char **s = (const char **)malloc(cnt * sizeof(char *));	
	
	for (i = 0; i < cnt; i++)
	{
		s[i] = sfTenLastDocs[i];
		p[i] = (char *)malloc(strlen(s[i]) + 4);
	}

	be_bold_font->GetTruncatedStrings(s, cnt, B_TRUNCATE_SMART, w, p);
	
	for (i = 0; i < cnt; i++)
	{
		BMessage *msg = new BMessage(msg_OpenRecent);
		
		entry_ref ref;
		if (get_ref_for_path(s[i], &ref) == B_OK)
		{
			msg->AddRef("refs", &ref);
			fRecent->AddItem(new BMenuItem(p[i], msg));
		}
		else if (strncmp(s[i], "ftp://", 6) == 0)
		{
			msg->AddString("url", s[i]);
			fRecent->AddItem(new BMenuItem(p[i], msg));
		}
		else
			delete msg;

		free(p[i]);
	}
	
	free(s);
	free(p);

	fText->MenusBeginning();
} /* PDoc::MenusBeginning */

void PDoc::UpdateShortCuts()
{
	int used = 0, i;
	PDoc *doc;
	
	doclist unassigned;
	doclist::iterator di;
	
	for (di = sfDocList.begin(); di != sfDocList.end(); di++)
	{
		doc = dynamic_cast<PDoc*>(*di);
		if (!doc)
			continue;

		if (!doc->IsWorksheet() && doc->fShortcut >= 10)
			unassigned.push_back(doc);
		else
			used |= 1 << doc->fShortcut;
	}
	
	used ^= ~0;
	
	for (i = 1; i < 10 && unassigned.size(); i++)
	{
		if (used & (1 << i))
		{
			doc = static_cast<PDoc*>(unassigned.front());
			doc->fShortcut = i;
			unassigned.erase(unassigned.begin());
		}
	}
} /* PDoc::UpdateShortCuts */

static void RemoveShortcuts(BMenu *menu)
{
	BMenuItem *item;
	int cnt = menu->CountItems();
	
	while (cnt--)
	{
		item = menu->ItemAt(cnt);
		
		if (item->Submenu())
			RemoveShortcuts(item->Submenu());
		else
			item->SetShortcut(0, 0);
	}
} /* RemoveShortcuts */

void PDoc::ResetMenuShortcuts()
{
	RemoveShortcuts(fMBar);

	const keymap& km = CKeyMapper::Instance().Keymap();
	keymap::const_iterator ki;

	key_map *keyMap;
	char *charMap;

	get_key_map(&keyMap, &charMap);
	
	for (ki = km.begin(); ki != km.end(); ki++)
	{
		if ((*ki).first.prefix == 0)
		{
			BMenuItem *item;
			
			if (((*ki).second & 0xffffff00) == 0x65787400)
			{
				int nr = (*ki).second - 'ext0';
				item = fMBar->SubmenuAt(4)->ItemAt(nr);
			}
			else
				item = fMBar->FindItem((*ki).second);

			char *ch = charMap + keyMap->normal_map[(*ki).first.combo & 0x00ff];
			long modifiers = (*ki).first.combo >> 16;

			if (item && *ch++ == 1 && modifiers & B_COMMAND_KEY)
				item->SetShortcut(toupper(*ch), modifiers);
		}
	}
} /* PDoc::ResetMenuShortcuts */

void PDoc::ShowRecentMenu(BPoint where, bool showalways)
{
	if (! showalways)
	{
		BPoint Where;
		unsigned long btns;
		bigtime_t longEnough = system_time() + 250000;
	
		fText->GetMouse(&Where, &btns);
	
		do
		{
			BPoint p;
			
			fText->GetMouse(&p, &btns);
	
			if (!btns)
			{
				be_app->PostMessage(msg_Open);
				fButtonBar->SetDown(msg_BtnOpen, false);
				return;
			}
			
			if (fabs(Where.x - p.x) > 2 || fabs(Where.y - p.y) > 2)
				break;
		}
		while (system_time() < longEnough);
	}

	BPopUpMenu popup("Recent");
	BMenuItem *item;
	
	MenusBeginning();
	
	for (int i = 0; i < fRecent->CountItems(); i++)
	{
		item = fRecent->ItemAt(i);
		
		popup.AddItem(new BMenuItem(item->Label(), new BMessage(*item->Message())));
	}
	
	popup.SetFont(be_plain_font);
	
	if (popup.CountItems() == 0)
		popup.AddItem(new BMenuItem("Empty", NULL));

	BRect r;
	
	r.Set(where.x - 4, where.y - 20, where.x + 24, where.y + 4);

	popup.SetTargetForItems(this);
	popup.Go(where, true, true, r);

	fButtonBar->SetDown(msg_BtnOpen, false);
} /* PDoc::ShowRecentMenu */

#pragma mark - Dialogs

void PDoc::AddDialog(HDialog *inDialog, bool inWindowModal)
{
	fDialogs.push_back(inDialog);
	if (inWindowModal)
		fWindowModal = inDialog;
} /* PDoc::AddDialog */

void PDoc::RemoveDialog(HDialog *inDialog)
{
	vector<HDialog*>::iterator i = find(fDialogs.begin(), fDialogs.end(), inDialog);
	if (i != fDialogs.end())
	{
		fDialogs.erase(i);
		if (fWindowModal == inDialog)
			fWindowModal = NULL;
	}
} /* PDoc::RemoveDialog */

void PDoc::MakeModal(HDialog *inDialog)
{
	if (!fWindowModal)
		fWindowModal = inDialog;
} /* PDoc::MakeModal */
//
//HDialog* PDoc::GetOpenSelectionDialog()
//{
//	BWindow *d;
//	int i;
//	
//	for (i = 0; i < fDialogs.CountItems(); i++)
//	{
//		d = (BWindow *)fDialogs.ItemAt(i);
//		if (typeid(*d) == typeid(COpenSelection))
//		{
//			d->SetWorkspaces(1 << current_workspace());
//			d->Activate();
//			return static_cast<COpenSelection*>(d);
//		}
//	}
//	
//	COpenSelection *dlog = DialogCreator<COpenSelection>::CreateDialog(this);
//	dlog->Show();
//	
//	return dlog;
//} /* CCellWindow::GetOpenSelectionDialog */
//
//HDialog* PDoc::GetGoToLineDialog()
//{
//	BWindow *d;
//	int i;
//	
//	for (i = 0; i < fDialogs.CountItems(); i++)
//	{
//		d = (BWindow *)fDialogs.ItemAt(i);
//		if (typeid(*d) == typeid(CGoToLine))
//		{
//			d->SetWorkspaces(1 << current_workspace());
//			d->Activate();
//			return static_cast<CGoToLine*>(d);
//		}
//	}
//	
//	CGoToLine *dlog = DialogCreator<CGoToLine>::CreateDialog(this);
//	dlog->Show();
//	
//	return dlog;
//} /* CCellWindow::GetGoToLineDialog */
//
//HDialog* PDoc::GetInfoDialog()
//{
//	BWindow *d;
//	int i;
//	
//	for (i = 0; i < fDialogs.CountItems(); i++)
//	{
//		d = (BWindow *)fDialogs.ItemAt(i);
//		if (typeid(*d) == typeid(CInfoDialog))
//		{
//			d->SetWorkspaces(1 << current_workspace());
//			d->Activate();
//			return static_cast<CInfoDialog*>(d);
//		}
//	}
//	
//	CInfoDialog *dlog = DialogCreator<CInfoDialog>::CreateDialog(this);
//	dlog->Show();
//	
//	return dlog;
//} /* CCellWindow::GetInfoDialog */
//
//HDialog* PDoc::GetConvertDialog()
//{
//	BWindow *d;
//	int i;
//	
//	for (i = 0; i < fDialogs.CountItems(); i++)
//	{
//		d = (BWindow *)fDialogs.ItemAt(i);
//		if (typeid(*d) == typeid(CConvertDialog))
//		{
//			d->SetWorkspaces(1 << current_workspace());
//			d->Activate();
//			return static_cast<CConvertDialog*>(d);
//		}
//	}
//	
//	CConvertDialog *dlog = DialogCreator<CConvertDialog>::CreateDialog(this);
//	dlog->Show();
//	
//	return dlog;
//} /* CCellWindow::GetConvertDialog */
//
////HDialog* PDoc::GetDiffOptionsDialog()
////{
////	BWindow *d;
////	int i;
////	
////	for (i = 0; i < fDialogs.CountItems(); i++)
////	{
////		d = (BWindow *)fDialogs.ItemAt(i);
////		if (typeid(*d) == typeid(CDiffOptions))
////		{
////			d->Activate();
////			return static_cast<CDiffOptions*>(d);
////		}
////	}
////	
////	CDiffOptions *dlog = DialogCreator<CDiffOptions>::CreateDialog(this);
////	dlog->Show();
////	
////	return dlog;
////} /* CCellWindow::GetDiffOptionsDialog */
////

#pragma mark - IDE

void PDoc::IDEBringToFront()
{
	entry_ref ide;
	if (be_roster->FindApp("application/x-mw-BeIDE", &ide))
		THROW(("BeIDE was not found"));

	if (be_roster->IsRunning(&ide))
		be_roster->ActivateApp(be_roster->TeamFor(&ide));
	else if (be_roster->Launch(&ide) != B_OK)
		THROW(("Could not launch BeIDE"));
} /* PDoc::IDEBringToFront */

void PDoc::IDEAddFile()
{
	BMessage msg(kCreateVerb), reply;
	
	PropertyItem item;
	item.form = formDirect;
	strcpy(item.property, "file");
	msg.AddData("target", PROPERTY_TYPE, &item, sizeof(item));
	strcpy(item.property, "project");
	msg.AddData("target", PROPERTY_TYPE, &item, sizeof(item));
	
	msg.AddRef("data", fFile);
	
	IDEBringToFront();
	SendToIDE(msg, &reply);
	if (reply.HasInt32("error"))
		THROW(("An error occured adding the file"));
} /* PDoc::IDEAddFile */

void PDoc::IDERemoveFile()
{
	BMessage msg(kDeleteVerb), reply;
	
	PropertyItem item;
	item.form = formDirect;
	strcpy(item.property, "file");
	msg.AddData("target", PROPERTY_TYPE, &item, sizeof(item));
	strcpy(item.property, "project");
	msg.AddData("target", PROPERTY_TYPE, &item, sizeof(item));
	
	msg.AddRef("data", fFile);
	
	IDEBringToFront();
	SendToIDE(msg, &reply);
	if (reply.HasInt32("error"))
		THROW(("An error occured removing the file"));
} /* PDoc::IDERemoveFile */

void PDoc::IDEMake()
{
	PostToAll(msg_Save, false);		// Save all documents first

	BMessage msg(kMakeProject);
	PropertyItem item;
	item.form = formDirect;
	strcpy(item.property, "project");
	msg.AddData("target", PROPERTY_TYPE, &item, sizeof(item));
	
	entry_ref ide;
	if (be_roster->FindApp("application/x-mw-BeIDE", &ide))
		THROW(("BeIDE was not found"));

	if (be_roster->IsRunning(&ide))
	{
		BMessenger msgr(NULL, be_roster->TeamFor(&ide));
		msgr.SendMessage(&msg);
	}
	else
		THROW(("BeIDE is not running"));	
} /* PDoc::IDEMake */

void PDoc::IDEProject2Group()
{
	BMessage msg(kGetVerb), reply;
	PropertyItem item;
	item.form = formDirect;
	strcpy(item.property, "files");
	msg.AddData("target", PROPERTY_TYPE, &item, sizeof(item));
	strcpy(item.property, "project");
	msg.AddData("target", PROPERTY_TYPE, &item, sizeof(item));
	
	SendToIDE(msg, &reply);

	if (reply.HasInt32("error"))
		THROW(("An error occured while retrieving the project files"));
	else
	{
		PGroupWindow *gw = new PGroupWindow;
		
		BMessage msg(B_REFS_RECEIVED);
		
		int32 c = 0;
		entry_ref ref;
#if (B_BEOS_VERSION > B_BEOS_VERSION_5)
		// BeOS > R5 wants the name as const char *, we obey...
		const char *name;
#else
		char *name;
#endif
		type_code type;
		
		FailOSErr(reply.GetInfo(B_REF_TYPE, 0, &name, &type));
		
		while (reply.FindRef(name, c++, &ref) == B_OK)
			msg.AddRef("refs", &ref);
		
		gw->PostMessage(&msg);
	}
} /* PDoc::IDEProject2Group */

const long cmd_AndyFeature = 4000;
const char kFileName[] = "File", kTextFileRef[] = "TextRef";

bool PDoc::IDEOpenSourceHeader(entry_ref& ref)
{
	entry_ref ide;
	if (be_roster->FindApp("application/x-mw-BeIDE", &ide))
		return false;

	if (be_roster->IsRunning(&ide))
	{
		BMessage msg(cmd_AndyFeature);
		
		msg.AddRef(kTextFileRef, &ref);
		msg.AddString(kFileName, ref.name);

		BMessenger msgr(NULL, be_roster->TeamFor(&ide));
		msgr.SendMessage(&msg);
		
		return true;
	}
	else
		return false;
} /* PDoc::IDEOpenSourceHeader */

#pragma mark - Positioning

void PDoc::Stack()
{
	BRect r;
	int c;
	
	c = 1 << current_workspace();
	
	{
		BScreen s;
		r = s.Frame();
	}
	
	r.top += 15;
	r.InsetBy(14, 9);
	
	doclist::iterator di;
	
	for (di = sfDocList.begin(); di != sfDocList.end(); di++)
	{
		PDoc *w = dynamic_cast<PDoc*>(*di);
		
		if (w == NULL || w->IsWorksheet())
			continue;
		
		w->SetWorkspaces(c);

		w->MoveTo(r.left, r.top);
		w->ResizeTo(r.right - r.left, r.bottom - r.top);
		
		r.top += 20;
		r.left += 5;
		
		w->Activate(true);
	}
} /* PDoc::Stack */

void PDoc::Tile()
{
	BRect r;
	int cnt, ws, ch, cv, ww, wh;
	
	ws = 1 << current_workspace();
	
	{
		BScreen s;
		r = s.Frame();
	}

	r.InsetBy(9, 9);

	cnt = 0;
	
	doclist::iterator di;
	
	for (di = sfDocList.begin(); di != sfDocList.end(); di++)
		{
		PDoc *w = dynamic_cast<PDoc *>(*di);
		if (w && !w->IsWorksheet())
			cnt++;
	}
	
	ch = cnt;
	cv = 1;
	
	while (cv < ch)
	{
		cv++;
		if (cnt % cv)
			ch = (cnt / cv) + 1;
		else
			ch = (cnt / cv);
	}
	
	ww = (int)r.Width() / ch;
	wh = (int)r.Height() / cv;
	
	cnt = 0;
	for (di = sfDocList.begin(); di != sfDocList.end(); di++)
	{
		PDoc *w = dynamic_cast<PDoc *>(*di);
		if (w && !w->IsWorksheet())
		{
		w->SetWorkspaces(ws);
		
			w->MoveTo(r.left + (cnt % ch) * ww, 20.0 + r.top + (cnt / ch) * wh);
		w->ResizeTo(ww - 8.0, wh - 15.0);
		
		w->Activate();
			cnt++;
		}
	}
} /* PDoc::Tile */

#pragma mark - Scripting

BHandler* PDoc::ResolveSpecifier(BMessage *msg, int32 index,
						BMessage *specifier, int32 form, const char *property)
{
	if (strcmp(property, "Line") == 0)
	{
		long indx, range;
		
		if (form == B_INDEX_SPECIFIER &&
			specifier->FindInt32("index", &indx) == B_OK &&
			indx > 0 && indx <= fText->LineCount())
		{
			msg->PopSpecifier();
			return new CLineObject(fText, fText->RealLine2Line(indx - 1), 1);
		}
		else if (form == B_RANGE_SPECIFIER &&
			specifier->FindInt32("index", &indx) == B_OK &&
			specifier->FindInt32("range", &range) == B_OK)
		{
			msg->PopSpecifier();
			return new CLineObject(fText, fText->RealLine2Line(indx), range);
		}
		else
			return NULL;
	}
	else if (strcmp(property, "Char") == 0 || strcmp(property, "Character") == 0)
	{
		long indx, range;
		
		if (form == B_INDEX_SPECIFIER &&
			specifier->FindInt32("index", &indx) == B_OK &&
			indx > 0 && indx <= fText->Size())
		{
			msg->PopSpecifier();
			return new CCharObject(fText, indx, 1);
		}
		else if (form == B_RANGE_SPECIFIER &&
			specifier->FindInt32("index", &indx) == B_OK &&
			specifier->FindInt32("range", &range) == B_OK)
		{
			msg->PopSpecifier();
			return new CCharObject(fText, indx, range);
		}
		else
			return NULL;
	}
	else
		return BWindow::ResolveSpecifier(msg, index, specifier, form, property);
} /* PDoc::ResolveSpecifier */

status_t PDoc::GetSupportedSuites(BMessage *data)
{
	status_t err;
	err = data->AddString("suites", "suite/x-vnd.Hekkel-textdoc");
	if (err) return err;
	return BWindow::GetSupportedSuites(data);
} /* PDoc::GetSupportedSuites */

