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

	Created: 12/07/97 22:53:14 by Maarten Hekkelman
*/

#include "pe.h"
#include "PText.h"
#include "PApp.h"
#include "CLanguageAddOn.h"
#include "CLangIntf.h"
#include "PMessages.h"
#include "PItalicMenuItem.h"
#include "utf-support.h"
#include "PText.h"
#include "Utils.h"
#include "HError.h"
#include "HColorUtils.h"

const int kMaxNameSize = 256;

CLanguageProxy::CLanguageProxy(CLangIntf& intf, const char *text, int size,
	int encoding, int *starts, rgb_color *colors)
	: fInterface(intf)
{
	fCIndx = 0;
	fText = text;
	fSize = size;
	fEncoding = encoding;
	fFunctionScanHandler = NULL;
	fStarts = starts;
	fColors = colors;
	fNestLevel = 0;
} /* CLanguageProxy::CLanguageProxy */

CLanguageProxy::CLanguageProxy(CLangIntf& intf, PText& text,
	CFunctionScanHandler* handler)
	: fInterface(intf)
{
	fCIndx = 0;
	fText = text.Text();
	fSize = text.Size();
	fEncoding = text.Encoding();
	fFunctionScanHandler = handler;
	fStarts = NULL;
	fColors = NULL;
	fNestLevel = 0;
} /* CLanguageProxy::CLanguageProxy */

int CLanguageProxy::Move(int ch, int state)
{
	int c = fInterface.ec[ch];
	
	if (c && fInterface.chk[fInterface.base[state] + c] == state)
		return fInterface.nxt[fInterface.base[state] + c];
	else
		return 0;
} /* CLanguageProxy::Move */

int CLanguageProxy::IsKeyWord(int state)
{
	if (state)
		return fInterface.accept[state];
	else
		return 0;
} /* CLanguageProxy::IsKeyWord */

const char *CLanguageProxy::Text() const
{
	return fText;
} /* CLanguageProxy::Text */

int CLanguageProxy::Size() const
{
	return fSize;
} /* CLanguageProxy::Size */

void CLanguageProxy::SetColor(int start, int color)
{
	if (start < 0)
		start = 0;
	if (color < kLTextColor || color >= kLEndColor)
		color = kLTextColor;
	
	if (fStarts && fCIndx < 99 &&
		(fCIndx == 0 || fColors[fCIndx - 1] != gColor[color]))
	{
		if (fCIndx && start == fStarts[fCIndx - 1])
			fCIndx--;
		
		fColors[fCIndx] = gColor[color];
		fStarts[fCIndx] = start;
		fCIndx++;
	}
} /* CLanguageProxy::SetColor */

void CLanguageProxy::AddFunction(const char *name, const char *match, 
	int offset, bool italic, const char *params)
{
	FailNilMsg(fFunctionScanHandler, "Not a valid call in this context");
	fFunctionScanHandler->AddFunction(name, match, offset, italic, fNestLevel, params);
} /* CLanguageProxy::AddFunction */

void CLanguageProxy::AddInclude(const char *name, const char *open, bool italic)
{
	FailNilMsg(fFunctionScanHandler, "Not a valid call in this context");
	fFunctionScanHandler->AddInclude(name, open, italic);
} /* CLanguageProxy::AddFunction */

void CLanguageProxy::AddSeparator()
{
	FailNilMsg(fFunctionScanHandler, "Not a valid call in this context");
	fFunctionScanHandler->AddSeparator();
} /* CLanguageProxy::AddSeparator */

bool CLanguageProxy::Includes() const
{
	return gPopupIncludes;
} /* CLanguageProxy::Includes */

bool CLanguageProxy::Prototypes() const
{
	return gPopupProtos;
} /* CLanguageProxy::Prototypes */

bool CLanguageProxy::Types() const
{
	return gPopupFuncs;
} /* CLanguageProxy::Types */

void CLanguageProxy::CharInfo(const char *txt, int& unicode, int& len) const
{
	if (fEncoding == 0)
	{
		len = mcharlen(txt);
		unicode = municode(txt);
	}
	else
	{
		unicode = maptounicode(fEncoding, *txt);
		len = 1;
	}
} /* CharInfo */

bool CLanguageProxy::isalpha_uc(int unicode) const
{
	return ::isalpha_uc(unicode);
} /* isalpha_uc */

bool CLanguageProxy::isalnum_uc(int unicode) const
{
	return ::isalnum_uc(unicode);
} /* isalnum_uc */

bool CLanguageProxy::isspace_uc(int unicode) const
{
	return ::isspace_uc(unicode);
} /* isspace_uc */

void CLanguageProxy::IncNestLevel() {
	fNestLevel++;
} /* IncNestLevel */

void CLanguageProxy::DecNestLevel() {
	if (fNestLevel)
		fNestLevel--;
} /* DecNestLevel */

void CLanguageProxy::SetNestLevel(uint32 nestLevel) {
	fNestLevel = nestLevel;
} /* SetNestLevel */
