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
#include "HError.h"
#include "HPreferences.h"
#include "Prefs.h"
#ifdef BONE_BUILD
	#define closesocket(X)	close(X)
	#include <arpa/inet.h>
#endif
#include <socket.h>
#include <netdb.h>

static string sfPassword;

const unsigned long
	msg_SelectedListItem = 'slct',
	msg_SelectedDirectory = 'dirS',
	msg_ToggleDot = 'dotT';

CFtpDialog::CFtpDialog(BRect frame, const char *name, window_type type, int flags,
	BWindow *owner)
	: HDialog(frame, name, type, flags, owner, NULL)
{

	fReply = new char[1024];
	fPath = new char[PATH_MAX];
	fSave = false;
	fSocket = -1;

	Create();
	Layout();

#if 0
	// Build Extension->Mimetype list // Takes looong
	typedef pair<string,string> entry;
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
					fExtMime[extCStr] = mimCStr;
				}
			}
		}
	}
#else
	// perhaps it's better to go with some predefiend types:
	fExtMime["aiff"] = "audio/x-aiff";
	fExtMime["bz2"] = "application/x-bzip2";
	fExtMime["cc"] = "text/x-source-code";
	fExtMime["cpp"] = "text/x-source-code";
	fExtMime["css"] = "text/css";
	fExtMime["gif"] = "image/gif";
	fExtMime["gz"] = "application/x-gzip";
	fExtMime["h"] = "text/x-source-code";
	fExtMime["htm"] = "text/html";
	fExtMime["html"] = "text/html";
	fExtMime["jpeg"] = "image/jpeg";
	fExtMime["jpg"] = "image/jpeg";
	fExtMime["mod"] = "audio/x-mod";
	fExtMime["mov"] = "video/quicktime";
	fExtMime["mp3"] = "audio/x-mpeg";
	fExtMime["ogg"] = "audio/ogg.vorbis";
	fExtMime["pdf"] = "application/pdf";
	fExtMime["php"] = "text/x-php";
	fExtMime["pl"] = "text/x-perl";
	fExtMime["pkg"] = "application/x-scode-UPkg";
	fExtMime["png"] = "image/png";
	fExtMime["py"] = "text/x-source-code";
	fExtMime["rar"] = "application/x-rar";
	fExtMime["swf"] = "application/x-shockwave-flash";
	fExtMime["tar"] = "application/x-tar";
	fExtMime["tga"] = "image/x-targa";
	fExtMime["tgz"] = "application/x-gzip";
	fExtMime["txt"] = "text/plain";
	fExtMime["xml"] = "text/xml";
	fExtMime["zip"] = "application/zip";
#endif

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

void CFtpDialog::Create(void)
{
	fMainView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	// Server-Box
	fLoginBox = new HBox(fMainView, "", B_FOLLOW_RIGHT|B_FOLLOW_TOP);
	fServerName = new HTextControl(fLoginBox, "srvr");
	fUserName = new HTextControl(fLoginBox, "user");
	fPassword = new HTextControl(fLoginBox, "pass");
	fPassword->TextView()->HideTyping(true);
	fUsePassive = new HCheckBox(fLoginBox, "pssv");
	fUsePassive->SetValue(gPrefs->GetPrefInt(prf_I_PassiveFtp, 1));
	fConnectButton = new HButton(fLoginBox, "cnct", 'cnct');

	fFileName = new HTextControl(fMainView, "name", 
								 B_FOLLOW_LEFT_RIGHT|B_FOLLOW_BOTTOM);

	// Buttons
	fCancelButton = new HButton(fMainView, "cncl", 'cncl', 
								B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM);
	fOkButton = new HButton(fMainView, "ok", 'ok  ', 
							B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM);

	// List
	fDirectoryField = new HMenuField(fMainView, "path");
	fShowDotted = new HCheckBox(fMainView, "dotf", 
								new BMessage(msg_ToggleDot),
								B_FOLLOW_RIGHT|B_FOLLOW_TOP);

	fListView = new BListView(Bounds(), "list", B_MULTIPLE_SELECTION_LIST, 
							  B_FOLLOW_ALL);
	fListView->SetInvocationMessage(new BMessage(msg_SelectedListItem));
	fListView->SetListType(B_MULTIPLE_SELECTION_LIST);
	fScrollView = new HScrollView(fMainView, "scrll", fListView, B_FOLLOW_ALL, 
								  B_FRAME_EVENTS, false, true);

	SetDefaultButton(fConnectButton);
	
	fServerName->SetText(gPrefs->GetPrefString(prf_S_LastFtpServer));
	fUserName->SetText(gPrefs->GetPrefString(prf_S_LastFtpUser));

	if (sfPassword.length())
		fPassword->SetText(sfPassword.c_str());

	if (strlen(GetText("srvr")) && strlen(GetText("user")))
		fPassword->MakeFocus();
	else
		fServerName->MakeFocus();
	
	fFileName->Hide();

} // CFtpDialog::Create

void CFtpDialog::Layout(void)
{
	fServerName->ResizeLocalized("Host:");
	fUserName->ResizeLocalized("Username:");
	fPassword->ResizeLocalized("Password:");
	fUsePassive->ResizeLocalized("Passive Mode");
	fConnectButton->ResizeLocalized("Connect");
	fCancelButton->ResizeLocalized("Cancel");
	fOkButton->ResizeLocalized("Open");
	fShowDotted->ResizeLocalized("Show All!");
	fDirectoryField->ResizeLocalized();
	fFileName->ResizeLocalized();

	//### Layout
	float dx = fMainView->StringWidth("m");
	float dy = fMainView->StringWidth("n");

	// Server-Box
	//
	fServerName->MoveTo(dx, dy);
	fUserName->MoveTo(dx, fServerName->Bottom()+dy);
	fPassword->MoveTo(dx, fUserName->Bottom()+dy);
	fUsePassive->MoveTo(dx, fPassword->Bottom()+dy);
	fConnectButton->MoveTo(2*dx, fUsePassive->Bottom()+dy);
	//
	float bw = max(max(dx*10, fCancelButton->Width()), fOkButton->Width());
	bw = max(bw, fServerName->Right());
	bw = max(bw, fUserName->Right());
	bw = max(bw, fPassword->Right());
	bw = max(bw, fUsePassive->Right());
	bw = max(bw, fConnectButton->Right()+2*dx);
	//
	fServerName->SetWidth(bw);
	fUserName->SetWidth(bw);
	fPassword->SetWidth(bw);
	fUsePassive->SetWidth(bw);
	fConnectButton->SetWidth(bw-2*dx);
	//
	fLoginBox->MoveTo(fMainView->Right()-bw-3*dx, dy);
	fLoginBox->ResizeTo(bw+2*dx, fConnectButton->Bottom()+dy);

	// Buttons
	fOkButton->MoveTo(fLoginBox->Left()+dx, 
					  fMainView->Bottom()-dy-fOkButton->Height());
	fOkButton->SetWidth(bw);
	//
	fCancelButton->MoveTo(fOkButton->Left(), 
						  fOkButton->Top()-dy-fCancelButton->Height());
	fCancelButton->SetWidth(bw);

	// List
	fDirectoryField->MoveTo(dx, dy);
	fShowDotted->MoveTo(fLoginBox->Left()-fShowDotted->Width()-dx, dy);
	//
	fScrollView->MoveTo(dx, fDirectoryField->Bottom()+dy);
	fScrollView->ResizeTo(fShowDotted->Right()-dx, 
						  Frame().Height()-fDirectoryField->Bottom()-2*dy);
	//
	fFileName->MoveTo(dx, fScrollView->Bottom()-fFileName->Height());
	fFileName->SetWidth(fScrollView->Width());
	//
	float minW = 200;
	float minH = 100;
	minW = max(minW, 
		       bw + 3*dx + fShowDotted->Width() + fDirectoryField->Width());
	minH = max(minH, fLoginBox->Bottom()+2*fOkButton->Height()+3*dy);
	ResizeToLimits(minW, 99999, minH, 99999);
} // CFtpDialog::Layout

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
		fIcons[mimeType] = bm;
		return bm;
	}
	else
		return iterBmp->second;
} // CFtpDialog::GetIcon

void CFtpDialog::MakeItSave(const char *name)
{
	fSave = true;
	fListView->SetListType(B_SINGLE_SELECTION_LIST);

	fFileName->Show();
	fScrollView->ResizeBy(0, -fFileName->Height() - 2);
	const char *t = strrchr(name, '/');
	if (t)
		fFileName->SetText(t + 1);
	else
		fFileName->SetText(name);
	
	fOkButton->SetLabel("Save");
	
	SetTitle("Save on Server");
} // CFtpDialog::MakeItSave

bool CFtpDialog::OKClicked()
{
	CFtpListItem *i;

	if (fSave)
	{
		i = dynamic_cast<CFtpListItem*>(
			fListView->ItemAt(fListView->CurrentSelection()));
		if (i && i->IsDirectory())
		{
			ChangeDirectory();
			return false;
		}
		else
		{
			try
			{
				BMessage msg(msg_DoFtpSave);
				msg.AddPointer("url", new URLData(GetText("srvr"), GetText("user"),
					GetText("pass"), fPath, GetText("name")));
				if (fOwner)
					fOwner->PostMessage(&msg);
				else
					fCaller.SendMessage(&msg);
				return true;
			}
			catch (HErr& e)
			{
				e.DoError();
			}
		}
	}
	else
	{
		int32 dirCnt = 0;
		int32 filCnt = 0;
		int32 n = -1;
		while ((i = dynamic_cast<CFtpListItem*>(
			fListView->ItemAt(fListView->CurrentSelection(++n))))) {
			if (i && i->IsDirectory())
				dirCnt++;
			else
				filCnt++;
		}
		if (dirCnt == 1  && filCnt == 0)
		{
			ChangeDirectory();
			return false;
		}
		else if (dirCnt == 0 && filCnt > 0)
		{
			n = -1;
			Hide();
			while ((i = dynamic_cast<CFtpListItem*>(
				fListView->ItemAt(fListView->CurrentSelection(++n))))) {
				try
				{
					URLData url(GetText("srvr"), GetText("user"), GetText("pass"), fPath, *i);
					gApp->NewWindow(url);
				}
				catch (HErr& e)
				{
					e.DoError();
				}
			}
			
			return true;
		}
		else
		{
			beep();
		}
	}
	return false;
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
			if (src != fDirectoryField->Menu()->ItemAt(0))
			{
				int i = 1;

				while (true)
				{
					if (i >= fDirectoryField->Menu()->CountItems())
						break;
					
					BMenuItem *I = fDirectoryField->Menu()->ItemAt(i);
					
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
			CFtpListItem *i = dynamic_cast<CFtpListItem*>(
				fListView->ItemAt(fListView->CurrentSelection()));
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
						s_printf(fSocketFD, "pass %s\r\n", GetText("pass"));
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

		gPrefs->SetPrefString(prf_S_LastFtpUser, GetText("user"));
		gPrefs->SetPrefString(prf_S_LastFtpServer, GetText("srvr"));
		sfPassword = GetText("pass");

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
	BMenu* menu = fDirectoryField->Menu();
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
	
	for (int i = fListView->CountItems() - 1; i >= 0; i--)
		delete fListView->RemoveItem(i);
	
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
		int size = sizeof(saData);

		bool passive = IsOn("pssv");
		if (passive) {
			// switch to passive mode
			s_printf(fSocketFD, "pasv\r\n");
		} else {
			FailSockErr(bind(data, (struct sockaddr *)&saData, sizeof(saData)));
			FailSockErr(listen(data, 5));
			// [zooey]: calling getsockname() on a socket that has been bound to 
			// IN_ADDR_ANY (the wildcard-address) will *not* return any IP-address,
			// as this will only be setup by the system during connect or accept.
			// 	[refer to W.R. Stevens - Unix Network Programming, Vol 1, p. 92]
			// BeOS R5 however, *does* fill in the IP-address at this stage (that's
			// why this code worked for R5 but didn't work for BONE).
			// In order to fix this problem, we simply use the IP-address of the
			// command-socket for the PORT-command:
			//
			// fetch port from data-socket:
			FailSockErr(getsockname(data, (struct sockaddr *)&saData, &size));
			unsigned char *pap = (unsigned char *)&saData.sin_port;
			// fetch ip-address from cmd-socket:
			FailSockErr(getsockname(fSocketFD->sSocket, (struct sockaddr *)&saCmd, 
											&size));
			unsigned char *sap = (unsigned char *)&saCmd.sin_addr.s_addr;
			// combine both into the PORT-command:
			s_printf(fSocketFD, "port %d,%d,%d,%d,%d,%d\r\n", sap[0], sap[1], 
						sap[2], sap[3], pap[0], pap[1]);
		}
	
		int state = 1;
		SOCK* dsf = NULL;

		while (state)
		{
			GetReply();

			switch (state)
			{
				case 1:
					if (passive) {
						unsigned int sap[4];
						unsigned int pap[2];
						if (*fReply != '2')
							THROW(("Pasv command failed: %s", fReply));
						char* pos = strchr(fReply,'(');
						if (!pos)
							THROW(("Answer to Pasv has unknown format: %s", fReply));
						int cnt = sscanf(pos+1, "%u,%u,%u,%u,%u,%u", 
											  &sap[0], &sap[1], &sap[2], &sap[3], 
											  &pap[0], &pap[1]);
						if (cnt != 6)
							THROW(("Could not parse answer to Pasv (%d of 6): %s", 
									 cnt, fReply));
						char ipAddr[20];
						sprintf(ipAddr, "%d.%d.%d.%d", sap[0], sap[1], sap[2], sap[3]);
						saData.sin_port = htons(pap[0]*256+pap[1]);
						saData.sin_addr.s_addr = inet_addr(ipAddr);
						FailOSErr(connect(data, (struct sockaddr *)&saData, sizeof(saData)));
						dsf = s_open(data, "r+");
					} else {
						if (*fReply != '2')
							THROW(("Port command failed: %s", fReply));
					}
					s_printf(fSocketFD, "list\r\n");
					state = 2;
					break;
				
				case 2:
					if (*fReply == '1')
					{
						int ds = 0;
						if (!passive) {
							FailSockErr(ds = accept(data, (struct sockaddr *)&saData, &size));
							dsf = s_open(ds, "r+");
						}
						
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
									fListView->AddItem(item);
								}
								else
									delete item;
							}
							if (entryCount == 0)
								THROW(("Could not get listing."));

							fListView->Invalidate();
							UpdateIfNeeded();
							s_close(dsf);
							if (!passive)
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
