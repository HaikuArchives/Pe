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

#include "CTextBuffer.h"

#include "HError.h"

const int
	kBlockSize = 2048;

CTextBuffer::CTextBuffer()
	: fText(strdup(""))
	, fLogicalSize(0)
	, fPhysicalSize(0)
	, fGap(0)
	, fGapSize(0)
{
	FailNil(fText);
} /* CTextBuffer::CTextBuffer */

CTextBuffer::~CTextBuffer()
{
	free(fText);
} /* CTextBuffer::~CTextBuffer */

void CTextBuffer::Insert(const char *bytes, int numBytes, int index)
{
	ASSERT(numBytes >= 0);
	ASSERT(index >= 0 && index <= fLogicalSize);

	if (numBytes < 0 || index < 0 || index > fLogicalSize) return;
	
	index = max(min(fLogicalSize, index), 0);
	
	if (index != fGap)
		_MoveGap(index);
	
	if (fGapSize < numBytes)
		_ResizeGap(numBytes + kBlockSize);

	ASSERT(numBytes <= fGapSize);
	memcpy(fText + fGap, bytes, numBytes);
	
	fGapSize -= numBytes;
	fGap += numBytes;
	fLogicalSize += numBytes;
} /* CTextBuffer::Insert */

void CTextBuffer::Delete(int from, int to)
{
	int index = from;
	int cnt = to - from;
	
	ASSERT(cnt != 0);
	ASSERT(from < to);
	ASSERT(index >= 0);
	ASSERT(index < fLogicalSize);
	if (cnt <= 0 || from < 0 || index >= fLogicalSize)
		return;
	
	index = max(min(fLogicalSize - 1, index), 0);
	_MoveGap(index);
	
	fGapSize += cnt;
	fLogicalSize -= cnt;
	
	if (fGapSize > kBlockSize)
		_ResizeGap(kBlockSize);
} /* CTextBuffer::Delete */

void CTextBuffer::Overwrite(int offset, const char *txt)
{
	int len = strlen(txt);

	ASSERT(offset >= 0);
	ASSERT(offset + len <= fLogicalSize);
	ASSERT(len >= 0);

	if (offset < 0) offset = 0;
	if (offset + len > fLogicalSize) len = fLogicalSize - offset;
	if (len <= 0) return;

	if (offset + len <= fGap)
	{
		memcpy(fText + offset, txt, len);
	}
	else if (offset >= fGap)
	{
		memcpy(fText + fGapSize + offset, txt, len);
	}
	else
	{
		int p1, p2;
		p1 = fGap - offset;
		p2 = len - p1;
		
		memcpy(fText + offset, txt, p1);
		memcpy(fText + fGap + fGapSize, txt + p1, p2);
	}
} /* CTextBuffer::Replace */

const char* CTextBuffer::Buffer()
{
	_MoveGap(fLogicalSize);
	if (fGapSize < 1)
		_ResizeGap(kBlockSize);
	fText[fLogicalSize] = 0;

	return fText;
}

void CTextBuffer::CopyText(char *buf, int index, int len) const
{
	ASSERT(index >= 0);
	ASSERT(index + len <= fLogicalSize);
	ASSERT(len >= 0);

	if (index < 0) index = 0;
	if (index + len > fLogicalSize) len = fLogicalSize - index;
	if (len <= 0) return;

	if (index + len <= fGap)
	{
		memcpy(buf, fText + index, len);
	}
	else if (index >= fGap)
	{
		memcpy(buf, fText + fGapSize + index, len);
	}
	else
	{
		int p1, p2;
		p1 = fGap - index;
		p2 = len - p1;
		
		memcpy(buf, fText + index, p1);
		memcpy(buf + p1, fText + fGap + fGapSize, p2);
	}
} /* CTextBuffer::Copy */

int CTextBuffer::CharLen(int index) const
{
	ASSERT(index >= 0);
	ASSERT(index <= fLogicalSize);

	if (index < fLogicalSize && index >= 0)
	{
		char b[8];
		CopyText(b, index, min(7, fLogicalSize - index));
		b[7] = 0;
	
		return mcharlen(b);
	}
	else
		return 1;
} /* CTextBuffer::CharLen */

int CTextBuffer::PrevCharLen(int index) const
{
	ASSERT(index <= fLogicalSize);
	ASSERT(index >= 0);

	if (index > 0 && index <= fLogicalSize)
	{
		char b[8];
		int cnt = max(0, min(7, index));
		CopyText(b, index - cnt, cnt);
		b[cnt] = 0;
	
		return mprevcharlen(b + cnt);
	}
	else
		return 1;
} /* CTextBuffer::PrevCharLen */

void CTextBuffer::CharInfo(int offset, int& unicode, int& len) const
{
	ASSERT(offset >= 0);
//	ASSERT(index <= fLogicalSize);

	if (offset >= fLogicalSize || offset < 0)
	{
		unicode = 0;
		len = 1;
	}
	else
	{
		char b[8];
		CopyText(b, offset, min(7, fLogicalSize - offset));
		b[7] = 0;
	
		len = mcharlen(b);
		unicode = municode(b);
	}
} /* CTextBuffer::CharInfo */

void CTextBuffer::_MoveGap(int offset)
{
	if (fGap == offset) return;
	
	ASSERT(offset >= 0);
	ASSERT(offset <= fLogicalSize);
	
	int gapEnd = fGap + fGapSize;
	int src, dst, cnt = 0;
	
	if (offset > fGap)
	{
		int trail = fPhysicalSize - gapEnd;
		src = gapEnd;
		dst = fGap;
		cnt = min(trail, fGapSize + offset - src);
	}
	else
	{
		src = offset;
		dst = offset + fGapSize;
		cnt = gapEnd - dst;
	}
	
	if (cnt > 0)
	{
		ASSERT(dst + cnt <= fPhysicalSize);
		memmove(fText + dst, fText + src, cnt);
	}
	
	fGap = offset;
} /* CTextBuffer::_MoveGap */

void CTextBuffer::_ResizeGap(int gapSize)
{
	if (fGapSize == gapSize) return;
	
	char *t = fText;
	
	try
	{
		if (gapSize > fGapSize)
		{
			fText = (char *)realloc(fText, fLogicalSize + gapSize);
			FailNil(fText);
		}

		memmove(fText + fGap + gapSize, fText + fGap + fGapSize,
			fPhysicalSize - (fGap + fGapSize));
		
		if (gapSize < fGapSize)
		{
			fText = (char *)realloc(fText, fLogicalSize + gapSize);
			FailNil(fText);
		}

		fGapSize = gapSize;
		fPhysicalSize = fLogicalSize + gapSize;
	}
	catch (HErr& e)
	{
		fText = t;
		throw;
	}
} /* CTextBuffer::_ResizeGap */

