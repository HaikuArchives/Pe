/*
 * HeaderHeader is an Extension.
 */

#include "PeAddOn.h"
#include "PDoc.h"
#include "PText.h"
#include "CLanguageInterface.h"
#include <ctype.h>
#include <time.h>
#include <FilePanel.h>
#include <MenuItem.h>
#include <PopUpMenu.h>

static const char *sTrackerSig = "application/x-vnd.Be-TRAK";

static const char *sAboutText = "HeaderHeader extension for Pe\n"
"\n"
"This extension prepends a chooseable licence header the current file, \n"
"using the appropriate comment characters for the language used, \n"
"and informations from a selected People file as author.\n"
"\n"
"First select your own People file from \"Set author"B_UTF8_ELLIPSIS"\", \n"
"then just select the header you want to use.\n"
"You can also add your own templates in the settings folder. \n"
"Use the \"Open template folder"B_UTF8_ELLIPSIS"\" menu, \n"
"create text files there which contain the licence or informations you want. \n"
"\n"
"Keywords from the list below will be replaced by the corresponding value:\n"
"%COMMS%		Start of comment block string\n"
"%COMMC%		Comment block continuation string\n"
"%COMML%		Comment block continuation string (blank if possible)\n"
"%COMME%		End of comment block string\n"
"%FILENAME%		Filename\n"
"%FILENAMENOEXT%	Filename without extension\n"
"%YEAR%		Current year\n"
"%DATE%		Current date (YYYY-mm-dd)\n"
"%TIME%		Current time (HH:MM:SS)\n"
"(from the people file)\n"
"%AUTHOR%		Your name\n"
"%AUTHORMAIL%	Your email\n"
"%COMPANY%		Company name\n"
"%AUTHORURL%		Your url\n"
;

static const char *sHaikuHeaderTemplate = "%COMMS%\n"
"%COMMC% Copyright %YEAR%, Haiku, Inc.\n"
"%COMMC% Distributed under the terms of the MIT License.\n"
"%COMMC%\n"
"%COMMC% Authors:\n"
"%COMMC%		%AUTHOR% <%AUTHORMAIL%>\n"
"%COMME%\n"
;

/*
static const char *sHaikuAddMeHeaderMatch = \
" * Authors:
";
static const char *sHaikuAddMeHeaderTemplate = \
" *	 	%AUTHOR% <%AUTHORMAIL%>
";
*/

static const char *sHaikuMeHeaderTemplate = "%COMMS%\n"
"%COMMC% Copyright %YEAR%, %AUTHOR%, <%AUTHORMAIL%>.\n"
"%COMMC% Distributed under the terms of the MIT License.\n"
"%COMME%\n"
;

/*
static const char *sHaikuMeAddMeHeaderTemplate = \
" * Copyright %YEAR%, %AUTHOR%, <%AUTHORMAIL%>.
";
*/

static const char *sHaikuMeRightsHeaderTemplate = "%COMMS%\n"
"%COMMC% Copyright %YEAR%, %AUTHOR%, <%AUTHORMAIL%>. All rights reserved.\n"
"%COMMC% Distributed under the terms of the MIT License.\n"
"%COMME%\n"
;

static const char *sIdMeMITCreatedHeaderTemplate =
"%COMMS%	$Id: %FILENAME% $\n"
"%COMML%\n"
"%COMML%	Copyright %YEAR% %AUTHOR%\n"
"%COMML%\n"
"%COMML%	Distributed under the MIT License\n"
"%COMML%\n"
"%COMML%	Created: %DATE%\n"
"%COMME%\n"
;

static const char *sIdMeFullMITCreatedHeaderTemplate =
"%COMMS%	$Id: %FILENAME% $\n"
"%COMML%	\n"
"%COMML%	Copyright %YEAR%\n"
"%COMML%	        %AUTHOR%  All rights reserved.\n"
"%COMML%	\n"
"%COMML%	Redistribution and use in source and binary forms, with or without\n"
"%COMML%	modification, are permitted provided that the following conditions are met:\n"
"%COMML%	1. Redistributions of source code must retain the above copyright notice,\n"
"%COMML%	   this list of conditions and the following disclaimer.\n"
"%COMML%	2. Redistributions in binary form must reproduce the above copyright notice,\n"
"%COMML%	   this list of conditions and the following disclaimer in the documentation\n"
"%COMML%	   and/or other materials provided with the distribution.\n"
"%COMML%	3. All advertising materials mentioning features or use of this software\n"
"%COMML%	   must display the following acknowledgement:\n"
"%COMML%	   \n"
"%COMML%	    This product includes software developed by %AUTHOR%.\n"
"%COMML%	\n"
"%COMML%	4. The name of %AUTHOR% may not be used to endorse or\n"
"%COMML%	   promote products derived from this software without specific prior\n"
"%COMML%	   written permission.\n"
"%COMML%	\n"
"%COMML%	THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,\n"
"%COMML%	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND\n"
"%COMML%	FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL\n"
"%COMML%	AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,\n"
"%COMML%	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,\n"
"%COMML%	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;\n"
"%COMML%	OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,\n"
"%COMML%	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR\n"
"%COMML%	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF\n"
"%COMML%	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 	\n"
"%COMML%\n"
"%COMML%	Created: %DATE% %TIME%\n"
"%COMME%\n"
;

static const char *sIdMeFullMITRevisedCreatedHeaderTemplate =
"%COMMS%	$Id: %FILENAME% $\n"
"%COMML%	\n"
"%COMML%	Copyright %YEAR%\n"
"%COMML%	        %AUTHOR%  All rights reserved.\n"
"%COMML%	\n"
"%COMML%	Redistribution and use in source and binary forms, with or without\n"
"%COMML%	modification, are permitted provided that the following conditions are met:\n"
"%COMML%	1. Redistributions of source code must retain the above copyright notice,\n"
"%COMML%	   this list of conditions and the following disclaimer.\n"
"%COMML%	2. Redistributions in binary form must reproduce the above copyright notice,\n"
"%COMML%	   this list of conditions and the following disclaimer in the documentation\n"
"%COMML%	   and/or other materials provided with the distribution.\n"
"%COMML%	4. The name of %AUTHOR% may not be used to endorse or\n"
"%COMML%	   promote products derived from this software without specific prior\n"
"%COMML%	   written permission.\n"
"%COMML%	\n"
"%COMML%	THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,\n"
"%COMML%	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND\n"
"%COMML%	FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL\n"
"%COMML%	AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,\n"
"%COMML%	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,\n"
"%COMML%	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;\n"
"%COMML%	OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,\n"
"%COMML%	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR\n"
"%COMML%	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF\n"
"%COMML%	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 	\n"
"%COMML%\n"
"%COMML%	Created: %DATE% %TIME%\n"
"%COMME%\n"
;


//------------------------------------------------------------------------------
//	#pragma mark - class PanelHandler

class PanelHandler : public BHandler {
	public:
		PanelHandler();
		~PanelHandler();
	virtual void	MessageReceived(BMessage *message);

	status_t	Wait();
	BMessage*	Message() const { return fMessage; };
	private:
		sem_id fSem;
		BMessage *fMessage;
};


PanelHandler::PanelHandler()
{
	fSem = create_sem(0, "PanelHandlerTrigger");
	fMessage = NULL;
}


PanelHandler::~PanelHandler()
{
	delete_sem(fSem);
	delete fMessage;
}


void
PanelHandler::MessageReceived(BMessage *message)
{
	delete fMessage;
	fMessage = new BMessage(*message);
	release_sem(fSem);
}


status_t
PanelHandler::Wait()
{
	return acquire_sem(fSem);
}


//------------------------------------------------------------------------------
//	#pragma mark - class MimeRefFilter

class MimeRefFilter : public BRefFilter {
	public:
			MimeRefFilter(const char *mime);
			MimeRefFilter(const char *mimes[]);
			//MimeRefFilter(BList *mimes);
	virtual	~MimeRefFilter();

#ifdef __HAIKU__
	virtual	bool	Filter(const entry_ref *ref, BNode *node, 
							struct stat_beos *st, const char *filetype);
#else
	virtual	bool	Filter(const entry_ref *ref, BNode *node, 
							struct stat *st, const char *filetype);
#endif

	private:
		BList	*fMimes;
};

MimeRefFilter::MimeRefFilter(const char *mime)
{
	fMimes = new BList;
	fMimes->AddItem(new BString(mime));
}


MimeRefFilter::MimeRefFilter(const char *mimes[])
{
	int i;
	fMimes = new BList;
	for (i = 0; mimes[i]; i++)
	{
		fMimes->AddItem(new BString(mimes[i]));
	}
}


MimeRefFilter::~MimeRefFilter()
{
	int i;
	for (i = fMimes->CountItems() - 1; i > -1; i--)
	{
		delete (BString *)fMimes->ItemAt(i);
	}
	delete fMimes;
}


bool
#ifdef __HAIKU__
MimeRefFilter::Filter(const entry_ref *ref, BNode *node, 
					struct stat_beos *st, const char *filetype)
#else
MimeRefFilter::Filter(const entry_ref *ref, BNode *node, 
					struct stat *st, const char *filetype)
#endif
{
	int i;
	// allow folders else it's quite hard to navigate :)
	if (node->IsDirectory())
		return true;
	//BNode target;
	if (node->IsSymLink())
	{
		// init target 
		// node = &target;
		// if
		return true;
	}
	
	for (i = fMimes->CountItems() - 1; i > -1; i--)
	{
		BString *mime = (BString *)fMimes->ItemAt(i);
		if (filetype == *mime)
			return true;
	}
	return false;
}


//------------------------------------------------------------------------------
//	#pragma mark - implementation


status_t
GetSettingsDir(BDirectory &dir, BPath &path)
{
	//BPath path;
	status_t err;
	// TODO: build list from text files
	err = find_directory(B_USER_SETTINGS_DIRECTORY, &path, true);
	if (err < B_OK)
		return err;
	dir.SetTo(path.Path());
	if (!dir.Contains("pe"))
		dir.CreateDirectory("pe", NULL);
	path.Append("pe");
	dir.SetTo(path.Path());
	if (!dir.Contains("HeaderTemplates"))
		dir.CreateDirectory("HeaderTemplates", NULL);
	path.Append("HeaderTemplates");
	dir.SetTo(path.Path());
	return B_OK;
}

void
AddTemplateItem(BPopUpMenu *menu, 
	const char *name, 
	const char *tmpl, 
	const char *match=NULL)
{
	BMessage *msg;
	BMenuItem *item;
	msg = new BMessage('head');
	msg->AddString("template", tmpl);
	if (match)
		msg->AddString("match", match);
	item = new BMenuItem(name, msg);
	menu->AddItem(item);
}


BPopUpMenu *
BuildPopUp(BDirectory &dir)
{
	BPopUpMenu *menu = new BPopUpMenu("menu", false);
	//BMenuItem *item;
	//BMessage *msg;
	//status_t err;

	//menu->SetFont(be_plain_font);

	AddTemplateItem(menu, B_UTF8_COPYRIGHT " Haiku", 
		sHaikuHeaderTemplate);

/*
	AddTemplateItem(menu, B_UTF8_COPYRIGHT " Haiku (Add me)", 
		sHaikuAddMeHeaderTemplate, sHaikuAddMeHeaderMatch);
*/

	AddTemplateItem(menu, B_UTF8_COPYRIGHT " Me (Haiku)", 
		sHaikuMeHeaderTemplate);

	AddTemplateItem(menu, B_UTF8_COPYRIGHT " Me (Haiku) + Rights Reserved", 
		sHaikuMeRightsHeaderTemplate);

	AddTemplateItem(menu, "Id + " B_UTF8_COPYRIGHT "Me + MIT + Created", 
		sIdMeMITCreatedHeaderTemplate);

	AddTemplateItem(menu, "Id + " B_UTF8_COPYRIGHT "Me + Full MIT + Created", 
		sIdMeFullMITCreatedHeaderTemplate);

	AddTemplateItem(menu, "Id + " B_UTF8_COPYRIGHT "Me + Full MIT Revised + Created", 
		sIdMeFullMITRevisedCreatedHeaderTemplate);

	if (dir.InitCheck() < B_OK)
		return menu;

	entry_ref ref;
	while (dir.GetNextRef(&ref) == B_OK)
	{
		BFile file(&ref, B_READ_ONLY);
		if (file.InitCheck() < B_OK)
			continue;
		BString str;
		char *p = str.LockBuffer(1024);
		memset(p, 0, 1024);
		if (file.Read(p, 1024) <= 0)
			continue;
		str.UnlockBuffer();
		
		AddTemplateItem(menu, ref.name, str.String());
	}

	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("Open template folder"B_UTF8_ELLIPSIS, 
		new BMessage('optf')));
	menu->AddItem(new BMenuItem("Set author"B_UTF8_ELLIPSIS, 
		new BMessage('seta')));
	menu->AddItem(new BMenuItem("About"B_UTF8_ELLIPSIS, 
		new BMessage(B_ABOUT_REQUESTED)));
	return menu;
}


status_t
RunPopUpMenu(BPoint where, BString &header, BString &fileName, 
	CLanguageInterface *languageInterface)
{
	status_t err;
	BPath path;
	BDirectory dir;
	err = GetSettingsDir(dir, path);
	err = B_ERROR;
	BPopUpMenu *menu = BuildPopUp(dir);
	if (menu == NULL)
		return B_ERROR;
	
	BMenuItem *item = menu->Go(where, false, true);
	//if (item && item->Message())
	//	item->Message()->PrintToStream();

	switch ((item && item->Message()) ? item->Message()->what : 0)
	{
		case 'head':
		{
			if (item->Message()->FindString("template", &header) < B_OK)
				break;
			BString tmp;
			time_t now = time(NULL);
			struct tm *tim = localtime(&now);
			// date
			char *p;
			p = tmp.LockBuffer(100);
			memset(p, 0, 100);
			strftime(p, 100, "%Y-%m-%d", tim);
			tmp.UnlockBuffer();
			header.ReplaceAll("%DATE%", tmp.String());
			tmp.Truncate(0);
			
			p = tmp.LockBuffer(100);
			memset(p, 0, 100);
			strftime(p, 100, "%T", tim);
			tmp.UnlockBuffer();
			header.ReplaceAll("%TIME%", tmp.String());
			tmp.Truncate(0);

			// year
			p = tmp.LockBuffer(10);
			memset(p, 0, 10);
			strftime(p, 10, "%Y", tim);
			tmp.UnlockBuffer();
			header.ReplaceAll("%YEAR%", tmp.String());
			tmp.Truncate(0);

			// fetch from query on META:email==** ?
			p = tmp.LockBuffer(B_PATH_NAME_LENGTH);
			memset(p, 0, B_PATH_NAME_LENGTH);
			err = dir.ReadAttr("pe:author_people", B_STRING_TYPE, 0LL, p, 
				B_PATH_NAME_LENGTH);
			tmp.UnlockBuffer();
			//printf("ppl:%s\n", tmp.String());
			BNode people;
			if (err > 0)
				people.SetTo(tmp.String());
			tmp.Truncate(0);
			
			BString attr;

			static struct {
				const char *tmplName;
				const char *attrName;
			} attrMap[] = {
				{ "%AUTHOR%", "META:name" },
				{ "%AUTHORMAIL%", "META:email" },
				{ "%COMPANY%", "META:company" },
				{ "%AUTHORURL%", "META:url" },
				{ NULL, NULL }
			};
			int i;

			for (i = 0; attrMap[i].tmplName; i++)
			{
				p = attr.LockBuffer(256);
				memset(p, 0, 256);
				err = people.ReadAttr(attrMap[i].attrName, B_ANY_TYPE, 
					0LL, p, 256);
				//printf("ReadAttr: %d, %s\n", err, attr.String());
				attr.UnlockBuffer();

				tmp << attr;
				header.ReplaceAll(attrMap[i].tmplName, tmp.String());
				tmp.Truncate(0);
				attr.Truncate(0);
			}

			BString fileNameNoExt(fileName);
			if (fileNameNoExt.FindLast('.') > -1)
				fileNameNoExt.Truncate(fileNameNoExt.FindLast('.'));
			header.ReplaceAll("%FILENAMENOEXT%", fileNameNoExt.String());
			header.ReplaceAll("%FILENAME%", fileName.String());
			/*
			tmp << "Haiku";
			header.ReplaceAll("%PROJECT%", tmp.String());
			tmp.Truncate(0);
			*/

			// better values for C++
			BString language("C/C++");
			BString commentLineStart("/*");
			BString commentLineEnd("");
			BString commentBlockStart("/*");
			BString commentBlockCont(" *");
			BString commentBlockLazy("");
			BString commentBlockLineEnd("");
			BString commentBlockEnd(" */");
			if (languageInterface)
			{
				// if not C++
				if (language != languageInterface->Name())
				{
					language = languageInterface->Name();
					commentLineStart = languageInterface->LineCommentStart();
					commentLineEnd = languageInterface->LineCommentEnd();
					// I'd miss a CommentCanSpanLines()
					// let's assume line end means can span
					if (commentLineEnd.Length())
					{
						commentBlockStart = commentLineStart;
						commentBlockCont = "";
						commentBlockLazy = "";
						commentBlockLineEnd = "";
						commentBlockEnd = commentLineEnd;
					}
					else
					{
						commentBlockStart = commentLineStart;
						commentBlockCont = commentLineStart;
						commentBlockLazy = commentLineStart;
						commentBlockLineEnd = commentLineEnd;
						commentBlockEnd = commentLineStart;
					}
					/*
					printf("LANG:'%s' CS:'%s' CE:'%s'\n", 
						language.String(), 
						commentLineStart.String(), 
						commentLineEnd.String());
					*/
				}
			}
			// comment start
			header.ReplaceAll("%COMMS%", commentBlockStart.String());
			// comment cont'd
			header.ReplaceAll("%COMMC%", commentBlockCont.String());
			// comment cont'd lazy (blank if possible)
			header.ReplaceAll("%COMML%", commentBlockLazy.String());
			// comment end
			header.ReplaceAll("%COMME%", commentBlockEnd.String());
			// comment line end
			commentBlockLineEnd << "\n";
			header.ReplaceAll("\n", commentBlockLineEnd.String());


			err = B_OK;
			break;
		}
		case 'optf':
		{
			const char *args[] = {path.Path(), NULL};
			err = be_roster->Launch(sTrackerSig, 1, (char **)args);
			//printf("err %s\n", strerror(err));
			err = B_CANCELED;
			break;
		}
		case 'seta':
		{
			MimeRefFilter filter("application/x-person");
			BPath path;
			entry_ref people;

			if (find_directory(B_USER_DIRECTORY, &path) == B_OK)
			{
				path.Append("people");
				get_ref_for_path(path.Path(), &people);
			}

			BFilePanel *panel;
			panel = new BFilePanel(B_OPEN_PANEL, NULL, &people,
				B_FILE_NODE, false, NULL, &filter);
			// trick to synchronously use BFilePanel
			PanelHandler *handler = new PanelHandler;
			if (panel->Window()->Lock())
			{
				panel->Window()->AddHandler(handler);
				panel->Window()->Unlock();
			}
			panel->SetTarget(BMessenger(handler));
			panel->Show();
			if (handler->Wait() < B_OK)
				break;
			if (!handler->Message())
				break;
			if (handler->Message()->what == B_CANCEL)
				break;
			entry_ref ref;
			//panel->Message()->PrintToStream();
			if (panel->GetNextSelectedRef(&ref) == B_OK)
			{
				//printf("ref:%s\n", ref.name);
				path.SetTo(&ref);
				dir.WriteAttr("pe:author_people", B_STRING_TYPE, 0LL, 
					path.Path(), strlen(path.Path()));
			}
			delete panel;
			delete handler;
			err = B_CANCELED;
			break;
		}
		case B_ABOUT_REQUESTED:
		{
			BString tmpPath("/tmp/Pe-HeaderHeader-About-");
			tmpPath << system_time() << "-" << getpid() << ".txt";
			entry_ref ref;
			get_ref_for_path(tmpPath.String(), &ref);
			{
				BFile f(&ref, B_CREATE_FILE | B_WRITE_ONLY);
				err = f.InitCheck();
				if (err < 0)
					break;
				f.Write(sAboutText, strlen(sAboutText));
				f.SetPermissions(0444);
			}
			BMessage msg(B_REFS_RECEIVED);
			msg.AddRef("refs", &ref);
			err = be_app_messenger.SendMessage(&msg);
			err = B_CANCELED;
			break;
		}
		case 0:
			err = B_CANCELED;
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
	extern "C" _EXPORT status_t perform_edit(MTextAddOn *addon);
#else
#	pragma export on
	extern "C"
		status_t perform_edit(MTextAddOn *addon);
#	pragma export reset
#endif

status_t perform_edit(MTextAddOn *addon)
{
	status_t result = B_OK;
	entry_ref headerFile;
	BPoint where(0,0);

	if (addon->Window() && addon->Window()->Lock())
	{
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

	CLanguageInterface *languageInterface = NULL;
	if (addon->Window())
	{
		PDoc *doc = dynamic_cast<PDoc *>(addon->Window());
		if (doc && doc->TextView())
		{
			int lang = doc->TextView()->Language();
			if (lang > -1)
				languageInterface = CLanguageInterface::FindIntf(lang);
		}
	}

	BString header;
	result = RunPopUpMenu(where, header, fileName, languageInterface);
	//printf("result %s\n", strerror(result));
	if (result == B_CANCELED)
		return B_OK;
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

