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

	Created: 03/09/98 23:14:35
*/

#include "pe.h"
#include "CHTMLBar.h"
#include "PApp.h"
#include "HPreferences.h"
#include "PDoc.h"
#include "PText.h"
#include "PMessages.h"
#include "HAppResFile.h"
#include "HStream.h"
#include "HDefines.h"
#include "ResourcesMisc.h"

CHTMLBar *CHTMLBar::sfInstance;

CHTMLBar* CHTMLBar::Instance()
{
	if (sfInstance == NULL)
		sfInstance = new CHTMLBar();

	return sfInstance;
} /* CHTMLBar::Instance */

void CHTMLBar::Close()
{
	if (sfInstance)
	{
		sfInstance->Lock();
	
		gPrefs->SetPrefRect("HTMLPalettePos", sfInstance->Frame());

		sfInstance->Quit();
	}
} /* CHTMLBar::Close */

CHTMLBar::CHTMLBar()
	: BWindow(gPrefs->GetPrefRect("HTMLPalettePos", BRect(20, 20, 120, 120)),
		"HTML Palette",
		B_FLOATING_WINDOW,
		B_AVOID_FRONT | B_AVOID_FOCUS | B_WILL_ACCEPT_FIRST_CLICK | B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
{
	const void *p;
	size_t size;
	
	if (gPrefsDir.Contains("HTML Palette"))
	{
		try
		{
			BEntry e;
			BFile f;
			BResources r;
			
			FailOSErr(gPrefsDir.FindEntry("HTML Palette", &e, true));
			FailOSErr(f.SetTo(&e, B_READ_ONLY));
			FailOSErr(r.SetTo(&f));
			
			size_t size;
			p = r.FindResource(rtyp_Htmp, (int32)rid_Htmp_HtmlPalette, &size);
			if (p == NULL) THROW(("The custom HTML Palette file does not contain a valid palette resource"));
		}
		catch (HErr& e)
		{
			e.DoError();
			p = HResources::GetResource(rtyp_Htmp, rid_Htmp_HtmlPalette, size);
		}
	}
	else
		p = HResources::GetResource(rtyp_Htmp, rid_Htmp_HtmlPalette, size);
	
	if (p == NULL) THROW(("Could not find a palette resource"));
	
	BMemoryIO data(p, size);
	BPositionIO& buf = data;
	
	char barname[256];
	long flags, width;
	
	buf >> barname >> flags >> width;
	
	BRect frame(Bounds());
	frame.right = width;
	
	AddChild(fButtonPane = new CHTMLButtonPane(frame, "buttonpane", data));

	SetTitle(barname);
	ResizeTo(width, fButtonPane->Bounds().Height() - 2);
	
	BRect screenR;
	do { screenR = BScreen().Frame(); } while (false);
	
	frame = Frame();
	if (frame.left < screenR.left)			frame.OffsetBy(screenR.left - frame.left, 0);
	if (frame.top < screenR.top)			frame.OffsetBy(0, screenR.top - frame.top);
	if (frame.right > screenR.right)			frame.OffsetBy(screenR.right - frame.right, 0);
	if (frame.bottom > screenR.bottom)	frame.OffsetBy(0, screenR.bottom - frame.bottom);
	
	if (Frame() != frame)
		MoveTo(frame.left, frame.top);
} /* CHTMLBar::CHTMLBar */
			
void CHTMLBar::MessageReceived(BMessage *msg)
{
	BWindow::MessageReceived(msg);
} /* CHTMLBar::MessageReceived */

bool CHTMLBar::QuitRequested()
{
	Hide();
	return false;
} /* CHTMLBar::QuitRequested */

#pragma mark - Pane

CHTMLButtonPane::CHTMLButtonPane(BRect frame, const char *name, BPositionIO& data)
	: BView(frame, name, 0, B_WILL_DRAW)
{
	long bCnt;
	
	data >> bCnt;
	
	be_plain_font->GetHeight(&fFH);

	BRect r;
	r = Bounds();
	r.bottom = r.top + ButtonHeight();

	while (bCnt--)
	{
		long type;
		
		data >> type;
		
		switch (type)
		{
			case 'btn ':
				fButtons.push_back(new CHTMLSimpleButton(r, data, this));
				break;
			case 'menu':
				fButtons.push_back(new CHTMLMenu(r, data, this));
				break;
			case 'cmnd':
				fButtons.push_back(new CHTMLCmdButton(r, data, this));
				break;
			case 'dlog':
				fButtons.push_back(new CHTMLDialog(r, data, this));
				break;
		}
		
		r.OffsetBy(0, ButtonHeight());
	}
	
	ResizeTo(r.Width(), r.top);
} /* CHTMLButtonPane::CHTMLButtonPane */
			
void CHTMLButtonPane::Draw(BRect /*update*/)
{
	for (int i = 0; i < fButtons.size(); i++)
		fButtons[i]->Draw();
} /* CHTMLButtonPane::Draw */

void CHTMLButtonPane::MouseDown(BPoint where)
{
	for (int i = 0; i < fButtons.size(); i++)
		if (fButtons[i]->Frame().Contains(where))
		{
			fButtons[i]->Click(where);
			break;
		}
} /* CHTMLButtonPane::MouseDown */

#pragma mark - Button

CHTMLButton::CHTMLButton(BRect frame, BPositionIO& data, CHTMLButtonPane *pane)
{
	fFrame = frame;
	fPane = pane;
	
	char label[256];
	data >> label;
	fLabel = strdup(label);
} /* CHTMLButton::CHTMLButton */

CHTMLButton::~CHTMLButton()
{
	free(fLabel);
} /* CHTMLButton::~CHTMLButton */

void CHTMLButton::Draw()
{
	fPane->SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	fPane->SetFont(be_plain_font);
	
	fPane->FillRect(fFrame, B_SOLID_LOW);

	BRect r = fFrame;

	r.bottom -= 1;
	
	fPane->BeginLineArray(9);
	
	fPane->AddLine(r.LeftBottom(), r.RightBottom(), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_4_TINT));
	
	r.bottom -= 1;
	
	fPane->AddLine(BPoint(r.left + 1, r.bottom), r.RightBottom(), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
	fPane->AddLine(r.RightBottom(), BPoint(r.right, r.top + 1), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));

	r.InsetBy(1, 1);

	fPane->AddLine(r.LeftBottom(), r.RightBottom(), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	fPane->AddLine(r.RightBottom(), r.RightTop(), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	fPane->AddLine(r.LeftTop(), r.RightTop(), kWhite);
	fPane->AddLine(r.LeftTop(), r.LeftBottom(), kWhite);

	r.InsetBy(1, 1);

	fPane->AddLine(r.LeftTop(), r.RightTop(), kWhite);
	fPane->AddLine(r.LeftTop(), r.LeftBottom(), kWhite);
	
	fPane->EndLineArray();	
	
	fPane->SetHighColor(kBlack);
	
	fPane->MovePenTo(r.left + 5, r.bottom - fPane->BaseLine());
	fPane->DrawString(fLabel);
} /* CHTMLButton::Draw */

bool CHTMLButton::TrackClick()
{
	BPoint p;
	unsigned long btns;
	bool in = false;
	
	do
	{
		fPane->GetMouse(&p, &btns);
		
		if (fFrame.Contains(p))
		{
			if (! in)
			{
				in = true;
				
				BRect r(fFrame);
				r.bottom -= 2;
				
				r.InsetBy(1, 1);

				fPane->SetHighColor(kWhite);
				fPane->SetLowColor(kBlack);

				fPane->FillRect(r, B_SOLID_LOW);
				
				fPane->MovePenTo(r.left + 6, r.bottom - fPane->BaseLine() - 1);
				fPane->DrawString(fLabel);
			}
		}
		else
		{
			if (in)
			{
				in = false;
				Draw();
			}
		}
		
		snooze(10000);
	}
	while (btns);
	
	if (in)
		Draw();

	return in;
} /* CHTMLButton::TrackClick */
			
CHTMLSimpleButton::CHTMLSimpleButton(BRect frame, BPositionIO& data, CHTMLButtonPane *pane)
	: CHTMLButton(frame, data, pane)
{
	char glos[256];
	data >> glos;
	fGlossary = strdup(glos);
} /* CHTMLSimpleButton::CHTMLSimpleButton */

CHTMLSimpleButton::~CHTMLSimpleButton()
{
	free(fGlossary);
} /* CHTMLSimpleButton::~CHTMLSimpleButton */
			
void CHTMLSimpleButton::Draw()
{
	CHTMLButton::Draw();
} /* CHTMLSimpleButton::Draw */

void CHTMLSimpleButton::Click(BPoint /*where*/)
{
	if (TrackClick())
	{
		PDoc *doc = PDoc::TopWindow();
		if (doc)
		{
			BMessage msg(msg_HtmlButton);
			
			msg.AddString("glossy", fGlossary);
			
			doc->PostMessage(&msg, doc->TextView());
		}
	}
} /* CHTMLSimpleButton::Click */
			
CHTMLMenu::CHTMLMenu(BRect frame, BPositionIO& data, CHTMLButtonPane *pane)
	: CHTMLButton(frame, data, pane)
{
	fMenu = new BPopUpMenu("mymenu");
	fMenu->SetFont(be_plain_font);
	
	char label[256], glos[256];
	
	while (true)
	{
		data >> label;
		
		if (label[0] == 0)
			break;
		
		data >> glos;
		
		fMenu->AddItem(new CItem(label, glos));
	}
} /* CHTMLMenu::CHTMLMenu */

CHTMLMenu::~CHTMLMenu()
{
	delete fMenu;
} /* CHTMLMenu::~CHTMLMenu */

void CHTMLMenu::Draw()
{
	CHTMLButton::Draw();
	
	float x = fFrame.right - 10;
	float y = fFrame.top + 7;
	
	fPane->BeginLineArray(3);
	fPane->AddLine(BPoint(x, y), BPoint(x + 4, y), kBlack);
	y += 1; x += 1;
	fPane->AddLine(BPoint(x, y), BPoint(x + 2, y), kBlack);
	y += 1; x += 1;
	fPane->AddLine(BPoint(x, y), BPoint(x, y), kBlack);
	fPane->EndLineArray();
} /* CHTMLMenu::Draw */

void CHTMLMenu::Click(BPoint /*where*/)
{
	BRect r(fFrame);
	r.bottom -= 2;
	
	r.InsetBy(1, 1);

	fPane->SetHighColor(kWhite);
	fPane->SetLowColor(kBlack);

	fPane->FillRect(r, B_SOLID_LOW);
	
	fPane->MovePenTo(r.left + 6, r.bottom - 3);
	fPane->DrawString(fLabel);

	CItem *item = dynamic_cast<CItem*>(fMenu->Go(fPane->ConvertToScreen(fFrame.LeftBottom()), false, true, fPane->ConvertToScreen(fFrame)));

	if (item)
	{
		PDoc *doc = PDoc::TopWindow();
		if (doc)
		{
			BMessage msg(msg_HtmlButton);
			
			msg.AddString("glossy", item->fGlossy);
			
			doc->PostMessage(&msg, doc->TextView());
		}
	}
	
	Draw();
} /* CHTMLMenu::Click */

CHTMLCmdButton::CHTMLCmdButton(BRect frame, BPositionIO& data, CHTMLButtonPane *pane)
	: CHTMLButton(frame, data, pane)
{
	data >> fCmd;
} /* CHTMLCmdButton::CHTMLCmdButton */
			
void CHTMLCmdButton::Click(BPoint /*where*/)
{
	if (TrackClick())
	{
		PDoc *doc = PDoc::TopWindow();
		if (doc)
			doc->PostMessage(fCmd, doc->TextView());
	}
} /* CHTMLCmdButton::Click */

CHTMLDialog::CHTMLDialog(BRect frame, BPositionIO& data, CHTMLButtonPane *pane)
	: CHTMLButton(frame, data, pane)
{
	char s[512];
	data >> s;
	
	fExt = strdup(s);
} /* CHTMLDialog::CHTMLDialog */

CHTMLDialog::~CHTMLDialog()
{
	free(fExt);
} /* CHTMLDialog::~CHTMLDialog */

void CHTMLDialog::Click(BPoint /*where*/)
{
	if (TrackClick())
	{
		PDoc *doc = PDoc::TopWindow();
		if (doc)
		{
			BMessage msg(msg_HtmlExtension);
			
			msg.AddString("ext", fExt);
			
			doc->PostMessage(&msg, doc->TextView());
		}
	}
} /* CHTMLDialog::Click */
