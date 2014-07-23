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

	Created: 03/31/98 14:11:16
*/

#ifndef CFTPSTREAM_H
#define CFTPSTREAM_H


class URLData {

public:
	URLData(const char *server, const char *username, const char *password,
		const char *path, const char *file);
	URLData(const URLData& data);
	URLData(const char *url);
	~URLData();
	
	void PrintToStream();
	bool IsValid() const;
	
	const char *Server() const;
	const char *Username() const;
	const char *Password() const;
	const char *Path() const;
	const char *File() const;

	URLData& operator+= (const char *relUrl);

private:
	char *fServer;
	char *fUsername;
	char *fPassword;
	char *fPath;
	char *fFile;
};

class CFtpStream : public BPositionIO {
public:
			CFtpStream(URLData& spec, bool retrieve, bool passive);
virtual	~CFtpStream();
		
virtual	ssize_t ReadAt(off_t pos, void *buffer, size_t size);
virtual	ssize_t WriteAt(off_t pos, const void *buffer, size_t size);

virtual	off_t Seek(off_t position, uint32 seek_mode);
virtual	off_t Position() const;

			status_t Flush();
			void Retrieve();

protected:

			int CheckReply(const char *reply);
			void Automaton(int action);

			URLData fURL;
			BMallocIO fData;
			bool fPassive;
};

inline bool URLData::IsValid() const
{
	return fServer != NULL && fFile != NULL;
} /* URLData::IsValid */

inline const char *URLData::Server() const
{
	return fServer ? fServer : B_EMPTY_STRING;
} /* URLData::Server */

inline const char *URLData::Username() const
{
	return fUsername ? fUsername : B_EMPTY_STRING;
} /* URLData::*Username */

inline const char *URLData::Password() const
{
	return fPassword ? fPassword : B_EMPTY_STRING;
} /* URLData::*Password */

inline const char *URLData::Path() const
{
	return fPath ? fPath : B_EMPTY_STRING;
} /* URLData::*Path */

inline const char *URLData::File() const
{
	return fFile ? fFile : B_EMPTY_STRING;
} /* URLData::*File */

#endif // CFTPSTREAM_H
