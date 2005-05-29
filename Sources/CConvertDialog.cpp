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

	Created: 10/28/97 09:19:58
*/


#include "pe.h"
#include "CConvertDialog.h"
#include "PMessages.h"
#include "PDoc.h"
#include "PText.h"
#include "PMessages.h"

CConvertDialog::CConvertDialog(BRect frame, const char *name, window_type type, int flags,
			BWindow *owner)
	: HDialog(frame, name, type, flags | B_NOT_RESIZABLE, owner)
{
	PDoc *doc = dynamic_cast<PDoc*>(owner);
	FailNil(doc);
	
	Create();
	Layout();

	fFrom = fSrc->Menu();
	fTo = fDest->Menu();
	
	int e = doc->TextView()->Encoding();
	if (e)
	{
		fFrom->ItemAt(e + 1)->SetMarked(true);
		fTo->ItemAt(e + 1)->SetMarked(true);
	}
	else
	{
		fFrom->ItemAt(0)->SetMarked(true);
		fTo->ItemAt(0)->SetMarked(true);
	}
	
	SetDefaultButton(fOkButton);
	fOkButton->SetEnabled(false);

	Show();
} /* CConvertDialog::CConvertDialog */
		
void CConvertDialog::Create(void)
{
	fMainView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	//
	fBox = new HBox(fMainView);

	BPopUpMenu* srcMenu = new BPopUpMenu("src-menu");
	SetupEncodingMenu(srcMenu);
	fSrc = new HMenuField(fBox, "src", srcMenu);
	
	BPopUpMenu* destMenu = new BPopUpMenu("dest-menu");
	SetupEncodingMenu(destMenu);
	fDest = new HMenuField(fBox, "dest", destMenu);

	// Buttons
	fCancelButton = new HButton(fMainView, "cncl", 'cncl', 
							    B_FOLLOW_RIGHT|B_FOLLOW_TOP);
	fOkButton = new HButton(fMainView, "ok", 'ok  ', 
						    B_FOLLOW_RIGHT|B_FOLLOW_TOP);
}

void CConvertDialog::Layout(void)
{
	fBox->SetLabel("Change the character encoding");
	fSrc->ResizeLocalized("From:");
	fDest->ResizeLocalized("To:");
	fCancelButton->ResizeLocalized("Cancel");
	fOkButton->ResizeLocalized("OK");

	float div = fSrc->StringWidth(fSrc->Label());
	div = max(div, fDest->StringWidth(fDest->Label()));
	fSrc->SetDivider(div+15);
	fDest->SetDivider(div+15);

	float dx = fMainView->StringWidth("m");
	float dy = fMainView->StringWidth("n");

	fSrc->MoveTo(dx, dy*4);
	fDest->MoveTo(dx, fSrc->Bottom()+dy);

	float bw = fCancelButton->Width() + fOkButton->Width() + 3*dx;
	bw = max(bw, fBox->StringWidth(fBox->Label()));
	bw = max(bw, fSrc->Right());
	bw = max(bw, fDest->Right());

	fSrc->MenuBar()->SetMaxContentWidth(bw-div-15-dx);
	fDest->MenuBar()->SetMaxContentWidth(bw-div-15-dx);

	fBox->MoveTo(dx, dy);
	fBox->ResizeTo(bw+2*dx, fDest->Bottom()+dy);
	
	fOkButton->MoveTo(fMainView->Right()-dx-fOkButton->Width(),
					  fBox->Bottom()+2*dy);
	fCancelButton->MoveTo(fOkButton->Left()-dx-fCancelButton->Width(),
						  fOkButton->Top());
	float minW = fBox->Width()+2*dx;
	float minH = fOkButton->Bottom()+2*dy;
	ResizeToLimits(minW, minW, minH, minH);
}

bool CConvertDialog::OKClicked()
{
	int f, t;
	f = fFrom->IndexOf(fFrom->FindMarked()) - 2;
	t = fTo->IndexOf(fTo->FindMarked()) - 2;
	
	BMessage m(msg_DoConvertEncoding);
	m.AddInt32("from", f);
	m.AddInt32("to", t);
	fOwner->PostMessage(&m, static_cast<PDoc*>(fOwner)->TextView());
	
	return true;
} /* CConvertDialog::OKClicked */

bool CConvertDialog::CancelClicked()
{
	return true;
} /* CConvertDialog::CancelClicked */

void CConvertDialog::UpdateFields()
{
	bool enabled 
		= fFrom->IndexOf(fFrom->FindMarked()) != fTo->IndexOf(fTo->FindMarked());
	fOkButton->SetEnabled(enabled);
} /* CConvertDialog::UpdateFields */

void CConvertDialog::SetupEncodingMenu(BMenu* menu)
{
	// [zooey, 2005]: 
	//		N.B.: The order of the following strings must match the definition
	//			  of the respective constants in UTF8.h.
	//			  This is lame, of course, but I suppose this dialog is used
	//			  rarely enough to keep this as is.
	const char* encodings[] = {
		"UTF-8",
		"-",
		"ISO 8859-1",
		"ISO 8859-2",
		"ISO 8859-3",
		"ISO 8859-4",
		"ISO 8859-5",
		"ISO 8859-6",
		"ISO 8859-7",
		"ISO 8859-8",
		"ISO 8859-9",
		"ISO 8859-10",
		"Macintosh Roman",
		"Shift-JIS",
		"EUC Packed Japanese",
		"JIS",
		"Windows 1252",
		"Unicode",
		"KOI8-R",
		"Windows 1251",
		"DOS 866",
		"DOS 437",
		"EUC Korean",
		"ISO 8859-13",
		"ISO 8859-14",
		"ISO 8859-15",
		NULL
	};
	for(int e = 0; encodings[e] != NULL; ++e)
	{
		if (*encodings[e] == '-')
			menu->AddSeparatorItem();
		else
			menu->AddItem(new BMenuItem(encodings[e], new BMessage('chng')));
	}
}
