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

#ifndef CLANGINTF_H
#define CLANGINTF_H

#include <map>
#include <String.h>

class CFunctionScanHandler;
class CLanguageProxy;

class CLangIntf {
friend class CLanguageProxy;
public:
		~CLangIntf();

		void Balance(PText& text);
		bool Balance(PText& text, int& start,  int& end);
		void ScanForFunctions(PText& text, CFunctionScanHandler& handler);
		void ColorLine(const char *buf, int size,
				int& state, int *starts, rgb_color *colors);
		int FindNextWord(PText& text, int offset, int& mlen);
		
		const char *Text() const;
		int Size() const;
		int Encoding() const;
		
		const char *Name() const;
		const char *Extensions() const;
		void SetExtensions(const char *ext);
		
static	void ChooseDefault();
		
		int *Starts() const;
		rgb_color *Colors() const;

static	void SetupLanguageInterfaces();
static	CLangIntf* FindIntf(const char *filename);
static	CLangIntf* FindIntf(int indx)	{ return fInterfaces[indx]; }
static CLangIntf* FindByName(const char *language);

static CLangIntf* NextIntf(int& cookie);
static int GetIndex(const CLangIntf* intf);
			
		const char* LineCommentStart() const;
		const char* LineCommentEnd() const;
		
protected:
		CLangIntf();
		CLangIntf(const char *path, image_id image);
		
		void InitTables(const char *kwFile);
		void RegisterExtension(const char *ext);

static	unsigned char *sfWordBreakTable;
		
		// data members for old (DFA-based) keyword implementation, needed to 
		// support binary-only language-addons that use this implementation
		// (like phpext):
		unsigned char ec[128];
		unsigned short *accept, *base, *nxt, *chk;

		const char *fLanguage, *fExtensions, *fKeywordFile;		
		const char *fLineCommentStart, *fLineCommentEnd;
		bool (*fBalance)(CLanguageProxy& proxy, int& start, int& end);
		void (*fScanForFunctions)(CLanguageProxy& proxy);
		void (*fColorLine)(CLanguageProxy& proxy, int& state);
		int (*fFindNextWord)(const CLanguageProxy& proxy);
		image_id fImage;

		// member introduced for proper interface version detection:
		int16 fInterfaceVersion;
		
		// members for new (map-based) keyword implementation:
public:
		int AddToCurrentKeyWord(int ch, int state);
		int LookupCurrentKeyWord(int state) const;
		int LookupKeyWord(const BString& word) const;
protected:
		static const int kKeyWordBufSize = 128;
		char fKeyWordBuf[kKeyWordBufSize+1];
		typedef map<BString, int> KeyWordMap;
		mutable bool fHaveParsedKeyWords;
		mutable KeyWordMap fKeyWordMap;
		
static	vector<CLangIntf*>	fInterfaces;
};

inline const char* CLangIntf::LineCommentStart() const
{
	return fLineCommentStart;
} /* CLangIntf::LineCommentStart */

inline const char* CLangIntf::LineCommentEnd() const
{
	return fLineCommentEnd;
} /* CLangIntf::LineCommentEnd */

inline const char *CLangIntf::Name() const
{
	return fLanguage;
} /* CLangIntf::Name */


class CFunctionScanHandler {
public:
		CFunctionScanHandler();
virtual	~CFunctionScanHandler();

virtual	void AddFunction(const char *name, const char *match, int offset, 
	bool italic, uint32 nestLevel, const char *params);
virtual	void AddInclude(const char *name, const char *open, bool italic);
virtual	void AddSeparator();
};

#endif // CLANGINTF_H
