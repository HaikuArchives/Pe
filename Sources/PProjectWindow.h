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

	Created: 11/18/98 21:23:57
*/

#ifndef PPROJECTWINDOW_H
#define PPROJECTWINDOW_H

#include "CDoc.h"

const ulong
	msg_PGroupItemInvoked = 'IvkG',
	msg_PRemove			 = 'Rmve',
	msg_PAdd				 = 'Add ';

class PGroupStatus;
class PGroupItem;
class PToolBar;
class HButtonBar;

class PProjectWindow
	: public BWindow
	, public CDoc
{
  public:
	PProjectWindow(const entry_ref *doc);
	~PProjectWindow();

	virtual bool QuitRequested();

	virtual void MessageReceived(BMessage *msg);
	virtual void SaveRequested(entry_ref& directory, const char *name);
	
private:
	virtual void ReadData(BPositionIO& file);
	virtual void ReadAttr(BFile& file);
	virtual void WriteData(BPositionIO& file);
	virtual void WriteAttr(BFile& file);
	
	void OpenItem();
	void AddRefs(BMessage *msg);
	void AddRef(const entry_ref& ref);
	void AddFiles();
	
	const char *AddGroup(const char *e, BDirectory& d);
	
	int fLookahead, fTokenLength;
	char *fHeader, *fFooter;
	
	BOutlineListView *fList;
	PGroupStatus *fStatus;
	PToolBar *fToolBar;
	HButtonBar *fButtonBar;
	BFilePanel *fPanel;
	vector<entry_ref> fSourceFiles, fResourceFiles, fLibraries;
};

#endif // PPROJECTWINDOW_H
