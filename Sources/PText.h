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

#ifndef PTEXT_H
#define PTEXT_H

typedef float g_unit_t;

#include <list>

#include <Messenger.h>
#include <View.h>

#include "PTextBuffer.h"
#include "CFontStyle.h"

#include <stack>

class BPopUpMenu;
class PCmd;
class PStatus;
class PExec;
class CLanguageInterface;
class CFunctionScanHandler;
class PSplitter;
class PDoc;
class PErrorWindow;

using std::stack;


struct LineInfo {
	unsigned long start;
	unsigned char state		: 8;
	unsigned char dirty		: 1;
	unsigned char nl		: 1;

	LineInfo() { start = state = dirty = nl = 0; };
	LineInfo(int32 nStart, int32 nState, bool nNl = true)
		: start(nStart), state(nState), dirty(1), nl(nNl) {};
};

struct WordInfo {
	short version;
	short classCnt;
	unsigned char classMap[256];
	short forwardCnt;
	unsigned char forwardTable[1];
};

typedef vector<LineInfo> VLineInfo;

class PText
	: public BView
{

//friend class PFontTabsCmd;	// lets keep life simple

enum {
	kNormalFont			= 0,
	kAltFont			= 1,
	kIndividualFont		= 2,
};

public:
			PText(BRect frame, PTextBuffer& txt, BScrollBar *bars[],
				  const char *ext);
			~PText();

			void ReInit();

			void SetStatus(PStatus *status);

virtual		void AttachedToWindow();

virtual		void Draw(BRect updateRect);
virtual		void MouseDown(BPoint where);
			void TrackDrag(BPoint where);
virtual		void KeyDown(const char *bytes, int32 numBytes);
			bool DoKeyCommand(BMessage *msg);
virtual		void MouseMoved(BPoint where, uint32 code, const BMessage *a_message);
virtual		void MessageReceived(BMessage *msg);
virtual		void WindowActivated(bool active);
			void MenusBeginning();

virtual		void FrameMoved(BPoint newPosition);
virtual		void FrameResized(float w, float h);

			void Pulse();

			void HandleDrop(BMessage *msg);

			void ProcessCommand(uint32 what, void *param);

			void SetText(const char *utf8Text, size_t size);
			const char* Text();
			int32 Size() const;
			void SetDefaultLanguageByExtension(const char *ext);
			void SetLanguage(int32 indx);
			int32 Language() const;

			void CollectSettings(BMessage& msg);
			void ApplySettings(const BMessage& msg);
			void GetSettingsMW(BPositionIO& set);
			void SetSettingsMW(BPositionIO& set);

			void SetFontKind(int32 kind);
			int32 FontKind() const;
			void FontChanged(bool reInit = true);

			void ChangedInfo(BMessage *msg);

			void GetSelectedText(char*& s, bool extend = false);
			void CopyBlock(char*& s, int32 from, int32 to);

			bool WaitMouseMoved(BPoint where);
			void ShowContextualMenu(BPoint where);
			void ShowFunctionMenu(BPoint where, int32 which);

			void SetDirty(bool dirty);
			bool IsDirty();

			void AdjustScrollBars();
			void ScrollBarChanged(BScrollBar *bar, g_unit_t newValue);
			void ScrollToCaret(bool keepContext);
			void ScrollToOffset(int32 offset, int32 part, bool keepContext);
			void ScrollToSelection(bool keepContext, bool centerVertically);
	virtual	void ScrollTo(BPoint p);

			void ShowTabStops(bool show);

			void DrawLine(int32 lineNr, g_unit_t y, bool buffer);
//			void DrawString(BView *v, const char *buf, int32 size, float hv, float& x, float y);
			void RedrawDirtyLines();
			void ShiftLines(int32 first, int32 dy, int32 part);
			void HiliteSelection();
			void ToggleCaret();
			void DrawCaret();
			void HideCaret();
			void ShowCaret();
			void InvalidateRange(int32 fromOffset, int32 toOffset, int32 part);
			void InvalidateRange(int32 fromOffset, int32 toOffset);
			void InvalidateLines(int32 fromLine, int32 toLine, int32 part);
			void InvalidateLines(int32 fromLine, int32 toLine);

			status_t PageSetup();
			status_t Print();

			int32 FindWord(uint8 key, bool subWord = false);
			int32 FindWord(int32 indx, uint8 key, bool mouseSelect = false, bool subWord = false);
			int32 FindNextWord(int32 offset);
			void RecalculateLineBreaks();
			void InsertLinebreaks();
			void RemoveLinebreaks();
			int32 FindLineBreak(int32 offset, bool hard = false);
			int32 RewrapLines(int32 from, int32 to, bool hard = false);
			void Kiss(int32 ch);
			int32 FindTheOther(int32 offset, int32 ch);
			void AutoIndent(int32 offset);
			void SmartBrace();
			void ScanForFunctions(CFunctionScanHandler& handler);
			void HashLines(vector<int>& hv, bool ignoreCase, bool ignoreWhite);

			int32 Offset2Line(int32 offset);
			BPoint Offset2Position(int32 offset, int32 part = -1);
			int32 LinePosition2Offset(int32 line, g_unit_t position);
			int32 Position2Offset(BPoint where, int32 part = -1);
			int32 LineStart(int32 line) const;
			int32 LineState(int32 line) const;
			int32 LineCount() const;
			bool SoftStart(int32 line) const;
			void OffsetLineBreaks(int32 bytes, int32 line);
			int32 DeleteLineBreaks(int32 from, int32 to);
			int32 Offset2Column(int32 offset);
			int32 Column2Offset(int32 lineNr, int32 column);
			int32 RealLine2Line(int32 lineNr);

			// block select mode:
			bool IsFixedFont() const;
			bool IsBlockSelect() const				{ return fBlockSelect; }
			void SetBlockSelect(bool block)			{ fBlockSelect = block; }
			void BlockOffsetsForLine(int32 lineNr, int32& startOffset, int32& endOffset);

			void Selection2Region(BRegion& rgn, int32 part = -1);
			void ChangeSelection(int32 newAnchor, int32 newCaret, bool block = false);
			void SelectLine(int32 lineNr);
			void SelectParagraph();
			void InvertRegion(BRegion& rgn);

			void CharKeyDown(const char *bytes, int32 numBytes);
			void BackspaceKeyDown();
			void DeleteKeyDown();
			void GlossaryKey(int32 ch, int32 modifiers);
			void GlossaryButton(const char *glossy);

			g_unit_t TextWidth(int32 offset, int32 len) const;
			g_unit_t StringWidth(const char *buf, int32 len) const;

			void Insert(const char *text, int32 textLen, int32 offset);
			void Delete(int32 from, int32 to);
			void RestyleDirtyLines(int32 from);
			void TextChanged(int32 from, int32 to);
			void TypeString(const char *string);

			void Cut(int32 append = 0);
			void Copy(int32 append = 0);
			void Paste();
			void Clear();

			void TouchLine(int32 lineNr);
			void TouchLines(int32 from, int32 to = -1);

			int32 Anchor();
			int32 Caret();
			void SetCaret(int32 newCaret);
			void SetMark(int32 offset);
			void Select(int32 anchor, int32 caret, bool update, bool block);
			void CenterSelection();

			void RegisterCommand(PCmd *cmd);
			void Undo();
			void Redo();
			void ResetUndo();
			void FlushUndo();

			void Find(uint32 msg, void *args);
			bool CanReplace(const char *s, bool ignoreCase, bool regx);
			bool FindNext(const char *what, int32& offset, bool ignoreCase,
					bool wrap, bool backward, bool entireWord, bool regx,
					bool scroll = true, int32* foundLen = NULL);
			bool IsIncSearching() const;
			void DoIncSearch(bool forward);
			void IncSearchKey(const char *bytes, int32 numBytes);

			void JumpToFunction(const char *func, int32 offset);
			void FindNextError(bool backward);

			void ExecuteSelection();
			void KillCurrentJob();
			void PrepareForOutput();
			void SetCWD(const char *cwd);
			const char *GetCWD() const;
			void SendTextToJob(const char *txt);
			void SetErrorWindow(PErrorWindow *window);

			void SetSplitter(PSplitter *splitter);
			void TrackSplitter(BPoint where);
			void SplitterMoved(g_unit_t dy);
			void DoneMovingSplitter();
			void SplitWindow();
			void UnsplitWindow();
			void SwitchPart(int32 toPart);

			const PTextBuffer& TextBuffer() const;
			const char* FindString() const;
			const char* ReplaceString() const;
			bool IsExecuting() const;
			const BFont& Font() const;
			int32 TabStops() const;
			bool SyntaxColoring() const;
			bool ShowInvisibles() const;
			bool SoftWrap() const;
			int32 WrapType() const;
			int32 WrapWidth() const;
			PDoc* Doc() const;

			void SelectionChanged();
			void TextBufferChanged();
			void ActivePartChanged(int32 oldActivePart);
			void LinesShifted(int32 first, int32 dy);
			void PrepareForRedrawDirtyLines();
			void UpdateBraceHighlights();

private:
			void VerticallyScrollToSelection(int32 startOffset,
											 int32 endOffset,
											 bool keepContext,
											 bool centered,
											 int32 part = -1);
			void HorizontallyScrollToSelection(int32 startOffset,
											   int32 endOffset,
											   bool keepContext);
			BRect CursorFrame(int32 caret);
			void SetupBitmap();
			BRect PartBounds(int32 part);
			bool OffsetIsOutsideOfContextArea(int32 offset);
			void NavigateOverFunctions(char direction);

			BBitmap *fLineMap;
			BView	*fLineView;
			BFont fFont;
			font_height fFH;
			BPopUpMenu *fMainPopUp;

			bigtime_t fNextCaret;
			bigtime_t fLastMouseTime;
			BScrollBar *fHScrollBar, *fVScrollBar1, *fVScrollBar2;
			stack<PCmd*> fDoneCmds, fUndoneCmds;
			int32 fMouseClicks;

			g_unit_t fLineHeight, fTabWidth;
			g_unit_t fWalkOffset;

			bool fUsingDefaultLanguage;
			CLanguageInterface *fLangIntf;
			BRect fBounds;
			PTextBuffer& fText;
			VLineInfo fLineInfo;
			CFontStyle *fMetrics;
			int32 fAnchor, fCaret, fStoredCaret;
			bool fBlockSelect;
			bool fCaretVisible;	// logically visible
			bool fCaretDrawn;	// actually drawn on screen
			long fCaretShielded;
			PStatus *fStatus;
			PCmd* fLastCommand;
			PExec *fExec;
			bool fWindowActive;
			bool fSyntaxColoring;
			bool fShowInvisibles;
			int32 fSavedCaret, fSavedAnchor;
			int32 fDragStart;
			int32 fDragButtons;
			PSplitter *fSplitter;
			g_unit_t fSplitAt;
			bool fSplitCursorShown;
			int32 fActivePart, fOPCaret, fOPAnchor, fOPMark;
			BPoint fLastMouseDown;
			int32 fIncSearch;
			char fIncPat[256];
			int32 fIncCaret;
			int32 fMark;
			int32 fNrArgument;
			int32 fShift;
			int32 fLastKillPoint;
			bool fAppendNextCut;
			PCmd* fLastSavedStateCmd;
			float fDefaultCharWidth;
			BMessage *fPrintSettings;
			BMessenger fErrorWindowMessenger;

static		PText *sfDragSource;

			rgb_color fLowColor, fTextColor;
			rgb_color fKeywordColor, fStringColor, fCommentColor;

			struct HighlightInfo {
				HighlightInfo()
					: fromOffset(-1)
				{
				}

				int	fromOffset;
				int32 toOffset;
			};
			typedef std::list<HighlightInfo*> HighlightList;

			struct BraceHighlightInfo : HighlightInfo {
				int	line;
			};

			int32 fHighlightCursor;
			int32 fHighlightChangeCounter;
			int32 fHighlightPart;
			BraceHighlightInfo fBraceHighlight1;
			BraceHighlightInfo fBraceHighlight2;
			HighlightList fHighlights;

// saveable settings
			int32 fTabStops;
			char *fCWD;
			bool fSoftWrap;
			int32 fWrapType;
			int32 fWrapWidth;
			int32 fFontKind;
};

inline void PText::HideCaret() {
	if (fCaretVisible) ToggleCaret();
	fNextCaret = system_time() + 500000;
}

inline void PText::ShowCaret() {
	if (!fCaretVisible) ToggleCaret();
	fNextCaret = system_time() + 500000;
}

inline int32 PText::Anchor() {
	return fAnchor;
}

inline int32 PText::Caret() {
	return fCaret;
}

inline const PTextBuffer& PText::TextBuffer() const {
	return fText;
}

inline bool PText::IsExecuting() const {
	return fExec != NULL;
}

inline int32 PText::TabStops() const {
	return fTabStops;
}

inline bool PText::SyntaxColoring() const {
	return fSyntaxColoring;
}

inline bool PText::ShowInvisibles() const {
	return fShowInvisibles;
}

inline int32 PText::LineCount() const {
	return fLineInfo.size();
}

inline int32 PText::LineStart(int32 line) const {
	if (line >= 0 && line < LineCount())
		return fLineInfo[line].start;
	else if (line < 0)
		return 0;
	else
	{
		ASSERT(line < LineCount());
		return fText.Size();
	}
}

inline int32 PText::LineState(int32 line) const {
	if (line >= 0 && line < LineCount())
		return fLineInfo[line].state;
	else
	{
#if DEBUG
		ASSERT(line >= 0 && line < LineCount());
#endif
		return 0;
	}
}

inline g_unit_t PText::StringWidth(const char *buf, int32 len) const
{
	return fMetrics->StringWidth(buf, len);
} /* PText::StringWidth */

inline bool PText::IsIncSearching() const
{
	return fIncSearch != 0;
} /* PText::IsIncSearching */

inline bool PText::SoftWrap() const
{
	return fSoftWrap;
} /* PText::SoftWrap */

inline int32 PText::WrapType() const
{
	return fWrapType;
} /* PText::WrapType */

inline int32 PText::WrapWidth() const
{
	return fWrapWidth;
} /* PText::WrapWidth */

inline bool PText::SoftStart(int32 line) const
{
	return ! fLineInfo[line].nl;
} /* PText::SoftStart */

inline const char* PText::GetCWD() const
{
	return fCWD;
} /* PText::GetCWD */

inline const BFont& PText::Font() const
{
	return fFont;
} // PText::Font

inline bool PText::IsFixedFont() const
{
	return fFont.IsFixed();
} // PText::IsFixedFont

inline void PText::ProcessCommand(uint32 what, void *param)
{
	BMessage msg(what);
	msg.AddPointer("param", param);
	BMessenger(this).SendMessage(&msg);
}

#endif // PTEXT_H
