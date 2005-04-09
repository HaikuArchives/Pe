/*	$Id$
	
	Copyright 2005 Oliver Tappe
	
	Distributed under the MIT License
*/

/*
 * CProjectMakeFile implements the CProjectFile-interface for Makefiles.
 *
 * In a file with mimetype 'text/x-makefile', Pe searches for special
 * markup comments:
 *		#%{
 * 	#%}
 * and interpretes anything between those comments as a list of named
 * groups of source-files. Instead of opening such files as text, Pe treats
 * opens a project-window, containing all source-files found.
 * This project-window can then be used to easily open the sources, and
 * add/remove sources, too.
 *
 * Here's an example of a relevant Makefile fragment:
 *
 * 	#%{
 * 	SRCS = sourcefile1.cpp sourcefile2.cpp
 * 	RSRCS = resfile.rsrc
 * 	#%}
 * 
 * A second markup format (from Eddie) is support as well:
 *
 * 	# @src->@
 * 	SRCS = sourcefile1.cpp sourcefile2.cpp
 * 	RSRCS = resfile.rsrc
 * 	# @<-src@
 *
 * Complete examples can be found in the Be-provided sample code, e.g:
 * 	/boot/optional/sample-code/network_kit/FtpClient/makefile
 *
 */

#include "pe.h"
#include "Utils.h"

#include <cctype>

#include <Entry.h>
#include <Entry.h>

#include "CProjectMakeFile.h"

CProjectMakeFile::CProjectMakeFile()
	:	fHaveProjectInfo(false)
{
}

CProjectMakeFile::CProjectMakeFile(const BPath& path)
	:	fHaveProjectInfo(false)
{
	SetTo(path);
}

CProjectMakeFile::~CProjectMakeFile()
{
}

static const char *skip_to(const char *t, char c)
{
	while (*t && *t != c)
		t++;
	return t;
} // skip_to

static const char *skip_white(const char *t)
{
	while (*t)
	{
		if (*t == '\\' && t[1] == '\n')
			t += 2;
		if (*t == ' ' || *t == '\t')
			t++;
		else if (*t == '#')
			t = skip_to(t + 1, '\n');
		else
			break;
	}

	return t;
} // skip_white

static const char *next_path(const char *t, const char *& p, int& pl)
{
	if (*t == '"')
	{
		p = ++t;
		t = strchr(t, '"');
		if (t)
		{
			pl = t - p;
			t++;
		}
		else
			p = NULL;
	}
	else if (*t == '\'')
	{
		p = ++t;
		t = strchr(t, '\'');
		if (t)
		{
			pl = t - p;
			t++;
		}
		else
			p = NULL;
	}
	else if (*t)
	{
		p = t;
		while (*t && ! isspace(*t))
			t++;
		pl = t - p;
	}
	else
		p = NULL;
	
	return t;
} // next_path

static const char* groupStart;

const char *CProjectMakeFile::_AddGroup(const char *t)
{
	if (!groupStart)
		groupStart = t;
	t = skip_white(t);

	if (isalnum(*t))
	{
		const char *n = t;

		BString groupComment(groupStart, n-groupStart);

printf("groupComment = %s\n", groupComment.String());
		
		while (isalnum(*t) || *t == '_')
			t++;
		
		int nl = t - n;
		
		t = skip_white(t);
		
		if (*t != '=')
			return skip_to(t, '\n');

		t++;
		
		BString s(n, nl);

		CProjectGroupItem *group
			= new CProjectGroupItem(fParentPath.String(), s.String(), 
											groupComment.String());
		fItems.push_back(group);
		
		groupStart = NULL;

		t = skip_white(t);
		
		while (true)
		{
			const char *p;
			int pl;
			
			t = next_path(t, p, pl);
			if (p == NULL)
				break;

			s.SetTo(p, pl);

			entry_ref ref;
			BEntry e;

			if (s.Length())
				group->AddItem(new CProjectItem(fParentPath.String(), s.String()));
			
			t = skip_white(t);

			if (*t == '\n')
				break;
		}
	}

	if (*t)
	{
		t = skip_to(t, '\n');
		if (t) t++;
	}

	return t;
}

status_t CProjectMakeFile::Parse()
{
	BPath path( fParentPath.String(), fLeafName.String());
	BFile prjFile;
	status_t res = prjFile.SetTo(path.Path(), B_READ_ONLY);
	if (res != B_OK)
		return res;
	
	off_t size;
	res = prjFile.GetSize(&size);
	if (res != B_OK)
		return res;

	BString contents;
	char* t = contents.LockBuffer(size+1);
	if (!t)
		return B_NO_MEMORY;
	
	if (prjFile.Read(t, size) != size)
		return B_IO_ERROR;

	const char *s, *e;
	
	s = strstr(t, "#%{");
	if (s) {
		// Pe format
		e = s ? strstr(s, "#%}") : s;
	} else {
		// Eddie format
		s = strstr(t, "# @src->@");
		e = s ? strstr(s, "# @<-src@") : s;
	}
	
	if (s < e)
	{
		s = skip_to(s + 3, '\n') + 1;
		
		int l = s - t;
		fHeader.SetTo(t, l);
		
		l = t + size - e;
		fFooter.SetTo(e, l);
		
		while (s < e)
			s = _AddGroup(s);

		if (groupStart && s>groupStart)
			fFooter.Prepend(groupStart, s-groupStart);
	} else {
		fHeader.SetTo(t);
		fFooter.Truncate(0);
	}

	contents.UnlockBuffer(size);

	fHaveProjectInfo = true;

	return B_OK;
}

static void SerializeItemToString(CProjectItem* item, 
											 const BPath& startPath, BString& str)
{
	char path[PATH_MAX];
	BPath itemPath( item->ParentPath().String(), item->LeafName().String());
	RelativePath(startPath, itemPath, path);
	char quot = (strchr(path, ' ') != NULL ? '"' : ' ');
	if (path[0]=='.' && path[1]=='/')
		str << "\t" << quot << path+2 << quot << " \\\n";
	else
		str << "\t" << quot << path << quot << " \\\n";
}

static void SerializeGroupItemToString(CProjectGroupItem* item, 
													const BPath& startPath, BString& str)
{
	str << item->GroupComment() << item->LeafName() << " = \\\n";
	list<CProjectItem*>::iterator iter;
	for( iter = item->begin(); iter != item->end(); ++iter)
		SerializeItemToString(*iter, startPath, str);
	str << "\n";
}

status_t CProjectMakeFile::WriteToFile( const char* mimetype)
{
	BPath path( fParentPath.String(), fLeafName.String());
	BFile prjFile;
	status_t res = prjFile.SetTo(path.Path(), 
										  B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
	if (res != B_OK)
		return res;

	BNodeInfo nodeInfo(&prjFile);
	nodeInfo.SetType(mimetype);

	BString contents;
	contents 
		<< fHeader;

	list<CProjectItem*>::iterator iter;
	for( iter = fItems.begin(); iter != fItems.end(); ++iter) {
		CProjectGroupItem* groupItem = dynamic_cast<CProjectGroupItem*>(*iter);
		if (groupItem)
			SerializeGroupItemToString(groupItem, path, contents);
	}

	contents << fFooter;

	prjFile.Write(contents.String(), contents.Length());

	return B_OK;
}		
