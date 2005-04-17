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
	CProjectMakeFile(const char* path);
	virtual ~CProjectMakeFile();
	
	virtual status_t Parse();
	virtual bool HasBeenParsed() const	{ return true; }
	virtual bool HaveProjectInfo() const
													{ return fHaveProjectInfo; }
	virtual status_t SerializeToFile(BPositionIO* file) const;

private:
	const char* _AddGroup(const char* name);
	
	BString fHeader;
	BString fFooter;
	bool fHaveProjectInfo;
};

#endif
