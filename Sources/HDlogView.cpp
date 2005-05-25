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

	Created: 11/27/97 22:08:39 by Maarten Hekkelman
*/

#include "pe.h"
#include "HDlogView.h"
#include "HDefines.h"

HDlogView::HDlogView(BRect frame, const char *name)
	: BView(frame, name, B_FOLLOW_ALL_SIDES, B_WILL_DRAW)
{
	SetViewColor(kViewColor);
} /* HDlogView::HDlogView */
		
HDlogView::~HDlogView()
{
} /* HDlogView::~HDlogView */

void HDlogView::Draw(BRect update)
{
	BView::Draw(update);

	if (fLines.size() > 0)
	{
		vector<BRect>::iterator ri;
		
		BeginLineArray(fLines.size() * 2);
		for (ri = fLines.begin(); ri != fLines.end(); ri++)
		{
			BRect r = *ri;
			if (r.Width() > r.Height())
			{
				AddLine(r.LeftTop(), r.RightTop(), kShadow);
				AddLine(r.LeftBottom(), r.RightBottom(), kWhite);
			}
			else
			{
				AddLine(r.LeftTop(), r.LeftBottom(), kShadow);
				AddLine(r.RightTop(), r.RightBottom(), kWhite);
			}
		}
		EndLineArray();
	}
} /* HDlogView::Draw */

void HDlogView::AddMyLine(BRect r)
{
	fLines.push_back(r);
} /* HDlogView::AddMyLine */


HButton::HButton(BView* view, const char* name, uint32 cmd, uint32 resizingMode)
: BButton(BRect(0, 0, 100, 50), name, "", new BMessage(cmd), resizingMode)
{
	view->AddChild(this);
} /* HButton::HButton */

void HButton::ResizeLocalized(const char* label)
{
	SetLabel(label);
	ResizeToPreferred();
} /* HButton::ResizeLocalized */


HStringView::HStringView(BView* view, uint32 resizingMode)
: BStringView(BRect(0, 0, 100, 50), "", "", resizingMode)
{
	view->AddChild(this);
} /* HStringView::HStringView */

void HStringView::ResizeLocalized(const char* label)
{
	SetText(label);
	ResizeToPreferred();
} /* HStringView::ResizeLocalized */


HTextControl::HTextControl(BView* view, const char* name, uint32 resizingMode)
: BTextControl(BRect(0, 0, 100, 50), name, NULL, "", NULL, resizingMode)
{
	view->AddChild(this);
} /* HTextControl::HTextControl */

void HTextControl::ResizeLocalized(const char* label)
{
	SetLabel(label);
	ResizeToPreferred();
} /* HTextControl::ResizeLocalized */


HMenuField::HMenuField(BView* view, const char* name, uint32 resizingMode)
: BMenuField(BRect(0, 0, 100, 50), name, NULL, new BMenu(""), resizingMode)
{
	view->AddChild(this);
} /* HMenuField::HMenuField */

void HMenuField::ResizeLocalized(const char* label)
{
	ResizeToPreferred();
} /* HMenuField::ResizeLocalized */


HCheckBox::HCheckBox(BView* view, const char* name, BMessage* msg, 
					 uint32 resizingMode)
: BCheckBox(BRect(0, 0, 100, 50), name, "", NULL, resizingMode)
{
	view->AddChild(this);
} /* HCheckBox::HCheckBox */

void HCheckBox::ResizeLocalized(const char* label)
{
	SetLabel(label);
	ResizeToPreferred();
} /* HCheckBox::ResizeLocalized */


HScrollView::HScrollView(BView* view, const char *name, BView *target,
				uint32 resizingMode = B_FOLLOW_LEFT|B_FOLLOW_TOP,
				uint32 flags = 0,
				bool horizontal = false,
				bool vertical = false,
				border_style border = B_FANCY_BORDER)
: BScrollView(name, target, resizingMode, flags, horizontal, vertical, border)
{
	view->AddChild(this);
} /* HScrollView::HScrollView */

