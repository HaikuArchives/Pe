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

	Created: 11/12/97 10:36:38 by Maarten Hekkelman
*/

#include "pe.h"
#include "PGlossyWindow.h"
#include "CGlossary.h"
#include "PMessages.h"
#include "PDoc.h"
#include "PText.h"
#include "PApp.h"
#include "HError.h"
#include "HDefines.h"
#include "HPreferences.h"
#include "Prefs.h"

PGlossyWindow *gGlossyWindow = NULL;

class PGlossyItem : public BListItem {
public:
		PGlossyItem(const char *name, int ch, int modifiers);
		~PGlossyItem();
	
virtual	void DrawItem(BView *owner, BRect bounds, bool complete = false);
virtual	void Update(BView *owner, const BFont *font);
		
		int Char() const;
		int Modifiers() const;
		
private:
		char *fName;
		int fChar, fModifiers;
};

PGlossyItem::PGlossyItem(const char *name, int ch, int modifiers)
{
	fName = strdup(name);
	FailNil(fName);

	fChar = ch;
	fModifiers = modifiers;
} /* PGlossyItem::PGlossyItem */

PGlossyItem::~PGlossyItem()
{
	if (fName) free(fName);
} /*PGlossyItem::~PGlossyItem */

void PGlossyItem::DrawItem(BView *owner, BRect bounds, bool /*complete*/)
{
	BPoint p = bounds.LeftBottom();
	p.y -= 2; p.x += 2;
	
	if (IsSelected())
	{
		owner->SetLowColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
		owner->SetHighColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	}
	else
	{	
		owner->SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
		owner->SetHighColor(kWhite);
	}
	
	owner->FillRect(bounds, B_SOLID_LOW);
	
	owner->StrokeLine(bounds.LeftTop(), bounds.RightTop());
	owner->SetHighColor(IsSelected() ? tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT) : tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	owner->StrokeLine(bounds.LeftBottom(), bounds.RightBottom());
	
	owner->SetHighColor(kBlack);
	owner->DrawString(fName, p);
	owner->SetLowColor(kWhite);
} /* PGlossyItem::DrawItem */

void PGlossyItem::Update(BView *owner, const BFont *font)
{
	font_height fh;
	be_plain_font->GetHeight(&fh);
	float ih = fh.ascent + fh.descent + 2;

	SetWidth(owner->Bounds().Width());
	SetHeight(ih);
} /* PGlossyItem::Update */

inline int PGlossyItem::Char() const
{
	return fChar;
} /* PGlossyItem::Char */

inline int PGlossyItem::Modifiers() const
{
	return fModifiers;
} /* PGlossyItem::Char */

#pragma mark -

PGlossyWindow::PGlossyWindow()
	: BWindow(gPrefs->GetPrefRect(prf_R_GlossaryPosition, BRect(100, 100, 200, 300)),
		"Glossary", B_TITLED_WINDOW,
		B_WILL_ACCEPT_FIRST_CLICK | B_NOT_ZOOMABLE)
{
	BRect r(Bounds());
	
	r.right -= B_V_SCROLL_BAR_WIDTH;
	
	fList = new BListView(r, "glossylist", B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
	AddChild(new BScrollView("glossyscroller", fList, B_FOLLOW_ALL_SIDES, 0, false, true, B_NO_BORDER));
	
	fList->SetInvocationMessage(new BMessage(msg_DoGlossy));
	if (gPrefs->GetPrefInt(prf_I_SingleClickGlossary, 0))
		fList->SetSelectionMessage(new BMessage(msg_DoGlossy));
	
	ParseGlossaryFile();
	
	r = Frame();
	{
		BScreen s;
		r = r & s.Frame();
	}
	
	if (!r.IsValid() || r.Width() < 30 || r.Height() < 30)
		r.Set(100, 100, 200, 300);
	
	if (Frame() != r)
	{
		MoveTo(r.left, r.top);
		ResizeTo(r.Width(), r.Height());
	}
	
	Show();
} /* PGlossyWindow::PGlossyWindow */

PGlossyWindow::~PGlossyWindow()
{
} /* PGlossyWindow::~PGlossyWindow */

bool PGlossyWindow::QuitRequested()
{
	gPrefs->SetPrefRect(prf_R_GlossaryPosition, Frame());
	gGlossyWindow = NULL;
	
	return true;
} /* PGlossyWindow::QuitRequested */

void PGlossyWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case msg_DoGlossy:
			DoGlossy();
			break;
			
		case msg_ReloadGlossary:
			ParseGlossaryFile();
			break;
		
		default:
			BWindow::MessageReceived(msg);
	}
} /* PGlossyWindow::MessageReceived */

void PGlossyWindow::DoGlossy()
{
	PDoc *doc = PDoc::TopWindow();
	
	if (!doc)
	{
		beep();
		return;
	}
	
	int ix = fList->CurrentSelection();
	
	if (ix >= 0)
	{
		PGlossyItem *item = dynamic_cast<PGlossyItem*>(fList->ItemAt(ix));
		
		if (item)
		{
			BMessage m(msg_DoGlossy);
			m.AddInt32("char", item->Char());
			m.AddInt32("modifiers", item->Modifiers());
			
			doc->PostMessage(&m, doc->TextView());
		}
	}
} /* PGlossyWindow::DoGlossy */

static BFile& operator>>(BFile& file, char *str)
{
	char c;
	
	do
	{
		if (file.Read(&c, 1) != 1) break;
		*str++ = c;
	}
	while (c != '\n' && c != '\r');

	*str = 0;
	
	return file;
} /* operator>> */

void PGlossyWindow::ParseGlossaryFile()
{
	PGlossyItem *item;
	
	while (fList->CountItems())
	{
		item = dynamic_cast<PGlossyItem*>(fList->ItemAt(0));
		if (item)
		{
			fList->RemoveItem(item);
			delete item;
		}
	}

	BFile file;
	FailOSErr(file.SetTo(&gPrefsDir, "Glossary", B_READ_ONLY));
	
	char s[256];

	file >> s;
	
	if (strncmp(s, "### ", 4)) THROW(("Invalid glossary file"));
	
	file >> s;
	while (*s && strncmp(s, "##", 2) == 0)
	{
		int key = 0, mods = 0;
		char *kp;
		
		kp = strtok(s + 2, " -\n");
		while (kp && strncmp(kp, "##", 2))
		{
			if (strcmp(kp, "control") == 0)
				mods |= B_CONTROL_KEY;
			else if (strcmp(kp, "shift") == 0)
				mods |= B_SHIFT_KEY;
			else if (strcmp(kp, "command") == 0)
				mods |= B_COMMAND_KEY;
			else if (strcmp(kp, "option") == 0)
				mods |= B_OPTION_KEY;
			else if (strcmp(kp, "menu") == 0)
				mods |= B_MENU_KEY;
			else if (strlen(kp) == 1)
				key = *kp;
			else if (tolower(*kp) == 'f')
				key = (strtol(kp + 1, &kp, 10) + 1) | 0x0100;

			kp = strtok(NULL, " -\n");
		}
		
		if (kp && strncmp(kp, "##", 2) == 0 && (kp = strtok(NULL, "\n")) != NULL)
			fList->AddItem(item = new PGlossyItem(kp, key, mods));

		do
			file >> s;
		while (*s && strncmp(s, "##", 2));
	}
} /* PGlossyWindow::ParseGlossaryFile */
