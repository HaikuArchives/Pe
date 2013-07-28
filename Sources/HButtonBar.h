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

#include "HLibHekkel.h"

class HHelpWindow;
class HButtonBar;

using std::vector;

struct IMPEXP_LIBHEKKEL BtnTemplate {
	int32 resID;
	int32 cmd;
	int32 flags;
};

IMPEXP_LIBHEKKEL enum BtnFlags {
	bfMenu,
	bfToggle,
	bfSpace,
	bfSeparator,
	bfDualIcon
};

IMPEXP_LIBHEKKEL enum BtnBarFlags {
	bbDragger,
	bbAcceptFirstClick
};


class IMPEXP_LIBHEKKEL HTool {
public:
			HTool(HButtonBar *bar, float x, float width, int32 cmd, const char *help);
virtual	~HTool();

virtual	void Draw(bool pushed = false) = 0;
virtual	void MouseEnter(bool pushed = false);
virtual	void MouseLeave();

	void SetOn(bool on);
	void SetDown(bool pushed);
	void SetEnabled(bool enabled = true);
	void SetVisible(bool visible);

	BRect Frame() const			{ return fFrame; }
	int32 Cmd() const			{ return fCmd; }
	bool IsToggle() const		{ return fToggle; }
	bool IsDown() const			{ return fDown; }
	bool IsOn() const			{ return fOn; }
	bool IsMenu() const			{ return fMenu; }
	bool IsEnabled() const		{ return fEnabled; }
	bool IsVisible() const		{ return fVisible; }
	const char* Help() const	{ return fHelp; }

protected:
	void DrawFrame(bool enter, bool active);
	void DrawButton(unsigned char *icondat, bool pushed = false);
	void ReadToolbarImage(unsigned char** dest, int32 resID);

	BRect fFrame;
	HButtonBar *fBar;
	char *fHelp;
	int32 fCmd;
	bool fMenu, fToggle, fEnabled, fVisible, fDown, fOn;
	unsigned char *fImageStd, *fImageAlt;
};

class IMPEXP_LIBHEKKEL HToolButton : public HTool {
public:
			HToolButton(HButtonBar *bar, int32 resID, int32 cmd, float x, int32 flags, const char *help);

virtual	void Draw(bool pushed = false);
virtual	void MouseEnter(bool pushed = false);
virtual	void MouseLeave();
};

class IMPEXP_LIBHEKKEL HToolStateButton : public HTool {
public:
			HToolStateButton(HButtonBar *bar, int32 resID1, int32 resID2, int32 cmd, float x, int flags, const char *help);

virtual	void Draw(bool pushed = false);
virtual	void MouseEnter(bool pushed = false);
virtual	void MouseLeave();
};

class IMPEXP_LIBHEKKEL HToolSeparator : public HTool {
public:
			HToolSeparator(HButtonBar *bar, float x);

virtual	void Draw(bool pushed = false);
};

class IMPEXP_LIBHEKKEL HButtonBar : public BView {
public:
			HButtonBar(BRect frame, const char *name, int32 resID, BHandler *target = NULL);
			~HButtonBar();

virtual	void Draw(BRect update);
virtual	void MouseMoved(BPoint where, uint32 code, const BMessage *a_message);
virtual	void MouseDown(BPoint where);

virtual	void WindowActivated(bool active);

			void SetTarget(BHandler *target);
			BHandler* Target() const;

			void SetDown(int32 cmd, bool down);
			void SetOn(int32 cmd, bool on);
			void SetEnabled(int32 cmd, bool enabled = true);
			void SetVisible(int32 cmd, bool visible);

			bool IsActive() 		{ return Window()->IsActive(); }

private:
			int32 FindTool(BPoint where);

			void ShowHelp();
			void HideHelp();
virtual	void Pulse();

			bool fDragger, fAcceptFirstClick;
			BHandler *fTarget;
			vector<HTool*> fTools;
			int32 fLastToolOver;
			HHelpWindow *fHelp;
			bigtime_t fLastEnter, fLastDisplay;
};


#endif // HBUTTONBAR_H
