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
#include "CLanguageInterface.h"
#include "CKeyCapturer.h"
#include "CFindDialog.h"
#include "CStdErrBox.h"
#include "HColorControl.h"
#include "HPreferences.h"
#include "HDefines.h"
#include "HTabSheet.h"
#include "MAlert.h"
#include "CMimeBox.h"
#include "CGrepBox.h"
#include "PGlossyWindow.h"
#include "ResourcesBindings.h"
#include "Prefs.h"

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
	BWindow *owner, BPositionIO* data)
	: HDialog(frame, name, type, flags, owner, data)
{
	fPageSetup = NULL;
	fKBKeys = NULL;
	fCurrentSuffix = NULL;

	BAutolock lock(this);

	font_family ff;
	font_style fs;
	be_plain_font->GetFamilyAndStyle(&ff, &fs);

	strcpy(fFontFamily, gPrefs->GetPrefString(prf_S_FontFamily, ff));
	strcpy(fFontStyle, gPrefs->GetPrefString(prf_S_FontStyle, fs));
	fFontSize = gPrefs->GetPrefDouble(prf_D_FontSize, be_fixed_font->Size());

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

	strcpy(fAltFontFamily, gPrefs->GetPrefString(prf_S_AltFontFamily, ff));
	strcpy(fAltFontStyle, gPrefs->GetPrefString(prf_S_AltFontStyle, fs));
	fAltFontSize = gPrefs->GetPrefDouble(prf_D_AltFontSize, be_fixed_font->Size());

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

	CLanguageInterface *intf;
	int cookie = 0;
	while ((intf = CLanguageInterface::NextIntf(cookie)) != NULL)
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
	fSearchPaths->SetPrefName(prf_X_SearchPath);

	fIncludePaths = dynamic_cast<CPathsBox*>(FindView("incP"));
	FailNil(fIncludePaths);
	fIncludePaths->SetPrefName(prf_X_IncludePath);

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
			OkClicked();
		else
			CancelClicked();
	}

	Hide();
	return CDoc::CountDocs() == 0;
} /* CPrefsDialog::QuitRequested */

bool CPrefsDialog::OkClicked()
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

	GetColor("lowc", kColorLow,				prf_C_Low);
	GetColor("txtc", kColorText,			prf_C_Text);
	GetColor("selc", kColorSelection,		prf_C_Selection);
	GetColor("mrkc", kColorMark,			prf_C_Mark);
	GetColor("keyc", kColorKeyword1,		prf_C_Keyword);
	GetColor("ke2c", kColorKeyword2,		prf_C_AltKeyword);
	GetColor("cmtc", kColorComment1,		prf_C_Comment);
	GetColor("cm2c", kColorComment2,		prf_C_AltComment);
	GetColor("strc", kColorString1,			prf_C_String);
	GetColor("tgsc", kColorString2,			prf_C_Tagstring);
	GetColor("numc", kColorNumber1,			prf_C_Number);
	GetColor("nu2c", kColorNumber2,			prf_C_AltNumber);
	GetColor("opec", kColorOperator1,		prf_C_Operator);
	GetColor("op2c", kColorOperator2,		prf_C_AltOperator);
	GetColor("sepc", kColorSeparator1,		prf_C_Separator);
	GetColor("se2c", kColorSeparator2,		prf_C_AltSeparator);
	GetColor("proc", kColorPreprocessor1,	prf_C_Preprocessor);
	GetColor("pr2c", kColorPreprocessor2,	prf_C_AltProcessor);
	GetColor("errc", kColorError1,			prf_C_Error);
	GetColor("er2c", kColorError2,			prf_C_AltError);
	GetColor("sidc", kColorIdentifierSystem,prf_C_SystemIdentifier);
	GetColor("chcc", kColorCharConst,		prf_C_CharConstant);
	GetColor("uidc", kColorIdentifierUser,	prf_C_UserIdentifier);
	GetColor("tagc", kColorTag,				prf_C_Tag);
	GetColor("attr", kColorAttribute,		prf_C_Attribute);
	GetColor("us1c", kColorUserSet1,		prf_C_User1);
	GetColor("us2c", kColorUserSet2,		prf_C_User2);
	GetColor("us3c", kColorUserSet3,		prf_C_User3);
	GetColor("us4c", kColorUserSet4,		prf_C_User4);
	GetColor("invc", kColorInvisibles,		prf_C_Invisibles);

	DefineInvColors(gColor[kColorSelection]);

	gPrefs->SetPrefString(prf_S_FontFamily, fFontFamily);
	gPrefs->SetPrefString(prf_S_FontStyle, fFontStyle);
	gPrefs->SetPrefDouble(prf_D_FontSize, fFontSize);

	gPrefs->SetPrefString(prf_S_AltFontFamily, fAltFontFamily);
	gPrefs->SetPrefString(prf_S_AltFontStyle, fAltFontStyle);
	gPrefs->SetPrefDouble(prf_D_AltFontSize, fAltFontSize);

	strcpy(gTabChar, GetText("stca"));
	gPrefs->SetPrefString(prf_S_TabChar, gTabChar);
	strcpy(gSpaceChar, GetText("ssca"));
	gPrefs->SetPrefString(prf_S_SpaceChar, gSpaceChar);
	strcpy(gReturnChar, GetText("srca"));
	gPrefs->SetPrefString(prf_S_ReturnChar, gReturnChar);
	strcpy(gControlChar, GetText("scca"));
	gPrefs->SetPrefString(prf_S_ControlChar, gControlChar);

	gPrefs->SetPrefInt(prf_I_AutoIndent, gAutoIndent = IsOn("auin"));
	gPrefs->SetPrefInt(prf_I_SyntaxColoring, gSyntaxColoring = IsOn("syco"));
	gPrefs->SetPrefInt(prf_I_SpacesPerTab, gSpacesPerTab = atoi(GetText("tabs")));
	gPrefs->SetPrefInt(prf_I_Balance, gBalance = IsOn("baty"));
	gPrefs->SetPrefInt(prf_I_BlockCursor, gBlockCursor = IsOn("blcu"));
	gPrefs->SetPrefInt(prf_I_FlashCursor, gFlashCursor = IsOn("flcu"));
	gPrefs->SetPrefInt(prf_I_SmartBraces, gSmartBrace = IsOn("smbr"));
	gPrefs->SetPrefInt(prf_I_AltHome, IsOn("alho"));
	gPrefs->SetPrefInt(prf_I_ShowTabs, IsOn("stab"));
	gPrefs->SetPrefInt(prf_I_WindowToWorkspace, IsOn("smwo"));

	gPrefs->SetPrefInt(prf_I_DiffCase, IsOn("dica"));
	gPrefs->SetPrefInt(prf_I_DiffWhite, IsOn("diwh"));

	gPrefs->SetPrefInt(prf_I_SoftWrap, IsOn("sowr"));

	if (IsOn("wrwi"))
		gPrefs->SetPrefInt(prf_I_WrapType, 1);
	else if (IsOn("wrpa"))
		gPrefs->SetPrefInt(prf_I_WrapType, 2);
	else
		gPrefs->SetPrefInt(prf_I_WrapType, 3);

	gPrefs->SetPrefInt(prf_I_WrapCol, atoi(GetText("wrft")));

	gPrefs->SetPrefInt(prf_I_Includes, gPopupIncludes = IsOn("shin"));
	gPrefs->SetPrefInt(prf_I_Protos, gPopupProtos = IsOn("shpr"));
	gPrefs->SetPrefInt(prf_I_Types, gPopupFuncs = IsOn("shty"));
	gPrefs->SetPrefInt(prf_I_SortPopup, IsOn("sopo"));

	if (IsOn("swne"))
		gPrefs->SetPrefInt(prf_I_Startup, 1);
	else if (IsOn("swop"))
		gPrefs->SetPrefInt(prf_I_Startup, 2);
	else
		gPrefs->SetPrefInt(prf_I_Startup, 3);

	gPrefs->SetPrefInt(prf_I_RedirectStdErr, gRedirectStdErr = IsOn("rdse"));
	gPrefs->SetPrefInt(prf_I_Worksheet, gUseWorksheet = IsOn("wosh"));

	int cookie = 0;
	CLanguageInterface *intf;
	while ((intf = CLanguageInterface::NextIntf(cookie)) != NULL)
		intf->SetExtensions(fSuffixes[cookie - 1].c_str());

	item = fDefLanguage->FindMarked();
	gPrefs->SetPrefString(prf_S_DefLang, item ? item->Label() : "None");
	CLanguageInterface::ChooseDefault();

	gPrefs->SetPrefInt(prf_I_RestorePosition, gRestorePosition = IsOn("repo"));
	gPrefs->SetPrefInt(prf_I_RestoreFont, gRestoreFont = IsOn("refo"));
	gPrefs->SetPrefInt(prf_I_RestoreSelection, gRestoreSelection = IsOn("rese"));
	gPrefs->SetPrefInt(prf_I_RestoreScrollbar, gRestoreScrollbar = IsOn("resc"));
	gPrefs->SetPrefInt(prf_I_RestoreCwd, gRestoreCWD = IsOn("recw"));

	gPrefs->SetPrefInt(prf_I_SavedState, gSavedState = fState->IndexOf(fState->FindMarked()));

	gPrefs->SetPrefInt(prf_I_RecentSize, gRecentBufferSize = atoi(GetText("resi")));
	gPrefs->SetPrefInt(prf_I_SkipTmp, IsOn("sktm"));
	gPrefs->SetPrefInt(prf_I_ZoomOpen, IsOn("zoop"));
	gPrefs->SetPrefInt(prf_I_FullPath, IsOn("fupa"));

	gPrefs->SetPrefInt(prf_I_Parent, IsOn("sepa"));
	gPrefs->SetPrefInt(prf_I_BeIncludes, IsOn("incl"));

	gPrefs->SetPrefInt(prf_I_Backup, IsOn("maba"));
	gPrefs->SetPrefInt(prf_I_NlAtEof, IsOn("fonl"));
	gPrefs->SetPrefInt(prf_I_Verify, IsOn("vofm"));

	gPrefs->SetPrefInt(prf_I_IdeMenu, IsOn("idmn"));
	gPrefs->SetPrefInt(prf_I_MwPlugins, IsOn("mwpl"));

	gPrefs->SetPrefInt(prf_I_PassiveFtp, IsOn("pftp"));

	gPrefs->SetPrefInt(prf_I_ShowGlossary, IsOn("shgl"));
	gPrefs->SetPrefInt(prf_I_SingleClickGlossary, IsOn("sigl"));

	gPrefs->SetPrefInt(prf_I_ShowHtmlPalette, IsOn("shtp"));
	gPrefs->SetPrefInt(prf_I_ShowHtmlpaletteForHtml, IsOn("shtP"));

	gPrefs->SetPrefInt(prf_I_IsearchIgncase, IsOn("isic"));
	gPrefs->SetPrefInt(prf_I_CenterFound, IsOn("cesf"));

	gPrefs->SetPrefInt(prf_I_RelativeGroupPaths, IsOn("rlgp"));
	gPrefs->SetPrefInt(prf_I_SortGroup, IsOn("sogr"));

	gPrefs->SetPrefInt(prf_I_SortProject, IsOn("prso"));
	gPrefs->SetPrefInt(prf_I_AutodetectProjects, IsOn("prad"));

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
} /* CPrefsDialog::OkClicked */

void CPrefsDialog::SetColor(const char* const viewName, int colorId)
{
	HColorControl *cc;
	if ((cc = dynamic_cast<HColorControl*>(FindView(viewName))))
		cc->SetColor(gColor[colorId]);
} /* CPrefsDialog::SetColor */

void CPrefsDialog::GetColor(const char* const viewName, int colorId, const char* const prefName)
{
	HColorControl *cc;
	if ((cc = dynamic_cast<HColorControl*>(FindView(viewName))))
	{
		gColor[colorId] = cc->Color();
		gPrefs->SetPrefColor(prefName, gColor[colorId]);
	}
} /* CPrefsDialog::GetColor */

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
	sprintf(s, "%d", gPrefs->GetPrefInt(prf_I_FontSize, 9));
	SetText("fons", s);

	sprintf(s, "%d", gPrefs->GetPrefInt(prf_I_AltFontSize, 10));
	SetText("alfs", s);

//	if (fPageSetup) free(fPageSetup);
//	fPageSetup = (char *)Hex2Bin(gPrefs->GetPrefString(prf_S_DefaultPageSetup, ""), fPageSetupSize);

	SetColor("lowc", kColorLow);
	SetColor("txtc", kColorText);
	SetColor("selc", kColorSelection);
	SetColor("mrkc", kColorMark);
	SetColor("keyc", kColorKeyword1);
	SetColor("ke2c", kColorKeyword2);
	SetColor("cmtc", kColorComment1);
	SetColor("cm2c", kColorComment2);
	SetColor("strc", kColorString1);
	SetColor("tgsc", kColorString2);
	SetColor("numc", kColorNumber1);
	SetColor("nu2c", kColorNumber2);
	SetColor("opec", kColorOperator1);
	SetColor("op2c", kColorOperator2);
	SetColor("sepc", kColorSeparator1);
	SetColor("se2c", kColorSeparator2);
	SetColor("proc", kColorPreprocessor1);
	SetColor("pr2c", kColorPreprocessor2);
	SetColor("errc", kColorError1);
	SetColor("er2c", kColorError2);
	SetColor("sidc", kColorIdentifierSystem);
	SetColor("chcc", kColorCharConst);
	SetColor("uidc", kColorIdentifierUser);
	SetColor("tagc", kColorTag);
	SetColor("attr", kColorAttribute);
	SetColor("us1c", kColorUserSet1);
	SetColor("us2c", kColorUserSet2);
	SetColor("us3c", kColorUserSet3);
	SetColor("us4c", kColorUserSet4);
	SetColor("invc", kColorInvisibles);

	SetOn("auin", gPrefs->GetPrefInt(prf_I_AutoIndent, 1));
	SetOn("syco", gPrefs->GetPrefInt(prf_I_SyntaxColoring, 1));
	SetOn("baty", gPrefs->GetPrefInt(prf_I_Balance, 1));
	SetOn("blcu", gPrefs->GetPrefInt(prf_I_BlockCursor, 0));
	SetOn("flcu", gPrefs->GetPrefInt(prf_I_FlashCursor, 1));
	SetOn("smbr", gPrefs->GetPrefInt(prf_I_SmartBraces, 1));
	SetOn("alho", gPrefs->GetPrefInt(prf_I_AltHome, 0));
	SetOn("stab", gPrefs->GetPrefInt(prf_I_ShowTabs, 1));
	SetOn("smwo", gPrefs->GetPrefInt(prf_I_WindowToWorkspace, 1));

	sprintf(s, "%d", gSpacesPerTab);
	SetText("tabs", s);

	SetOn("dica", gPrefs->GetPrefInt(prf_I_DiffCase, false));
	SetOn("diwh", gPrefs->GetPrefInt(prf_I_DiffWhite, false));

	bool sw;
	SetOn("sowr", sw = gPrefs->GetPrefInt(prf_I_SoftWrap, false));

	int wrap = gPrefs->GetPrefInt(prf_I_WrapType, 3);
	switch (wrap)
	{
		case 1:		SetOn("wrwi", true); break;
		case 2:		SetOn("wrpa", true); break;
		default:	SetOn("wrfi", true); break;
	}

	sprintf(s, "%d", gPrefs->GetPrefInt(prf_I_WrapCol, 80));
	SetText("wrft", s);

	SetEnabled("wrft", /*sw && */wrap == 3);

	SetText("stca", gTabChar);
	SetText("ssca", gSpaceChar);
	SetText("srca", gReturnChar);
	SetText("scca", gControlChar);

	SetOn("shin", gPrefs->GetPrefInt(prf_I_Includes, 1));
	SetOn("shpr", gPrefs->GetPrefInt(prf_I_Protos, 0));
	SetOn("shty", gPrefs->GetPrefInt(prf_I_Types, 1));
	SetOn("sopo", gPrefs->GetPrefInt(prf_I_SortPopup, 0));

	switch (gPrefs->GetPrefInt(prf_I_Startup))
	{
		default: SetOn("swne", true); break;
		case 2: SetOn("swop", true); break;
		case 3: SetOn("swno", true); break;
	}

	SetOn("rdse", gPrefs->GetPrefInt(prf_I_RedirectStdErr, 1));
	SetOn("wosh", gPrefs->GetPrefInt(prf_I_Worksheet, 1));

	fSuffixes.erase(fSuffixes.begin(), fSuffixes.end());

	CLanguageInterface *intf;
	int cookie = 0, i = 0;
	const char *defLang = gPrefs->GetPrefString(prf_S_DefLang, "None");

	while ((intf = CLanguageInterface::NextIntf(cookie)) != NULL)
	{
		fSuffixes.push_back(intf->Extensions());
		if (strcmp(intf->Name(), defLang) == 0)
			SetValue("dela", i + 3);
		i++;
	}

	SetText("sufx", fSuffixes.size() ? fSuffixes[GetValue("lang") - 1].c_str() : "");

	SetOn("repo", gPrefs->GetPrefInt(prf_I_RestorePosition, 1));
	SetOn("refo", gPrefs->GetPrefInt(prf_I_RestoreFont, 1));
	SetOn("rese", gPrefs->GetPrefInt(prf_I_RestoreSelection, 1));
	SetOn("resc", gPrefs->GetPrefInt(prf_I_RestoreScrollbar, 1));
	SetOn("recw", gPrefs->GetPrefInt(prf_I_RestoreCwd, 1));

	sprintf(s, "%d", gRecentBufferSize);
	SetText("resi", s);
	SetOn("sktm", gPrefs->GetPrefInt(prf_I_SkipTmp, 1));
	SetOn("zoop", gPrefs->GetPrefInt(prf_I_ZoomOpen, 0));
	SetOn("fupa", gPrefs->GetPrefInt(prf_I_FullPath, 1));

	SetOn("sepa", gPrefs->GetPrefInt(prf_I_Parent, 1));
	SetOn("incl", gPrefs->GetPrefInt(prf_I_BeIncludes, 1));

	SetOn("maba", gPrefs->GetPrefInt(prf_I_Backup, 0));
	SetOn("fonl", gPrefs->GetPrefInt(prf_I_NlAtEof, 1));
	SetOn("vofm", gPrefs->GetPrefInt(prf_I_Verify, 1));

	SetOn("idmn", gPrefs->GetPrefInt(prf_I_IdeMenu, 1));
	SetOn("mwpl", gPrefs->GetPrefInt(prf_I_MwPlugins, 1));

	SetOn("pftp", gPrefs->GetPrefInt(prf_I_PassiveFtp, 1));

	SetOn("shgl", gPrefs->GetPrefInt(prf_I_ShowGlossary, 0));
	SetOn("sigl", gPrefs->GetPrefInt(prf_I_SingleClickGlossary, 0));

	SetOn("shtp", gPrefs->GetPrefInt(prf_I_ShowHtmlPalette, 1));
	SetOn("shtP", gPrefs->GetPrefInt(prf_I_ShowHtmlpaletteForHtml, 1));

	SetEnabled("shtP", IsOn("shtp"));

	SetOn("isic", gPrefs->GetPrefInt(prf_I_IsearchIgncase, 1));
	SetOn("cesf", gPrefs->GetPrefInt(prf_I_CenterFound, 0));

	SetOn("sogr", gPrefs->GetPrefInt(prf_I_SortGroup, 1));
	SetOn("rlgp", gPrefs->GetPrefInt(prf_I_RelativeGroupPaths, 0));

	SetOn("prso", gPrefs->GetPrefInt(prf_I_SortProject, 1));
	SetOn("prad", gPrefs->GetPrefInt(prf_I_AutodetectProjects, 1));

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
				CLanguageInterface *intf = NULL;
				while ((intf = CLanguageInterface::NextIntf(cookie)) != NULL) {
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
	int resID = rid_Cmnd_Editing;

	while (true)
	{
		long cnt, cmd;
		char s[256];
		const char *name;

		size_t size;
		const void *p = HResources::GetResource(rtyp_Cmnd, resID, size, &name);
		if (p == NULL)
			break;

		BMemoryIO buf(p, size);
		buf >> cnt;

		MyItem *mom = new MyItem(name, 0);
		fKBCommands->AddItem(mom);

		if (resID == rid_Cmnd_Extensions)
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

void CPrefsDialog::Show()
{
	HDialog::Show();
	if (LockLooper())
	{
		HTabSheet* tabBook = (HTabSheet*)FindView("tabB");
		BRect okFrame = FindView("ok  ")->Frame();
		if (tabBook) {
			BPoint bottomRight = tabBook->AdjustBottomRightOfAllPanes();
			if (bottomRight.x < okFrame.right)
				bottomRight.x = okFrame.right;
			if (bottomRight.y < okFrame.bottom)
				bottomRight.y = okFrame.bottom;
			ResizeTo(bottomRight.x+5, bottomRight.y+5);
		}
		UnlockLooper();
	}
} /* CPrefsDialog::Show */
