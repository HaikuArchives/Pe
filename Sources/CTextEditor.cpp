/*	$Id: $

	Copyright 2008 Oliver Tappe <zooey@hirschkaefer.de>
	
	Distributed under the MIT License
*/

#include "pe.h"

#include "CTextEditor.h"

#include "CTextBuffer.h"
#include "CTextStructure.h"
#include "CTextSelection.h"
#include "CTextSearch.h"

CTextEditor::CTextEditor()
	: fTextBuffer(new CTextBuffer)
	, fTextStructure(new CTextStructure(fTextBuffer))
	, fTextSelection(new CTextSelection(fTextBuffer, fTextStructure))
	, fTextSearch(new CTextSearch(fTextBuffer))
{
}

CTextEditor::~CTextEditor()
{
	delete fTextBuffer;
}

void CTextEditor::Insert(const char *bytes, int numBytes, int index)
{
	fTextBuffer->Insert(bytes, numBytes, index);
}

void CTextEditor::Delete(int from, int to)
{
	fTextBuffer->Delete(from, to);
}

void CTextEditor::Overwrite(int offset, const char *txt)
{
	fTextBuffer->Overwrite(offset, txt);
}

int CTextEditor::Size() const
{
	return fTextBuffer->Size();
}

const char* CTextEditor::Buffer()
{
	return fTextBuffer->Buffer();
}

char CTextEditor::operator[] (int indx) const
{
	return fTextBuffer->operator[](indx);
}

void CTextEditor::CopyText(char *buf, int index, int len) const
{
	fTextBuffer->CopyText(buf, index, len);
}

int CTextEditor::CharLen(int index) const
{
	return fTextBuffer->CharLen(index);
}

int CTextEditor::PrevCharLen(int index) const
{
	return fTextBuffer->PrevCharLen(index);
}

void CTextEditor::CharInfo(int offset, int& unicode, int& len) const
{
	fTextBuffer->CharInfo(offset, unicode, len);
}
