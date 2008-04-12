/*	$Id: $

	Copyright 2008 Oliver Tappe <zooey@hirschkaefer.de>
	
	Distributed under the MIT License
*/

#include "pe.h"

#include "CTextSelection.h"

#include "CTextBuffer.h"
#include "CTextStructure.h"

CTextSelection::CTextSelection(CTextBuffer* buffer, CTextStructure* structure)
	: fTextBuffer(buffer)
	, fTextStructure(structure)
{
}

CTextSelection::~CTextSelection()
{
}
