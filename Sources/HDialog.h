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

	Created: 02-06-02
*/

#ifndef HDIALOG_H
#define HDIALOG_H

#include "HStream.h"
#include "HAppResFile.h"

struct dRect {
	short left, top, right, bottom;
	
	BRect ToBe();
};

template <class T>
class DialogCreator {
public:
static	T* CreateDialog(BWindow *owner);
static	T* CreateDialog(BWindow *owner, BPositionIO& tmpl);
};

template <class T>
T* DialogCreator<T>::CreateDialog(BWindow *owner)
{
	size_t size;
	const void *p = HResources::GetResource('DLOG', T::sResID, size);
	if (!p) throw HErr("missing resource");
	BMemoryIO buf(p, size);
	
	T *d = CreateDialog(owner, buf);
	
	return d;
} /* CreateDialog */

template <class T>
T* DialogCreator<T>::CreateDialog(BWindow *owner, BPositionIO& tmpl)
{
	T::RegisterFields();

	dRect r;
	char n[256];
	window_type t;
	int f;

	tmpl >> r >> n >> t >> f;
	return new T(GetPosition(r, owner), n, t, f, owner, tmpl);
} /* DialogCreator<T>::CreateDialog */

template <class T>
void MakeDialog(BWindow* owner, T*& dlog)
{
	dlog = DialogCreator<T>::CreateDialog(owner);
} // MakeDialog

typedef void (*FieldCreator)(int kind, BPositionIO& data, BView*& inside);

class HDialog : public BWindow {
public:
			HDialog(BRect frame, const char *name, window_type type, int flags,
				BWindow *owner, BPositionIO& data);
			~HDialog();
	
			enum { sResID = 100 };
	
			void CreateField(int kind, BPositionIO& data, BView*& inside);
virtual	void MessageReceived(BMessage *msg);

virtual	bool OKClicked();
virtual	bool CancelClicked();
virtual	void UpdateFields();

			bool IsOn(const char *name) const;
			void SetOn(const char *name, bool on = true);
			
			const char* GetText(const char *name) const;
			void SetText(const char *name, const char *text);
			
			const char* GetLabel(const char *name) const;
			void SetLabel(const char *name, const char *label);
			
			void SetEnabled(const char *name, bool enabled = true);
			
			bool GetDouble(const char *name, double& d) const;

			int GetValue(const char *id) const;
			void SetValue(const char *id, int v);
			
static		void RegisterFieldCreator(int kind, FieldCreator fieldCreator);
static		void RegisterFields();

protected:
			void BuildIt(BPositionIO& data);
	
			BView *fMainView;
			BWindow *fOwner;
};

BRect GetPosition(dRect dr, BWindow *owner);
extern float gFactor;


#endif // HDIALOG_H
