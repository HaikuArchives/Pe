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

	Created: 10/21/97 21:16:55
*/

#include "pe.h"
#include "PDoc.h"
#include "PText.h"
#include "CChangeCase.h"
#include "PMessages.h"

CChangeCase::CChangeCase(BRect frame, const char *name, window_type type, int flags,
	BWindow *owner, BPositionIO& data)
	: HDialog(frame, name, type, flags, owner, data)
{
	SetOn("lower", true);
	Show();
} /* CChangeCase::CChangeCase */
		
bool CChangeCase::OKClicked()
{
	BMessage m(msg_DoChangeCase);
	
	int newCase;
	
	if (IsOn("lower"))
		newCase = 1;
	else if (IsOn("upper"))
		newCase = 2;
	else if (IsOn("capword"))
		newCase = 3;
	else if (IsOn("capsent"))
		newCase = 4;
	else
		newCase = 5;

	m.AddInt32("newcase", newCase);

	PDoc *d = static_cast<PDoc*>(fOwner);
	if (d)
		BMessenger(d->TextView()).SendMessage(&m);
	else
		THROW(("Huh???"));
	
	return true;
} /* CChangeCase::OKClicked */

void CChangeCase::UpdateFields()
{
	
} /* CChangeCase::UpdateFields */
