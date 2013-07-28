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

#ifndef PTEXTBUFFER_H
#define PTEXTBUFFER_H

#include <Debug.h>
#include "utf-support.h"

class PTextBuffer {
public:
		PTextBuffer();
virtual	~PTextBuffer();

		void Insert(const char *bytes, int32 numBytes, int32 index);
		void Delete(int32 from, int32 to);
		void Replace(int32 offset, const char *txt);

		char operator[] (int32 indx) const;

		int32 Size() const;
		const char* Buffer();

		int32 ChangeCounter() const;

		int32 CharLen(int32 offset) const;
		int32 PrevCharLen(int32 offset) const;
		void CharInfo(int32 offset, int32& unicode, int32& len) const;

		void Copy(char *buf, int32 indx, int32 len) const;

		void PrintToStream();

		void ChangeToNL(int32 indx);

		PTextBuffer& operator=(const PTextBuffer& b);

protected:
		void MoveGap(int32 offset);
		void ResizeGap(int32 size);

		char *fText;
		int32 fLogicalSize;
		int32 fPhysicalSize;
		int32 fGap;
		int32 fGapSize;
		int32 fChangeCounter;
};


inline int32 PTextBuffer::ChangeCounter() const
{
	return fChangeCounter;
}

inline char PTextBuffer::operator[] (int32 indx) const
{
	ASSERT(indx >= 0);
	ASSERT(indx < fLogicalSize);
	if (indx < 0 || indx >= fLogicalSize)
		return 0;
	else
		return fText[indx < fGap ? indx : indx + fGapSize];
}

inline int32 PTextBuffer::Size() const {
	return fLogicalSize;
}

#endif // PTEXTBUFFER_H
