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
#include "HTMLImage.dlog.r.h"

#if __INTEL__
extern "C" _EXPORT long perform_edit(MTextAddOn *addon);
#else
#pragma export on
extern "C" {
long perform_edit(MTextAddOn *addon);
}
#pragma export reset
#endif

long Image(MTextAddOn *addon);
int SkipGIFDataBlock(BPositionIO& data);
void GetGIFSize(entry_ref& ref, short& width, short& height);
char *RelativePath(entry_ref& a, entry_ref& b);

static bool sOK = false;
static char *sPath, *sMap, *sAlt, *sWidth, *sHeight, *sBorder, *sAlign;
static MTextAddOn *sAddon;

const unsigned long
	kWindowWidth = 250,
	kWindowHeight = 125,
	kMsgOK = 'ok  ',
	kMsgCancel = 'cncl';

const rgb_color
	kViewColor = { 0xDB, 0xDB, 0xDB, 0 };

class CImageDialog : public HDialog {
public:
		enum { sResID = 1 };
		
		CImageDialog(BRect frame, const char *name, window_type type, int flags,
			BWindow *owner, BPositionIO& data);

virtual bool OKClicked();
virtual bool CancelClicked();
virtual void MessageReceived(BMessage *msg);
};

CImageDialog::CImageDialog(BRect frame, const char *name, window_type type, int flags,
			BWindow *owner, BPositionIO& data)
	: HDialog(frame, name, type, flags, owner, data)
{
	FindView("path")->MakeFocus(true);
} /* CImageDialog::CImageDialog */

bool CImageDialog::OKClicked()
{
	sOK = true;
	sPath = strdup(GetText("path"));
	sMap = strdup(GetText("map"));
	sAlt = strdup(GetText("alt"));
	sWidth = strdup(GetText("width"));
	sHeight = strdup(GetText("height"));
	sBorder = strdup(GetText("border"));
	sAlign = strdup(GetText("align"));

	return true;
} /* CImageDialog::OKClicked */

bool CImageDialog::CancelClicked()
{
	sOK = false;
	return true;
} /* CImageDialog::CancelClicked */

void CImageDialog::MessageReceived(BMessage *msg)
{
	if (msg->WasDropped() && msg->HasRef("refs"))
	{
		try
		{
			short width, height;
			
			entry_ref ref;
			
			FailOSErr(msg->FindRef("refs", &ref));
			
			try
			{
				GetGIFSize(ref, width, height);

				char s[32];
				
				sprintf(s, "%d", width);
				SetText("width", s);
				
				sprintf(s, "%d", height);
				SetText("height", s);
			}
			catch (HErr& e)
			{
				e.DoError();
			}
			
			entry_ref doc;
			sAddon->GetRef(doc);
			
			char *sp = RelativePath(doc, ref);
			
			SetText("path", sp);
			
			char *np = strrchr(sp, '/');
			
			if (np == NULL) np = sp;
			else np++;
			
			int l = max_c(strlen(np) - 4, 0);
			if (strcasecmp(np + l, ".gif") == 0)
				np[l] = 0;
			
			SetText("alt", np);
			
			free(sp);
		}
		catch (HErr& err)
		{
			err.DoError();
		}
	}
	else
		HDialog::MessageReceived(msg);
} /* CImageDialog::MessageReceived */

long perform_edit(MTextAddOn *addon)
{
	long result = B_NO_ERROR;
	
	try
	{
		sAddon = addon;
		
		BMemoryIO tmpl(kDLOG1, kDLOG1Size);
		CImageDialog *p = DialogCreator<CImageDialog>::CreateDialog(addon->Window(), tmpl);
		p->Show();
		
		long l;
		wait_for_thread(p->Thread(), &l);

		if (sOK)
			result = Image(addon);
	}
	catch (HErr& e)
	{
		e.DoError();
		result = B_ERROR;
	}
	
	return result;
} /* perform_edit */

long Image(MTextAddOn *addon)
{
	char s[2048], *sp;
	
	strcpy(s, "<img");
	sp = s + strlen(s);
	
	if (*sPath)
	{
		sprintf(sp, " src=\"%s\"", sPath);
		sp += strlen(sp);
	}
	
	if (*sWidth)
	{
		sprintf(sp, " width=%s", sWidth);
		sp += strlen(sp);
	}
	
	if (*sHeight)
	{
		sprintf(sp, " height=%s", sHeight);
		sp += strlen(sp);
	}
	
	if (*sBorder)
	{
		sprintf(sp, " border=%s", sBorder);
		sp += strlen(sp);
	}
	
	if (*sAlign)
	{
		sprintf(sp, " align=%s", sAlign);
		sp += strlen(sp);
	}

	if (*sAlt)
	{
		sprintf(sp, " alt=\"%s\"", sAlt);
		sp += strlen(sp);
	}
	
	if (*sMap)
	{
		sprintf(sp, " ismap map=%s", sMap);
		sp += strlen(sp);
	}
	
	strcat(sp, ">");
	
	long st, end;
	addon->GetSelection(&st, &end);
	addon->Insert(s);
	st += strlen(s);
	addon->Select(st, st);

	return 0;
} /* Image */

void GetGIFSize(entry_ref& ref, short& width, short& height)
{
	BFile file;
	BPositionIO& data = file;
	
	FailOSErr(file.SetTo(&ref, B_READ_ONLY));
	
	char buf[256];
	
	CheckedRead(file, buf, 3);
	buf[3] = 0;

	if (strcmp(buf, "GIF"))
		throw HErr("Not a gif file");
	
	CheckedRead(file, buf, 3);
	
	if (strcmp(buf, "87a") && strcmp(buf, "89a"))
		throw HErr("Not a known gif file");

	CheckedRead(file, buf, 7);
	
	int bitPixel = 2 << (buf[4] & 0x07);
	file.Seek(bitPixel * 3, SEEK_CUR);

	while (true)
	{
		unsigned char c;
		
		data >> c;
		
		if (c == ';')
		{
			// huh?
			throw HErr("Not a valid GIF file");
		}
		
		if (c == '!')
		{
			data >> c;
			while (SkipGIFDataBlock(file))
				;
			continue;
		}
		
		if (c != ',')
			continue;
		
		CheckedRead(file, buf, 9);
		
		width = B_LENDIAN_TO_HOST_INT16(*(short *)(buf + 4));
		height = B_LENDIAN_TO_HOST_INT16(*(short *)(buf + 6));
		return;
	}
} /* GetGIFSize */

int SkipGIFDataBlock(BPositionIO& data)
{
	unsigned char count;

	data >> count;

	if (count)
	{
		char buf[256];
		CheckedRead(data, buf, count);
	}
	
	return count;
} /* GetGIFDataBlock */

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
