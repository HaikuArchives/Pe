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

	Created: 10/21/97 09:19:05
*/

#include "pe.h"

static char *rcsid = "$Id$";

#include "PApp.h"
#include "PDoc.h"
#include "PMessages.h"
#include "CFindDialog.h"
#include "CPrefsDialog.h"
#include "Utils.h"
#include "PGroupWindow.h"
#include "CGlossary.h"
//#include "CKeywords.h"
#include "CLangIntf.h"
#include "HColorUtils.h"
#include "PGlossyWindow.h"
#include "utf-support.h"
#include "CHTMLBar.h"
#include "PIDECompat.h"
#include "PText.h"
#include "COpenSelection.h"
#include "CFtpDialog.h"
#include "PAbout.h"
#include "CUrlOpener.h"
#include "CDiffWindow.h"
#include "PProjectWindow.h"
#include <be_apps/NetPositive/NetPositive.h>
#include "HPreferences.h"

BDirectory gAppDir, gCWD, gPrefsDir;
BFile gAppFile;
PApp *gApp;
rgb_color gColor[kLastColor], gInvColor[kLastColor];
bool gAutoIndent, gSyntaxColoring, gBalance, gBlockCursor, gFlashCursor, gSmartBrace;
bool gPopupIncludes, gPopupProtos, gPopupFuncs;
int gSpacesPerTab;
bool gUseWorksheet;
bool gRestorePosition, gRestoreFont, gRestoreSelection, gRestoreScrollbar, gRestoreCWD;
int gSavedState, gRecentBufferSize;
uid_t gUid;
gid_t gGid;
char gTabChar[4], gReturnChar[4], gSpaceChar[4], gControlChar[4];

static void SaveRecentMenu()
{
	int i = 0;
	char path[PATH_MAX];
	
	while (PDoc::GetNextRecent(path, i))
		gPrefs->SetIxPrefString("recentdoc", i, path);
} /* SaveRecentMenu */

static void RestoreRecentMenu()
{
	char name[12];
	int i = gRecentBufferSize;
	const char *path;
	
	while (i > 0)
	{
		try
		{
			sprintf(name, "recentdoc%d", i--);
			path = gPrefs->GetPrefString(name);
			if (path)
				PDoc::AddRecent(path);
		}
		catch(...){}
	}
} /* RestoreRecentMenu */

class CPrefOpener : public MThread {
public:
virtual	long Execute();
};

long CPrefOpener::Execute()
{
	try
	{
		CPrefsDialog *prefsDialog = DialogCreator<CPrefsDialog>::CreateDialog(NULL);
		prefsDialog->Run();
		
		PApp *app = dynamic_cast<PApp*>(be_app);
		if (app)
			app->fPrefsDialog = prefsDialog;
	}
	catch (HErr& e)
	{
		e.DoError();
	}
	catch (...)
	{
//		puts("general exception");
	}
	
	return 0;
} /* CPrefOpened::Execute */

PApp::PApp()
	: BApplication("application/x-vnd.beunited.pe")
{
#if BETA
	MInfoAlert a("This is a beta release of Pe.\n");
	a.Go();
#endif

	try
	{
		try
		{
			BPath settings;
			
			FailOSErr(find_directory(B_USER_SETTINGS_DIRECTORY, &settings, true));
			
			BDirectory e;
			FailOSErrMsg(e.SetTo(settings.Path()), "~/config/settings directory not found ?!?!?");
			if (!e.Contains("pe", B_DIRECTORY_NODE))
				FailOSErr(e.CreateDirectory("pe", &gPrefsDir));
			else
			{
				BEntry d;
				FailOSErr(e.FindEntry("pe", &d, B_DIRECTORY_NODE));
				FailOSErr(gPrefsDir.SetTo(&d));
			}
		}
		catch (HErr& e)
		{
			e.DoError();
		}
			
		gPrefs = new HPreferences("pe/settings");
		gPrefs->ReadPrefFile();
		
		app_info ai;
		GetAppInfo(&ai);
		
		BEntry entry(&ai.ref);
		
		FailOSErr(gAppFile.SetTo(&entry, B_READ_ONLY));
		
		BPath appName;
		FailOSErr(entry.GetPath(&appName));
		
		BPath dir;
		FailOSErr(appName.GetParent(&dir));
		FailOSErr(gAppDir.SetTo(dir.Path()));
		FailOSErr(gCWD.SetTo(dir.Path()));
	
		fOpenPanel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this), NULL,
				0, true, new BMessage(B_REFS_RECEIVED));
		FailNil(fOpenPanel);
		
		PDoc::LoadAddOns();

		InitUTFTables();

		rgb_color c = { 0xFF, 0xFF, 0xFF, 0 };			gColor[kLowColor] = gPrefs->GetPrefColor("low color", c);
		c.red = 0x00; c.green = 0x00; c.blue = 0x00;	gColor[kTextColor] = gPrefs->GetPrefColor("text color", c);
		c.red = 0x39; c.green = 0x74; c.blue = 0x79;	gColor[kKeyWordColor] = gPrefs->GetPrefColor("keyword color", c);
		c.red = 0xA1; c.green = 0x64; c.blue = 0x0E;	gColor[kCommentColor] = gPrefs->GetPrefColor("comment color", c);
		c.red = 0x3f; c.green = 0x48; c.blue = 0x84;	gColor[kStringColor] = gPrefs->GetPrefColor("string color", c);
		c.red = 0x85; c.green = 0x19; c.blue = 0x19;	gColor[kCharConstColor] = gPrefs->GetPrefColor("char constant color", c);
		c.red = 0x44; c.green = 0x8a; c.blue = 0x00;	gColor[kUser1] = gPrefs->GetPrefColor("user1", c);
														gColor[kUser2] = gPrefs->GetPrefColor("user2", c);
														gColor[kUser3] = gPrefs->GetPrefColor("user3", c);
														gColor[kUser4] = gPrefs->GetPrefColor("user4", c);
		c.red = 0x88; c.green = 0x88; c.blue = 0x88;	gColor[kTagColor] = gPrefs->GetPrefColor("tag color", c);
//		c.red = 0x40; c.green = 0x75; c.blue = 0x75;	gColor[kImageColor] = gPrefs->GetPrefColor("image color", c);
//		c.red = 0x80; c.green = 0x20; c.blue = 0x20;	gColor[kAnchorColor] = gPrefs->GetPrefColor("anchor color", c);
		c.red = 0x3f; c.green = 0x48; c.blue = 0x84;	gColor[kTagStringColor] = gPrefs->GetPrefColor("tagstring color", c);

		c.red = 0xff; c.green = 0xec; c.blue = 0x7c;	gColor[kSelectionColor] = gPrefs->GetPrefColor("selection color", c);
		c.red = 200; c.green = 100; c.blue = 100;		gColor[kInvisiblesColor] = gPrefs->GetPrefColor("invisibles color", c);
		c.red = 0; c.green = 0; c.blue = 255;				gColor[kMarkColor] = gPrefs->GetPrefColor("mark color", c);
		
		DefineInvColors(gColor[kSelectionColor]);

		gAutoIndent = gPrefs->GetPrefInt("auto indent", 1);
		gSyntaxColoring = gPrefs->GetPrefInt("syntax coloring", 1);
		gSpacesPerTab = gPrefs->GetPrefInt("spaces per tab", 4);
		gBalance = gPrefs->GetPrefInt("balance", 1);
		gBlockCursor = gPrefs->GetPrefInt("block cursor", 0);
		gFlashCursor = gPrefs->GetPrefInt("flash cursor", 1);
		gSmartBrace = gPrefs->GetPrefInt("smart braces", 1);
		
		gPopupIncludes = gPrefs->GetPrefInt("includes", 1);
		gPopupProtos = gPrefs->GetPrefInt("protos", 1);
		gPopupFuncs = gPrefs->GetPrefInt("types", 1);
		
		gRedirectStdErr = gPrefs->GetPrefInt("redirect stderr", 1);
		gUseWorksheet = gPrefs->GetPrefInt("worksheet", 1);

		gRestorePosition = gPrefs->GetPrefInt("restore position", 1);
		gRestoreFont = gPrefs->GetPrefInt("restore font", 1);
		gRestoreSelection = gPrefs->GetPrefInt("restore selection", 1);
		gRestoreScrollbar = gPrefs->GetPrefInt("restore scrollbar", 1);
		gRestoreCWD = gPrefs->GetPrefInt("restore cwd", 1);
		gSavedState = gPrefs->GetPrefInt("saved state", 0);
		
		if (gPrefs->GetIxPrefString("mimetype", 0) == NULL)
			gPrefs->SetIxPrefString("mimetype", 0, "text/plain");
		
		strcpy(gTabChar, gPrefs->GetPrefString("tab char", "»"));
		strcpy(gReturnChar, gPrefs->GetPrefString("return char", "¬"));
		strcpy(gSpaceChar, gPrefs->GetPrefString("space char", "."));
		strcpy(gControlChar, gPrefs->GetPrefString("control char", "¿"));
		
		gUid = getuid();
		gGid = getgid();

		gRecentBufferSize = gPrefs->GetPrefInt("recent size", 10);
		RestoreRecentMenu();

		gRxInstalled = true;
		fFindDialog = DialogCreator<CFindDialog>::CreateDialog(NULL);
		fFindDialog->Run();
		
		InitSelectedMap();

		CLangIntf::SetupLanguageInterfaces();
	
		fPrefsDialog = NULL;
		CPrefOpener *prefOpener = new CPrefOpener;
		fPrefOpener = prefOpener->Thread();
		
		if (gPrefs->GetPrefInt("show htmlpalette", 1))
			CHTMLBar::Instance()->Show();
		
		try
		{
			gGlossary = new CGlossary;
		}
		catch (HErr& e)
		{
			e.DoError();
		}

		if (gUseWorksheet)
			OpenWorksheet();

//		prefOpener->Run();
	}
	catch (HErr& e)
	{
		e.DoError();
		exit(1);
	}
	catch(...)
	{
		beep();
		exit(1);
	}
} /* PApp::PApp */

PApp::~PApp()
{
	if (fOpenPanel)
	{
		delete fOpenPanel;
		fOpenPanel = NULL;
	}
} /* PApp::~PApp */

bool PApp::QuitRequested()
{
	BWindow *doc;
	
	while ((doc = dynamic_cast<BWindow*>(CDoc::FirstDoc())) != NULL)
	{
		doc->Lock();
		if (doc->QuitRequested())
			doc->Quit();
		else
		{
			doc->Unlock();
			return false;
		}
	}

	CHTMLBar::Close();
	
	if (fFindDialog)
	{
		fFindDialog->Lock();
		(void)fFindDialog->QuitRequested();
		fFindDialog->Quit();
	}
	
	long l;
	if (fPrefOpener) wait_for_thread(fPrefOpener, &l);
	
	if (fPrefsDialog)
	{
		fPrefsDialog->Lock();
		fPrefsDialog->Quit();
	}
	
	return true;
} /* PApp::QuitRequested */

void PApp::DispatchMessage(BMessage *msg, BHandler *handler)
{
       if ( msg->what == B_ARGV_RECEIVED ) {
               int32 argc;
               if (msg->FindInt32("argc",&argc) != B_OK) {
                       argc=0;
               }
               const char ** argv = new (const char*)[argc];
               for (int arg = 0; (arg < argc) ; arg++) {
                       if (msg->FindString("argv",arg,&argv[arg]) != B_OK) {
                               argv[arg] = "";
                       }
               }
               const char * cwd;
               if (msg->FindString("cwd",&cwd) != B_OK) {
                       cwd = "";
               }
               ArgvReceived(argc, argv, cwd);
       } else {
               BApplication::DispatchMessage(msg,handler);
       }
} /* PApp::DispatchMessage */

void PApp::ReadyToRun()
{
	PDoc *doc = PDoc::TopWindow();
	if (!doc || doc->IsWorksheet())
	{
		switch (gPrefs->GetPrefInt("startup"))
		{
			case 1:
				if (CDoc::CountDocs() == 0 || (doc && doc->IsWorksheet() && CDoc::CountDocs() == 1))
					NewWindow();
				break;

			case 2:
				PostMessage(msg_Open);
				break;

			default:
				if (CDoc::CountDocs() == 0)
					NewWindow();
				break;
		}
	}
	
	BPath p;
	if (find_directory(B_USER_DIRECTORY, &p) == B_OK)
		gCWD.SetTo(p.Path());
	
	if (gPrefs->GetPrefInt("showglossary", 0))
		PostMessage(msg_ShowGlossary);
} /* PApp::ReadyToRun */

void PApp::NewWindow()
{
	try
	{
		new PDoc;
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* PApp::NewWindow */

CDoc* PApp::OpenWindow(const entry_ref& doc, bool show)
{
	try
	{
		BNode n(&doc);
		char mime[256];
		
		if (BNodeInfo(&n).GetType(mime))
			mime[0] = 0;

		if (strcmp(mime, "text/x-vnd.Hekkel-Pe-Group") == 0 ||
			strcmp(mime, "text/x-pe-group") == 0)
		{
			PGroupWindow *w = dynamic_cast<PGroupWindow*>(CDoc::FindDoc(doc));
			if (w)
			{
				if (gPrefs->GetPrefInt("window to workspace", 1))
					w->SetWorkspaces(1 << current_workspace());
				if (show)
					w->Activate(true);
				return CDoc::FindDoc(doc);
			}
			else
				return new PGroupWindow(&doc);
		}
		else if (strcmp(mime, "text/x-makefile") == 0)
		{
			PProjectWindow *w = dynamic_cast<PProjectWindow*>(CDoc::FindDoc(doc));
			if (w)
			{
				if (gPrefs->GetPrefInt("window to workspace", 1))
					w->SetWorkspaces(1 << current_workspace());
				if (show)
					w->Activate(true);
				return CDoc::FindDoc(doc);
			}
			else
				return new PProjectWindow(&doc);
		}
		else
		{
			PDoc *d = dynamic_cast<PDoc*>(CDoc::FindDoc(doc));
			if (d)
			{
				if (gPrefs->GetPrefInt("window to workspace", 1))
					d->SetWorkspaces(1 << current_workspace());
				if (show)
					d->Activate(true);
				return CDoc::FindDoc(doc);
			}
			else
				return new PDoc(&doc, show);
		}
	}
	catch (HErr& e)
	{
		e.DoError();
	}
	
	return NULL;
} /* PApp::OpenWindow */

PDoc* PApp::OpenWorksheet()
{
	try
	{
		if (!gPrefsDir.Contains("Worksheet", B_FILE_NODE | B_SYMLINK_NODE))
		{
			BFile file;
			gPrefsDir.CreateFile("Worksheet", &file);
		}
		
		BEntry w;
		entry_ref wr;

		FailOSErr(gPrefsDir.FindEntry("Worksheet", &w, true));
		FailOSErr(w.GetRef(&wr));
		OpenWindow(wr);
		
		PDoc *d = dynamic_cast<PDoc*>(CDoc::FindDoc(wr));
		if (d)
			d->MakeWorksheet();
		
		return d;
	}
	catch (HErr& e)
	{
		e.DoError();
	}
	
	return NULL;
} /* PApp::OpenWorksheet */

static void Usage()
{
	fprintf(stderr, "Usage: pe [\"+\"linenr] file1 file2 ...\n");
} /* Usage */

void PApp::ArgvReceived(int32 argc, const char *argv[], const char * cwd)
{
	try
	{
		int i = 1, lineNr = -1;
		char *p;
		
		while (i < argc)
		{
			switch (argv[i][0])
			{
				case '-':
					if (strcmp(argv[i], "-reload_worksheet") == 0)
					{
						PDoc *d = OpenWorksheet();
						if (d && d->Lock())
							d->Quit();
						d = OpenWorksheet();
					} else {
						Usage();
					}
					break;
				
				case '+':
					lineNr = strtoul(argv[i] + 1, &p, 10) - 1;
					if (!p || p == argv[i] + 1) Usage();
					break;
				
				default:
				{
					BPath path;
					if (argv[i][0] == '/') {
						path.SetTo(argv[i]);
					} else {
						path.SetTo(cwd,argv[i]);
					}
					FailOSErr (path.InitCheck());
					entry_ref doc;
					PDoc *d;
	
					FailOSErr (get_ref_for_path(path.Path(), &doc));
					
					BEntry e;
					FailOSErr(e.SetTo(&doc));
					
					if (e.Exists())
						d = dynamic_cast<PDoc*>(OpenWindow(doc));
					else
					{
						d = new PDoc;
						BAutolock lock(d);
						
						if (lock.IsLocked()) 
						{
							BPath p;
							if (BEntry(&doc).GetPath(&p) == B_OK)
								d->SetTitle(p.Path());
							else
								d->SetTitle(doc.name);
						}
					}
					
					if (d && lineNr >= 0)
					{
						BMessage m(msg_SelectLines);
						m.AddInt32("from", lineNr);
						m.AddInt32("to", lineNr + 1);
						d->PostMessage(&m, d->PreferredHandler());
					}
					break;
				}
			}
			i++;
		}
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* PApp::ArgvReceived */

void PApp::RefsReceived(BMessage	*inMessage)
{
	try
	{
		unsigned long type;
		long count;
	
		inMessage->GetInfo("refs", &type, &count);
	
		for (int i = 0; i < count; i++)
		{
			entry_ref doc;
			FailOSErr (inMessage->FindRef("refs", i, &doc));
			PDoc *d = dynamic_cast<PDoc*>(OpenWindow(doc));
			if (!d) continue;

			if (inMessage->HasData(kTokenIdentifier, kTokenIDType))
			{
				TokenIdentifier *ident;
				ssize_t size;
				
				FailOSErr(inMessage->FindData(kTokenIdentifier, kTokenIDType, (const void**)&ident, &size));
				
				BMessage msg(msg_SelectError);
				FailOSErr(msg.AddInt32("start", ident->eOffset));
				FailOSErr(msg.AddInt32("length", ident->eLength));
				
				BMessenger msgr(d->TextView());
				FailOSErr(msgr.SendMessage(&msg));
			}
			else if (inMessage->HasInt32("be:selection_offset"))
			{
				int32 offset, length;

				FailOSErr(inMessage->FindInt32("be:selection_offset", &offset));
				FailOSErr(inMessage->FindInt32("be:selection_length", &length));
				
				BMessage msg(msg_Select);
				FailOSErr(msg.AddInt32("anchor", offset));
				FailOSErr(msg.AddInt32("caret", offset + length));
				
				BMessenger msgr(d->TextView());
				FailOSErr(msgr.SendMessage(&msg));
			}
			else if (inMessage->HasInt32("be:line"))
			{
				int32 line;

				FailOSErr(inMessage->FindInt32("be:line", &line));
				
				BMessage msg(msg_SelectLines);
				FailOSErr(msg.AddInt32("from", line));
				FailOSErr(msg.AddInt32("to", line));
				
				BMessenger msgr(d->TextView());
				FailOSErr(msgr.SendMessage(&msg));
			}
		}
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* PApp::RefsReceived */

void PApp::MessageReceived(BMessage *msg)
{
	try
	{
		switch (msg->what)
		{
			case msg_SaveAll:
			{
				const doclist& lst = CDoc::DocList();
				doclist::const_iterator di;
				
				for (di = lst.begin(); di != lst.end(); di++)
				{
					BWindow *w = dynamic_cast<PDoc*>(*di);
					if (w)
						w->PostMessage(msg_Save);
				}
				break;
			}
			
			case msg_CloseAll:
			{
				const doclist& lst = CDoc::DocList();
				doclist::const_reverse_iterator di;
				
				for (di = lst.rbegin(); di != lst.rend(); di++)
				{
					PDoc *doc = dynamic_cast<PDoc*>(*di);
					
					if (doc && ! doc->IsWorksheet() && doc->Lock())
					{
						if (doc->QuitRequested())
							doc->Quit();
						else
						{
							doc->Unlock();
							break;
						}
					}
				}
				break;
			}
	
			case msg_OpenSelected:
				DialogCreator<COpenSelection>::CreateDialog(NULL);
				break;
			
			case msg_OpenInclude:
			{
				const char *i;
				FailOSErr(msg->FindString("include", &i));
				FindAndOpen(i);
				break;
			}
			
			case msg_FindCmd:
			{
				int c = 1 << current_workspace();
				
				if (gPrefs->GetPrefInt("window to workspace", 1))
					FindDialog()->SetWorkspaces(c);
				
				FindDialog()->Show();
				
				FindDialog()->Activate(true);
				break;
			}
			
			case B_REFS_RECEIVED:
			case 'OpFi':
				RefsReceived(msg);
				break;
			
			case msg_About:
				new PAboutBox();
				break;
			
			case msg_IDE_Make:
				PDoc::IDEMake();
				break;
			
			case msg_IDE_BringToFront:
				PDoc::IDEBringToFront();
				break;
			
			case msg_IDE_Project2Group:
				PDoc::IDEProject2Group();
				break;
			
			case msg_FindDifferences:
			{
				BRect r = PDoc::NextPosition();
				r.right = r.left + 400;
				r.bottom = r.top + 150;
				
				new CDiffWindow(r, "Differences");
				break;
			}
			
			case msg_FtpOpen:
			{
				CFtpDialog *ftpo = DialogCreator<CFtpDialog>::CreateDialog(NULL);
				ftpo->Show();
				break;
			}
			
			case msg_Open:
			{
				if (fOpenPanel->IsShowing())
				{
					fOpenPanel->Window()->SetWorkspaces(1 << current_workspace());
					fOpenPanel->Window()->Activate();
				}
				else
				{
					BEntry entry;
					gCWD.GetEntry(&entry);
					
					BAutolock lock(fOpenPanel->Window());
					
					entry_ref ref;
					entry.GetRef(&ref);
					fOpenPanel->SetPanelDirectory(&ref);
					fOpenPanel->Window()->SetWorkspaces(1 << current_workspace());
					
					if (gPrefs->GetPrefInt("zoomopen", 0))
					{
						BRect r = BScreen().Frame();
						
						fOpenPanel->Window()->MoveTo(r.left + 80, r.top + 40);
						fOpenPanel->Window()->ResizeTo(480, r.Height() - 50);
					}
					
					fOpenPanel->Show();
				}
				break;
			}
			
			case msg_CommandLineOpen:
			{
				entry_ref doc;
				FailOSErr (msg->FindRef("refs", &doc));
				
				BWindow *w;
				BEntry e;
				
				if (e.SetTo(&doc) == B_OK && e.Exists())
					w = dynamic_cast<BWindow*>(OpenWindow(doc));
				else
				{
					w = new PDoc;
					BAutolock lock(w);
					
					if (lock.IsLocked()) 
					{
						BPath p;
						if (BEntry(&doc).GetPath(&p) == B_OK)
							w->SetTitle(p.Path());
						else
							w->SetTitle(doc.name);
					}
				}
				
				long lineNr;
				if (w && msg->FindInt32("line", &lineNr) == B_OK)
				{
					BMessage m(msg_SelectLines);
					FailOSErr(m.AddInt32("from", lineNr - 1));
					FailOSErr(m.AddInt32("to", lineNr));
					w->PostMessage(&m, w->PreferredHandler());
				}
				
				if (w)
				{
					BMessage reply;
					reply.AddInt32("thread", w->Thread());
					msg->SendReply(&reply);
				}
				break;
			}
			
			case msg_New:
				new PDoc();
				break;
			
			case msg_Select:
			{
				PDoc *doc;
				if ((doc = PDoc::TopWindow()) != NULL)
				{
					BMessage copy(*msg);
					doc->PostMessage(&copy);
				}
				break;
			}
			
			case msg_DocClosed:
				if (CDoc::CountDocs() < 1)
					Quit();
				break;
			
			case msg_Tile:
				PDoc::Tile();
				break;
			
			case msg_Stack:
				PDoc::Stack();
				break;
	
			case msg_Zoom:
				if (PDoc::TopWindow())
					PDoc::TopWindow()->Zoom();
				break;
			
			case msg_Worksheet:
				OpenWorksheet();
				break;
				
			case msg_NewProject:
				new PGroupWindow;
				break;
			
			case msg_Help:
				DisplayHelp();
				break;
			
			case 1:
				puts(rcsid);
				break;
			
			case msg_Quit:
				PostMessage(B_QUIT_REQUESTED);
				break;
			
			case msg_Preferences:
			{
				long l;
				if (fPrefOpener) wait_for_thread(fPrefOpener, &l);
				fPrefOpener = (thread_id)NULL;
				if (fPrefsDialog)
				{
					BAutolock lock(fPrefsDialog);

					int c = 1 << current_workspace();
					
					if (fPrefsDialog->IsHidden())
						fPrefsDialog->Show();
					
					if (gPrefs->GetPrefInt("window to workspace", 1))
						fPrefsDialog->SetWorkspaces(c);

					fPrefsDialog->Activate(true);
				}
				else
					THROW(("Preferences panel failed to open"));
				break;
			}

			case msg_ShowGlossary:
				if (gGlossyWindow)
				{
					if (gPrefs->GetPrefInt("window to workspace", 1))
						gGlossyWindow->SetWorkspaces(1 << current_workspace());
					gGlossyWindow->Activate(true);
				}
				else
					gGlossyWindow = new PGlossyWindow;
				break;

			case msg_ShowHTMLPalette:
			{
				BWindow *w = CHTMLBar::Instance();

				if (gPrefs->GetPrefInt("window to workspace", 1))
					w->SetWorkspaces(1 << current_workspace());

				BAutolock lock(w);
				if (w->IsHidden())
					w->Show();
				break;
			}
			
			default:
				BApplication::MessageReceived(msg);
		}
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* PApp::MessageReceived */

int main()
{
	gApp = new PApp;
	be_app->Run();

	SaveRecentMenu();
	
	gPrefs->WritePrefFile();

	delete be_app;
	
	return 0;
} /* main */

void PApp::DisplayInBrowser(const entry_ref& doc)
{
	try
	{
		BMessage msg(B_NETPOSITIVE_OPEN_URL);
		
		BPath path;
		FailOSErr(BEntry(&doc).GetPath(&path));

		string url = "file://";
		url += path.Path();
		
		msg.AddString("be:url", url.c_str());

		char sig[B_MIME_TYPE_LENGTH];
		BMimeType("text/html").GetPreferredApp(sig);
		team_id team = be_roster->TeamFor(sig);

		if (team > 0)
		{
			BMessenger msr(sig, team);
			msg.AddSpecifier("Window", (int32)0);
			msr.SendMessage(&msg, (BHandler*)NULL, 1000);
			be_roster->ActivateApp(team);
		}
		else if (be_roster->Launch(sig, &msg))
			THROW(("Could not launch browser"));
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* PApp::DisplayInBrowser */

void PApp::DisplayHelp()
{
	BEntry entry;
	gAppDir.FindEntry("Documentation", &entry, true);
	BDirectory docdir(&entry);
	docdir.FindEntry("index.html", &entry, true);
	
	if (entry.InitCheck() || !entry.Exists())
		THROW(("The documentation could not be found."));
	
	entry_ref ref;
	entry.GetRef(&ref);
	
	DisplayInBrowser(ref);
} /* PApp::DisplayHelp */

BHandler *PApp::ResolveSpecifier(BMessage *msg, int32 index,
			BMessage *specifier, int32 form, const char *property)
{
	BHandler *result = NULL;

	try
	{
		PDoc *doc = NULL;

		if (strcmp(property, "Window") == 0 && form == B_NAME_SPECIFIER)
		{
			msg->PopSpecifier();
			
			entry_ref file;
			const char *fileName;

			FailOSErr(specifier->FindString("name", &fileName));
			if (get_ref_for_path(fileName, &file) == B_OK)
			{
				doc = dynamic_cast<PDoc*>(CDoc::FindDoc(file));
				if (doc == NULL)
					doc = dynamic_cast<PDoc*>(OpenWindow(file, true));
			}
		}
		
		if (doc)
			doc->PostMessage(msg);
		else
			result = BApplication::ResolveSpecifier(msg, index, specifier, form, property);
	}
	catch (HErr& e)
	{
		BMessage reply;
		reply.AddString("Error", e);
		msg->SendReply(&reply);
	}
	
	return result;
} /* PApp::ResolveSpecifier */

void PApp::FindAndOpen(const char *file)
{
	char *bi = NULL;
	bool found = false;

	try
	{
		BEntry e;
		entry_ref doc;
		BDirectory d;

		if (strncmp(file, "ftp://", 6) == 0)
		{
			CUrlOpener *uo = new CUrlOpener(file);
			uo->Run();
			return;
		}
		
		if (gPrefs->GetPrefInt("beincludes"))
		{
			bi = strdup(getenv("BEINCLUDES"));
			char *ip = bi;
			char *p = ip;
			
			while (p && !found)
			{
				char *pe = strchr(p, ';');
				if (pe) *pe = 0;
				
				FailOSErr(d.SetTo(p));
				if (d.Contains(file, B_FILE_NODE | B_SYMLINK_NODE))
				{
					FailOSErr(d.FindEntry(file, &e, true));
					if (!e.IsFile()) THROW((0));
					FailOSErr(e.GetRef(&doc));
					found = true;
				}
				
				p = (pe && pe[1]) ? pe + 1 : NULL;
			}
		}
		
		if (!found)
		{
			const char *p;
			int i = 0;

			while ((p = gPrefs->GetIxPrefString("includepath", i++))!= NULL && !found)
			{
				if (e.SetTo(p) != B_OK || !e.Exists())
					continue;

				FailOSErr(d.SetTo(p));
				if (d.Contains(file, B_FILE_NODE | B_SYMLINK_NODE))
				{
					FailOSErr(d.FindEntry(file, &e, true));
					if (!e.IsFile()) THROW((0));
					FailOSErr(e.GetRef(&doc));
					found = true;
				}
			}
		}
		
		if (found)
			OpenWindow(doc);
		else
			beep();
	}
	catch (HErr& e)
	{
		beep();
	}
	
	if (bi) free(bi);
} // PApp::FindAndOpen
