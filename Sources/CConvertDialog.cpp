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
			BWindow *owner, BPositionIO* data)
	: HDialog(frame, name, type, flags, owner, data)
{
	PDoc *doc = dynamic_cast<PDoc*>(owner);
	FailNil(doc);
	
	BMenuField *mf = dynamic_cast<BMenuField*>(FindView("src"));
	FailNil(mf);
	fFrom = mf->Menu();
	
	mf = dynamic_cast<BMenuField*>(FindView("dest"));
	FailNil(mf);
	fTo = mf->Menu();
	
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
	
	SetEnabled("ok", false);

	Show();
} /* CConvertDialog::CConvertDialog */
		
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
	bool enabled = fFrom->IndexOf(fFrom->FindMarked()) != fTo->IndexOf(fTo->FindMarked());
	SetEnabled("ok", enabled);
} /* CConvertDialog::UpdateFields */
