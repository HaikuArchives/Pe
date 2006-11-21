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

	Created: 09/18/97 20:36:58
*/

#include "pe.h"

#include <String.h>

#include "CKeywords.h"
#include "CLanguageAddOn.h"
#include "HError.h"

/*
 * New implementation of keyword lookup, a straightforward, map-based lookup:
 * [zooey]: 
 *     I know that using a hashmap should be faster, but since we do not know
 *     the amount of words contained in the map beforehand, the memory footprint
 *     of the hashmap would be (much) worse than that of a map.
 *     Benchmarks have indicated that lookup speed is good enough with maps
 *		 anyway (it performs at least as well as the older DFA-approach), 
 *		 so I have decided to use a map for now.
 */
void GenerateKWMap(const char *file, const char *ext, multimap<BString,int>& kwMap)
{
	try
	{
		BPath settings;
		bool isNew = false;
		
		FailOSErr(find_directory(B_USER_SETTINGS_DIRECTORY, &settings, true));
	
		BString p;
		p << settings.Path() << "/pe/" << file;
	
		BEntry e;
		FailOSErrMsg(e.SetTo(p.String(), B_FILE_NODE), 
						 "Settings directory was not found?");

		BString keywords;
		if (!e.Exists())
		{
			// copy resources into separate file in settings-folder, such that
			// the user can edit that in order to modify the keywords for that
			// specific language:
			isNew = true;
			
			BFile rf;
			FailOSErr(rf.SetTo(ext, B_READ_ONLY));
			BResources res;
			FailOSErr(res.SetTo(&rf));
			
			size_t s;
			const char *r = (const char*)res.LoadResource('KeyW', file, &s);
			
			if (!r) THROW(("Missing resource"));
			
			BFile txtfile(p.String(), B_CREATE_FILE | B_READ_WRITE);
			CheckedWrite(txtfile, r, s);
			keywords.SetTo(r, s);
		} else {
			BFile txtfile(p.String(), B_READ_ONLY);
			off_t size;
			FailOSErr(txtfile.GetSize(&size));
			char* kw = keywords.LockBuffer(size+1);
			if (kw) {
				CheckedRead(txtfile, kw, size);
				keywords.UnlockBuffer(size);
			}
		}

		const char* kw = keywords.String();
		const char* white = " \n\r\t";
		const char* start = kw + strspn(kw, white);
		const char* end = start + strcspn(start, white);
		BString word;
		char* buf;
		int currType = kKeywordLanguage;
		while (start < end) {
			// ideally, we'd like to use this:
			//			word.SetTo(start, end-start);
			// but the implementation of SetTo() seems to do a strlen() without
			// clamping it to the given length, which (as we give it a pretty
			// long string) results in pathetic performance.
			// So we roll our own SetTo():
			FailNil(buf = word.LockBuffer(end-start+1));
			memcpy(buf, start, end-start);
			buf[end-start] = '\0';
			word.UnlockBuffer(end-start);
			if (!word.Compare("//", 2)) {
				// a comment, so we skip to end of line:
				start += strcspn(start, "\n");
				start += strspn(start, white);
				end = start + strcspn(start, white);
			} else {
				if (word[0] == '-') {
					// it's a keyword-class specifier, we check which one:
					if (!word.ICompare("-Pe-Keywords-Language-"))
						currType = kKeywordLanguage;
					else if (!word.ICompare("-Pe-Keywords-User1-"))
						currType = kKeywordUser1;
					else if (!word.ICompare("-Pe-Keywords-User2-"))
						currType = kKeywordUser2;
					else if (!word.ICompare("-Pe-Keywords-User3-"))
						currType = kKeywordUser3;
					else if (!word.ICompare("-Pe-Keywords-User4-"))
						currType = kKeywordUser4;
					else {
						// be compatible with old style, meaning that an unknown
						// '-' entry bumps the type... Ignore a leading '-' entry
						if (!kwMap.empty())
							currType++;
						// ...and skips to end of line:
						end = start + strcspn(start, "\n");
					}
				} else {
					kwMap.insert(pair<BString, int>(word, currType));
				}
				start = end + strspn(end, white);
				end = start + strcspn(start, white);
			}
		}
		// DEBUG-OUTPUT:
		//cout << "Elements in MAP:" << endl;
		//int i = 0;
		//for (multimap<BString, int>::iterator it = kwMap.begin(); it != kwMap.end(); it++) {
		//	cout << ++i << ":  [" << (*it).second << ": " << (*it).first.String() << "]" << endl;
		//}
	}
	catch (HErr& err)
	{
		err.DoError();
	}
} /* GenerateKWMap */



/*
 * Old implementation of keyword lookup, DFA-based lookup:
 * This is only kept in here in order to support binary-only language-addons
 * that use it (like phpext). This can (and should) be dropped once there
 * is no more need to use the binary-only addons (aka: when the source of
 * phpext is found).
 */
#include <fs_attr.h>
struct kws {
	int accept;
	unsigned short trans[128];
	
	kws();
	void PrintToStream();
};

typedef map<int, kws> kwm;

static void AddKeyword(kwm& dfa, unsigned char *nkw, int& nextState, int a);
static void CompressDFA(kwm& dfa, vector<int>& accept, unsigned char ec[],
	vector<int>& base, vector<int>& nxt, vector<int>& chk, bool used[]);
static void RemapChars(kwm& dfa, unsigned char *map);
static void GenerateKWTables(FILE *file,
	vector<int>& accept, unsigned char ec[], vector<int>& base,
	vector<int>& nxt, vector<int>& chk);
static bool GetTablesFromAttributes(const char *file, unsigned char ec[],
	unsigned short *& accept, unsigned short *& base,
	unsigned short *& nxt, unsigned short *& chk);
static void WriteTablesToAttributes(const char *file,
	vector<int>& accept, unsigned char ec[], vector<int>& base,
	vector<int>& nxt, vector<int>& chk);

static void AddKeyword(kwm& dfa, unsigned char *nkw, int& nextState, int a)
{
	int state = 1, laststate = 1;
	unsigned char *nkwi = nkw;
	
	while (*nkwi)
	{
		laststate = state;
		state = dfa[state].trans[*nkwi];
		if (!state)
			break;
		nkwi++;
	}
	
	if (state)
	{
		if (dfa[state].accept == 0)
			dfa[state].accept = a;
	}
	else
	{
		state = laststate;
		
		while (*nkwi)
		{
//			dfa[state].trans[*nkwi] = state = nextState;	fucking compiler....

			kws& ks = dfa[state];
			state = nextState++;
			ks.trans[*nkwi] = state;
			nkwi++;
		}

		dfa[state].accept = a;
	}
} /* AddKeyword */

static void CompressDFA(kwm& dfa, vector<int>& accept, unsigned char ec[],
	vector<int>& base, vector<int>& nxt, vector<int>& chk, bool used[])
{
	int i, j, k = 0, b, s;
	unsigned short *t;
	
	for (i = 0; i < 128; i++)
		ec[i] = used[i] ? ++k : 0;

	RemapChars(dfa, ec);

	b = k + 1;

	accept.reserve(dfa.size());
	accept.push_back(0);

	nxt = vector<int>(b);
	chk = vector<int>(b);
	base = vector<int>(dfa.size() + 1);
	
	kwm::iterator ki;
	s = 1;
	for (ki = dfa.begin(); ki != dfa.end(); ki++, s++)
	{
		accept.push_back((*ki).second.accept);
		t = (*ki).second.trans;
		
		bool ok = false;
		
		while (!ok)
		{
			for (i = 0; i < b; i++)
			{
				ok = true;
				
				for (j = 1; j <= k; j++)
				{
					if ((i + j >= b) || t[j] && nxt[i + j])
					{
						ok = false;
						break;
					}
				}
				
				if (ok)
				{
					for (j = 1; j <= k; j++)
					{
						if (t[j])
						{
							nxt[i + j] = t[j];
							chk[i + j] = s;
						}
					}
					base[s] = i;
					break;
				}
			}
			
			if (!ok)
			{
				b++;
				nxt.push_back(0);
				chk.push_back(0);
			}
		}
	}
} /* CompressDFA */

static void RemapChars(kwm& dfa, unsigned char *ec)
{
	kwm::iterator ki;
	
	for (ki = dfa.begin(); ki != dfa.end(); ki++)
	{
		kws a, b;
		
		a = (*ki).second;
		
		b.accept = a.accept;
		for (int i = 0; i < 128; i++)
			b.trans[ec[i]] = a.trans[i];
		
		(*ki).second = b;
	}
} /* RemapChars */

kws::kws()
{
	accept = 0;
	memset(trans, 0, 256);
} /* kws::kws */

void kws::PrintToStream()
{
	printf("%s state\n", accept ? "n accepting" : " non-accepting");
	
	for (int i = 0; i < 128; i++)
	{
		if (trans[i])
			printf("\ton %c move to state %d\n", i, trans[i]);
	}
} /* kws::PrintToStream */

static void GenerateKWTables(FILE *f,
	vector<int>& accept, unsigned char ec[], vector<int>& base,
	vector<int>& nxt, vector<int>& chk)
{
	bool used[128];
	int nextState = 2, set = 1, i;
	kwm dfa;
	char s[2048];
	
	for (i = 0; i < 128; i++)
		used[i] = false;

	while (fgets(s, 2047, f))
	{
		char *p;
		
		if (s[0] == '-')
		{
			set++;
			continue;
		}

		p = strtok(s, " \n\r");
		while (p)
		{
			for (i = 0; p[i]; i++)
			{
				if (p[i] < 0)
				{
					p = NULL;
					break;
				}
				used[p[i]] = true;
			}

			if (p)
				AddKeyword(dfa, (unsigned char *)p, nextState, set);
			
//			dfa[1].PrintToStream();
			
			p = strtok(NULL, " \n\r");
		}
	}
	
//	PrintDFA(dfa);

	CompressDFA(dfa, accept, ec, base, nxt, chk, used);
} /* GenerateKWTables */

static bool GetTablesFromAttributes(const char *file, unsigned char ec[],
	unsigned short *& accept, unsigned short *& base,
	unsigned short *& nxt, unsigned short *& chk)
{
	bool result = true;
	
	try
	{
		BFile kwf;
		time_t modified, stored;
		
		FailOSErr(kwf.SetTo(file, B_READ_ONLY));
		
		FailOSErr(kwf.GetModificationTime(&modified));
		if (kwf.ReadAttr("prebuild at", 'time', 0, &stored, sizeof(stored)) == 0)
			stored = 0;

		attr_info ai;

		if (modified != stored)
			result = false;
		else
		{
			FailOSErr(kwf.GetAttrInfo("accept", &ai));
			accept = new unsigned short[(int)(ai.size) / sizeof(unsigned short)];
			FailNil(accept);
			if (kwf.ReadAttr("accept", 'tabl', 0, accept, ai.size) != ai.size)
				THROW(("io error"));
			
			FailOSErr(kwf.GetAttrInfo("base", &ai));
			base = new unsigned short[(int)(ai.size) / sizeof(unsigned short)];
			FailNil(base);
			if (kwf.ReadAttr("base", 'tabl', 0, base, ai.size) != ai.size)
				THROW(("io error"));
			
			FailOSErr(kwf.GetAttrInfo("chk", &ai));
			chk = new unsigned short[(int)(ai.size) / sizeof(unsigned short)];
			FailNil(chk);
			if (kwf.ReadAttr("chk", 'tabl', 0, chk, ai.size) != ai.size)
				THROW(("io error"));
			
			FailOSErr(kwf.GetAttrInfo("nxt", &ai));
			nxt = new unsigned short[(int)(ai.size) / sizeof(unsigned short)];
			FailNil(nxt);
			if (kwf.ReadAttr("nxt", 'tabl', 0, nxt, ai.size) != ai.size)
				THROW(("io error"));
			
			if (kwf.ReadAttr("ec", 'tabl', 0, ec, 128) != 128)
				THROW(("io error"));
		}
	}
	catch (HErr& err)
	{
		result = false;
	}
	
	return result;
} /* GetTablesFromAttributes */

static void WriteTablesToAttributes(const char *file,
	vector<int>& accept, unsigned char ec[], vector<int>& base,
	vector<int>& nxt, vector<int>& chk)
{
	time_t modified = 0;

	try
	{
		BFile kwf;
		
		FailOSErr(kwf.SetTo(file, B_READ_WRITE));
		if (kwf.WriteAttr("ec", 'tabl', 0, ec, 128) != 128)
			THROW(("io error"));
	
		unsigned short *tbl;
	
		tbl = new unsigned short[accept.size()];
		copy(accept.begin(), accept.end(), tbl);
		kwf.WriteAttr("accept", 'tabl', 0, tbl,
			accept.size() * sizeof(unsigned short));
		delete[] tbl;
		
		tbl = new unsigned short[base.size()];
		copy(base.begin(), base.end(), tbl);
		kwf.WriteAttr("base", 'tabl', 0, tbl,
			base.size() * sizeof(unsigned short));
		delete[] tbl;
		
		tbl = new unsigned short[nxt.size()];
		copy(nxt.begin(), nxt.end(), tbl);
		kwf.WriteAttr("nxt", 'tabl', 0, tbl,
			nxt.size() * sizeof(unsigned short));
		delete[] tbl;
		
		tbl = new unsigned short[chk.size()];
		copy(chk.begin(), chk.end(), tbl);
		kwf.WriteAttr("chk", 'tabl', 0, tbl,
			chk.size() * sizeof(unsigned short));
		delete[] tbl;
		
		time(&modified);
		kwf.WriteAttr("prebuild at", 'time', 0, &modified, sizeof(modified));
	}
	catch (HErr& err)
	{
		err.DoError();
	}
	
	if (modified)
		BNode(file).SetModificationTime(modified);
} /* WriteTablesToAttributes */

void GenerateKWTables(const char *file, const char *ext,
	unsigned char ec[],
	unsigned short *& accept, unsigned short *& base,
	unsigned short *& nxt, unsigned short *& chk)
{
	try
	{
		BPath settings;
		FILE *f;
		bool isNew = false;
		
		FailOSErr(find_directory(B_USER_SETTINGS_DIRECTORY, &settings, true));
	
		char p[PATH_MAX];
		strcpy(p, settings.Path());
		strcat(p, "/pe/");
		strcat(p, file);
	
		BEntry e;
		FailOSErrMsg(e.SetTo(p, B_FILE_NODE), "Settings directory was not found?");

		if (!e.Exists())
		{
			isNew = true;
			f = fopen(p, "w");
			
			BFile rf;
			FailOSErr(rf.SetTo(ext, B_READ_ONLY));
			BResources res;
			FailOSErr(res.SetTo(&rf));
			
			size_t s;
			char *r = (char *)res.FindResource('KeyW', file, &s);
			
			if (!r) THROW(("Missing resource"));
			
			fwrite(r, 1, s, f);
			fclose(f);
			free(r);
		}
		else if (!GetTablesFromAttributes(p, ec, accept, base, nxt, chk))
			isNew = true;

		if (isNew)
		{
			f = fopen(p, "r");
	
			vector<int> yy_accept, yy_base, yy_nxt, yy_chk;
			
			if (f)
			{
				GenerateKWTables(f, yy_accept, ec, yy_base, yy_nxt, yy_chk);
				fclose(f);
			}
			else
				memset(ec, 0, 128);

			accept = new unsigned short[yy_accept.size()];
			copy(yy_accept.begin(), yy_accept.end(), accept);
			
			base = new unsigned short[yy_base.size()];
			copy(yy_base.begin(), yy_base.end(), base);
			
			nxt = new unsigned short[yy_nxt.size()];
			copy(yy_nxt.begin(), yy_nxt.end(), nxt);
			
			chk = new unsigned short[yy_chk.size()];
			copy(yy_chk.begin(), yy_chk.end(), chk);
			
			fclose(f);
			
			WriteTablesToAttributes(p, yy_accept, ec, yy_base, yy_nxt, yy_chk);
		}
	}
	catch (HErr& err)
	{
		err.DoError();
	}
} /* GenerateKWTables */

