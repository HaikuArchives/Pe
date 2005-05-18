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

#include "Types.r"


resource rtyp_Popu (rid_Popu_FtpPath)
{
	"pub",
	{
		Item		{ "/",		'dir ',		none,	noKey	},
		Item		{ "pub",	'dir ',		none,	noKey	}
	};
};

resource 'DLOG' (128, "Open from Server") {
	{ 0, 0, 375, 200 }, "Open From Server", B_TITLED_WINDOW, NORMAL,
	{
		Caption		{{  10,  10, 100,  26 }, "csrvr",		"Server:"							},
		Edit		{{  10,  30, 100,  46 }, "srvr",		"",				"", "", 0, 0		},
		CheckBox	{{  12,  48, 100,  64 }, "pssv",		"Passive"							},
		Caption		{{  10,  70, 100,  86 }, "cuser",		"User Name:"						},
		Edit		{{  10,  90, 100, 106 }, "user",		"",				"", "", 24, 0		},
		Caption		{{  10, 110, 100, 126 }, "csrvr",		"Password:"							},
		Edit		{{  10, 130, 100, 146 }, "pass",		"",				"", "", 128, 0		},
		
		Button		{{  25, 160,  85, 180 }, "cnct",		"Connect",		'cnct'				},
		
		Line		{{ 106,  10, 107, 160 }														},
		
		PopupMenu	{{ 115,  10, 295,  30 }, "path",		"",				rid_Popu_FtpPath, 0	},
		
		List		{{ 115,  40, 365, 160 }, "list"												},
		
		CheckBox	{{ 300,  10, 365,  26 }, "dotf",		"Show All"							},
		
		Edit		{{  10, 174, 210, 190 }, "name",		"",				"", "", 0, 0		},
		
		Button		{{ 305, 170, 365, 190 }, "ok  ",		"Open",			'ok  '				},
		Button		{{ 235, 170, 295, 190 }, "cncl",		"Cancel",		'cncl'				}
	}
};
