//
// WebPaste Extension for Pe Editor:
// pastes the selected text online at http://rafb.net/.
//
// written by François Revol, 2008.
//

// TODO:
// Someone said http://pastebin.ca/ handles UTF-8 correctly (unline rafb.net)
// maybe later add configurable multiple handlers ?

#include "PeAddOn.h"
#include "PDoc.h"
#include "PText.h"
#include "CLanguageInterface.h"
#include <stdio.h>

#define PASTE_URL "http://rafb.net/paste/paste.php"
#define OPEN_IN_BROWSER 1
#define COPY_TO_CLIPBOARD 1

static struct {
	const char *pe_name;
	const char *cgi_name;
} kLanguageNamesMap[] = {
/*
	{ "Antlr",	""	},
	{ "ASM-x86",	""	},
	{ "D",	""	},
	{ "Diff",	""	},
	{ "HTML-CSS-JS-PHP",	""	},
	{ "HTML-JS",	""	},
	{ "HTML",	""	},
	{ "Jam",	""	},
	{ "Lout",	""	},
	{ "Lua",	""	},
	{ "Mathematica",	""	},
	{ "Oberon-2",	""	},
	{ "Rez",	""	},
	{ "Shell",	""	},
	{ "TeX",	""	},
*/
	//{ ,	"C89"	/*C (C89)*/ }
	//{ ,	"C"	/*C (C99)*/ },
	{ "C/C++",	"C++"	},
	//{ ,	"C#"	},
	{ "Java",	"Java"	},
	{ "Pascal",	"Pascal"	},
	{ "Perl",	"Perl"	},
	//{ ,	"PHP"	},
	//{ ,	"PL/I"	},
	{ "Python",	"Python"	},
	//{ ,	"Ruby"	},
	//{ ,	"Scheme"	},
	{ "SQL",	"SQL"	},
	//{ ,	"VB"	},
	//{ ,	"XML"	},
	/*{ ,	"Plain Text Wrap"	},
	{ ,	"Plain Text"	},*/
	{ NULL, NULL }
};

//------------------------------------------------------------------------------
//	#pragma mark - class StdIOInString

class StdIOInString : public BString {
public:
	StdIOInString(FILE *f, bool closeOnDelete=true);
	virtual ~StdIOInString();
private:
	FILE *fFile;
	bool fCloDelete;
};

StdIOInString::StdIOInString(FILE *f, bool closeOnDelete)
{
	fFile = f;
	fCloDelete = closeOnDelete;
	ssize_t len = 0;
	do {
		len = Length();
		char *buf = LockBuffer(len + 4096);
		memset(buf + len, 0, 4096);
		buf += len;
		int got = fread(buf, 1, 4096, fFile);
		//fprintf(stderr, "got %d\n", got);
		UnlockBuffer();
		if (got < 4096) {
			buf[got] = '\0';
			return;
		}
	} while (true);
}

StdIOInString::~StdIOInString()
{
	if (fCloDelete)
		fclose(fFile);
}


//------------------------------------------------------------------------------
//	#pragma mark - Helper functions

static const char *PeLangToPasteLang(const char *lang);
static status_t UrlEscape(BString &str);
status_t POpenOut(const char *cmd, BString &output);
static status_t RunWgetPaste(const char *language, const char *nick, const char *desc, int tabstops, const char *text);

//------------------------------------------------------------------------------

const char *
PeLangToPasteLang(const char *lang)
{
	int i;
	BString l(lang);
	for (i = 0; kLanguageNamesMap[i].pe_name; i++)
	{
		if (l == kLanguageNamesMap[i].pe_name)
			return kLanguageNamesMap[i].cgi_name;
	}
	return "Plain Text";
	//return "Plain Text Wrap";
}

status_t
UrlEscape(BString &str)
{
	// XXX: assumes UTF-8!
	for (int32 i = 0; i < str.Length(); i++) {
		if (str[i] >= '0' && str[i] < '9')
			continue;
		if (str[i] >= 'A' && str[i] < 'Z')
			continue;
		if (str[i] >= 'a' && str[i] < 'z')
			continue;
		uint8 c = (uint8)str[i];
		// seems those are byte-based, not UTF68 char-based
		char escape[5];
		//printf("%x\n", c);
		sprintf(escape, "%%%2.2x", c);
		str.Remove(i,1);
		str.Insert(escape, i);
		i+=2;
	}
	return B_OK;
}



status_t 
POpenOut(const char *cmd, BString &output)
{
	FILE *f;
	f = popen(cmd, "r");
	if (!f)
		return errno;
	StdIOInString out(f);
	output = out;
	return B_OK;
}


// sample post:
// wget --post-data="lang=C++&nick=mmu_man&desc=SamplePasting&cvt_tabs=No&text=int%20main()%20{}" http://rafb.net/paste/paste.php
// <td><small>URL: http://rafb.net/p/ngZGZU48.html</small></td>
// http://rafb.net/p/ngZGZU48.html

status_t
RunWgetPaste(const char *language, const char *nick, const char *desc, int tabstops, const char *text)
{
	BString langStr(PeLangToPasteLang(language));
	BString nickStr(nick);
	BString descStr(desc);
	BString textStr(text);
	BString tabsStr;
	nickStr.Truncate(30);
	descStr.Truncate(50);
	UrlEscape(langStr);
	UrlEscape(nickStr);
	UrlEscape(descStr);
	UrlEscape(textStr);
	if (tabstops)
		tabsStr << tabstops;
	else
		tabsStr << "No";
	BString postStr;
	postStr << "lang=" << langStr << "&";
	postStr << "nick=" << nickStr << "&";
	postStr << "desc=" << descStr << "&";
	postStr << "cvt_tabs=" << tabsStr << "&";
	postStr << "text=" << textStr;
	
	BString command("wget");
	command << " -O - "; // output to stdout
	//command << "--header=\"Content-Type: text/plain; charset=utf-8\" ";
	command << "--post-data=\"" << postStr << "\"";
	command << " ";
	command << PASTE_URL;
	
	BString result;
	status_t err;
	
	err = POpenOut(command.String(), result);
	// TEST
	/*
	err = B_OK;
	FILE *ftmp = fopen("/work/pe/trunk/xpTxSh16.html", "r");
	printf("FILE:%p\n", ftmp);
	StdIOInString tmp(ftmp);
	result = tmp;
	*/
	
	//fprintf(stderr, "error 0x%08lx\n", err);
	if (err < B_OK)
		return err;
	int32 urlIndex = result.FindFirst("URL: ");
	if (urlIndex < 0)
		return EIO;
	urlIndex += 5;
	int32 urlEnd = result.FindFirst('<', urlIndex);
	BString pasteUrl;
	if (urlEnd <= urlIndex)
		return EIO;
	result.CopyInto(pasteUrl, urlIndex, urlEnd - urlIndex);

	//printf("got %s\n", result.String());

#ifdef COPY_TO_CLIPBOARD
	if (be_clipboard->Lock()) {
		be_clipboard->Clear();
		BMessage *clip;
		clip = be_clipboard->Data();
		if (clip) {
			clip->AddData("text/plain", B_MIME_TYPE, pasteUrl.String(), pasteUrl.Length());
			clip->AddData("text/url", B_MIME_TYPE, pasteUrl.String(), pasteUrl.Length());
			clip->AddData("text/x-url", B_MIME_TYPE, pasteUrl.String(), pasteUrl.Length());
			be_clipboard->Commit();
		}
		be_clipboard->Unlock();
	}
#endif

#ifdef OPEN_IN_BROWSER
	char *args[2] = { NULL, NULL };
	args[0] = (char *)pasteUrl.String();
	be_roster->Launch(B_URL_HTTP, 1, args);
#endif

	return B_OK;
}


//------------------------------------------------------------------------------
//	#pragma mark - Addon Entry

#if __INTEL__
	extern "C" _EXPORT long perform_edit(MTextAddOn* addon);
#else
	#pragma export on
	extern "C" {
		long perform_edit(MTextAddOn* addon);
	}
	#pragma export reset
#endif


long
perform_edit(MTextAddOn* addon)
{
	status_t err;
	long selStart;
	long selEnd;

	addon->GetSelection(&selStart, &selEnd);

	if (selEnd <= selStart)
		return B_ERROR;

	int length = selEnd - selStart;

	// Do NOT paste anything if we have less than 5 chars.
	// It's probably unwanted action.
	if (length < 5) {
		MIdeaAlert("The text selection is too short.\n"
					"Please select at least five characters.").Go();
		return B_ERROR;
	}

	BString selection;
	selection.SetTo(addon->Text() + selStart, length);
	
	BString description;
	BString language("C++");
	BString nickname("unknown");
	int tabstops = 0;
	if (addon->Window())
	{
		description = addon->Window()->Title();
		PDoc *doc = dynamic_cast<PDoc *>(addon->Window());
		if (doc && doc->TextView())
		{
			CLanguageInterface *langintf = NULL;
			int lang = doc->TextView()->Language();
			if (lang > -1)
				langintf = CLanguageInterface::FindIntf(lang);
			if (langintf)
				language = langintf->Name();
			tabstops = doc->TextView()->TabStops();
		}
	}
	
	

	err = RunWgetPaste(language.String(), 
		nickname.String(), 
		description.String(), 
		tabstops,
		selection.String());
	if (err >= B_OK)
		return B_OK;


	if (err == B_LAUNCH_FAILED) {
		MStopAlert("Can't run wget!").Go();
		return B_OK;
	}
	
	BString message("Error running wget.");
	MInfoAlert(message.String()).Go();

	return B_OK;
}

