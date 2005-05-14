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
type rt_TBR {
	longint
		standard			= 0,
		dragger				= 1,
		acceptFirstClick	= 2;		// Flags
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
resource rt_TBR (ri_TBR_DOCUMENT_WIN) {
	standard,
	{
		ri_TBI_NEW_DOCUMENT,	0,					msg_New,			0,				"New Document",
		ri_TBI_OPEN_DOCUMENT,	0,					msg_BtnOpen,		menu,			"Open Document",
		ri_TBI_SAVE_DOCUMENT,	0,					msg_Save,			0,				"Save Document",
		0,						0,					0,					separator,		"",
		ri_TBI_EXECUTE_COMMAND,	0,					msg_Execute,		toggle,			"Execute Command",
		ri_TBI_FUNCTION_POPUP,	0,					msg_FuncPopup,		menu,			"Function Popupmenu",
		ri_TBI_HEADER_POPUP,	0,					msg_HeaderPopup,	menu,			"Header Popupmenu",
		0,						0,					0,					separator,		"",
		ri_TBI_FIND,			0,					msg_FindCmd,		0,				"Find",
		ri_TBI_INC_SEARCH,		0,					msg_IncSearchBtn,	toggle,			"Incremental Search",
		0,						0,					0,					separator,		"",
		ri_TBI_READONLY_OFF,	ri_TBI_READONLY_ON,	msg_ReadOnly,		dual,			"Read Only",
		ri_TBI_FILE_OPTIONS,	0,					msg_Info,			0,				"File Options…",
		ri_TBI_SOFTWRAP_OFF,	ri_TBI_SOFTWRAP_ON,	msg_SoftWrap,		dual,			"Softwrap",
		0,						0,					0,					separator,		"",
		ri_TBI_EDIT_AS_PROJECT,	0,					msg_EditAsPrj,		0,				"Edit As Project"
	}
};

/* Differences Window */
resource rt_TBR (ri_TBR_DIFFERENCES_WIN) {
	standard,
	{
		ri_TBI_FOLDER_1,		0,					msg_DiffFile1,		menu|toggle,	"File 1",
		ri_TBI_FOLDER_2,		0,					msg_DiffFile2,		menu|toggle,	"File 2",
		0,						0,					0,					separator,		"",
		ri_TBI_ARROW_CIRCLE,	0,					msg_RefreshDiffs,	0,				"Refresh",
		0,						0,					0,					separator,		"",
		ri_TBI_ARROW_LEFT,		0,					msg_MergeTo1,		0,				"Merge to File 1",
		ri_TBI_ARROW_RIGHT,		0,					msg_MergeTo2,		0,				"Merge to File 2",
	}
};

/* Group Window */
resource rt_TBR (ri_TBR_GROUP_WIN) {
	acceptFirstClick,
	{
		ri_TBI_FILE_ADD,		0,					'Add ',				0,				"Add File",
		ri_TBI_FILE_REMOVE,		0,					'Rmve',				0,				"Remove File",
		0,						0,					0,					space,			"",
		ri_TBI_SAVE_DOCUMENT,	0,					msg_Save,			0,				"Save Document"
	}
};

/* Project Window */
resource rt_TBR (ri_TBR_PROJECT_WIN) {
	standard,
	{
		ri_TBI_FILE_ADD,		0,					'Add ',				0,				"Add File",
		ri_TBI_FILE_REMOVE,		0,					'Rmve',				0,				"Remove File",
		0,						0,					0,					space,			"",
		ri_TBI_SAVE_DOCUMENT,	0,					msg_Save,			0,				"Save Document",
		0,						0,					0,					space,			"",
		ri_TBI_FILE_PENCIL,		0,					msg_EditAsText,		0,				"Edit Project as Text"
	}
};
