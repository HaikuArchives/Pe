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

	Created: 03/31/98 14:29:11
*/

#include "pe.h"
#include "CFtpStream.h"
#include "CLogin.h"
#include "Sstdio.h"
#include "CAlloca.h"

#include <E-mail.h>
#include <socket.h>
#include <netdb.h>

URLData::URLData(const char *server, const char *username, const char *password,
	const char *path, const char *file)
{
	fServer = *server ? strdup(server) : NULL;
	fUsername = *username ? strdup(username) : NULL;
	fPassword = *password ? strdup(password) : NULL;
	fPath = *path ? strdup(path) : NULL;
	fFile = *file ? strdup(file) : NULL;
} /* URLData::URLData */

URLData::URLData(const URLData& data)
{
	fServer = data.fServer ? strdup(data.fServer) : NULL;
	fUsername = data.fUsername ? strdup(data.fUsername) : NULL;
	fPassword = data.fPassword ? strdup(data.fPassword) : NULL;
	fPath = data.fPath ? strdup(data.fPath) : NULL;
	fFile = data.fFile ? strdup(data.fFile) : NULL;
} /* URLData::URLData */

URLData::URLData(const char *url)
{
	fServer = fUsername = fPassword = fPath = fFile = NULL;

	if (strncasecmp(url, "ftp://", 6) == 0)
	{
		CAlloca buf(strlen(url) + 1);
		strcpy(buf, url);
		
		char *p = buf + 6, *r = p;
		
		do
		{
			if (*p == '%')
			{
				char s[4], *t;
				s[0] = *++p;
				s[1] = *++p;
				s[2] = 0;
				
				int c = strtoul(s, &t, 16);
				*r++ = c;
				p++;
			}
			else
				*r++ = *p++;
		}
		while (*p);

		*r = 0;

		p = strchr(buf + 6, '/');
		
		if (p)
		{
			int l = p - buf - 6;
			fServer = (char *)malloc(l + 1);
			FailNil(fServer);
			strncpy(fServer, buf + 6, l);
			fServer[l] = 0;
			
			char *f = strrchr(p + 1, '/');
			
			if (f)
			{
				l = f - p - 1;
				fPath = (char *)malloc(l + 1);
				strncpy(fPath, p + 1, l);
				fPath[l] = 0;
			}
			else
				f = p;

			l = strlen(f);
			fFile = (char *)malloc(l + 1);
			strncpy(fFile, f + 1, l);
			fFile[l] = 0;
		}
	}
} /* CFTPOpen::ParseFtpUrl */

URLData::~URLData()
{
	if (fServer) free(fServer);
	if (fUsername) free(fUsername);
	if (fPassword) free(fPassword);
	if (fPath) free(fPath);
	if (fFile) free(fFile);
} /* URLData::URLData */

URLData& URLData::operator+= (const char *relPath)
{
	if (!IsValid())
		THROW(("Url is not valid in the first place!"));
	
	if (strchr(relPath, '/') == NULL)
	{
		if (fFile) free(fFile);
		fFile = strdup(relPath);
	}
	else
	{
		char path[PATH_MAX] = "";
		
		if (fPath)
		{
			strcpy(path, fPath);
			if (path[strlen(path)] != '/')
				strcat(path, "/");
		}

		strcat(path, relPath);
		
		char *f = strrchr(path, '/');
		*f++ = 0;
		
		free(fFile);
		free(fPath);
		
		fFile = strdup(f);
		fPath = strdup(path);
	}

	return *this;
} /* URLData::operator~= */

void URLData::PrintToStream()
{
	printf("URL:\n\tServer=%s,\n\tDirectory=%s,\n\tFile=%s,\n\tUsername=%s\n",
		fServer, fPath, fFile, fUsername);
} /* URLData::PrintToStream */

#pragma mark -

CFtpStream::CFtpStream(URLData& data, bool retrieve)
	: fURL(data)
{
	if (retrieve)
		Retrieve();
} /* CFtpStream::CFtpStream */

CFtpStream::~CFtpStream()
{
	
} /* CFtpStream::CFtpStream */

static void GetReply(struct SOCK *sock, int& r, char* msg)
{
	if (! s_gets(msg, 1024, sock))
		THROW(("Read error"));

	char *m;
	r = strtoul(msg, &m, 10);
	
	if (*m == '-')
	{
		int r2;
		do
		{
			if (! s_gets(msg, 1024, sock))
				THROW(("Read error"));
			r2 = strtoul(msg, &m, 10);
		}
		while (m == NULL || r2 != r || *m != ' ');
	}
} /* GetReply */

inline void FailSockErr(int e)
{
	if (e < 0) THROW(("Network error: %s", strerror(errno)));
} /* FailSockErr */

void CFtpStream::Retrieve()
{
	Automaton(1);
} /* CFtpStream::Retrieve */

void CFtpStream::Flush()
{
	Automaton(2);
} /* CFtpStream::Flush */

void CFtpStream::Automaton(int action)
{
	int ctrl = 0, data = 0, r;
	char msg[1024];
	SOCK *csSock = NULL;
	
	try
	{
		if (! fURL.IsValid())
			THROW(("The data to connect is not complete"));
		
		ctrl = socket(AF_INET, SOCK_STREAM, 0);
		if (ctrl < 0)
			THROW(("Failed to get socket: %s", strerror(errno)));
		data = socket(AF_INET, SOCK_STREAM, 0);
		if (data < 0)
			THROW(("Failed to get socket: %s", strerror(errno)));
		
		csSock = s_open(ctrl, "r+");
		
		struct hostent *host;
		if ((host = gethostbyname(fURL.Server())) == NULL)
			THROW(("Failed to get server address: %s", strerror(errno)));
		
		struct sockaddr_in sa;
		sa.sin_family = AF_INET;
		sa.sin_port = htons(21);
		sa.sin_addr.s_addr = *(unsigned int *)host->h_addr;
		
		FailOSErr(connect(ctrl, (struct sockaddr *)&sa, sizeof(sa)));
		
		mail_pop_account pa;
		char username[256], password[256];

		strcpy(username, strlen(fURL.Username()) ? fURL.Username() : "anonymous");

		if (strlen(fURL.Password()))
			strcpy(password, fURL.Password());
		else if (get_pop_account(&pa) == B_OK)
			strcpy(password, pa.reply_to);
		else
			strcpy(password, "no.one@nowhere.na");
		
		int state = 1;
		while (state)
		{
			GetReply(csSock, r, msg);

			switch (state)
			{
				case 1:
					if ((r / 100) != 2)
						THROW(("Connect failed: %s", msg));
					s_printf(csSock, "user %s\r\n", username);
					state = 2;
					break;
				
				case 2:
					if ((r / 100) == 3)
					{
						s_printf(csSock, "pass %s\r\n", password);
						state = 3;
						break;
					}
					else if ((r / 100) != 2 && (r / 100) != 5)
						THROW(("Failed to login: %s", msg));

					// fall thru
				
				case 3:
					if ((r / 100) == 5)
					{
						bool ok;
						
						CLogin *l = DialogCreator<CLogin>::CreateDialog(NULL);
						FailNil(l);

						l->Connect(fURL.Server(), username, password, &ok);
						
						if (ok)
						{
							s_printf(csSock, "noop\r\n");
							state = 1;
							continue;
						}
						else
							THROW((0));
					}
					else if (state == 3 && (r / 100) != 2)
						THROW(("Failed to login: %s", msg));
					else if (strlen(fURL.Path()))
					{
						s_printf(csSock, "cwd %s\r\n", fURL.Path());
						state = 4;
						break;
					}
		
					// fall thru
				
				case 4:
				{
					if (state == 4 && (r / 100) != 2)
						THROW(("Failed to change directory: %s", msg));
					
					memset(&sa, 0, sizeof(sa));
					sa.sin_family = AF_INET;
//					sa.sin_addr = INADDR_ANY;
					FailSockErr(bind(data, (struct sockaddr *)&sa, sizeof(sa)));
					FailSockErr(listen(data, 5));
					
					int size = sizeof(sa);
					FailSockErr(getsockname(data, (struct sockaddr *)&sa, &size));
					unsigned char *sap = (unsigned char *)&sa.sin_addr.s_addr, *pap = (unsigned char *)&sa.sin_port;
					
					s_printf(csSock, "port %d,%d,%d,%d,%d,%d\r\n", sap[0], sap[1], sap[2], sap[3], pap[0], pap[1]);
					state = 5;
					break;
				}
				case 5:
					if ((r / 100) != 2)
						THROW(("Port command failed: %s", msg));
					if (action == 1)
					{
						s_printf(csSock, "retr %s\r\n", fURL.File());
						state = 6;
					}
					else
					{
						s_printf(csSock, "stor %s\r\n", fURL.File());
						state = 7;
					}
					break;
				
				case 6:
					if ((r / 100) == 1)
					{
						int ds;
						int size = sizeof(sa);

						FailSockErr(ds = accept(data, (struct sockaddr *)&sa, &size));
						
						try
						{
							do
							{
								FailSockErr(r = recv(ds, msg, 1024, 0));
								if (r)
									fData.Write(msg, r);
							}
							while (r);
						
							closesocket(ds);
						}
						catch (HErr& e)
						{
							closesocket(ds);
							throw;
						}
						
						state = 8;
					}
					else
						THROW(("Failed to retrieve file: %s", msg));
					break;
					
				case 7:
					if ((r / 100) == 1)
					{
						int ds;
						int size = sizeof(sa);

						FailSockErr(ds = accept(data, (struct sockaddr *)&sa, &size));
						
						try
						{
							FailSockErr(send(ds, fData.Buffer(), fData.BufferLength(), 0));
							closesocket(ds);
						}
						catch (HErr& e)
						{
							closesocket(ds);
							throw;
						}
						
						state = 8;
					}
					else
						THROW(("Failed to store file: %s", msg));
					break;

				case 8:
					if ((r / 100) != 2)
						THROW(("Failed to %s file: %s", action == 1 ? "retrieve" : "store",  msg));
					s_printf(csSock, "quit\r\n");
					state = 0;
					break;

			}
		}
		
		s_close(csSock);
		closesocket(data);
		closesocket(ctrl);
	}
	catch (HErr& e)
	{
		if (csSock) s_close(csSock);
		if (ctrl) closesocket(ctrl);
		if (data) closesocket(data);

		throw;
	}
} /* CFtpStream::Automaton */

ssize_t CFtpStream::ReadAt(off_t pos, void *buffer, size_t size)
{
	return fData.ReadAt(pos, buffer, size);
} /* CFtpStream::ReadAt */

ssize_t CFtpStream::WriteAt(off_t pos, const void *buffer, size_t size)
{
	return fData.WriteAt(pos, buffer, size);
} /* CFtpStream::WriteAt */

off_t CFtpStream::Seek(off_t position, uint32 seek_mode)
{
	return fData.Seek(position, seek_mode);
} /* CFtpStream::Seek */

off_t CFtpStream::Position() const
{
	return fData.Position();
} /* CFtpStream::Position */

int CFtpStream::CheckReply(const char *reply)
{
	if (isdigit(reply[0]) && isdigit(reply[1]) && isdigit(reply[2]) && reply[3] == ' ')
	{
		return reply[0] - '0';
	}
	else
		return 0;
} /* CFtpStream::CheckReply */

