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

	Created: 03/30/98 22:58:03
*/

#include "pe.h"
#include "CLogin.h"
#include "CFtpDialog.h"

char CLogin::sfUsername[25] = "", CLogin::sfPassword[129] = "";

CLogin::CLogin(BRect frame, const char *name, window_type type, int flags,
				BWindow *owner, BPositionIO& data)
	: HDialog(frame, name, type, flags, owner, data)
{
	BTextControl *pwd = dynamic_cast<BTextControl*>(FindView("password"));
	if (sfUsername[0])
		pwd->SetText("********");
	
	BTextView *shadow;
	AddChild(shadow = new BTextView(BRect(0, 0, 0, 0), "shadow", BRect(0, 0, 0, 0), 0, 0));
	shadow->SetText(sfPassword);
	
	if (pwd)
		pwd->TextView()->AddFilter(new CPassWordSniffer(shadow));

	BTextControl *username = dynamic_cast<BTextControl*>(FindView("username"));
	if (sfUsername[0])
		username->SetText(sfUsername);
	username->MakeFocus(true);
} /* CLogin::CLogin */

CLogin::~CLogin()
{
} /* CLogin::~CLogin */
			
bool CLogin::OKClicked()
{
	BTextView *tv = dynamic_cast<BTextView*>(FindView("shadow"));

	if (tv)
	{
		strcpy(fUsername, GetText("username"));
		strcpy(sfUsername, fUsername);
		strcpy(fPassword, tv->Text());
		strcpy(sfPassword, fPassword);
		*fOK = true;
	}

	return true;
} /* CLogin::OKClicked */

bool CLogin::CancelClicked()
{
	*fOK = false;
	return true;
} /* CLogin::CancelClicked */

void CLogin::Connect(const char *server, char *username, char *password, bool *ok)
{
	char name[NAME_MAX];
	BStringView *cap = dynamic_cast<BStringView*>(FindView("server"));
	if (cap)
	{
		sprintf(name, cap->Text(), server);
		cap->SetText(name);
	}
	
	fUsername = username;
	fPassword = password;
	fOK = ok;
	
	Show();
	
	long l;
	wait_for_thread(Thread(), &l);
} /* CLogin::Connect */
