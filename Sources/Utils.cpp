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
#include "PApp.h"
#include "Utils.h"
#include "HError.h"

//uchar gSelectedMap[256];
//
//void InitSelectedMap()
//{
//	BScreen screen;
//	rgb_color c;
//	int i;
//	
//	for (i = 0; i < 256; i++)
//	{
//		c = screen.ColorForIndex(i);
//		c.red = c.red * 2 / 3;
//		c.green = c.green * 2 / 3;
//		c.blue = c.blue * 2 / 3;
//		gSelectedMap[i] = screen.IndexForColor(c);
//	}
//} /* InitSelectedMap */

double Round(double d)
{
	double heel, deel;
	deel = modf(fabs(d), &heel);
	
	if (deel > 0.5)
		heel += 1.0;
	else if (deel == 0.5 && fmod(heel, 2.0) != 0.0)
		heel += 1.0;
	
	return d < 0 ? -heel : heel;
} // round
	
void MakeOutline(BRegion& rgn)
{
	BRegion t1, t2, t3;
	
	t1 = rgn;
	t1.OffsetBy(1, 1);
	
	t2 = rgn;
	t2.OffsetBy(-1, -1);
	
	t3 = rgn;
	
	t3.Exclude(&t1);
	rgn.Exclude(&t2);
	rgn.Include(&t3);
} /* MakeOutline */

bool RegionIsValid(BRegion& rgn)
{
	bool valid = rgn.Frame().IsValid();
	int i;
	
	for (i = 0; valid && i < rgn.CountRects(); i++)
		valid = rgn.RectAt(i).IsValid();
	
	if (!valid)
	{
		puts("Invalid region:");
		rgn.PrintToStream();

		FILE *f = fopen("pe.log", "w");
		if (!f) throw(strerror(errno));
		fseek(f, 0, SEEK_END);
 
		time_t t;
		time(&t);

		fprintf(f, "---\ndate: %sInvalid region in pe:\n", ctime(&t));
		
		BRect b = rgn.Frame();
		
		fprintf(f, "Frame(%s): %g, %g, %g, %g\n", b.IsValid() ? "valid" : "invalid",
			b.left, b.top, b.right, b.bottom);

		int i;
		
		for (i = 0; i < rgn.CountRects(); i++)
		{
			b = rgn.RectAt(i);
			fprintf(f, "Frame(%s): %g, %g, %g, %g\n", b.IsValid() ? "valid" : "invalid",
				b.left, b.top, b.right, b.bottom);
		}
		
		if (rgn.Frame().IsValid())
			fputs("Frame was valid", f);
		else
		{
			fputs("Frame Invalid", f);
			valid = true;
		}
		
		fclose(f);
	}

	return valid;
} /* RegionIsValid */


void OpenInTracker(const entry_ref& ref)
{
	BMessage msg(B_REFS_RECEIVED);
	msg.AddRef("refs", &ref);

	entry_ref app;
	if (be_roster->FindApp("application/x-vnd.Be-TRAK", &app))
		THROW((0));

	if (be_roster->IsRunning(&app))
	{
		BMessenger msr(NULL, be_roster->TeamFor(&app));
		msr.SendMessage(&msg);
	}
	else if (be_roster->Launch(&app, &msg))
		THROW(("Tracker not running???"));
} /* OpenInTracker */

void SendToIDE(const BMessage& msg, BMessage *reply)
{
	entry_ref ide;
	if (be_roster->FindApp("application/x-mw-BeIDE", &ide))
		THROW(("BeIDE was not found"));

	if (be_roster->IsRunning(&ide))
	{
		BMessenger msgr(NULL, be_roster->TeamFor(&ide));
		msgr.SendMessage(const_cast<BMessage*>(&msg), reply);
	}
	else
		THROW(("BeIDE is not running"));	
} /* SendToIDE */

void RelativePath(const entry_ref& a, const entry_ref& b, char* path)
{
	BPath pa, pb;
	
	FailOSErr(BEntry(&a).GetPath(&pa));
	FailOSErr(BEntry(&b).GetPath(&pb));
	
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
		strcpy(path, "./");
		strcat(path, sb);
	}
	else
	{
		path[0] = 0;
		char *t;
		
		while ((t = strchr(sa, '/')) != NULL)
		{
			strcat(path, "../");
			sa = t + 1;
		}
		
		strcat(path, sb);
	}
} /* RelativePath */

void TruncPathString(char *path, int textTraitsID, int width)
{
	if (be_plain_font->StringWidth(path) < width)
		return;
	
	char *p = strchr(path, kDirectorySeparator), *r;
	r = p + 1;
	
	if (!p)
		return;
	
	float w = be_plain_font->StringWidth("…") +
		be_plain_font->StringWidth(path, r - path);

	while (p && be_plain_font->StringWidth(p) + w > width)
	{
		p = strchr(p + 1, kDirectorySeparator);
	}
	
	if (p > r)
	{
		strcpy(r, "…");
		strcat(r, p);
	}
} // TruncPathString

void c2pstrcpy(unsigned char *p, const char *s)
{
	p[0] = 0;
	
	char *t = (char *)p + 1;
	
	while (*s)
		*t++ = *s++, ++(p[0]);
}

void p2cstrcpy(char *p, const unsigned char *s)
{
	int l = *s++;
	
	while (l--)
		*p++ = *s++;
	
	*p = 0;
}
//
//OSErr AEGetDescData(const AEDesc *desc, DescType *typeCode, void *dataBuffer, ByteCount maximumSize, ByteCount *actualSize)
//{
//	*typeCode = desc->descriptorType;
//	Handle h = (Handle)desc->dataHandle;
//	ByteCount dataSize = GetHandleSize(h);
//	if (dataSize > maximumSize)
//		*actualSize = maximumSize;
//	else
//		*actualSize = dataSize;
//	BlockMoveData(*h, dataBuffer, *actualSize);
//	return noErr;
//}
//

