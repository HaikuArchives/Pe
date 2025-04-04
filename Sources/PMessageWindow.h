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

	Created: 09/29/97 11:46:57
*/

#ifndef PMESSAGEWINDOW_H
#define PMESSAGEWINDOW_H

class PMessageItem : public BListItem {
public:
			PMessageItem();
			~PMessageItem();
	
virtual	void DrawItem(BView *owner, BRect bounds, bool complete = false);
virtual	void Update(BView *owner, const BFont *font);

			void SetError(const char *err);
			void SetFile(const char *file);
			void SetLine(int line);
			void SetSel(int selStart, int selLen);
			void SetSrc(const char *src);
			void SetDesc(const char *desc);
			void SetKind(int kind);
			
			void ExtendError(const char *err, int indx = -1);
			
			float PreferredHeight() const;
			bool IsValid() const;
		
virtual	void ShowMessage();

protected:
			void InitIcons();
		
static		BBitmap *sfErrorIcon, *sfErrorIconSelected,
				*sfInfoIcon, *sfInfoIconSelected,
				*sfWarningIcon, *sfWarningIconSelected;

			char *fErr;
			char *fFile;
			char *fSrc;
			int fLine;
			int fSelStart, fSelLen;
			char *fDesc;
			int fState;
			int fKind;
};

class PMessageWindow : public BWindow {
public:
		PMessageWindow(const char *title);
		~PMessageWindow();

virtual	void MessageReceived(BMessage *msg);
		
		void AddMessage(PMessageItem *err);

private:
		BListView *fMessageList;
};

#endif // PMESSAGEWINDOW_H
