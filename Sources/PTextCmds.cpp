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

	Created: 02/18/98 17:44:08
*/

#include "pe.h"
#include "PCmd.h"
#include "PText.h"
#include "PTextCmds.h"

#include <algorithm>

#pragma mark --- Shift Command ---

PShiftCmd::PShiftCmd(PText *txt, bool right)
	: PCmd("Shift", txt)
{
	fShiftRight = right;
	
	fFrom = min(fText->Caret(), fText->Anchor());
	fTo = max(fText->Caret(), fText->Anchor());
	if (fTo == fFrom || fTo > fText->LineStart(fText->Offset2Line(fTo)))
	{
		int lt = fText->Offset2Line(fTo);
		if (lt < fText->LineCount() - 1)
			fTo = fText->LineStart(lt + 1);
		else
			fTo = fText->Size();
	}
} /* PShiftCmd::PShiftCmd */

void PShiftCmd::Do()
{
	int i, lf, c;
	
	c = fText->LineCount();
	lf = fText->Offset2Line(fFrom);

	for (i = lf; i < fText->Offset2Line(fTo); i++)
	{
		if (! fText->SoftStart(i))
		{
			int ix = fText->LineStart(i);
			if (fShiftRight)
			{
				fText->Insert("\t", 1, ix);
				fTo++;
			}
			else if (fText->TextBuffer()[ix] == '\t')
			{
				fText->Delete(ix, ix + 1);
				fTo--;
			}
		}
	}
	
	fText->Select(fText->LineStart(lf), fText->LineStart(fText->Offset2Line(fTo)), true, false);
	fFrom = min(fText->Caret(), fText->Anchor());
	fTo = max(fText->Caret(), fText->Anchor());
	
	if (fText->LineCount() != c)
		Redraw();
	else
		Update();
} /* PShiftCmd::Do */

void PShiftCmd::Undo()
{
	int i, lf, c;
	
	c = fText->LineCount();
	lf = fText->Offset2Line(fFrom);
	
	for (i = lf; i < fText->Offset2Line(fTo); i++)
	{
		if (! fText->SoftStart(i))
		{
			int ix = fText->LineStart(i);
			if (fShiftRight)
			{
				fText->Delete(ix, ix + 1);
				fTo--;
			}
			else
			{
				fText->Insert("\t", 1, ix);
				fTo++;
			}
		}
	}

	fText->Select(fText->LineStart(lf), fText->LineStart(fText->Offset2Line(fTo)), true, false);
	fFrom = min(fText->Caret(), fText->Anchor());
	fTo = max(fText->Caret(), fText->Anchor());

	if (fText->LineCount() != c)
		Redraw();
	else
		Update();
} /* PShiftCmd::Undo */

#pragma mark -
#pragma mark --- Twiddle Command ---

PTwiddleCmd::PTwiddleCmd(PText *txt)
	: PCmd("Twiddle", txt)
{
	fFrom = min(fText->Caret(), fText->Anchor());
	fTo = max(fText->Caret(), fText->Anchor());
	
	if (fFrom == fTo)
	{
		int line;
		line = fText->Offset2Line(fFrom);
		
		if (fFrom == fText->LineStart(line))
		{
			fTo += txt->TextBuffer().CharLen(fTo);
			fTo += txt->TextBuffer().CharLen(fTo);
		}
		else if (fFrom == fText->Size() || fFrom == fText->LineStart(line + 1) - 1)
		{
			fFrom -= txt->TextBuffer().PrevCharLen(fFrom);
			fFrom -= txt->TextBuffer().PrevCharLen(fFrom);
		}
		else
		{
			fTo += txt->TextBuffer().CharLen(fTo);
			fFrom -= txt->TextBuffer().PrevCharLen(fFrom);
		}
	}
	
	if (fText->TextBuffer()[fFrom] == '\n' ||
		fText->TextBuffer()[fTo - 1] == '\n' ||
		fTo > fText->Size() || fFrom < 0)
	{
		THROW((0));
	}
} /* PTwiddleCmd::PTwiddleCmd */

void PTwiddleCmd::Do()
{
	char *s;
	int size = fTo - fFrom;
	
	fText->ChangeSelection(fFrom, fTo);
	fText->GetSelectedText(s);

// reverse text, keep in mind that we might have long chars in between
	char *t = (char *)alloca(size + 1);
	int x = size;

	while (*s)
	{
		int l = mcharlen(s);
		x -= l;
		memcpy(t + x, s, l);
		s += l;
	}

	fText->Delete(fFrom, fTo);
	fText->Insert(t, size, fFrom);

	Redraw();
} /* PTwiddleCmd::Do */

void PTwiddleCmd::Undo()
{
	Do();
} /* PTwiddleCmd::Undo */

#pragma mark -
#pragma mark --- Comment Command ---

PCommentCmd::PCommentCmd(PText *txt, bool comment,
			const char *before, const char *after)
	: PCmd(comment ? "Comment" : "Uncomment", txt)
{
	fComment = comment;
	
	fFrom = min(fText->Caret(), fText->Anchor());
	fTo = max(fText->Caret(), fText->Anchor());
	
	if (strlen(before) > 7 || strlen(after) > 7) THROW(("comment strings too long"));
	
	strcpy(fBefore, before);
	strcpy(fAfter, after);
	
	if (fTo == fFrom || fTo > fText->LineStart(fText->Offset2Line(fTo)))
	{
		int lt = fText->Offset2Line(fTo);
		if (lt < fText->LineCount() - 1)
			fTo = fText->LineStart(lt + 1);
		else
			fTo = fText->Size();
	}
} /* PCommentCmd::PCommentCmd */

void PCommentCmd::Do()
{
	int i, lf, c;
	
	c = fText->LineCount();
	lf = fText->Offset2Line(fFrom);

	for (i = lf; i < fText->Offset2Line(fTo); i++)
	{
		if (! fText->SoftStart(i))
		{
			if (fComment)
				CommentLine(i);
			else if (UncommentLine(i))
				fLines.push_back(i);
		}
	}
	
	fText->Select(fText->LineStart(lf), fText->LineStart(fText->Offset2Line(fTo)), true, false);
	fFrom = min(fText->Caret(), fText->Anchor());
	fTo = max(fText->Caret(), fText->Anchor());

	if (fText->LineCount() != c)	
		Redraw();
	else
		Update();
} /* PCommentCmd::Do */

void PCommentCmd::Undo()
{
	int i, lf, c;

	c = fText->LineCount();
	lf = fText->Offset2Line(fFrom);
	
	if (fComment)
	{
		for (i = lf; i < fText->Offset2Line(fTo); i++)
			if (! fText->SoftStart(i))
				UncommentLine(i);
	}
	else
	{
		vector<int>::iterator ii;
		
		for (ii = fLines.begin(); ii != fLines.end(); ii++)
			CommentLine(*ii);
		
		fLines.erase(fLines.begin(), fLines.end());
	}
	
	fText->Select(fText->LineStart(lf), fText->LineStart(fText->Offset2Line(fTo)), true, false);
	fFrom = min(fText->Caret(), fText->Anchor());
	fTo = max(fText->Caret(), fText->Anchor());
	
	if (fText->LineCount() != c)	
		Redraw();
	else
		Update();
} /* PCommentCmd::Undo */

void PCommentCmd::CommentLine(int line)
{
	int ix = fText->LineStart(line);

	fText->Insert(fBefore, strlen(fBefore), ix);
	
	ix = fText->LineCount() > line ? fText->LineStart(line + 1) - 1 : fText->Size();
	fText->Insert(fAfter, strlen(fAfter), ix);
	
	fTo += strlen(fBefore) + strlen(fAfter);
} /* PCommentCmd::CommentLine */

bool PCommentCmd::UncommentLine(int line)
{
	char b[8];
	int j = strlen(fBefore);
	int ix = fText->LineStart(line);
	
	b[j] = 0;
	while (j--)
		b[j] = fText->TextBuffer()[ix + j];

	if (strcmp(b, fBefore) == 0)
	{
		fText->Delete(ix, ix + strlen(fBefore));

		if (strlen(fAfter))
		{
			ix = line < fText->LineCount() - 1 ? fText->LineStart(line + 1) : fText->Size();
			j = strlen(fAfter);
			ix -= j + 1;
			
			b[j] = 0;
			while (j--)
				b[j] = fText->TextBuffer()[ix + j];
	
			if (strcmp(b, fAfter) == 0)
				fText->Delete(ix, ix + strlen(fAfter));
		}

		fTo -= strlen(fBefore) + strlen(fAfter);

		return true;
	}
	else
		return false;
} /* PCommentCmd::UncommentLine */

#pragma mark -
#pragma mark --- Wrap Command ---

PWrapCmd::PWrapCmd(PText *txt)
	: PCmd("Wrap", txt)
{
	fCaret = txt->Caret();
	fAnchor = txt->Anchor();
	fSavedText = NULL;
} /* PWrapCmd::PWrapCmd */

PWrapCmd::~PWrapCmd()
{
	if (fSavedText) free(fSavedText);
} /* PWrapCmd::PWrapCmd */

void PWrapCmd::Do()
{
	fText->Select(fAnchor, fCaret, true, false);
	fText->GetSelectedText(fSavedText);
	fText->InsertLinebreaks();
	fNewLen = abs(fText->Caret() - fText->Anchor());
	Redraw();
} /* PWrapCmd::Do */

void PWrapCmd::Undo()
{
	int a = min(fAnchor, fCaret);
	fText->Delete(a, a + fNewLen);
	if (fSavedText)
	{
		fText->Insert(fSavedText, strlen(fSavedText), a);
		free(fSavedText);
		fSavedText = NULL;
	}
	fText->Select(fAnchor, fCaret, true, false);
	
	Redraw();
} /* PWrapCmd::Undo */

#pragma mark -
#pragma mark --- Unwrap Command ---

PUnwrapCmd::PUnwrapCmd(PText *txt)
	: PCmd("Unwrap", txt)
{
	fCaret = txt->Caret();
	fAnchor = txt->Anchor();
	
	if (fCaret == fAnchor)
	{
		fAnchor = 0;
		fCaret = fText->Size() - 1;
	}
	
	fSavedText = NULL;
} /* PUnwrapCmd::PUnwrapCmd */

PUnwrapCmd::~PUnwrapCmd()
{
	if (fSavedText) free(fSavedText);
} /* PUnwrapCmd::~PUnwrapCmd */

void PUnwrapCmd::Do()
{
	fText->Select(fAnchor, fCaret, true, false);
	fText->GetSelectedText(fSavedText);
	fText->RemoveLinebreaks();
	fNewLen = abs(fText->Caret() - fText->Anchor());
	Redraw();
} /* PUnwrapCmd::Do */

void PUnwrapCmd::Undo()
{
	int a = min(fAnchor, fCaret);
	fText->Delete(a, a + fNewLen);
	if (fSavedText)
	{
		fText->Insert(fSavedText, strlen(fSavedText), a);
		free(fSavedText);
		fSavedText = NULL;
	}
	fText->Select(fAnchor, fCaret, true, false);
	
	Redraw();
} /* PUnwrapCmd::Undo */

#pragma mark -
#pragma mark --- Justify Command ---

PJustifyCmd::PJustifyCmd(PText *txt)
	: PCmd("Justify", txt)
{
} /* PJustifyCmd::PJustifyCmd */

PJustifyCmd::~PJustifyCmd()
{
	if (fSavedText) free(fSavedText);
} /* PJustifyCmd::~PJustifyCmd */

void PJustifyCmd::Do()
{
	if (fText->Caret() == fText->Anchor())
		fText->SelectParagraph();
	
	fAnchor = min(fText->Anchor(), fText->Caret());
	fOldLen = max(fText->Anchor(), fText->Caret()) - fAnchor;
	
	fText->GetSelectedText(fSavedText);
	
	char *t = (char *)malloc(strlen(fSavedText) + 1);
	FailNil(t);
	
	char *p, *q;
	
	p = fSavedText;
	q = t;
	
	while (*p)
	{
		if (isspace(*p))
		{
			while (isspace(*++p))
				;
			*q++ = ' ';
		}
		else
			*q++ = *p++;
	}
	
	*q = 0;
	fNewLen = strlen(t);
	
	fText->Delete(fAnchor, fAnchor + fOldLen);
	fText->Insert(t, fNewLen, fAnchor);
	fText->Select(fAnchor, fAnchor + fNewLen, true, false);
	fText->RewrapLines(fAnchor, fAnchor + fNewLen, true);
	Update();
	fText->InsertLinebreaks();
	fNewLen = abs(fText->Anchor() - fText->Caret());
} /* PJustifyCmd::Do */

void PJustifyCmd::Undo()
{
	char *t;
	
	fText->Select(fAnchor, fAnchor + fNewLen, true, false);
	fText->GetSelectedText(t);
	
	fText->Delete(fAnchor, fAnchor + fNewLen);
	fText->Insert(fSavedText, fOldLen, fAnchor);
	
	swap(fSavedText, t);
	free(t);
	swap(fOldLen, fNewLen);
	
	fText->Select(fAnchor, fAnchor + fNewLen, true, false);
	
	Update();
} /* PJustifyCmd::Undo */

void PJustifyCmd::Redo()
{
	Undo();
} /* PJustifyCmd::Redo */

#pragma mark -
#pragma mark --- Change Case Command ---

PChangeCaseCmd::PChangeCaseCmd(PText *txt, int newCase)
	: PCmd("Change Case", txt)
{
	fText->GetSelectedText(fSavedText);
	
	if (fSavedText)
	{
		fAnchor = min(fText->Anchor(), fText->Caret());
		fCaret = max(fText->Anchor(), fText->Caret());
	}
	else
	{
		fAnchor = 0;
		fCaret = fText->Size();
		fSavedText = strdup(fText->Text());
	}
	
	ChangeCase(newCase);
} /* PChangeCaseCmd::PChangeCaseCmd */

PChangeCaseCmd::~PChangeCaseCmd()
{
	free(fSavedText);
} /* PChangeCaseCmd::~PChangeCaseCmd */

void PChangeCaseCmd::Do()
{
	char *p;
	
	fText->Select(fAnchor, fCaret, true, false);
	fText->GetSelectedText(p);
	fText->Delete(fAnchor, fCaret);
	fText->Insert(fSavedText, fCaret - fAnchor, fAnchor);
	
	free(fSavedText);
	fSavedText = p;
		
	Redraw();
} /* PChangeCaseCmd::Do */

void PChangeCaseCmd::Undo()
{
	Do();
} /* PChangeCaseCmd::Undo */

void PChangeCaseCmd::ChangeCase(int newCase)
{
	char *p = fSavedText;
	int state = 0;
	
	switch (newCase)
	{
		case 1:
			while (*p)
			{
				if (isalpha(*p))
					*p = tolower(*p);
				p++;
			}
			break;
		case 2:
			while (*p)
			{
				if (isalpha(*p))
					*p = toupper(*p);
				p++;
			}
			break;
		case 3:
			while (*p)
			{
				if (isalnum(*p) && state == 0)
				{
					*p = toupper(*p);
					state = 1;
				}
				else if (isalnum(*p) && state == 1)
					*p = tolower(*p);
				else if (!isalnum(*p) && state == 1)
					state = 0;
				p++;
			}
			break;
		case 4:
			while (*p)
			{
				if (isalnum(*p) && state == 0)
				{
					*p = toupper(*p);
					state = 1;
				}
//				else if (isalnum(*p) && state == 1)
//					*p = tolower(*p);
				else if (*p == '.' && state == 1)
					state = 0;
				p++;
			}
			break;
		case 5:
			while (*p)
			{
				if (isalnum(*p) && state == 0)
				{
					*p = toupper(*p);
					state = 1;
				}
//				else if (isalnum(*p) && state == 1)
//					*p = tolower(*p);
				else if (*p == '\n' && state == 1)
					state = 0;
				p++;
			}
			break;
	}
} /* PChangeCaseCmd::ChangeCase */

#pragma mark -
#pragma mark --- Change Encoding Command ---

PEncodingCmd::PEncodingCmd(PText *txt, int from, int to)
	: PCmd("Change Encoding", txt)
{
	fSourceEncoding = from;
	fDestEncoding = to;
	
	fAnchor = txt->Anchor();
	fCaret = txt->Caret();
	
	if (fAnchor == fCaret)
	{
		fAnchor = 0;
		fCaret = txt->Size();
	}
	
	fSaved = (char *)malloc(fCaret - fAnchor);
	fSrcLen = fCaret - fAnchor;
	
	FailNil(fSaved);
	txt->TextBuffer().Copy(fSaved, fAnchor, fSrcLen);
	
	fPrevEncoding = txt->Encoding();
} /* PEncodingCmd::PEncodingCmd */

PEncodingCmd::~PEncodingCmd()
{
	if (fSaved) free(fSaved);
} /* PEncodingCmd::~PEncodingCmd */

void PEncodingCmd::Do()
{
	if (fSourceEncoding >= 0 && fDestEncoding >= 0)
	{
		long tl = 3 * fSrcLen;
		char *p1 = (char *)malloc(tl);
		FailNil(p1);
		int32 state = 0;
		
		FailOSErr(convert_to_utf8(fSourceEncoding, fSaved, &fSrcLen, p1, &tl, &state));
		
		fDstLen = 2 * tl;
		char *p2 = (char *)malloc(fDstLen);
		FailNil(p2);
		
		FailOSErr(convert_from_utf8(fDestEncoding, p1, &tl, p2, &fDstLen, &state));
		
		free(p1);
		
		fCaret = fAnchor + fSrcLen;
		fText->Delete(fAnchor, fCaret);
		fText->Insert(p2, fDstLen, fAnchor);
		
		free(p2);
		
		fText->SetEncoding(fDestEncoding + 1);
		if (fText->Caret() != fText->Anchor())
			fText->SetCaret(fAnchor + fDstLen);
	}
	else if (fSourceEncoding >= 0)
	{
		fDstLen = 3 * fSrcLen;
		
		char *p = (char *)malloc(fDstLen);
		FailNil(p);
		
		int32 state = 0;

		FailOSErr(convert_to_utf8(fSourceEncoding, fSaved, &fSrcLen,
			p, &fDstLen, &state));

		fCaret = fAnchor + fSrcLen;
		fText->Delete(fAnchor, fCaret);
		fText->Insert(p, fDstLen, fAnchor);
		free(p);
		
		fText->SetEncoding(0);
		if (fText->Caret() != fText->Anchor())
			fText->SetCaret(fAnchor + fDstLen);
	}
	else if (fDestEncoding >= 0)
	{
		fDstLen = fSrcLen * 2;
		
		char *p = (char *)malloc(fDstLen);
		FailNil(p);
		
		int32 state = 0;
		
		FailOSErr(convert_from_utf8(fDestEncoding, fSaved, &fSrcLen,
			p, &fDstLen, &state));

		fCaret = fAnchor + fSrcLen;
		fText->Delete(fAnchor, fCaret);
		fText->Insert(p, fDstLen, fAnchor);
		free(p);
		
		fText->SetEncoding(fDestEncoding + 1);
		if (fText->Caret() != fText->Anchor())
			fText->SetCaret(fAnchor + fDstLen);
	}

	Update();
} /* PEncodingCmd::Do */

void PEncodingCmd::Undo()
{
	fText->Delete(fAnchor, fAnchor + fDstLen);
	fText->Insert(fSaved, fSrcLen, fAnchor);
	fText->SetEncoding(fSourceEncoding + 1);
	fText->Select(fAnchor, fCaret, true, false);
	
	Update();
} /* PEncodingCmd::Undo */

