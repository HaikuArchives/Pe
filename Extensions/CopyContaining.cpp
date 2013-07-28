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

	Created: 09/15/97 02:33:13
*/

#include "PeAddOn.h"
#include <pcreposix.h>
#define REG_NOERROR 0
#include "copycon.r.h"

#if __INTEL__
extern "C" _EXPORT status_t perform_edit(MTextAddOn *addon);
#else
#pragma export on
extern "C" {
status_t perform_edit(MTextAddOn *addon);
}
#pragma export reset
#endif

status_t CopyContaining(MTextAddOn *addon);
status_t SuffixLines(MTextAddOn *addon);

static bool sOK = false;
static bool sSelectionOnly;
static char *sText = NULL;

const unsigned long
	kWindowWidth = 250,
	kWindowHeight = 125,
	kMsgOK = 'ok  ',
	kMsgCancel = 'cncl';

const rgb_color
	kViewColor = { 0xDB, 0xDB, 0xDB, 0 };

class CCopyContainingDialog : public HDialog {
public:
		enum { sResID = 1 };

		CCopyContainingDialog(BRect frame, const char *name, window_type type, int flags,
			BWindow *owner, BPositionIO* data);

virtual bool OkClicked();
virtual bool CancelClicked();
};

CCopyContainingDialog::CCopyContainingDialog(BRect frame, const char *name, window_type type, int flags,
			BWindow *owner, BPositionIO* data)
	: HDialog(frame, name, type, flags, owner, data)
{
	SetOn("sel", sSelectionOnly);
	FindView("text")->MakeFocus(true);
} /* CCopyContainingDialog::CCopyContainingDialog */

bool CCopyContainingDialog::OkClicked()
{
	sOK = true;
	sText = strdup(GetText("text"));
	sSelectionOnly = IsOn("sel");
	return true;
} /* CCopyContainingDialog::OkClicked */

bool CCopyContainingDialog::CancelClicked()
{
	sOK = false;
	return true;
} /* CCopyContainingDialog::CancelClicked */

status_t perform_edit(MTextAddOn *addon)
{
	status_t result = B_NO_ERROR;

	try
	{
		int32 s, e, l;
		addon->GetSelection(&s, &e);
		sSelectionOnly = (s != e);

		BMemoryIO tmpl(kDLOG1, kDLOG1Size);
		CCopyContainingDialog *p = DialogCreator<CCopyContainingDialog>::CreateDialog(addon->Window(), tmpl);
		p->Show();

		wait_for_thread(p->Thread(), &l);
		if (sOK)
			result = CopyContaining(addon);

		if (sText)
			free(sText);
	}
	catch (HErr& e)
	{
		e.DoError();
		result = B_ERROR;
	}

	return result;
} /* perform_edit */

status_t CopyContaining(MTextAddOn *addon)
{
	//  error checking

	int32 selBeg;
	int32 selEnd;

	if (sSelectionOnly)
	{
		addon->GetSelection(&selBeg, &selEnd);
		if (selEnd <= selBeg)
			return B_ERROR;
	}
	else
	{
		selBeg = 0;
		selEnd = addon->TextLength();
	}

	//  set-up

	regex_t pb;
	memset(&pb, 0, sizeof(pb));
	int r = regcomp(&pb, sText, REG_EXTENDED);

	if (r == REG_NOERROR)
	{
		BString clip;
		BString line;
		const char *txt = addon->Text()+selBeg;
		int size = selEnd - selBeg;
		int beg = 0;
		int pos = -1;
		int len;

		while (++pos <= size)
		{
			if ((pos == size || txt[pos] == '\n'))
			{
				if ((len = pos-beg) > 0) {
#ifdef __HAIKU__
					// I assume on Haiku it's fast enough
					line.SetTo(txt+beg, len);
#else
					// Fast workaround for slow BString.SetTo() on BeOS
					char *buf;
					FailNil(buf = line.LockBuffer(len+1));
					memcpy(buf, txt+beg, len);
					buf[len] = '\0';
					line.UnlockBuffer(len);
#endif
					if (regexec(&pb, line.String(), 0, NULL, 0) == REG_NOERROR)
					{
						clip << line << '\n';
					}
				}
				beg = pos + 1;
			}
		}

		if (be_clipboard->Lock())
		{
			be_clipboard->Clear();
			be_clipboard->Data()->AddData("text/plain", B_MIME_DATA, clip.String(), clip.Length());
			be_clipboard->Commit();
			be_clipboard->Unlock();
		}
	}
	else
	{
		char err[100];

		regerror(r, &pb, err, 100);

		MAlert *a = new MWarningAlert(err);
		a->Go();
	}

	regfree(&pb);

	return B_NO_ERROR;
} /* CopyContaining */
