/*	$Id$
	
	Copyright 2005 Oliver Tappe
	
	Distributed under the MIT License
*/

/*
 * CProjectJamFile implements the CProjectFile-interface for Jamfiles.
 *
 * In a file with mimetype 'text/x-jamfile', Pe searches for special
 * (XML-like) markup comments:
 *		# <pe-src>
 * 	# </pe-src>
 * and interpretes anything between those comments as a list of named
 * groups of source-files. Instead of opening such files as text, Pe
 * opens a project-window, containing all source-files found.
 * This project-window can then be used to easily open the sources, and
 * add/remove sources, too.
 *
 * Here's an example of a relevant Jamfile fragment:
 *
 * 	# <pe-src>
 * 	Application TestApp :  TestSource1.cpp TestSource2.cpp : be	;
 * 	# </pe-src>
 *
 */

#include "pe.h"
#include "Utils.h"

#include <cctype>

#include <Entry.h>

#include "CProjectJamFile.h"

#include "HPreferences.h"

/*
 * CProjectJamSerializer
 *		a struct that implements the serialization of Jamfile-items.
 */
struct CProjectJamSerializer : public CProjectSerializer
{
	CProjectJamSerializer( BString& result, const char* startPath);
	virtual ~CProjectJamSerializer();
	virtual void SerializeItem(const CProjectItem* item);
	virtual void SerializeGroupItem(const CProjectGroupItem* item);
	virtual void SerializeFile(const CProjectFile* item);
	BString& fResult;
	BPath fStartPath;
};

CProjectJamSerializer::CProjectJamSerializer( BString& result, 
															 const char* startPath)
	:	fResult(result)
	,	fStartPath(startPath)
{
}

CProjectJamSerializer::~CProjectJamSerializer()
{
}

void CProjectJamSerializer::SerializeItem(const CProjectItem* item)
{
	char path[PATH_MAX];
	BPath itemPath( item->ParentPath().String(), item->LeafName().String());
	RelativePath(fStartPath, itemPath, path);
	const char *p = (path[0]=='.' && path[1]=='/') ? path+2 : path;
	if (strchr(p,' '))
		fResult << "\t" << '"' << p << '"' << "\n";
	else
		fResult << "\t" << p << "\n";
}

void CProjectJamSerializer::SerializeGroupItem(const CProjectGroupItem* item)
{
	fResult << item->GroupHeader() << item->LeafName();
	if (item->GroupName().Length())
		fResult << ' ' << item->GroupName() << " : ";
	fResult << '\n';
	list<CProjectItem*>::const_iterator iter;
	for( iter = item->begin(); iter != item->end(); ++iter)
		(*iter)->SerializeTo(this);
	fResult << item->GroupFooter();
}

void CProjectJamSerializer::SerializeFile(const CProjectFile* item)
{
	// each CProjectFile lives in a file of its own right, so we write
	// this item to its own file:
	if (item->HasBeenParsed())
		item->SerializeToFile(NULL);
}



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



CProjectJamFile::CProjectJamFile()
	:	fHaveProjectInfo(false)
	,	fHasBeenParsed(false)
{
}

CProjectJamFile::CProjectJamFile(const char* path)
	:	fHaveProjectInfo(false)
	,	fHasBeenParsed(false)
{
	SetTo(path);
}

CProjectJamFile::~CProjectJamFile()
{
}

const char* CProjectJamFile::_ParseJamPath( const char* t, BString& jamPath)
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
	// now replace jam-top varname with real jam-top:
	BString var;
	var << fJamTopVarName << "/";
	jamPath.ReplaceFirst(var.String(), fJamTopPath.String());
	// now replace jam-top varname-expression with real jam-top:
	var.Truncate(0);
	var << "$(" << fJamTopVarName << ")/";
	jamPath.ReplaceFirst(var.String(), fJamTopPath.String());

	return t;
}

/* 
 * _AddGroup()
 *		Tries to extract a group from between the given string-pointers.
 *		A group is expected to have this structure:
 *			<rule> [<group-name> :] (<entry-name>)+ [: (<anything>)+ ] ;
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
		
		if (*t == ':') {
			t++;
			group	= new CProjectGroupItem(fParentPath.String(), rule.String(),
													groupName.String());
		} else {
			// no groupName available:
			group	= new CProjectGroupItem(fParentPath.String(), rule.String());
			// what we have in groupName is in fact the first item, so we add it:
			group->AddItem(new CProjectItem(fParentPath.String(), 
													  groupName.String()),
								gPrefs->GetPrefInt("sortproject", 1) != 0);
		}
		
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
	// we use our own format, since this gives us the chance to implement
	// other domain-declarations, too (like e.g. <pe-inc> for include-paths):
	while (1)
	{
		groupStart = strstr(t, "# <pe-src>");
		groupEnd = groupStart ? strstr(groupStart, "# </pe-src>") : NULL;
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
	t = _ParseJamPath(t, aJamPath);
	if (!aJamPath.Length())
		return false;
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
	const char* inclStart;
	const char* inclEnd;
	const char* t = contents.String();
	while (1)
	{
		inclStart = strstr(t, "# <pe-inc>");
		inclEnd = inclStart ? strstr(inclStart, "# </pe-inc>") : NULL;
		if (!(inclStart < inclEnd))
			break;

		bool ok = _ParseIncludeBlock(inclStart, inclEnd);
		if (!ok)
			break;
		t = inclEnd;
	}
}

const char* CProjectJamFile::_ParseSubJamfile(const char* t)
{
	t = skip_white(t);

	BString aJamPath;
	t = _ParseJamPath(t, aJamPath);
	if (!aJamPath.Length())
		return NULL;
	BString leaf;
	int32 slashPos = aJamPath.FindLast('/');
	if (slashPos < 0)
		leaf.SetTo(aJamPath);
	else
		leaf.SetTo(aJamPath.String()+slashPos+1);
	aJamPath << "/Jamfile";
	BString dspName;
	dspName << "<" << leaf << ">";
	CProjectJamFile* subPrj = new CProjectJamFile(aJamPath.String());
	subPrj->DisplayName(dspName);
	AddItem(subPrj, false);
	
	t = skip_white(t);
	if (*t == ';') {
		t++;
		return t;
	} else
		return NULL;
}

void CProjectJamFile::_ParseSubJamfiles(const BString& contents)
{
	const char* subJamStart;
	const char* t = contents.String();
	while (t)
	{
		subJamStart = strstr(t, "SubInclude ");
		if (!subJamStart)
			break;
		t = _ParseSubJamfile(subJamStart+11);
		fHaveProjectInfo = true;
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
	
	const char* t = contents.String();
	const char* topStart = strstr(t, "SubDir ");
	if (!topStart)
		return B_NO_INIT;
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
	t = _ParseJamPath(t, localJamPath);
	int32 pos = fParentPath.FindLast(localJamPath);
	if (pos < B_OK)
		return B_NO_INIT;
	if (pos)
		fJamTopPath.SetTo(fParentPath.String(), pos);
	else
		fJamTopPath << fParentPath << '/';
	// now fJamTopPath should contain the topmost path of the jamfile hierarchy,
	// such that we can use it to resolve any include paths (which are relative
	// to that top)

	_ParseSources(contents);
	_ParseIncludes(contents);
	_ParseSubJamfiles(contents);

	fHasBeenParsed = true;

	return B_OK;
}

status_t CProjectJamFile::SerializeToFile(BPositionIO* file) const
{
	status_t res = B_OK;
	BString contents(fHeader);

	BPath path( fParentPath.String(), fLeafName.String());
	CProjectJamSerializer serializer(contents, path.Path());

	// need to find out now if dirty, since serialization resets the info!
	bool isDirty = IsDirty();

	list<CProjectItem*>::const_iterator iter;
	for( iter = fItems.begin(); iter != fItems.end(); ++iter)
		(*iter)->SerializeTo(&serializer);

	contents << fFooter;

	if (file || isDirty)
		res = WriteToFile(file, contents, "text/x-jamfile");

	return res;
}		
