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
#include "HDefines.h"
#include "HError.h"
#include "HColorUtils.h"
#include <parsedate.h>

CFtpListItem::CFtpListItem(const char *s)
{
	BBitmap bm(BRect(0, 0, 15, 15), B_COLOR_8_BIT);
	
	if (s[0] == 'd')
	{
		fDirectory = true;
		BMimeType("application/x-vnd.Be-directory").GetIcon(&bm, B_MINI_ICON);
	}
	else if (s[0] == 'l')
	{
		fDirectory = false;
		BMimeType("application/x-vnd.Be-symlink").GetIcon(&bm, B_MINI_ICON);
	}
	else
	{
		fDirectory = false;
		BMimeType("application/octet-stream").GetIcon(&bm, B_MINI_ICON);
	}
	
	memcpy(fIcon, bm.Bits(), 256);
	
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
		BBitmap bm(BRect(0, 0, 15, 15), B_COLOR_8_BIT);
	
		if (IsSelected())
		{
			unsigned char icon[256];
			for (int i = 0; i < 256; i++)
				icon[i] = gSelectedMap[fIcon[i]];
			bm.SetBits(icon, 256, 0, B_COLOR_8_BIT);
			owner->SetLowColor(kShadow);
		}
		else
		{
			bm.SetBits(fIcon, 256, 0, B_COLOR_8_BIT);
			owner->SetLowColor(kWhite);
		}
	
		BRect r(bounds);
		owner->FillRect(r, B_SOLID_LOW);
		r.InsetBy(1, 1);
	
		font_height fh;
		be_plain_font->GetHeight(&fh);
	
		owner->SetDrawingMode(B_OP_OVER);
		owner->DrawBitmap(&bm, BPoint(r.left + 2, r.top));
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
