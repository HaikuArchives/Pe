/*
	Copyright 2005 Oliver Tappe
	
	Distributed under the MIT License
*/

#include "pe.h"

#include <NodeMonitor.h>
#include <fs_attr.h>
#include <String.h>

#include "CDoc.h"
#include "CDocIO.h"
#include "CFtpStream.h"
#include "HError.h"
#include "HPreferences.h"
#include "MAlert.h"
#include "Prefs.h"

static void ConvertLinebreaks(BString& str, int fromType, int toType)
{
	const char* lbStr[] = {
		"\n", "\r", "\r\n"
	};
	if (fromType == kle_UNKNOWN)
		fromType = kle_LF;
	if (toType == kle_UNKNOWN)
		toType = kle_LF;
	const char* fromStr = lbStr[fromType];
	const char* toStr = lbStr[toType];
	if (fromType != toType)
	{
		vector<int> posVect;
		for( int32 pos = 0; (pos = str.FindFirst(fromStr, pos)) >= B_OK; ++pos)
			posVect.push_back(pos);
		int32 szDiff = (int32)strlen(toStr)-(int32)strlen(fromStr);
		int32 newLen = str.Length() + szDiff * (int32)posVect.size();
		if (newLen <= 0)
			str.Truncate(0);
		else
		{
			BString dest;
			const char* oldAdr = str.String();
			char* destBuf = dest.LockBuffer(newLen);
			char* newAdr = destBuf;
			FailNil(newAdr);
			int32 pos, len;
			int32 lastPos = 0;
			int fromLen = strlen(fromStr);
			int toLen = strlen(toStr);
			for( uint32 i=0; i<posVect.size(); ++i)
			{
				pos = posVect[i];
				len = pos - lastPos;
				if (len > 0) 
				{
					memcpy(newAdr, oldAdr, len);
					oldAdr += len;
					newAdr += len;
				}
				memcpy(newAdr, toStr, toLen);
				oldAdr += fromLen;
				newAdr += toLen;
				lastPos = pos+fromLen;
			}
			len = str.Length() + 1 - lastPos;
			if (len > 0)
				memcpy(newAdr, oldAdr, len);
			dest.UnlockBuffer(newAdr-destBuf+len-1);
			str.Adopt(dest);
		}
	}
}

static int32 DetermineLineEndType(const BString& str)
{
	for( const char* s = str.String(); *s; ++s)
	{
		if (*s == '\n')
			return kle_LF;
		if (*s == '\r')
			return (*(s+1) == '\n') ? kle_CRLF : kle_CR;
	}
	return kle_LF;
}

static int32 DetermineEncoding(const BString& str)
{
	return B_UNICODE_UTF8;
}

static void CopyAttributes(BFile& from, BFile& to)
{
	try
	{
		char name[NAME_MAX];
		
		while (from.GetNextAttrName(name) == B_OK)
		{
			attr_info ai;
			
			if (strcmp(name, "BEOS:TYPE") == 0)
				continue;
			
			FailOSErr(from.GetAttrInfo(name, &ai));
			
			char *buf = new char [ai.size];
			from.ReadAttr(name, ai.type, 0, buf, ai.size);
			to.WriteAttr(name, ai.type, 0, buf, ai.size);
			delete [] buf;
		}	
		
		mode_t perm;
		from.GetPermissions(&perm);
		to.SetPermissions(perm);
	}
	catch (HErr& e)
	{
		e.DoError();
	}
}

#pragma mark -

CDocIO::CDocIO(CDoc* doc)
	: fDoc(doc)
	, fLastSaved(0)
{
}

CDocIO::~CDocIO()
{
}

bool CDocIO::VerifyFile()
{
	return true;
}

const entry_ref* CDocIO::EntryRef() const
{
	return NULL;
}

const URLData* CDocIO::URL() const
{
	return NULL;
}

void CDocIO::SetEntryRef(const entry_ref* ref)
{
}

BLooper* CDocIO::Target()
{
	return NULL;
}

void CDocIO::StartWatchingFile()
{
}

void CDocIO::StopWatchingFile(bool stopDirectory)
{
}

void CDocIO::HandleNodeMonitorMsg(BMessage* msg)
{
}

bool CDocIO::DoPreEditTextConversions(BString& docText)
{
	// convert from document's native encoding to internal type (UTF-8):
	fConv.SetTo(fDoc->Encoding());
	FailOSErr(fConv.ConvertToUtf8(docText));
	if (fConv.HadToSubstitute() && fConv.ErrorPos() >= 0)
	{
		BString err;
		if (fDoc->Encoding() != B_UNICODE_UTF8)
			err = BString("An error occurred when converting the document ")
					<< "from its native encoding to UTF-8.\n"
					<< "Please use the 'Change Encoding'-dialog to select "
					<< "the correct encoding.";
		else
			err = BString("An error occurred when checking the document's ")
					<< "UTF-8 encoding.\n"
					<< "The first problematic character is highlighted.";
		MInfoAlert a(err.String(), "I See");
		a.Go();
		return false;
	}

	// convert from document's native linebreaks to internal type (LF):
	ConvertLinebreaks(docText, fDoc->LineEndType(), kle_LF);

	return true;
}

bool CDocIO::DoPostEditTextConversions(BString& docText)
{
	if (fDoc->Encoding() != B_UNICODE_UTF8)
	{
		// convert from internal encoding (UTF-8) to native type:
		fConv.SetTo(fDoc->Encoding());
		FailOSErr(fConv.ConvertFromUtf8(docText));
		if (fConv.HadToSubstitute() && fConv.ErrorPos() >= 0)
		{
			fDoc->HighlightErrorPos(fConv.ErrorPos());
			BString err = 
				BString("An error occurred when converting the document ")
					<< "to the requested destination encoding.\n"
					<< "The first problematic character is highlighted.";
			MInfoAlert a(err.String(), "I See");
			a.Go();
			return false;
		}
	}

	if (gPrefs->GetPrefInt(prf_I_NlAtEof, 1) && docText.Length() > 0 
		&& docText[docText.Length() - 1] != '\n')
	{
		docText << "\n";
	}

	// convert from internal linebreaks (LF) to native type:
	ConvertLinebreaks(docText, kle_LF, fDoc->LineEndType());

	return true;
}

#pragma mark -

CLocalDocIO::CLocalDocIO(CDoc* doc, const entry_ref* eref, BLooper* target)
	: CDocIO(doc)
	, fEntryRef(NULL)
	, fTarget(target)
{
	if (eref)
		fEntryRef = new entry_ref(*eref);
}

CLocalDocIO::~CLocalDocIO()
{
	delete fEntryRef;
}

bool CLocalDocIO::IsLocal() const
{
	return false;
}

const char* CLocalDocIO::Name() const
{
	return fEntryRef ? fEntryRef->name : fDoc->DefaultName();
}

const entry_ref* CLocalDocIO::EntryRef() const
{
	return fEntryRef;
}

const URLData* CLocalDocIO::URL() const
{
	return NULL;
}

bool CLocalDocIO::ReadDoc(bool readAttributes)
{
	BFile file;
	FailOSErr(file.SetTo(fEntryRef, B_READ_ONLY));

	BMessage settingsMsg;
	if (readAttributes)
		fDoc->ReadAttr(file, settingsMsg);

	off_t size;
	FailOSErr(file.GetSize(&size));
	BString docText;
	char* buf = docText.LockBuffer(size);
	FailNil(buf);
	int sz = file.Read(buf, size);
	docText.UnlockBuffer(size);
	if (sz < size)
		THROW(("A read error occurred: %s", strerror(errno)));

	if (readAttributes)
	{
		// determine and set the type of linebreaks:
		int32 lineEndType;
		if (settingsMsg.FindInt32("line breaks", &lineEndType) != B_OK)
			lineEndType = DetermineLineEndType(docText);
		fDoc->SetLineEndType(lineEndType);
	
		// set the encoding:
		int32 encoding;
		if (settingsMsg.FindInt32("encoding", &encoding) != B_OK)
			encoding = DetermineEncoding(docText);
		// [zooey]: For a short period of time (one of my bugs),
		// 			Pe used -1 as UTF-8, we compensate:
		if (encoding < 0)
			encoding = B_UNICODE_UTF8;
		fDoc->SetEncoding(encoding);
	}
	
	bool result = DoPreEditTextConversions(docText);

	fDoc->SetText(docText);
	
	if (readAttributes)
		fDoc->ApplySettings(settingsMsg);
		
	if (!result && fConv.ErrorPos() >= 0)
		fDoc->HighlightErrorPos(fConv.ErrorPos());

	FailOSErr(BEntry(fEntryRef).GetModificationTime(&fLastSaved));

	return result;
}
	
bool CLocalDocIO::WriteDoc()
{
	char name[B_FILE_NAME_LENGTH];
	BEntry e(fEntryRef, true);
	bool existed = e.Exists();

	try
	{
		BMessage settingsMsg;
		fDoc->CollectSettings(settingsMsg);
		BString docText;
		fDoc->GetText(docText);
		if (!DoPostEditTextConversions(docText))
			return false;
	
		FailOSErr(e.GetName(name));
	
		time_t created;
		if (existed)
		{
			FailOSErr(e.GetCreationTime(&created));

			string bname(name);
			bname += '~';
			FailOSErr(e.Rename(bname.c_str(), true));
		}

		BFile file;
		BDirectory dir;
		FailOSErr(e.GetParent(&dir));
		FailOSErr(dir.CreateFile(name, &file, true));
		CheckedWrite(file, docText.String(), docText.Length());
		if (existed)
		{
			BFile old;
			FailOSErr(old.SetTo(&e, B_READ_ONLY));
			CopyAttributes(old, file);
			FailOSErr(file.SetCreationTime(created));
		}
		fDoc->WriteAttr(file, settingsMsg);
		file.Sync();

		if (existed && !gPrefs->GetPrefInt(prf_I_Backup))
			FailOSErr(e.Remove());

		// Update MIME type info
		e.SetTo(&dir, name);
		BPath path(&e);
		if (!strlen(fDoc->MimeType()) && path.InitCheck() == B_OK
			&& update_mime_info(path.Path(), false, true, false) == B_OK) 
		{
			// takeover MIME type from file
			char s[NAME_MAX];
			if (BNodeInfo(&file).GetType(s) == B_OK)
				fDoc->SetMimeType(s, false);
		}
		else
			BNodeInfo(&file).SetType(fDoc->MimeType());

		time(&fLastSaved);
	}
	catch (HErr& err)
	{
		err.DoError();
		// Now don't check error codes anymore... hope this is right
		BEntry(fDoc->EntryRef(), true).Remove();
		if (existed)
			e.Rename(name);
		return false;
	}
	return true;
}

void CLocalDocIO::SetEntryRef(const entry_ref* ref)
{
	delete fEntryRef;
	if (ref)
		fEntryRef = new entry_ref(*ref);
	else
		fEntryRef = NULL;
}

BLooper* CLocalDocIO::Target()
{
	return fTarget;
}

void CLocalDocIO::StartWatchingFile()
{
	if (fEntryRef == NULL || fTarget == NULL)
		return;

	// start monitoring this file for changes
	BNode node(fEntryRef);
	if (node.GetNodeRef(&fNodeRef) == B_OK)
		watch_node(&fNodeRef, B_WATCH_NAME | B_WATCH_STAT, fTarget);

	node_ref directoryNodeRef;
	BEntry entry(fEntryRef);
	if (entry.GetParent(&entry) == B_OK
		&& node.SetTo(&entry) == B_OK
		&& node.GetNodeRef(&directoryNodeRef) == B_OK)
		watch_node(&directoryNodeRef, B_WATCH_DIRECTORY, fTarget);
}

void CLocalDocIO::StopWatchingFile(bool stopDirectory)
{
	if (fEntryRef == NULL || fTarget == NULL)
		return;

	watch_node(&fNodeRef, B_STOP_WATCHING, fTarget);

	// if we get late messages, we don't want to deal with them
	fNodeRef.device = -1;
	fNodeRef.node = -1;

	if (stopDirectory) {
		node_ref directoryNodeRef;
		BEntry entry(fEntryRef);
		BNode node;
		if (entry.GetParent(&entry) == B_OK
			&& node.SetTo(&entry) == B_OK
			&& node.GetNodeRef(&directoryNodeRef) == B_OK)
			watch_node(&directoryNodeRef, B_STOP_WATCHING, fTarget);
	}
}

void CLocalDocIO::HandleNodeMonitorMsg(BMessage* msg)
{
	int32 opcode;
	if (fEntryRef == NULL || msg->FindInt32("opcode", &opcode) != B_OK)
		return;

	const char *name;
	if ((opcode == B_ENTRY_CREATED
			|| opcode == B_ENTRY_MOVED && fNodeRef.node == -1)
		&& msg->FindString("name", &name) == B_OK
		&& !strcmp(name, fEntryRef->name))
	{
		VerifyFile();
	}
	else if (msg->FindInt64("node") != fNodeRef.node)
	{
		// filter out other stuff that comes from watching the directory
		return;
	}

	switch (opcode)
	{
		case B_ENTRY_MOVED:
		{
			int64 directory;
			if (msg->FindInt64("to directory", &directory) == B_OK)
				fEntryRef->directory = directory;

			const char *name;
			if (msg->FindString("name", &name) == B_OK)
				fEntryRef->set_name(name);

			fDoc->NameChanged();
			break;
		}
		case B_ENTRY_REMOVED:
		{
			StopWatchingFile(false);
				// We don't want to stop monitoring the directory; BTW, it
				// will get automatically updated on next save, the monitoring
				// slot is not lost
			fDoc->SetDirty(true);
			break;
		}
		case B_STAT_CHANGED:
			VerifyFile();
			break;
	}
}

bool CLocalDocIO::VerifyFile()
{
	bool result = true;
	try
	{
		if (gPrefs->GetPrefInt(prf_I_Verify, 1))
		{
			BFile file;
			FailOSErr(file.SetTo(fEntryRef, B_READ_ONLY));

			time_t t;
			FailOSErr(file.GetModificationTime(&t));
			if (fLastSaved && t > fLastSaved + 1)
			{
				char s[PATH_MAX + 20];
				sprintf(s, "File %s was modified by another application, reload it?", fEntryRef->name);
				MInfoAlert a(s, "Reload", "Cancel");

				if (a.Go() == 1)
					result = false;
			}

			fLastSaved = time(NULL);
				// if more than one update request was issued
				// in the mean time, only the first one is
				// considered
		}
	}
	catch (HErr& e)
	{
		// file seems to be gone, but it doesn't really matter...
		// (we could only run into problems if the directory is gone)
	}
	return result;
}

#pragma mark -

CFtpDocIO::CFtpDocIO(CDoc* doc, const URLData& url)
	: CDocIO(doc)
	, fURL(new URLData(url))
{
}

CFtpDocIO::~CFtpDocIO()
{
	delete fURL;
}

bool CFtpDocIO::IsLocal() const
{
	return false;
}

const char* CFtpDocIO::Name() const
{
	return fURL ? fURL->File() : fDoc->DefaultName();
}

const entry_ref* CFtpDocIO::EntryRef() const
{
	return NULL;
}

const URLData* CFtpDocIO::URL() const
{
	return fURL;
}

bool CFtpDocIO::ReadDoc(bool readAttributes)
{
	CFtpStream ftp(*fURL, true, gPrefs->GetPrefInt(prf_I_PassiveFtp, 1));

	off_t size = ftp.Seek(0, SEEK_END);
	ftp.Seek(0, SEEK_SET);
	BString docText;
	char* buf = docText.LockBuffer(size);
	FailNil(buf);
	int sz = ftp.ReadAt(0, buf, size);
	docText.UnlockBuffer(size);
	if (sz < size)
		THROW(("A read error occurred: %s", strerror(errno)));

	if (readAttributes)
	{
		// determine and set the type of linebreaks:
		int32 lineEndType = DetermineLineEndType(docText);
		fDoc->SetLineEndType(lineEndType);
	
		// set the encoding:
		int32 encoding = DetermineEncoding(docText);
		// [zooey]: For a short period of time (one of my bugs),
		// 			Pe used -1 as UTF-8, we compensate:
		if (encoding < 0)
			encoding = B_UNICODE_UTF8;
		fDoc->SetEncoding(encoding);
	}
	
	bool result = DoPreEditTextConversions(docText);

	fDoc->SetText(docText);
	
	if (!result && fConv.ErrorPos() >= 0)
		fDoc->HighlightErrorPos(fConv.ErrorPos());

	return result;
}
	
bool CFtpDocIO::WriteDoc()
{
	try
	{
		BMessage settingsMsg;
		fDoc->CollectSettings(settingsMsg);
		BString docText;
		fDoc->GetText(docText);
		if (!DoPostEditTextConversions(docText))
			return false;
	
		CFtpStream ftp(*fURL, false, 
					   gPrefs->GetPrefInt(prf_I_PassiveFtp, 1));
		CheckedWrite(ftp, docText.String(), docText.Length());
		ftp.Flush();
	}
	catch (HErr& err)
	{
		err.DoError();
		return false;
	}
	return true;
}

