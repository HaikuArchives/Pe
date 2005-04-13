/*	$Id$
	
	Copyright 2005 Oliver Tappe
	
	Distributed under the MIT License
*/

/*
 * CProjectJamFile implements the CProjectFile-interface for Jamfiles.
 *
 * In a file with mimetype 'text/x-jamfile', Pe searches for special
 * markup comments:
 *		# @src->@
 * 	# @<-src@
 * and interpretes anything between those comments as a list of named
 * groups of source-files. Instead of opening such files as text, Pe
 * opens a project-window, containing all source-files found.
 * This project-window can then be used to easily open the sources, and
 * add/remove sources, too.
 *
 * Here's an example of a relevant Jamfile fragment:
 *
 * 	# @src->@
 * 	Application TestApp :  TestSource1.cpp TestSource2.cpp : be	;
 * 	# @<-src@
 *
 */

#include "pe.h"
#include "Utils.h"

#include <cctype>

#include <Entry.h>

#include "CProjectJamFile.h"

#include "HPreferences.h"

static const char *skip_to(const char *t, char c)
{
	while (*t && *t != c)
		t++;
	return t;
} // skip_to

static const char *skip_white(const char *t, bool multiline=true)
{
	while (*t)
	{
		if (*t == ' ' || *t == '\t' || (multiline && *t == '\n'))
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

const char* get_jam_path( const char* t, BString& jamPath)
{
	const char *p;
	int pl;
	jamPath.Truncate(0);
	t = skip_white(t);
	while(*t && *t != ';') {
		t = next_path(t, p, pl);
		if (jamPath.Length())
			jamPath << "/";
		jamPath << BString(p, pl);
		t = skip_white(t);
	}
	return t;
}



CProjectJamFile::CProjectJamFile()
	:	fHaveProjectInfo(false)
{
}

CProjectJamFile::CProjectJamFile(const BPath& path)
	:	fHaveProjectInfo(false)
{
	SetTo(path);
}

CProjectJamFile::~CProjectJamFile()
{
}

/* 
 * _AddGroup()
 *		Tries to extract a group from between the given string-pointers.
 *		A group is expected to have this structure:
 *			<rule> <group-name> : (<entry-name>)+ [: (<anything>)+ ] ;
 *		Whitespace inside of a group doesn't matter
 */
CProjectGroupItem *CProjectJamFile::_AddGroup(const char* s, const char* e, 
															 const char* buftop)
{
	const char* groupEnd = NULL;
	const char* t = s;

	const char *p;
	int pl;

	t = skip_white(t);

	BString groupHeader(buftop, t-buftop);
		
	t = next_path(t, p, pl);
	if (!p)
		return NULL;
	BString rule(p, pl);						// e.g. Application

	t = skip_white(t);

	CProjectGroupItem *group = NULL;
	
	if (isalnum(*t))
	{
		const char *n = t;

		t = next_path(t, p, pl);
		if (!p)
			return NULL;
		
		int nl = t - n;
		BString groupName(n, nl);
		
		t = skip_white(t);
		
		if (*t != ':')
			return NULL;

		t++;
		
		if (rule.Length()) {
			rule << ' ';
			groupName.Prepend(rule);
		}
		group	= new CProjectGroupItem(fParentPath.String(), groupName.String());
		group->GroupHeader(groupHeader);
		fItems.push_back(group);
		
		groupEnd = t;
		t = skip_white(t);

		while (*t && *t != ':' && *t != ';')
		{
			
			t = skip_white(t);
		
			t = next_path(t, p, pl);
			if (p == NULL)
				break;

			BString name(p, pl);
			if (name.Length())
				group->AddItem(new CProjectItem(fParentPath.String(), 
														  name.String()),
									gPrefs->GetPrefInt("sortproject", 1) != 0);
			
			t = skip_white(t, false);
			if (*t == '\n')
				t++;

			groupEnd = t;
			t = skip_white(t);
		}
	}

	if (groupEnd) {
		BString groupFooter(groupEnd, e-groupEnd);
		if (group)
			group->GroupFooter(groupFooter);
	}
	return group;
}

void CProjectJamFile::_ParseSources(const BString& contents)
{
	const char* groupStart;
	const char* groupEnd;
	const char* t = contents.String();
	// we use Eddie's format, since this gives us the chance to implement
	// other domain-declarations, too (like e.g. @inc->@ for include-paths):
	while (1)
	{
		groupStart = strstr(t, "# @src->@");
		groupEnd = groupStart ? strstr(groupStart, "# @<-src@") : NULL;
		if (!(groupStart < groupEnd))
			break;

		groupStart = skip_to(groupStart, '\n');
		if (*groupStart)
			groupStart++;
		
		CProjectGroupItem* group = _AddGroup(groupStart, groupEnd, t);
		if (!group)
			break;
		t = groupEnd;

		fHaveProjectInfo = true;
	}
	if (t<contents.String()+contents.Length()) {
		// remainder is main footer:
		fFooter.SetTo(t, contents.String()+contents.Length()-t);
	}
	if (fItems.empty()) {
		// TODO:
		// 	nothing found, maybe we should try to look out for some common
		// 	rules here (Application, SharedLibrary and StaticLibrary)?
	}
}

bool CProjectJamFile::_ParseIncludeStmt(const char*& t)
{
	const char* p;
	int pl;
	t = skip_white(t);

	t = next_path(t, p, pl);
	if (!p)
		return false;
	BString rule(p, pl);
	if (rule.ICompare("SubDirHdrs") && rule.ICompare("SubDirSysHdrs"))
		return false;

	t = skip_white(t);

	BString aJamPath;
	t = get_jam_path(t, aJamPath);
	if (!aJamPath.Length())
		return false;
	// now replace jam-top varname-expression with real jam-top:
	BString var;
	var << "$(" << fJamTopVarName << ")/";
	aJamPath.ReplaceFirst(var.String(), fJamTopPath.String());
printf("path = %s\n", aJamPath.String());
	_AddIncludePath(aJamPath);
	
	t = skip_white(t);
	if (*t == ';') {
		t++;
		return true;
	} else
		return false;
}

bool CProjectJamFile::_ParseIncludeBlock(const char* start, const char* end)
{
	const char* t = start;
	while(*t && t<end) {
		if (!_ParseIncludeStmt(t))
			return false;
		t = skip_white(t);
	}
	return true;
}

void CProjectJamFile::_ParseIncludes(const BString& contents)
{
	const char* t = contents.String();
	const char* topStart = strstr(t, "SubDir ");
	if (!topStart)
		return;
	t = skip_to(topStart, ' ');
	t = skip_white(t);

	// now fetch top-variable (usually 'TOP'):
	const char* p;
	int pl;
	t = next_path(t, p, pl);
	fJamTopVarName.SetTo(p, pl);
	t = skip_white(t);

	// remainder is local jam path (relative to jam-top):
	BString localJamPath;
	t = get_jam_path(t, localJamPath);
	int32 pos = fParentPath.FindLast(localJamPath);
	if (pos < B_OK)
		return;
	fJamTopPath.SetTo(fParentPath.String(), pos);
printf("topname = %s jamtop = %s\n", fJamTopVarName.String(), fJamTopPath.String());
	// now fJamTopPath should contain the topmost path of the jamfile hierarchy,
	// such that we can use it to resolve any include paths (which are relative
	// to that top)

	const char* inclStart;
	const char* inclEnd;
	while (1)
	{
		inclStart = strstr(t, "# @inc->@");
		inclEnd = inclStart ? strstr(inclStart, "# @<-inc@") : NULL;
		if (!(inclStart < inclEnd))
			break;

		bool ok = _ParseIncludeBlock(inclStart, inclEnd);
		if (!ok)
			break;
		t = inclEnd;
	}
}

status_t CProjectJamFile::Parse()
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
	char* buf = contents.LockBuffer(size+1);
	if (!buf)
		return B_NO_MEMORY;
	
	if (prjFile.Read(buf, size) != size)
		return B_IO_ERROR;

	contents.UnlockBuffer(size);
	
	_ParseSources(contents);
	_ParseIncludes(contents);

	return B_OK;
}

static void SerializeItemToString(CProjectItem* item, 
											 const BPath& startPath, BString& str)
{
	char path[PATH_MAX];
	BPath itemPath( item->ParentPath().String(), item->LeafName().String());
	RelativePath(startPath, itemPath, path);
	const char *p = (path[0]=='.' && path[1]=='/') ? path+2 : path;
	if (strchr(path, ' ') != NULL)
		str << "\t" << '"' << p << '"' << "\n";
	else
		str << "\t" << p << "\n";
}

static void SerializeGroupItemToString(CProjectGroupItem* item, 
													const BPath& startPath, BString& str)
{
	str << item->GroupHeader() << item->LeafName() << " : \n";
	list<CProjectItem*>::iterator iter;
	for( iter = item->begin(); iter != item->end(); ++iter)
		SerializeItemToString(*iter, startPath, str);
	str << item->GroupFooter();
}

status_t CProjectJamFile::WriteToFile( const char* mimetype)
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
