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

	Created: 11/14/98 13:50:36
*/

#include "pe.h"
#include "PApp.h"
#include "CFtpDialog.h"
#include "CFtpListItem.h"
#include "CFtpStream.h"
#include "PMessages.h"
#include "PDoc.h"
#include "CLogin.h"
#include "HError.h"
#include "HPreferences.h"
#ifdef BONE_BUILD
	#define closesocket(X)	close(X)
#endif
#include <socket.h>
#include <netdb.h>
#include <String.h>

const unsigned long
	msg_SelectedListItem = 'slct',
	msg_SelectedDirectory = 'dirS',
	msg_ToggleDot = 'dotT';

filter_result CPassWordSniffer::Filter(BMessage *msg, BHandler **target)
{
	BTextView *tv = dynamic_cast<BTextView*>(*target);

	if (tv)
	{
		// copy selection
		long st, en;

		tv->GetSelection(&st, &en);
		fShadow->Select(st, en);
		
		unsigned long c;
		msg->FindInt32("raw_char", (long *)&c);

		if (c != B_TAB && c != B_RETURN)
			BMessenger(fShadow).SendMessage(msg);
		
		if (isprint(c) || c > 128)
		{
			msg->ReplaceString("bytes", "*");
			msg->ReplaceInt32("key", 0x19);
			msg->ReplaceInt8("byte", '*');
			msg->ReplaceInt32("raw_char", '*');
			msg->ReplaceInt32("modifiers", 0);
		}
	}
	
	return B_DISPATCH_MESSAGE;
} /* CPassWordSniffer::Filter */

CFtpDialog::CFtpDialog(BRect frame, const char *name, window_type type, int flags,
	BWindow *owner, BPositionIO& data)
	: HDialog(frame, name, type, flags, owner, data)
{
	BTextControl *password = dynamic_cast<BTextControl*>(FindView("pass"));
	fDirectoryPopup = dynamic_cast<BMenuField*>(FindView("path"))->Menu();
	fList = dynamic_cast<BListView*>(FindView("list"));
	
	BTextView *shadow;
	AddChild(shadow = new BTextView(BRect(0, 0, 0, 0), "shadow", BRect(0, 0, 0, 0), 0, 0));
	
	if (password)
		password->TextView()->AddFilter(new CPassWordSniffer(shadow));
	
	fList->SetInvocationMessage(new BMessage(msg_SelectedListItem));

	fReply = new char[1024];
	fPath = new char[PATH_MAX];
	fSave = false;
	fSocket = -1;

	SetDefaultButton(static_cast<BButton*>(FindView("cnct")));
	
	SetText("srvr", gPrefs->GetPrefString("last ftp server"));

	if (strlen(CLogin::Username()))
	{
		SetText("user", CLogin::Username());
		SetText("pass", "********");
		shadow->SetText(CLogin::Password());
	}
	else
	{
		SetText("user", gPrefs->GetPrefString("last ftp user"));
		if (strlen(GetText("srvr")) && strlen(GetText("user")))
			password->MakeFocus();
		else
			FindView("srvr")->MakeFocus();
	}
	
	static_cast<BControl*>(FindView("dotf"))->SetMessage(new BMessage(msg_ToggleDot));
	
	FindView("name")->Hide();

	// Build Extension->Mimetype list // Takes looong
	typedef pair<string,string> entry;
	if (0) {
		BMessage mimDat;
		BMessage extDat;
		BMimeType mimTyp;
		int32 mimIdx = -1;
		int32 extIdx = 0;
		const char* mimCStr;
		const char* extCStr;
		if (BMimeType::GetInstalledTypes(&mimDat) == B_OK) {
			while (mimDat.FindString("types", ++mimIdx, &mimCStr) == B_OK) {
				if ((mimTyp.SetTo(mimCStr) == B_OK) && (mimTyp.GetFileExtensions(&extDat) == B_OK)) {
					extIdx = -1;
					while (extDat.FindString("extensions", ++extIdx, &extCStr) == B_OK) {
						BString extStr(extCStr);
						extStr.ToLower();
						if (extStr.ByteAt(0) == '.')  extStr.Remove(0, 1);
						fExtMime.insert(entry(extCStr, mimCStr));
					}
				}
			}
		}
	// perhaps it's better to go with some predefiend types
	} else {
		fExtMime.insert(entry("aiff", "audio/x-aiff"));
		fExtMime.insert(entry("bz2", "application/x-bzip2"));
		fExtMime.insert(entry("cc", "text/x-source-code"));
		fExtMime.insert(entry("cpp", "text/x-source-code"));
		fExtMime.insert(entry("css", "text/css"));
		fExtMime.insert(entry("gif", "image/gif"));
		fExtMime.insert(entry("gz", "application/x-gzip"));
		fExtMime.insert(entry("h", "text/x-source-code"));
		fExtMime.insert(entry("htm", "text/html"));
		fExtMime.insert(entry("html", "text/html"));
		fExtMime.insert(entry("jpeg", "image/jpeg"));
		fExtMime.insert(entry("jpg", "image/jpeg"));
		fExtMime.insert(entry("mod", "audio/x-mod"));
		fExtMime.insert(entry("mov", "video/quicktime"));
		fExtMime.insert(entry("mp3", "audio/x-mpeg"));
		fExtMime.insert(entry("ogg", "audio/ogg.vorbis"));
		fExtMime.insert(entry("pdf", "application/pdf"));
		fExtMime.insert(entry("php", "text/x-php"));
		fExtMime.insert(entry("pl", "text/x-perl"));
		fExtMime.insert(entry("pkg", "application/x-scode-UPkg"));
		fExtMime.insert(entry("png", "image/png"));
		fExtMime.insert(entry("py", "text/x-source-code"));
		fExtMime.insert(entry("rar", "application/x-rar"));
		fExtMime.insert(entry("swf", "application/x-shockwave-flash"));
		fExtMime.insert(entry("tar", "application/x-tar"));
		fExtMime.insert(entry("tga", "image/x-targa"));
		fExtMime.insert(entry("tgz", "application/x-gzip"));
		fExtMime.insert(entry("txt", "text/plain"));
		fExtMime.insert(entry("xml", "text/xml"));
		fExtMime.insert(entry("zip", "application/zip"));
	}

} // CFtpDialog::CFtpDialog

CFtpDialog::~CFtpDialog()
{
	delete[] fReply;
	delete[] fPath;
	
	map<string,BBitmap*>::iterator iter;
	for (iter = fIcons.begin(); iter != fIcons.end(); iter++)
		delete iter->second;
	
	if (fSocket >= 0)
		closesocket(fSocket);
} // CFtpDialog::~CFtpDialog

BBitmap* CFtpDialog::GetIcon(const char *mimeType, const char *tryExtension)
{
	map<string,string>::iterator iterExt;
	map<string,BBitmap*>::iterator iterBmp;

	if (*tryExtension != '\0' && (iterExt = fExtMime.find(tryExtension)) != fExtMime.end())
		return GetIcon(iterExt->second.c_str());
	
	if ((iterBmp = fIcons.find(mimeType)) == fIcons.end())
	{
		BBitmap* bm = new BBitmap(BRect(0, 0, 15, 15), B_COLOR_8_BIT);
		if (BMimeType(mimeType).GetIcon(bm, B_MINI_ICON) != B_OK)
			BMimeType("application/octet-stream").GetIcon(bm, B_MINI_ICON);
		pair<string,BBitmap*> p1(mimeType, bm);
		fIcons.insert(p1);
		return bm;
	}
	else
		return iterBmp->second;
} // CFtpDialog::GetIcon

void CFtpDialog::MakeItSave(const char *name)
{
	fSave = true;
	FindView("name")->Show();
	
	const char *t = strrchr(name, '/');
	if (t)
		SetText("name", t + 1);
	else
		SetText("name", name);
	
	BButton *btn = static_cast<BButton*>(FindView("ok  "));
	btn->SetLabel("Save");
	
	SetTitle("Save on Server");
} // CFtpDialog::MakeItSave

bool CFtpDialog::OKClicked()
{
	CFtpListItem *i = dynamic_cast<CFtpListItem*>(fList->ItemAt(fList->CurrentSelection()));
	if (i == reinterpret_cast<CFtpListItem*>(NULL) && !fSave)
	{
		beep();
		return false;
	}

	if (i && i->IsDirectory())
	{
		ChangeDirectory();
		return false;
	}
	else
	{
		Hide();
	
		try
		{
			BTextView *tv = dynamic_cast<BTextView*>(FindView("shadow"));
			FailNil(tv);

			if (fSave)
			{
				BMessage msg(msg_DoFtpSave);
				msg.AddPointer("url", new URLData(GetText("srvr"), GetText("user"),
					tv->Text(), fPath, GetText("name")));
				fOwner->PostMessage(&msg);
			}
			else
			{
				URLData url(GetText("srvr"), GetText("user"), tv->Text(), fPath, *i);
				gApp->NewWindow(url);
			}
		}
		catch (HErr& e)
		{
			e.DoError();
		}
		
		return true;
	}
} // CFtpDialog::OKClicked

void CFtpDialog::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case 'cnct':
			Connect();
			break;
		
		case msg_SelectedDirectory:
		{
			BMenuItem *src;
			FailOSErr(msg->FindPointer("source", (void**)&src));
			
			strcpy(fPath, "/");
			if (src != fDirectoryPopup->ItemAt(0))
			{
				int i = 1;

				while (true)
				{
					if (i >= fDirectoryPopup->CountItems())
						break;
					
					BMenuItem *I = fDirectoryPopup->ItemAt(i);
					
					strcat(fPath, I->Label());
					strcat(fPath, "/");

					if (src == I)
						break;
					
					++i;
				}
			}
			
			ChangeDirectory();
			break;
		}
		
		case msg_SelectedListItem:
		{
			CFtpListItem *i = dynamic_cast<CFtpListItem*>(fList->ItemAt(fList->CurrentSelection()));
			if (i == reinterpret_cast<CFtpListItem*>(NULL))
			{
				beep();
				return;
			}
			
			if (i->IsDirectory())
			{
				strcat(fPath, *i);
				strcat(fPath, "/");
				ChangeDirectory();
			}
			else if (OKClicked())
				Quit();
			break;
		}
		
		case msg_ToggleDot:
			ListDirectory();
			break;
		
		default:
			HDialog::MessageReceived(msg);
			break;
	}
} // CFtpDialog::MessageReceived

void CFtpDialog::GetReply()
{
	if (! s_gets(fReply, 1024, fSocketFD))
		THROW(("Read error"));

	char *m;
	int r = strtoul(fReply, &m, 10);
	
	if (*m == '-')
	{
		int r2;
		do
		{
			if (! s_gets(fReply, 1024, fSocketFD))
				THROW(("Read error"));
			r2 = strtoul(fReply, &m, 10);
		}
		while (m == NULL || r2 != r || *m != ' ');
	}
} // CFtpDialog::GetReply

inline void FailSockErr(int e)
{
	if (e < 0) THROW(("Network error: %s", strerror(errno)));
} /* FailSockErr */

void CFtpDialog::Connect()
{
	try
	{
		BTextView *tv = dynamic_cast<BTextView*>(FindView("shadow"));
		FailNil(tv);

		if (fSocket >= 0)
		{
			if (fSocketFD) s_close(fSocketFD);
			closesocket(fSocket);
		}
		
		fSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (fSocket < 0)
			THROW(("Failed to get socket: %s", strerror(errno)));
		
		fSocketFD = s_open(fSocket, "r+");
		FailNil(fSocketFD);
		
		SetDefaultButton(static_cast<BButton*>(FindView("ok  ")));
		
		struct hostent *host;
		if ((host = gethostbyname(GetText("srvr"))) == NULL)
			THROW(("Failed to get server address: %s", strerror(errno)));
		
		struct sockaddr_in sa;
		sa.sin_family = AF_INET;
		sa.sin_port = htons(21);
		sa.sin_addr.s_addr = *(unsigned int *)host->h_addr;
		
		FailOSErr(connect(fSocket, (struct sockaddr *)&sa, sizeof(sa)));
		
		int state = 1;
		while (state)
		{
			GetReply();

			switch (state)
			{
				case 1:
					if (*fReply != '2')
						THROW(("Connect failed: %s", fReply));
					s_printf(fSocketFD, "user %s\r\n", GetText("user"));
					state = 2;
					break;
				
				case 2:
					if (*fReply == '3')
					{
						s_printf(fSocketFD, "pass %s\r\n", tv->Text());
						state = 3;
						break;
					}
					else if (*fReply != '2' && *fReply != '5')
						THROW(("Failed to login: %s", fReply));

					// fall thru
				
				case 3:
					if (*fReply == '5')
						THROW(("Incorrect username/password.\n%s", fReply));
					if (state == 3 && *fReply != '2')
						THROW(("Failed to login: %s", fReply));
					
					state = 0;
					break;
			}
		}

		CLogin::SetUsername(GetText("user"));
		gPrefs->SetPrefString("last ftp user", CLogin::Username());
		gPrefs->SetPrefString("last ftp server", GetText("srvr"));
		CLogin::SetPassword(tv->Text());

		GetPWD();
	}
	catch (HErr& e)
	{
		e.DoError();
		s_close(fSocketFD); fSocketFD = NULL;
		closesocket(fSocket); fSocket = -1;
		
		SetDefaultButton(static_cast<BButton*>(FindView("cnct")));
	}
	
	if (fSocketFD)
		ListDirectory();
} // CFtpDialog::Connect

void CFtpDialog::UpdateDirectoryPopup()
{
	BMenu* menu = fDirectoryPopup;
	BMenuItem *item;
	
	while ((item = menu->ItemAt(menu->CountItems() - 1)) != NULL)
	{
		menu->RemoveItem(item);
		delete item;
	}
	
	char p[PATH_MAX], *l;
	strcpy(p, fPath);
	
	l = strtok(p, "/");
	
	menu->AddItem(new BMenuItem("/", new BMessage(msg_SelectedDirectory)));
	
	while (l)
	{
		menu->AddItem(new BMenuItem(l, new BMessage(msg_SelectedDirectory)));
		l = strtok(NULL, "/");
	}
	
	menu->ItemAt(menu->CountItems() - 1)->SetMarked(true);
} // CFtpDialog::UpdateDirectoryPopup

void CFtpDialog::ListDirectory()
{
	if (fSocket < 0)
	{
		beep();
		return;
	}
	
	for (int i = fList->CountItems() - 1; i >= 0; i--)
		delete fList->RemoveItem(i);
	
	int data = 0;
	
	try
	{
		struct sockaddr_in saData;
		struct sockaddr_in saCmd;
		
		data = socket(AF_INET, SOCK_STREAM, 0);
		if (data < 0)
			THROW(("Failed to get socket: %s", strerror(errno)));

		memset(&saData, 0, sizeof(saData));
		saData.sin_family = AF_INET;
		FailSockErr(bind(data, (struct sockaddr *)&saData, sizeof(saData)));
		FailSockErr(listen(data, 5));
		
		// [zooey]: calling getsockname() on a socket that has been bound to 
		// IN_ADDR_ANY (the wildcard-address) will *not* return any IP-address,
		// as this will only be setup by the system during connect or accept.
		// 		[refer to W.R. Stevens - Unix Network Programming, Vol 1, p. 92]
		// BeOS R5 however, *does* fill in the IP-address at this stage (that's
		// why this code worked for R5 but didn't work for BONE).
		// In order to fix this problem, we simply use the IP-address of the
		// command-socket for the PORT-command:
		int size = sizeof(saData);
		// fetch port from data-socket:
		FailSockErr(getsockname(data, (struct sockaddr *)&saData, &size));
		unsigned char *pap = (unsigned char *)&saData.sin_port;
		// fetch ip-address from cmd-socket:
		FailSockErr(getsockname(fSocketFD->sSocket, (struct sockaddr *)&saCmd, &size));
		unsigned char *sap = (unsigned char *)&saCmd.sin_addr.s_addr;
		
		// combine both into the PORT-command:
		s_printf(fSocketFD, "port %d,%d,%d,%d,%d,%d\r\n", sap[0], sap[1], sap[2], sap[3], pap[0], pap[1]);
	
		int state = 1;

		while (state)
		{
			GetReply();

			switch (state)
			{
				case 1:
					if (*fReply != '2')
						THROW(("Port command failed: %s", fReply));
					s_printf(fSocketFD, "list\r\n");
					state = 2;
					break;
				
				case 2:
					if (*fReply == '1')
					{
						int ds;
						FailSockErr(ds = accept(data, (struct sockaddr *)&saData, &size));
						SOCK* dsf = s_open(ds, "r+");
						
						try
						{
							CFtpListItem *item;
							char s[256];
							bool showAll = IsOn("dotf");
							int  entryCount = 0;
							
							while (s_gets(s, 256, dsf))
							{
								entryCount++;
								item = new CFtpListItem(this, s);
								if (item->IsValid() && (showAll || !item->IsDotFile()))
								{
									fList->AddItem(item);
								}
								else
									delete item;
							}
							if (entryCount == 0)
								THROW(("Could not get listing."));

							fList->Invalidate();
							UpdateIfNeeded();
							s_close(dsf);
							closesocket(ds);
						}
						catch (HErr& e)
						{
							EnableUpdates();
							s_close(dsf);
							closesocket(ds);
							throw;
						}
						
						state = 3;
					}
					else
						THROW(("Failed to get listing: %s", fReply));
					break;
				
				case 3:
					if (*fReply != '2')
						THROW(("Something went wrong fetching the directory listing"));
					state = 0;
					break;
			}
		}
		
		closesocket(data);
	}
	catch (HErr& e)
	{
		if (data) closesocket(data);
		e.DoError();
	}
} // CFtpDialog::ListDirectory

void CFtpDialog::ChangeDirectory()
{
	if (fSocket < 0)
	{
		beep();
		return;
	}
	
	try
	{
		s_printf(fSocketFD, "cwd %s\r\n", fPath);
	
		GetReply();

		if (*fReply != '2')
			THROW(("Changing directory failed.\n%s", fReply));
		
		ListDirectory();
	}
	catch (HErr& err)
	{
		err.DoError();
	}

	GetPWD();
} // CFtpDialog::ChangeDirectory

void CFtpDialog::GetPWD()
{
	if (fSocket < 0)
	{
		beep();
		return;
	}
	
	try
	{
		s_printf(fSocketFD, "PWD\r\n");
		
		GetReply();
		
		if (*fReply != '2')
			THROW(("Could not get current working directory.\n%s", fReply));
	
		char *e;
		int n;
		
		if (sscanf(fReply, "%*d \"%n%s\"", &n, fPath) == 1)
		{
			strcpy(fPath, fReply + n);
			if ((e = strchr(fPath, '"')) != NULL) *e = 0;
			
			strcat(fPath, "/");
		}
		else
			strcpy(fPath, "/error!!!");

		UpdateDirectoryPopup();
	}
	catch (HErr& e)
	{
		strcpy(fPath, "/");
		throw;
	}
} // CFtpDialog::GetPWD
