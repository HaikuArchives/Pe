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
#include "ResourcesToolbars.h"


/* Type: ToolbarButton */
type rtyp_Tbar {
	longint
		standard			= 0,
		dragger				= 1;		// Flags
	longint = $$CountOf(buttons);
	array buttons {
		longint;						// ResID for icon 1
		longint;						// ResID for icon 2
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


/* Document Window */
resource rtyp_Tbar (rid_Tbar_DocumentWin) {
	standard,
	{
		rid_Timg_NewDocument,	0,						msg_New,			0,				"New document",
		rid_Timg_OpenDocument,	0,						msg_BtnOpen,		menu,			"Open document",
		rid_Timg_SaveDocument,	0,						msg_Save,			0,				"Save document",
		0,						0,						0,					separator,		"",
		rid_Timg_ExecuteCmd,	0,						msg_Execute,		toggle,			"Execute command",
		rid_Timg_FunctionPopup,	0,						msg_FuncPopup,		menu,			"Function popup-menu",
		rid_Timg_HeaderPopup,	0,						msg_HeaderPopup,	menu,			"Header popup-menu",
		0,						0,						0,					separator,		"",
		rid_Timg_Find,			0,						msg_FindCmd,		0,				"Find",
		rid_Timg_IncSearch,		0,						msg_IncSearchBtn,	toggle,			"Incremental search",
		0,						0,						0,					separator,		"",
		rid_Timg_ReadonlyOff,	rid_Timg_ReadonlyOn,	msg_ReadOnly,		dual,			"Read only",
		rid_Timg_FileOptions,	0,						msg_Info,			0,				"File options…",
		rid_Timg_SoftwrapOff,	rid_Timg_SoftwrapOn,	msg_SoftWrap,		dual,			"Soft-wrap",
		0,						0,						0,					separator,		"",
		rid_Timg_EditAsProject,	0,						msg_EditAsProject,	0,				"Edit as project"
	}
};

/* Differences Window */
resource rtyp_Tbar (rid_Tbar_DifferencesWin) {
	standard,
	{
		rid_Timg_Folder1,		0,						msg_DiffFile1,		menu|toggle,	"File 1",
		rid_Timg_Folder2,		0,						msg_DiffFile2,		menu|toggle,	"File 2",
		0,						0,						0,					separator,		"",
		rid_Timg_ArrowCircle,	0,						msg_RefreshDiffs,	0,				"Refresh",
		0,						0,						0,					separator,		"",
		rid_Timg_ArrowLeft,		0,						msg_MergeTo1,		0,				"Merge to file 1",
		rid_Timg_ArrowRight,	0,						msg_MergeTo2,		0,				"Merge to file 2",
	}
};

/* Group Window */
resource rtyp_Tbar (rid_Tbar_GroupWin) {
	standard,
	{
		rid_Timg_FileAdd,		0,						'Add ',				0,				"Add file",
		rid_Timg_FileRemove,	0,						'Rmve',				0,				"Remove file",
		0,						0,						0,					space,			"",
		rid_Timg_SaveDocument,	0,						msg_Save,			0,				"Save document"
	}
};

/* Project Window */
resource rtyp_Tbar (rid_Tbar_ProjectWin) {
	standard,
	{
		rid_Timg_FileAdd,		0,						'Add ',				0,				"Add file",
		rid_Timg_FileRemove,	0,						'Rmve',				0,				"Remove file",
		0,						0,						0,					space,			"",
		rid_Timg_SaveDocument,	0,						msg_Save,			0,				"Save document",
		0,						0,						0,					space,			"",
		rid_Timg_FilePencil,	0,						msg_EditAsText,		0,				"Edit project as text"
	}
};
