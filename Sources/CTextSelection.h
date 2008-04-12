/*	$Id: $
	
	Copyright 2008 Oliver Tappe <zooey@hirschkaefer.de>
	
	Distributed under the MIT License
*/

#ifndef CTEXTSELECTION_H
#define CTEXTSELECTION_H

class CTextBuffer;
class CTextStructure;

class CTextSelection {
public:
		CTextSelection(CTextBuffer* buffer, CTextStructure* structure);
		~CTextSelection();

private:
		CTextBuffer* fTextBuffer;
		CTextStructure* fTextStructure;
};

#endif // CTEXTSELECTION_H
