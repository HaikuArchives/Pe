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
#include "CFtpDialog.h"
#include "CFtpListItem.h"
#include "CFtpStream.h"
#include "PMessages.h"
#include "PDoc.h"
#include "CLogin.h"
#include "HError.h"
#include "HPreferences.h"
#include <socket.h>
#include <netdb.h>

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
} // CFtpDialog::CFtpDialog

CFtpDialog::~CFtpDialog()
{
	delete[] fReply;
	delete[] fPath;
	
	if (fSocket >= 0)
		closesocket(fSocket);
} // CFtpDialog::~CFtpDialog

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
				new PDoc(url);
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
		struct sockaddr_in sa;
		
		data = socket(AF_INET, SOCK_STREAM, 0);
		if (data < 0)
			THROW(("Failed to get socket: %s", strerror(errno)));

		memset(&sa, 0, sizeof(sa));
		sa.sin_family = AF_INET;
//		sa.sin_addr = INADDR_ANY;
		FailSockErr(bind(data, (struct sockaddr *)&sa, sizeof(sa)));
		FailSockErr(listen(data, 5));
		
		int size = sizeof(sa);
		FailSockErr(getsockname(data, (struct sockaddr *)&sa, &size));
		unsigned char *sap = (unsigned char *)&sa.sin_addr.s_addr, *pap = (unsigned char *)&sa.sin_port;
		
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
						int size = sizeof(sa);
		
						FailSockErr(ds = accept(data, (struct sockaddr *)&sa, &size));
						SOCK* dsf = s_open(ds, "r+");
						
						try
						{
							CFtpListItem *i;
							char s[256];
							
							if (!s_gets(s, 256, dsf))
								THROW(("Could not get listing."));
							
							bool showAll = IsOn("dotf");
							
							while (s_gets(s, 256, dsf))
							{
								i = new CFtpListItem(s);
								if (showAll || *(const char *)*i != '.')
								{
									fList->AddItem(i);
									i->SetHeight(18);
								}
								else
									delete i;
							}

							fList->Invalidate();
							UpdateIfNeeded();
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
