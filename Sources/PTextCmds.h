/*	$Id$
	
	Copyright 1996, 1997, 1998, 2002
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
*/

#ifndef PTEXTCMDS_H
#define PTEXTCMDS_H

class PShiftCmd : public PCmd {
public:
		PShiftCmd(PText *txt, bool right);

virtual	void Do();
virtual	void Undo();

private:
		int fFrom, fTo;
		bool fShiftRight;
};

class PTwiddleCmd : public PCmd {
public:
		PTwiddleCmd(PText *txt);

virtual void Do();
virtual void Undo();

private:
		int fFrom, fTo;
};

class PCommentCmd : public PCmd {
public:
		PCommentCmd(PText *txt, bool comment,
			const char *before, const char *after);
	
virtual void Do();
virtual void Undo();

private:
		void CommentLine(int line);
		bool UncommentLine(int line);
		
		bool fComment;
		int fFrom, fTo;
		char fBefore[8], fAfter[8];
		vector<int> fLines;
};

class PWrapCmd : public PCmd {
public:
			PWrapCmd(PText *txt);
			~PWrapCmd();

virtual	void Do();
virtual	void Undo();

private:
			int fCaret, fAnchor, fNewLen;
			char *fSavedText;
};

class PUnwrapCmd : public PCmd {
public:
			PUnwrapCmd(PText *txt);
			~PUnwrapCmd();

virtual	void Do();
virtual	void Undo();

private:
			int fCaret, fAnchor, fNewLen;
			char *fSavedText;
};

class PJustifyCmd : public PCmd {
public:
			PJustifyCmd(PText *txt);
			~PJustifyCmd();
			
virtual	void Do();
virtual	void Undo();
virtual	void Redo();

private:
			int fAnchor, fOldLen, fNewLen;
			char *fSavedText;
};

class PChangeCaseCmd : public PCmd {
public:
		PChangeCaseCmd(PText *txt, int newCase);
		~PChangeCaseCmd();

virtual	void Do();
virtual	void Undo();

private:

		void ChangeCase(int newCase);

		char *fSavedText;
		int fAnchor, fCaret;
};

class PEncodingCmd : public PCmd {
public:
		PEncodingCmd(PText *txt, int from, int to);
		~PEncodingCmd();

virtual void Do();
virtual void Undo();

private:
		int fSourceEncoding, fDestEncoding, fPrevEncoding;
		char *fSaved;
		long fAnchor, fCaret, fSrcLen, fDstLen;
};

#endif // PTEXTCMDS_H
