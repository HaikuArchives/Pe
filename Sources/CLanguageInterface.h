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

	Created: 09/19/97 10:49:36
*/

#ifndef CLANGUAGEINTERFACE_H
#define CLANGUAGEINTERFACE_H

#include <map>
#include <vector>

#include <image.h>
#include <String.h>

class CFunctionScanHandler;
class CLanguageProxy;
class PText;
struct rgb_color;

class CLanguageInterface {
friend class CLanguageProxy;
public:
		~CLanguageInterface();

		void Balance(PText& text);
		bool Balance(PText& text, int32& start,  int32& end);
		void ScanForFunctions(PText& text, CFunctionScanHandler& handler);
		void ColorLine(const char *buf, int32 size,
				int32& state, int32 *starts, rgb_color *colors);
		int32 FindNextWord(PText& text, int32 offset, int32& mlen);

		const char *Text() const;
		int32 Size() const;

		const char *Name() const;
		const char *Extensions() const;
		void SetExtensions(const char *ext);

static	void ChooseDefault();

		int32 *Starts() const;
		rgb_color *Colors() const;

static	void SetupLanguageInterfaces();
static	CLanguageInterface* FindIntf(int32 index)	{ return fInterfaces[index]; }
static	CLanguageInterface* FindByExtension(const char *filename);
static CLanguageInterface* FindByName(const char *language);

static CLanguageInterface* NextIntf(int32& cookie);
static int32 GetIndex(const CLanguageInterface* intf);

		const char* LineCommentStart() const;
		const char* LineCommentEnd() const;

protected:
		CLanguageInterface();
		CLanguageInterface(const char *path, image_id image);

		void InitTables(const char *kwFile);
		void RegisterExtension(const char *ext);
		void GenerateKeywordMap(const char *ext) const;

static	unsigned char *sfWordBreakTable;

		const char *fLanguage, *fExtensions, *fKeywordFile;
		const char *fLineCommentStart, *fLineCommentEnd;
		bool (*fBalance)(CLanguageProxy& proxy, int32& start, int32& end);
		void (*fScanForFunctions)(CLanguageProxy& proxy);
		void (*fColorLine)(CLanguageProxy& proxy, int32& state);
		int32 (*fFindNextWord)(const CLanguageProxy& proxy);
		image_id fImage;

		// member introduced for proper interface version detection:
		int16 fInterfaceVersion;

		// members for map-based keyword implementation:
public:
		int32 AddToCurrentKeyword(int32 ch, int32 state);
		int32 LookupCurrentKeyword(int32 state, int32 inSets=0) const;
		int32 LookupKeyword(const BString& word, int32 inSets=0) const;
protected:
		static const int32 kKeywordBufSize = 128;
		char fKeywordBuf[kKeywordBufSize+1];
		typedef std::multimap<BString, int> KeywordMap;
		mutable bool fHaveParsedKeywords;
		mutable KeywordMap fKeywordMap;
//		void GenerateKeywordMap(const char *ext);

static	std::vector<CLanguageInterface*>	fInterfaces;
};

inline const char* CLanguageInterface::LineCommentStart() const
{
	return fLineCommentStart;
} /* CLanguageInterface::LineCommentStart */

inline const char* CLanguageInterface::LineCommentEnd() const
{
	return fLineCommentEnd;
} /* CLanguageInterface::LineCommentEnd */

inline const char *CLanguageInterface::Name() const
{
	return fLanguage;
} /* CLanguageInterface::Name */


class CFunctionScanHandler {
public:
		CFunctionScanHandler();
virtual	~CFunctionScanHandler();

virtual	void AddFunction(const char *name, const char *match, int32 offset,
	bool italic, uint32 nestLevel, const char *params);
virtual	void AddInclude(const char *name, const char *open, bool italic);
virtual	void AddSeparator(const char* name);
};

#endif // CLANGUAGEINTERFACE_H
