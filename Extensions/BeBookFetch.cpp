//-----------------------------------------------------------------------------
// Ashamelessly stolen from Alan Ellis's Be API Fetch for Eddie.
//-----------------------------------------------------------------------------

/*
This is an extension for Pe. It allows you to search the BeBook for the current
selection (as BeIDE does).
I used Alan Ellis's Be API Fetch (http://www.bebits.com/app/6) as a base (yeah,
riiight!). Ok, I stole most of his code, but he allow us to do so (phew!) :-)

In the current encarnation you only get displayed the first result of the query.
This means that you don't get a window with the results (like BeIDE has).
Anyway... its handy.
*/

#include <vector>
#include <ctype.h>

#include <Alert.h>
#include <Path.h>
#include <Query.h>
#include <Roster.h>
#include <String.h>
#include <Volume.h>
#include <VolumeRoster.h>

#include "MTextAddOn.h"

#define kBeTypeAttrib  "BEOS:TYPE"
#define kIndexFileType "application/x-vnd.Be-doc_bookmark"
#define kName          "name"
#define kFuncDirName   "CodeName"
#define kTrackerSig    "application/x-vnd.Be-TRAK"
#define kNoMatch       "Nothing was found for: "
#define kAlertName     "Pe's BeAPI Fetcher Alert"
#define kBtnText       "Mmmm"

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

uint32 FetchQuery(query_op Op, const char* Selection, vector<BString>& Results,
                  bool CaseSensitive = true)
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

void DisplayResults(vector<BString>& Results)
{
// For now just open the first one. Anyone wants to add a window to show results
// and allow the selection of the html to show (as BeIDE does)?

//	if (Results.size() < 2)
//	{
		char* arg = (char *)Results[0].String(); 

		// This one has one issue: if you happen to have another Tracker exec
		// liying around on your boot volume... it will get executed instead of
		// just call the already-running-Tracker's ArgRecv.
		be_roster->Launch(kTrackerSig, 1, &arg);
//	}
//	else
//	{

//	}
}

//-----------------------------------------------------------------------------

long perform_edit(MTextAddOn *addon)
{
	long selStart;
	long selEnd;

	addon->GetSelection(&selStart, &selEnd);
	if (selEnd <= selStart)
		return B_ERROR; // should we BAlert the user about the empty selection?

	int length = selEnd - selStart;

	// Crappy code follows...
	BString selection;
	selection.SetTo(addon->Text() + selStart, length);

    // ... to avoid hanging Pe if we are called with long selections.
	int pos = 0;
	while ((pos < length) || pos < 30) // Hope 30 is ok.
	{
		if (!isalnum(selection[pos]) && (selection[pos] != '_')) break;
		pos++;
	}

	if (pos > 0)
		selection.Truncate(pos);
	else
		return B_ERROR;

	vector <BString> results;

	if (FetchQuery(B_EQ, selection.String(), results) > 0)
	{
		DisplayResults(results);
	}
	else
	{
		// Lets try the same selection as a function name ...
		BString funcname(selection.String());
		funcname += "()";

		if (FetchQuery(B_EQ, funcname.String(), results) > 0)
		{
			DisplayResults(results);
		}
		else
		{
			// Still nothing? Lets Slap a B on the front and check for Classes!
			BString section("B");
			section += selection.String();

			if (FetchQuery(B_EQ, section.String(), results, false) > 0)
			{
				DisplayResults(results);
			}			
			else
			{
				if (FetchQuery(B_CONTAINS, selection.String(), results, false) > 0)
				{
					DisplayResults(results);
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