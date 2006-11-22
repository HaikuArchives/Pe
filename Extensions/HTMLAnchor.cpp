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
#include "HTMLAnchor.dlog.r.h"

#if __INTEL__
extern "C" _EXPORT long perform_edit(MTextAddOn *addon);
#else
#pragma export on
extern "C" {
long perform_edit(MTextAddOn *addon);
}
#pragma export reset
#endif

long Anchor(MTextAddOn *addon);
int SkipGIFDataBlock(BPositionIO& data);
void GetGIFSize(entry_ref& ref, short& width, short& height);
char *RelativePath(entry_ref& a, entry_ref& b);

static bool sOK = false;
static char *sPath;
static MTextAddOn *sAddon;

const unsigned long
	kWindowWidth = 250,
	kWindowHeight = 125,
	kMsgOK = 'ok  ',
	kMsgCancel = 'cncl';

const rgb_color
	kViewColor = { 0xDB, 0xDB, 0xDB, 0 };

class CAnchorDialog : public HDialog {
public:
		enum { sResID = 1 };

		CAnchorDialog(BRect frame, const char *name, window_type type, int flags,
			BWindow *owner, BPositionIO* data);

virtual bool OkClicked();
virtual bool CancelClicked();
virtual void MessageReceived(BMessage *msg);
};

CAnchorDialog::CAnchorDialog(BRect frame, const char *name, window_type type, int flags,
			BWindow *owner, BPositionIO* data)
	: HDialog(frame, name, type, flags, owner, data)
{
	FindView("path")->MakeFocus(true);
} /* CAnchorDialog::CAnchorDialog */

bool CAnchorDialog::OkClicked()
{
	sOK = true;
	sPath = strdup(GetText("path"));

	return true;
} /* CAnchorDialog::OkClicked */

bool CAnchorDialog::CancelClicked()
{
	sOK = false;
	return true;
} /* CAnchorDialog::CancelClicked */

void CAnchorDialog::MessageReceived(BMessage *msg)
{
	if (msg->WasDropped() && msg->HasRef("refs"))
	{
		try
		{
			entry_ref ref, doc;

			FailOSErr(msg->FindRef("refs", &ref));

			if (sAddon->GetRef(doc) == B_OK)
			{
				char *sp = RelativePath(doc, ref);
				SetText("path", sp);
				free(sp);
			}
			else
				throw HErr("Save the HTML document first!");
		}
		catch (HErr& err)
		{
			err.DoError();
		}
	}
	else
		HDialog::MessageReceived(msg);
} /* CAnchorDialog::MessageReceived */

long perform_edit(MTextAddOn *addon)
{
	long result = B_NO_ERROR;

	try
	{
		sAddon = addon;

		BMemoryIO tmpl(kDLOG1, kDLOG1Size);
		CAnchorDialog *p = DialogCreator<CAnchorDialog>::CreateDialog(addon->Window(), tmpl);
		p->Show();

		long l;
		wait_for_thread(p->Thread(), &l);

		if (sOK)
		{
			result = Anchor(addon);
			free(sPath);
		}
	}
	catch (HErr& e)
	{
		e.DoError();
		result = B_ERROR;
	}

	return result;
} /* perform_edit */

long Anchor(MTextAddOn *addon)
{
	char s[2048];

	long st, end, len;
	addon->GetSelection(&st, &end);
	len = end - st;

	if (*sPath)
		sprintf(s, "<a href=\"%s\">", sPath);
	else
		strcpy(s, "<a href=\"\">");

	addon->Select(st, st);
	addon->Insert(s);

	end += strlen(s);
	addon->Select(end, end);
	addon->Insert("</a>");
	return B_OK;
} /* Anchor */

char *RelativePath(entry_ref& a, entry_ref& b)
{
	BEntry e;

	BPath pa, pb;

	FailOSErr(e.SetTo(&a));
	FailOSErr(e.GetPath(&pa));

	FailOSErr(e.SetTo(&b));
	FailOSErr(e.GetPath(&pb));

	char result[PATH_MAX];
	const char *s, *sa, *sb;

	s = sa = pa.Path();
	sb = pb.Path();

	int ma, mb;

	ma = strrchr(sa, '/') - sa;
	mb = strrchr(sb, '/') - sb;

	while (*sa == *sb)
	{
		sa++;
		sb++;
	}

	if (sa > s && (sa[-1] != '/' || sb[-1] != '/'))
	{
		while (sa > s && sa[-1] != '/')
		{
			sa--;
			sb--;
		}
	}

	if (strchr(sa, '/') == NULL)
	{
		strcpy(result, "./");
		strcat(result, sb);
	}
	else
	{
		result[0] = 0;
		char *t;

		while ((t = strchr(sa, '/')) != NULL)
		{
			strcat(result, "../");
			sa = t + 1;
		}

		strcat(result, sb);
	}

	return strdup(result);
} /* RelativePath */
