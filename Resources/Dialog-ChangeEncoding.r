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


resource rtyp_Popu (rid_Popu_EncConv, "Conversion") {
	"Conversion",
	{
		Item		{ "UTF-8",				'chng',	none,	noKey	},
		Separator	{												},
		Item		{ "ISO 8859 1",			'chng',	none,	noKey	},
		Item		{ "ISO 8859 2",			'chng',	none,	noKey	},
		Item		{ "ISO 8859 3",			'chng',	none,	noKey	},
		Item		{ "ISO 8859 4",			'chng',	none,	noKey	},
		Item		{ "ISO 8859 5",			'chng',	none,	noKey	},
		Item		{ "ISO 8859 6",			'chng',	none,	noKey	},
		Item		{ "ISO 8859 7",			'chng',	none,	noKey	},
		Item		{ "ISO 8859 8",			'chng',	none,	noKey	},
		Item		{ "ISO 8859 9",			'chng',	none,	noKey	},
		Item		{ "ISO 8859 10",		'chng',	none,	noKey	},
		Item		{ "Macintosh Roman",	'chng',	none,	noKey	},
		Item		{ "Shift-JIS",			'chng',	none,	noKey	},
		Item		{ "Extended UNIX Code",	'chng',	none,	noKey	}
	}
};

resource 'DLOG' (7, "Convert") {
	{ 0, 0, 210, 120 }, "Convert", B_TITLED_WINDOW, NORMAL,
	{
		Caption		{{  10,  10, 200,  26 }, "cap",		"Change the character encoding"				},
		PopupMenu	{{   9,  29, 200,  59 }, "src",		"From:",			rid_Popu_EncConv, 40	},
		PopupMenu	{{	 9,  49, 200,  79 }, "dest",	"To:",				rid_Popu_EncConv, 40	},

		Button		{{ 140,  90, 200, 110 }, "ok",		"OK",				'ok  '					},
		Button		{{  70,  90, 130, 110 }, "cancel",	"Cancel",			'cncl'					}
	}
};
