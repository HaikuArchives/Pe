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

	Created: 03/09/98 22:53:28
*/

#ifndef CHTMLBAR_H
#define CHTMLBAR_H


class CHTMLBar;
class CHTMLButtonPane;
class CHTMLButton;

class CHTMLBar : public BWindow {
public:

virtual	void MessageReceived(BMessage *msg);

static		CHTMLBar* Instance();
static		void Close();
			
private:
			CHTMLBar();
virtual	bool QuitRequested();

static		CHTMLBar *sfInstance;
			CHTMLButtonPane *fButtonPane;
};

#pragma mark - Pane

class CHTMLButtonPane : public BView {
public:
			CHTMLButtonPane(BRect frame, const char *name, BPositionIO& data);
			
virtual	void Draw(BRect update);
virtual	void MouseDown(BPoint where);

			float BaseLine() const;
			float ButtonHeight() const;
			
private:
			vector<CHTMLButton*> fButtons;
			font_height fFH;
};

inline float CHTMLButtonPane::BaseLine() const
{
	return fFH.descent;
} /* CHTMLButtonPane::BaseLine */

inline float CHTMLButtonPane::ButtonHeight() const
{
	return fFH.ascent + fFH.descent + fFH.leading + 7;
} /* CHTMLButtonPane::ButtonHeight */

#pragma mark - Button

class CHTMLButton {
public:
			CHTMLButton(BRect frame, BPositionIO& data, CHTMLButtonPane *pane);
virtual	~CHTMLButton();
			
			BRect Frame() const;
			
virtual	void Draw();
virtual	void Click(BPoint where) = 0;
			
protected:
			
			bool TrackClick();
			
			BRect fFrame;
			CHTMLButtonPane *fPane;
			char *fLabel;
};

class CHTMLSimpleButton : public CHTMLButton {
public:
			CHTMLSimpleButton(BRect frame, BPositionIO& data, CHTMLButtonPane *pane);
			~CHTMLSimpleButton();
			
virtual	void Draw();
virtual	void Click(BPoint where);
			
private:
			char *fGlossary;
};

class CHTMLMenu : public CHTMLButton {
public:
			CHTMLMenu(BRect frame, BPositionIO& data, CHTMLButtonPane *pane);
			~CHTMLMenu();

virtual	void Draw();
virtual	void Click(BPoint where);

private:
			class CItem : public BMenuItem {
			public:
				CItem(const char *label, const char *glossy)
					: BMenuItem(label, NULL)
				{
					fGlossy = strdup(glossy);
				};
				~CItem()
				{
					free(fGlossy);
				};
					
				char *fGlossy;
			};
			
			BPopUpMenu *fMenu;
};

class CHTMLCmdButton : public CHTMLButton {
public:
			CHTMLCmdButton(BRect frame, BPositionIO& data, CHTMLButtonPane *pane);
			
virtual	void Click(BPoint where);
			
private:
			unsigned long fCmd;
};

class CHTMLDialog : public CHTMLButton {
public:
			CHTMLDialog(BRect frame, BPositionIO& data, CHTMLButtonPane *pane);
			~CHTMLDialog();

virtual	void Click(BPoint where);

private:
			char *fExt;
};

inline BRect CHTMLButton::Frame() const
{
	return fFrame;
} /* CHTMLButton::Frame */

#endif // CHTMLBAR_H
