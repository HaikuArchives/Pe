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

#include "CProjectFile.h"

CProjectItem::CProjectItem()
{
}

CProjectItem::CProjectItem(const char* path, const char* leafName)
	:	fParentPath(path)
	,	fLeafName(leafName)
{
}

CProjectItem::~CProjectItem()
{
}

status_t CProjectItem::SetTo(const char* path, const char* leafName)
{
	fParentPath.SetTo(path);
	fLeafName.SetTo(leafName);
	return B_OK;
}

status_t CProjectItem::SetTo(const BPath& path)
{
	BPath parentPath;
	path.GetParent(&parentPath);
	return SetTo(parentPath.Path(), path.Leaf());
}



CProjectGroupItem::CProjectGroupItem()
{
}

CProjectGroupItem::CProjectGroupItem(const char* path, const char* leafName,
												 const char* groupComment)
	:	inherited(path, leafName)
	,	fGroupComment(groupComment)
{
}

CProjectGroupItem::~CProjectGroupItem()
{
}

void CProjectGroupItem::RemoveItem(CProjectItem* item)
{
	list<CProjectItem*>::iterator iter;
	for( iter = fItems.begin(); iter != fItems.end(); ) {
		CProjectItem* child = *iter;
		CProjectGroupItem* groupChild = dynamic_cast<CProjectGroupItem*>(child);
		if (child == item) {
			if (groupChild) {
				list<CProjectItem*>::iterator subiter;
				for( subiter = groupChild->fItems.begin(); 
					  subiter != groupChild->fItems.end(); ++subiter)
					groupChild->RemoveItem(*subiter);
			}
			++iter;
			fItems.remove(child);
			delete child;
		} else {
			++iter;
			if (groupChild)
				groupChild->RemoveItem(item);
		}
	}
}



CProjectFile::CProjectFile()
{
}

CProjectFile::~CProjectFile()
{
}

