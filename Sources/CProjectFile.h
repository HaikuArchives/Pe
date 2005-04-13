/*	$Id$
	
	Copyright 2005 Oliver Tappe
	
	Distributed under the MIT License
*/

#ifndef CPROJECTFILE_H
#define CPROJECTFILE_H

#include <list>
#include <vector>

#include <Locker.h>
#include <Path.h>
#include <String.h>

class BPositionIO;

/*
 * CProjectItem
 *		represents a source-file in a project (displayed in a PProjectWindow).
 * 	It basically just stores the path of the parent and the leaf-name.
 */
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



/*
 * CProjectGroupItem
 *		represents a group inside a project; each group can have multiple
 *		items (sources). The group itself is typically the name of the jam 
 *		target that depends on the sources it contains
 */
class CProjectGroupItem : public CProjectItem
{
	typedef CProjectItem inherited;
public:
	CProjectGroupItem();
	CProjectGroupItem(const char* path, const char* leafName);
	virtual ~CProjectGroupItem();
	//
	list<CProjectItem*>::iterator begin()
													{ return fItems.begin(); }
	list<CProjectItem*>::iterator end()
													{ return fItems.end(); }
	int32 AddItem(CProjectItem* item, bool sorted);
	void RemoveItem(CProjectItem* item);
	//
	const BString& GroupHeader() const	{ return fGroupHeader; }
	const BString& GroupFooter() const	{ return fGroupFooter; }
	//
	void GroupHeader( const BString& gh)
													{ fGroupHeader = gh; }
	void GroupFooter( const BString& gf)
													{ fGroupFooter = gf; }
	//
	bool ContainsFile(entry_ref* fileRef);

protected:
	list<CProjectItem*> fItems;
	BString fGroupHeader;
	BString fGroupFooter;
};



/*
 * CProjectFile
 *		abstracts a project, i.e. a file-hierarchy that has been parsed from a
 *		Makefile/Jamfile. Files can be added/removed and the hierarchy can be 
 *		written back to the Makefile/Jamfile.
 *		Additionally, the notion of a list of include-paths is supported (which
 *		is taken from the Makefile/Jamfile, too).
 */
class CProjectFile : public CProjectGroupItem
{
	typedef CProjectGroupItem inherited;
public:
	CProjectFile();
	virtual ~CProjectFile();
	
	virtual status_t Parse() = 0;
	virtual bool HaveProjectInfo() = 0;
	virtual status_t WriteToFile(const char* mimetype) = 0;
	//
	time_t ActivationTime() const			{ return fActivationTime; }
	void ActivationTime(time_t at)		{ fActivationTime = at; }
	//
	void GetIncludePaths(vector<BString>& inclPathVect);
protected:
	void _AddIncludePath(const BString& inclPath)
													{ fIncludePaths.push_back(inclPath); }
private:
	vector<BString> fIncludePaths;
	time_t fActivationTime;
};



/*
 * CProjectRoster
 *		allows easy retrieval of currently open projects
 */
class CProjectRoster
{
public:
	CProjectRoster();
	//
	void AddProject(CProjectFile* pf);
	void RemoveProject(CProjectFile* pf);
	//
	bool GetIncludePathsForFile(entry_ref* fileRef, 
										 vector<BString>& inclPathVect);
	bool GetAllIncludePaths(vector<BString>& inclPathVect);

private:
	list<CProjectFile*> fProjects;
	BLocker fLocker;
};

extern CProjectRoster* ProjectRoster;


#endif
