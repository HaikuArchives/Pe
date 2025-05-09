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

#include "PText.h"
#include "PStatus.h"
#include "Utils.h"
#include "PApp.h"
#include "CDoc.h"
#include "PDoc.h"
#include "PMessages.h"
#include "HError.h"
#include "HDefines.h"

#include <ControlLook.h>
#include <tracker_private.h>
#include <DirMenu.h>

PStatus::PStatus(BRect frame, PText *txt)
	: BView(frame, "status", B_FOLLOW_BOTTOM | B_FOLLOW_LEFT, B_WILL_DRAW)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	BFont font(be_plain_font);
	font.SetSize(ceilf(font.Size() * 0.75));
	SetFont(&font);

	font_height fontHeight;
	font.GetHeight(&fontHeight);

	fBaseline = (Bounds().bottom + Bounds().top
		+ ceilf(fontHeight.ascent) - ceilf(fontHeight.descent)) / 2;

	fText = txt;
	fOffset = 0;
	fPath = NULL;
} /* PStatus::PStatus */

PStatus::~PStatus()
{
	if (fPath) free(fPath);
} /* PStatus::~PStatus */

void PStatus::Draw(BRect updateRect)
{
	BRect bounds(Bounds());

	SetHighColor(tint_color(ViewColor(), B_DARKEN_2_TINT));
	StrokeRect(bounds);
	bounds.InsetBy(1, 1);

	be_control_look->DrawMenuBarBackground(this,
		bounds, updateRect,	ViewColor());

	SetHighColor(kBlack);
	MovePenTo(be_control_look->DefaultItemSpacing() * 0.25, fBaseline);
	char s[32];
	int32 line = fText->Offset2Line(fOffset);
	sprintf(s, "%" B_PRId32 ",%" B_PRId32, line + 1, fText->Offset2Column(fOffset) + 1);
	DrawString(s);
} /* PStatus::Draw */

void PStatus::SetOffset(int newOffset)
{
	fOffset = newOffset;
	Draw(Bounds());
} /* PStatus::SetOffset */

void PStatus::SetPath(const char *path)
{
	if (fPath) free(fPath);
	fPath = strdup(path);
} /* PStatus::SetPath */

void PStatus::MouseDown(BPoint where)
{
	bigtime_t longEnough = system_time() + 250000;

	do
	{
		BPoint p;
		uint32 btns;

		GetMouse(&p, &btns);

		if (!btns)
		{
			Window()->PostMessage(msg_GoToLine, fText);
			return;
		}

		if (fabs(where.x - p.x) > 2 || fabs(where.y - p.y) > 2)
			break;
	}
	while (system_time() < longEnough);

	if (fPath)
	{
		BEntry entry;
		status_t status = entry.SetTo(fPath);

		if (status != B_OK || !entry.Exists())
			return;

		BPrivate::BDirMenu* menu = new BDirMenu(NULL,
			BMessenger(kTrackerSignature), B_REFS_RECEIVED);

		menu->Populate(&entry, Window(), false, false, true, false, true);

		BPoint point = Bounds().LeftBottom();
		point.y += 3;
		ConvertToScreen(&point);
		BRect clickToOpenRect(Bounds());
		ConvertToScreen(&clickToOpenRect);
		menu->Go(point, true, true, clickToOpenRect);
		delete menu;
	}
	else
		beep();
} /* PStatus::MouseDown */
