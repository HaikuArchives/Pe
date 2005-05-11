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

#include "PMessages.h"


/* Type: ToolbarButton */
type 'BtnB' {
	longint
		dragger				= 1,
		acceptFirstClick	= 2;		// Flags
	longint = $$CountOf(buttons);
	array buttons {
		longint
			new				= -1,
			open			= -2;		// ResID for icon
		longint;						// Cmd
		longint
			menu			= 1,
			toggle			= 2,
			space			= 4,
			separator		= 8,
			dual			= 16;		// Flags
		cstring;						// help string
	};
};


/* Text Edit Window */
resource 'BtnB' (0) {
	0,
	{
		new,	msg_New,			0,				"New Document",
		open,	msg_BtnOpen,		menu,			"Open Document",
		14,		msg_Save,			0,				"Save Document",
		0,		0,					separator,		"",
		11,		msg_Execute,		toggle,			"Execute Command",
		10,		msg_FuncPopup,		menu,			"Function Popupmenu",
		32,		msg_HeaderPopup,	menu,			"Header Popupmenu",
		0,		0,					separator,		"",
		17,		msg_FindCmd,		0,				"Find",
		13,		msg_IncSearchBtn,	toggle,			"Incremental Search",
		0,		0,					separator,		"",
		15,		msg_ReadOnly,		dual,			"Read Only",
		12,		msg_Info,			0,				"File Options…",
		18,		msg_SoftWrap,		dual,			"Softwrap",
		0,		0,					separator,		"",
		31,		msg_EditAsPrj,		0,				"Edit As Project"
	}
};

/* Find Differences Window */
resource 'BtnB' (2) {
	0,
	{
		28,		msg_DiffFile1,		menu|toggle,	"File 1",
		29,		msg_DiffFile2,		menu|toggle,	"File 2",
		0,		0,					separator,		"",
		27,		msg_RefreshDiffs,	0,				"Refresh",
		0,		0,					separator,		"",
		25,		msg_MergeTo1,		0,				"Merge to File 1",
		26,		msg_MergeTo2,		0,				"Merge to File 2",
	}
};

/* Group Window */
resource 'BtnB' (1) {
	acceptFirstClick,
	{
		20,		'Add ',				0,				"Add File",
		21,		'Rmve',				0,				"Remove File",
		0,		0,					space,			"",
		14,		msg_Save,			0,				"Save Document"
	}
};

/* Project Window */
resource 'BtnB' (1) {
	0,
	{
		20,		'Add ',				0,				"Add File",
		21,		'Rmve',				0,				"Remove File",
		0,		0,					space,			"",
		14,		msg_Save,			0,				"Save Document",
		0,		0,					space,			"",
		22,		msg_EditAsText,	0,					"Edit Project as Text"
	}
};
