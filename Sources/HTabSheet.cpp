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

#include "pe.h"
#include "HTabSheet.h"
#include "HDialog.h"
#include "HDlogView.h"
#include "HDefines.h"

const int
	kListWidth = 80;

HTabSheet::HTabSheet(BRect frame, const char *name)
	: BView(frame, name, B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_NAVIGABLE)
{
	SetViewColor(kViewColor);
	SetLowColor(kViewColor);
	SetFont(be_plain_font);
	
	fListArea = frame;
	float f = 10 * gFactor;
	
	fListArea.InsetBy(f, f);
	fListArea.OffsetTo(f, f);
	fListArea.right = fListArea.left + kListWidth * gFactor;
	
	fEntries = new BListView(fListArea, "entries");
	fEntries->SetSelectionMessage(new BMessage(msg_Flip));
	AddChild(new BScrollView("scroller", fEntries));
	
//	fListArea.right += B_V_SHROLL_BAR_WIDTH;

	font_height fh;
	be_plain_font->GetHeight(&fh);
	
	fClientArea.Set(fListArea.right + f, ceil(f + 4 + fh.ascent + fh.descent + 8 * gFactor),
		frame.right - f, frame.bottom - 4 * f);
	
	fCurrent = 0;
} /* HTabSheet::HTabSheet */

HTabSheet::~HTabSheet()
{
	for (int i = 0; i < fDescs.CountItems(); i++)
		free((char *)fDescs.ItemAt(i));
} /* HTabSheet::~HTabSheet */

void HTabSheet::AttachedToWindow()
{
	fEntries->SetTarget(this);
} /* HTabSheet::AttachedToWindow */

void HTabSheet::MouseDown(BPoint where)
{
	BView::MouseDown(where);
} /* HTabSheet::MouseDown */

void HTabSheet::Draw(BRect update)
{
	BRect f;
	BPoint p;

	font_height fh;
	be_plain_font->GetHeight(&fh);

	f = fClientArea;
	f.top = fListArea.top;
	f.bottom = ceil(fClientArea.top - 8 * gFactor);

	f.InsetBy(2, 2);
	
	SetLowColor(0x77, 0xdd, 0xdd);
	FillRect(f, B_SOLID_LOW);

	p.x = f.left + 4;
	p.y = f.bottom - fh.descent;
	DrawString((char *)fDescs.ItemAt(fCurrent), p);
	SetLowColor(ViewColor());

	BeginLineArray(8);

	f.InsetBy(-2, -2);
	AddLine(f.LeftTop(), f.RightTop(), kShadow);
	AddLine(f.LeftTop(), f.LeftBottom(), kShadow);
	AddLine(f.RightTop(), f.RightBottom(), kWhite);
	AddLine(f.LeftBottom(), f.RightBottom(), kWhite);
	
	f.InsetBy(1, 1);
	AddLine(f.LeftTop(), f.RightTop(), kDarkShadow);
	AddLine(f.LeftTop(), f.LeftBottom(), kDarkShadow);
	AddLine(f.RightTop(), f.RightBottom(), kShadow);
	AddLine(f.LeftBottom(), f.RightBottom(), kShadow);
	
	EndLineArray();
} /* HTabSheet::Draw */

BRect HTabSheet::ClientArea()
{
	return fClientArea;
} /* HTabSheet::ClientArea */

BView* HTabSheet::AddSheet(const char *name, const char *desc)
{
	BView *r = new HDlogView(ClientArea(), name);
	AddChild(r);
	fPanes.AddItem(r);
	fEntries->AddItem(new BStringItem(name));
	fDescs.AddItem(desc ? strdup(desc) : strdup(name));

	if (fPanes.CountItems() > 1)
		r->Hide();
	else
		fEntries->Select(0);

	return r;
} /* HTabSheet::AddSheet */

void HTabSheet::MessageReceived(BMessage *msg)
{
	if (msg->what == msg_Flip)
	{
		int newCurrent = fEntries->CurrentSelection();
		
		if (newCurrent < 0 || newCurrent > fEntries->CountItems() - 1)
			FlipTo(fCurrent);
		else if (newCurrent != fCurrent)
		{
			static_cast<BView*>(fPanes.ItemAt(fCurrent))->Hide();
			fCurrent = newCurrent;
			static_cast<BView*>(fPanes.ItemAt(fCurrent))->Show();
			Draw(Bounds());
		}
	}
	
	BView::MessageReceived(msg);
} /* HTabSheet::MessageReceived */

void HTabSheet::FlipTo(int page)
{
	fEntries->Select(page);
} /* HTabSheet::FlipTo */
