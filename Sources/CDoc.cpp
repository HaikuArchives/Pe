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

#include "pe.h"
#include "CDoc.h"
#include "PApp.h"
#include "CFtpStream.h"
#include "HError.h"
#include "MAlert.h"
#include "HPreferences.h"
#include "CMessages.h"
#include <fs_attr.h>

doclist CDoc::sfDocList;
vector<char*> CDoc::sfTenLastDocs;

CDoc::CDoc(const char* mimetype, BLooper *target, const entry_ref *doc)
{
	ASSERT(strlen(mimetype) < 64);
	fSavePanel = NULL;
	fTarget = target;
	fDirty = false;
	fURL = NULL;
//	fSaveAttributes = true;
	fMimeType = mimetype;
	
	if (doc)
	{
		entry_ref ref;
		BEntry e;
		FailOSErr(e.SetTo(doc, true));
		
		fFile = new entry_ref;
		FailOSErr(e.GetRef(fFile));
	
		FailOSErr(e.SetTo(doc));
		FailOSErr(e.GetParent(&gCWD));

		BNode node;
		FailOSErr(node.SetTo(fFile));
		
		struct stat st;
		FailOSErr(node.GetStat(&st));

		fReadOnly = ! ((gUid == st.st_uid && (S_IWUSR & st.st_mode)) ||
							(gGid == st.st_gid && (S_IWGRP & st.st_mode)) ||
							(S_IWOTH & st.st_mode));
		
		char s[NAME_MAX];
		if (BNodeInfo(&node).GetType(s) == B_OK)
			fMimeType = s;
	}
	else
	{
		fFile = NULL;
		fReadOnly = false;
	}

	sfDocList.push_back(this);
} /* CDoc::CDoc */

CDoc::CDoc(BLooper *target, URLData& url)
{
	fSavePanel = NULL;
	fTarget = target;
	fDirty = false;
	fURL = new URLData(url);
	fFile = NULL;
	fReadOnly = false;

	sfDocList.push_back(this);
} /* CDoc::CDoc */

CDoc::~CDoc()
{
	sfDocList.remove(this);
	
	if (fSavePanel) delete fSavePanel;
	if (fFile) delete fFile;
	
	be_app->PostMessage(msg_DocClosed);
} /* CDoc::~CDoc */

bool CDoc::QuitRequested()
{
	bool result = true;
	fWaitForSave = false;

	if (fDirty)
	{
		char title[256];
		sprintf(title, "Save changes to '%s' before closing?", fFile ? fFile->name : "Untitled");
		
		MInfoAlert alert(title, "Save", "Cancel", "Don't save");
		
		switch (alert.Go())
		{
			case 3:
				break;
				
			case 2:
				result = false;
				break;
				
			default:
				if (fFile)
					Save();
				else
				{
					result = false;
					fWaitForSave = true;
					SaveAs();
				}
				break;
		}
	}

	return result;
} /* CDoc::QuitRequested */

void CDoc::Read()
{
	if (!fFile && !fURL) THROW(("No file available"));
	
	if (fFile)
	{
		BFile file;
		FailOSErr(file.SetTo(fFile, B_READ_ONLY));
		
		ReadData(file);
		ReadAttr(file);
	}
	else
	{
		CFtpStream ftp(*fURL, true, gPrefs->GetPrefInt("passive ftp", 1));
		ReadData(ftp);
	}
} /* CDoc::Read */

//void CDoc::ReadAttr(BFile& file)
//{
//} /* CDoc::ReadAttr */

status_t CDoc::WriteState()
{
	if (fFile)
	{
		BFile file;
		status_t res = file.SetTo(fFile, B_READ_WRITE);
		if (res != B_OK)
			return res;
		WriteAttr(file);
		return file.Sync();
	}
	else
		return B_NO_INIT;
} /* CDoc::WriteState */

//void CDoc::WriteAttr(BFile& file)
//{
//} /* CDoc::WriteAttr */

void CDoc::Save()
{
	if (fURL)
	{
		try
		{
			CFtpStream ftp(*fURL, false, gPrefs->GetPrefInt("passive ftp", 1));
			WriteData(ftp);
			ftp.Flush();
			
			SetDirty(false);
		}
		catch (HErr& e)
		{
			e.DoError();
		}
	}
	else if (fFile && !fReadOnly)
	{
		char name[B_FILE_NAME_LENGTH];
		BEntry e(fFile, true);
		bool existed = e.Exists();
		time_t created;

		FailOSErr(e.GetName(name));

		try
		{
			if (existed)
			{
				FailOSErr(BFile(fFile, B_READ_ONLY).GetCreationTime(&created));
	
				string bname(name);
				bname += '~';
				
				FailOSErr(e.Rename(bname.c_str(), true));
			}

			BFile file;
			BDirectory dir;
			
			FailOSErr(e.GetParent(&dir));
			FailOSErr(dir.CreateFile(name, &file, true));
			WriteData(file);
			(void)file.Sync();	// might fail
			WriteAttr(file);
			(void)file.Sync();	// might also fail
	
			if (existed)
			{
				FailOSErr(file.SetCreationTime(created));
				
				if (gPrefs->GetPrefInt("save attr", 1))
				{
					BFile old;
					FailOSErr(old.SetTo(&e, B_READ_ONLY));
					CopyAttributes(old, file);
				}
				
				if (!gPrefs->GetPrefInt("backup"))
					FailOSErr(e.Remove());
			}

			BNodeInfo(&file).SetType(fMimeType.c_str());
			SetDirty(false);
		}
		catch (HErr& err)
		{
			err.DoError();
// Now don't check error codes anymore... hope this is right
			BEntry(fFile, true).Remove();
			if (existed)
				e.Rename(name);
		}
	}
	else
		SaveAs();
} /* CDoc::Save */

void CDoc::SaveAs()
{
	if (!fSavePanel)
		CreateFilePanel();

	BWindow *w = fSavePanel->Window();
	FailNil(w);
	w->Lock();
	
	char newName[B_FILE_NAME_LENGTH];
	if (!fFile) NameAFile(newName);
	
	char *f = strrchr(newName, '/');
	if (! f) f = newName;
	else f++;
	
	char s[256];
	sprintf(s, "Save %s as:", fFile ? fFile->name : f);
	
	w->SetTitle(s);
	fSavePanel->SetSaveText(fFile ? fFile->name : f);
	
	if (fFile)
	{
		BEntry e(fFile), p;
		e.GetParent(&p);
		fSavePanel->SetPanelDirectory(&p);
	}
	else if (newName[0] == '/')
	{
		BEntry e;
		e.SetTo(newName);
		e.GetParent(&e);
		if (e.Exists())
		{
			BDirectory d(&e);
			fSavePanel->SetPanelDirectory(&d);
		}
	}
	else
		fSavePanel->SetPanelDirectory(&gCWD);

	fSavePanel->SetMessage(new BMessage(B_SAVE_REQUESTED));
	fSavePanel->SetTarget(fTarget);

	w->Unlock();

	if (!fSavePanel->IsShowing())
		fSavePanel->Show();
	else
		fSavePanel->Window()->Activate();
} /* CDoc::SaveAs */

void CDoc::NameAFile(char *name)
{
	strcpy(name, "Untitled");
} /* CDoc::NameAFile */

void CDoc::SaveRequested(entry_ref& directory, const char *name)
{
	try
	{
		if (fURL) delete fURL;
		fURL = NULL;

		BDirectory dir(&directory);
		gCWD = dir;

		BEntry e(&dir, name);
		FailOSErr(e.InitCheck());
		if (e.Exists())
		{
			entry_ref xr;
			
			e.GetRef(&xr);
			
			if (fFile && xr == *fFile)	// its me, help!!!
			{
				BFile file;
				FailOSErr(file.SetTo(fFile, B_READ_WRITE));
				FailOSErr(file.SetSize(0));
			}
			else
				e.Remove();
		}
		
		fReadOnly = false;
		
		if (!fFile) fFile = new entry_ref;
		FailNil(fFile);
		FailOSErr(e.GetRef(fFile));
		
		Save();
		
		if (fFile)
		{
			BPath p;
			FailOSErr(e.GetPath(&p));
			AddRecent(p.Path());
		}
		
		if (fSavePanel)
		{
			delete fSavePanel;
			fSavePanel = NULL;
		}
	}
	catch (HErr& e)
	{
		e.DoError();
		delete fFile;
		fFile = NULL;
	}
} /* CDoc::SaveRequested */

void CDoc::SetDirty(bool dirty)
{
	fDirty = dirty;
} /* CDoc::SetDirty */

bool CDoc::IsReadOnly()
{
	return fReadOnly;
} /* CDoc::IsReadOnly */

void CDoc::SetReadOnly(bool readOnly)
{
	if ((readOnly && fDirty) || !fFile)
	{
		MWarningAlert a("You have to save the file first before marking it read-only");
		a.Go();
	}
	else
	{
		try
		{
			BEntry e;
			FailOSErr(e.SetTo(fFile, true));
			BPath p;
			FailOSErr(e.GetPath(&p));
			
			struct stat st;
			FailOSErr(stat(p.Path(), &st));
			
			if (readOnly)
				st.st_mode &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
			else
			{
// is this the right way, or is it a hack???
				mode_t t = umask(0);	// get the umask
				umask(t);				// and reset it.
				st.st_mode |= (S_IWUSR | S_IWGRP | S_IWOTH) & ~t;
			}
			
			FailOSErr(chmod(p.Path(), st.st_mode));

			fReadOnly = readOnly;
		}
		catch (HErr& e)
		{
			e.DoError();
		}
	}
} /* CDoc::SetReadOnly */

void CDoc::CreateFilePanel()
{
	fSavePanel = new BFilePanel(B_SAVE_PANEL);
	FailNil(fSavePanel);
} /* CDoc::CreateFilePanel */

void CDoc::CopyAttributes(BFile& from, BFile& to)
{
	try
	{
		char name[NAME_MAX];
		
		while (from.GetNextAttrName(name) == B_OK)
		{
			attr_info ai;
			
			if (strcmp(name, "pe-info") == 0 ||
				strcmp(name, "FontSettings") == 0 ||
				strcmp(name, "BEOS:TYPE") == 0)
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
} /* CDoc::CopyAttributes */

CDoc* CDoc::FindDoc(const entry_ref& doc)
{
	doclist::iterator di;
	
	for (di = sfDocList.begin(); di != sfDocList.end(); di++)
	{
		if ((*di)->fFile && *(*di)->fFile == doc)
			return (*di);
	}
	return NULL;
} /* CDoc::FindDoc */

void CDoc::AddRecent(const char *path)
{
	char *s;
	if ((s = strstr(path, "/config/settings/pe/Worksheet")) != NULL && 
		strlen(s) == 29 /*strlen("/config/settings/pe/Worksheet")*/)
		return; // don't add the worksheet
	
	if (gPrefs->GetPrefInt("skiptmp", 1))
	{
		BPath tp;
		try
		{
			FailOSErr(find_directory(B_COMMON_TEMP_DIRECTORY, &tp));
			if (strncmp(tp.Path(), path, strlen(tp.Path())) == 0)
				return;
		}
		catch (...) {}
	}

	vector<char*>::iterator di;
	
	for (di = sfTenLastDocs.begin(); di != sfTenLastDocs.end(); di++)
	{
		char *p = *di;
		if (p && strcmp(p, path) == 0)
		{
			if (di != sfTenLastDocs.begin())
			{
				sfTenLastDocs.erase(di);
				sfTenLastDocs.insert(sfTenLastDocs.begin(), p);
			}
			return;
		}
	}

	while (sfTenLastDocs.size() >= gRecentBufferSize)
	{
		char *p = sfTenLastDocs.back();
		sfTenLastDocs.erase(sfTenLastDocs.end() - 1);
		free(p);
	}
	
	sfTenLastDocs.insert(sfTenLastDocs.begin(), strdup(path));
} /* CDoc::AddRecent */

bool CDoc::GetNextRecent(char *path, int& indx)
{
	if (indx >= sfTenLastDocs.size())
		return false;
	else
	{
		vector<char*>::iterator li;
		
		li = sfTenLastDocs.begin() + indx++;
		
		strcpy(path, *(li));
		return true;
	}
} /* CDoc::GetNextRecent */

void CDoc::PostToAll(unsigned long msg, bool async)
{
	doclist::iterator di;
	
	doclist lst = sfDocList;
	
	for (di = lst.begin(); di != lst.end(); di++)
	{
		BWindow *w = dynamic_cast<BWindow*>(*di);
		if (w)
		{
			BMessage reply;
			
			BLooper *me = BLooper::LooperForThread(find_thread(NULL));

			if (async || w == dynamic_cast<BWindow*>(me))
				w->PostMessage(msg);
			else
			{
				BMessenger msgr(w);
				FailOSErr(msgr.SendMessage(msg, &reply));
			}
		}
	}
} /* CDoc::PostToAll */

void CDoc::SetMimeType(const char *type)
{
	fMimeType = type;
	
	if (fFile)
	{
		BNode node;
		FailOSErr(node.SetTo(fFile));
		FailOSErr(BNodeInfo(&node).SetType(type));
	}
} /* CDoc::SetMimeType */
