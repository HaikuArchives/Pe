//
// WebPaste Extension for Pe Editor:
// pastes the selected text online at http://rafb.net/.
//
// written by François Revol, 2008.
//

#include "PeAddOn.h"
#include <stdio.h>

#define PASTE_URL "http://rafb.net/paste/paste.php"
#define OPEN_IN_BROWSER 1
#define COPY_TO_CLIPBOARD 1

//------------------------------------------------------------------------------

#if 0
class StdDataIO : public BDataIO {
public:
	StdDataIO(FILE *f, bool closeOnDelete=true);
	virtual ~StdDataIO();
	virtual ssize_t Read(void *buffer, size_t size);
	virtual ssize_t Write(const void *buffer, size_t size);
private:
	FILE *fFile;
	bool fCloDelete;
}

StdDataIO::StdDataIO(FILE *f, bool closeOnDelete)
{
	fFile = f;
	fCloDelete = closeOnDelete;
}

StdDataIO::~StdDataIO()
{
	if (fCloDelete)
		fclose(fFile);
}

#endif

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

static const char *PeLangToPasteLang(const char *lang);
static status_t UrlEscape(BString &str);
status_t POpenOut(const char *cmd, BString &output);
static status_t RunWgetPaste(const char *language, const char *nick, const char *desc, const char *text);

//------------------------------------------------------------------------------

const char *
PeLangToPasteLang(const char *lang)
{
	BString l(lang);
	if (l == "C")
		return "C (99)";
	if (l == "C++")
		return "C++";
	if (l == "Java")
		return "Java";
	//XXX
	return "Plain Text";
	
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
RunWgetPaste(const char *language, const char *nick, const char *desc, const char *text)
{
	BString langStr(PeLangToPasteLang(language));
	BString nickStr(nick);
	BString descStr(desc);
	BString textStr(text);
	UrlEscape(langStr);
	UrlEscape(nickStr);
	UrlEscape(descStr);
	UrlEscape(textStr);
	BString postStr;
	postStr << "lang=" << langStr << "&";
	postStr << "nick=" << nickStr << "&";
	postStr << "desc=" << descStr << "&";
	postStr << "cvt_tabs=No" << "&";
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

	
/*
	BString message("Error running wget.\n");
	message << pasteUrl;
	MInfoAlert(message.String()).Go();
*/
	
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

	// Do NOT run the query if we have less than 3 chars. It works, but takes
	// ages and produces an useless giant popup menu.
	if (length < 3) {
		MIdeaAlert("The text selection is too short.\n"
					"Please select at least three characters.").Go();
		return B_ERROR;
	}

	BString selection;
	selection.SetTo(addon->Text() + selStart, length);
	
	BString description;
	if (addon->Window()) {
		description = addon->Window()->Title();
	}

	err = RunWgetPaste("C++", 
		"unknown", 
		description.String(), 
		selection.String());
	if (err >= B_OK)
		return B_OK;


	if (err == B_LAUNCH_FAILED) {
		MStopAlert("Can't run Wget!").Go();
		return B_OK;
	}
	
	BString message("Error running wget.");
	MInfoAlert(message.String()).Go();

	return B_OK;
}
