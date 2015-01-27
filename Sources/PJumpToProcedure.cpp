/*
 * Copyright 2015, Axel Dörfler, axeld@pinc-software.de.
 * Distributed under the terms of the MIT License.
 */

#include "PJumpToProcedure.h"

#include <Window.h>

#include "CAlloca.h"
#include "PDoc.h"
#include "PMessages.h"

PJumpToProcedure::PJumpToProcedure(PText* text, int32 from, int32 to)
	:
	fOffset(-1)
{
	fMatchLength = abs(to - from);

	CAlloca buffer(fMatchLength + 1);
	text->TextBuffer().Copy(buffer, from, fMatchLength);
	buffer[fMatchLength] = '\0';

	fMatch = buffer;

	// Search in the current document
	if (_Jump(text))
		return;

	// Search in other documents

	for (doclist::const_iterator iterator = CDoc::DocList().begin();
			iterator != CDoc::DocList().end(); iterator++) {
		PDoc* document = dynamic_cast<PDoc*>(*iterator);
		if (document == NULL || document->TextView() == text)
			continue;

		if (_Jump(document->TextView())) {
			document->Activate();
			return;
		}
	}

	// TOOD: detect #include statements, and jump to the include file
	// TODO: do a simple search in all open files
	// TODO: do a simple search in all files in the same directory
}

bool
PJumpToProcedure::_Jump(PText* text)
{
	text->ScanForFunctions(*this);

	if (!fName.IsEmpty()) {
		BMessage jump(msg_JumpToProcedure);
		jump.AddInt32("offset", fOffset);
		jump.AddString("function", fName.String());
		text->Window()->PostMessage(&jump, text);
		return true;
	}

	return false;
}

void
PJumpToProcedure::AddFunction(const char* name, const char* match,
	int32 offset, bool italic, uint32 nestLevel, const char* params)
{
	// TODO: take context into account (object->, number of arguments)
	const char* found = strstr(name, fMatch);
	if (found != NULL && (found == name || !isalnum(found[-1]))
			&& !isalnum(found[fMatchLength])) {
		fName = name;
		fOffset = offset;
	}
}

void
PJumpToProcedure::AddInclude(const char* name, const char* open, bool italic)
{
}

void
PJumpToProcedure::AddSeparator(const char* name)
{
}
