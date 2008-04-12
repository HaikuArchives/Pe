/*	$Id: $
	
	Copyright 2008 Oliver Tappe <zooey@hirschkaefer.de>
	
	Distributed under the MIT License
*/

#ifndef CTEXTSTRUCTURE_H
#define CTEXTSTRUCTURE_H

class CTextBuffer;

class CTextStructure {
public:
		CTextStructure(CTextBuffer* buffer);
		~CTextStructure();

private:
		CTextBuffer* fTextBuffer;
};

#endif // CTEXTSTRUCTURE_H
