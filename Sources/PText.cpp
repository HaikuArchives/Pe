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

	Created: 09/19/1997
*/

#include "pe.h"

#include <signal.h>

#include "PText.h"
#include "FontStyle.h"
#include "Utils.h"
#include "PStatus.h"
#include "PApp.h"
#include "PCmd.h"
#include "CLangIntf.h"
#include "PDoc.h"
#include "PExec.h"
#include "PToolBar.h"
#include "PMessages.h"
#include "HPreferences.h"
#include "HError.h"
#include "HAppResFile.h"
#include "HStream.h"
#include "CAlloca.h"
#include "HButtonBar.h"
#include "PItalicMenuItem.h"
#include "PSplitter.h"
#include "CGlossary.h"
#include "KeyBindings.h"
#include "CFindDialog.h"
#include "MAlert.h"
#include "HDefines.h"
#include "HColorUtils.h"

PText* PText::sfDragSource = NULL;

struct PSettings {
	BRect	wpos;
	float	vscroll, hscroll;
	long	anchor;
	long	caret;
	long	flags;
	float	fontsize;
	unsigned char	tabstops;
	unsigned char	encoding;
};

enum {
	flShowTabs,
	flSoftWrap,
	flSyntaxColoring
};

PText::PText(BRect frame, PTextBuffer& txt, BScrollBar *bars[], const char *ext)
	: BView(frame, "text view", B_FOLLOW_ALL_SIDES, B_ASYNCHRONOUS_CONTROLS |
		B_WILL_DRAW | B_NAVIGABLE | B_PULSE_NEEDED | B_FRAME_EVENTS)
	, fText(txt)
{
	fAnchor = fCaret = 0;
	fBlockSelect = false;
	fWalkOffset = 3.0;
	fNextCaret = 0;
	fCaretVisible = false;
	fStatus = NULL;
	fLastCommand = NULL;
	fExec = NULL;
	fWindowActive = false;
	fMetrics = NULL;
	fDragStart = -1;
	fSyntaxColoring = gSyntaxColoring;
	fCWD = NULL;
	fShowInvisibles = gPrefs->GetPrefInt("show invisibles", 0);
	fLangIntf = NULL;
	fLineEndType = leLF;
	SetLanguage(ext);
	fLastMouseTime = 0;
	fIncSearch = 0;
	fMark = -1;
	fNrArgument = 0;
	fAppendNextCut = false;
	fLastKillPoint = -1;
	
	fSoftWrap = gPrefs->GetPrefInt("softwrap", false);
	fWrapType = gPrefs->GetPrefInt("wraptype", 3);
	fWrapWidth = gPrefs->GetPrefInt("wrapcol", 80);
	
	fSplitAt = 0;
	fActivePart = 2;
	
	fVScrollBar1 = bars[0];
	fVScrollBar2 = bars[1];
	fHScrollBar = bars[2];
	
	fHScrollBar->SetTarget(this);
	fVScrollBar1->SetTarget(this);
	fVScrollBar2->SetTarget(this);

	gPrefs->InitTextFont(&fFont);

	SetViewColor(gColor[kLowColor]);
	
	fTabStops = gPrefs->GetPrefInt("spaces per tab", 4);

	fLineMap = NULL;
	fLineView = NULL;
	
	fMainPopUp = static_cast<BPopUpMenu*>(HResources::GetMenu(30, true));
	FailNil(fMainPopUp);
	fMainPopUp->SetFont(be_plain_font);
	fMainPopUp->SetRadioMode(false);

	ReInit();
	
	RecalculateLineBreaks();
} /* PText::PText */

void PText::ReInit()
{
	SetFont(&fFont);
	
	font_family ff;
	font_style fs;
	fFont.GetFamilyAndStyle(&ff, &fs);

	fFont.GetHeight(&fFH);
	fLineHeight = ceil(fFH.ascent + fFH.descent + fFH.leading);
	fMetrics = CFontStyle::Locate(ff, fs, fFont.Size(), fFont.Encoding());

	fTabWidth = fTabStops * StringWidth(" ", 1);
	
	if (Window())
	{
		PToolBar *toolBar = Doc()->ToolBar();
		if (toolBar) toolBar->SetTabWidth(fTabWidth, fTabStops);
	}
	
	RecalculateLineBreaks();
	AdjustScrollBars();
	
	SetupBitmap();
} /* PText::ReInit */

PText::~PText()
{
	FlushUndo();
	
	if (fExec)
		fExec->Cancel();
	
	if (fCWD)
		free(fCWD);
	
	if (fMainPopUp)
		delete fMainPopUp;
} /* PText::~PText */

void PText::SetLanguage(const char *ext)
{
	CLangIntf *i = CLangIntf::FindIntf(ext);
	if (i != fLangIntf)
	{
		fLangIntf = i;
		TouchLines(0);
		RestyleDirtyLines(0);
		Invalidate();
	}
} /* PText::SetLanguage */

int PText::Language () const
{
	return CLangIntf::GetIndex(fLangIntf);
} // PText::GetLanguage

void PText::SetLanguage(int indx)
{
	CLangIntf *i = (indx >= 0) ? CLangIntf::FindIntf(indx) : CLangIntf::FindIntf("");
	if (i != fLangIntf)
	{
		fLangIntf = i;
		TouchLines(0);
		RestyleDirtyLines(0);
		Invalidate();
	}
} /* PText::SetLanguage */

void PText::SetupBitmap()
{
	if (fLineMap) delete fLineMap;

	BRect r(0, 0, Bounds().Width(), fLineHeight - 1);

	fLineMap = new BBitmap(r, B_RGB_32_BIT, true);
	fLineView = new BView(r, "line view", 0, B_WILL_DRAW);

	if (fLineMap->Lock())
	{
		fLineMap->AddChild(fLineView);
		fLineView->SetViewColor(ViewColor());
		fLineView->SetLowColor(ViewColor());
		fLineView->SetFont(&fFont);
		fLineMap->Unlock();
	}
} /* PText::SetupBitmap */

void PText::SetText(char *text, size_t size)
{
	if (fText.Size()) fText.Delete(0, fText.Size());
	fText.Insert(text, size, 0);

	fLineEndType = fText.CurrentLE();
	if (fLineEndType != leLF)
		fText.TranslateToLF(fLineEndType);

	RecalculateLineBreaks();
	Invalidate();

	FlushUndo();
} /* PText::SetText */

void PText::AttachedToWindow()
{
	Window()->SetPulseRate(100000);
	fMainPopUp->SetTargetForItems(this);
	fBounds = Bounds();
	Doc()->ButtonBar()->SetDown(msg_SoftWrap, fSoftWrap);
} /* PText::AttachedToWindow */

const char* PText::Text()
{
	return fText.Buffer();
} /* PText::Text */

int PText::Size() const
{
	return fText.Size();
} /* PText::Size */

void PText::SetDirty(bool dirty)
{
	bool wasDirty = Doc()->IsDirty();
	
	if (wasDirty != dirty)
		Doc()->SetDirty(dirty);
} /* PText::SetDirty */

bool PText::IsDirty()
{
	return Doc()->IsDirty();
} /* PText::IsDirty */

void PText::GetSelectedText(char*& s, bool extend) 
{
	if (fAnchor != fCaret)
	{
		if (extend && !fBlockSelect)
		{
			int end = max(fCaret, fAnchor);
			int begin = min(fCaret, fAnchor);

			if (fText[end] == '.')
			{
				end++;
				while (isalpha(fText[end]))
					end++;
				ChangeSelection(begin, end);
			}
		}
		
		if (fBlockSelect)
			CopyBlock(s, min(fCaret, fAnchor), max(fCaret, fAnchor));
		else
		{
			int size = abs(fCaret - fAnchor);
			s = (char *)malloc(size + 1);
			FailNil(s);
			fText.Copy(s, min(fCaret, fAnchor), size);
			s[size] = 0;
		}
	}
	else
		s = NULL;
} /* PText::GetSelectedText */

void PText::Select(int anchor, int caret, bool update, bool block)
{
	if (update)
	{
		ChangeSelection(min(fText.Size(), max(0, anchor)),
			min(fText.Size(), max(0, caret)), block);
		if (fStatus) fStatus->SetOffset(fCaret);
	}
	else
	{
		fAnchor = anchor;
		fCaret = caret;
		fBlockSelect = block;
	}
} /* PText::Select */

void PText::SetStatus(PStatus *status)
{
	fStatus = status;
//	fStatus->SetHScroll(3);
//	fStatus->SetTabWidth(fTabWidth, fTabStops);
} /* PText::SetStatus */

void PText::WindowActivated(bool active)
{
	if (fWindowActive != active)
	{
		if (!active) HideCaret();
		fWindowActive = active;
		HiliteSelection();
	}
	
	if (active)
		AdjustScrollBars();
} /* PText::WindowActivated */

void PText::SetSettings(BMessage& msg)
{
	int32 i, anchor = 0, caret = 0;
	float f;
	const char *s1, *s2;
	bool b;
	
	if (gRestoreFont)
	{
		if (msg.FindInt32("tabstop", &i) == B_OK)			fTabStops = i;
//		if (msg.FindBool("show tabs", &b) == B_OK)			ShowTabStops(b);
		if (msg.FindFloat("fontsize", &f) == B_OK)			fFont.SetSize(f);
		if (msg.FindString("fontfamily", &s1) == B_OK && msg.FindString("fontstyle", &s2) == B_OK)
																		fFont.SetFamilyAndStyle(s1, s2);
		if (msg.FindBool("syntaxcoloring", &b) == B_OK)	fSyntaxColoring = b;
		if (msg.FindInt32("encoding", &i) == B_OK)			SetEncoding(i);

		if (msg.FindBool("softwrap", &b) == B_OK)			fSoftWrap = b;
		Doc()->ButtonBar()->SetDown(msg_SoftWrap, fSoftWrap);

		if (msg.FindInt32("wraptype", &i) == B_OK)			fWrapType = i;
		if (msg.FindInt32("wrapwidth", &i) == B_OK)		fWrapWidth = i;
	}

	ReInit();

	if (gRestoreSelection)
	{
		bool block = false;
		msg.FindInt32("anchor", &anchor);
		msg.FindInt32("caret", &caret);
		msg.FindBool("block", &block);
		Select(anchor, caret, true, block);
		fWalkOffset = Offset2Position(fCaret).x;
	}
	
	if (gRestoreScrollbar)
	{
		if (msg.FindFloat("vscroll", &f) == B_OK)				fVScrollBar2->SetValue(f);
		if (msg.FindFloat("hscroll", &f) == B_OK)				fHScrollBar->SetValue(f);
	}
	
	if (gRestoreCWD && msg.FindString("cwd", &s1) == B_OK)
	{
		if (fCWD) free(fCWD);
		fCWD = strdup(s1);
	}
	
	if (msg.FindString("language", &s1) == B_OK)
	{
		fLangIntf = CLangIntf::FindByName(s1);
		TouchLines(0);
		RestyleDirtyLines(0);
	}
} /* PText::SetSettings */

void PText::GetSettings(BMessage& msg)
{
	FailOSErr(msg.MakeEmpty());
	
	FailOSErr(msg.AddInt32("tabstop", fTabStops));
	FailOSErr(msg.AddFloat("fontsize", fFont.Size()));
	
	FailOSErr(msg.AddBool("show tabs", Doc()->ToolBar()->ShowsTabs()));
	
	font_family ff;
	font_style fs;
	fFont.GetFamilyAndStyle(&ff, &fs);
	
	FailOSErr(msg.AddString("fontfamily", ff));
	FailOSErr(msg.AddString("fontstyle", fs));

	FailOSErr(msg.AddInt32("anchor", fAnchor));
	FailOSErr(msg.AddInt32("caret", fCaret));
	FailOSErr(msg.AddFloat("vscroll", fVScrollBar2->Value()));
	FailOSErr(msg.AddFloat("hscroll", fHScrollBar->Value()));
	FailOSErr(msg.AddBool("syntaxcoloring", fSyntaxColoring));
	
	if (fFont.Encoding()) FailOSErr(msg.AddInt32("encoding", fFont.Encoding()));
	
	FailOSErr(msg.AddBool("softwrap", fSoftWrap));

	if (fSoftWrap)
	{
		FailOSErr(msg.AddInt32("wraptype", fWrapType));
		FailOSErr(msg.AddInt32("wrapwidth", fWrapWidth));
	}
	
	if (Language())
		FailOSErr(msg.AddString("language", fLangIntf->Name()));
	
	if (fCWD) FailOSErr(msg.AddString("cwd", fCWD));
} /* PText::GetSettings */

void PText::GetSettingsMW(BPositionIO& set)
{
	long anchor, caret, tabs, flags, fontsize;
	font_family font;
	font_style style;

	anchor = min(fAnchor, fCaret);
	caret = max(fAnchor, fCaret);
	tabs = fTabStops;
	flags = 0x01000000;
	fontsize = (long)fFont.Size();
	fFont.GetFamilyAndStyle(&font, &style);

	set << htonl(anchor) << htonl(caret) << htonl(tabs) << htonl(flags) << htonl(fontsize);
	set.Write(font, sizeof(font));
	set.Write(style, sizeof(style));
} /* PText::GetSettingsMW */

void PText::SetSettingsMW(BPositionIO& set)
{
	long anchor, caret, tabs, flags, fontsize;
	font_family font;
	font_style style;

	try
	{
		set >> anchor >> caret >> tabs >> flags >> fontsize;
		if (set.Read(font, sizeof(font)) != sizeof(font) ||
			set.Read(style, sizeof(style)) != sizeof(style))
			THROW(("IO Error"));

		if (gRestoreFont)
		{
			if (tabs)
				fTabStops = ntohl(tabs);
			if (*font && *style)
				fFont.SetFamilyAndStyle(font, style);
			if (fontsize)
				fFont.SetSize((float)(ntohl(fontsize)));
		}
		
		ReInit();

		if (gRestoreSelection)
		{
			Select(ntohl(anchor), ntohl(caret), true, false);
			fWalkOffset = Offset2Position(fCaret).x;
		}
	}
	catch (...) {}
} /* PText::SetSettingsMW */

void PText::SelectLine(int lineNr)
{
	if (lineNr < 0) lineNr = 0;
	if (lineNr >= LineCount()) lineNr = LineCount() - 1;

	int f = LineStart(lineNr);
	int t = lineNr < LineCount() - 1 ? LineStart(lineNr + 1) : fText.Size();
	ChangeSelection(f, t);
	ScrollToCaret();
	fStatus->SetOffset(fCaret);
} /* PText::SelectLine */

void PText::SetCaret(int newCaret)
{
	HideCaret();
	fStatus->SetOffset(fAnchor = fCaret = max(0, min(newCaret, fText.Size())));
	ScrollToCaret();
	ShowCaret();
} /* PText::SetCaret */

void PText::SetMark(int offset)
{
	if (fMark >= 0)
		TouchLine(Offset2Line(fMark));
	
	fMark = offset;
	
	if (fMark >= 0)
		TouchLine(Offset2Line(fMark));
} /* PText::SetMark */

void PText::SetSplitter(PSplitter *splitter)
{
	fSplitter = splitter;
} /* PText::SetSplitter */

void PText::SetEncoding(int encoding)
{
	fFont.SetEncoding(encoding);
	fText.SetEncoding(encoding);
	
	font_family fam;
	font_style sty;
	fFont.GetFamilyAndStyle(&fam, &sty);
	
	fMetrics = CFontStyle::Locate(fam, sty, fFont.Size(), encoding);
	
	SetFont(&fFont);
	
	RecalculateLineBreaks();
} /* PText::SetEncoding */

void PText::ShowTabStops(bool show)
{
	PToolBar *toolBar = Doc()->ToolBar();
	
	if (show != toolBar->ShowsTabs())
	{
		toolBar->SetShowsTabs(show);
	
		if (show)
		{
			toolBar->ResizeBy(0, kTabStopHeight);
			ResizeBy(0, -kTabStopHeight);
			MoveBy(0, kTabStopHeight);
		}
		else
		{
			toolBar->ResizeBy(0, -kTabStopHeight);
			ResizeBy(0, kTabStopHeight);
			MoveBy(0, -kTabStopHeight);
		}
		
		toolBar->SetTabWidth(fTabWidth, fTabStops);
		toolBar->SetShowsTabs(show);
		toolBar->Draw(toolBar->Bounds());
	}
} /* PText::ShowTabStops */

void PText::SetCWD(const char *cwd)
{
	if (!fCWD || strcmp(cwd, fCWD))
	{
		if (fCWD) free(fCWD);
		fCWD = strdup(cwd);
	}
} /* PText::SetCWD */

PDoc* PText::Doc() const
{
	return dynamic_cast<PDoc*>(Window());
} // PText::Doc

#pragma mark - Wrap

int PText::FindLineBreak(int offset, bool hard)
{
	if (fSoftWrap || hard)
	{
		if (fWrapType == 3)
		{
			int i, mark, w, sw, wl;
			
			i = mark = offset;
			w = sw = 0;
		
			while (i < fText.Size())
			{
				int nb;
				
				if (fText[i] == ' ')
					nb = i;
				else
					nb = fLangIntf->FindNextWord(*this, i, wl);
		
				int nw = w;
	
				if (fText[nb] != ' ' && fText[nb] != '\n')
					nw += wl + sw;
				
				if (nw > fWrapWidth)
				{
					if (w > 0 || sw > 0)
						return mark;
					else
					{
						do
						{
							i += fText.CharLen(i);
							w++;
						}
						while (w < fWrapWidth);
						
						return i;
					}
				}
				else if (fText[nb] == '\n')
				{
					return nb + 1;
				}
				else if (fText[nb] == '\t')
				{
					nw = (((w + sw) / fTabStops) + 1) * fTabStops;
					w = nw;
		
					if (w > fWrapWidth)
						return nb + 1;
		
					i = mark = nb + 1;
					sw = 0;
				}
				else if (fText[nb] == ' ')
				{
					mark = i = nb + 1;
					sw++;
				}
				else
				{
					w += sw + wl;
					mark = i = nb + 1;
					sw = 0;
				}
			}
			
			return fText.Size();
		}
		else
		{
			g_unit_t W = (fWrapType == 1 ? Bounds().Width() : 300) - 4;
			
			int i, mark, wl;
			float w, sw, ww;
			
			i = mark = offset;
			w = sw = ww = 0;
		
			while (i < fText.Size())
			{
				int nb;
				
				if (fText[i] == ' ')
					nb = i;
				else
				{
					nb = fLangIntf->FindNextWord(*this, i, wl);
					ww = TextWidth(i, nb - i + 1);
				}
		
				float nw = w;
	
				if (fText[nb] != ' ' && fText[nb] != '\n')
					nw += ww + sw;
				
				if (nw > W)
				{
					if (w > 0 || sw > 0)
						return mark;
					else
					{
						ww = 0;
						
						do
						{
							int l = fText.CharLen(i);
							ww += TextWidth(i, l);
							i += l;
						}
						while (ww < W);
						
						return i;
					}
				}
				else if (fText[nb] == '\n')
				{
					return nb + 1;
				}
				else if (fText[nb] == '\t')
				{
					int t = (int)floor((w + sw) / fTabWidth) + 1;
					w = (rint(t * fTabWidth) > rint(w) ? t * fTabWidth : (t + 1) * fTabWidth);
		
					if (w > W)
					{
						return nb + 1;
					}
		
					i = mark = nb + 1;
					sw = 0;
				}
				else if (fText[nb] == ' ')
				{
					mark = i = nb + 1;
					sw += fMetrics->CharWidth(" ");
				}
				else
				{
					w += sw + ww;
					mark = i = nb + 1;
					sw = 0;
				}
			}
			
			return fText.Size();
		}
	}
	else	// no softwrapping
	{
		int i = offset - 1;

		while (++i < fText.Size())
		{
			if (fText[i] == '\n')
			 return i + 1;
		}
		
		return fText.Size();
	}
} /* PText::FindLineBreak */

int PText::RewrapLines(int from, int to, bool hard)
{
	int offset, state, lb, fl, dy;
	
// first remove invalid linebreaks
	fl = min(Offset2Line(from), LineCount() - 1);

	VLineInfo::iterator ls, le;

	ls = fLineInfo.begin() + fl;
	
//	while (ls != fLineInfo.begin() && (*ls).nl == false)
//		ls--;
	if (ls != fLineInfo.begin() && (*ls).nl == false)
		ls--;
	
	if (ls == fLineInfo.begin())
		(*ls).dirty = true;

	le = ls + 1;
	
	while (le != fLineInfo.end() && (*le).start < to)
		le++;
	
	while (le != fLineInfo.end() && (*le).nl == false)
		le++;
	
	if (le == fLineInfo.end())
		to = fText.Size();
	else
		to = (*le).start;

	if (ls + 1 != le)
	{
		dy = (ls + 1) - le;
		fLineInfo.erase(ls + 1, le);
	}
	else
		dy = 0;

	le = ls + 1;
	
// then determine the new ones
	state = (*ls).state;
	lb = (*ls).start;
	
	offset = FindLineBreak(lb, hard);
	while (offset < to)
	{
		int len = offset - lb;
		CAlloca txt(len + 1);
		
		fText.Copy(txt, lb, len);
		txt[len] = 0;
		fLangIntf->ColorLine(txt, len, state, NULL, NULL);
		
		le = fLineInfo.insert(le, LineInfo(offset, state, fText[offset - 1] == '\n')) + 1;
		dy++;
		
		lb = offset;
		offset = FindLineBreak(offset, hard);
	}
	
	if (offset > 0 &&
		offset == fText.Size() &&
		fText[--offset] == '\n' &&
		fLineInfo.back().start != offset + 1)
	{
		int len = offset - lb;

		CAlloca txt(len + 1);

		fText.Copy(txt, lb, len);
		txt[len] = 0;
		fLangIntf->ColorLine(txt, len, state, NULL, NULL);

		fLineInfo.push_back(LineInfo(offset + 1, state, true));
		dy++;
	}

	AdjustScrollBars(); 

	return dy;
} /* PText::RewrapLines */

void PText::RecalculateLineBreaks()
{
	fLineInfo.erase(fLineInfo.begin(), fLineInfo.end());
	fLineInfo.push_back(LineInfo(0, 0));
	
	RewrapLines(0, fText.Size());
} /* PText::RecalculateLineBreaks */

#pragma mark - Undo

void PText::Undo()
{
	PCmd *cmd;
	
	if (fDoneCmds.size() == 0)
		return;
	
	HideCaret();
	
	fLastCommand = NULL;

	try 
	{
		cmd = fDoneCmds.top();
		if (cmd)
		{
			cmd->Undo();
			fDoneCmds.pop();
			fUndoneCmds.push(cmd);
			ScrollToCaret();
		}
		
		if (cmd && ! cmd->WasDirty())
			SetDirty(false);
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* PText::Undo */

void PText::Redo()
{
	PCmd *cmd;

	if (fUndoneCmds.size() == 0)
		return;

	HideCaret();

	fLastCommand = NULL;
	
	try
	{
		cmd = fUndoneCmds.top();
		if (cmd)
		{
			cmd->Redo();
			fUndoneCmds.pop();
			fDoneCmds.push(cmd);
			ScrollToCaret();
		}
		
		cmd = fUndoneCmds.size() ? fUndoneCmds.top() : NULL;
		if (cmd && ! cmd->WasDirty())
			SetDirty(true);
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* PText::Redo */

void PText::RegisterCommand(PCmd *cmd)
{
	try
	{
		cmd->Do();
		
		if (cmd->IsUndoable())
		{
			fDoneCmds.push(cmd);
			fLastCommand = cmd;
			
			while (fUndoneCmds.size())
			{
				delete fUndoneCmds.top();
				fUndoneCmds.pop();
			}
		}
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* PText::RegisterCommand */

void PText::ResetUndo()
{
	stack<PCmd*> t;
	
	while (! fDoneCmds.empty())
		t.push(fDoneCmds.top()), fDoneCmds.pop(), t.top()->SetDirty(true);
	while (! t.empty())
		fDoneCmds.push(t.top()), t.pop();
	
	while (! fUndoneCmds.empty())
		t.push(fUndoneCmds.top()), fUndoneCmds.pop(), t.top()->SetDirty(true);
	while (! t.empty())
		fUndoneCmds.push(t.top()), t.pop();
} /* PText::ResetUndo */
	
void PText::FlushUndo()
{
	while (fDoneCmds.size())
	{
		delete fDoneCmds.top();
		fDoneCmds.pop();
	}

	while (fUndoneCmds.size())
	{
		delete fUndoneCmds.top();
		fUndoneCmds.pop();
	}
} // PText::FlushUndo

#pragma mark - Text

void PText::CopyBlock(char*& s, int from, int to)
{
	int l1, l2, c1, c2, line;
	
	l1 = Offset2Line(from);
	l2 = Offset2Line(to);
	if (l1 > l2) swap(l1, l2);
	c1 = Offset2Column(from);
	c2 = Offset2Column(to);
	if (c1 > c2) swap(c1, c2);
	
	string t;
	
	for (line = l1; line <= l2; line++)
	{
		int o1, o2, l;
		
		o1 = Column2Offset(line, c1);
		o2 = Column2Offset(line, c2);
		l = o2 - o1;
		
		CAlloca txt(o2 - o1 + 2);
		fText.Copy(txt, o1, l);
		txt[l] = '\n';
		txt[l + 1] = 0;
		
		t += txt;
	}
	
	s = (char *)malloc(t.size() + 1);
	FailNil(s);
	strcpy(s, t.c_str());
} // PText::CopyBlock

void PText::Insert(const char *bytes, int numBytes, int offset)
{
	if (Doc()->IsReadOnly()) THROW(("Document is read-only"));
	
	ASSERT(numBytes >= 0);
	ASSERT(offset >= 0);
	ASSERT(offset <= fText.Size());

	int line = Offset2Line(offset);
		
	if (fMark >= offset)
		SetMark(fMark + numBytes);
	
	fText.Insert(bytes, numBytes, offset);
	OffsetLineBreaks(numBytes, line + 1);
	
	fShift = 0;
	TextChanged(offset, offset + numBytes);
} /* PText::Insert */

void PText::Delete(int from, int to)
{
	if (Doc()->IsReadOnly()) THROW(("Document is read-only"));

	ASSERT(from >= 0);
	ASSERT(to > from);
	ASSERT(to <= fText.Size());

	if (fMark >= from && fMark <= to)
		SetMark(from);
	else if (fMark > to)
		SetMark(fMark + from - to);
	
	fShift = -DeleteLineBreaks(from, to);
	fText.Delete(from, to);
	
	TextChanged(from, from);
} /* PText::Delete */

void PText::BackspaceKeyDown()
{
	if (Doc()->IsReadOnly()) THROW(("Document is read-only"));

	PTypingCmd *tc = dynamic_cast<PTypingCmd*>(fLastCommand);
	if (!tc)
		RegisterCommand(tc = new PTypingCmd(this));
	
	int cl = fText.PrevCharLen(fCaret);

	if (fAnchor == fCaret)
	{
		if (fCaret == 0)
		{
			beep();
			return;
		}
		fCaret -= cl;
		
// store info for undo
		if (tc->fInsertedLen > 0)
			tc->fInsertedLen -= cl;
		else
		{
			tc->fDeleted = (char *)realloc(tc->fDeleted, tc->fDeletedLen + cl);
			FailNil(tc->fDeleted);
			memmove(tc->fDeleted + cl, tc->fDeleted, tc->fDeletedLen);
			tc->fDeletedLen += cl;
			tc->fDeletedIndx -= cl;
			while (cl--)
				tc->fDeleted[cl] = fText[fCaret + cl];
		}
	}
	
	int from = min(fCaret, fAnchor);
	int to = max(fCaret, fAnchor);
	
	fCaret = fAnchor = from;
	Delete(from, to);

	fWalkOffset = Offset2Position(fCaret).x;
	RedrawDirtyLines();
} /* PText::BackspaceKeyDown */

void PText::DeleteKeyDown()
{
	if (Doc()->IsReadOnly()) THROW(("Document is read-only"));

	PTypingCmd *tc = dynamic_cast<PTypingCmd*>(fLastCommand);
	if (!tc)
		RegisterCommand(tc = new PTypingCmd(this));
	
	int cl = fText.CharLen(fCaret);

	if (fAnchor == fCaret)
	{
		if (fCaret == fText.Size())
		{
			beep();
			return;
		}
		fCaret += cl;

// store info for undo
		tc->fDeleted = (char *)realloc(tc->fDeleted, tc->fDeletedLen + cl);
		FailNil(tc->fDeleted);
		for (int i = 0; i < cl; i++)
			tc->fDeleted[tc->fDeletedLen + i] = fText[fAnchor + i];
		tc->fDeletedLen += cl;
	}
	
	int from = min(fCaret, fAnchor);
	int to = max(fCaret, fAnchor);
	
	fCaret = fAnchor = from;
	Delete(from, to);

	fWalkOffset = Offset2Position(fCaret).x;
	RedrawDirtyLines();
} /* PText::DeleteKeyDown */

void PText::CharKeyDown(const char *bytes, int numBytes)
{
	if (Doc()->IsReadOnly()) THROW(("Document is read-only"));

	PTypingCmd *tc = dynamic_cast<PTypingCmd*>(fLastCommand);
	if (!tc)
		RegisterCommand(tc = new PTypingCmd(this));
	
	int from = min(fCaret, fAnchor);
	int to = max(fCaret, fAnchor);

	if (fAnchor != fCaret)
	{
		fCaret = fAnchor = from;
		Delete(from, to);
	}
	
	Insert(bytes, numBytes, fCaret);
	fCaret = fAnchor = from + numBytes;

	tc->fInsertedLen += numBytes;
	
	if (numBytes == 1)
	{
		switch (bytes[0])
		{
			case ']': if (gBalance) Kiss(']'); break;
			case ')': if (gBalance) Kiss(')'); break;
			case '}':
				if (gBalance) Kiss('}');
				if (gSmartBrace) SmartBrace();
				break;
			case '\n': if (gAutoIndent) AutoIndent(from); break;
		}
	}
	
	fWalkOffset = Offset2Position(fCaret).x;
	RedrawDirtyLines();
	ScrollToCaret();
} /* PText::CharKeyDown */

void PText::TextChanged(int from, int to)
{
	int dy = 0;
	
	int line = Offset2Line(to);
	int lc = LineCount();
	dy = RewrapLines(from, to);

	if (dy + fShift)
	{
		if (fSplitAt > 0) ShiftLines(line + 1, dy + fShift, 1);
		ShiftLines(line + 1, dy + fShift, 2);
	}

	if (lc > LineCount() && fShift == 0)
	{
		fShift = -1;
		dy = max(dy, 2);
	}

	TouchLines(max(line + fShift - 1, 0), line + dy);
	RestyleDirtyLines(max(line + fShift, 0));

	SetDirty(true);
} /* PText::TextChanged */

void PText::TypeString(const char *string)
{
	if (Doc()->IsReadOnly()) THROW(("Document is read-only"));

	CharKeyDown(string, strlen(string));
	fStatus->SetOffset(fCaret);
} /* PText::TypeString */

#pragma mark - Split

const g_unit_t
	kSplitMinimum = 50;

#ifndef B_V_SCROLL_BAR_WIDTH
#define B_V_SCROLL_BAR_WIDTH 16
#endif

void PText::TrackSplitter(BPoint where)
{
	unsigned long btns;
	
	BRect b;
	BPoint p;

	ConvertFromScreen(&where);

	p = where;
	b = Bounds();
	
	if (fSplitAt == 0)
	{
		BRect src, dst;

		src = dst = b;
		src.bottom -= kSplitterHeight;
		dst.top += kSplitterHeight;
		
		CopyBits(src, dst);
	
		src.bottom = kSplitterHeight;
		Invalidate(src);
	
		fSplitAt = kSplitterHeight;
		Window()->UpdateIfNeeded();
		
		AdjustScrollBars();
	}

	do
	{
		g_unit_t dy =
			p.y - where.y;

		if (dy != 0)
		{
			if (fSplitAt + dy > b.bottom - b.top - kSplitterHeight)
				dy = b.bottom - kSplitterHeight - fSplitAt;
			else if (fSplitAt + dy < kSplitterHeight)
				dy = kSplitterHeight - fSplitAt;
			
			if (dy != 0)
				SplitterMoved(dy);
			
			where.y += dy;
		}
		
		snooze(10000);
		GetMouse(&p, &btns);
	}
	while (btns);
	
	DoneMovingSplitter();
} /* PText::SplitterMoved */

void PText::SplitterMoved(g_unit_t dy)
{
	g_unit_t ns = fSplitAt + dy;

	if (dy > 0)
	{
		fVScrollBar2->ResizeBy(0, -dy);
		fVScrollBar2->MoveBy(0, dy);
		
		fSplitter->MoveBy(0, dy);
		
		fVScrollBar1->ResizeTo(B_V_SCROLL_BAR_WIDTH, ns - kSplitterHeight + 1);
	}
	else
	{
		fVScrollBar1->ResizeTo(B_V_SCROLL_BAR_WIDTH, ns - kSplitterHeight + 1);

		fSplitter->MoveBy(0, dy);

		fVScrollBar2->MoveBy(0, dy);
		fVScrollBar2->ResizeBy(0, -dy);
	}

	BRect src(Bounds()), dst;

	if (ns > kSplitMinimum)
		fVScrollBar1->Show();
	else if (!fVScrollBar1->IsHidden())
		fVScrollBar1->Hide();

	if (src.Height() - ns < kSplitMinimum)
	{
		if (!fVScrollBar2->IsHidden())
		{
			fVScrollBar2->Hide();
			FillRect(ConvertFromParent(fVScrollBar2->Frame()), B_SOLID_LOW);
		}
	}
	else if (fVScrollBar2->IsHidden())
		fVScrollBar2->Show();

	src.top = fSplitAt - kSplitterHeight;
	dst = src;
	dst.OffsetBy(0, dy);
	
	CopyBits(src, dst);

	if (dy > 0)
	{
		src.top = fSplitAt - kSplitterHeight;
		src.bottom = max((int)ns - kSplitterHeight + 1, kSplitterHeight);//ns + 1;
	}
	else
		src.top = dst.bottom - 1;

	Invalidate(src);

	fSplitAt = ns;
	Window()->UpdateIfNeeded();
	
	AdjustScrollBars();
	
	Sync();
} /* PText::SplitterMoved */

void PText::DoneMovingSplitter()
{
	BRect b(Bounds()), f(Frame());
	
	g_unit_t v = fActivePart == 1 ? fVScrollBar1->Value() : fVScrollBar2->Value();
	
	if (kSplitMinimum > fSplitAt ||
		b.Height() - kSplitMinimum < fSplitAt + kSplitterHeight)
	{
		fActivePart = 2;
		fVScrollBar2->SetValue(v);
		
		fVScrollBar1->ResizeTo(B_V_SCROLL_BAR_WIDTH, 0);

		fSplitter->MoveTo(f.right + 1, f.top);

		fVScrollBar2->MoveTo(f.right + 1, f.top + kSplitterHeight);

		fVScrollBar2->ResizeTo(B_V_SCROLL_BAR_WIDTH, f.Height() - kSplitterHeight + 1);
		if (fVScrollBar2->IsHidden())
			fVScrollBar2->Show();
		
		fSplitAt = 0;

		TouchLines(floor(v / fLineHeight));
		RedrawDirtyLines();
	}
} /* PText::DoneMovingSplitter */

void PText::SwitchPart(int newPart)
{
	HideCaret();
	
	fActivePart = newPart;
	HiliteSelection();
} /* PText::SwitchPart */

void PText::SplitWindow()
{
	BRect b(Bounds()), f(Frame());
	
	if (b.Height() < kSplitMinimum * 2)
	{
		beep();
		return;
	}
	
	float m = b.Height() / 2;
	
	fSplitAt = floor(m);
	
	fVScrollBar2->ResizeTo(B_V_SCROLL_BAR_WIDTH, f.Height() - fSplitAt);
	fVScrollBar2->MoveTo(f.right + 1, f.top + fSplitAt);
	fSplitter->MoveTo(f.right + 1, f.top + fSplitAt - kSplitterHeight);
	fVScrollBar1->ResizeTo(B_V_SCROLL_BAR_WIDTH, fSplitAt - kSplitterHeight - 1);
	if (fVScrollBar1->IsHidden())
		fVScrollBar1->Show();
	
	AdjustScrollBars();
} /* PText::SplitWindow */

void PText::UnsplitWindow()
{
	SplitterMoved(-fSplitAt);
	DoneMovingSplitter();
} /* PText::UnsplitWindow */

void PText::FrameMoved(BPoint newPosition)
{
	float dy;
	if (fSplitAt > 0)
		dy = fVScrollBar1->Frame().top - newPosition.y;
	else
		dy = fSplitter->Frame().top - newPosition.y;

	fVScrollBar1->MoveBy(0, -dy);
	fSplitter->MoveBy(0, -dy);
	fVScrollBar2->ResizeBy(0, dy);
	fVScrollBar2->MoveBy(0, -dy);
	
	if (fSplitAt == 0)
		fVScrollBar1->ResizeTo(B_V_SCROLL_BAR_WIDTH, 0);
	
	fSplitter->Invalidate(fSplitter->Bounds());
//	fStatus->Invalidate(fStatus->Bounds());
	Window()->UpdateIfNeeded();
} /* PText::FrameMoved */

#pragma mark - Scroll

void PText::AdjustScrollBars()
{
	if (fVScrollBar1 == NULL || fVScrollBar2 == NULL || fHScrollBar == NULL)
		return;
	
	g_unit_t h, y;
	
	int lines = LineCount() + 1;

	if (fSplitAt > 0)
	{
		h = fBounds.Height() - fSplitAt;
		y = (lines * fLineHeight - h) / fLineHeight;
	
		fVScrollBar2->SetRange(0.0, max(y * fLineHeight, (float)0));
		fVScrollBar2->SetSteps(fLineHeight, max(h - fLineHeight, (float)0));
	
		h = fSplitAt - kSplitterHeight;
		y = (lines * fLineHeight - h) / fLineHeight;
	
		fVScrollBar1->SetRange(0.0, max(y * fLineHeight, (float)0));
		fVScrollBar1->SetSteps(fLineHeight, max(h - fLineHeight, (float)0));
	}
	else
	{
		h = fBounds.Height();
		y = (lines * fLineHeight - h) / fLineHeight;
	
		fVScrollBar2->SetRange(0.0, max(y * fLineHeight, (float)0));
		fVScrollBar2->SetSteps(fLineHeight, max(h - fLineHeight, (float)0));
	
		fVScrollBar1->SetRange(0.0, max(y * fLineHeight, (float)0));
		fVScrollBar1->SetSteps(fLineHeight, max(h - fLineHeight, (float)0));
	}
	
	if (fSoftWrap)
	{
		switch (fWrapType)
		{
			case 1:	fHScrollBar->SetRange(0, 0); break;
			case 2:	fHScrollBar->SetRange(0, max((float)0, (float)300 - Bounds().Width())); break;
			case 3:	fHScrollBar->SetRange(0, max((float)0, (StringWidth("m", 1) * fWrapWidth) - Bounds().Width())); break;
		}
	}
	else
		fHScrollBar->SetRange(0, 100000);

	fHScrollBar->SetSteps(fTabWidth/fTabStops, fBounds.Width() / 2);
} /* PText::AdjustScrollBars */

void PText::ScrollToCaret()
{
	ScrollToOffset(fCaret, fActivePart);
} /* PText::ScrollToCaret */

void PText::ScrollToOffset(int offset, int part)
{
	BRect b(fBounds);
	BPoint p = Offset2Position(offset);
	int line = Offset2Line(offset);
	g_unit_t y = -1, x = -1;
	
	if (part == 1)
		b.bottom = fSplitAt - kSplitterHeight;
	else
		b.top = fSplitAt;

	if (fAnchor < offset && line < LineCount() - 1 && LineStart(line) == offset)
	{
		line--;
		p.y -= fLineHeight;
	}
	
	if (p.y < b.top)
	{
		y = line * fLineHeight;
	}
	else if (p.y + fLineHeight > b.bottom)
	{
		y = (line + 1) * fLineHeight - b.Height() + 2;
	}
	
	if (p.x < b.left + 3)
		x = max((g_unit_t)0, p.x - 3);
	else if (p.x > b.right - 3)
		x = p.x - b.Width() + 3;
	
	if (y >= 0 || x >= -1)
	{
		if (y >= 0)
		{
			Window()->UpdateIfNeeded();
			if (part == 1)
				fVScrollBar1->SetValue(y);
			else
				fVScrollBar2->SetValue(y);
		}
		
		if (x >= 0)
			fHScrollBar->SetValue(x);

		Window()->UpdateIfNeeded();
	}
} /* PText::ScrollToOffset */

void PText::FrameResized(float /*w*/, float /*h*/)
{
	AdjustScrollBars();
	fBounds = Bounds();
	SetupBitmap();
		
	if (fSoftWrap && fWrapType == 1 &&
		! Window()->MessageQueue()->FindMessage(B_VIEW_RESIZED, 0))
	{
		RecalculateLineBreaks();
		RedrawDirtyLines();
	}
	
	if (fSplitAt > 0)
		DoneMovingSplitter();
} /* PText::FrameResized */

void PText::ScrollBarChanged(BScrollBar *bar, g_unit_t dy)
{
	BRect src, dst;
	
	do {
		src = fBounds & ConvertFromScreen(BScreen().Frame());
	} while (false);
	
	if (fCaretVisible && ((fActivePart == 1) == (fVScrollBar1 == bar)))
	{
		fNextCaret = system_time() + 500000;
		fCaretVisible = false;
		
		SetDrawingMode(B_OP_INVERT);
		BPoint p = Offset2Position(fCaret);
		p.y += 1 + dy;
		StrokeLine(p, BPoint(p.x, p.y + fLineHeight - 1));
		SetDrawingMode(B_OP_COPY);
	}
	
	if (bar == fVScrollBar1)
	{
		if (dy > 0)
		{
			src.top += dy;
			src.bottom = fSplitAt - kSplitterHeight - 1;
		}
		else
			src.bottom = fSplitAt + dy - kSplitterHeight - 1;
		
		dst = src;
		dst.OffsetBy(0, -dy);
		
		CopyBits(src, dst);
		
		if (dy < 0)
			src.bottom = min(fSplitAt - kSplitterHeight - 1, dst.top);
		else
			src.top = dst.bottom;
	}
	else
	{
		if (dy > 0)
			src.top = fSplitAt + dy;
		else
		{
			src.top = fSplitAt;
			src.bottom += dy;
		}
		
		src.top += 1;
		dst = src;
		dst.OffsetBy(0, -dy);
		
		CopyBits(src, dst);
		
		if (dy < 0)
		{
			src.top = fSplitAt;
			src.bottom = src.top - dy;
		}
		else
			src.top = max(fSplitAt, dst.bottom);

	}

	BRect dirty(src & fBounds);
	
		Invalidate(dirty);
		Window()->UpdateIfNeeded();

} /* PText::ScrollBarChanged */

void PText::ScrollTo(BPoint p)
{
	BView::ScrollTo(p);

	Doc()->ToolBar()->SetHOffset(-p.x + 3);
} /* PText::ScrollBy */

void PText::CenterSelection()
{
	int top, cnt;
	
	top = Offset2Line(min(fAnchor, fCaret));
	cnt = Offset2Line(max(fAnchor, fCaret)) - top + 1;
	
	if (fSplitAt == 0 || fActivePart == 2)
	{
		float h = Bounds().bottom - fSplitAt;

		if (cnt * fLineHeight < h)
		{
			cnt = (int)((h / fLineHeight) - cnt) / 2;
			top -= cnt;
		}
		
		fVScrollBar2->SetValue(top * fLineHeight);
	}
	else
	{
		if (cnt * fLineHeight < fSplitAt)
		{
			cnt = (int)((fSplitAt / fLineHeight) - cnt) / 2;
			top -= cnt;
		}
		
		fVScrollBar1->SetValue(top * fLineHeight);
	}
} /* PText::CenterSelection */

#pragma mark - Mouse

void PText::MouseDown(BPoint where)
{
	fLastCommand = NULL;
	if (fIncSearch)
	{
		fIncSearch = 0;
		Doc()->ButtonBar()->SetDown(msg_IncSearchBtn, false);
	}

	try
	{
		if (fSplitAt > 0 && where.y >= fSplitAt - kSplitterHeight && where.y <= fSplitAt)
		{
			BPoint p(where);
			ConvertToScreen(&p);
			TrackSplitter(p);
			return;
		}
		
		long modifiers;
		
		FailOSErr(Looper()->CurrentMessage()->FindInt32("modifiers", &modifiers));
		FailOSErr(Looper()->CurrentMessage()->FindInt32("buttons", (long *)&fDragButtons));
		
		bigtime_t t = system_time(), dbl;
		get_click_speed(&dbl);
		
		if ((t - fLastMouseTime <= dbl) &&
			fabs(where.y - fLastMouseDown.y) < 3 &&
			fabs(where.x - fLastMouseDown.x) < 3)
		{
			fMouseClicks = fMouseClicks % 3 + 1;
		}
		else
		{
			fLastMouseDown = where;
			fMouseClicks = 1;
		}
		
		fLastMouseTime = t;
		
		int curOffset = Position2Offset(where);
		int anchor1, anchor2;
		anchor1 = min(fAnchor, fCaret);
		anchor2 = max(fAnchor, fCaret);
		
		if (where.y > fSplitAt && fActivePart == 1)
			SwitchPart(2);
		else if (where.y < fSplitAt && fActivePart == 2)
			SwitchPart(1);

		if ((curOffset < anchor1 || curOffset > anchor2 || anchor1 == anchor2) &&
			fDragButtons & (B_SECONDARY_MOUSE_BUTTON | B_TERTIARY_MOUSE_BUTTON))
		{
			ShowContextualMenu(where);
			return;
		}

		if (curOffset > anchor1 && curOffset < anchor2 &&
			(fDragButtons & (B_SECONDARY_MOUSE_BUTTON | B_TERTIARY_MOUSE_BUTTON) ||
			 (fMouseClicks == 1 && WaitMouseMoved(where))))
		{
			char *s;
			GetSelectedText(s);
			FailNil(s);
			
			BMessage drag(B_SIMPLE_DATA);
			FailOSErr(drag.AddData("text/plain", B_MIME_DATA, s, strlen(s)));
			FailOSErr(drag.AddString("be:clip_name", "Text Snippet"));
			
			fDragStart = min(fAnchor, fCaret);
			sfDragSource = this;

			fSavedAnchor = fAnchor;
			fSavedCaret = fCaret;
		
			BRegion rgn;
			Selection2Region(rgn, fActivePart);
			DragMessage(&drag, rgn.Frame());

			free(s);
			return;
		}

		if (modifiers & B_SHIFT_KEY)
			ChangeSelection(fAnchor, curOffset, fBlockSelect);
		else
		{
			if (modifiers & B_OPTION_KEY)
				fBlockSelect = true;
			
			bool balanced = false;
			
			switch (fMouseClicks)
			{
				case 1:
					anchor1 = anchor2 = curOffset;
					break;
				case 2:
				{
					int subco = 0;
					
					if (Offset2Position(curOffset).x > where.x)
						subco = 1;
					
					switch (fText[curOffset - subco])
					{
						case '{':
						case '(':
						case '[':
						case '<':
							anchor1 = curOffset + 1 - subco;
							anchor2 = FindTheOther(anchor1, fText[curOffset - subco]);
							if (anchor2 == -1)
							{
								beep();
								anchor2 = anchor1;
							}
							balanced = true;
							break;

						case '}':
						case ')':
						case ']':
						case '>':
							anchor2 = curOffset - subco;
							anchor1 = FindTheOther(anchor2, fText[curOffset - subco]);
							if (anchor1 == -1)
							{
								beep();
								anchor1 = anchor2;
							}
							balanced = true;
							break;
						
						default:
							anchor1 = FindWord(curOffset, B_LEFT_ARROW);
							anchor2 = FindWord(anchor1, B_RIGHT_ARROW);
							if (anchor1 > curOffset || anchor2 < curOffset)
								anchor1 = anchor2 = curOffset;
							break;
					}
					break;
				}
				case 3:
					int line = Offset2Line(curOffset);
					anchor1 = LineStart(line);
					anchor2 = line < LineCount() - 1 ?
						LineStart(line + 1) : fText.Size();
					break;
			}

			ChangeSelection(anchor1, anchor2, fBlockSelect);
			
			if (balanced)
				return;
		}

		fStatus->SetOffset(fCaret);
		
		g_unit_t v = -1;
		BPoint cur;
		
		unsigned long buttons;
		GetMouse(&cur, &buttons);
		while (buttons)
		{
			if (where != cur || v != (fActivePart == 1 ? fVScrollBar1->Value() : fVScrollBar2->Value()))
			{
				curOffset = Position2Offset(cur, fActivePart);
				
				if (curOffset < anchor1)
				{
					if (fMouseClicks == 2)
						curOffset = FindWord(curOffset, B_LEFT_ARROW, true);
					else if (fMouseClicks == 3)
						curOffset = LineStart(Offset2Line(curOffset));
					
					ChangeSelection(max(anchor2, curOffset), min(anchor1, curOffset), modifiers & B_OPTION_KEY);
				}
				else if (curOffset > anchor2)
				{
					if (fMouseClicks == 2)
						curOffset = FindWord(curOffset, B_RIGHT_ARROW, true);
					else if (fMouseClicks == 3)
					{
						int line = Offset2Line(curOffset);
						curOffset = line < LineCount() - 1 ?
							LineStart(line + 1) : fText.Size();
					}
					
					ChangeSelection(min(anchor1, curOffset), max(anchor2, curOffset), modifiers & B_OPTION_KEY);
				}
				
				v = fActivePart == 1 ? fVScrollBar1->Value() : fVScrollBar2->Value();

				ScrollToCaret();
				fStatus->SetOffset(fCaret);
				
				where = cur;
			}
			
			snooze(20000);
			GetMouse(&cur, &buttons);
		}

		ShowCaret();
		fWalkOffset = Offset2Position(fCaret).x;
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* PText::MouseDown */

void PText::MouseMoved(BPoint where, uint32 code, const BMessage *a_message)
{
	if (!a_message)
	{
		if (fWindowActive)
		{
			if (fSplitAt > 0 && where.y >= fSplitAt - kSplitterHeight && where.y <= fSplitAt)
				be_app->SetCursor(PSplitter::Cursor());
			else if (Bounds().Contains(where))
				be_app->SetCursor(B_I_BEAM_CURSOR);
			else
				be_app->SetCursor(B_HAND_CURSOR);
		}
	}
	else if (a_message->HasData("text/plain", B_MIME_DATA) ||
			 a_message->HasRef("refs"))
	{
		switch (code)
		{
			case B_ENTERED_VIEW:
				if (!fWindowActive)
					HiliteSelection();
				fSavedAnchor = fAnchor;
				fSavedCaret = fCaret;
				fWindowActive = true;
			case B_INSIDE_VIEW:
				TrackDrag(where);
				break;
			case B_EXITED_VIEW:
				HideCaret();
				fAnchor = fSavedAnchor;
				fCaret = fSavedCaret;
				fWindowActive = Window()->IsActive();
				if (!fWindowActive)
					HiliteSelection();
				break;
		}
	}
} /* PText::MouseMoved */

void PText::TrackDrag(BPoint where)
{
	BRect b(Bounds());

	b.InsetBy(0, -10);
	
	int part = (where.y > fSplitAt) ? 2 : 1;
	SwitchPart(part);
	
	if (part == 2)
		b.top += fSplitAt;
	else
		b.bottom = b.top + fSplitAt + 20;
	
	int offset = Position2Offset(where, part);

	if (offset >= min(fSavedCaret, fSavedAnchor) &&
		offset <= max(fSavedCaret, fSavedAnchor))
	{
		HideCaret();
		fAnchor = fSavedAnchor;
		fCaret = fSavedCaret;
		
		ScrollToOffset(offset, part);
	}
	else if (offset != fCaret)
		SetCaret(offset);
	else
		ShowCaret();
} /* PText::TrackDrag */

void PText::HandleDrop(BMessage *msg)
{
	fWindowActive = Window()->IsActive();
	
	char *s = NULL;
	ssize_t sl;

	if (msg->HasData("text/plain", B_MIME_TYPE))
	{
		char *t;

		FailOSErr(msg->FindData("text/plain", B_MIME_TYPE, (const void**)&t, &sl));
		s = new char[sl];
		FailNil(s);
		memcpy(s, t, sl);
	}
	else if (msg->HasRef("refs"))
	{
		entry_ref ref;
		
		FailOSErr(msg->FindRef("refs", &ref));
		
		BFile file;
		FailOSErr(file.SetTo(&ref, B_READ_ONLY));
		
		key_info ki;
		if (get_key_info(&ki) == B_OK && ki.modifiers & (B_COMMAND_KEY | B_OPTION_KEY))
			be_app->RefsReceived(msg);
		else
		{
			char m[B_MIME_TYPE_LENGTH];
			BNodeInfo(&file).GetType(m);
			
			if (strncmp(m, "text/", 5) == 0)
			{
				sl = file.Seek(0, SEEK_END);
				file.Seek(0, SEEK_SET);
				
				s = new char[sl];
				if (file.Read(s, sl) != sl)
					THROW(("Read Error"));
			}
		}
	}

	if (s)
	{
		int offset;
		if (msg->IsSourceRemote() || msg->IsSystem() || sfDragSource != this)
			offset = -1;
		else if (fDragButtons & B_TERTIARY_MOUSE_BUTTON)
			offset = -1;
		else if (fDragButtons & B_SECONDARY_MOUSE_BUTTON)
		{
			BPopUpMenu popup("copy or move");
			popup.SetFont(be_plain_font);
			popup.AddItem(new BMenuItem("Move", NULL));
			popup.AddItem(new BMenuItem("Copy", NULL));
			BMenuItem *item = popup.Go(msg->DropPoint(), false, true);

			if (!item)
			{
				HideCaret();
				fAnchor = fSavedAnchor;
				fCaret = fSavedCaret;
				return;
			}

			if (popup.IndexOf(item) == 0)
				offset = fDragStart;
			else
				offset = -1;
		}
		else
			offset = fDragStart;

		if (offset == -1 || fCaret < offset || fCaret > offset + sl)
		{
			if (sfDragSource == this)
			{
				int a, c;
				
				a = Offset2Line(fSavedAnchor);
				c = Offset2Line(fSavedCaret);
				TouchLines(min(a, c), max(a, c));
			}
			RegisterCommand(new PDropCmd(this, s, sl, offset, fCaret));
		}
		
		if (msg->HasRef("refs"))
			delete s;
	}
} /* PText::HandleDrop */

bool PText::WaitMouseMoved(BPoint where)
{
	bigtime_t longEnough = system_time() + 250000;
	
	do
	{
		BPoint p;
		unsigned long btns;
		
		GetMouse(&p, &btns);
		
		if (!btns)
			return false;
		
		if (fabs(where.x - p.x) > 2 || fabs(where.y - p.y) > 2)
			return true;
	}
	while (system_time() < longEnough);
	
	return true;
} /* PText::WaitMouseMoved */

class CSortMenuInfo
{
  public:
	bool operator() (void * const& a, void * const& b)
	{
		return strcasecmp(((const BMenuItem*)a)->Label(), ((const BMenuItem*)b)->Label()) < 0;
	}
};

struct MenuFunctionScanHandler : public CFunctionScanHandler {
	void AddFunction(const char *name, const char *match, int offset,
		bool italic)
	{
		BMessage *msg = new BMessage(msg_JumpToProcedure);
		msg->AddInt32("offset", offset);
		msg->AddString("function", match);
		
		if (italic)
			functions.push_back(new PItalicMenuItem(name, msg));
		else
			functions.push_back(new BMenuItem(name, msg));
	}
	
	void AddInclude(const char *name, const char *open, bool italic)
	{
		BMessage *msg = new BMessage(msg_OpenInclude);
		msg->AddString("include", open);
		
		if (italic)
			includes.push_back(new PItalicMenuItem(name, msg));
		else
			includes.push_back(new BMenuItem(name, msg));
	}
	
	void AddSeparator()
	{
		functions.push_back(new BSeparatorItem);
	}

	vector<void*> includes, functions;
};

void PText::ShowFunctionMenu(BPoint where)
{
	MenuFunctionScanHandler handler;

	vector<void*>& includes = handler.includes;
	vector<void*>& functions = handler.functions;

	ScanForFunctions(handler);
	
	key_info ki;
	
	FailOSErr(get_key_info(&ki));
	
	bool optionDown = (ki.modifiers & (B_OPTION_KEY | B_SHIFT_KEY | B_COMMAND_KEY)) != 0;
	
	if (optionDown != gPrefs->GetPrefInt("sortpopup"))
	{
		sort(includes.begin(), includes.end(), CSortMenuInfo());
		sort(functions.begin(), functions.end(), CSortMenuInfo());
	}
	
#if (B_BEOS_VERSION <= B_BEOS_VERSION_4)
	// BeOS < R4.5 has no AsyncAutoDestruct, so we simulate that (kind of):
	static BPopUpMenu * popup = NULL;
	if (popup)
		delete popup;
	popup = new BPopUpMenu("Funcs");
#else
	BPopUpMenu *popup = new BPopUpMenu("Funcs");
	popup->SetAsyncAutoDestruct(true);
#endif
	popup->SetFont(be_plain_font);
	
	if (includes.size() == 0 && functions.size() == 0)
	{
		popup->AddItem(new BMenuItem("Nothing Found", NULL));
	}
	else
	{
		vector<void*>::iterator i;
		for (i = includes.begin(); i != includes.end(); i++)
		{
			popup->AddItem((BMenuItem *)*i);
		}
		
		if (i && functions.size())
			popup->AddSeparatorItem();
		
		for (i = functions.begin(); i != functions.end(); i++)
		{
			popup->AddItem((BMenuItem *)*i);
		}
	}

	BRect r(where.x - 4, where.y - 20, where.x + 24, where.y + 4);
	popup->SetTargetForItems(this);
	popup->Go(where, true, true, r, true);

	Doc()->ButtonBar()->SetDown(msg_FuncPopup, false);
} /* PText::ShowFunctionMenu */
 
void PText::ShowContextualMenu(BPoint where)
{
	BRect r;
	
	r.Set(where.x - 4, where.y - 4, where.x + 4, where.y + 4);
	ConvertToScreen(&r);
	
	if (fDragButtons & B_SECONDARY_MOUSE_BUTTON)
	{
		fMainPopUp->FindItem(msg_ToggleSyntaxColoring)->SetMarked(fSyntaxColoring);
		fMainPopUp->FindItem(B_COPY)->SetEnabled(fAnchor != fCaret);
		fMainPopUp->FindItem(B_CUT)->SetEnabled(fAnchor != fCaret);
		fMainPopUp->FindItem(msg_Clear)->SetEnabled(fAnchor != fCaret);
		fMainPopUp->FindItem(B_SELECT_ALL)->SetEnabled(abs(fCaret - fAnchor) < fText.Size());
		fMainPopUp->FindItem(msg_ToggleShowInvisibles)->SetMarked(fShowInvisibles);
		fMainPopUp->FindItem(msg_SoftWrap)->SetMarked(fSoftWrap);
	
		BFont f;
		f.SetFamilyAndStyle(gPrefs->GetPrefString("alt font family"), gPrefs->GetPrefString("alt font style"));
		f.SetSize(gPrefs->GetPrefDouble("alt font size"));
		
		if (f == fFont)
		{
			f.SetFamilyAndStyle(gPrefs->GetPrefString("font family"), gPrefs->GetPrefString("font style"));
			f.SetSize(gPrefs->GetPrefDouble("font size"));
			fMainPopUp->FindItem(msg_ToggleFont)->SetMarked(true);
		}
		else
			fMainPopUp->FindItem(msg_ToggleFont)->SetMarked(false);

		BMessage *msg = new BMessage(msg_ToggleFont);
		font_family ff;
		font_style fs;
		f.GetFamilyAndStyle(&ff, &fs);
		msg->AddString("family", ff);
		msg->AddString("style", fs);
		msg->AddFloat("size", f.Size());
		fMainPopUp->FindItem(msg_ToggleFont)->SetMessage(msg);
	
		be_clipboard->Lock();
		fMainPopUp->FindItem(B_PASTE)->SetEnabled(be_clipboard->Data()->HasData("text/plain", B_MIME_DATA));
		be_clipboard->Unlock();
		
		fMainPopUp->Go(ConvertToScreen(where), true, true, r, true);
	}
	else
		ShowFunctionMenu(ConvertToScreen(where));
} /* PText::ShowContextualMenu */

#pragma mark - Lines

void PText::OffsetLineBreaks(int bytes, int line)
{
	for (int i = line; i < LineCount(); i++)
		fLineInfo[i].start += bytes;
} /* PText::OffsetLineBreaks */

int PText::DeleteLineBreaks(int from, int to)
{
	ASSERT (to > from);
	
	int lFrom = Offset2Line(from);
	int lTo = Offset2Line(to);

	ASSERT(lTo < LineCount());

	if (lFrom == lTo)
		OffsetLineBreaks(from - to, lFrom + 1);
	else
	{
		VLineInfo::iterator ilf, ilt;
		
		if (lFrom + 1 >= LineCount())
			return 0;
		
		ilf = fLineInfo.begin() + lFrom + 1;
		if (lTo + 1 > LineCount())
			ilt = fLineInfo.end();
		else
			ilt = fLineInfo.begin() + lTo + 1;
		
		fLineInfo.erase(ilf, ilt);
		OffsetLineBreaks(from - to, lFrom + 1);
	}
	
	return lTo - lFrom;
} /* PText::DeleteLineBreaks */

void PText::InsertLinebreaks()
{
	int a, c;
	
	if (fAnchor == fCaret)
	{
		a = 0;
		c = fText.Size();
	}
	else
	{
		a = min(fAnchor, fCaret);
		c = max(fAnchor, fCaret);
	}
	
	int i = a;
	
	while (true)
	{
		int line = Offset2Line(i) + 1;
		
		if (line == LineCount() || LineStart(line) > c)
			break;

		i = LineStart(line);
		
		if (! fLineInfo[line].nl)
		{
			if (fText[i - 1] == ' ')
				fText.Replace(i - 1, "\n");
			else
			{
				fText.Insert("\n", 1, i);
				OffsetLineBreaks(1, line);
				i++;
				c++;
			}
		}
	}
	
	RecalculateLineBreaks();
	
	if (fAnchor > fCaret)
		fAnchor = c;
	else if (fAnchor < fCaret)
		fCaret = c;
	
	SetDirty(true);
} /* PText::InsertLinebreak */

void PText::RemoveLinebreaks()
{
	int a, c;
	
	if (fAnchor == fCaret)
	{
		a = 0;
		c = fText.Size();
	}
	else
	{
		a = min(fAnchor, fCaret);
		c = max(fAnchor, fCaret);
	}
	
	for (int i = a; i < c; i++)
		if (fText[i] == '\n')
		{
			if (fText[i + 1] == '\n')
			{
				i++;
				continue;
			}
			
			int d = i + 1;
			while (d < fText.Size() - 1 && (fText[d] == ' ' || fText[d] == '\t'))
				d++;

			Delete(i, d);
			Insert(" ", 1, i);
			c -= (d - i - 1);
		}
	
	if (fAnchor > fCaret)
		fAnchor = c;
	else if (fAnchor < fCaret)
		fCaret = c;

	SetDirty(true);
} /* PText::RemoveLinebreaks */

int PText::Offset2Line(int offset)
{
	int l = 0, r = LineCount() - 1, i;
	
	if (offset > fText.Size())
		return r;
	
	while (l <= r)
	{
		i = (l + r) / 2;
		int lb = LineStart(i);

		if (offset == lb)
			return i;
		else if (offset < lb)
			r = i - 1;
		else
			l = i + 1;
	}
	
	return max(0, r);
} /* PText::Offset2Line */

BPoint PText::Offset2Position(int offset, int part)
{
	int line = Offset2Line(offset);
	
	if (part < 0) part = fActivePart;
	
	float x = 0;
	int o = offset - LineStart(line);
	int s = LineStart(line);
	
	while (o > 0)
	{
		int cl;
		if (fText[s] == '\t')
		{
			cl = 1;
			int t = (int)floor(x / fTabWidth) + 1;
			x = (Round(t * fTabWidth) > Round(x) ? t * fTabWidth : (t + 1) * fTabWidth);
		}
		else
		{
			cl = fText.CharLen(s);
			if (cl == 1)
			{
				if (fText[s] >= 0 && iscntrl(fText[s]))
					x += StringWidth(gControlChar, strlen(gControlChar));
				else
				{
					char c[2] = { fText[s], 0 };
					x += fMetrics->CharWidth(c);
				}
			}
			else
				x += TextWidth(s, cl);
		}
		
		s += cl;
		o -= cl;
	}
	
	BPoint p;
	p.x = x + 2;
	if (part == 1)
		p.y = line * fLineHeight - fVScrollBar1->Value();
	else
		p.y = line * fLineHeight - fVScrollBar2->Value() + fSplitAt;
	return p;
} /* PText::Offset2Position */

int PText::LinePosition2Offset(int line, g_unit_t position)
{
	position -= 3;

	if (position < 0) return 0;
	
	int l = LineStart(line);
	int m = (line < LineCount() - 1) ? LineStart(line + 1) - 1 : fText.Size();
	
	float x = 0, lx = 0;
	int o = 0;
	int s = l, cl = 0;

	while (o + l < m && x < position)
	{
		if (fText[s] == '\t')
		{
			cl = 1;
			int t = (int)floor(x / fTabWidth) + 1;
			lx = t * fTabWidth - x;
			x = ceil(t * fTabWidth);
		}
		else
		{
			cl = fText.CharLen(s);

			if (cl == 1)
			{
				if (fText[s] >= 0 && iscntrl(fText[s]))
					lx = StringWidth(gControlChar, strlen(gControlChar));
				else
				{
					char c[2] = { fText[s], 0 };
					lx = (*fMetrics)[c];
				}
			}
			else
				lx = TextWidth(s, cl);

			x += lx;
		}
		
		s += cl;
		o += cl;
	}
	
	if (x > position)
	{
		if (gBlockCursor)
			o--;
		else
		{
			float dx = x - position;
			if (dx > lx / 2)
				o -= cl;
		}
	}

	return o;
} /* PText::LinePosition2Offset */

int PText::Position2Offset(BPoint where, int part)
{
	int line;
	
	if (part == -1)
	{
		if (where.y >= fSplitAt)
			part = 2;
		else
			part = 1;
	}
	
	if (part == 2)
		line = max(0, (int)floor((where.y - fSplitAt + fVScrollBar2->Value()) / fLineHeight));
	else
		line = max(0, (int)floor((where.y + fVScrollBar1->Value()) / fLineHeight));
	
	if (line > LineCount() - 1)
		line = LineCount() - 1;

	return LineStart(line) + LinePosition2Offset(line, where.x);
} /* PText::Position2Offset */

int PText::Offset2Column(int offset)
{
	int line = Offset2Line(offset);
	int col = 0, i = LineStart(line);

	while (i < offset)
	{
		if (fText[i] == '\t')
		{
			col = ((col / fTabStops) + 1) * fTabStops;
			i++;
		}
		else
		{
			col++;
			i += fText.CharLen(i);
		}
	}
	
	return col;
} /* PText::Offset2Column */

int PText::Column2Offset(int lineNr, int column)
{
	int max, c = 0, result;
	result = lineNr < LineCount() ? LineStart(lineNr) : fText.Size();
	
	if (lineNr < LineCount() - 1)
		max = LineStart(lineNr + 1) - 1;
	else
		max = fText.Size();
	
	while (result < max && c < column)
	{
		if (fText[result] == '\t')
		{
			result++;
			c = ((c / fTabStops) + 1) * fTabStops;
		}
		else
		{
			result += fText.CharLen(result);
			c++;
		}
	}
	
	return result;
} // PText::Column2Offset

int PText::RealLine2Line(int lineNr)
{
	if (fSoftWrap)
	{
		int i = 0;
		
		while (++i < lineNr && i < LineCount())
		{
			while (! fLineInfo[i].nl && i < LineCount() - 1)
				i++, lineNr++;
		}
		
		while (lineNr < LineCount() && ! fLineInfo[lineNr].nl)
			lineNr++;
	}

	return lineNr;
} /* PText::RealLine2Line */

g_unit_t PText::TextWidth(int offset, int len) const
{
	if (offset + len > fText.Size())
		len = fText.Size() - offset;
	
	float w = 0;

	if (len > 0)
	{
		CAlloca txt(len + 1);
		fText.Copy(txt, offset, len);
		txt[len] = 0;

		w = StringWidth(txt, len);
	}

	return w;
} /* PText::TextWidth */

void PText::Selection2Region(BRegion& rgn, int part)
{
	rgn.MakeEmpty();
	BRect clip(Bounds());
	
	if (part < 0) part = fActivePart;
	
	if (part == 1)
		clip.bottom = fSplitAt - kSplitterHeight - 1;
	else
		clip.top = fSplitAt;

	if (fAnchor == fCaret)
		return;

	int first, last, firstLine, lastLine;
	first = min(fAnchor, fCaret);
	last = max(fAnchor, fCaret);
	
	firstLine = Offset2Line(first);
	lastLine = Offset2Line(last);
	
	BPoint p1, p2;
	
	p1 = Offset2Position(first, part);
	p1.x = max(3, (int)Round(p1.x));
	p1.y = Round(p1.y);
	p2 = Offset2Position(last, part);
	p2.x = Round(p2.x);
	p2.y = Round(p2.y);

	if (p1.y == p2.y)
	{
		BRect r(p1.x, p1.y, p2.x, p1.y + fLineHeight);
		if (clip.Intersects(r))
			rgn.Include(r & clip);
	}
	else if (fBlockSelect)
	{
		BRect r(p1.x, p1.y, p2.x, p2.y + fLineHeight);
		if (clip.Intersects(r))
			rgn.Include(r & clip);
	}
	else
	{
		BRect r, b(Bounds());
		
		int lines = Offset2Line(last) - Offset2Line(first) - 1;
		
		r.Set(p1.x, p1.y, b.right, p1.y + fLineHeight);
		if (clip.Intersects(r))
			rgn.Include(r & clip);
		
		r.left = 3;
		r.OffsetBy(0, fLineHeight);
		while (lines--)
		{
			if (clip.Intersects(r))
				rgn.Include(r & clip);
			r.OffsetBy(0, fLineHeight);
		}
		
		r.right = p2.x;
		if (clip.Intersects(r))
			rgn.Include(r & clip);
	}
} /* PText::Selection2Region */

void PText::TouchLine(int lineNr)
{
	ASSERT(lineNr >= 0);
	ASSERT(lineNr < LineCount());
	
	if (lineNr >= 0 && lineNr < LineCount())
		fLineInfo[lineNr].dirty = true;
} /* PText::TouchLine */

void PText::TouchLines(int fromLine, int toLine)
{
	if (toLine == -1 || toLine >= LineCount())
		toLine = LineCount() - 1;
	
	ASSERT(fromLine >= 0);
	if (fromLine < 0) fromLine = 0;
	
	for (int i = fromLine; i <= toLine; i++)
		fLineInfo[i].dirty = true;
} /* PText::TouchLines */

void PText::RestyleDirtyLines(int from)
{
	for (int i = from; i < LineCount(); i++)
	{
		if (fLineInfo[i].dirty)
		{
			int state = (i > 0 ? fLineInfo[i].state : 0);
			
			while (fLineInfo[i].dirty && ++i < LineCount())
			{
				int len = fLineInfo[i].start - fLineInfo[i - 1].start;
				
				CAlloca txt(len + 1);
				fText.Copy(txt, fLineInfo[i - 1].start, len);
				txt[len] = 0;
				
				fLangIntf->ColorLine(txt, len, state, NULL, NULL);

				if (state != fLineInfo[i].state)
				{
					fLineInfo[i].dirty = true;
					fLineInfo[i].state = state;
				}
			}
		}
	}
} /* PText::RestyleDirtyLines */

void PText::SelectParagraph()
{
	const char *txt = Text();
	size_t size = Size();
	int i, j;
	
	i = min(fAnchor, fCaret);
	j = max(fAnchor, fCaret);
	
	if (fSoftWrap)
	{
		while (i > 0)
		{
			if (i > 1 && txt[i - 1] == '\n')
				break;
			i--;
		}
		
		while (j < size - 1)
		{
			if (txt[j] == '\n')
				break;
			j++;
		}
	}
	else
	{
		while (i > 0)
		{
			if (i > 1 && txt[i - 1] == '\n' && (i < 2 || txt[i - 2] == '\n'))
				break;
			i--;
		}
		
		while (j < size - 1)
		{
			if (txt[j] == '\n' && (j > size - 2 || txt[j + 1] == '\n'))
				break;
			j++;
		}
	}
		
	ChangeSelection(i, j);
} /* PText::SelectParagraph */

void PText::BlockOffsetsForLine(int lineNr, int& startOffset, int& endOffset)
{
	int s, e, c, ls;
	
	s = Offset2Column(fAnchor);
	e = Offset2Column(fCaret);
	
	if (e < s) swap(e, s);
	
	c = 0;
	ls = LineStart(lineNr);
	startOffset = ls;
	
	while (c < s)
	{
		if (fText[startOffset] == '\t')
		{
			startOffset += 1;
			c = ((c / fTabStops) + 1) * fTabStops;
		}
		else
		{
			startOffset += fText.CharLen(startOffset);
			c++;
		}
	}
	
	endOffset = startOffset;
	
	while (c < e)
	{
		if (fText[endOffset] == '\t')
		{
			endOffset += 1;
			c = ((c / fTabStops) + 1) * fTabStops;
		}
		else
		{
			endOffset += fText.CharLen(endOffset);
			c++;
		}
	}
} // PText::BlockOffsetsForLine

#pragma mark - Language

int PText::FindWord(int key)
{
	return FindWord(fCaret, key);
} /* PText::FindWord */

int PText::FindWord(int i, int key, bool mouseSelect)
{
	int unicode, len, uClass = -1;
	bool skip = ! mouseSelect;
	
	if (key == B_RIGHT_ARROW)
	{
		while (i < fText.Size())
		{
			fText.CharInfo(i, unicode, len);
			
			if (uClass == -1)
				uClass = mclass(unicode);

			if ((fText[i] != '_' && !isalnum_uc(unicode)) == skip && (skip || uClass == mclass(unicode)))
				i += len;
			else if (skip)
				skip = false;
			else
				break;
		}
	}
	else if (key == B_LEFT_ARROW)
	{
		while (i > 0)
		{
			i -= fText.PrevCharLen(i);
			fText.CharInfo(i, unicode, len);

			if (uClass == -1)
				uClass = mclass(unicode);

			if ((fText[i] != '_' && !isalnum_uc(unicode)) == skip && (skip || uClass == mclass(unicode)))
				;
			else if (skip)
				skip = false;
			else
			{
				i += len;
				break;
			}
		}
	}
	
	return i;
} /* PText::FindWord */

int PText::FindTheOther(int offset, int ch)
{
	int start, end;

	start = end = offset;
	
	if (fLangIntf->Balance(*this, start, end))
	{
		if (fText[start - 1] == ch)
			return end;
		else if (fText[end] == ch)
			return start;
	}

	return -1;
} /* PText::FindTheOther */

void PText::Kiss(int ch)
{
	int pp = FindTheOther(fCaret - 1, ch);
	
	if (pp == -1)
		beep();
	else
	{
		int a, c;

		a = fAnchor;
		c = fCaret;
	
		float curPos = fActivePart == 1 ? fVScrollBar1->Value() : fVScrollBar2->Value();

		ChangeSelection(pp - 1, pp);
		ScrollToCaret();
		snooze(150000);
		ChangeSelection(a, c);

		if (fActivePart == 1)
			fVScrollBar1->SetValue(curPos);
		else
			fVScrollBar2->SetValue(curPos);
	}
} /* PText::Kiss */

void PText::AutoIndent(int offset)
{
	PTypingCmd *tc = dynamic_cast<PTypingCmd*>(fLastCommand);
	FailNil(tc);

	int line, ls, i, pp;
	char buf[256];

	line = Offset2Line(offset);
	ls = LineStart(line);
	pp = min(offset - ls, 255);
	i = 0;

	while ((fText[ls + i] == ' ' || fText[ls + i] == '\t') && i < pp)
	{
		buf[i] = fText[ls + i];
		i++;
	}

	if (gSmartBrace && offset>0 && fText[offset - 1] == '{')
		buf[i++] = '\t';

	if (i > 0)
	{
		Insert(buf, i, fCaret);
		fAnchor = fCaret += i;
		TouchLine(line + 1);
		
		tc->fInsertedLen += i;
	}
} /* PText::AutoIndent */

void PText::SmartBrace()
{
	int open, close;
	
	close = fCaret - 1;
	open = FindTheOther(close, '}');
	
	if (open >= 0)
	{
		int openLine, closeLine;
		
		openLine = Offset2Line(open);
		closeLine = Offset2Line(close);

		if (openLine == closeLine)
			return;
		
		char buf[128];
		int oWhite = 0, i = LineStart(openLine);
		
		while (isspace(fText[i]) && oWhite < 127)
			buf[oWhite++] = fText[i++];
		
		i = LineStart(closeLine);
		while (isspace(fText[i]))
			i++;
		
		if (i < close)
		{
			if (oWhite == 0)
			{
				BackspaceKeyDown();
				TypeString("\n}");
			}
			else
			{
				memmove(buf + 1, buf, oWhite);
				buf[0] = '\n';
				fAnchor = --fCaret;
				CharKeyDown(buf, oWhite + 1);
				fAnchor = ++fCaret;
			}
		}
		else
		{
			fLastCommand = NULL;
	
			fAnchor = LineStart(closeLine);
			fCaret = close;
			
			if (oWhite || fCaret != close)
				CharKeyDown(buf, oWhite);
			else if (fAnchor < fCaret)
				BackspaceKeyDown();
			
			fAnchor = ++fCaret;
		}
	}
} /* PText::SmartBrace */

void PText::ScanForFunctions(CFunctionScanHandler& handler)
{
	fLangIntf->ScanForFunctions(*this, handler);
} /* PText::ScanForFunctions */

void PText::HashLines(vector<int>& hv, bool ignoreCase, bool ignoreWhite)
{
	unsigned int i = 0, h = 0;
	char c;

	while (i < fText.Size())
	{
		while ((c = fText[i++]) != '\n' && i < fText.Size())
		{
			if (! (ignoreWhite && isspace(c)))
				h = Hash(h, (unsigned int)(unsigned char)(ignoreCase && isupper(c) ? tolower(c) : c));
		}
		
		hv.push_back(h);
		h = 0;
	}
} /* PText::HashLines */

#pragma mark - Keys

void PText::KeyDown(const char *bytes, int32 numBytes)
{
	try
	{
		long modifiers, key, ch;

		HideCaret();
		be_app->ObscureCursor();

		FailOSErr(Looper()->CurrentMessage()->FindInt32("modifiers", &modifiers));
		FailOSErr(Looper()->CurrentMessage()->FindInt32("key", &key));
		FailOSErr(Looper()->CurrentMessage()->FindInt32("raw_char", &ch));
		
		if (fIncSearch)
		{
			IncSearchKey(bytes, numBytes);
			return;
		}
		
		switch (bytes[0])
		{
			case B_FUNCTION_KEY:
				if (Doc()->IsReadOnly()) THROW(("Document is read-only"));
				
				key |= 0x0100;

				if (gGlossary->IsGlossaryShortcut(key, modifiers))
				{
					GlossaryKey(key, modifiers);
					ScrollToCaret();
				}
				break;
			
			case B_BACKSPACE:
				BackspaceKeyDown();
				ScrollToCaret();
				break;
			
			case B_DELETE:
				DeleteKeyDown();
				ScrollToCaret();
				break;
			
			case B_ESCAPE:
			case B_INSERT:
				break;
			
			default:
			{
				if (Doc()->IsReadOnly()) THROW(("Document is read-only"));

				if (gGlossary->IsGlossaryShortcut(ch, modifiers))
				{
					GlossaryKey(ch, modifiers);
				}
				else if (numBytes > 1)
					CharKeyDown(bytes, numBytes);
				else if (ch != B_FUNCTION_KEY &&
						 !(modifiers & (B_CONTROL_KEY | B_COMMAND_KEY))
//						 (!iscntrl(bytes[0]) || ch == B_RETURN || ch == B_TAB))
						// [zooey]: does anyone happen to know what above line
						//          were meant to do? I deactivated it as it busts
						//          entering non-ASCII characters in encodings other
						//          than UTF8.
				)
				{
					CharKeyDown(bytes, numBytes);
				}
				else if (modifiers & B_NUM_LOCK && (isdigit(bytes[0]) || ispunct(bytes[0])))
				{
					CharKeyDown(bytes, numBytes);
				}
				
				ScrollToCaret();
				break;
			}
		}
		
		if (fCaret == fAnchor)
			ShowCaret();
		
		fStatus->SetOffset(fCaret);
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* PText::KeyDown */

bool PText::DoKeyCommand(BMessage *msg)
{
	unsigned long what = msg->what;

	if (fIncSearch && what == kmsg_Delete_Character_Left)
	{
		char s = B_BACKSPACE;
		IncSearchKey(&s, 1);
		return true;
	}
	
	float h = Bounds().Height();
	BScrollBar *bar = fActivePart == 1 ? fVScrollBar1 : fVScrollBar2;
	float barValue = bar->Value();

	int line = Offset2Line(fCaret);
	int newCaret = fCaret, newAnchor = fAnchor;
	int linesPerPage = (int)floor((fActivePart == 1 ? fSplitAt : h - fSplitAt) / fLineHeight) - 1;
	int topline = (int)floor(barValue / fLineHeight);
	int lastMark = fMark;
	
	bool scroll = true, handled = true, extend = fCaret != fAnchor, catchOffset = true;
	bool clearLastCommand = true;
	
	switch (what)
	{
		case kmsg_Move_Character_Left:
			if (extend)
				newAnchor = newCaret = min(fCaret, fAnchor);
			else
				newAnchor = newCaret = max(0, fCaret - fText.PrevCharLen(fCaret));
			break;
		case kmsg_Move_Character_Right:
			if (extend)
				newAnchor = newCaret = max(fCaret, fAnchor);
			else
				newAnchor = newCaret = min(fCaret + fText.CharLen(fCaret), fText.Size());
			break;
		case kmsg_Move_Word_Left:
			newAnchor = newCaret = FindWord(B_LEFT_ARROW);
			break;
		case kmsg_Move_Word_Right:
			newAnchor = newCaret = FindWord(B_RIGHT_ARROW);
			break;
		case kmsg_Move_to_Beginning_of_Line:
		case kmsg_Extend_Selection_to_Beginning_of_Line:
		{
			int ls = LineStart(line);
			
			if (gPrefs->GetPrefInt("althome", 1))
			{
				int w = ls;

				while (fText[w] == ' ' || fText[w] == '\t')
					w++;
				
				if (fCaret > w || fCaret == ls)
					newCaret = w;
				else
					newCaret = ls;
			}
			else
				newCaret = ls;
				
			if (what == kmsg_Move_to_Beginning_of_Line)
				newAnchor = newCaret;
			else
				extend = true;
			break;
		}
		case kmsg_Move_to_End_of_Line:
			if (line < LineCount() - 1)
			{
				newCaret = LineStart(line + 1);
				newAnchor = newCaret -= fText.PrevCharLen(newCaret);
			}
			else
				newAnchor = newCaret = fText.Size();
			break;
		case kmsg_Move_to_Previous_Line:
			if (extend)
				newAnchor = newCaret = min(fCaret, fAnchor);
			else if (line > 0)
				newAnchor = newCaret = LineStart(line - 1) + LinePosition2Offset(line - 1, fWalkOffset);
			else
				newAnchor = newCaret = 0;
			catchOffset = extend;
			break;
		case kmsg_Move_to_Next_Line:
			if (extend)
				newAnchor = newCaret = max(fCaret, fAnchor);
			else if (line < LineCount() - 1)
				newAnchor = newCaret = LineStart(line + 1) + LinePosition2Offset(line + 1, fWalkOffset);
			else
				newAnchor = newCaret = fText.Size();
			catchOffset = extend;
			break;
		case kmsg_Move_to_Top_of_Page:
			if (line > topline + 1)
				line = topline;
			else
				line = max(0, line - linesPerPage);
			newAnchor = newCaret = LineStart(line) + LinePosition2Offset(line, fWalkOffset);
			catchOffset = false;
			break;
		case kmsg_Move_to_Bottom_of_Page:
			if (line + 1 < topline + linesPerPage)
				line = min(LineCount() - 1, topline + linesPerPage);
			else
				line = min(LineCount() - 1, line + linesPerPage);
			newAnchor = newCaret = LineStart(line) + LinePosition2Offset(line, fWalkOffset);
			catchOffset = false;
			break;
		case kmsg_Move_to_Beginning_of_File:
			newAnchor = newCaret = 0;
			break;
		case kmsg_Move_to_End_of_File:
			newAnchor = newCaret = fText.Size();
			break;
		case kmsg_Delete_Character_Left:
			clearLastCommand = false;
			BackspaceKeyDown();
			newAnchor = newCaret = fCaret;
			break;
		case kmsg_Delete_Character_Right:
			clearLastCommand = false;
			DeleteKeyDown();
			newAnchor = newCaret = fCaret;
			break;
		case kmsg_Delete_to_End_of_Line:
			if (fAnchor == fCaret)
			{
				if (line < LineCount() - 1)
					fCaret = LineStart(line + 1) - fText.PrevCharLen(LineStart(line + 1));
				else
					fCaret = fText.Size();
			}
			DeleteKeyDown();	
			newAnchor = newCaret = fCaret;
			break;
		case kmsg_Delete_to_End_of_File:
			if (fAnchor == fCaret)
				fCaret = fText.Size();
			DeleteKeyDown();
			newAnchor = newCaret = fCaret;
			break;
		case kmsg_Extend_Selection_with_Character_Left:
			extend = true;
			newCaret = max(0, fCaret - fText.PrevCharLen(fCaret));
			break;
		case kmsg_Extend_Selection_with_Character_Right:
			extend = true;
			newCaret = min(fCaret + fText.CharLen(fCaret), fText.Size());
			break;
		case kmsg_Extend_Selection_with_Previous_Word:
			extend = true;
			newCaret = FindWord(B_LEFT_ARROW);
			break;
		case kmsg_Extend_Selection_with_Next_Word:
			extend = true;
			newCaret = FindWord(B_RIGHT_ARROW);
			break;
		case kmsg_Extend_Selection_to_Current_Line:
			ChangeSelection(LineStart(line), line < LineCount() - 1 ? LineStart(line + 1) - 1 : fText.Size());
			break;
		case kmsg_Extend_Selection_to_Previous_Line:
			extend = true;
			if (line > 0)
				newCaret = LineStart(line - 1) + LinePosition2Offset(line - 1, fWalkOffset);
			else
				newCaret = 0;
			catchOffset = false;
			break;
		case kmsg_Extend_Selection_to_Next_Line:
			extend = true;
			if (line < LineCount() - 1)
				newCaret = LineStart(line + 1) + LinePosition2Offset(line + 1, fWalkOffset);
			else
				newCaret = fText.Size();
			catchOffset = false;
			break;
		case kmsg_Extend_Selection_to_End_of_Line:
			extend = true;
			if (line < LineCount() - 1)
				newCaret = LineStart(line + 1) - fText.PrevCharLen(LineStart(line + 1));
			else	
				newCaret = fText.Size();
			break;
		case kmsg_Extend_Selection_to_Beginning_of_Page:
			extend = true;
			if (line > topline)
			{
				// Move caret to the top of the page, unless the current
				// selection extends beyond the top and the caret is located
				// at the beginning of the second visible line. We do this
				// to work around a feature of ScrollToOffset() -- when the
				// caret is at the beginning of the line, it scrolls such
				// that the last line of the selection is visible.
				if (line == topline + 1 && fCaret > fAnchor
					&& fCaret == LineStart(line)
					&& Offset2Line(fAnchor) < topline)
					line = max(0, line - linesPerPage);
				else
					line = topline;
			}
			else
				line = max(0, line - linesPerPage);
			newCaret = LineStart(line) + LinePosition2Offset(line, fWalkOffset);
			catchOffset = false;
			break;
		case kmsg_Extend_Selection_to_End_of_Page:
			extend = true;
			if (line + 1 < topline + linesPerPage)
				line = min(LineCount() - 1, topline + linesPerPage);
			else
				line = min(LineCount() - 1, line + linesPerPage);
			newCaret = LineStart(line) + LinePosition2Offset(line, fWalkOffset);
			catchOffset = false;
			break;
		case kmsg_Extend_Selection_to_Beginning_of_File:
			extend = true;
			newCaret = 0;
			break;
		case kmsg_Extend_Selection_to_End_of_File:
			extend = true;
			newCaret = fText.Size();
			break;
		case kmsg_Scroll_One_Line_Up:
			bar->SetValue(barValue - fLineHeight);
			scroll = false;
			catchOffset = false;
			break;
		case kmsg_Scroll_One_Line_Down:
			bar->SetValue(barValue + fLineHeight);
			scroll = false;
			catchOffset = false;
			break;
		case kmsg_Scroll_Page_Up:
			bar->SetValue(barValue - linesPerPage * fLineHeight);
			scroll = false;
			catchOffset = false;
			break;
		case kmsg_Scroll_Page_Down:
			bar->SetValue(barValue + linesPerPage * fLineHeight);
			scroll = false;
			catchOffset = false;
			break;
		case kmsg_Scroll_to_Start_of_File:
			bar->SetValue(0);
			scroll = false;
			catchOffset = false;
			break;
		case kmsg_Scroll_to_End_of_File:
		{
			float vmin, vmax;
			bar->GetRange(&vmin, &vmax);
			bar->SetValue(vmax);
			scroll = false;
			catchOffset = false;
			break;
		}
		case kmsg_OpenLine:
			fLastCommand = NULL;
			TypeString("\n");
			newAnchor = newCaret;
			break;
		case kmsg_Mark:
			SetMark(newCaret);
			clearLastCommand = false;
			break;
		case kmsg_Recenter:
			bar->SetValue((line - linesPerPage / 2) * fLineHeight);
			scroll = false;
			catchOffset = false;
			TouchLines(0);
			RedrawDirtyLines();
			break;
		case kmsg_ExchangeMarkAndPoint:
			if (fMark == -1) THROW(("Mark not set"));
			newCaret = fMark;
			if (fAnchor != fCaret)
				newAnchor = newCaret;
			SetMark(fCaret);
			break;
		case kmsg_CutRegion:
			if (fMark == -1) THROW(("Mark not set"));
			fAnchor = fMark;
			RegisterCommand(new PCutCmd(this));
			fMark = newCaret = newAnchor = fCaret;
			break;
		case kmsg_CopyRegion:
			if (fMark == -1) THROW(("Mark not set"));
			fAnchor = fMark;
			Copy();
			break;
		case kmsg_ClearRegion:
			if (fMark == -1) THROW(("Mark not set"));
			fAnchor = fMark;
			RegisterCommand(new PClearCmd(this));
			fMark = newCaret = newAnchor = fCaret;
			break;
		case kmsg_MarkAll:
			newCaret = newAnchor = 0;
			SetMark(fText.Size());
			break;
		case kmsg_MarkWord:
			if (fNrArgument > 0 && fNrArgument < 10)
			{
				while (fNrArgument--)
					fCaret = FindWord(B_RIGHT_ARROW);
				SetMark(fCaret);
			}
			else
				SetMark(FindWord(B_RIGHT_ARROW));
			fCaret = newCaret;
			break;
		case kmsg_NrArgument:
			FailOSErr(msg->FindInt32("Nr Argument", (long *)&fNrArgument));
			break;
		case kmsg_Cut_Word:
			fAnchor = fCaret;
			if (fNrArgument > 0 && fNrArgument < 10)
			{
				while (fNrArgument--)
					fCaret = FindWord(B_RIGHT_ARROW);
			}
			else
				fCaret = FindWord(B_RIGHT_ARROW);
			RegisterCommand(new PCutCmd(this, fLastKillPoint == fAnchor));
			fLastKillPoint = fAnchor;
			break;
		case kmsg_Cut_Word_Backward:
			fAnchor = fCaret;
			if (fNrArgument > 0 && fNrArgument < 10)
			{
				while (fNrArgument--)
					fCaret = FindWord(B_LEFT_ARROW);
			}
			else
				fCaret = FindWord(B_LEFT_ARROW);
			RegisterCommand(new PCutCmd(this, (fLastKillPoint == fAnchor || fAppendNextCut) ? 2 : 0));
			fLastKillPoint = newAnchor = newCaret = fCaret;
			break;
		case kmsg_Cut_to_End_of_Line:
			fAnchor = fCaret;
			if (fNrArgument > 0 && fNrArgument < 10)
			{
				line = min(LineCount() - 1, line + fNrArgument - 1);
				fCaret = line < LineCount() - 1 ? LineStart(line + 1) : fText.Size();
			}
			else if (line < LineCount() - 1 && fCaret == LineStart(line + 1) - 1)
				fCaret = LineStart(line + 1);
			else
				fCaret = line < LineCount() - 1 ? LineStart(line + 1) - 1 : fText.Size();
			RegisterCommand(new PCutCmd(this, fLastKillPoint == fAnchor));
			fLastKillPoint = fAnchor;
			break;
		case kmsg_AppendNextCut:
			fAppendNextCut = true;
			break;
			
		default:
			handled = false;
			scroll = false;
			clearLastCommand = false;
			break;
	}

	if (what != kmsg_NrArgument)
		fNrArgument = 0;

	if (clearLastCommand)
		fLastCommand = NULL;

	if (fIncSearch && handled)
	{
		fIncSearch = 0;
		Doc()->ButtonBar()->SetDown(msg_IncSearchBtn, false);
	}

	if (extend)
		ChangeSelection(newAnchor, newCaret, fBlockSelect);
	else if (newCaret != fCaret)
		SetCaret(newCaret);

	if (catchOffset)
		fWalkOffset = Offset2Position(fCaret).x;

	if (scroll)
		ScrollToCaret();
	
	if (lastMark != fMark)
		RedrawDirtyLines();

	if (fLastKillPoint != fAnchor)
		fLastKillPoint = -1;

	return handled;
} /* PText::DoKeyCommand */

void PText::GlossaryKey(int ch, int modifiers)
{
	fLastCommand = NULL;
	
	char *s, *g, lws[256];
	GetSelectedText(s);
	
	int i, j = 0, line, offset;
	
	offset = min(fAnchor, fCaret);
	line = Offset2Line(offset);
	i = LineStart(line);

	while (i < offset && isspace(fText[i]) && j < 255)
	{
		lws[j++] = fText[i++];
	}
	lws[j] = 0;
	
	gGlossary->GetGlossary(ch, modifiers, s, Window()->Title(), lws, g, i);
	i += min(fAnchor, fCaret);
	
	if (g)
	{
		TypeString(g);
		free(g);
		SetCaret(i);
	}
	
	if (s) free(s);
} /* PText::GlossaryKey */

void PText::GlossaryButton(const char *glossy)
{
	fLastCommand = NULL;
	
	char *s, *g, lws[256];
	GetSelectedText(s);
	
	int i, j = 0, line, offset;
	
	offset = min(fAnchor, fCaret);
	line = Offset2Line(offset);
	i = LineStart(line);

	while (i < offset && isspace(fText[i]) && j < 255)
	{
		lws[j++] = fText[i++];
	}
	lws[j] = 0;
	
	gGlossary->ReplaceGlossary(glossy, s, Window()->Title(), lws, g, i);
	i += min(fAnchor, fCaret);
	
	if (g)
	{
		TypeString(g);
		free(g);
		SetCaret(i);
	}
	
	if (s) free(s);
} /* PText::GlossaryButton */

#pragma mark - Find

void PText::Find(unsigned long msgWhat, void *args)
{
	const char *what, *with;
	bool wrap, ignoreCase, backward, word, grep;
	
	if (args)
	{
		BMessage *msg = (BMessage *)args;
		
		msg->FindString("what", &what);
		msg->FindString("with", &with);
		
		msg->FindBool("wrap", &wrap);
		msg->FindBool("case", &ignoreCase);
		msg->FindBool("back", &backward);
		msg->FindBool("word", &word);
		msg->FindBool("grep", &grep);
	
		PDoc *w = (PDoc *)Window();
		w->Activate(true);
	}
	else
	{
		what = strdup(gFindDialog->FindString());
		with = strdup(gFindDialog->ReplaceString());
		wrap = gFindDialog->Wrap();
		ignoreCase = gFindDialog->IgnoreCase();
		backward = gFindDialog->Backward();
		word = gFindDialog->Word();
		grep = gFindDialog->Grep();
	}

	if (*what != 0)
	{
		int offset;
		
		switch (msgWhat)
		{
			case msg_Find:
				offset = backward ? min(fCaret, fAnchor) : max(fCaret, fAnchor);
				FindNext((unsigned char *)what, offset, ignoreCase, wrap, backward, word, grep, true);
				break;
	
			case msg_FindAgain:
				offset = backward ? min(fCaret, fAnchor) : max(fCaret, fAnchor);
				if (gFindDialog->IsInMultiFileState())
				{
					if (!FindNext((unsigned char *)what, offset, ignoreCase, false, false, word, grep, true))
						gFindDialog->PostMessage(msg_FindInNextFile);
				}
				else
					FindNext((unsigned char *)what, offset, ignoreCase, wrap, backward, word, grep, true);
				break;
	
			case msg_FindAgainBackward:
				offset = backward ? max(fCaret, fAnchor) : min(fCaret, fAnchor);
				FindNext((unsigned char *)what, offset, ignoreCase, wrap, !backward, word, grep, true);
				break;
				
			case msg_Replace:
				if (CanReplace(what, ignoreCase, grep))
					RegisterCommand(new PReplaceCmd(this, min(fCaret, fAnchor), abs(fCaret - fAnchor),
						false, gFindDialog->Backward()));
				else
					Window()->PostMessage(msg_FindCmd);
				break;
			
			case msg_ReplaceAndFind:
				if (CanReplace(what, ignoreCase, grep))
					RegisterCommand(new PReplaceCmd(this, min(fCaret, fAnchor), abs(fCaret - fAnchor),
						true, gFindDialog->Backward()));
				break;
			
			case msg_ReplaceAndFindBackward:
				if (CanReplace(what, ignoreCase, grep))
					RegisterCommand(new PReplaceCmd(this, min(fCaret, fAnchor), abs(fCaret - fAnchor),
						true, !gFindDialog->Backward()));
				break;
			
			case msg_ReplaceAll:
				RegisterCommand(new PReplaceAllCmd(this));
				break;
		}
	}
} /* PText::Find */

bool PText::CanReplace(const char *what, bool ignoreCase, bool grep)
{
	bool result = false;
	
	if (!what || !*what)
		return result;
	
	char *t;
	GetSelectedText(t, false);
	if (!t)
		return result;
	
	if (gRxInstalled && grep)
	{
		regmatch_t m, *match = &m;
		int a = min(fCaret, fAnchor), r;
		
		regex_t pb;
		memset(&pb, 0, sizeof(pb));
		
		if (rx_regcomp(&pb, what, ignoreCase) == REG_NOERROR)
		{
			r = rx_regexec(&pb, t, strlen(t), 1, match,
				(a == 0 || fText[a - 1] == '\n') ? 0 : REG_NOTBOL);
	
			result = (r == REG_NOERROR && m.rm_so == 0 && m.rm_eo == strlen(t));
		}

		regfree(&pb);
	}
	else
		 if (ignoreCase)
		result = strcasecmp(what, t) == 0;
	else
		result = strcmp(what, t) == 0;
	
	free(t);

	return result;
} /* CCellView::CanReplace */

bool PText::FindNext(const unsigned char *what, int& offset, bool ignoreCase,
	bool wrap, bool backward, bool entireWord, bool grep, bool scroll)
{
	int skip[256], wl = strlen((char *)what);
	bool wrapped = false;
	const regex_t *pb = NULL;
	
	ASSERT(what);
	
	if (fText.Size() == 0)
	{
		beep();
		return false;
	}
	
	if (gRxInstalled && grep)
		pb = gFindDialog->PatternBuffer();
	else
	{
		if (backward)
			initskip_b(what, skip, ignoreCase);
		else
			initskip(what, skip, ignoreCase);
	}
	
	while (true)
	{
		if (gRxInstalled && grep)
		{
			int a = offset, r;
 
			if (backward) a--;
			regmatch_t match[2];
			
			if (backward)
			{
				r = rx_regexec(pb, fText.Buffer(), a, 1, match,
					(a == 0 || fText[a - 1] == '\n') ? 0 : REG_NOTBOL, true);
 
				offset = match[0].rm_so;
				wl = match[0].rm_eo - match[0].rm_so;
			}
			else
			{
				r = rx_regexec(pb, fText.Buffer() + a, fText.Size() - a, 1, match,
					(a == 0 || fText[a - 1] == '\n') ? 0 : REG_NOTBOL);
 
				offset += match[0].rm_so;
				wl = match[0].rm_eo - match[0].rm_so;
			}
			
			if (r == 1)
			{
				offset = (backward ? -1 : fText.Size());
				wl = 1;
			}
		}
		else if (backward)
		{
			if (offset > 0)
				offset = mismatchsearch_b(what, (unsigned char *)fText.Buffer(),
					offset + wl - 1, skip, ignoreCase);
		}
		else
		{
			if (offset < fText.Size())
				offset += mismatchsearch(what, (unsigned char *)fText.Buffer() + offset,
					fText.Size() - offset, skip, ignoreCase) + 1;
		}

		if (!entireWord ||
			(FindWord(offset + 1, B_LEFT_ARROW) == offset &&
			 FindWord(offset, B_RIGHT_ARROW) == offset + wl))
		{
			if ((backward && offset >= 0) || (!backward && offset < fText.Size()))
			{
				if (scroll) 
				{
					ChangeSelection(offset, offset + wl);

					if (gPrefs->GetPrefInt("centerfound", 0))
						CenterSelection();
					else
						ScrollToCaret();
				}
				else
				{
					fAnchor = offset;
					fCaret = offset + wl;
				}
				return true;
			}
		}
		else if (entireWord)
		{
			offset += (backward ? -2 : 1);
			if (offset >= 0 && offset <= fText.Size())
				continue;
		}

		if (wrap && !wrapped)
		{
			offset = backward ? fText.Size() : 0;
			wrapped = true;
			continue;
		}
		else
			break;
	}
	
	if (!gFindDialog->IsInMultiFileState())
		beep();

	return false;
} /* PText::FindNext */

void PText::JumpToFunction(const char *func, int offset)
{
	HideCaret();

	int lineNr = Offset2Line(offset);
	int ls = LineStart(lineNr);

	int a, c;
	a = fAnchor;
	c = fCaret;

	BScrollBar *bar;

	if (fActivePart == 1)
		bar = fVScrollBar1;
	else
		bar = fVScrollBar2;
	
	if (FindNext((unsigned char *)func, offset, false, false, false, false, false, false) ||
		FindNext((unsigned char *)func, offset, false, false, true, false, false, false))
	{
		swap(fAnchor, a);
		swap(fCaret, c);

		int line = Offset2Line(a);
		bar->SetValue(line * fLineHeight);
		ChangeSelection(a, c);
	}
	else
		ChangeSelection(ls, ls);
} /* PText::JumpToFunction */

void PText::FindNextError(bool backward)
{
	try
	{
		regmatch_t matches[5];
		regex_t rx;
		char err[100];
		entry_ref ref;
		int line = 0, size;
		const char *text;
		bool found = false;

		memset(&rx, 0, sizeof(rx));
		
		const char kErrLine[] =
			"^([^:]+):([0-9]+): (warning: )?(.*)";
		
		int r = regcomp(&rx, kErrLine, REG_EXTENDED | REG_NEWLINE);
		
		if (r)
		{
			regerror(r, &rx, err, 100);
			THROW((err));
		}
		
		text = fText.Buffer();
		if (backward)
			size = min(fCaret, fAnchor) - 1;
		else
		{
			size = fText.Size();
			text += max(fCaret, fAnchor);
			size = fText.Buffer() + size - text;
		}
		
		while (!found && size > 0)
		{
			if (backward)
				r = rx_regexec(&rx, text, size, 5, matches, 0, true);
			else
				r = rx_regexec(&rx, text, size, 5, matches, 0, false);
			
			if (r)
			{
				if (r == REG_NOMATCH)
				{
					beep();
					return;
				}
				else
				{
					regerror(r, &rx, err, 100);
					THROW((err));
				}
			}

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif
			
			char file[PATH_MAX], *t;
			
			if (text[matches[1].rm_so] != kDirectorySeparator && fCWD)
			{
				strcpy(file, fCWD);

				int len = strlen(file);
				file[len++] = kDirectorySeparator;
				file[len] = 0;
				t = file + len;
		
				int fl = matches[1].rm_eo - matches[1].rm_so;
				strncpy(t, text + matches[1].rm_so, fl);
				t[fl] = 0;
			}
			else
			{
				int fl = matches[1].rm_eo - matches[1].rm_so;
				strncpy(file, text + matches[1].rm_so, fl);
				file[fl] = 0;
			}

			if (get_ref_for_path(file, &ref) == B_OK && BEntry(&ref).Exists())
			{
				line = strtoul(text + matches[2].rm_so, &t, 10);
				found = true;
			}
			else
			{
				if (backward)
					size -= matches[0].rm_so;
				else
				{
					text += matches[0].rm_eo;
					size -= matches[0].rm_eo;
				}
			}
		}

		if (found)
		{
			PDoc *doc = dynamic_cast<PDoc*>(gApp->OpenWindow(ref, true));
			
			if (doc)
			{
				BMessage msg(msg_SelectError);

				int infoLen = matches[4].rm_eo - matches[4].rm_so;
				CAlloca info(infoLen + 1);
				
				strncpy(info, text + matches[4].rm_so, infoLen);
				info[infoLen] = 0;

				msg.AddInt32("line", line - 1);
				msg.AddBool("warning", matches[3].rm_so > 0 && matches[3].rm_eo > matches[3].rm_so);
				msg.AddString("info", info);

				BMessenger(doc->TextView()).SendMessage(&msg);
			}
			
			SelectLine(Offset2Line(text + matches[0].rm_so - fText.Buffer()));
		}
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* PText::FindNextError */

void PText::DoIncSearch(bool forward)
{
	bool ignCase = gPrefs->GetPrefInt("isearch_igncase", 1);

	if (fIncSearch)
	{
		if (fIncPat[0] == 0)
		{
			const char *s = gFindDialog->FindString();
			if (s[0])
				IncSearchKey(s, strlen(s));
		}
		else
		{
			int offset = fIncCaret = forward ? fCaret : fAnchor;
			if (FindNext((unsigned char*)fIncPat, offset, ignCase, false, ! forward, false, false, true))
			{
				BMessage msg(msg_EnterSearchString);
				msg.AddString("string", fIncPat);
				gFindDialog->PostMessage(&msg);
			}
			else
				beep();
		}
	}
	else
	{
		if (forward)
			fIncSearch = 1;
		else
			fIncSearch = 2;

		fIncPat[0] = 0;
		fIncCaret = fCaret;
		Doc()->ButtonBar()->SetDown(msg_IncSearchBtn, true);
	}
} /* PText::DoIncSearch */

void PText::IncSearchKey(const char *bytes, int numBytes)
{
	int ipl = strlen(fIncPat);
	bool ignCase = gPrefs->GetPrefInt("isearch_igncase", 1);
	
	if (bytes[0] == B_RETURN || bytes[0] == B_ESCAPE)
		fIncSearch = 0;
	else if (bytes[0] == B_BACKSPACE)
	{
		if (ipl)
			fIncPat[ipl - 1] = 0;
		else
			fIncSearch = 0;
	}
	else if (mcharlen(bytes) > 1 || bytes[0] == B_TAB || !iscntrl(bytes[0]))
	{
		if (ipl + numBytes > 255)
			THROW(("this incremental search pattern is too long"));
		strncat(fIncPat, bytes, numBytes);
		fIncPat[ipl + numBytes] = 0;
	}
	else
		fIncSearch = 0;
	
	if (fIncSearch)
	{
		int offset = fIncCaret;

		if (FindNext((unsigned char*)fIncPat, offset, ignCase, false, fIncSearch == 2, false, false, true))
		{
			BMessage msg(msg_EnterSearchString);
			msg.AddString("string", fIncPat);
			gFindDialog->PostMessage(&msg);
		}
		else
		{
			fIncPat[ipl] = 0;
			beep();
		}
	}
	
	if (fIncSearch == 0)
	{
		Doc()->ButtonBar()->SetDown(msg_IncSearchBtn, false);
		fWalkOffset = Offset2Position(fCaret).x;
	}
} /* PText::IncSearchKey */

#pragma mark - Exec

const int kBufferSize = 4096;

bool gRedirectStdErr;

#define kPWD		":^@_#-->"
#define kPWDL		8

const char
	kMPWCommandTemplate[] =
		"%s;"
		"Echo \"" kPWD "\"`Directory`",
	kMPWCWDTemplate[] =
		"Set Pe 1; Export Pe; Directory \"%s\";";

#include "PExec.h"
#include "PErrorWindow.h"

PExec::PExec(PText *txt, const char *cmd, const char *wd)
	: MThread("execvp")
{
	fText = txt;
	fWindow = fText->Looper();

	BPath appDirPath, settingsDirPath;
	BEntry e;
	FailOSErr(gAppDir.GetEntry(&e));
	FailOSErr(e.GetPath(&appDirPath));
	FailOSErr(gPrefsDir.GetEntry(&e));
	FailOSErr(e.GetPath(&settingsDirPath));

	char buf[2048];
	sprintf(buf, "export PE_DIR=\"%s\" PE_SETTINGS_DIR=\"%s\"; "
		"%s"
		"\necho \"" kPWD "\"$PWD",
		appDirPath.Path(), settingsDirPath.Path(), cmd);
	fCmd = strdup(buf);
	
	fWD = wd ? strdup(wd) : NULL;
	FailNil(fCmd);
	fErrorWindow = NULL;
	fPID = 0;
} /* PExec::PExec */

PExec::~PExec()
{
	free(fCmd);
	if (fWD) free(fWD);
} /* PExec::~PExec */

long PExec::Execute()
{
	int ofd[2], efd[2], pid, err;
	
	try
	{
		FailOSErr(pipe(ofd));
		if (gRedirectStdErr)
			FailOSErr(pipe(efd));
		
		pid = fork();
		if (pid < 0) FailOSErr(pid);
		
		if (pid == 0)		// child
		{
			setpgid(0, 0);		// Make this a new process group
			
			fflush(stdout);
			close(STDOUT_FILENO);
			err = dup(ofd[1]);		// What will happen if this fails???
			if (err < 0) perror("duplicating stdout"); //THROW((err));

			fflush(stderr);
			close(STDERR_FILENO);
			if (gRedirectStdErr)
				err = dup(efd[1]);
			else
				err = dup(ofd[1]);
			if (err < 0) perror("duplicating stderr"); //THROW((err));
			
			close(ofd[0]);
			close(ofd[1]);
			
			if (gRedirectStdErr)
			{
				close(efd[0]);
				close(efd[1]);
			}

			if (fWD) chdir(fWD);

			char *args[5];
			args[0] = "/bin/sh";
			args[1] = "-c";
			args[2] = fCmd;
			args[3] = NULL;
			
			if (execvp(args[0], args))
			{
				beep();		// what else can we do???
				exit(1);	// this may kill Pe, but what will happen if we just return?
			}
		}
		else if (pid >= 0)	// parent
		{
			fPID = pid;

			close(ofd[1]);
			
			if (gRedirectStdErr)
				fErrorWindow = new PErrorWindow(efd, fWD);

			bool prepared = false;
			char buf[kBufferSize] = "", *pwd = NULL;
			int flags;

			fcntl(ofd[0], F_GETFL, &flags);
			fcntl(ofd[0], F_SETFL, flags | O_NONBLOCK);
			
			int rr, state = 0;
			
			while ((rr = read(ofd[0], buf + state, kBufferSize - 1 - state)) != 0)
			{
				if (rr > 0)
				{
					buf[rr + state] = 0;
					
					char *p = buf - 1 + state;
					while (*++p && state <= kPWDL)
					{
						switch (state)
						{
							case kPWDL:
									if (*p == '/')
									{
										state++;
										pwd = p;
									}
									else
										state = 0;
									break;
							default:
								if (*p == kPWD[state])
									state++;
								else
									state = 0;
						}
					}
					
					char sc = 0;
					
					if (state)
					{
						sc = p[-state];
						p[-state] = 0;
					}
					
					if (strlen(buf))
					{
						BAutolock lock(fWindow);
						if (lock.IsLocked())
						{
							if (!prepared)
							{
								fText->PrepareForOutput();
								prepared = true;
							}
							
							fText->TypeString(buf);
						}
					}
					
					if (state)
					{
						p[-state] = sc;
						memmove(buf, p - state, state);
					}
				}
				else if (errno == EAGAIN)
					snooze(100000);
				else
					break;
				
				if (Cancelled())
					break;
			}

			if (pwd)
			{
				char *e = strchr(pwd, '\n');
				if (e) *e = 0;

				BAutolock lock(fWindow);
				if (lock.IsLocked())
					fText->SetCWD(pwd);
			}

			close(ofd[0]);
		}
	}
	catch (HErr& e)
	{
		e.DoError();
	}

	BMessage msg(msg_ExecFinished);
	fWindow->PostMessage(&msg, fText);
		
	return 0;
} /* PExec::Execute */

#pragma mark -

void PText::ExecuteSelection()
{
	if (fExec)
	{
		MWarningAlert a("Can only execute one command at the time per window, sorry!");
		a.Go();
		return;
	}
	
	char *s;
	GetSelectedText(s);

	if (!s)
	{
		int line = Offset2Line(fCaret);
		int from, to;
		
		from = LineStart(line);
		to = (line == LineCount() - 1 ? fText.Size() : LineStart(line + 1) - 1);
		s = (char *)malloc(to - from + 1);
		s[to - from] = 0;
		FailNil(s);
		fText.Copy(s, from, to - from);
	}

	fExec = new PExec(this, s, fCWD);
	fExec->Run();
	Doc()->ButtonBar()->SetDown(msg_Execute, true);
	
	free(s);
} /* PText::ExecuteSelection */

void PText::KillCurrentJob()
{
	if (fExec && fExec->fPID)
		send_signal(-fExec->fPID, SIGINT);
} /* PText::KillCurrentJob */

void PText::PrepareForOutput()
{
	fLastCommand = NULL;
	int nc;

	if (fAnchor != fCaret)
		nc = max(fAnchor, fCaret);
	else
	{
		int line = Offset2Line(fCaret);
		if (line < LineCount() - 1)
			nc = LineStart(line + 1) - 1;
		else
			nc = fText.Size();
	}

	ChangeSelection(nc, nc);
	
	CharKeyDown("\n", 1);
} /* PText::PrepareForOutput */

#pragma mark - Draw

void PText::Draw(BRect updateRect)
{
	fBounds = Bounds();

	int start, end, i, cl;
	float y, v;
	BRect r;
	BRegion clip;
	
	cl = Offset2Line(fCaret);
	
	SetLowColor(gColor[kLowColor]);
	
	if (fSplitAt > 0)
	{
		v = fVScrollBar1->Value();
		
		r = fBounds;
		r.bottom = fSplitAt;
		clip.Include(r);

		if (clip.Frame().IsValid())
		{
			SetHighColor(gColor[kLowColor]);
			StrokeLine(fBounds.LeftTop(), fBounds.RightTop());
			ConstrainClippingRegion(&clip);
			
			start = max(0, (int)floor((updateRect.top + v) / fLineHeight));
			end = min(LineCount(),
				(int)ceil((min(updateRect.bottom, fSplitAt - kSplitterHeight) + v) / fLineHeight) + 1);

			y = ceil(fLineHeight * start - v);
			
			for (i = start; i < end; i++)
			{
				DrawLine(i, y, false);
				y += fLineHeight;
				if (cl == i) fCaretVisible = false;
			}
	
			r.Set(0, y, updateRect.right, fSplitAt - kSplitterHeight);
			if (updateRect.Intersects(r))
				FillRect(r, B_SOLID_LOW);

			BPoint p1(0, fSplitAt - kSplitterHeight), p2(updateRect.right, fSplitAt - kSplitterHeight);
			BeginLineArray(5);
			AddLine(p1, p2, kDarkShadow);	p1.y++; p2.y++;
			AddLine(p1, p2, kWhite);			p1.y++; p2.y++;
			AddLine(p1, p2, kViewColor);	p1.y++; p2.y++;
			AddLine(p1, p2, kShadow);		p1.y++; p2.y++;
			AddLine(p1, p2, kDarkShadow);	p1.y++; p2.y++;
			EndLineArray();
		}
	}
	
	r = fBounds;
	r.top = fSplitAt;
	clip.MakeEmpty();
	clip.Include(r);

	if (clip.Frame().IsValid())
	{
		SetHighColor(gColor[kLowColor]);
		StrokeLine(BPoint(0, fSplitAt), BPoint(updateRect.right, fSplitAt));
		ConstrainClippingRegion(&clip);
	
		v = fVScrollBar2->Value();
		
		start = max(0, (int)floor((max(updateRect.top - fSplitAt, (float)0) + v) / fLineHeight));
		end = min(LineCount(), (int)ceil((updateRect.bottom + v - fSplitAt) / fLineHeight) + 1);
		
		y = ceil(fSplitAt + fLineHeight * start - v);

		for (i = start; i < end; i++)
		{
			DrawLine(i, y, false);
			y += fLineHeight;
			if (cl == i) fCaretVisible = false;
		}
		
		SetLowColor(gColor[kLowColor]);
		r.Set(0, y, updateRect.right, updateRect.bottom);
		if (updateRect.Intersects(r))
			FillRect(r, B_SOLID_LOW);
   	}

	ConstrainClippingRegion(NULL);
} /* PText::Draw */

void PText::DrawLine(int lineNr, float y, bool buffer)
{
	fLineInfo[lineNr].dirty = false;

	BView *vw = this;
	float x = 0, hv;
	BRect E;
	
	if (buffer && fLineMap->Lock())
	{
		vw = fLineView;
		E = fLineMap->Bounds();
		hv = fHScrollBar->Value();
		y = E.bottom - fFH.descent;
	}
	else
	{
		float v = fHScrollBar->Value();
		E.Set(v, y + 1, v + fBounds.Width(), y + fLineHeight);
		hv = 0;
		y = E.bottom - fFH.descent;
	}
	vw->SetFont(&fFont);
	vw->SetLowColor(gColor[kLowColor]);
	vw->FillRect(E, B_SOLID_LOW);
	
	int s, e, l;
	s = LineStart(lineNr);
	if (lineNr < LineCount() - 1)
	{
		e = LineStart(lineNr + 1);
		l = e - s - fLineInfo[lineNr + 1].nl;
	}
	else
	{
		e = fText.Size();
		l = e - s;
	}
	
	ASSERT(l >= 0);
	
	CAlloca b(l + 1);
	int starts[100];
	rgb_color colors[100];
	
	if (l)
	{
		fText.Copy(b, s, l);
		b[l] = 0;

		int state = LineState(lineNr);

		if (fSyntaxColoring)
		{
			memset(starts, 0, sizeof(int) * 100);
			fLangIntf->ColorLine(b, l, state, starts, colors);
		}
		else
		{
			colors[0] = gColor[kTextColor];
			starts[0] = 0;
			starts[1] = 0;
		}
	}

	int a, c;
	a = min(fAnchor, fCaret);
	c = max(fAnchor, fCaret);

	if (a < c && a < e && c >= s)
	{
		BRect r(E);

		if (fBlockSelect)
		{
			BlockOffsetsForLine(lineNr, a, c);
			a += s;
			c += s;
			
			if (fWindowActive)
			{
				r.left = Offset2Position(fAnchor).x - hv;
				r.right = Offset2Position(fCaret).x - hv;
				if (r.right < r.left) swap(r.right, r.left);
				
				vw->SetLowColor(gColor[kSelectionColor]);
				vw->FillRect(r, B_SOLID_LOW);
			}
		}
		else
		{
			if (fWindowActive)
			{
				if (a > s)
					r.left = Offset2Position(a).x - hv;
				else
					r.left = 3;
	
				if (c < e || (lineNr == LineCount() - 1 && c == fText.Size()))
					r.right = Offset2Position(c).x - hv;
	
				vw->SetLowColor(gColor[kSelectionColor]);
				vw->FillRect(r, B_SOLID_LOW);
			}
			else if (a <= e && c > s)
			{
				if (a > s)
					r.left = Offset2Position(a).x - hv;
				else
					r.left = 3;
	
				if (c < e || (lineNr == LineCount() - 1 && c == fText.Size()))
					r.right = Offset2Position(c).x - hv;
	
				vw->SetHighColor(gColor[kSelectionColor]);
				
				vw->StrokeLine(r.LeftBottom(), r.LeftTop());
				vw->StrokeLine(r.RightBottom(), r.RightTop());
				
				if (a >= s)
					vw->StrokeLine(r.LeftTop(), r.RightTop());
				
				if (c < e + 1)
					vw->StrokeLine(r.LeftBottom(), r.RightBottom());
				
				if (c > e && a >= s)
				{
					float x1, x2;
					
					if (Offset2Line(c) == lineNr + 1)
					{
						x1 = max(r.left, Offset2Position(c).x - hv);
						x2 = fBounds.right;
					}
					else
					{
						x1 = 3;
						x2 = r.left;
					}
						
					vw->StrokeLine(BPoint(x1, r.bottom), BPoint(x2, r.bottom));
				}
				
				if (a < s && (c < e || (lineNr == LineCount() - 1 && c == fText.Size())))
				{
					float x1, x2;
	
					if (Offset2Line(a) == lineNr - 1)
					{
						x1 = 3;
						x2 = min(r.right, Offset2Position(a).x - hv);
					}
					else
					{
						x1 = r.right;
						x2 = fBounds.right;
					}
					
					vw->StrokeLine(BPoint(x1, r.top), BPoint(x2, r.top));
				}
			}
		}
		
		if (a > s)
			vw->SetLowColor(gColor[kLowColor]);
	}
	
	int i, j, ci = 0;
	bool inSelection;
	
	if (fShowInvisibles)
	{
		i = 0;
		j = 0;
		x = 0;
		
		inSelection = (i >= a - s && i < c - s);
		
		if (inSelection && fWindowActive)
		{
			vw->SetLowColor(gColor[kSelectionColor]);
			vw->SetHighColor(gInvColor[kInvisiblesColor]);
		}
		else
		{
			vw->SetLowColor(gColor[kLowColor]);
			vw->SetHighColor(gColor[kInvisiblesColor]);
		}

		while (i < l)
		{
			if (b[i] == '\t')
			{
				if (i - j > 0)
					x += StringWidth(b + j, i - j);
				vw->DrawString(gTabChar, BPoint(x + 3 - hv, y));

				int t = (int)floor(x / fTabWidth) + 1;
				x = (Round(t * fTabWidth) > Round(x) ? t * fTabWidth : (t + 1) * fTabWidth);
				j = i + 1;
			}
			else if (b[i] == ' ')
			{
				if (i > j)
					x += StringWidth(b + j, i - j);

				vw->DrawString(gSpaceChar, BPoint(x + 3 - hv, y));
				j = i;
			}

			if (a != c && fWindowActive)
			{
				if (i == a - s)
				{
					inSelection = true;
					vw->SetLowColor(gColor[kSelectionColor]);
					vw->SetHighColor(gInvColor[kInvisiblesColor]);
				}
				else if (i == c - s)
				{
					inSelection = false;
					vw->SetLowColor(gColor[kLowColor]);
					vw->SetHighColor(gColor[kInvisiblesColor]);
				}
			}
	
			i++;
		}
		
		if (i > j)
			x += StringWidth(b + j, i - j);

		if (lineNr < LineCount() - 1 && fLineInfo[lineNr + 1].nl)
			vw->DrawString(gReturnChar, BPoint(x + 3 - hv, y));
	}
	
	bool draw = false;

	i = 0;
	j = 0;
	x = 0;

	inSelection = fWindowActive && (i >= a - s && i < c - s);

	if (inSelection)
		vw->SetHighColor(gInvColor[kTextColor]);
	else
		vw->SetHighColor(gColor[kTextColor]);
	
	while (i < l)
	{
		if (b[i] == '\t' || i == a - s || i == c - s || (b[i] >= 0 && iscntrl(b[i])))
			draw = true;
		
		if (b[i] == '\n')
			b[i] = 0;
		
		if (starts[ci] == i)
			draw = true;
		
		if (draw)
		{
			if (b[i] == '\t')
			{
				if (i - j > 0)
				{
					vw->DrawString(b + j, i - j, BPoint(x + 3 - hv, y));
					x += StringWidth(b + j, i - j);
				}

				int t = (int)floor(x / fTabWidth) + 1;
				x = (Round(t * fTabWidth) > Round(x) ? t * fTabWidth : (t + 1) * fTabWidth);
				j = i + 1;
			}
			else if (b[i] >= 0 && iscntrl(b[i]))
			{
				if (i - j > 0)
				{
					vw->DrawString(b + j, i - j, BPoint(x + 3 - hv, y));
					x += StringWidth(b + j, i - j);
				}
				
				rgb_color savedColor;
				
				if (fShowInvisibles)
				{
					savedColor = vw->HighColor();
					if (inSelection)
						vw->SetHighColor(gInvColor[kInvisiblesColor]);
					else
						vw->SetHighColor(gColor[kInvisiblesColor]);
				}
				vw->DrawString(gControlChar, BPoint(x + 3 - hv, y));
				
				if (fShowInvisibles)
					vw->SetHighColor(savedColor);

				x += StringWidth(gControlChar, strlen(gControlChar));
				j = i + 1;
			}
			else if (i > j)
			{
				vw->DrawString(b + j, i - j, BPoint(x + 3 - hv, y));
				x += StringWidth(b + j, i - j);
				j = i;
			}
			
			if (a != c && fWindowActive)
			{
				if (i == a - s)
				{
					inSelection = true;
					vw->SetLowColor(gColor[kSelectionColor]);
					vw->SetHighColor(ci ? LookupDistinctColor(colors[ci - 1]) : gInvColor[kTextColor]);
				}
				else if (i == c - s)
				{
					inSelection = false;
					vw->SetLowColor(gColor[kLowColor]);
					vw->SetHighColor(ci ? colors[ci - 1] : gColor[kTextColor]);
				}
			}

			draw = false;

			if (starts[ci] == i)
			{
				if (inSelection)
					vw->SetHighColor(LookupDistinctColor(colors[ci]));
				else
					vw->SetHighColor(colors[ci]);
				ci++;
			}
		}

		if (b[i] == 0)
			break;

		i++;
	}

	if (i > j)
		vw->DrawString(b + j, i - j, BPoint(x + 3 - hv, y));
	
	if (fMark >= 0 && fMark <= fText.Size() && Offset2Line(fMark) == lineNr)
	{
		float x = Offset2Position(fMark).x;
		
		vw->SetHighColor(gColor[kMarkColor]);
		vw->StrokeLine(BPoint(x, E.top), BPoint(x, E.bottom));
		vw->SetHighColor(kBlack);
	}
	
	if (buffer)
	{
		fLineView->Sync();
		fLineMap->Unlock();
	}
} /* PText::DrawLine */

void PText::RedrawDirtyLines()
{
	BRect b(fBounds);
	int i;
	float y1, y2;

	BRegion clip1, clip2;
	BRect r(b);
	
	r.bottom = fSplitAt - kSplitterHeight;
	clip1.Include(r);
	
	r = b;
	r.top = fSplitAt;
	clip2.Include(r);
	
	for (i = 0; i < LineCount(); i++)
	{
		if (fLineInfo[i].dirty)
		{
			y1 = i * fLineHeight - fVScrollBar1->Value();
			y2 = i * fLineHeight - fVScrollBar2->Value() + fSplitAt;
			
			if ((y1 < -fLineHeight || y1 > fSplitAt) &&
				(y2 < fSplitAt - fLineHeight || y2 > b.bottom))
			{
				fLineInfo[i].dirty = false;
				continue;
			}

			DrawLine(i, y1, true);
			
			if (y1 > -fLineHeight && y1 < fSplitAt)
			{
				if (clip1.Frame().IsValid())
				{
					ConstrainClippingRegion(&clip1);
					DrawBitmap(fLineMap, BPoint(fHScrollBar->Value(), y1 + 1));
				}
			}

			if (y2 > fSplitAt - fLineHeight && y2 < b.bottom)
			{
				if (clip2.Frame().IsValid())
				{
					ConstrainClippingRegion(&clip2);
					DrawBitmap(fLineMap, BPoint(fHScrollBar->Value(), y2 + 1));
				}
			}
		}
	}
	ConstrainClippingRegion(NULL);

	y1 = LineCount() * fLineHeight - fVScrollBar1->Value() + 1;
	if (y1 < fSplitAt - kSplitterHeight)
	{
		r = b;
		r.top = y1;
		r.bottom = fSplitAt - kSplitterHeight;
		SetLowColor(gColor[kLowColor]);
		FillRect(r, B_SOLID_LOW);
	}
	
	y2 = LineCount() * fLineHeight - fVScrollBar2->Value() + fSplitAt + 1;
	if (y2 < b.bottom)
	{
		r = b;
		r.top = y2;
		SetLowColor(gColor[kLowColor]);
		FillRect(r, B_SOLID_LOW);
	}
	
	if (fSplitAt > 0)
	{
		BPoint p1(0, fSplitAt - kSplitterHeight), p2(b.right, fSplitAt - kSplitterHeight);
		BeginLineArray(5);
		AddLine(p1, p2, kDarkShadow);	p1.y++; p2.y++;
		AddLine(p1, p2, kWhite);			p1.y++; p2.y++;
		AddLine(p1, p2, kViewColor);	p1.y++; p2.y++;
		AddLine(p1, p2, kShadow);		p1.y++; p2.y++;
		AddLine(p1, p2, kDarkShadow);	p1.y++; p2.y++;
		EndLineArray();
	}
	
	fCaretVisible = false;
} /* PText::RedrawDirtyLines */

void PText::HiliteSelection()
{
	int l1, l2;
	
	l1 = Offset2Line(min(fAnchor, fCaret));
	l2 = Offset2Line(max(fAnchor, fCaret));
	
	for (int i = l1; i <= l2; i++)
		TouchLine(i);
	
	RedrawDirtyLines();
} /* PText::HiliteSelection */

void PText::ChangeSelection(int newAnchor, int newCaret, bool block)
{
	int na, nc;
	int oa, oc;
	
	na = min(newAnchor, newCaret);
	nc = max(newAnchor, newCaret);
	
	oa = min(fAnchor, fCaret);
	oc = max(fAnchor, fCaret);
	
	int ls, le;
	
	if (fBlockSelect)
	{
		ls = Offset2Line(min(na, oa));
		le = Offset2Line(max(nc, oc));
		
		TouchLines(ls, le);
	}
	else if (nc < oa || na > oc)
	{
		ls = Offset2Line(na);
		le = Offset2Line(nc);
		
		TouchLines(ls, le);
	
		ls = Offset2Line(oa);
		le = Offset2Line(oc);
		
		TouchLines(ls, le);
	}
	else
	{
		ls = Offset2Line(min(oa, na));
		le = Offset2Line(max(oa, na));
		
		TouchLines(ls, le);
	
		ls = Offset2Line(min(oc, nc));
		le = Offset2Line(max(oc, nc));
		
		TouchLines(ls, le);
	}

	fCaret = newCaret;
	fAnchor = newAnchor;
	fBlockSelect = block;
	
	RedrawDirtyLines();
} /* PText::ChangeSelection */

void PText::Pulse()
{
	if (IsFocus() && fWindowActive && fNextCaret < system_time() && (gFlashCursor || ! fCaretVisible))
		ToggleCaret();
} /* PText::Pulse */

void PText::ToggleCaret()
{
	if (fAnchor != fCaret || !fWindowActive || (fIncSearch && fCaretVisible))
		return;
	
	fNextCaret = system_time() + 500000;
	
	Window()->UpdateIfNeeded();
	Sync();
	
	BPoint p = Offset2Position(fCaret);

	BRegion clip;
	BRect r(fBounds);
	if (fActivePart == 1)
		r.bottom = fSplitAt;
	else
		r.top = fSplitAt;
	clip.Include(r);

	if (gBlockCursor)
	{
		float w;

		if (fCaret < fText.Size() - 1)
		{
			if (isspace(fText[fCaret]))
				w = StringWidth(" ", 1);
			else if (iscntrl(fText[fCaret]))
				w = StringWidth("¿", strlen("¿"));
			else
				w = TextWidth(fCaret, fText.CharLen(fCaret));
		}
		else
			w = StringWidth(" ", 1);
			
		r.Set(p.x, p.y + 1, p.x + w, p.y + fLineHeight);
	}
	else
		r.Set(p.x, p.y + 1, p.x, p.y + fLineHeight);

	if (clip.Frame().IsValid())
	{
		if (clip.Intersects(r))
		{
			ConstrainClippingRegion(&clip);
			SetDrawingMode(B_OP_INVERT);

			FillRect(r);
			
			fCaretVisible = !fCaretVisible;

			SetDrawingMode(B_OP_COPY);
			ConstrainClippingRegion(NULL);
		}
		else
			fCaretVisible = false;
	}
} /* PText::ToggleCaret */

void PText::ShiftLines(int first, int dy, int part)
{
	BRect b(Bounds()), src, dst;
	float v, dv = dy * fLineHeight;
	
	if (part == 1)
	{
		v = fVScrollBar1->Value();
		b.top += 1;
		b.bottom = fSplitAt - kSplitterHeight;
	}
	else
	{
		v = fVScrollBar2->Value();
		b.top = fSplitAt + 1;
	}

	int tl = (int)(v / fLineHeight);
	
	src = b;
	
	if (dy < 0)
	{
		src.top = b.top + max(first * fLineHeight - v, (float)0) - dv;
		src.bottom = b.bottom;
	}
	else
	{
		src.top = max(b.top, b.top + first * fLineHeight - v);
		src.bottom = b.bottom - dv;
	}
	
	if (src.IsValid())
	{
		dst = src;
		dst.OffsetBy(0, dv);
		
		CopyBits(src, dst);
		
		if (dy > 0)
			src.bottom = dst.top;
		else
			src.top = dst.bottom;
		
		Invalidate(src);
		Window()->UpdateIfNeeded();
	}
	else if (first < tl + ceil(b.Height() / fLineHeight))
		Draw(b);
} /* PText::ShiftLinesPart2 */

#pragma mark - Commands

BFilePanel *gCwdPanel = NULL;

void PText::MessageReceived(BMessage *msg)
{
	try
	{
		unsigned long what = msg->what;
		void *args = &msg;
		
		if (msg->WasDropped())
			HandleDrop(msg);
		else switch (what)
		{
			case B_CUT:
				RegisterCommand(new PCutCmd(this));
				break;

			case msg_CutAppend:
				RegisterCommand(new PCutCmd(this, true));
				break;

			case B_COPY:
				Copy();
				break;
			
			case msg_CopyAppend:
				Copy(true);
				break;

			case B_PASTE:
				RegisterCommand(new PPasteCmd(this));
				break;

			case msg_Clear:
				RegisterCommand(new PClearCmd(this));
				break;

			case msg_ShiftLeft:
			{
				PShiftLeftCmd *cmd = new PShiftLeftCmd(this);
				if (!cmd->IsNoOp())
					RegisterCommand(cmd);
				break;
			}

			case msg_ShiftRight:
				RegisterCommand(new PShiftRightCmd(this));
				break;

			case msg_Undo:
				Undo();
				break;

			case msg_Redo:
				Redo();
				break;

			case B_SELECT_ALL:
				ChangeSelection(0, fText.Size());
				break;

			case msg_SelectLine:
			{
				int cl = Offset2Line(fCaret);
				ChangeSelection(LineStart(cl), cl > LineCount() - 1 ? fText.Size() :
					LineStart(cl + 1));
				break;
			}

			case msg_Find:
			case msg_FindAgain:
			case msg_Replace:
			case msg_ReplaceAndFind:
			case msg_ReplaceAll:
			case msg_FindAgainBackward:
			case msg_ReplaceAndFindBackward:
				fLastCommand = NULL;
				Find(what, NULL);
				break;

			case msg_DlgFind:
				Find(msg_Find, args);
				break;
 
			case msg_DlgReplace:
				Find(msg_Replace, args);
				break;
 
			case msg_DlgReplaceAndFind:
				Find(msg_ReplaceAndFind, args);
				break;
 
			case msg_DlgReplaceAll:
				Find(msg_ReplaceAll, args);
				break;
 
			case msg_EnterSearchString:
			case msg_EnterReplaceString:
			case msg_FindSelection:
			case msg_FindSelectionBackward:
			{
				char *s;
				GetSelectedText(s);
				
				if (s)
				{
					BMessage nmsg(msg->what);
					nmsg.AddString("string", s);
					gFindDialog->PostMessage(&nmsg);
					free(s);
				}
				break;
			}

			case msg_QueryCanReplace:
			{
				BMessage reply(msg_ReplyCanReplace);
				reply.AddBool("canreplace", CanReplace(msg->FindString("what"),
					msg->FindBool("case"), msg->FindBool("grep")));
				msg->SendReply(&reply);
				break;
			}

			case msg_JumpToProcedure:
			{
				long l;
				const char *f;
				FailOSErr(msg->FindInt32("offset", &l));
				FailOSErr(msg->FindString("function", &f));
				JumpToFunction(f, l);
				break;
			}
			
			case msg_JumpToMarker:
			{
				long l;
				FailOSErr(msg->FindInt32("linenr", &l));
				Window()->UpdateIfNeeded();
				fVScrollBar2->SetValue(l * fLineHeight);
				SetCaret(LineStart(l));
				break;
			}

			case msg_Balance:
				fLangIntf->Balance(*this);
				break;

			case msg_OutputWillFollow:
				PrepareForOutput();
				break;

			case msg_TypeString:
			{
				fLastCommand = NULL;
				const char *s;
				FailOSErr(msg->FindString("string", &s));
				TypeString(s);
				break;
			}

			case msg_ExecFinished:
				fExec = NULL;
				Doc()->ButtonBar()->SetDown(msg_Execute, false);
				break;
 
			case msg_Twiddle:
				RegisterCommand(new PTwiddleCmd(this));
				break;

			case msg_ChangeFontAndTabs:
 //				RegisterCommand(new PFontTabsCmd(this, msg));
				ChangedInfo(msg);
				break;
			
			case msg_FindInNextFile:
				gFindDialog->PostMessage(msg_FindInNextFile);
				break;
		
			case msg_FindNextError:
				if (Doc()->IsWorksheet())
					FindNextError(false);
				else
				{
					PDoc *doc = PDoc::GetWorksheet();
					if (doc)
						doc->TextView()->ProcessCommand(msg_FindNextError, NULL);
				}
				break;

			case msg_FindPreviousError:
				if (Doc()->IsWorksheet())
					FindNextError(true);
				else
				{
					PDoc *doc = PDoc::GetWorksheet();
					if (doc)
						doc->TextView()->ProcessCommand(msg_FindPreviousError, NULL);
				}
				break;

			case msg_Comment:
				RegisterCommand(new PCommentCmd(this, true,
					fLangIntf->LineCommentStart(), fLangIntf->LineCommentEnd()));
				break;
			
			case msg_Uncomment:
				RegisterCommand(new PCommentCmd(this, false,
					fLangIntf->LineCommentStart(), fLangIntf->LineCommentEnd()));
				break;
			
			case msg_Wrap:
				RegisterCommand(new PWrapCmd(this));
				break;
			
			case msg_Unwrap:
				RegisterCommand(new PUnwrapCmd(this));
				break;
			
			case msg_Justify:
				RegisterCommand(new PJustifyCmd(this));
				break;
			
			case msg_SelectParagraph:
				SelectParagraph();
				break;
			
			case msg_ChangeWD:
			{
				if (!gCwdPanel)
					gCwdPanel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this),
						NULL, B_DIRECTORY_NODE, false, new BMessage(msg_ChangedWD));
				else
					gCwdPanel->SetTarget(this);
				gCwdPanel->Show();
				break;
			}
			
			case msg_ChangedWD:
			{
				entry_ref ref;
				BEntry e;
				
				FailOSErr(msg->FindRef("refs", &ref));
				FailOSErr(e.SetTo(&ref));
				if (!e.IsDirectory()) THROW(("This is not a directory!"));
				
				BPath p;
				FailOSErr(e.GetPath(&p));
				
				if (fCWD) free(fCWD);
				fCWD = strdup(p.Path());
				FailNil(fCWD);
				
				SetDirty(true);
				break;
			}

			case msg_ToggleSyntaxColoring:
				fSyntaxColoring = !fSyntaxColoring;
				Draw(Bounds());
				break;
			
			case msg_DoGoToLine:
			{
				long line;
				FailOSErr(msg->FindInt32("line", &line));
				if (line > 0 && line < LineCount())
				{
					ChangeSelection(LineStart(line), LineStart(line - 1));
					CenterSelection();
				}
				else
					beep();
				break;
			}
			
//			case msg_DoChangeCase:
//			{
//				long newCase;
//				FailOSErr(msg->FindInt32("newcase", &newCase));
//				RegisterCommand(new PChangeCaseCmd(this, newCase));
//				break;
//			}
			
			case msg_ChangeCaseLower:
			{
				RegisterCommand(new PChangeCaseCmd(this, 1));
				break;
			}
			
			case msg_ChangeCaseUpper:
			{
				RegisterCommand(new PChangeCaseCmd(this, 2));
				break;
			}
			
			case msg_ChangeCaseCap:
			{
				RegisterCommand(new PChangeCaseCmd(this, 3));
				break;
			}
			
			case msg_ChangeCaseLine:
			{
				RegisterCommand(new PChangeCaseCmd(this, 5));
				break;
			}
			
			case msg_ChangeCaseSent:
			{
				RegisterCommand(new PChangeCaseCmd(this, 4));
				break;
			}
			
			case msg_DoConvertEncoding:
			{
				int32 f, t;
				FailOSErr(msg->FindInt32("from", &f));
				FailOSErr(msg->FindInt32("to", &t));
				RegisterCommand(new PEncodingCmd(this, f, t));
				break;
			}

			case msg_ToggleShowInvisibles:
				fShowInvisibles = !fShowInvisibles;
				Draw(Bounds());
				break;
			
			case msg_DoGlossy:
			{
				long ch, mod;
				FailOSErr(msg->FindInt32("char", &ch));
				FailOSErr(msg->FindInt32("modifiers", &mod));
				GlossaryKey(ch, mod);
				break;
			}
			
			case msg_HTMLButton:
			{
				const char* glossy;
				FailOSErr(msg->FindString("glossy", &glossy));
				GlossaryButton(glossy);
				break;
			}
			
			case msg_Execute:
				ExecuteSelection();
				break;
				
			case msg_CancelCommand:
				KillCurrentJob();
				break;
			
			case msg_IncSearchBtn:
				if (fIncSearch)
				{
					fIncSearch = false;
					Doc()->ButtonBar()->SetDown(msg_IncSearchBtn, false);
				}
				else
					DoIncSearch(true);
				break;
			
			case msg_IncSearch:
			case msg_IncSearchBackward:
				DoIncSearch(what == msg_IncSearch);
				break;
				
			case msg_SoftWrap:
				fSoftWrap = !fSoftWrap;
				Doc()->ButtonBar()->SetOn(msg_SoftWrap, fSoftWrap);
				RecalculateLineBreaks();
				RedrawDirtyLines();
				break;
			
			case msg_FuncPopup:
			{
				BPoint p;
				FailOSErr(msg->FindPoint("where", &p));
				ShowFunctionMenu(p);
				break;
			}
			
			case kmsg_SwitchActivePart:
			{
				if (fSplitAt > 0)
				{
					HideCaret();
					fActivePart = (fActivePart == 1) ? 2 : 1;
					
					int a, c;
					a = fAnchor;
					c = fCaret;
					
					ChangeSelection(fOPAnchor, fOPCaret);
					fOPCaret = c;
					fOPAnchor = a;
					swap(fMark, fOPMark);
				}
				else
					beep();
				break;
			}
			
			case kmsg_SplitWindow:
			{
				if (fSplitAt == 0)
				{
					SplitWindow();
					
					fOPAnchor = fAnchor;
					fOPCaret = fCaret;
					fOPMark = fMark;
 
 //					Window()->PostMessage(kmsg_Recenter, this);
					BMessage m(kmsg_Recenter);
					MessageReceived(&m);
					fVScrollBar1->SetValue(fVScrollBar2->Value());
				}
				else
					beep();
				break;
			}
			
			case kmsg_UnsplitWindow:
			{
				if (fSplitAt > 0)
					UnsplitWindow();
				else
					beep();
				break;
			}
			
			case msg_SelectLines:
			{
				long from, to;

				FailOSErr(msg->FindInt32("from", &from));
				FailOSErr(msg->FindInt32("to", &to));
				
				bool activate = false;
				msg->FindBool("activate", &activate);
				
				if (activate)
					Window()->Activate();
 
				if (fSoftWrap)
				{
					from = RealLine2Line(from);
					to = RealLine2Line(to);
					while (! fLineInfo[to].nl && to < LineCount() - 1)
						to++;
				}
				from = from < LineCount() - 1 ? LineStart(from) : fText.Size();
				to = to < LineCount() - 1 ? LineStart(to) : fText.Size();
					
				ChangeSelection(from, to);
				CenterSelection();
				break;
			}
			
			case msg_SelectError:
			{
				long line;
				FailOSErr(msg->FindInt32("line", &line));
				line = RealLine2Line(line);
				SelectLine(line);
				
				if (gPrefs->GetPrefInt("centerfound"))
					CenterSelection();
				break;
			}
			
			case msg_Select:
			{
				bool b;

				long a, c;
				FailOSErr(msg->FindInt32("anchor", &a));
				FailOSErr(msg->FindInt32("caret", &c));
			
				if (msg->FindBool("skipspaces", &b) == B_OK && b)
				{
					int s = LineStart(Offset2Line(a));
					
					while (isspace(fText[s++]) && s < Size())
						a++, c++;
				}

				if (msg->FindBool("activate", &b) == B_OK && b)
					Window()->Activate();
				
				ChangeSelection(a, c);
				CenterSelection();
				break;
			}
			
			case msg_SmoothUpdate:
				TouchLines(0);
				RedrawDirtyLines();
				Sync();
				break;
			case msg_ToggleFont:
				ChangedInfo(msg);
				break;
			case 'test':
				ASSERT(false);
				break;
			
			default:
				if (!DoKeyCommand(msg))
					BView::MessageReceived(msg);
		}
	}
	catch (HErr& e)
	{
		e.DoError();
	}
	
	fStatus->SetOffset(fCaret);
} /* PText::MessageReceived */

void PText::Cut(int append)
{
	if (Doc()->IsReadOnly()) THROW(("Document is read-only"));

	Copy(append);
	Clear();
} /* PText::Cut */

void PText::Copy(int append)
{
	char *s;
	int size = abs(fCaret - fAnchor);
	
	if (append == 0 && fAppendNextCut)
		append = 1;

	fAppendNextCut = false;

	try
	{
		if (size)
		{
			s = (char *)malloc(size);
			FailNil(s);
			
			fText.Copy(s, min(fCaret, fAnchor), size);
			
			be_clipboard->Lock();
			if (append && be_clipboard->Data()->HasData("text/plain", B_MIME_DATA))
			{
				ssize_t aSize;
				const char *txt;
				FailOSErr(be_clipboard->Data()->FindData("text/plain", B_MIME_DATA, (const void **)&txt, &aSize));
				
				s = (char *)realloc(s, size + aSize);
				FailNil(s);

				if (append == 1)
				{
					memmove(s + aSize, s, size);
					memcpy(s, txt, aSize);
				}
				else
					memcpy(s + size, txt, aSize);

				size += aSize;
			}
			
			be_clipboard->Clear();
			be_clipboard->Data()->AddData("text/plain", B_MIME_DATA, s, size);
			be_clipboard->Commit();
			be_clipboard->Unlock();
			
			free(s);
		}
	}
	catch(HErr& e)
	{
		e.DoError();
	}
} /* PText::Cut */

void PText::Paste()
{
	if (Doc()->IsReadOnly()) THROW(("Document is read-only"));
	be_clipboard->Lock();
	
	try
	{
		int from, to;
		from = min(fCaret, fAnchor);
		to = min(max(fCaret, fAnchor), fText.Size());

		if (fAnchor != fCaret)
			Delete(from, to);
		
		ssize_t size;
		char *text;

		if (be_clipboard->Data()->FindData("text/plain", B_MIME_DATA, (const void**)&text, &size) == B_NO_ERROR)
		{
			Insert(text, size, from);
			SetCaret(from + size);
			if (fMark >= 0)
				SetMark(from);
		}
		
		TouchLines(Offset2Line(from));
	}
	catch (HErr& e)
	{
		e.DoError();
	}
	
	be_clipboard->Unlock();
} /* PText::Paste */

void PText::Clear()
{
	if (Doc()->IsReadOnly()) THROW(("Document is read-only"));

	int size = abs(fCaret - fAnchor);
	if (size)
	{
		int from, to;

		from = min(fCaret, fAnchor);
		to = max(fCaret, fAnchor);

		fAnchor = fCaret = from;

		Delete(from, to);
	}
} /* PText::Clear */

void PText::MenusBeginning()
{
	bool hasSelection = (fCaret != fAnchor);
	be_clipboard->Lock();
	bool hasClip = be_clipboard->Data()->HasData("text/plain", B_MIME_DATA);
	be_clipboard->Unlock();
	
	BMenuBar *mbar = dynamic_cast<BMenuBar*>(Window()->FindView("mbar"));
	FailNil(mbar);
	
	const char *what, *with;
	what = gFindDialog->FindString();
	with = gFindDialog->ReplaceString();
	
	mbar->FindItem(B_CUT)->SetEnabled(hasSelection);
	mbar->FindItem(B_COPY)->SetEnabled(hasSelection);
	mbar->FindItem(B_PASTE)->SetEnabled(hasClip);
	mbar->FindItem(msg_Clear)->SetEnabled(hasSelection);

	mbar->FindItem(B_SELECT_ALL)->SetEnabled(abs(fCaret - fAnchor) < fText.Size());
//	mbar->FindItem(msg_ChangeCase)->SetEnabled(false);//hasSelection);
	mbar->FindItem(msg_EnterSearchString)->SetEnabled(hasSelection);
	mbar->FindItem(msg_FindAgain)->SetEnabled(what != NULL && *what);
	mbar->FindItem(msg_FindSelection)->SetEnabled(hasSelection);
	mbar->FindItem(msg_Comment)->SetEnabled(strlen(fLangIntf->LineCommentStart()));
	mbar->FindItem(msg_Uncomment)->SetEnabled(strlen(fLangIntf->LineCommentStart()));
	
	mbar->FindItem(msg_Wrap)->SetEnabled(fSoftWrap);
	
	bool canReplace = CanReplace(what, gFindDialog->IgnoreCase(), gFindDialog->Grep());
	mbar->FindItem(msg_ReplaceAndFind)->SetEnabled(canReplace);
	mbar->FindItem(msg_ReplaceAll)->SetEnabled(what && *what);
	mbar->FindItem(msg_FindInNextFile)->SetEnabled(gFindDialog->IsInMultiFileState());

	if (fDoneCmds.size())
	{
		mbar->FindItem(msg_Undo)->SetEnabled(true);
		char s[64] = "Undo ";
		strcat(s, fDoneCmds.top()->Desc());
		mbar->FindItem(msg_Undo)->SetLabel(s);
	}
	else
		mbar->FindItem(msg_Undo)->SetEnabled(false);

	if (fUndoneCmds.size())
	{
		mbar->FindItem(msg_Redo)->SetEnabled(true);
		char s[64] = "Redo ";
		strcat(s, fUndoneCmds.top()->Desc());
		mbar->FindItem(msg_Redo)->SetLabel(s);
	}
	else
		mbar->FindItem(msg_Redo)->SetEnabled(false);
} /* PText::MenusBeginning */

void PText::ChangedInfo(BMessage *msg)
{
	const char *family, *style, *s;
	int32 i;
	float f;
	bool b, dirty = false;
	
	if (msg->FindString("family", &family) == B_OK &&
				msg->FindString("style", &style) == B_OK)
		fFont.SetFamilyAndStyle(family, style);
	
	if (msg->FindFloat("size", &f) == B_OK)
		fFont.SetSize(f);
	
	if (msg->FindInt32("tabs", &i) == B_OK)
		fTabStops = i;
	
	if (msg->FindBool("syntaxcol", &b) == B_OK)
		fSyntaxColoring = b;
	
	if (msg->FindBool("show invisibles", &b) == B_OK)
		fShowInvisibles = b;
	
	if (msg->FindBool("show tabs", &b) == B_OK)
		ShowTabStops(b);
	
	if (msg->FindInt32("encoding", &i) == B_OK)
		SetEncoding(i);
	
	if (msg->FindInt32("line breaks", &i) == B_OK)
		fLineEndType = i;
	
	if (msg->FindBool("softwrap", &b) == B_OK)
	{
		fSoftWrap = b;
		Doc()->ButtonBar()->SetOn(msg_SoftWrap, fSoftWrap);
	}
	
	if (msg->FindInt32("wraptyp", &i) == B_OK)
		fWrapType = i;
	
	if (msg->FindInt32("wrapwidth", &i) == B_OK)
		fWrapWidth = i;
	
	if (msg->FindString("mime", &s) == B_OK)
	{
		static_cast<PDoc*>(Window())->SetMimeType(s);
		dirty = true;
	}
	
	if (msg->FindInt32("language", &i) == B_OK)
		SetLanguage(i);
	
	SetDirty(dirty);
	ReInit();
	Invalidate();
} /* PText::ChangedInfo */
