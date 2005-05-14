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


resource 'MENU' (81, "Kind") {
	"Method:",
	{
		Item		{ "Directory Scan",				'MFKn',	none,	noKey	},
		Item		{ "Open Windows",				'MFKn',	none,	noKey	},
		Item		{ "$BEINCLUDES",				'MFKn',	none,	noKey	}
	}
};

resource 'MENU' (82, "Dir") {
	"Dir",
	{
		Item		{ "Other…",						'SelO',	none,	noKey	}
	}
};

resource 'MENU' (83, "File Name") {
	"FileName",
	{
		Item		{ "Any File Name",				'chng',	none,	noKey	},
		Item		{ "File Name ends with",		'chng',	none,	noKey	},
		Item		{ "File Name begins with",		'chng',	none,	noKey	},
		Item		{ "File Name contains",			'chng',	none,	noKey	}
	}
};

resource 'MENU' (84, "Grep Pattern") {
	"Find:",
	{
		Item		{ "Add this pattern…",			'addP',	none,	noKey	},
		Separator	{														}
	}
};

/* Find Dialog */
resource 'DLOG' (8, "Find") {
	{ 0, 0, 350, 214 }, "Find", B_TITLED_WINDOW, NORMAL,
	{
		PopupMenu	{{   5,   9,  52,  25 }, "pats",	" ",					84, 1				},
		Edit		{{  53,  10, 230,  42 }, "find",							"", "", "", 0, 0	},
		Caption		{{  10,  46,  52,  62 }, "crepl",	"Replace:"									},
		Edit		{{  53,  46, 230,  78 }, "repl",							"", "", "", 0, 0	},

		CheckBox	{{  10,  94,  99, 110 }, "case",	"Ignore Case"								},
		CheckBox	{{  10, 116,  99, 132 }, "wrap",	"Wrap Around"								},
		CheckBox	{{ 100,  94, 179, 110 }, "back",	"Backwards"									},
		CheckBox	{{ 100, 116, 179, 132 }, "word",	"Entire Word"								},
		CheckBox	{{ 180,  94, 250, 110 }, "grep",	"Grep"										},
		CheckBox	{{ 180, 116, 250, 132 }, "btch",	"Batch"										},
		
		Line		{{  10, 138, 340, 139 }															},
		
		CheckBox	{{  10, 144,  90, 160 }, "mult",	"Multi-File"								},
		PopupMenu	{{  75, 142, 238, 162 }, "meth",	"Method:",				81, 45				},
		CheckBox	{{ 270, 144, 340, 160 }, "recu",	"Recursive"									},
		PopupMenu	{{  10, 164, 340, 184 }, "sdir",	"Starting Directory:",	82, 110				},
		CheckBox	{{  10, 188, 115, 204 }, "text",	"Text Files Only"							},
		PopupMenu	{{ 120, 186, 245, 206 }, "namp",	"",						83, 0				},
		Edit		{{ 270, 186, 340, 206 }, "name",							"", "", "", 0, 0	},

		Button		{{ 260,  10, 340,  30 }, "bfnd",	"Find",					'Find'				},
		Button		{{ 260,  40, 340,  60 }, "brpl",	"Replace",				'Rplc'				},
		Button		{{ 260,  70, 340,  90 }, "br&f",	"Replace & Find",		'RpFn'				},
		Button		{{ 260, 100, 340, 120 }, "ball",	"Replace All",			'RpAl'				}
	}
};
