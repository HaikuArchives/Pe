//-----------------------------------------------------------------------------
// BeBookFetch: looks up the selected text against BeBook's doc bookmarks.
//
// Contains (legally) stolen code from:
//  - Alan Ellis's Be API Fetch for Eddie (http://www.bebits.com/app/6).
//
// Pe version and new bugs by Oscar Lesta.
//
// ToDo: improve the popup display by reading doc_bookmark attributes to avoid
//       duplicates items (append, for example, "be:kit_name" attr content).
//-----------------------------------------------------------------------------

#include <vector>
#include <ctype.h>

#include <Alert.h>
#include <AppDefs.h>
#include <MenuItem.h>
#include <Messenger.h>
#include <Path.h>
#include <PopUpMenu.h>
#include <Query.h>
#include <Roster.h>
#include <String.h>
#include <Volume.h>
#include <VolumeRoster.h>

#include <typeinfo> // otherwise I get an error in PDoc.h :-/
#include <Debug.h>  // ditto.
#include "PDoc.h"

#include "MTextAddOn.h"
#include "HButtonBar.h"
#include "PToolBar.h"

//------------------------------------------------------------------------------

#define kBeTypeAttrib  "BEOS:TYPE"
#define kIndexFileType "application/x-vnd.Be-doc_bookmark"
#define kName          "name"
#define kFuncDirName   "CodeName"
#define kTrackerSig    "application/x-vnd.Be-TRAK"
#define kNoMatch       "Nothing was found for: "
#define kAlertName     "Pe's BeAPI Fetcher Alert"
#define kBtnText       "Mmmm"
#define kPopupName     "API Selections"

//------------------------------------------------------------------------------

#if __INTEL__
	extern "C" _EXPORT long perform_edit(MTextAddOn *addon);
#else
	#pragma export on
	extern "C" {
		long perform_edit(MTextAddOn *addon);
	}
	#pragma export reset
#endif

//-----------------------------------------------------------------------------

// Function Prototypes
uint32	FetchQuery(query_op Op, const char* Selection, vector<BString>& Results, bool CaseSensitive = false);
void	DisplayResults(vector<BString>& Results, MTextAddOn *addon);
BString	FormatResultString(const BString& ResultPath);
void	PopUpSelection(vector<BString>& Results, MTextAddOn *addon);
void	OpenDoc(char* arg);

//-----------------------------------------------------------------------------

long perform_edit(MTextAddOn *addon)
{
	long selStart;
	long selEnd;

	addon->GetSelection(&selStart, &selEnd);

	if (selEnd <= selStart)
		return B_ERROR;

	int length = selEnd - selStart;

	// Do _NOT_ Query if we have less than 3 chars. It works, but takes ages and
	// yields to an unuseful giant popup menu.
	if (length < 3)
	{
		(new BAlert("BeBookFetch", "The text selection is too short.\n"
		"Please select three or more characters.", "Ok"))->Go(NULL);

		return B_ERROR;
	}

	BString selection;
	selection.SetTo(addon->Text() + selStart, length);

    // ... to avoid hanging Pe if we are called with long selections.
	int pos = 0;
	while ((pos < length) || pos < 30) // Hope 30 is ok.
	{
		if (!isalnum(selection[pos]) && (selection[pos] != '_')) break;
		pos++;
	}

	if (!pos) return B_ERROR;

	selection.Truncate(pos);

	// Do _NOT_ Query if we have less than 3 chars. It works, but takes ages and
	// yields to an unuseful giant popup menu.
	if (selection.CountChars() < 3)
	{
		(new BAlert("BeBookFetch", "The text selection is too short.\n"
			"Make sure the selection doesn't contains puntuations or symbols.",
			"Ok"))->Go(NULL);

		return B_ERROR;
	}

	vector <BString> results;

	if (FetchQuery(B_EQ, selection.String(), results) > 0)
	{
		DisplayResults(results, addon);
	}
	else
	{
		// Lets try the same selection as a function name ...
		BString funcname(selection.String());
		funcname += "()";

		if (FetchQuery(B_EQ, funcname.String(), results) > 0)
		{
			DisplayResults(results, addon);
		}
		else
		{
			// Still nothing? Lets Slap a B on the front and check for Classes!
			BString section("B");
			section += selection.String();

			if (FetchQuery(B_EQ, section.String(), results, false) > 0)
			{
				DisplayResults(results, addon);
			}
			else
			{
				if (FetchQuery(B_CONTAINS, selection.String(), results, false) > 0)
				{
					DisplayResults(results, addon);
				}
				else
				{
					BString message(kNoMatch);
					message += selection.String();
					(new BAlert(kAlertName, message.String(), kBtnText))->Go(NULL);
				}
			}
		}
	}
	return B_NO_ERROR;
}

//-----------------------------------------------------------------------------

uint32 FetchQuery(query_op Op, const char* Selection, vector<BString>& Results,
                  bool CaseSensitive)
{
	BQuery query;

	query.PushAttr(kBeTypeAttrib);
	query.PushString(kIndexFileType);
	query.PushOp(B_EQ);
	query.PushAttr(kName);
	query.PushString(Selection, CaseSensitive);
	query.PushOp(Op);
	query.PushOp(B_AND);

	BVolume vol;
	BVolumeRoster roster;

	roster.GetBootVolume(&vol);
	query.SetVolume(&vol);

	if (B_NO_INIT == query.Fetch())
	{
		return 0;
	}

	BEntry entry;
	BPath path;
	int32 counter = 0;

	while (query.GetNextEntry(&entry) != B_ENTRY_NOT_FOUND)
	{
		if (entry.InitCheck() == B_OK)
		{
			entry.GetPath(&path);

			if (path.InitCheck() == B_OK)
			{
				Results.push_back(path.Path());
				counter++;
			}
		}
	}
	return counter;
}

//-----------------------------------------------------------------------------

void DisplayResults(vector<BString>& Results, MTextAddOn *addon)
{
	if (Results.size() < 2)
	{
		char* arg = (char *)Results[0].String();
		OpenDoc(arg);
	}
	else
	{
		PopUpSelection(Results, addon);
	}
}

//-----------------------------------------------------------------------------

void OpenDoc(char* arg)
{
// Note:
// I don't just call be_roster->Launch() here because:
// if you happen to have another Tracker exec somewhere on your boot volume...
// it will get executed instead of just calling the already-running-Tracker's
// ArgRecv method. And that's bad, because you get two Tracker running, and it
// takes too long to get the results :-)

	BMessage msg(B_ARGV_RECEIVED);
	msg.AddString("cwd", "/"); // OT's TTracker::ArgvReceived fault.
	msg.AddString("argv", "Tracker");
	msg.AddString("argv", arg);
	msg.AddInt32("argc", 2);

	BMessenger messenger(kTrackerSig);
	if (messenger.IsValid())
		messenger.SendMessage(&msg);
	else
		be_roster->Launch(kTrackerSig, 1, &arg);
}

//-----------------------------------------------------------------------------

BString FormatResultString(const BString& ResultPath)
{
	BString result;

	int32 end = ResultPath.FindFirst(kFuncDirName) - 2;
	int32 start = ResultPath.FindLast('/', end) + 1;

	if (end - start > 1)
	{
		ResultPath.CopyInto(result, start, end - start + 1);
		result += "::";
		result += &ResultPath.String()[ResultPath.FindLast('/', ResultPath.Length()) + 1];
	}
	else
	{
		int32 secbegin = ResultPath.FindLast('/', ResultPath.Length()) + 1;

		ResultPath.CopyInto(result, secbegin, ResultPath.Length() - secbegin);
	}

	return result;
}

//------------------------------------------------------------------------------

void PopUpSelection(vector<BString>& Results, MTextAddOn *addon)
{
	BPopUpMenu selections(kPopupName, false, false);
	selections.SetFont(be_plain_font);

	for (uint32 i = 0; i < Results.size(); i++)
	{
		BMenuItem* item = new BMenuItem(FormatResultString(Results[i]).String(), new BMessage(i));
		selections.AddItem(item);
	}

	// Here we get a nice BPoint to popup the results (end of the toolbar).
	BPoint menupos;

	if (addon->Window()->Lock()) {
		float x = addon->Window()->FindView("ButtonBar")->Frame().right + 2;
		float y = addon->Window()->FindView("ToolBar")->Frame().Height() - 2;

		menupos = addon->Window()->Frame().LeftTop() + BPoint(x, y);
		addon->Window()->Unlock();
	}

	BMenuItem* item = selections.Go(menupos, false, true);

	if (item != NULL)
	{
		int index = item->Command();
		char* arg = (char *)Results[index].String();
		OpenDoc(arg);
	}
}

//------------------------------------------------------------------------------
