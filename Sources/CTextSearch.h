/*	$Id: $
	
	Copyright 2008 Oliver Tappe <zooey@hirschkaefer.de>
	
	Distributed under the MIT License
*/

#ifndef CTEXTSEARCH_H
#define CTEXTSEARCH_H

class CTextBuffer;

class CTextSearch {
public:
		CTextSearch(CTextBuffer* buffer);
		~CTextSearch();

private:
		CTextBuffer* fTextBuffer;
};

#endif // CTEXTSEARCH_H
