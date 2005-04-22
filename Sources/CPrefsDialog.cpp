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

	Created: 10/20/97 20:28:28
*/

#include "pe.h"
#include "CPrefsDialog.h"
#include "PMessages.h"
#include "Utils.h"
#include "PDoc.h"
#include "PApp.h"
#include "CGlossary.h"
#include "HColorUtils.h"
#include "CListBox.h"
#include "CPathsBox.h"
#include "CLangIntf.h"
#include "CKeyCapturer.h"
#include "CFindDialog.h"
#include "CStdErrBox.h"
#include "HColorControl.h"
#include "HPreferences.h"
#include "HDefines.h"
#include "MAlert.h"
#include "CMimeBox.h"
#include "CGrepBox.h"
#include "PGlossyWindow.h"

typedef BStringItem CStringItem;

class MyItem : public CStringItem
{
public:
		MyItem(const char *s, int one, int two = 0)
		: CStringItem(s), fOne(one), fTwo(two) {}
		int fOne, fTwo;
};

const unsigned long
	msg_ChangeTarget = 'ChTr',
	msg_DefPageSetup = 'Page',
	msg_StoreSuffix = 'sufx',
	
	msg_SelectSearchFolder = 'SlsF',
	msg_AddSearchFolder = 'AdsF',
	msg_ChangeSearchFolder = 'CnsF',
	msg_DeleteSearchFolder = 'DlsF',
	
	msg_SelectedKBCommand = 'KBCm',
	msg_SelectedKBBinding = 'KBBn',
	msg_AddKB = 'AddK',
	msg_DeleteKB = 'DelK',
	
	msg_SelectToolServer = 'seto',
	msg_LanguageSelected = 'lang';

CPrefsDialog::CPrefsDialog(BRect frame, const char *name, window_type type, int flags,
	BWindow *owner, BPositionIO& data)
	: HDialog(frame, name, type, flags, owner, data)
{
	fPageSetup = NULL;
	fKBKeys = NULL;
	fCurrentSuffix = NULL;

	BAutolock lock(this);

	font_family ff;
	font_style fs;
	be_plain_font->GetFamilyAndStyle(&ff, &fs);

	strcpy(fFontFamily, gPrefs->GetPrefString("font family", ff));
	strcpy(fFontStyle, gPrefs->GetPrefString("font style", fs));
	fFontSize = gPrefs->GetPrefDouble("font size", be_fixed_font->Size());

	BMenuField *mf = dynamic_cast<BMenuField*>(FindView("font"));
	FailNil(mf);
	
	fFont = mf->Menu();
	FailNil(fFont);

	for (int i = 0; i < count_font_families(); i++)
	{
		get_font_family(i, &ff);
		BMenu *fontItem = new BMenu(ff);
		FailNil(fontItem);
		fFont->AddItem(new BMenuItem(fontItem, new BMessage(msg_FieldChanged)));
		fontItem->SetFont(be_plain_font);
		
		for (int j = 0; j < count_font_styles(ff); j++)
		{
			get_font_style(ff, j, &fs);
			
			BMessage *msg = new BMessage(msg_FieldChanged);
			msg->AddString("family", ff);
			msg->AddString("style", fs);
			fontItem->AddItem(new BMenuItem(fs, msg));
		}
	}
	
	fFont->SetRadioMode(true);

	be_fixed_font->GetFamilyAndStyle(&ff, &fs);

	strcpy(fAltFontFamily, gPrefs->GetPrefString("alt font family", ff));
	strcpy(fAltFontStyle, gPrefs->GetPrefString("alt font style", fs));
	fAltFontSize = gPrefs->GetPrefDouble("alt font size", be_fixed_font->Size());

	mf = dynamic_cast<BMenuField*>(FindView("alfo"));
	FailNil(mf);
	
	fAltFont = mf->Menu();
	FailNil(fAltFont);

	for (int i = 0; i < count_font_families(); i++)
	{
		get_font_family(i, &ff);
		BMenu *fontItem = new BMenu(ff);
		FailNil(fontItem);
		fAltFont->AddItem(new BMenuItem(fontItem, new BMessage(msg_FieldChanged)));
		fontItem->SetFont(be_plain_font);
		
		for (int j = 0; j < count_font_styles(ff); j++)
		{
			get_font_style(ff, j, &fs);
			
			BMessage *msg = new BMessage(msg_FieldChanged);
			msg->AddString("altfamily", ff);
			msg->AddString("altstyle", fs);
			fontItem->AddItem(new BMenuItem(fs, msg));
		}
	}
	
	fAltFont->SetRadioMode(true);

	fLanguage = static_cast<BMenuField*>(FindView("lang"))->Menu();
	fLanguage->SetRadioMode(true);

	mf = dynamic_cast<BMenuField*>(FindView("dela"));
	FailNil(mf);
	fDefLanguage = mf->Menu();
	FailNil(fDefLanguage);

	CLangIntf *intf;
	int cookie = 0;
	while ((intf = CLangIntf::NextIntf(cookie)) != NULL)
	{
		fLanguage->AddItem(new BMenuItem(intf->Name(), new BMessage(msg_LanguageSelected)));
		fDefLanguage->AddItem(new BMenuItem(intf->Name(), new BMessage(msg_FieldChanged)));
	}

	fLang = -1; // mark as unitialized, will be initialized when "lang" is set
	SetValue("lang", 1);
	
	BTextControl *c = static_cast<BTextControl*>(FindView("sufx"));
	c->SetModificationMessage(new BMessage(msg_StoreSuffix));
	
	fState = static_cast<BMenuField*>(FindView("sast"))->Menu();
	fState->SetRadioMode(true);
	fState->ItemAt(gSavedState)->SetMarked(true);
	
	fSearchPaths = dynamic_cast<CPathsBox*>(FindView("srcP"));
	FailNil(fSearchPaths);
	fSearchPaths->SetPrefName("searchpath");
	
	fIncludePaths = dynamic_cast<CPathsBox*>(FindView("incP"));
	FailNil(fIncludePaths);
	fIncludePaths->SetPrefName("includepath");
	
	fStdErrBox = dynamic_cast<CStdErrBox*>(FindView("StEb"));
	FailNil(fStdErrBox);
	
	fMimetypes = dynamic_cast<CMimeBox*>(FindView("mimetypes"));
	
	fKBCommands = dynamic_cast<BOutlineListView*>(FindView("kact"));
	fKBKeys = dynamic_cast<BListView*>(FindView("keys"));

	FailNil(fKBCommands);
	FailNil(fKBKeys);
	
	fCap = dynamic_cast<CKeyCapturer*>(FindView("kebi"));
	FailNil(fCap);
	
	fGrepBox = dynamic_cast<CGrepBox*>(FindView("grls"));
	FailNil(fGrepBox);
	
	InitKeybindingPage();

	CancelClicked();
} /* CPrefsDialog::CPrefsDialog */

CPrefsDialog::~CPrefsDialog()
{
//	if (fPageSetup) free(fPageSetup);
	
	fKeymap.erase(fKeymap.begin(), fKeymap.end());
} /* CPrefsDialog::~CPrefsDialog */

bool CPrefsDialog::QuitRequested()
{
	BButton *ok = static_cast<BButton*>(FindView("ok  "));
	if (ok->IsEnabled())
	{
		MWarningAlert a("The preferences have changed. Save changes before closing?", "Save", "Discard");
		if (a == 1)
			OKClicked();
		else
			CancelClicked();
	}

	Hide();
	return CDoc::CountDocs() == 0;
} /* CPrefsDialog::QuitRequested */

bool CPrefsDialog::OKClicked()
{
	BMenuItem *item = fFont->FindMarked();
	if (item)
	{
		strcpy(fFontFamily, item->Label());
		item = item->Submenu()->FindMarked();
		if (item) strcpy(fFontStyle, item->Label());
	}
	
	fFontSize = atof(GetText("fons"));

	item = fAltFont->FindMarked();
	if (item)
	{
		strcpy(fAltFontFamily, item->Label());
		item = item->Submenu()->FindMarked();
		if (item) strcpy(fAltFontStyle, item->Label());
	}
	
	fAltFontSize = atof(GetText("alfs"));

	HColorControl *cc = dynamic_cast<HColorControl*>(FindView("lowc"));
	if (cc) { gColor[kLowColor] = cc->Color(); gPrefs->SetPrefColor("low color", gColor[kLowColor]); }
	cc = dynamic_cast<HColorControl*>(FindView("txtc"));
	if (cc) { gColor[kTextColor] = cc->Color(); gPrefs->SetPrefColor("text color", gColor[kTextColor]); }

	cc = dynamic_cast<HColorControl*>(FindView("selc"));
	if (cc) { gColor[kSelectionColor] = cc->Color(); gPrefs->SetPrefColor("selection color", gColor[kSelectionColor]); }
	cc = dynamic_cast<HColorControl*>(FindView("mrkc"));
	if (cc) { gColor[kMarkColor] = cc->Color(); gPrefs->SetPrefColor("mark color", gColor[kMarkColor]); }
	
	cc = dynamic_cast<HColorControl*>(FindView("keyc"));
	if (cc) { gColor[kKeyWordColor] = cc->Color(); gPrefs->SetPrefColor("keyword color", gColor[kKeyWordColor]); }
	cc = dynamic_cast<HColorControl*>(FindView("ke2c"));
	if (cc) { gColor[kAltKeyWordColor] = cc->Color(); gPrefs->SetPrefColor("alt keyword color", gColor[kAltKeyWordColor]); }
	cc = dynamic_cast<HColorControl*>(FindView("cmtc"));
	if (cc) { gColor[kCommentColor] = cc->Color(); gPrefs->SetPrefColor("comment color", gColor[kCommentColor]); }
	cc = dynamic_cast<HColorControl*>(FindView("cm2c"));
	if (cc) { gColor[kAltCommentColor] = cc->Color(); gPrefs->SetPrefColor("alt comment color", gColor[kAltCommentColor]); }
	
	cc = dynamic_cast<HColorControl*>(FindView("strc"));
	if (cc) { gColor[kStringColor] = cc->Color(); gPrefs->SetPrefColor("string color", gColor[kStringColor]); }
	cc = dynamic_cast<HColorControl*>(FindView("tgsc"));
	if (cc) { gColor[kAltStringColor] = cc->Color(); gPrefs->SetPrefColor("tagstring color", gColor[kAltStringColor]); }
	cc = dynamic_cast<HColorControl*>(FindView("numc"));
	if (cc) { gColor[kNumberColor] = cc->Color(); gPrefs->SetPrefColor("number color", gColor[kNumberColor]); }
	cc = dynamic_cast<HColorControl*>(FindView("nu2c"));
	if (cc) { gColor[kAltNumberColor] = cc->Color(); gPrefs->SetPrefColor("alt number color", gColor[kAltNumberColor]); }

	cc = dynamic_cast<HColorControl*>(FindView("opec"));
	if (cc) { gColor[kOperatorColor] = cc->Color(); gPrefs->SetPrefColor("operator color", gColor[kOperatorColor]); }
	cc = dynamic_cast<HColorControl*>(FindView("op2c"));
	if (cc) { gColor[kAltOperatorColor] = cc->Color(); gPrefs->SetPrefColor("alt operator color", gColor[kAltOperatorColor]); }
	cc = dynamic_cast<HColorControl*>(FindView("sepc"));
	if (cc) { gColor[kSeparatorColor] = cc->Color(); gPrefs->SetPrefColor("separator color", gColor[kSeparatorColor]); }
	cc = dynamic_cast<HColorControl*>(FindView("se2c"));
	if (cc) { gColor[kAltSeparatorColor] = cc->Color(); gPrefs->SetPrefColor("alt separator color", gColor[kAltSeparatorColor]); }

	cc = dynamic_cast<HColorControl*>(FindView("proc"));
	if (cc) { gColor[kPreProcessorColor] = cc->Color(); gPrefs->SetPrefColor("preprocessor color", gColor[kPreProcessorColor]); }
	cc = dynamic_cast<HColorControl*>(FindView("pr2c"));
	if (cc) { gColor[kAltProcessorColor] = cc->Color(); gPrefs->SetPrefColor("altprocessor color", gColor[kAltProcessorColor]); }
	cc = dynamic_cast<HColorControl*>(FindView("errc"));
	if (cc) { gColor[kErrorColor] = cc->Color(); gPrefs->SetPrefColor("error color", gColor[kErrorColor]); }
	cc = dynamic_cast<HColorControl*>(FindView("er2c"));
	if (cc) { gColor[kAltErrorColor] = cc->Color(); gPrefs->SetPrefColor("alt error color", gColor[kAltErrorColor]); }

	cc = dynamic_cast<HColorControl*>(FindView("sidc"));
	if (cc) { gColor[kSystemIdentifierColor] = cc->Color(); gPrefs->SetPrefColor("system identifier color", gColor[kSystemIdentifierColor]); }
	cc = dynamic_cast<HColorControl*>(FindView("chcc"));
	if (cc) { gColor[kCharConstColor] = cc->Color(); gPrefs->SetPrefColor("char constant color", gColor[kCharConstColor]); }

	cc = dynamic_cast<HColorControl*>(FindView("uidc"));
	if (cc) { gColor[kUserIdentifierColor] = cc->Color(); gPrefs->SetPrefColor("user identifier color", gColor[kUserIdentifierColor]); }
	cc = dynamic_cast<HColorControl*>(FindView("tagc"));
	if (cc) { gColor[kTagColor] = cc->Color(); gPrefs->SetPrefColor("tag color", gColor[kTagColor]); }

	cc = dynamic_cast<HColorControl*>(FindView("us1c"));
	if (cc) { gColor[kUser1] = cc->Color(); gPrefs->SetPrefColor("user1", gColor[kUser1]); }
	cc = dynamic_cast<HColorControl*>(FindView("us2c"));
	if (cc) { gColor[kUser2] = cc->Color(); gPrefs->SetPrefColor("user2", gColor[kUser2]); }
	cc = dynamic_cast<HColorControl*>(FindView("us3c"));
	if (cc) { gColor[kUser3] = cc->Color(); gPrefs->SetPrefColor("user3", gColor[kUser3]); }
	cc = dynamic_cast<HColorControl*>(FindView("us4c"));
	if (cc) { gColor[kUser4] = cc->Color(); gPrefs->SetPrefColor("user4", gColor[kUser4]); }

	cc = dynamic_cast<HColorControl*>(FindView("invc"));
	if (cc) { gColor[kInvisiblesColor] = cc->Color(); gPrefs->SetPrefColor("invisibles color", gColor[kInvisiblesColor]); }

	DefineInvColors(gColor[kSelectionColor]);

	gPrefs->SetPrefString("font family", fFontFamily);
	gPrefs->SetPrefString("font style", fFontStyle);
	gPrefs->SetPrefDouble("font size", fFontSize);
	
	gPrefs->SetPrefString("alt font family", fAltFontFamily);
	gPrefs->SetPrefString("alt font style", fAltFontStyle);
	gPrefs->SetPrefDouble("alt font size", fAltFontSize);

	strcpy(gTabChar, GetText("stca"));
	gPrefs->SetPrefString("tab char", gTabChar);
	strcpy(gSpaceChar, GetText("ssca"));
	gPrefs->SetPrefString("space char", gSpaceChar);
	strcpy(gReturnChar, GetText("srca"));
	gPrefs->SetPrefString("return char", gReturnChar);
	strcpy(gControlChar, GetText("scca"));
	gPrefs->SetPrefString("control char", gControlChar);
	
	gPrefs->SetPrefInt("auto indent", gAutoIndent = IsOn("auin"));
	gPrefs->SetPrefInt("syntax coloring", gSyntaxColoring = IsOn("syco"));
	gPrefs->SetPrefInt("spaces per tab", gSpacesPerTab = atoi(GetText("tabs")));
	gPrefs->SetPrefInt("balance", gBalance = IsOn("baty"));
	gPrefs->SetPrefInt("block cursor", gBlockCursor = IsOn("blcu"));
	gPrefs->SetPrefInt("flash cursor", gFlashCursor = IsOn("flcu"));
	gPrefs->SetPrefInt("smart braces", gSmartBrace = IsOn("smbr"));
	gPrefs->SetPrefInt("althome", IsOn("alho"));
	gPrefs->SetPrefInt("show tabs", IsOn("stab"));
	gPrefs->SetPrefInt("window to workspace", IsOn("smwo"));

	gPrefs->SetPrefInt("diffcase", IsOn("dica"));
	gPrefs->SetPrefInt("diffwhite", IsOn("diwh"));

	gPrefs->SetPrefInt("softwrap", IsOn("sowr"));
	
	if (IsOn("wrwi"))
		gPrefs->SetPrefInt("wraptype", 1);
	else if (IsOn("wrpa"))
		gPrefs->SetPrefInt("wraptype", 2);
	else
		gPrefs->SetPrefInt("wraptype", 3);

	gPrefs->SetPrefInt("wrapcol", atoi(GetText("wrft")));

	gPrefs->SetPrefInt("includes", gPopupIncludes = IsOn("shin"));
	gPrefs->SetPrefInt("protos", gPopupProtos = IsOn("shpr"));
	gPrefs->SetPrefInt("types", gPopupFuncs = IsOn("shty"));
	gPrefs->SetPrefInt("sortpopup", IsOn("sopo"));
	
	if (IsOn("swne"))
		gPrefs->SetPrefInt("startup", 1);
	else if (IsOn("swop"))
		gPrefs->SetPrefInt("startup", 2);
	else
		gPrefs->SetPrefInt("startup", 3);

	gPrefs->SetPrefInt("redirect stderr", gRedirectStdErr = IsOn("rdse"));
	gPrefs->SetPrefInt("worksheet", gUseWorksheet = IsOn("wosh"));
	
	int cookie = 0;
	CLangIntf *intf;
	while ((intf = CLangIntf::NextIntf(cookie)) != NULL)
		intf->SetExtensions(fSuffixes[cookie - 1].c_str());
	
	item = fDefLanguage->FindMarked();
	gPrefs->SetPrefString("def lang", item ? item->Label() : "None");
	CLangIntf::ChooseDefault();
	
	gPrefs->SetPrefInt("restore position", gRestorePosition = IsOn("repo"));
	gPrefs->SetPrefInt("restore font", gRestoreFont = IsOn("refo"));
	gPrefs->SetPrefInt("restore selection", gRestoreSelection = IsOn("rese"));
	gPrefs->SetPrefInt("restore scrollbar", gRestoreScrollbar = IsOn("resc"));
	gPrefs->SetPrefInt("restore cwd", gRestoreCWD = IsOn("recw"));

	gPrefs->SetPrefInt("saved state", gSavedState = fState->IndexOf(fState->FindMarked()));
	
	gPrefs->SetPrefInt("recent size", gRecentBufferSize = atoi(GetText("resi")));
	gPrefs->SetPrefInt("skiptmp", IsOn("sktm"));
	gPrefs->SetPrefInt("zoomopen", IsOn("zoop"));
	gPrefs->SetPrefInt("fullpath", IsOn("fupa"));
	
	gPrefs->SetPrefInt("parent", IsOn("sepa"));
	gPrefs->SetPrefInt("beincludes", IsOn("incl"));

	gPrefs->SetPrefInt("save resources", IsOn("core"));
	gPrefs->SetPrefInt("backup", IsOn("maba"));
	gPrefs->SetPrefInt("nl at eof", IsOn("fonl"));
	gPrefs->SetPrefInt("verify", IsOn("vofm"));
	
	gPrefs->SetPrefInt("ide menu", IsOn("idmn"));
	gPrefs->SetPrefInt("mw plugins", IsOn("mwpl"));

	gPrefs->SetPrefInt("passive ftp", IsOn("pftp"));
	
	gPrefs->SetPrefInt("showglossary", IsOn("shgl"));
	gPrefs->SetPrefInt("singleclickglossary", IsOn("sigl"));

	gPrefs->SetPrefInt("show htmlpalette", IsOn("shtp"));
	gPrefs->SetPrefInt("show htmlpalette for html", IsOn("shtP"));
	
	gPrefs->SetPrefInt("isearch_igncase", IsOn("isic"));
	gPrefs->SetPrefInt("centerfound", IsOn("cesf"));
	
	gPrefs->SetPrefInt("relative group paths", IsOn("rlgp"));
	gPrefs->SetPrefInt("sortgroup", IsOn("sogr"));

	gPrefs->SetPrefInt("sortproject", IsOn("prso"));
	gPrefs->SetPrefInt("autodetect projects", IsOn("prad"));
	
	fSearchPaths->DoOK();
	fIncludePaths->DoOK();
	fMimetypes->DoOK();
	fGrepBox->DoOK();
	fStdErrBox->DoOK();
	
	SetEnabled("ok  ", false);
	SetEnabled("cncl", false);

	PDoc::PostToAll(msg_PrefsChanged, true);
	gFindDialog->PostMessage(msg_PrefsChanged);

	CKeyMapper::Instance().WriteKeymap(fKeymap);

	return false;
} /* CPrefsDialog::OKClicked */

bool CPrefsDialog::CancelClicked()
{
	BMenuItem *item = fFont->FindMarked();
	if (item)
	{
		item = item->Submenu()->FindMarked();
		if (item) item->SetMarked(false);
		fFont->FindMarked()->SetMarked(false);
	}
	
	item = fFont->FindItem(fFontFamily);
	if (item)
	{
		item->SetMarked(true);
		item = item->Submenu()->FindItem(fFontStyle);
		if (item) item->SetMarked(true);
	}

	item = fAltFont->FindMarked();
	if (item)
	{
		item = item->Submenu()->FindMarked();
		if (item) item->SetMarked(false);
		fAltFont->FindMarked()->SetMarked(false);
	}
	
	item = fAltFont->FindItem(fAltFontFamily);
	if (item)
	{
		item->SetMarked(true);
		item = item->Submenu()->FindItem(fAltFontStyle);
		if (item) item->SetMarked(true);
	}

	char s[32];
	sprintf(s, "%d", gPrefs->GetPrefInt("font size", 9));
	SetText("fons", s);

	sprintf(s, "%d", gPrefs->GetPrefInt("alt font size", 10));
	SetText("alfs", s);

//	if (fPageSetup) free(fPageSetup);
//	fPageSetup = (char *)Hex2Bin(gPrefs->GetPrefString("default page setup", ""), fPageSetupSize);
	
	HColorControl *cc = dynamic_cast<HColorControl*>(FindView("lowc"));
	if (cc) cc->SetColor(gColor[kLowColor]);
	cc = dynamic_cast<HColorControl*>(FindView("txtc"));
	if (cc) cc->SetColor(gColor[kTextColor]);
	
	cc = dynamic_cast<HColorControl*>(FindView("selc"));
	if (cc) cc->SetColor(gColor[kSelectionColor]);
	cc = dynamic_cast<HColorControl*>(FindView("mrkc"));
	if (cc) cc->SetColor(gColor[kMarkColor]);

	cc = dynamic_cast<HColorControl*>(FindView("keyc"));
	if (cc) cc->SetColor(gColor[kKeyWordColor]);
	cc = dynamic_cast<HColorControl*>(FindView("ke2c"));
	if (cc) cc->SetColor(gColor[kAltKeyWordColor]);
	cc = dynamic_cast<HColorControl*>(FindView("cmtc"));
	if (cc) cc->SetColor(gColor[kCommentColor]);
	cc = dynamic_cast<HColorControl*>(FindView("cm2c"));
	if (cc) cc->SetColor(gColor[kAltCommentColor]);
	
	cc = dynamic_cast<HColorControl*>(FindView("strc"));
	if (cc) cc->SetColor(gColor[kStringColor]);
	cc = dynamic_cast<HColorControl*>(FindView("tgsc"));
	if (cc) cc->SetColor(gColor[kAltStringColor]);
	cc = dynamic_cast<HColorControl*>(FindView("numc"));
	if (cc) cc->SetColor(gColor[kNumberColor]);
	cc = dynamic_cast<HColorControl*>(FindView("nu2c"));
	if (cc) cc->SetColor(gColor[kAltNumberColor]);

	cc = dynamic_cast<HColorControl*>(FindView("opec"));
	if (cc) cc->SetColor(gColor[kOperatorColor]);
	cc = dynamic_cast<HColorControl*>(FindView("op2c"));
	if (cc) cc->SetColor(gColor[kAltOperatorColor]);
	cc = dynamic_cast<HColorControl*>(FindView("sepc"));
	if (cc) cc->SetColor(gColor[kSeparatorColor]);
	cc = dynamic_cast<HColorControl*>(FindView("se2c"));
	if (cc) cc->SetColor(gColor[kAltSeparatorColor]);

	cc = dynamic_cast<HColorControl*>(FindView("proc"));
	if (cc) cc->SetColor(gColor[kPreProcessorColor]);
	cc = dynamic_cast<HColorControl*>(FindView("pr2c"));
	if (cc) cc->SetColor(gColor[kAltProcessorColor]);
	cc = dynamic_cast<HColorControl*>(FindView("errc"));
	if (cc) cc->SetColor(gColor[kErrorColor]);
	cc = dynamic_cast<HColorControl*>(FindView("er2c"));
	if (cc) cc->SetColor(gColor[kAltErrorColor]);

	cc = dynamic_cast<HColorControl*>(FindView("sidc"));
	if (cc) cc->SetColor(gColor[kSystemIdentifierColor]);
	cc = dynamic_cast<HColorControl*>(FindView("chcc"));
	if (cc) cc->SetColor(gColor[kCharConstColor]);

	cc = dynamic_cast<HColorControl*>(FindView("uidc"));
	if (cc) cc->SetColor(gColor[kUserIdentifierColor]);
	cc = dynamic_cast<HColorControl*>(FindView("tagc"));
	if (cc) cc->SetColor(gColor[kTagColor]);
	
	cc = dynamic_cast<HColorControl*>(FindView("us1c"));
	if (cc) cc->SetColor(gColor[kUser1]);
	cc = dynamic_cast<HColorControl*>(FindView("us2c"));
	if (cc) cc->SetColor(gColor[kUser2]);
	cc = dynamic_cast<HColorControl*>(FindView("us3c"));
	if (cc) cc->SetColor(gColor[kUser3]);
	cc = dynamic_cast<HColorControl*>(FindView("us4c"));
	if (cc) cc->SetColor(gColor[kUser4]);
	cc = dynamic_cast<HColorControl*>(FindView("invc"));
	if (cc) cc->SetColor(gColor[kInvisiblesColor]);
	
	SetOn("auin", gPrefs->GetPrefInt("auto indent", 1));
	SetOn("syco", gPrefs->GetPrefInt("syntax coloring", 1));
	SetOn("baty", gPrefs->GetPrefInt("balance", 1));
	SetOn("blcu", gPrefs->GetPrefInt("block cursor", 0));
	SetOn("flcu", gPrefs->GetPrefInt("flash cursor", 1));
	SetOn("smbr", gPrefs->GetPrefInt("smart braces", 1));
	SetOn("alho", gPrefs->GetPrefInt("althome", 0));
	SetOn("stab", gPrefs->GetPrefInt("show tabs", 1));
	SetOn("smwo", gPrefs->GetPrefInt("window to workspace", 1));

	sprintf(s, "%d", gSpacesPerTab);
	SetText("tabs", s);

	SetOn("dica", gPrefs->GetPrefInt("diffcase", false));
	SetOn("diwh", gPrefs->GetPrefInt("diffwhite", false));
	
	bool sw;
	SetOn("sowr", sw = gPrefs->GetPrefInt("softwrap", false));
	
	int wrap = gPrefs->GetPrefInt("wraptype", 3);
	switch (wrap)
	{
		case 1:		SetOn("wrwi", true); break;
		case 2:		SetOn("wrpa", true); break;
		default:	SetOn("wrfi", true); break;
	}

	sprintf(s, "%d", gPrefs->GetPrefInt("wrapcol", 80));
	SetText("wrft", s);

	SetEnabled("wrft", /*sw && */wrap == 3);
	
	SetText("stca", gTabChar);
	SetText("ssca", gSpaceChar);
	SetText("srca", gReturnChar);
	SetText("scca", gControlChar);

	SetOn("shin", gPrefs->GetPrefInt("includes", 1));
	SetOn("shpr", gPrefs->GetPrefInt("protos", 0));
	SetOn("shty", gPrefs->GetPrefInt("types", 1));
	SetOn("sopo", gPrefs->GetPrefInt("sortpopup", 0));

	switch (gPrefs->GetPrefInt("startup"))
	{
		default: SetOn("swne", true); break;
		case 2: SetOn("swop", true); break;
		case 3: SetOn("swno", true); break;
	}
	
	SetOn("rdse", gPrefs->GetPrefInt("redirect stderr", 1));
	SetOn("wosh", gPrefs->GetPrefInt("worksheet", 1));

	fSuffixes.erase(fSuffixes.begin(), fSuffixes.end());
	
	CLangIntf *intf;
	int cookie = 0, i = 0;
	const char *defLang = gPrefs->GetPrefString("def lang", "None");

	while ((intf = CLangIntf::NextIntf(cookie)) != NULL)
	{
		fSuffixes.push_back(intf->Extensions());
		if (strcmp(intf->Name(), defLang) == 0)
			SetValue("dela", i + 3);
		i++;
	}

	SetText("sufx", fSuffixes.size() ? fSuffixes[GetValue("lang") - 1].c_str() : "");

	SetOn("repo", gPrefs->GetPrefInt("restore position", 1));
	SetOn("refo", gPrefs->GetPrefInt("restore font", 1));
	SetOn("rese", gPrefs->GetPrefInt("restore selection", 1));
	SetOn("resc", gPrefs->GetPrefInt("restore scrollbar", 1));
	SetOn("recw", gPrefs->GetPrefInt("restore cwd", 1));

	sprintf(s, "%d", gRecentBufferSize);
	SetText("resi", s);
	SetOn("sktm", gPrefs->GetPrefInt("skiptmp", 1));
	SetOn("zoop", gPrefs->GetPrefInt("zoomopen", 0));
	SetOn("fupa", gPrefs->GetPrefInt("fullpath", 1));
	
	SetOn("sepa", gPrefs->GetPrefInt("parent", 1));
	SetOn("incl", gPrefs->GetPrefInt("beincludes", 1));

	SetOn("core", gPrefs->GetPrefInt("save resources", 1));
	SetOn("coat", gPrefs->GetPrefInt("save attributes", 1));

	SetOn("maba", gPrefs->GetPrefInt("backup", 0));
	SetOn("fonl", gPrefs->GetPrefInt("nl at eof", 1));
	SetOn("vofm", gPrefs->GetPrefInt("verify", 1));
	
	SetOn("idmn", gPrefs->GetPrefInt("ide menu", 1));
	SetOn("mwpl", gPrefs->GetPrefInt("mw plugins", 1));
	
	SetOn("pftp", gPrefs->GetPrefInt("passive ftp", 1));

	SetOn("shgl", gPrefs->GetPrefInt("showglossary", 0));
	SetOn("sigl", gPrefs->GetPrefInt("singleclickglossary", 0));

	SetOn("shtp", gPrefs->GetPrefInt("show htmlpalette", 1));
	SetOn("shtP", gPrefs->GetPrefInt("show htmlpalette for html", 1));
	
	SetEnabled("shtP", IsOn("shtp"));

	SetOn("isic", gPrefs->GetPrefInt("isearch_igncase", 1));
	SetOn("cesf", gPrefs->GetPrefInt("centerfound", 0));
	
	SetOn("sogr", gPrefs->GetPrefInt("sortgroup", 1));
	SetOn("rlgp", gPrefs->GetPrefInt("relative group paths", 0));

	SetOn("prso", gPrefs->GetPrefInt("sortproject", 1));
	SetOn("prad", gPrefs->GetPrefInt("autodetect projects", 1));

	fSearchPaths->DoCancel();
	fIncludePaths->DoCancel();
	fMimetypes->DoCancel();
	fGrepBox->DoCancel();
	fStdErrBox->DoCancel();

	SetEnabled("ok  ", false);
	SetEnabled("cncl", false);
	
	CKeyMapper::Instance().ReadKeymap(fKeymap);
	fKBCommands->DeselectAll();
	
	return false;
} /* CPrefsDialog::CancelClicked */

void CPrefsDialog::UpdateFields()
{
	const char *ff, *fs;
	if (CurrentMessage()->FindString("family", &ff) == B_NO_ERROR &&
		CurrentMessage()->FindString("style", &fs) == B_NO_ERROR)
	{
		BMenuItem *item;

		item = fFont->FindMarked();
		if (item)
		{
			item = item->Submenu()->FindMarked();
			if (item) item->SetMarked(false);
			fFont->FindMarked()->SetMarked(false);
		}
		
		item = fFont->FindItem(ff);
		if (item)
		{
			item->SetMarked(true);
			item = item->Submenu()->FindItem(fs);
			if (item) item->SetMarked(true);
		}
	}

	if (CurrentMessage()->FindString("altfamily", &ff) == B_NO_ERROR &&
		CurrentMessage()->FindString("altstyle", &fs) == B_NO_ERROR)
	{
		BMenuItem *item;

		item = fAltFont->FindMarked();
		if (item)
		{
			item = item->Submenu()->FindMarked();
			if (item) item->SetMarked(false);
			fAltFont->FindMarked()->SetMarked(false);
		}
		
		item = fAltFont->FindItem(ff);
		if (item)
		{
			item->SetMarked(true);
			item = item->Submenu()->FindItem(fs);
			if (item) item->SetMarked(true);
		}
	}
	
	bool statePe = (fState->IndexOf(fState->FindMarked()) == 0);
	SetEnabled("restore scrollbar", statePe);
	SetEnabled("restore cwd", statePe);
	
	SetEnabled("wrft", IsOn("wrfi"));

	SetEnabled("shtP", IsOn("shtp"));

	UpdateKBPage();
	
} /* CPrefsDialog::UpdateFields */

void CPrefsDialog::GetDefPageSetup()
{
//	BPrintJob prJob("a page setup job");
//	
//	if (fPageSetup)
//	{
//		BMessage *s = new BMessage;
//		if (s && s->Unflatten(fPageSetup) == B_NO_ERROR)
//			prJob.SetSettings(s);
//	}
//	
//	int result = prJob.ConfigPage();
//	
//	if (result == B_NO_ERROR)
//	{
//		BMessage s(prJob.Settings());
//
//		fPageSetupSize = s.FlattenedSize();
//		if (fPageSetup) free(fPageSetup);
//		fPageSetup = (char *)malloc(fPageSetupSize);
//		FailNil(fPageSetup);
//		result = s.Flatten(fPageSetup, fPageSetupSize);
//		FailOSErrMsg(result, "error flattening (%d)");
//		
//		UpdateFields();
//	}
} /* CPrefsDialog::GetDefPageSetup */
	
void CPrefsDialog::MessageReceived(BMessage *msg)
{
	long what = msg->what;
	
	switch (what)
	{
		case msg_ReloadGlossary:
			delete gGlossary;
			gGlossary = new CGlossary;
			if (gGlossyWindow)
				gGlossyWindow->PostMessage(msg_ReloadGlossary);
			break;
	
		case msg_SelectedKBCommand:
			UpdateKBPage();
			break;
	
		case msg_SelectedKBBinding:
			UpdateKBCapturer();
			break;
		
		case msg_AddKB:
			AddKeybinding();
			SetEnabled("ok  ", true);
			SetEnabled("cncl", true);
			break;
	
		case msg_DeleteKB:
			DeleteKeybinding();
			SetEnabled("ok  ", true);
			SetEnabled("cncl", true);
			break;
		
		case 'addP':
			fGrepBox->DoCancel();
			SetEnabled("ok  ", true);
			SetEnabled("cncl", true);
			break;
		
		case msg_StoreSuffix:
			if (fSuffixes.size()) {
				fSuffixes[GetValue("lang") - 1] = GetText("sufx");
			}
			if (fCurrentSuffix != 0) {
				if (strcmp(fCurrentSuffix,fSuffixes[fLang].c_str()) != 0) {
					SetEnabled("ok  ", true);
					SetEnabled("cncl", true);
				}
			}
			break;
		
		case msg_LanguageSelected:
			fLang = GetValue("lang") - 1;
			{
				int cookie = 0, i = 0;
				CLangIntf *intf = NULL;
				while ((intf = CLangIntf::NextIntf(cookie)) != NULL) {
					if (i == fLang) {
						fCurrentSuffix = intf->Extensions();
						break;
					}
					i++;
				}
			}
			if (fSuffixes.size()) {
				SetText("sufx", fSuffixes[fLang].c_str());
			}
			break;
	
		case msg_FieldChanged:
			SetEnabled("ok  ", true);
			SetEnabled("cncl", true);
			// fall through
		default:
			HDialog::MessageReceived(msg);
	}
} /* CPrefsDialog::MessageReceived */

void CPrefsDialog::CreateField(int kind, BPositionIO& data, BView*& inside)
{
	dRect r;
	char name[256];
	BView *v;
	
	switch (kind)
	{
		case 'pbox':
			data >> r >> name;
			inside->AddChild(v = new CPathsBox(r.ToBe(), name));
			break;
		case 'gbox':
			data >> r >> name;
			inside->AddChild(v = new CGrepBox(r.ToBe(), name));
			break;
		case 'keyc':
			data >> r >> name;
			inside->AddChild(v = new CKeyCapturer(r.ToBe(), name));
			break;
		case 'slbx':
			data >> r >> name;
			inside->AddChild(v = new CMimeBox(r.ToBe(), name));
			break;
		case 'ebox':
			data >> r >> name;
			inside->AddChild(v = new CStdErrBox(r.ToBe(), name));
			break;
	}
} /* CPrefsDialog::CreateField */


void CPrefsDialog::RegisterFields()
{
	RegisterFieldCreator('pbox', CreateField);
	RegisterFieldCreator('ebox', CreateField);
	RegisterFieldCreator('slbx', CreateField);
	RegisterFieldCreator('keyc', CreateField);
	RegisterFieldCreator('gbox', CreateField);
} /* CPrefsDialog::RegisterFields */

void CPrefsDialog::InitKeybindingPage()
{
	int resID = 0;
	
	while (true)
	{
		long cnt, cmd;
		char s[256];
		const char *name;

		size_t size;
		const void *p = HResources::GetResource('Cmnd', resID, size, &name);
		if (p == NULL)
			break;

		BMemoryIO buf(p, size);
		buf >> cnt;
		
		MyItem *mom = new MyItem(name, 0);
		fKBCommands->AddItem(mom);
		
		if (resID == 9)
		{
			BPopUpMenu menu("hoi");
			PDoc::BuildExtensionsMenu(&menu);
			cmd = 'ex\0\0';
		
			for (int i = 0; i < menu.CountItems(); i++) {
				uint16 extHash = HashString16(menu.ItemAt(i)->Label());
				fKBCommands->AddUnder(new MyItem(menu.ItemAt(i)->Label(), cmd|extHash), mom);
			}
		}
		else
		{
			while (cnt--)
			{
				buf >> cmd;
		
				int i = 0;
				do	buf >> s[i];
				while (s[i++]);
				
				fKBCommands->AddUnder(new MyItem(s, cmd), mom);
			}
		}
		fKBCommands->Collapse(mom);
		resID++;
	}
	
	fKBCommands->SetSelectionMessage(new BMessage(msg_SelectedKBCommand));
	fKBKeys->SetSelectionMessage(new BMessage(msg_SelectedKBBinding));
} /* CPrefsDialog::InitKeybindingsPage */

void CPrefsDialog::UpdateKBPage()
{
	if (fKBKeys == NULL)
		return;
	
	MyItem *mi;
	
	while (fKBKeys->CountItems() > 0)
		delete fKBKeys->RemoveItem((int32)0);
	
	fCap->SetShortcut(KeyShortcut());
	
	mi = static_cast<MyItem *>(fKBCommands->ItemAt(fKBCommands->CurrentSelection()));
	
	if (mi && mi->fOne)
	{
		keymap::iterator ki;
		
		for (ki = fKeymap.begin(); ki != fKeymap.end(); ki++)
		{
			if ((*ki).second == mi->fOne)
			{
				char s[256];
	
				CKeyCapturer::DescribeKeys((*ki).first, s);
				
				fKBKeys->AddItem(new MyItem(s, (*ki).first.prefix, (*ki).first.combo));
			}
		}
	}
} /* CPrefsDialog::UpdateKBPage */

void CPrefsDialog::UpdateKBCapturer()
{
	MyItem *mi = static_cast<MyItem*>(fKBKeys->ItemAt(fKBKeys->CurrentSelection()));
	
	KeyShortcut ks;

	if (mi)
	{
		ks.prefix = mi->fOne;
		ks.combo = mi->fTwo;
	}

	fCap->SetShortcut(ks);	
} /* CPrefsDialog::UpdateKBCapturer */

void CPrefsDialog::AddKeybinding()
{
	MyItem *mi;
	
	mi = static_cast<MyItem *>(fKBCommands->ItemAt(fKBCommands->CurrentSelection()));
	
	if (mi && mi->fOne)
	{
		KeyShortcut ks = fCap->Shortcut();

		ks.prefix &= BINDINGMASK;
		ks.combo &= BINDINGMASK;

		keymap::iterator ki = fKeymap.find(ks);
		if (ki != fKeymap.end() && (*ki).second != mi->fOne)
		{
			MInfoAlert a("Do you want to replace the current binding for this shortcut?",
				"Replace", "Cancel");
			if (a.Go() != 1)
				return;
		}
	
		fKeymap[ks] = mi->fOne;
		
		UpdateFields();
	}
} /* CPrefsDialog::AddKeybinding */

void CPrefsDialog::DeleteKeybinding()
{
	MyItem *mi;
	
	mi = static_cast<MyItem *>(fKBKeys->ItemAt(fKBKeys->CurrentSelection()));
	
	if (mi)
	{
		KeyShortcut ks;
		
		ks.prefix = mi->fOne & BINDINGMASK;
		ks.combo = mi->fTwo & BINDINGMASK;
		
		if (fKeymap.find(ks) != fKeymap.end())
		{
			fKeymap.erase(fKeymap.find(ks));
			UpdateFields();
		}
	}
} /* CPrefsDialog::DeleteKeybinding */
