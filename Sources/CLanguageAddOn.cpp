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

CLanguageProxy::CLanguageProxy(CLangIntf& intf, const char *text, int size,
	int encoding, int *starts, rgb_color *colors)
	: fInterface(intf)
{
	fCIndx = 0;
	fText = text;
	fSize = size;
	fEncoding = encoding;
	fFunctions = NULL;
	fIncludes = NULL;
	fStarts = starts;
	fColors = colors;
} /* CLanguageProxy::CLanguageProxy */

CLanguageProxy::CLanguageProxy(CLangIntf& intf, PText& text, vector<void*>* includes,
	vector<void*>* functions)
	: fInterface(intf)
{
	fCIndx = 0;
	fText = text.Text();
	fSize = text.Size();
	fEncoding = text.Encoding();
	fFunctions = functions;
	fIncludes = includes;
	fStarts = NULL;
	fColors = NULL;
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
	if (fStarts &&
		(fCIndx == 0 || *(int *)&fColors[fCIndx - 1] != *(int *)&color) &&
		fCIndx < 99)
	{
		if (fCIndx && start == fStarts[fCIndx - 1])
			fCIndx--;
		
		if (color < kLTextColor || color > kLTagStringColor)
			color = kLTextColor;
		
		fColors[fCIndx] = gColor[color];
		fStarts[fCIndx] = start;
		fCIndx++;
	}
} /* CLanguageProxy::SetColor */

void CLanguageProxy::AddFunction(const char *name, const char *match, int offset, bool italic)
{
	FailNilMsg(fFunctions, "Not a valid call in this context");

	BMessage *msg = new BMessage(msg_JumpToProcedure);
	msg->AddInt32("offset", offset);
	msg->AddString("function", match);
	
	if (italic)
		fFunctions->push_back(new PItalicMenuItem(name, msg));
	else
		fFunctions->push_back(new BMenuItem(name, msg));
} /* CLanguageProxy::AddFunction */

void CLanguageProxy::AddInclude(const char *name, const char *open, bool italic)
{
	FailNilMsg(fIncludes, "Not a valid call in this context");
	
	BMessage *msg = new BMessage(msg_OpenInclude);
	msg->AddString("include", open);
	
	if (italic)
		fIncludes->push_back(new PItalicMenuItem(name, msg));
	else
		fIncludes->push_back(new BMenuItem(name, msg));
} /* CLanguageProxy::AddFunction */

void CLanguageProxy::AddSeparator()
{
	FailNilMsg(fFunctions, "Not a valid call in this context");
	fFunctions->push_back(new BSeparatorItem);
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
