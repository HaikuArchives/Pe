/*
 * HeaderHeader is an Extension.
 */

#include "PeAddOn.h"
#include <ctype.h>
#include <time.h>
#include <MenuItem.h>
#include <PopUpMenu.h>

static const char *sHaikuHeaderTemplate = "/*
 * Copyright %YEAR%, Haiku.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		%AUTHOR% <%AUTHORMAIL%>
 */
";

/*
static const char *sHaikuAddMeHeaderTemplate = \
" *	 	%AUTHOR% <%AUTHORMAIL%>
";
*/


static const char *sHaikuMeHeaderTemplate = "/*
 * Copyright %YEAR%, %AUTHOR%, <%AUTHORMAIL%>.
 * Distributed under the terms of the MIT License.
 *		
 */
";

/*
static const char *sHaikuMeAddMeHeaderTemplate = \
" * Copyright %YEAR%, %AUTHOR%, <%AUTHORMAIL%>.
";
*/



//------------------------------------------------------------------------------
//	#pragma mark - implementation


void
AddTemplateItem(BPopUpMenu *menu, const char *name, const char *tmpl)
{
	BMessage *msg;
	BMenuItem *item;
	msg = new BMessage('head');
	msg->AddString("template", tmpl);
	item = new BMenuItem(name, msg);
	menu->AddItem(item);
}


BPopUpMenu *
BuildPopUp()
{
	BPopUpMenu *menu = new BPopUpMenu("menu", false);
	//BMenuItem *item;
	//BMessage *msg;
	//BPath path;
	//status_t err;

	menu->SetFont(be_plain_font);

	AddTemplateItem(menu, B_UTF8_COPYRIGHT" Haiku", 
		sHaikuHeaderTemplate);

	AddTemplateItem(menu, B_UTF8_COPYRIGHT" Me (Haiku)", 
		sHaikuMeHeaderTemplate);

/*
	// TODO: build list from text files
	err = find_directory(B_COMMON_SETTINGS_DIRECTORY, &path, true);
	if (err < B_OK)
		return p;
	BDirectory
	p->AddItem();

*/

	//p->AddSeparatorItem();
	//p->AddItem(new BMenuItem("Open Template Folder", new BMessage('optf')));
	return menu;
}


status_t
RunPopUpMenu(BPoint where, BString &header, BString &fileName)
{
	status_t err = B_ERROR;
	BPopUpMenu *menu = BuildPopUp();
	if (menu == NULL)
		return B_ERROR;
	
	BMenuItem *item = menu->Go(where, false, true);
	//if (item && item->Message())
	//	item->Message()->PrintToStream();

	switch ((item && item->Message()) ? item->Message()->what : 0) {
		case 'head':
		{
			if (item->Message()->FindString("template", &header) < B_OK)
				break;
			BString tmp;
			time_t now = time(NULL);
			struct tm *tim = localtime(&now);
			// date
			strftime(tmp.LockBuffer(100), 100, "%F", tim);
			tmp.UnlockBuffer();
			header.ReplaceAll("%DATE%", tmp.String());
			tmp.Truncate(0);
			// year
			strftime(tmp.LockBuffer(20), 20, "%Y", tim);
			tmp.UnlockBuffer();
			header.ReplaceAll("%YEAR%", tmp.String());
			tmp.Truncate(0);

			// TODO fetch from query on META:email==**
			// or specific people file
			tmp << "François Revol";
			header.ReplaceAll("%AUTHOR%", tmp.String());
			tmp.Truncate(0);
			tmp << "revol@free.fr";
			header.ReplaceAll("%AUTHORMAIL%", tmp.String());
			tmp.Truncate(0);

			header.ReplaceAll("%FILENAME%", fileName.String());
			/*
			tmp << "Haiku";
			header.ReplaceAll("%PROJECT%", tmp.String());
			tmp.Truncate(0);
			*/

			err = B_OK;
			break;
		}
		case 'optf':
			// TODO
			break;
		default:
			break;
	}
	delete menu;
	return err;
}

//------------------------------------------------------------------------------
//	#pragma mark - 


#if __INTEL__
	extern "C" _EXPORT long perform_edit(MTextAddOn *addon);
#else
#	pragma export on
	extern "C"
		long perform_edit(MTextAddOn *addon);
#	pragma export reset
#endif

long perform_edit(MTextAddOn *addon)
{
	long result = B_OK;
	entry_ref headerFile;
	BPoint where(0,0);

	if (addon->Window() && addon->Window()->Lock()) {
		uint32 buttons;
		addon->Window()->ChildAt(0)->GetMouse(&where, &buttons);
		addon->Window()->ChildAt(0)->ConvertToScreen(&where);
		addon->Window()->Unlock();
		where += BPoint(-3,-3);
	}

	result = addon->GetRef(headerFile);
	BString fileName;
	if (result >= B_OK)
		fileName = headerFile.name;

	BString header;
	result = RunPopUpMenu(where, header, fileName);
	printf("result %s\n", strerror(result));
	if (result < B_OK)
		return result;

#if 0
	// Do not change the case if a shift key was pressed
	if ((modifiers() & B_SHIFT_KEY) == 0)
		fileName.ToUpper();
#endif

	addon->Select(0, 0);

	addon->Insert(header.String());
	
	return result;
}

