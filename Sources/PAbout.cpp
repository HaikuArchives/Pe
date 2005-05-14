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

	Created: 09/15/97 23:11:37
*/

#include "pe.h"
#include "PAbout.h"
#include "HAppResFile.h"
#include "HError.h"
#include "ResourcesMisc.h"
#if STANDALONE
#else
#	include "PApp.h"
#endif

const char
	kAboutText[] =
#if __INTEL__
		"Pe 2.3 (x86) Open Source Version\n\n"
#else
		"Pe 2.3 (PPC) Open Source Version\n\n"
#endif
		"a high performance editor\n"
		"created by Maarten Hekkelman.\n"
		"Hekkelman Programmatuur B.V.\n"
		"Copyright © 1997, 1998, 2002.\n"
		"http://www.hekkelman.com/\n\n-\n\n"
		"Thanks to all those\nwho sent in feedback.\n"
		"Your help is invaluable.\n\n"
		"Special thanks to\nTom Hays\nfor improving the TeX addon\n\n"
		"and Be of course\nfor making this wonderful OS.\n\n"
		"-\n\n"
		"This is the Open Source version of Pe,\n"
		"compiled on:\n%s\n\n"
		"Pe now lives at http://www.beunited.org\n\n"
		"You can reach the current developers\n"
		"via Pe's mailing-list,\n"
		"pedevel@freelists.org\n\n"
		"-\n\n"
		"Please send suggestions and/or bugreports\n"
		"to pedevel@hirschkaefer.org\n\n"
		"Thanks to all contributors.\n\n",
	kAboutText2[] = "BeOS Rules!";

#if STANDALONE
const rgb_color
	gKeyWordColor = { 0, 0, 255, 0 },
	gCommentColor = { 255, 0, 0, 0 };
#endif

const long
	kWindowWidth = 250,
	kWindowHeight = 200,
	kPEOffsetH = 95,
	kPEOffsetV = 60,
	kAboutTextWidth = kWindowWidth - 20,
	kAboutTextHeight = 100;

const bigtime_t
	kDelay = 30000;

const unsigned long msg_Animate = 'anim';

static const char *Now()
{
#if STANDALONE
	return "now";
#else
	static const char *now = (char *)HResources::GetResource(rt_TIM, ri_TIM_COMPILED);
	return now;
#endif
} /* Now */

static BRect AboutRect()
{
	BScreen s;
	BRect f = s.Frame(), r;
	
	r.left = (f.Width() - kWindowWidth) / 2;
	r.top = (f.Height() - kWindowHeight) / 2;
	
	r.right = r.left + kWindowWidth;
	r.bottom = r.top + kWindowHeight;

	return r;
} /* AboutRect */

void PAboutBox::DetermineBounds(const char *txt, BRect &r)
{
	font_height fh;
	be_plain_font->GetHeight(&fh);
	float lh = ceil(fh.ascent + fh.descent + fh.leading);
	
	int lc = 0;

	do
	{
		if (*txt++ == '\n') lc++;
	}
	while (*txt);
	
	r.top = r.left = 0;
	r.right = kAboutTextWidth;
	r.bottom = lh * lc + 2 * kAboutTextHeight;
} /* PAboutBox::DetermineBounds */

void PAboutBox::DrawText(const char *txt, BView *vw)
{
	vw->FillRect(vw->Bounds(), B_SOLID_LOW);
	vw->SetFont(be_plain_font);

	font_height fh;
	be_plain_font->GetHeight(&fh);
	float lh = ceil(fh.ascent + fh.descent + fh.leading);

	const char *lp, *ep;
	float l = kAboutTextHeight + fh.ascent + fh.leading, w;
	
	lp = txt;
	
	while (*lp)
	{
		ep = strchr(lp, '\n');

		if (ep == NULL)
			break;
		else if (ep == lp)
			; // empty line
		else if (strncmp(lp, "-", ep - lp) == 0)
		{
			vw->MovePenTo(0, l - lh / 2);
			vw->StrokeLine(BPoint(kAboutTextWidth, l - lh / 2));
		}
//		else if (strncmp(lp, "--date--", ep - lp) == 0)
//		{
//			w = be_plain_font->StringWidth(__DATE__);
//			vw->MovePenTo((kAboutTextWidth - w) / 2, l);
//			vw->DrawString(__DATE__);
//		}
		else
		{
			w = be_plain_font->StringWidth(lp, ep - lp);
			vw->MovePenTo((kAboutTextWidth - w) / 2, l);
			vw->DrawString(lp, ep - lp);
		}
		
		l += lh;
		lp = ep + 1;
	}
} /* PAboutBox::DrawText */

PAboutBox::PAboutBox()
	: BWindow(AboutRect(), "aboutbox", B_BORDERED_WINDOW, 0)
{
	fOffscreen = new BBitmap(Bounds(), B_RGB_32_BIT, true);
	
	fAboutText = (char *)malloc(strlen(kAboutText) + strlen(Now()) + 1);
	FailNil(fAboutText);
	sprintf(fAboutText, kAboutText, Now());
	
	fPFont.SetFamilyAndStyle("Baskerville", "Roman");
	fPFont.SetSize(60);
	fPFont.SetSpacing(B_CHAR_SPACING);
	fEFont = fPFont;
	
	fPFont.SetShear(45);

	if (fOffscreen->Lock())
	{
		fOffscreen->AddChild(fOffView = new BView(Bounds(), "view", 0, B_WILL_DRAW));
		fOffView->SetFont(&fPFont);
		fOffscreen->Unlock();
	}

	BRect r;
	DetermineBounds(fAboutText, r);

	fOffText = new BBitmap(r, B_RGB_32_BIT, true);
	BView *tv;
	fOffText->AddChild(tv = new BView(r, "text", 0, B_WILL_DRAW));
	
	if (fOffText->Lock())
	{
		DrawText(fAboutText, tv);
		tv->Sync();
		fOffText->Unlock();
	}

	AddChild(fView = new PAboutView(Bounds(), fOffscreen, fOffText));

	fTextViewD.Set(10, 100, 10 + kAboutTextWidth, kWindowHeight);
	fTextViewS = fTextViewD;
	fTextViewS.OffsetTo(0, 0);
	
	fState1 = 0;
	fState2 = 0;
	fVelocity = 10;
	fNextFrame = 0;
	
	this->PostMessage(msg_Animate);
	
	Show();
} /* PAboutBox::PAboutBox */

PAboutBox::~PAboutBox()
{
	delete fOffscreen;
	free(fAboutText);

	delete fOffText;
	
#if STANDALONE
	be_app->PostMessage(B_QUIT_REQUESTED);
#endif
} /* PAboutBox::~PAboutBox */

void PAboutBox::MessageReceived(BMessage *msg)
{
	if (msg->what == msg_Animate)
		Animate();
	else
		BWindow::MessageReceived(msg);
} /* PAboutBox::MessageReceived */

void PAboutBox::Animate()
{
	BRect b(Bounds());

	bigtime_t now = system_time();
	if (fNextFrame > now)
		snooze(fNextFrame - now);
	fNextFrame = now + kDelay;
	
	if (fOffscreen->Lock())
	{
		fOffView->FillRect(b, B_SOLID_LOW);
		fOffView->DrawBitmap(fOffText, fTextViewS, fTextViewD);

		int i;
		uchar wipe[8] = { 0xEE, 0xCC, 0xAA, 0x88, 0x66, 0x44, 0x22, 0x00 };
		
		fOffView->SetDrawingMode(B_OP_ADD);
		
		for (i = 0; i < 7; i++)
		{
			fOffView->SetHighColor(wipe[i], wipe[i], wipe[i]);
			
			BPoint pl = fTextViewD.LeftTop(), pr = fTextViewD.RightTop();
			pl.y += i; pr.y += i;
			
			fOffView->StrokeLine(pl, pr);
			
			pl = fTextViewD.LeftBottom(), pr = fTextViewD.RightBottom();
			pl.y -= i; pr.y -= i;
			
			fOffView->StrokeLine(pl, pr);
		}
		
		fOffView->SetDrawingMode(B_OP_COPY);
		
		if ((++fTextStep % 2) == 0)
		{
			fTextViewS.OffsetBy(0, 1);
			if (fTextViewS.bottom > fOffText->Bounds().bottom)
			{
				fTextViewS.OffsetTo(0, 0);
			}
		}
		
		switch (fState1)
		{
			case 0:
			{
//				fOffView->FillRect(b, B_SOLID_LOW);
//				fOffView->SetHighColor(gColor[kKeyWordColor]);
				fOffView->SetHighColor(0x39, 0x74, 0x79, 255);
				
				BPoint p;
				p.y = kPEOffsetV;
				p.x = b.left + fState2;
				
				fState2 += (fVelocity *= 0.95);
		
				fOffView->DrawString("P", p);
				
				if (fState2 >= kPEOffsetH)
				{
					fState1++;
					fState2 = 45;
					fVelocity = 7.5;
				}
				break;
			}
			
			case 1:
			{
//				fOffView->FillRect(b, B_SOLID_LOW);
				fOffView->SetHighColor(0x39, 0x74, 0x79, 255);
				BPoint p;
				p.y = kPEOffsetV;
				p.x = b.left + kPEOffsetH;

				fPFont.SetShear(fState2);
				fOffView->SetFont(&fPFont);
				
				fOffView->DrawString("P", p);
				
				if (fState2 <= 90)
					fState2 += (fVelocity *= 1.05);
				else if (fState2 < 135)
					fState2 += (fVelocity *= 0.95);
				else
					fState1++;
				break;
			}
			
			case 2:
			{
//				fOffView->FillRect(b, B_SOLID_LOW);
				fOffView->SetHighColor(0x39, 0x74, 0x79, 255);
		
				BPoint p;
				p.y = kPEOffsetV;
				p.x = b.left + kPEOffsetH;

				fPFont.SetShear(fState2);
				fOffView->SetFont(&fPFont);
				
				fOffView->DrawString("P", p);
				
				if (fState2 > 90)
					fState2 -= (fVelocity *= 0.95);
				else
				{
					fPFont.SetShear(90);
					fState1++;
					fState2 = b.right;
					fPStop = kPEOffsetH + fPFont.StringWidth("P");
					fVelocity = 10;
				}
				break;
			}
			
			case 3:
			{
//				fOffView->FillRect(b, B_SOLID_LOW);
				fOffView->SetHighColor(0x39, 0x74, 0x79, 255);
		
				BPoint p;
				p.y = kPEOffsetV;
				p.x = b.left + kPEOffsetH;

				fOffView->SetFont(&fPFont);
				fOffView->DrawString("P", p);
				
				p.x = fState2;
				
				fState2 -= (fVelocity *= 0.95);
				
				fOffView->SetHighColor(0xa1, 0x64, 0x0e, 255);
				fEFont.SetShear(90);
				fOffView->SetFont(&fEFont);
				
				fOffView->DrawString("e", p);
				
				if (fState2 <= fPStop)
				{
					fState1 += 2;
//					fState2 = 135;
					fState2 = 90;
					fVelocity = 7.5;
				}
				break;
			}
			
			case 4:
			{
//				fOffView->FillRect(b, B_SOLID_LOW);
				fOffView->SetHighColor(0x39, 0x74, 0x79, 255);
		
				BPoint p;
				p.y = kPEOffsetV;
				p.x = b.left + kPEOffsetH;

				fOffView->SetFont(&fPFont);
				fOffView->DrawString("P", p);
				
				p.x = fPStop;
				
				fOffView->SetHighColor(0xa1, 0x64, 0x0e, 255);
				fEFont.SetShear(fState2);
				fOffView->SetFont(&fEFont);
				
				fOffView->DrawString("e", p);
				
				if (fState2 > 90)
					fState2 -= (fVelocity *= 1.05);
				else
					fState1++;
				break;
			}
			
			case 5:
			{
//				fOffView->FillRect(b, B_SOLID_LOW);
				fOffView->SetHighColor(0x39, 0x74, 0x79, 255);
		
				BPoint p;
				p.y = kPEOffsetV;
				p.x = b.left + kPEOffsetH;

				fPFont.SetShear(fState2);
				fOffView->SetFont(&fPFont);
				
				fOffView->DrawString("P", p);
				fOffView->SetHighColor(0xa1, 0x64, 0x0e, 255);
				fOffView->DrawString("e");
				
				if (fState2 <= 45)
					fState1++;
				else
					fState2 -= (fVelocity *= 0.95);
				break;
			}
			
			case 6:
			{
//				fOffView->FillRect(b, B_SOLID_LOW);
				fOffView->SetHighColor(0x39, 0x74, 0x79, 255);
		
				BPoint p;
				p.y = kPEOffsetV;
				p.x = b.left + kPEOffsetH;

				fPFont.SetShear(fState2);
				fOffView->SetFont(&fPFont);
				
				fOffView->DrawString("P", p);
				fOffView->SetHighColor(0xa1, 0x64, 0x0e, 255);
				fOffView->DrawString("e");
				
				if (fState2 < 90)
					fState2 += (fVelocity *= 0.95);
				else
				{
					fState1++;
//					fPFont.SetShear(90);
//					fState2 = b.right;
//					fPStop = - be_plain_font->StringWidth(fAboutText);
//				}
//				break;
//			}
//			
//			case 7:
//			{
////				fOffView->FillRect(b, B_SOLID_LOW);
//		
//				BPoint p;
//				p.y = kPEOffsetV;
//				p.x = b.left + kPEOffsetH;
//
//				fOffView->SetFont(&fPFont);
//				
//				fOffView->SetHighColor(0x39, 0x74, 0x79, 255);
//				fOffView->DrawString("P", p);
//				fOffView->SetHighColor(0xa1, 0x64, 0x0e, 255);
//				fOffView->DrawString("e");
//				
//				p.y = b.bottom - 4;
//				p.x = fState2;
//				
//				fOffView->SetFont(be_plain_font);
//				fOffView->SetHighColor(0, 0, 0);
//				fOffView->DrawString(fAboutText, p);
//				
//				if (--fState2 < fPStop)
//				{
					fState1++;
					fState2 = kPEOffsetV;
					fVelocity = 1;
					fPStop = b.left + kPEOffsetH + fPFont.StringWidth("P");
				}
//					fState2 = b.right;
				break;
			}
			
			case 8:
			{
//				fOffView->FillRect(b, B_SOLID_LOW);
		
//				fPFont.SetRotation(fVelocity * 10);
//				fEFont.SetRotation(360 - fVelocity * 10);
				
				BPoint p;
				p.y = kPEOffsetV;
				p.x = b.left + kPEOffsetH;

				fOffView->SetFont(&fPFont);
				fOffView->SetHighColor(0x39, 0x74, 0x79, 255);
				fOffView->DrawString("P", p);

				p.y = fState2;
//				p.x += fPFont->StringWidth("P");
				p.x = fPStop;

				fOffView->SetHighColor(0xa1, 0x64, 0x0e, 255);
				fOffView->DrawString("e", p);
				
				fState2 += (fVelocity *= 1.05);
				if (fState2 > b.bottom + 30)
				{
					fState1++;
					fState2 = kPEOffsetV;
					fVelocity = 1;
				}
				break;
			}
			
			case 9:
			{
//				fOffView->FillRect(b, B_SOLID_LOW);
		
				BPoint p;
				p.y = fState2;
				p.x = b.left + kPEOffsetH;
				
				fOffView->SetFont(&fPFont);
				fOffView->SetHighColor(0x39, 0x74, 0x79, 255);
				fOffView->DrawString("P", p);

				fState2 += (fVelocity *= 1.05);
				if (fState2 > b.bottom + 30)
				{
					fState1++;
					fState2 = 0;
					fVelocity = 1;
					
					fPFont.SetSize(32);
					fOffView->SetFont(&fPFont);
					
					fPStop = b.left + (b.Width() - fPFont.StringWidth(kAboutText2)) / 2;
				}
				break;
			}
			
			case 10:
			{
//				fOffView->FillRect(b, B_SOLID_LOW);
				
				BPoint p;
				p.y = fState2;
				p.x = fPStop;
				
				fOffView->SetHighColor(0, 0, 80);
				fOffView->DrawString("B", p);
				fOffView->SetHighColor(178, 0, 0);
				fOffView->DrawString("e");
				fOffView->SetHighColor(0, 0, 0);
				fOffView->DrawString("OS ");
				fOffView->SetHighColor(0x85, 0x19, 0x19, 255);
				fOffView->DrawString(kAboutText2 + 5);
				
				fState2 += (fVelocity *= 1.04);
				
				if (fState2 > kPEOffsetV)
				{
					fState1++;
					fState2 = 0;
				}
				break;
			}
			
			case 11:
			{
				BPoint p;
				p.y = kPEOffsetV;
				p.x = fPStop;
				
				fOffView->SetHighColor(0, 0, 80);
				fOffView->DrawString("B", p);
				fOffView->SetHighColor(178, 0, 0);
				fOffView->DrawString("e");
				fOffView->SetHighColor(0, 0, 0);
				fOffView->DrawString("OS ");
				fOffView->SetHighColor(0x85, 0x19, 0x19, 255);
				fOffView->DrawString(kAboutText2 + 5);

				if (++fState2 > 100)
				{
					fState1++;
					fState2 = 0;
				}
				break;
			}
			
			case 12:
			{
				BPoint p;
				p.y = kPEOffsetV;
				p.x = fPStop;
				
				unsigned int c = (int)fState2 * 10;
				
				fOffView->SetHighColor(c, c, (80 * (25 - fState2) + fState2 * 250) / 25);
				fOffView->DrawString("B", p);
				fOffView->SetHighColor((178 * (25 - fState2) + fState2 * 250) / 25 , c, c);
				fOffView->DrawString("e");
				fOffView->SetHighColor(c, c, c);
				fOffView->DrawString("OS ");
				
				rgb_color clr = { 0x85, 0x19, 0x19, 255 };
				clr.red = (uint8)((clr.red * (25 - fState2) + fState2 * 250) / 25);
				clr.green = (uint8)((clr.green * (25 - fState2) + fState2 * 250) / 25);
				clr.blue = (uint8)((clr.blue * (25 - fState2) + fState2 * 250) / 25);
				
				fOffView->SetHighColor(clr);
				fOffView->DrawString(kAboutText2 + 5);
				
				if (++fState2 > 25)
				{
					fState1++;
					fState2 = 0;
				}
				break;
			}
			
			case 13:
			{
				fOffView->SetDrawingMode(B_OP_COPY);
				
				if (++fState2 > 10)
				{
					fState1 = 0;
					fState2 = 0;
					fVelocity = 10;
					fPFont.SetShear(45);
					fPFont.SetSize(60);
					fOffView->SetFont(&fPFont);
				}
				break;
			}
			
			default:
				return;
		}
		
		fOffView->Sync();
		fOffscreen->Unlock();
	}
	
	if (Lock())
	{
		fView->Draw(b);
		Unlock();
	}
	
	PostMessage(msg_Animate);
} /* PAboutBox::Animate */

#pragma mark -> PAboutView <-

PAboutView::PAboutView(BRect frame, BBitmap *offscreen, BBitmap *offText)
	: BView(frame, "about view", 0, B_WILL_DRAW)
{
	fOffscreen = offscreen;
	fOffText = offText;
} /* PAboutView::PAboutView */

void PAboutView::Draw(BRect /*update*/)
{
	DrawBitmap(fOffscreen, BPoint(0, 0));
} /* PAboutView::Draw */

void PAboutView::MouseDown(BPoint /*where*/)
{
	Window()->PostMessage(B_QUIT_REQUESTED);
} /* PAboutView::MouseDown */

#if STANDALONE
main()
{
	new BApplication("application/x-be-executable");
	new PAboutBox;
	
	be_app->Run();
	delete be_app;
	return 0;
}
#endif
