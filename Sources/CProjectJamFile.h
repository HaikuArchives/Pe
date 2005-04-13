/*	$Id$
	
	Copyright 2005 Oliver Tappe
	
	Distributed under the MIT License
*/

#ifndef CPROJECTJAMFILE_H
#define CPROJECTJAMFILE_H

#include "CProjectFile.h"

class CProjectJamFile : public CProjectFile
{
	typedef CProjectFile inherited;
public:
	CProjectJamFile();
	CProjectJamFile(const BPath& path);
	virtual ~CProjectJamFile();
	
	virtual status_t Parse();
	virtual bool HaveProjectInfo()		{ return fHaveProjectInfo; }
	virtual status_t WriteToFile(const char* mimetype);

private:
	void _ParseSources(const BString& contents);
	bool _ParseIncludeStmt(const char*& t);
	bool _ParseIncludeBlock(const char* start, const char* end);
	void _ParseIncludes(const BString& contents);
	CProjectGroupItem* _AddGroup(const char* start, const char* end, 
										  const char* buftop);
	
	BString fHeader;
	BString fFooter;
	BString fJamTopVarName;
	BString fJamTopPath;
	bool fHaveProjectInfo;
};

#endif
