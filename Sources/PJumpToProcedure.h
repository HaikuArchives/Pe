/*
 * Copyright 2015, Axel Dörfler, axeld@pinc-software.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef PJUMP_TO_DECLARATION_H
#define PJUMP_TO_DECLARATION_H

#include "CLanguageInterface.h"
#include "PText.h"

class PJumpToProcedure : private CFunctionScanHandler {
public:
	PJumpToProcedure(PText* text, int32 from, int32 to);

	const char* Name() const { return fName; }
	int32 Offset() const { return fOffset; }

private:
	bool _Jump(PText* text);

	virtual	void AddFunction(const char* name, const char* match, int32 offset,
		bool italic, uint32 nestLevel, const char* params);
	virtual	void AddInclude(const char* name, const char* open, bool italic);
	virtual	void AddSeparator(const char* name);

private:
	const char*		fMatch;
	int32			fMatchLength;
	BString			fName;
	int32			fOffset;
};

#endif	// PJUMP_TO_DECLARATION_H
