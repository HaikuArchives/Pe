/*	$Id$
	
	Copyright 2005 Oliver Tappe
	
	Distributed under the MIT License
*/

#ifndef CPROJECTMAKEFILE_H
#define CPROJECTMAKEFILE_H

#include "CProjectFile.h"

class CProjectMakeFile : public CProjectFile
{
	typedef CProjectFile inherited;
public:
	CProjectMakeFile();
	CProjectMakeFile(const BPath& path);
	virtual ~CProjectMakeFile();
	
	virtual status_t Parse();
	virtual bool HaveProjectInfo()		{ return fHaveProjectInfo; }
	virtual status_t WriteToFile(const char* mimetype);

private:
	const char* _AddGroup(const char* name);
	
	BString fHeader;
	BString fFooter;
	bool fHaveProjectInfo;
};

#endif
