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
	
	Created: 02/23/98 20:50:08
	
cd ~/projects/pe/lpe
mwcc -prefix BeHeaders lpe.cpp
cp lpe ~/config/bin
*/

#include <vector>
#include <cstdio>
#include <cstdarg>
#include <Message.h>
#include <Roster.h>

const long msg_CommandLineOpen = 'Cmdl';

static int lineNr = -1;
static vector<int> threads;

void DoError(const char *e, ...);
void Usage();
void OpenInPe(entry_ref& ref);

void Usage()
{
	puts("usage: lpe [+linenr] file1 file2 ...");
	exit(1);
} /* Usage */

void DoError(const char *e, ...)
{
	char msg[1024] = "### lpe Error\n# ";
	
	va_list vl;
	va_start(vl, e);
	vsprintf(msg + strlen(msg), e, vl);
	va_end(vl);
	
	strcat(msg, "\n");

	fputs(msg, stderr);
	exit(1);
} /* error */

void OpenInPe(entry_ref& doc)
{
	BMessage msg(msg_CommandLineOpen), reply;
	msg.AddRef("refs", &doc);
	
	if (lineNr >= 0)
		msg.AddInt32("line", lineNr);

	entry_ref pe;
	if (be_roster->FindApp("application/x-vnd.beunited.pe", &pe))
		DoError("Could not find Pe!");
	
	if (! be_roster->IsRunning(&pe))
	{
		status_t err;
		err = be_roster->Launch(&pe);
		if (err) DoError("Error launching Pe (%s)", strerror(err));
	}
		
	BMessenger msr(NULL, be_roster->TeamFor(&pe));
	msr.SendMessage(&msg, &reply);

	if (reply.HasInt32("thread"))
	{
		thread_id tid;
		status_t err;
		
		err = reply.FindInt32("thread", (long *)&tid);
		if (err) DoError("Error getting thread id (%s)", strerror(err));
		
		threads.push_back(tid);
	}
	else
		DoError("No Thread ID in reply");
} /* OpenInPe */

main(int argc, char *argv[])
{
	if (argc < 2) Usage();
	
	int i = 0;
	char *p;

	while (++i < argc)
	{
		switch (argv[i][0])
		{
			case '+':
				lineNr = strtoul(argv[i] + 1, &p, 10);
				if (! p || p == argv[i] + 1) Usage();
				break;
			
			default:
			{
				status_t err;
				BEntry e;

				err = e.SetTo(argv[i], true);
				if (err) DoError("Error trying to access file %s, (%s)", argv[i], strerror(err));
//				if (! e.Exists()) DoError("File %s does not exist", argv[i]);
				if (e.Exists() && ! e.IsFile()) DoError("%s is not a regular file", argv[i]);
				
				entry_ref ref;
				err = e.GetRef(&ref);
				if (err) DoError("Error trying to access file %s, (%s)", argv[i], strerror(err));
				OpenInPe(ref);
			}
		}
	}
	
	vector<int>::iterator ti;
	
	for (ti = threads.begin(); ti != threads.end(); ti++)
	{
		long l;
		wait_for_thread((*ti), &l);
//		status_t err = wait_for_thread((*ti), &l);
//		if (err) DoError("Error waiting for thread: %s", strerror(err));
	}
	
	return 0;
} /* main */
