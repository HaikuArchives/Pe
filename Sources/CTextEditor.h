/*	$Id: $
	
	Copyright 2008 Oliver Tappe <zooey@hirschkaefer.de>
	
	Distributed under the MIT License
*/

#ifndef CTEXTEDITOR_H
#define CTEXTEDITOR_H

class CTextBuffer;
class CTextStructure;
class CTextSelection;
class CTextSearch;

class CTextEditor {
public:
		CTextEditor();
		~CTextEditor();

		void Insert(const char *bytes, int numBytes, int index);
		void Delete(int from, int to);
		void Overwrite(int offset, const char *txt);

		char operator[] (int indx) const;
		
		int Size() const;
		const char* Buffer();	// TODO: wrap auto-locker around this!
		
		int CharLen(int offset) const;
		int PrevCharLen(int offset) const;
		void CharInfo(int offset, int& unicode, int& len) const;
		
		void CopyText(char *buf, int indx, int len) const;
		
private:
		CTextBuffer* fTextBuffer;
		CTextStructure* fTextStructure;
		CTextSelection* fTextSelection;
		CTextSearch* fTextSearch;
};

#endif // CTEXTEDITOR_H
