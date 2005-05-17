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

	Created: 09/10/97 13:20:21
*/

#ifndef CDOC_H
#define CDOC_H

#include <list>
#include <string>

class CDoc;
class URLData;

typedef list<CDoc*> doclist;

class CDoc {
public:
			CDoc(const char* mimetype, BLooper *target, const entry_ref *doc = NULL);
			CDoc(BLooper *target, URLData& url);
virtual			~CDoc();
	
			bool QuitRequested();
			
			void Read(bool readAttributes = true);
virtual	void ReadData(BPositionIO& file) = 0;
virtual	void ReadAttr(BFile& file) = 0;

status_t		  WriteState();
virtual	void WriteData(BPositionIO& file) = 0;
virtual	void WriteAttr(BFile& file) = 0;

virtual	void Save();
virtual	void SaveAs();
virtual	void SaveRequested(entry_ref& directory, const char *name);
virtual	void NameAFile(char *newName);
		
			bool IsDirty();
virtual	void SetDirty(bool dirty);
		
			bool IsReadOnly();
			void SetReadOnly(bool readOnly);

static		CDoc* FindDoc(const entry_ref& doc);
static		CDoc* FirstDoc();
static		int CountDocs();
static		const doclist& DocList()		{ return sfDocList; }
static		void InvalidateAll();
static		void PostToAll(unsigned long msg, bool async);

static		void AddRecent(const char *path);
static		bool GetNextRecent(char *path, int& indx);
		
			void SetMimeType(const char *type);
			const char *MimeType() const;
		
			const entry_ref* File();

protected:

static		doclist sfDocList;
static		vector<char*> sfTenLastDocs;

virtual	void CreateFilePanel();
			void CopyAttributes(BFile& from, BFile& to);

			void StartWatchingFile(void);
			void StopWatchingFile(void);

			BLooper *fTarget;
			string fMimeType;
			bool fDirty;
			bool fWaitForSave;
			bool fReadOnly;
			entry_ref *fFile;
			node_ref fNodeRef;
			URLData *fURL;
			BFilePanel *fSavePanel;
};

inline bool CDoc::IsDirty()
{
	return fDirty;
} /* CDoc::IsDirty */

inline const entry_ref* CDoc::File()
{
	return fFile;
} /* CDoc::File */

inline int CDoc::CountDocs()
{
	return sfDocList.size();
} /* CDoc::CountDocs */

inline const char *CDoc::MimeType() const
{
	return fMimeType.c_str();
} /* CDoc::MimeType */

inline CDoc* CDoc::FirstDoc()
{
	if (sfDocList.size())
		return sfDocList.front();
	else
		return NULL;
} /* CDoc::FirstDoc */

#endif // CDOC_H
