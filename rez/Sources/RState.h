/*	$Id$

	Copyright 1996, 1997, 1998
	        Hekkelman Programmatuur B.V.  All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
	1. Redistributions of source code must retain the above copyright notice,
	   this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice,
	   this list of conditions and the following disclaimer in the documentation
	   and/or other materials provided with the distribution.
	3. All advertising materials mentioning features or use of this software
	   must display the following acknowledgement:

	    This product includes software developed by Hekkelman Programmatuur B.V.

	4. The name of Hekkelman Programmatuur B.V. may not be used to endorse or
	   promote products derived from this software without specific prior
	   written permission.

	THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
	FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
	AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
	OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


	Created: 12/02/98 15:38:11
*/

#ifndef RSTATE_H
#define RSTATE_H

#include <map>
#include "RElem.h"
#include "SymbolTable.h"

enum {
	tString, tInt, tIdent, tCase, tArray, tArrayEnd, tRaw
};

class BList;
class RState;
struct RElem;

typedef std::map<addr_t, addr_t> intmap;

extern intmap gValueMap;

struct RType {
	int type;
	RState *firstState;

	RType (int32 t, RState *f) : type(t), firstState(f) {};
};

class RState {
public:
			RState();
virtual	~RState();

static		RState* FirstState(int32 type);
static		void FinishType(int32 type, RState *states);
static		void CopyType(int32 type1, int32 type2);

virtual	RState* Shift(addr_t v, int32 token, RElem** head);
virtual	void SetNext(RState *next);

			RState *fNext;
static		intmap sfTypeMap;
};

struct RSymbol {
	addr_t sIdentifier;
	addr_t sValue;

	RSymbol(addr_t ident, addr_t val) : sIdentifier(ident), sValue(val) {};
};

class RSValue : public RState {
public:
			RSValue(int32 type);

virtual	void SetDefaultValue(addr_t v);
			void AddIdentifiers(BList *idents);
			bool ResolveIdentifier(addr_t& v);

			int32 fType;
			addr_t fValue;
			bool fHasDefault;
			BList *fIdents;
};

class RSNrValue : public RSValue {
public:
			RSNrValue(int32 size);

virtual	RState* Shift(addr_t v, int32 token, RElem** head);

			addr_t fSize;
};

class RSStringValue : public RSValue {
public:
		RSStringValue(int32 kind, int32 size = 0);

		enum {
			skStr, skPStr, skWStr, skCStr, skHex
		};

virtual RState* Shift(addr_t v, int32 token, RElem** head);

private:
		int32 fKind;
		addr_t fSize;
};

class RSArray : public RState {
public:
		RSArray(RState *data, int32 ident = 0, int32 fixedCount = 0);

virtual RState* Shift(addr_t v, int32 token, RElem** head);
};

class RSArrayNode : public RState {
public:
		RSArrayNode(RState *data, int32 ident = 0, int32 fixedCount = 0);

virtual RState* Shift(addr_t v, int32 token, RElem** head);
		void ResetCounter();

protected:
		RState *fHead;
		int32 fIdent;
		int32 fCount;
		int32 fFixedCount;
};

struct RCase {
	addr_t sIdent;
	RState *sStates;

	RCase(addr_t ident, RState *states) : sIdent(ident), sStates(states) {};
};

class RSSwitch : public RState {
public:
		RSSwitch(BList *cases);

virtual RState* Shift(addr_t v, int32 token, RElem** head);
virtual void SetNext(RState *next);

protected:
		BList *fCases;
};


#endif
