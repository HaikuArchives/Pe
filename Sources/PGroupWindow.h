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

#ifndef PGROUPWINDOW_H
#define PGROUPWINDOW_H

#include "CDoc.h"

const ulong
	msg_GroupItemInvoked = 'IvkG',
	msg_Remove			 = 'Rmve',
	msg_Add				 = 'Add ';

class PIconFinder;
class PToolBar;
class PGroupWindow;
class HButtonBar;

class PGroupStatus : public BView {
public:
			PGroupStatus(BRect frame, const char *name);
			~PGroupStatus();
		
virtual	void Draw(BRect update);
virtual	void MouseDown(BPoint where);
virtual	void AttachedToWindow();

			void SetPath(const char *path);
			void SetTypeahead(const char *text);

private:
			PGroupWindow *fOwner;
			char *fPath, *fText;
};

class CProjectItem;
class PProjectItem : public BStringItem
{
  public:
	PProjectItem(const char* name, int level = 0, CProjectItem* modelItem=NULL)
		: BStringItem(name, level), fModelItem(modelItem) {}
	
	CProjectItem* ModelItem()				{ return fModelItem; }
  private:
   CProjectItem* fModelItem;
};

class PEntryItem : public PProjectItem
{
  public:
	PEntryItem(const entry_ref& ref, int level = 0, CProjectItem* modelItem=NULL)
		: PProjectItem(ref.name, level, modelItem), fEntry(ref) {}

	const entry_ref& Ref() const
		{ return fEntry; }

	bool operator < (const PEntryItem& item) const
		{ return strcasecmp(Text(), item.Text()) < 0; }

	static int Compare(const void *a, const void *b)
	{
		return strcasecmp(
			(*((PEntryItem **)(a)))->fEntry.name,
			(*((PEntryItem **)(b)))->fEntry.name);
	} /* PEntryItem::Compare */

  private:
	entry_ref fEntry;
};

class PGroupWindow : public BWindow, public CDoc {
friend class PIconFinder;
public:
			PGroupWindow(const entry_ref *doc = NULL);
			~PGroupWindow();

virtual	bool QuitRequested();

virtual	void MessageReceived(BMessage *msg);
virtual	void SaveRequested(entry_ref& directory, const char *name);
		
			PEntryItem* AddRef(entry_ref& ref);
			
			int CountItems() const;

private:
virtual	void ReadData(BPositionIO& file);
virtual	void ReadAttr(BFile& file);
virtual	void WriteData(BPositionIO& file);
virtual	void WriteAttr(BFile& file);
		
			void OpenItem();
			void AddRefs(BMessage *msg);
			void AddFiles();
			void RemoveSelected();
			
virtual	void SetDirty(bool dirty);
			
			BListView *fList;
			PGroupStatus *fStatus;
			PToolBar *fToolBar;
			HButtonBar *fButtonBar;
			BFilePanel *fPanel;
			PIconFinder *fIconFinder;
			vector<PEntryItem*> *fNewItems;
};

inline int PGroupWindow::CountItems() const
{
	return fList->CountItems();
} /* PGroupWindow::CountItems */

#endif // PGROUPWINDOW_H
