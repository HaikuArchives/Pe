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

	Created: 12/30/97 23:02:18
*/

#include "pe.h"
#include "HHelpWindow.h"
#include "HButtonBar.h"
#include "HAppResFile.h"
#include "HError.h"
#include "HColorUtils.h"
#include "HDefines.h"
#include "HStream.h"

HButton::HButton(HButtonBar *bar, int resID, int cmd, float x, int flags, const char *help)
{
	fBar = bar;
	fCmd = cmd;
	fMenu = (flags & (1 << bfMenu)) != 0;
	fToggle = (flags & (1 << bfToggle)) != 0 || fMenu;
	fDown = false;
	fOn = false;
	fEnabled = true;
	fVisible = true;
	fHelp = strdup(help);
	
	if (resID >= 0)
	{
		fIcon = (unsigned char *)HResources::GetResource('MICN', resID);
		FailNilRes(fIcon);
	}
	else
	{
		fIcon = (unsigned char *)malloc(256);
		FailNil(fIcon);
		
		BBitmap bm(BRect(0, 0, 15, 15), B_COLOR_8_BIT);
		BMimeType mt;
		
		try
		{
			switch (resID)
			{
				case -1:
					FailOSErr(mt.SetTo("application/x-vnd.beunited.pe"));
					FailOSErr(mt.GetIconForType("text/plain", &bm, B_MINI_ICON));
					break;
				case -2:
				{
					BNode n;
					BPath p;
					BNodeInfo ni;
					
					FailOSErr(find_directory(B_USER_CONFIG_DIRECTORY, &p));
					FailOSErr(n.SetTo(p.Path()));
					FailOSErr(ni.SetTo(&n));
					FailOSErr(ni.GetTrackerIcon(&bm, B_MINI_ICON));
					break;
				}
			}
		}
		catch (HErr& e)
		{
			e.DoError();
			resID = 0;
		}
		
		if (resID != -3)
			memcpy(fIcon, bm.Bits(), 256);
	}
	
	fFrame.Set(x, 3, x + 15, 18);
	if (fMenu) fFrame.right += 7;
} /* HButton::HButton */

HButton::~HButton()
{
	free(fHelp);
} /* HButton::~HButton */

void HButton::Draw(bool pushed)
{
	BRect r(0, 0, 15, 15);
	BBitmap icon(r, B_COLOR_8_BIT);
	
	if (pushed)
	{
		unsigned char ic[256];
		for (int i = 0; i < 256; i++)
			ic[i] = gSelectedMap[fIcon[i]];
		icon.SetBits(ic, 256, 0, B_COLOR_8_BIT);
	}
	else if (! fEnabled)
	{
		unsigned char ic[256];
		for (int i = 0; i < 256; i++)
			ic[i] = gDisabledMap[fIcon[i]];
		icon.SetBits(ic, 256, 0, B_COLOR_8_BIT);
	}
	else
		icon.SetBits(fIcon, 256, 0, B_COLOR_8_BIT);

	fBar->SetHighColor(kViewColor);
	fBar->FillRect(fFrame);

	if (fVisible) {
		fBar->SetHighColor(0, 0, 0);
		fBar->SetDrawingMode(B_OP_OVER);
		fBar->DrawBitmap(&icon, fFrame.LeftTop());
		fBar->SetDrawingMode(B_OP_COPY);
		
		if (fMenu)
		{
			BRect r(fFrame);
			r.left = r.right - 6;
			
			if (pushed)
				fBar->SetLowColor(BScreen().ColorForIndex(gSelectedMap[0x1b]));
			else
				fBar->SetLowColor(kViewColor);
			fBar->FillRect(r, B_SOLID_LOW);
			fBar->SetLowColor(kViewColor);
			
			float x = fFrame.left + 18;
			float y = fFrame.top + 7;
			
			fBar->BeginLineArray(3);
			fBar->AddLine(BPoint(x, y), BPoint(x + 4, y), kBlack);
			y += 1; x += 1;
			fBar->AddLine(BPoint(x, y), BPoint(x + 2, y), kBlack);
			y += 1; x += 1;
			fBar->AddLine(BPoint(x, y), BPoint(x, y), kBlack);
			fBar->EndLineArray();
		}
	}
} /* HButton::Draw */

void HButton::SetOn(bool on)
{
	fOn = on;
	Draw(false);
	MouseLeave();
} /* HButton::SetOn */

void HButton::SetDown(bool pushed)
{
	fDown = pushed;
	Draw(fDown);
	MouseLeave();
} /* HButton::Push */

void HButton::SetEnabled(bool enabled)
{
	bool b = false;

	swap(b, fDown);
	MouseLeave();
	swap(b, fDown);
	
	fEnabled = enabled;
	Draw();
} /* HButton::SetEnabled */

void HButton::SetVisible(bool visible)
{
	fVisible = visible;
	Draw();
} /* HButton::SetVisible */

void HButton::MouseEnter(bool pushed)
{
	if (fVisible && fEnabled)
	{
		BRect r(fFrame);
		
		r.InsetBy(-1, -1);
	
		pushed = pushed || fDown;
	
		if (fMenu)
		{
			fBar->BeginLineArray(6);
			fBar->AddLine(r.LeftBottom(), r.RightBottom(), pushed ? kWhite : kDarkShadow);
			fBar->AddLine(r.RightTop(), r.RightBottom(), pushed ? kWhite : kDarkShadow);
			
			fBar->AddLine(r.LeftTop(), r.LeftBottom(), pushed ? kShadow : kWhite);
			fBar->AddLine(r.LeftTop(), r.RightTop(), pushed ? kShadow : kWhite);
			
			float x = r.left + 17;
			fBar->AddLine(BPoint(x, r.top), BPoint(x, r.bottom), kDarkShadow);
			x += 1;
			fBar->AddLine(BPoint(x, r.top), BPoint(x, r.bottom), kWhite);
			
			fBar->EndLineArray();
		}
		else
		{
			fBar->BeginLineArray(4);
			fBar->AddLine(r.LeftBottom(), r.RightBottom(), pushed ? kWhite : kDarkShadow);
			fBar->AddLine(r.RightTop(), r.RightBottom(), pushed ? kWhite : kDarkShadow);
			fBar->AddLine(r.LeftTop(), r.LeftBottom(), pushed ? kShadow : kWhite);
			fBar->AddLine(r.LeftTop(), r.RightTop(), pushed ? kShadow : kWhite);
			fBar->EndLineArray();
		}
	}
} /* HButton::MouseEnter */

void HButton::MouseLeave()
{
	if (fVisible && fEnabled)
	{
		BRect r(fFrame);
		
		r.InsetBy(-1, -1);
	
		if (fMenu)
		{
			fBar->BeginLineArray(6);
			fBar->AddLine(r.LeftBottom(), r.RightBottom(), fDown ? kWhite : kViewColor);
			fBar->AddLine(r.RightTop(), r.RightBottom(), fDown ? kWhite : kViewColor);
			fBar->AddLine(r.LeftTop(), r.LeftBottom(), fDown ? kDarkShadow : kViewColor);
			fBar->AddLine(r.LeftTop(), r.RightTop(), fDown ? kDarkShadow : kViewColor);
			
			float x = r.left + 17;
			fBar->AddLine(BPoint(x, r.top), BPoint(x, r.bottom), fDown ? kWhite : kViewColor);
			x += 1;
			fBar->AddLine(BPoint(x, r.top), BPoint(x, r.bottom), fDown ? kDarkShadow : kViewColor);
			
			fBar->EndLineArray();
		}
		else
		{
			fBar->BeginLineArray(4);
			fBar->AddLine(r.LeftBottom(), r.RightBottom(), fDown ? kWhite : kViewColor);
			fBar->AddLine(r.RightTop(), r.RightBottom(), fDown ? kWhite : kViewColor);
			fBar->AddLine(r.LeftTop(), r.LeftBottom(), fDown ? kDarkShadow : kViewColor);
			fBar->AddLine(r.LeftTop(), r.RightTop(), fDown ? kDarkShadow : kViewColor);
			fBar->EndLineArray();
		}
	}
} /* HButton::MouseLeave */

#pragma mark -

HDualIconButton::HDualIconButton(HButtonBar *bar, int resID, int cmd, float x, int flags, const char *help)
	: HButton(bar, resID, cmd, x, flags, help)
{
//	ASSERT(resID > 0);

	fAltIcon = (unsigned char *)HResources::GetResource('MICN', resID + 1);
	FailNilRes(fAltIcon);
	
	fToggle = true;
} /* HDualIconButton::HDualIconButton */

void HDualIconButton::Draw(bool pushed)
{
	if (fOn)
		swap(fIcon, fAltIcon);

	HButton::Draw(pushed);

	if (fOn)
		swap(fIcon, fAltIcon);
} /* HDualIconButton::Draw */

void HDualIconButton::MouseEnter(bool pushed)
{
	bool b = fDown;
	fDown = false;
	
	HButton::MouseEnter(pushed);
	
	fDown = b;
} /* HDualIconButton::MouseEnter */

void HDualIconButton::MouseLeave()
{
	bool b = fDown;
	fDown = false;
	
	HButton::MouseLeave();
	
	fDown = b;
} /* HDualIconButton::MouseLeave */

#pragma mark -

HButtonBar::HButtonBar(BRect frame, const char *name, int resID, BHandler *target)
	: BView(frame, name, B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW | B_PULSE_NEEDED)
{
	fTarget = target;
	fLastButtonOver = -1;
	fLastEnter = 0;
	fLastDisplay = 0;
	fHelp = NULL;
	
	long bCnt, flags;
	const void *p;
	size_t size;
	p = HResources::GetResource('BtnB', resID, size);
	FailNilRes(p);
	
	BMemoryIO buf(p, size);
	
	buf >> flags >> bCnt;
	
	fDragger = (flags & (1 << bbDragger)) != 0;
	fAcceptFirstClick = (flags & (1 << bbAcceptFirstClick)) != 0;
	
	float x = fDragger ? 12 : 4;
	
	while (bCnt--)
	{
		long bID, cmd, fl;
		char help[256];
			
		buf >> bID >> cmd >> fl >> help;
		
		if (fl & (1 << bfSpace))
			x += 10;
		else
		{
			if (fl & (1 << bfDualIcon))
				fButtons.push_back(new HDualIconButton(this, bID, cmd, x, fl, help));
			else
				fButtons.push_back(new HButton(this, bID, cmd, x, fl, help));
	
			x += 20;
			if (fl & (1 << bfMenu)) x += 10;
		}
	}
	
	ResizeTo(x, frame.Height());
} /* HButtonBar::HButtonBar */

HButtonBar::~HButtonBar()
{
	if (fHelp)
		HideHelp();
} /* HButtonBar::~HButtonBar() */

void HButtonBar::Draw(BRect update)
{
	BRect bounds(Bounds());
	
	SetHighColor(kShadow);
	StrokeLine(bounds.LeftBottom(), bounds.RightBottom());
	StrokeLine(bounds.RightTop(), bounds.RightBottom());
	
	SetHighColor(kWhite);
	StrokeLine(bounds.LeftTop(), bounds.RightTop());
	StrokeLine(bounds.LeftTop(), bounds.LeftBottom());

	SetHighColor(kViewColor);
	bounds.InsetBy(1, 1);
	FillRect(bounds);
	
	if (fDragger)
	{
		int x = 2, y;
		y = (int)bounds.Height() / 3;
	
		BPoint p1(3, 3), p2(4, 4);
		
		BeginLineArray(2 * x * y);
		while (true)
		{
			AddLine(p1, p1, kDarkShadow);
			AddLine(p2, p2, kWhite);
			p1.x += 3;
			p2.x += 3;
			AddLine(p1, p1, kDarkShadow);
			AddLine(p2, p2, kWhite);
			if (--y == 0) break;
			p1.x = 3; p1.y += 3;
			p2.x = 4; p2.y += 3;
		}
		EndLineArray();
	}
	
	for (vector<HButton*>::iterator i = fButtons.begin(); i != fButtons.end(); i++)
	{
		(*i)->Draw((*i)->IsDown());
		(*i)->MouseLeave();
	}
} /* HButtonBar::Draw */

void HButtonBar::MouseMoved(BPoint where, uint32 code, const BMessage *a_message)
{
	if (Bounds().Contains(where))
		be_app->SetCursor(B_HAND_CURSOR);
	
	if (fAcceptFirstClick || IsActive())
	{
		int btn = FindButton(where);
		
		if (btn != fLastButtonOver)
		{
			if (fHelp)
				HideHelp();

			if (fLastButtonOver >= 0)
				fButtons[fLastButtonOver]->MouseLeave();
			
			fLastButtonOver = btn;

			if (fLastButtonOver >= 0)
			{
				fButtons[fLastButtonOver]->MouseEnter();
				fLastEnter = system_time();
			}
			else
				fLastEnter = 0;
		}
	}
} /* HButtonBar::MouseMoved */

void HButtonBar::MouseDown(BPoint where)
{
	int btnID = FindButton(where);
	HButton *btn = btnID != -1 ? fButtons[btnID] : NULL;
	
	if (fHelp)
		HideHelp();

	if (btn && fTarget && btn->IsVisible() && btn->IsEnabled())
	{
		if (btn->IsMenu())
		{
			BMessage msg(btn->Cmd());

			BPoint p = btn->Frame().LeftBottom();
			p.y += 5;
			
			msg.AddPoint("where", ConvertToScreen(p));
			msg.AddBool("showalways", where.x > btn->Frame().right - 7);
			
			btn->SetDown(true);

			if (fTarget)
				fTarget->Looper()->PostMessage(&msg, fTarget);
			return;
		}
		
		unsigned long btns;
		bool in = false;
		
		do
		{
			GetMouse(&where, &btns);
			
			if (in != btn->Frame().Contains(where))
			{
				in = !in;
				
				btn->Draw(in);
				btn->MouseEnter(in);
			}
		}
		while (btns);
		
		if (in)
		{
			if (btn->IsToggle())
				btn->SetOn(!btn->IsOn());

			if (fTarget)
				fTarget->Looper()->PostMessage(btn->Cmd(), fTarget);
		}

		btn->Draw();
		if (in)
			btn->MouseEnter();
		else
		{
			btn->MouseLeave();
			fLastButtonOver = -1;
		}

		fLastEnter = 0;
	}
} /* HButtonBar::MouseDown */

int HButtonBar::FindButton(BPoint where)
{
	int btn = -1;

	for (vector<HButton*>::iterator i = fButtons.begin(); i != fButtons.end(); i++)
	{
		if ((*i)->Frame().Contains(where))
		{
			btn = i - fButtons.begin();
			break;
		}
	}
	
	return btn;
} /* HButtonBar::FindButton */

void HButtonBar::SetDown(int cmd, bool down)
{
	HButton *btn = NULL;
	
	for (vector<HButton*>::iterator i = fButtons.begin(); i != fButtons.end() && btn == NULL; i++)
	{
		if ((*i)->Cmd() == cmd)
			btn = *i;
	}
	
	if (btn && btn->IsDown() != down)
		btn->SetDown(down);
} /* HButtonBar::SetDown */

void HButtonBar::SetOn(int cmd, bool on)
{
	HButton *btn = NULL;
	
	for (vector<HButton*>::iterator i = fButtons.begin(); i != fButtons.end() && btn == NULL; i++)
	{
		if ((*i)->Cmd() == cmd)
			btn = *i;
	}
	
	if (btn && btn->IsToggle())
		btn->SetOn(on);
} /* HButtonBar::SetOn */

void HButtonBar::SetEnabled(int cmd, bool enabled)
{
	HButton *btn = NULL;
	
	for (vector<HButton*>::iterator i = fButtons.begin(); i != fButtons.end() && btn == NULL; i++)
	{
		if ((*i)->Cmd() == cmd)
			btn = *i;
	}
	
	if (btn)
		btn->SetEnabled(enabled);
} /* HButtonBar::SetEnabled */

void HButtonBar::SetVisible(int cmd, bool visible)
{
	HButton *btn = NULL;
	
	for (vector<HButton*>::iterator i = fButtons.begin(); i != fButtons.end() && btn == NULL; i++)
	{
		if ((*i)->Cmd() == cmd)
			btn = *i;
	}
	
	if (btn)
		btn->SetVisible(visible);
} /* HButtonBar::SetVisible */

void HButtonBar::SetTarget(BHandler *target)
{
	fTarget = target;
} /* HButtonBar::SetTarget */

void HButtonBar::WindowActivated(bool active)
{
	if (!active && fLastButtonOver >= 0)
	{
		fButtons[fLastButtonOver]->MouseLeave();

		if (fHelp)
			HideHelp();
	}

	fLastButtonOver = -1;
} /* HButtonBar::WindowActivated */

void HButtonBar::ShowHelp()
{
	font_height fh;
	
	be_plain_font->GetHeight(&fh);
	
	if (fLastButtonOver >= 0)
	{
		HButton *btn = fButtons[fLastButtonOver];
		if (!btn || !btn->IsVisible())
			return;
		BRect r(btn->Frame());
		
		r.OffsetBy(30, 30);
		r.right = r.left + be_plain_font->StringWidth(btn->Help()) + 2;
		r.bottom = r.top + fh.ascent + fh.descent;
		
		ConvertToScreen(&r);
		
		fHelp = new HHelpWindow(r, btn->Help());
		fLastEnter = 0;
		fLastDisplay = system_time();
	}
} /* HButtonBar::ShowHelp */

void HButtonBar::HideHelp()
{
	if (fHelp && fHelp->Lock())
		fHelp->Quit();
	fHelp = NULL;
	fLastDisplay = 0;
} /* HButtonBar::HideHelp */

void HButtonBar::Pulse()
{
	if (fHelp && fLastDisplay > 0 && fLastDisplay + 1000000 < system_time())
		HideHelp();
	else if (fLastEnter > 0 && fLastEnter + 750000 < system_time() && fLastDisplay == 0)
		ShowHelp();
} /* HButtonBar::Pulse */

