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

	Created: 11/14/98 13:51:16
*/

#include "pe.h"
#include "CFtpListItem.h"
#include "CFtpDialog.h"
#include "HDefines.h"
#include "HError.h"
#include "HColorUtils.h"
#include <parsedate.h>

CFtpListItem::CFtpListItem(CFtpDialog* dialog, const char *s)
: fDirectory(false), fValid(true), fDotFile(false)
{
	char name[NAME_MAX], date[64];
	int d, n;

// assume: flags nr? owner group size month mday time name
	if (sscanf(s + 10, "%*d%*s%*s%d%n%*s%*d%*d:%*d%n%s", &fSize, &d, &n, name) == 2)
	{
		fName = strdup(s + n + 11);
		
		memset(date, 0, 64);
		strncpy(date, s + d + 11, n - d);
		fMod = parsedate(date, -1);
		
		char *t = strchr(fName, '\r');
		if (t == NULL) t = strchr(fName, '\n');
		if (t) *t = 0;
	}
// else assume: flags nr? owner group size month mday year name
	else if (sscanf(s + 10, "%*d%*s%*s%d%n%*s%*d%*d%n%s", &fSize, &d, &n, name) == 2)
	{
		fName = strdup(s + n + 11);
		
		memset(date, 0, 64);
		strncpy(date, s + d + 11, n - d);
		fMod = parsedate(date, -1);

		char *t = strchr(fName, '\r');
		if (t == NULL) t = strchr(fName, '\n');
		if (t) *t = 0;
	}
	else
		fName = strdup(s);

	fDotFile = (*fName == '.');

	// Get the Icon
	switch (s[0]) {
		case 'd':
			fDirectory = true;
			fIconNew = dialog->GetIcon("application/x-vnd.Be-directory");
			break;
		case 'l':
			fIconNew = dialog->GetIcon("application/x-vnd.Be-symlink");
			break;
		default:
			string nam = fName;
			string ext = "";
			string::size_type pos = nam.rfind(".", nam.length());
			if (pos != string::npos)
			{
				ext = nam.substr(pos+1);
//				cout << "Extension(" << nam << ") .: '" << ext << "'" << endl;
			}
			fIconNew = dialog->GetIcon("application/octet-stream", ext.c_str());
	}

//	char name[NAME_MAX];
//	
//	if (sscanf(s + 10, "%*d%*s%*s%d%*s%*d%*d:%*d%s", &fSize, name) == 2)
//	{
//		
//		fName = strdup(name);
//	}
//	else
//		fName = strdup(s);
} // CFtpListItem::CFtpListItem

CFtpListItem::~CFtpListItem()
{
	free(fName);
} // CFtpListItem::~CFtpListItem

void CFtpListItem::DrawItem(BView *owner, BRect bounds, bool /*complete*/)
{
	try
	{
		owner->SetLowColor(IsSelected() ? tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT) : kWhite);
		BRect r(bounds);
		owner->FillRect(r, B_SOLID_LOW);
		r.InsetBy(1, 1);
	
		font_height fh;
		be_plain_font->GetHeight(&fh);
		
		owner->SetDrawingMode(B_OP_OVER);
		owner->DrawBitmap(fIconNew, BPoint(r.left + 2, r.top));
		owner->SetDrawingMode(B_OP_COPY);
		
		owner->DrawString(fName, BPoint(r.left + 22, r.bottom - fh.descent));
		owner->SetLowColor(kWhite);
	}
	catch (HErr& e)
	{
		BListView *lv = dynamic_cast<BListView*>(owner);
		if (lv) lv->RemoveItem(this);
		// e.DoError();
		beep();
	}
} // CFtpListItem::DrawItem

void CFtpListItem::Update(BView */*owner*/, const BFont */*font*/)
{
	font_height fh;
	be_plain_font->GetHeight(&fh);
	SetHeight(std::max((float)18.0, 4 + fh.descent + fh.ascent));
} // CFtpListItem::Update
