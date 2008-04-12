/*	$Id: $

	Copyright 2008 Oliver Tappe <zooey@hirschkaefer.de>
	
	Distributed under the MIT License
*/

#include "pe.h"

#include "CTextStructure.h"

#include "CTextBuffer.h"

CTextStructure::CTextStructure(CTextBuffer* buffer)
	: fTextBuffer(buffer)
{
}

CTextStructure::~CTextStructure()
{
}
