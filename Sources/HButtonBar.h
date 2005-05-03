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

	Created: 12/30/97 23:01:09
*/

#ifndef HBUTTONBAR_H
#define HBUTTONBAR_H

class HHelpWindow;

struct BtnTemplate {
	long resID;
	long cmd;
	long flags;
};

enum BtnFlags {
	bfMenu,
	bfToggle,
	bfSpace,
	bfSeparator,
	bfDualIcon
};

enum BtnBarFlags {
	bbDragger,
	bbAcceptFirstClick
};

class HButtonBar;

class HButton {
public:
			HButton(HButtonBar *bar, int resID, int cmd, float x, int flags, const char *help);
virtual	~HButton();
			
virtual	void Draw(bool pushed = false);
virtual	void MouseEnter(bool pushed = false);
virtual	void MouseLeave();

	void SetOn(bool on);
	void SetDown(bool pushed);
	void SetEnabled(bool enabled = true);
	void SetVisible(bool visible);
	
	BRect Frame() const;
	int Cmd() const;
	bool IsToggle() const;
	bool IsOn() const;
	bool IsDown() const;
	bool IsMenu() const;
	bool IsEnabled() const;
	bool IsVisible() const;
	
	const char* Help() const;

protected:
	HButtonBar *fBar;
	char *fHelp;
	bool fMenu, fToggle, fDown, fEnabled, fOn, fVisible;
	int fCmd;
	unsigned char *fIcon;
	BRect fFrame;
};

class HDualIconButton : public HButton {
public:
			HDualIconButton(HButtonBar *bar, int resID, int cmd, float x, int flags, const char *help);
			
virtual	void Draw(bool pushed = false);
virtual	void MouseEnter(bool pushed = false);
virtual	void MouseLeave();

private:
			unsigned char *fAltIcon;
};

class HSeparator : public HButton {
public:
			HSeparator(HButtonBar *bar, float x);
			
virtual	void Draw(bool pushed = false);
};

class HButtonBar : public BView {
public:
			HButtonBar(BRect frame, const char *name, int resID, BHandler *target = NULL);
			~HButtonBar();
		
virtual	void Draw(BRect update);
virtual	void MouseMoved(BPoint where, uint32 code, const BMessage *a_message);
virtual	void MouseDown(BPoint where);

virtual	void WindowActivated(bool active);
			
			void SetTarget(BHandler *target);
			BHandler* Target() const;
			
			void SetDown(int cmd, bool down);
			void SetOn(int cmd, bool on);
			void SetEnabled(int cmd, bool enabled = true);
			void SetVisible(int cmd, bool visible);
			
			bool IsActive() 		{ return Window()->IsActive(); }

private:
			int FindButton(BPoint where);
			
			void ShowHelp();
			void HideHelp();
virtual	void Pulse();

			bool fDragger, fAcceptFirstClick;
			BHandler *fTarget;
			vector<HButton*> fButtons;
			int fLastButtonOver;
			HHelpWindow *fHelp;
			bigtime_t fLastEnter, fLastDisplay;
};

inline BRect HButton::Frame() const
{
	return fFrame;
} /* HButton::Frame */

inline int HButton::Cmd() const
{
	return fCmd;
} /* HButton::Cmd */

inline bool HButton::IsToggle() const
{
	return fToggle;
} /* HButton::Toggle */

inline bool HButton::IsDown() const
{
	return fDown;
} /* HButton::Down */

inline bool HButton::IsOn() const
{
	return fOn;
} /* HButton::Down */

inline bool HButton::IsMenu() const
{
	return fMenu;
} /* HButton::Menu */

inline bool HButton::IsEnabled() const
{
	return fEnabled;
} /* HButton::IsEnabled */

inline bool HButton::IsVisible() const
{
	return fVisible;
} /* HButton::IsVisible */

inline const char* HButton::Help() const
{
	return fHelp;
} /* HButton::Help */

#endif // HBUTTONBAR_H
