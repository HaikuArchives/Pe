//==================================================================
//	MTextAddOn.h
//	Copyright 1996  Metrowerks Corporation, All Rights Reserved.
//==================================================================
//	This is a proxy class used by Editor add_ons.  It does not inherit from BView
//	but provides an abstract interface to a text engine.
 
#ifndef MTEXTADDON_H
#define MTEXTADDON_H

class PExtCmd;
class PText;
typedef PText MIDETextView;
class BWindow;
struct entry_ref;

class MTextAddOn
{
public:
								MTextAddOn(
									MIDETextView&	inTextView);

								MTextAddOn(
									MIDETextView&	inTextView,
									const char *extension);

	virtual						~MTextAddOn();
	virtual	const char*			Text();
	virtual	int32				TextLength() const;
	virtual	void				GetSelection(
									int32 *start, 
									int32 *end) const;
	virtual	void				Select(
									int32 newStart, 
									int32 newEnd);
	virtual void				Delete();
	virtual void				Insert(
									const char* inText);
	virtual void				Insert(
									const char* text, 
									int32 length);

	virtual	BWindow*			Window();
	virtual status_t			GetRef(
									entry_ref&	outRef);

private:

	MIDETextView&				fText;
	PExtCmd						*fCmd;
	bool							fDirty;
};

#if !__INTEL__
#pragma export on
#endif
extern "C" {
long perform_edit(MTextAddOn *addon);
}
#if !__INTEL__
#pragma export reset
#endif

typedef long (*perform_edit_func)(MTextAddOn *addon);

#endif
