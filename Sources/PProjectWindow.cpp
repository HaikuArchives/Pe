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

	Created: 11/18/98 21:24:03
*/

#include "pe.h"
#include "PProjectWindow.h"
#include "PDoc.h"
#include "Utils.h"
#include "PApp.h"
#include "PMessages.h"
#include "PGroupWindow.h"
#include "PToolBar.h"
#include "PKeyDownFilter.h"
#include "PTypeAHeadList.h"
#include <fs_attr.h>
#include <stack>
#include "HAppResFile.h"
#include "HButtonBar.h"
#include "HError.h"

const unsigned long msg_Done = 'done';

PProjectWindow::PProjectWindow(const entry_ref *doc)
	: BWindow(PDoc::NextPosition(), doc->name, B_DOCUMENT_WINDOW, 0)
	, CDoc("text/x-makefile", this, doc)
{
	fPanel = NULL;
	fToolBar = NULL;
	fButtonBar = NULL;
	
	ResizeTo(180, 400);
	SetSizeLimits(100, 100000, 100, 100000);
	
	BRect r(Bounds());

	BMenuBar *mbar;
	AddChild(mbar = HResources::GetMenuBar(r, 200));
	
	r.bottom = r.top + kToolBarHeight;
	r.OffsetBy(0, mbar->Bounds().bottom + 1);
	
	AddChild(fToolBar = new PToolBar(r, "toolbar"));

	r.bottom -= 2;
	r.OffsetTo(0, 0);
	
	fToolBar->AddChild(fButtonBar = new HButtonBar(r, "buttonbar", 1, this));
	
	r = Bounds();
	r.top = r.bottom - B_H_SCROLL_BAR_HEIGHT + 1;
	r.right -= B_V_SCROLL_BAR_WIDTH;
	AddChild(fStatus = new PGroupStatus(r, doc ? doc->name : NULL));

	r = Bounds();
	r.top = fToolBar->Frame().bottom;
	
	r.right -= B_V_SCROLL_BAR_WIDTH;
	r.bottom -= B_H_SCROLL_BAR_HEIGHT;
	fList = new PTypeAHeadList(r, "group", fStatus);
	fList->SetInvocationMessage(new BMessage(msg_PGroupItemInvoked));
	
	AddChild(new BScrollView("scroller", fList, B_FOLLOW_ALL_SIDES, 0, false, true, B_NO_BORDER));
	
	if (!doc) THROW(("No document defined!"));

	SetTitle(doc->name);
	
	BEntry e;
	FailOSErr(e.SetTo(doc));
	BPath p;
	FailOSErr(e.GetPath(&p));
	fStatus->SetPath(p.Path());
	AddRecent(p.Path());
	
	Read();
	
	if (fFooter == NULL) {
		gApp->NewWindow(fFile);
		Close();
	} else {
		fList->AddFilter(new PKeyDownFilter());
		fList->MakeFocus();
		Show();
	}
} /* PProjectWindow::PProjectWindow */

PProjectWindow::~PProjectWindow()
{
	if (fPanel)
	{
		delete fPanel;
		fPanel = NULL;
	}
	
	free(fHeader);
	free(fFooter);
} /* PProjectWindow::~PProjectWindow */

bool PProjectWindow::QuitRequested()
{
	fWaitForSave = false;

	if (IsDirty() && (fSavePanel == NULL || fSavePanel->IsShowing()))
	{
		if (!fFile)
			fWaitForSave = true;
		Save();
	}
	else
	{
		try
		{
			BFile file;
			
			FailOSErr(file.SetTo(fFile, B_READ_WRITE));
			
			BMessage msg;
			msg.AddRect("windowposition", Frame());
	
			ssize_t s = msg.FlattenedSize();
			char *fm = (char *)malloc(s);
			FailNil(fm);
			FailOSErr(msg.Flatten(fm, s));
			FailIOErr(file.WriteAttr("pe-info", 'info', 0, fm, s));
		}
		catch (HErr& e) {}
	}
	
	return !fWaitForSave;
} /* PProjectWindow::QuitRequested */

void PProjectWindow::MessageReceived(BMessage *msg)
{
	if (msg->WasDropped() && msg->HasRef("refs"))
		AddRefs(msg);
	else
		switch (msg->what)
		{
			case msg_PGroupItemInvoked:
				OpenItem();
				break;
			
			case msg_PRemove:
			{
				int s = 0;
				do
				{
					if (fList->IsItemSelected(s))
						delete fList->RemoveItem(s);
				}
				while (fList->IsItemSelected(s) ||
					(s = fList->CurrentSelection(s)) > 0 && s < fList->CountItems());
				
				Save();
				break;
			}
			
			case msg_Save:
				Save();
				break;
			
			case msg_SaveAs:
				SaveAs();
				break;
			
			case msg_PAdd:
				AddFiles();
				SetDirty(true);
				break;
			
			case B_REFS_RECEIVED:
				AddRefs(msg);
				break;
			
			case B_SAVE_REQUESTED:
			{
				entry_ref dir;
				const char *name;
				
				FailOSErr(msg->FindRef("directory", &dir));
				FailOSErr(msg->FindString("name", &name));
				SaveRequested(dir, name);
				break;
			}
			
			case msg_Close:
				PostMessage(B_QUIT_REQUESTED);
				break;
			
			case msg_EditAsText:
				gApp->NewWindow(fFile);
				Close();
				break;
			
			default:
				BWindow::MessageReceived(msg);
				break;
		}
} /* PProjectWindow::MessageReceived */

//-----------------

const char *skip_to(const char *t, char c);
const char *skip_white(const char *t);
const char *next_path(const char *t, const char *& p, int& pl);

const char *skip_to(const char *t, char c)
{
	while (*t && *t != c)
		t++;
	return t;
} // skip_to

const char *skip_white(const char *t)
{
	while (*t)
	{
		if (*t == '\\' && t[1] == '\n')
			t += 2;
		if (*t == ' ' || *t == '\t')
			t++;
		else if (*t == '#')
			t = skip_to(t + 1, '\n');
		else
			break;
	}

	return t;
} // skip_white

const char *next_path(const char *t, const char *& p, int& pl)
{
	if (*t == '"')
	{
		p = ++t;
		t = strchr(t, '"');
		if (t)
		{
			pl = t - p;
			t++;
		}
		else
			p = NULL;
	}
	else if (*t == '\'')
	{
		p = ++t;
		t = strchr(t, '\'');
		if (t)
		{
			pl = t - p;
			t++;
		}
		else
			p = NULL;
	}
	else if (*t)
	{
		p = t;
		while (*t && ! isspace(*t))
			t++;
		pl = t - p;
	}
	else
		p = NULL;
	
	return t;
} // next_path

const char *PProjectWindow::AddGroup(const char *t, BDirectory& d)
{
	t = skip_white(t);

	if (isalnum(*t))
	{
		const char *n = t;
		
		while (isalnum(*t) || *t == '_')
			t++;
		
		int nl = t - n;
		
		t = skip_white(t);
		
		if (*t != '=')
			return skip_to(t, '\n');

		t++;
		
		string s(n, nl);
		BStringItem *group;

		fList->AddItem(group = new BStringItem(s.c_str()));
		
		t = skip_white(t);
		
		stack<BListItem*> lst;
		
		while (true)
		{
			const char *p;
			int pl;
			
			t = next_path(t, p, pl);
			if (p == NULL)
				break;

			s.assign(p, pl);

			entry_ref ref;
			BEntry e;

				// do not follow links!!!			
			if (d.FindEntry(s.c_str(), &e, false) == B_OK && e.GetRef(&ref) == B_OK)
				lst.push(new PGroupItem(ref));
			else
				lst.push(new BStringItem(s.c_str()));
			
			t = skip_white(t);

			if (*t == '\n')
				break;
		}
		
		while (lst.size())
		{
			fList->AddUnder(lst.top(), group);
			lst.pop();
		}
	}

	if (*t)
	{
		t = skip_to(t, '\n');
		if (t) t++;
	}

	return t;
} // PProjectWindow::AddGroup

void PProjectWindow::ReadData(BPositionIO&)
{
	if (!fFile)
		THROW(("Can only read local makefiles"));

	char *t = NULL;
	
	try
	{
		BDirectory d;
		BFile file;

		FailOSErr(BEntry(fFile, true).GetParent(&d));
		FailOSErr(file.SetTo(fFile, B_READ_ONLY));
		
		size_t size = file.Seek(0, SEEK_END);
		file.Seek(0, SEEK_SET);
		
		t = (char *)malloc(size);
		
		if (file.Read(t, size) != size) THROW(("Error reading makefile"));

		const char *s, *e;
		
		s = strstr(t, "#%{");
		e = s ? strstr(s, "#%}") : s;
		
		if (s < e)
		{
			s = skip_to(s + 3, '\n') + 1;
			
			int l = s - t;
			fHeader = (char *)malloc(l + 1);
			FailNil(fHeader);
			memcpy(fHeader, t, l);
			fHeader[l] = 0;
			
			l = t + size - e;
			fFooter = (char *)malloc(l + 1);
			FailNil(fFooter);
			memcpy(fFooter, e, l);
			fFooter[l] = 0;
			
			t[e - t] = 0;
			
			while (s < e)
				s = AddGroup(s, d);
		}
		else
		{
			fHeader = strdup(t);
			fFooter = NULL;
		}
				
		free(t);
	}
	catch (HErr& e)
	{
		free(t);
		e.DoError();
	}	
} /* PProjectWindow::ReadData */

void PProjectWindow::ReadAttr(BFile& file)
{
	char *fm = NULL;
	try
	{
		attr_info ai;
		if (file.GetAttrInfo("pe-info", &ai) == B_NO_ERROR)
		{
			fm = (char *)malloc(ai.size);
			FailNil(fm);
			
			FailIOErr(file.ReadAttr("pe-info", ai.type, 0, fm, ai.size));
			
			BMessage msg;
			FailOSErr(msg.Unflatten(fm));
			
			BRect f;
			if (msg.FindRect("windowposition", &f) == B_NO_ERROR)
			{
				{
					BScreen s;
					f = f & s.Frame();
				}
				if (f.IsValid() && f.Width() > 50 && f.Height() > 50)
				{
					MoveTo(f.left, f.top);
					ResizeTo(f.Width(), f.Height());
				}
			}
		}
	}
	catch (HErr& e) {}
	if (fm) free(fm);
} /* PProjectWindow::ReadAttr */

void PProjectWindow::WriteData(BPositionIO& /*file*/)
{
	if (! fFile)
		THROW(("Can only write to a local file"));
		
	try
	{
		BPath p;
		FailOSErr(BEntry(fFile).GetPath(&p));
		
		FILE *f = fopen(p.Path(), "w");
		if (!f) THROW(("Failed to open file %s", p.Path()));
		
		fputs(fHeader, f);
		fputs("# Pe generated file list:", f);

		int i = 0;
		
		while (i < fList->FullListCountItems())
		{
			BStringItem *sItem = dynamic_cast<BStringItem*>(fList->FullListItemAt(i));
			FailNil(sItem);

			PGroupItem *pItem = dynamic_cast<PGroupItem *>(sItem);
			
			if (pItem == NULL)
				fprintf(f, "\n%s = \\\n", sItem->Text());
			else
			{
				char path[PATH_MAX];
				RelativePath(*fFile, pItem->Ref(), path);
				fprintf(f, "\t%s \\\n", path);
			}

			i++;
		}

		fputs("", f);

		if (fFooter)
			fputs(fFooter, f);
		
		fclose(f);
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* PProjectWindow::WriteData */

void PProjectWindow::WriteAttr(BFile& file)
{
	char *fm = NULL;
	try
	{
		BMessage msg;
		msg.AddRect("windowposition", Frame());

		ssize_t s = msg.FlattenedSize();
		fm = (char *)malloc(s);
		FailNil(fm);
		FailOSErr(msg.Flatten(fm, s));
		FailIOErr(file.WriteAttr("pe-info", 'info', 0, fm, s));
	}
	catch (HErr& e) {}
	if (fm) free(fm);
} /* PProjectWindow::WriteAttr */

void PProjectWindow::SaveRequested(entry_ref& directory, const char *name)
{
	CDoc::SaveRequested(directory, name);
	
	if (fFile)
	{
		SetTitle(name);
		
		BEntry e;
		BPath p;
		FailOSErr(e.SetTo(fFile));
		FailOSErr(e.GetPath(&p));
		
		fStatus->SetPath(p.Path());
	}
} /* PProjectWindow::SaveRequested */

void PProjectWindow::AddRef(const entry_ref& ref)
{
	PGroupItem *item;
	
	for (int i = 0; i < fList->CountItems(); i++)
	{
		item = static_cast<PGroupItem*>(fList->ItemAt(i));
		if (item->Ref() == ref)
			return;
	}

	int ix = fList->FullListCurrentSelection() + 1;
	if (ix <= 0) ix = fList->FullListCountItems();

	fList->AddItem(item = new PGroupItem(ref, 1), ix);
	Save();
} /* PProjectWindow::AddRef */

void PProjectWindow::AddFiles()
{
	if (fPanel)
		fPanel->Show();
	else
	{
		entry_ref ref;
		
		if (fFile)
		{
			BEntry e, p;
			FailOSErr(e.SetTo(fFile));
			FailOSErr(e.GetParent(&p));
			FailOSErr(p.GetRef(&ref));
		}
		
		fPanel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this), &ref);
		fPanel->SetButtonLabel(B_DEFAULT_BUTTON, "Add");
		fPanel->Show();
	}
} /* PProjectWindow::AddFiles */

void PProjectWindow::AddRefs(BMessage *msg)
{
	entry_ref ref;
	int c = 0;
	
	while (msg->FindRef("refs", c++, &ref) == B_OK)
		AddRef(ref);

} /* PProjectWindow::AddRefs */

void PProjectWindow::OpenItem()
{
	PGroupItem *gi;
	gi = dynamic_cast<PGroupItem*>(fList->ItemAt(fList->CurrentSelection()));
	if (gi)
	{
		try
		{
			BNode node;
			FailOSErr(node.SetTo(&gi->Ref()));
			BNodeInfo info;
			FailOSErr(info.SetTo(&node));
			
			char mime[B_MIME_TYPE_LENGTH];
			
			if (info.GetType(mime) || strncmp(mime, "text/", 5))
				OpenInTracker(gi->Ref());
			else
				gApp->OpenWindow(gi->Ref());
		}
		catch (HErr& e)
		{
			e.DoError();
			gApp->OpenWindow(gi->Ref());
		}
	}
} /* PProjectWindow::OpenItem */
