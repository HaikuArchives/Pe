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

	Created: 09/12/97 21:07:17
*/

#include "pe.h"
#include "PText.h"
#include "PDoc.h"
#include "MTextAddOn.h"
#include "PCmd.h"
#include "HError.h"

MTextAddOn::MTextAddOn(MIDETextView& inTextView)
	: fText(inTextView)
{
	fCmd = new PExtCmd(&fText, "Extension");
	fDirty = false;
} /* MTextAddOn::MTextAddOn */

MTextAddOn::MTextAddOn(MIDETextView& inTextView, const char *extension)
	: fText(inTextView)
{
	fCmd = new PExtCmd(&fText, extension);
	fDirty = false;
} /* MTextAddOn::MTextAddOn */

MTextAddOn::~MTextAddOn()
{
	fText.RedrawDirtyLines();
	if (fDirty) fText.RegisterCommand(fCmd);
} /* MTextAddOn::~MTextAddOn */

const char* MTextAddOn::Text()
{
	return fText.Text();
} /* MTextAddOn::Text */

int32 MTextAddOn::TextLength() const
{
	return fText.Size();
} /* MTextAddOn::TextLenght */

void MTextAddOn::GetSelection(int32 *start, int32 *end) const
{
	int32 a = fText.Anchor(), c = fText.Caret();
	*start = min(a, c);
	*end = max(a, c);
} /* MTextAddOn::GetSelection */

void MTextAddOn::Select(int32 newStart, int32 newEnd)
{
	fText.Select(newStart, newEnd, true, false);
} /* MTextAddOn::Select */

void MTextAddOn::Delete()
{
	int32 start, end;
	GetSelection(&start, &end);
	
	ExtAction action;
	action.aType = eaDelete;
	action.aOffset = start;
	action.aText = (char *)malloc(end - start + 1);
	FailNil(action.aText);
	fText.TextBuffer().Copy(action.aText, start, end - start);
	action.aText[end - start] = 0;
	fCmd->Actions().push_back(action);
	
	fText.Delete(start, end);
	fText.SetCaret(start);
	
	fDirty = true;
} /* MTextAddOn::Delete */

void MTextAddOn::Insert(const char* inText)
{
	Insert(inText, strlen(inText));

	fDirty = true;
} /* MTextAddOn::Insert */

void MTextAddOn::Insert(const char* text, int32 length)
{
	int32 start, end;
	GetSelection(&start, &end);
	
	if (start != end)
		Delete();

	ExtAction action;
	action.aType = eaInsert;
	action.aOffset = start;
	action.aText = (char *)malloc(length + 1);
	FailNil(action.aText);
	memcpy(action.aText, text, length);
	action.aText[length] = 0;
	fCmd->Actions().push_back(action);

	fText.Insert(text, length, start);

	fDirty = true;
} /* MTextAddOn::Insert */

BWindow* MTextAddOn::Window()
{
	return fText.Window();
} /* MTextAddOn::Window */

status_t MTextAddOn::GetRef(entry_ref&	outRef)
{
	status_t err = B_NO_ERROR;
	
	PDoc *doc = static_cast<PDoc*>(Window());
	if (doc->File())
		outRef = *doc->File();
	else
		err = B_ERROR;
	
	return err;
} /* MTextAddOn::GetRef */

