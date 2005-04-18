/*	$Id$
	
	Copyright 2005 Oliver Tappe
	
	Distributed under the MIT License
*/

/*
 * CProjectFile is an abstraction of Projects, i.e. a set of
 * named file-groups, each of which contains a set of files.
 * This abstraction is implemented to give support for
 * different kinds of "real world" project-files (i.e. Makefiles
 * and Jamfiles).
 * In other words: Pe supports parsing a Makefile/Jamfile and displaying
 * the relevant parts of it in a GUI (see PProjectWindow.cpp).
 * You can add/remove files, too (which will change the contents of
 * the Makefile/Jamfile accordingly).
 *
 * Concrete implementations of the CProjectFile interface live in 
 * CProjectMakeFile.* and CProjectJamFile.*.
 */


#include "pe.h"
#include "Utils.h"

#include <Autolock.h>

#include "CProjectFile.h"

/*
 * CProjectItem()
 */
CProjectItem::CProjectItem()
{
}

/*
 * 
 */
CProjectItem::CProjectItem(const char* path, const char* leafName)
	:	fParentPath(path)
	,	fLeafName(leafName)
{
}

/*
 * 
 */
CProjectItem::~CProjectItem()
{
}

/*
 * 
 */
status_t CProjectItem::SetTo(const char* path, const char* leafName)
{
	fParentPath.SetTo(path);
	fLeafName.SetTo(leafName);
	return B_OK;
}

/*
 * 
 */
status_t CProjectItem::SetTo(const char* path)
{
	BString fullPath(path);
	if (!fullPath.Length())
		return B_BAD_VALUE;
	int32 slashPos = fullPath.FindLast('/');
	if (slashPos < 0)
		return SetTo("", fullPath.String());
	BString parentPath(fullPath.String(), slashPos);
	return SetTo(parentPath.String(), fullPath.String()+slashPos+1);
}

/*
 * 
 */
void CProjectItem::SerializeTo( CProjectSerializer* serializer) const
{
	serializer->SerializeItem(this); 
}

/*
 * 
 */
const BString& CProjectItem::DisplayName() const
{
	if (fDisplayName.Length())
		return fDisplayName;
	else
		return fLeafName;
}



/*
 * 
 */
CProjectGroupItem::CProjectGroupItem()
{
}

/*
 * 
 */
CProjectGroupItem::CProjectGroupItem(const char* path, const char* leafName,
												 const char* groupName)
	:	inherited(path, leafName)
	,	fGroupName(groupName)
	,	fIsDirty(false)
{
	if (fGroupName.Length())
		// build display-name for a group that has a name specified:
		fDisplayName << fLeafName << ' ' << fGroupName;
}

/*
 * 
 */
CProjectGroupItem::~CProjectGroupItem()
{
	list<CProjectItem*>::iterator subiter;
	for( subiter = fItems.begin(); 
		  subiter != fItems.end(); ++subiter)
		delete *subiter;
}

/*
 * 
 */
bool CProjectGroupItem::ContainsFile(entry_ref* fileRef) const
{
	CProjectItem* child;
	BString fullPath;
	entry_ref eref;
	status_t res;
	list<CProjectItem*>::const_iterator iter;
	for( iter = fItems.begin(); iter != fItems.end(); ++iter) {
		child = *iter;
		fullPath.Truncate(0);
		fullPath << child->ParentPath() << '/' << child->LeafName();
		res = get_ref_for_path(fullPath.String(), &eref);
		if (res == B_OK && eref == *fileRef)
			return true;
		CProjectGroupItem* groupChild = dynamic_cast<CProjectGroupItem*>(child);
		if (groupChild && groupChild->ContainsFile(fileRef))
			return true;
	}
	return false;
}

/*
 * 
 */
int32 CProjectGroupItem::AddItem(CProjectItem* item, bool sorted)
{
	int32 pos = 0;
	fIsDirty = true;
	if (sorted) {
		list<CProjectItem*>::iterator iter;
		for( iter = fItems.begin(); iter != fItems.end(); ++iter, ++pos) {
			if (item->LeafName().ICompare((*iter)->LeafName()) < 0) {
				fItems.insert(iter, item);
				return pos;
			}
		}
	}
	pos = fItems.size();
	fItems.push_back(item);
	return pos;
}

/*
 * 
 */
void CProjectGroupItem::RemoveItem(CProjectItem* item)
{
	list<CProjectItem*>::iterator iter;
	for( iter = fItems.begin(); iter != fItems.end(); ) {
		CProjectItem* child = *iter;
		CProjectGroupItem* groupChild = dynamic_cast<CProjectGroupItem*>(child);
		if (child == item) {
			++iter;
			fItems.remove(child);
			delete child;
		} else {
			++iter;
			if (groupChild)
				groupChild->RemoveItem(item);
		}
	}
	fIsDirty = true;
}

/*
 * 
 */
void CProjectGroupItem::SerializeTo( CProjectSerializer* serializer) const
{
	serializer->SerializeGroupItem(this);
	fIsDirty = false;
}



/*
 * 
 */
CProjectFile::CProjectFile()
	:	fActivationTime(0)
{
	ProjectRoster->AddProject(this);
}

/*
 * 
 */
CProjectFile::~CProjectFile()
{
	ProjectRoster->RemoveProject(this);
}

/*
 * 
 */
void CProjectFile::GetIncludePaths(vector<BString>& inclPathVect) const
{
	inclPathVect.insert(inclPathVect.end(), fIncludePaths.begin(), 
							  fIncludePaths.end());
}

/*
 * 
 */
bool CProjectFile::IsDirty() const
{
	list<CProjectItem*>::const_iterator iter;
	for( iter = fItems.begin(); iter != fItems.end(); ++iter) {
		if ((*iter)->IsDirty())
			return true;
	}
	return false;
}

/*
 * 
 */
void CProjectFile::SerializeTo( CProjectSerializer* serializer) const
{
	serializer->SerializeFile(this); 
}

/*
 * 
 */
status_t 
CProjectFile::WriteToFile(BPositionIO* file, const BString& contents,
								  const char* mimetype) const
{
	BFile prjFile;
	if (!file) {
		BPath path( fParentPath.String(), fLeafName.String());
		status_t res 
			= prjFile.SetTo(path.Path(), 
								 B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
		if (res != B_OK)
			return res;

		BNodeInfo nodeInfo(&prjFile);
		nodeInfo.SetType(mimetype);
		file = &prjFile;
	}
	ssize_t sz = file->Write(contents.String(), contents.Length());
	if (sz != contents.Length())
		return sz < 0 ? sz : B_IO_ERROR;
	else
		return B_OK;
}




/*
 * 
 */
static CProjectRoster gProjectRoster;
CProjectRoster* ProjectRoster = &gProjectRoster;

CProjectRoster::CProjectRoster()
	:	fLocker("ProjectRosterLock")
{
}

/*
 * 
 */
void CProjectRoster::AddProject(CProjectFile* pf)
{
	BAutolock lock(&fLocker);
	fProjects.push_back(pf); 
}

/*
 * 
 */
void CProjectRoster::RemoveProject(CProjectFile* pf)
{
	BAutolock lock(&fLocker);
	fProjects.remove(pf); 
}

/* GetIncludePathsForFile()
 * 	tries to determine the project that contains the given file and,
 *		if such a project is found, passes back these projects include-paths.
 */
bool CProjectRoster::GetIncludePathsForFile(entry_ref* fileRef, 
														  vector<BString>& inclPathVect) const
{
	inclPathVect.clear();
	BAutolock lock(&fLocker);
	list<CProjectFile*>::const_iterator iter;
	for( iter=fProjects.begin(); iter != fProjects.end(); ++iter) {
		if ((*iter)->ContainsFile(fileRef)) {
			(*iter)->GetIncludePaths(inclPathVect);
			return true;
		}
	}
	return false;
}

/* GetAllIncludePaths()
 * 	returns the include-paths of all projects, sorted in descending
 *		order of the project-windows last activation time (i.e. projects
 *		have been used recently will be searched first).
 */
struct ProjectActivationTimeSorter {
	bool operator() (const CProjectFile* pfl, const CProjectFile* pfr) const {
		return pfl->ActivationTime() >= pfr->ActivationTime();
	}
};
bool CProjectRoster::GetAllIncludePaths(vector<BString>& inclPathVect)
{
	inclPathVect.clear();
	BAutolock lock(&fLocker);
	ProjectActivationTimeSorter compFunc;
	fProjects.sort(compFunc);
	list<CProjectFile*>::const_iterator iter;
	for( iter=fProjects.begin(); iter != fProjects.end(); ++iter) {
		(*iter)->GetIncludePaths(inclPathVect);
	}
	return true;
}
