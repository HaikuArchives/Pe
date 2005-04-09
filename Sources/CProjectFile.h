/*	$Id$
	
	Copyright 2005 Oliver Tappe
	
	Distributed under the MIT License
*/

#ifndef CPROJECTFILE_H
#define CPROJECTFILE_H

#include <list>

#include <Path.h>
#include <String.h>

class BPositionIO;

class CProjectItem
{
public:
	CProjectItem();
	CProjectItem(const char* path, const char* leafName);
	virtual ~CProjectItem();
	//
	status_t SetTo(const char* path, const char* leafName);
	status_t SetTo(const BPath& path);
	//
	const BString& ParentPath() const	{ return fParentPath; }
	const BString& LeafName() const		{ return fLeafName; }

protected:
	BString fParentPath;
	BString fLeafName;
};


class CProjectGroupItem : public CProjectItem
{
	typedef CProjectItem inherited;
public:
	CProjectGroupItem();
	CProjectGroupItem(const char* path, const char* leafName, 
							const char* groupComment);
	virtual ~CProjectGroupItem();
	//
	list<CProjectItem*>::iterator begin()
													{ return fItems.begin(); }
	list<CProjectItem*>::iterator end()
													{ return fItems.end(); }
	void AddItem(CProjectItem* item)		{ fItems.push_back(item); }
	void RemoveItem(CProjectItem* item);
	//
	const BString& GroupComment() const	{ return fGroupComment; }

protected:
	list<CProjectItem*> fItems;
	BString fGroupComment;
};



class CProjectFile : public CProjectGroupItem
{
	typedef CProjectGroupItem inherited;
public:
	CProjectFile();
	virtual ~CProjectFile();
	
	virtual status_t Parse() = 0;
	virtual bool HaveProjectInfo() = 0;
	virtual status_t WriteToFile(const char* mimetype) = 0;

private:
};


#endif
