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

#include "../Sources/KeyBindings.h"
#include "../Sources/PMessages.h"

type 'Cmnd' {
	longint = $$CountOf(binding);
	array binding {
		longint;										// Command
		cstring;										// Description
	}
};

type 'Bind' {
	longint = $$CountOf(cmdbinding);			// nr of Bindings
	array cmdbinding {

// The key combination
		integer
			Shift = 0x0001,
			Cmd = 0x0002,
			Ctrl = 0x0004,
			Opt = 0x0040,
			Menu = 0x0080;						// Modifiers
		char = 0;										// Raw Character
		char;											// Keycode

// The Prefix for this combination
		integer
			Shift = 0x0001,
			Cmd = 0x0002,
			Ctrl = 0x0004,
			Opt = 0x0040,
			Menu = 0x0080;						// Modifiers
		char = 0;										// Raw Character
		char;											// Keycode

// The command
		longint;										// Command
	};
};

//		kmsg_OpenHeaderSource, { Shift, '\t' }, "Open Header/Source File",
//		"Abort Shell Command",

Resource 'Cmnd' (0, "Editing commands") {
	{
		kmsg_NrArgument,										"Numerical Argument Prefix",
		kmsg_Recenter,											"Recenter",
		kmsg_OpenLine,											"Open Line",
		kmsg_AppendNextCut,									"Append Next Cut",
		kmsg_ClearRegion,											"Delete Region between Caret and Mark",
		kmsg_CopyRegion,										"Copy Region between Caret and Mark",
		kmsg_CutRegion,											"Cut Region between Caret and Mark",
		kmsg_ExchangeMarkAndPoint,							"Exchange Caret and Mark",
		kmsg_MarkWord,											"Mark Word",
		kmsg_MarkAll,												"Mark All",
		kmsg_Mark,													"Mark",
		kmsg_Cut_Word_Backward,								"Cut Previous Word",
		kmsg_Cut_Word,											"Cut Next Word",
		kmsg_Cut_to_End_of_Line,								"Cut to End of Line",
		kmsg_Delete_to_End_of_File,							"Delete to End of File",
		kmsg_Delete_to_End_of_Line,							"Delete to End of Line",
		kmsg_Delete_Character_Right,							"Delete Character Right",
		kmsg_Delete_Character_Left,							"Delete Character Left",
		kmsg_Scroll_to_End_of_File,							"Scroll to End of File",
		kmsg_Scroll_to_Start_of_File,							"Scroll to Start of File",
		kmsg_Scroll_Page_Down,									"Scroll Page Down",
		kmsg_Scroll_Page_Up,									"Scroll Page Up",
		kmsg_Scroll_One_Line_Down,							"Scroll One Line Down",
		kmsg_Scroll_One_Line_Up,								"Scroll One Line Up",
		kmsg_Extend_Selection_to_End_of_File,				"Extend Selection to End of File",
		kmsg_Extend_Selection_to_Beginning_of_File,		"Extend Selection to Beginning of File",
		kmsg_Extend_Selection_to_End_of_Page,			"Extend Selection to End of Page",
		kmsg_Extend_Selection_to_Beginning_of_Page,	"Extend Selection to Beginning of Page",
//		kmsg_Extend_Selection_to_Current_Line,			"Extend Selection to Current Line",
		kmsg_Extend_Selection_to_Next_Line,				"Extend Selection to Next Line",
		kmsg_Extend_Selection_to_Previous_Line,			"Extend Selection to Previous Line",
		kmsg_Extend_Selection_to_End_of_Line,			"Extend Selection to End of Line",
		kmsg_Extend_Selection_to_Beginning_of_Line,		"Extend Selection to Beginning of Line",
		kmsg_Extend_Selection_with_Next_Word,			"Extend Selection with Next Word",
		kmsg_Extend_Selection_with_Previous_Word,		"Extend Selection with Previous Word",
		kmsg_Extend_Selection_with_Character_Right,	"Extend Selection with Character Right",
		kmsg_Extend_Selection_with_Character_Left,		"Extend Selection with Character Left",
		kmsg_Move_to_End_of_File,							"Move to End of File",
		kmsg_Move_to_Beginning_of_File,					"Move to Beginning of File",
		kmsg_Move_to_Bottom_of_Page,					"Move to Bottom of Page",
		kmsg_Move_to_Top_of_Page,							"Move to Top of Page",
		kmsg_Move_to_Next_Line,								"Move to Next Line",
		kmsg_Move_to_Previous_Line,							"Move to Previous Line",
		kmsg_Move_to_End_of_Line,							"Move to End of Line",
		kmsg_Move_to_Beginning_of_Line,					"Move to Beginning of Line",
		kmsg_Move_Word_Right,								"Move Word Right",
		kmsg_Move_Word_Left,									"Move Word Left",
		kmsg_Move_Character_Right,							"Move Character Right",
		kmsg_Move_Character_Left,							"Move Character Left"
	}
};

Resource 'Cmnd' (1, "File Menu") {
	{
		msg_Quit,													"Quit",
		msg_Revert,												"Revert",
		msg_SaveACopy,											"Save a Copy As…",
		msg_FtpSave,												"Save on Server…",
		msg_SaveAs,												"Save As…",
		msg_SaveAll,												"Save All",
		msg_Save,													"Save",
		msg_CloseAll,												"Close All",
		msg_Close,													"Close",
		msg_OpenSelected,										"Open Selected File",
		msg_FtpOpen,												"Open from Server…",
		msg_Open,													"Open…",
		msg_NewProject,											"New Project",
		msg_New,													"New"
	}
};

Resource 'Cmnd' (2, "Edit Menu") {
	{
		msg_SelectParagraph,										"Select Paragraph",
		msg_SelectLine,											"Select Line",
		'SALL',														"Select All",
		msg_Clear,													"Clear",
		'PSTE',														"Paste",
		msg_CopyAppend,											"Copy and Append",
		'COPY',														"Copy",
		msg_CutAppend,											"Cut and Append",
		'CCUT',														"Cut",
		msg_Redo,													"Redo",
		msg_Undo,													"Undo"
	}
};

Resource 'Cmnd' (3, "Text Menu") {
	{
		msg_ShowInBrowser,										"Show in Browser",
		msg_Unwrap,												"Remove Linebreaks",
		msg_Wrap,													"Insert Linebreaks",
		msg_Justify,												"Justify",
		msg_Uncomment,											"Uncomment",
		msg_Comment,											"Comment",
		msg_ShiftRight,											"Shift Right",
		msg_ShiftLeft,												"Shift Left",
		msg_ConvertEncoding,									"Change Encoding…",
		msg_ChangeCaseSent,									"Capitalize Sentences",
		msg_ChangeCaseLine,									"Capitalize Lines",
		msg_ChangeCaseCap,										"Capitalize Words",
		msg_ChangeCaseLower,									"Change to Lowercase",
		msg_ChangeCaseUpper,									"Change to Uppercase",
		msg_Twiddle,												"Twiddle",
		msg_Balance,												"Balance"
	}
};

Resource 'Cmnd' (4, "Search Menu") {
	{
		msg_GoToLine,												"Go To Line…",
		msg_FindNextError,										"Jump to Next Error",
		msg_IncSearchBackward,									"Incremental Search Backwards",
		msg_IncSearch,												"Incremental Search",
		msg_FindInNextFile,										"Find in Next File",
		msg_ReplaceAll,											"Replace All",
		msg_ReplaceAndFindBackward,							"Replace and Find Backwards",
		msg_ReplaceAndFind,										"Replace and Find",
		msg_Replace,												"Replace…",
		msg_EnterReplaceString,									"Enter Replace String",
		msg_EnterSearchString,									"Enter Search String",
		msg_FindSelectionBackward,								"Find Selection Backwards",
		msg_FindSelection,											"Find Selection",
		msg_FindAgainBackward,									"Find Again Backwards",
		msg_FindAgain,												"Find Again",
		msg_FindCmd,												"Find…",
		msg_FindFunction,										"Find Function…"
	}
};

Resource 'Cmnd' (5, "Window Menu") {
	{
		msg_Worksheet,											"Open Worksheet",
		msg_Zoom,													"Zoom",
		msg_Tile,													"Tile",
		msg_Stack,													"Stack",
		msg_ShowHTMLPalette,									"HTML Palette",
		msg_ShowGlossary,										"Glossary",
		msg_ChangeWD,											"Working Directory…",
		msg_Preferences,											"Preferences…",
		msg_Info,													"File Options…",
	}
};

Resource 'Cmnd' (6, "Help Menu") {
	{
		msg_About,													"About…",
		msg_Help,													"Help…"
	}
};

Resource 'Cmnd' (7, "BeIDE Menu") {
	{
		msg_IDE_Make,											"Make",
		msg_IDE_Project2Group,									"Project -> Group",
		msg_IDE_Remove,										"Remove File",
		msg_IDE_Add,												"Add File",
		msg_IDE_BringToFront,									"Bring to Front"
	}
};

Resource 'Cmnd' (8, "Miscellaneous") {
	{
		kmsg_UnsplitWindow,										"Unsplit Window",
		kmsg_SwitchActivePart,									"Switch to Other Part of Window",
		kmsg_SplitWindow,										"Split Window",
		msg_CancelCommand,									"Cancel Executing Command",
		msg_Execute,												"Execute Command",
		msg_SwitchHeaderSource,								"Open Header/Source",
	}
};

Resource 'Cmnd' (9, "Extensions") {
	{
	}
};

// Ctrl = left Ctrl key on intel keyboard
// Opt = right Ctrl key on intel keyboard
// Cmd = either Alt key on intel keyboard

Resource 'Bind' (0, "Bindings for Editing") {
	{
		0, 0x61,					0, 0,		kmsg_Move_Character_Left,
		0, 0x63,					0, 0,		kmsg_Move_Character_Right,
		Ctrl, 0x61,				0, 0,		kmsg_Move_Word_Left,
		Ctrl, 0x63,				0, 0,		kmsg_Move_Word_Right,
		Opt, 0x61,				0, 0,		kmsg_Move_Word_Left,
		Opt, 0x63,				0, 0,		kmsg_Move_Word_Right,
		0, 0x20,					0, 0,		kmsg_Move_to_Beginning_of_Line,
		0, 0x35,					0, 0,		kmsg_Move_to_End_of_Line,
		0, 0x57,					0, 0,		kmsg_Move_to_Previous_Line,
		0, 0x62,					0, 0,		kmsg_Move_to_Next_Line,
		0, 0x21,					0, 0,		kmsg_Move_to_Top_of_Page,
		0, 0x36,					0, 0,		kmsg_Move_to_Bottom_of_Page,
		Ctrl, 0x20,				0, 0,		kmsg_Move_to_Beginning_of_File,
		Ctrl, 0x35,				0, 0,		kmsg_Move_to_End_of_File,
		Opt, 0x20,				0, 0,		kmsg_Move_to_Beginning_of_File,
		Opt, 0x35,				0, 0,		kmsg_Move_to_End_of_File,
		0, 0x1e,					0, 0,		kmsg_Delete_Character_Left,
		0, 0x34,					0, 0,		kmsg_Delete_Character_Right,
		Shift, 0x61,				0, 0,		kmsg_Extend_Selection_with_Character_Left,
		Shift, 0x63,				0, 0,		kmsg_Extend_Selection_with_Character_Right,
		Shift | Ctrl, 0x61,		0, 0,		kmsg_Extend_Selection_with_Previous_Word,
		Shift | Ctrl, 0x63,		0, 0,		kmsg_Extend_Selection_with_Next_Word,
		Shift | Opt, 0x61,		0, 0,		kmsg_Extend_Selection_with_Previous_Word,
		Shift | Opt, 0x63,		0, 0,		kmsg_Extend_Selection_with_Next_Word,
		Shift, 0x57,				0, 0,		kmsg_Extend_Selection_to_Previous_Line,
		Shift, 0x62,				0, 0,		kmsg_Extend_Selection_to_Next_Line,
		Shift, 0x20,				0, 0,		kmsg_Extend_Selection_to_Beginning_of_Line,
		Shift, 0x35,				0, 0,		kmsg_Extend_Selection_to_End_of_Line,
		Shift, 0x21,				0, 0,		kmsg_Extend_Selection_to_Beginning_of_Page,
		Shift, 0x36,				0, 0,		kmsg_Extend_Selection_to_End_of_Page,
		Shift | Ctrl, 0x20,		0, 0,		kmsg_Extend_Selection_to_Beginning_of_File,
		Shift | Ctrl, 0x35,		0, 0,		kmsg_Extend_Selection_to_End_of_File,
		Shift | Opt, 0x20,		0, 0,		kmsg_Extend_Selection_to_Beginning_of_File,
		Shift | Opt, 0x35,		0, 0,		kmsg_Extend_Selection_to_End_of_File,
		Ctrl, 0x57,				0, 0,		kmsg_Scroll_One_Line_Up,
		Ctrl, 0x62,				0, 0,		kmsg_Scroll_One_Line_Down,
		Opt, 0x57,				0, 0,		kmsg_Scroll_One_Line_Up,
		Opt, 0x62,				0, 0,		kmsg_Scroll_One_Line_Down
	}
};

Resource 'Bind' (1, "Bindings for File Menu") {
	{
		Cmd, 0x27,				0, 0,		msg_Quit,
//		0, 0,						0, 0,		msg_Revert,
//		0, 0,						0, 0,		msg_SaveACopy,
//		0, 0,						0, 0,		msg_SaveAs,
		Cmd | Shift, 0x3d,		0, 0,		msg_SaveAll,
		Cmd, 0x3d,				0, 0,		msg_Save,
		Cmd | Shift, 0x28,		0, 0,		msg_CloseAll,
		Cmd, 0x28,				0, 0,		msg_Close,
		Cmd, 0x3e,				0, 0,		msg_OpenSelected,
		Cmd | Shift, 0x2f,		0, 0,		msg_FtpOpen,
		Cmd, 0x2f,				0, 0,		msg_Open,
//		0, 0,						0, 0,		msg_NewProject,
		Cmd, 0x51,				0, 0,		msg_New
	}
};

Resource 'Bind' (2, "Bindings for Edit Menu") {
	{
		Cmd, 0x44,				0, 0,		msg_SelectLine,
		Cmd, 0x3c,				0, 0,		'SALL',
		Cmd, 0x4f,				0, 0,		'PSTE',
		Cmd, 0x4e,				0, 0,		'COPY',
		Cmd | Shift, 0x4e,		0, 0,		msg_CopyAppend,
		Cmd, 0x4d,				0, 0,		'CCUT',
		Cmd | Shift, 0x4d,		0, 0,		msg_CutAppend,
		Cmd | Shift, 0x4c,		0, 0,		msg_Redo,
		Cmd, 0x4c,				0, 0,		msg_Undo
	}
};

Resource 'Bind' (3, "Bindings for Text Menu") {
	{
		Cmd, 0x50,				0, 0,		msg_Balance,
		Cmd, 0x11,				0, 0,		msg_Twiddle,
//		0, 0,						0, 0,		msg_ChangeCaseUpper,
//		0, 0,						0, 0,		msg_ChangeCaseLower,
//		0, 0,						0, 0,		msg_ChangeCaseCap,	
//		0, 0,						0, 0,		msg_ConvertEncoding,
		Cmd, 0x31,				0, 0,		msg_ShiftLeft,
		Cmd, 0x32,				0, 0,		msg_ShiftRight,
//		0, 0,						0, 0,		msg_Comment,
//		0, 0,						0, 0,		msg_Uncomment,
//		Cmd, 0x33,				0, 0,		msg_Wrap,
//		0, 0,						0, 0,		msg_Unwrap,
		Cmd | Shift, 0x50,		0, 0,		msg_ShowInBrowser
	}
};

Resource 'Bind' (4, "Bindings for Search Menu") {
	{
		Cmd, 0x53,				0, 0,		msg_GoToLine,	
//		Cmd, 0x2e,				0, 0,		msg_FindNextError,
		Cmd, 0x42,				0, 0,		msg_FindInNextFile,
//		0, 0,						0, 0,		msg_ReplaceAll,
		Cmd | Shift, 0x2b,		0, 0,		msg_ReplaceAndFindBackward,
		Cmd, 0x2b,				0, 0,		msg_ReplaceAndFind,
		Cmd, 0x1d,				0, 0,		msg_Replace,
		Cmd | Shift, 0x29,		0, 0,		msg_EnterReplaceString,
		Cmd, 0x29,				0, 0,		msg_EnterSearchString,
		Cmd | Shift, 0x41,		0, 0,		msg_FindSelectionBackward,
		Cmd, 0x41,				0, 0,		msg_FindSelection,
		Cmd | Shift, 0x40,		0, 0,		msg_FindAgainBackward,
		Cmd, 0x40,				0, 0,		msg_FindAgain,
		Cmd, 0x3f,				0, 0,		msg_FindCmd,
		Cmd, 0x2e,				0, 0, 		msg_IncSearch,
		Cmd | Shift, 0x2e,		0, 0,		msg_IncSearchBackward,
		Cmd | Shift, 0x3f,		0, 0,		msg_FindFunction
	}
};

Resource 'Bind' (5, "Bindings for Window Menu") {
	{
//		0, 0,					0, 0,			msg_FontTabs,
//		0, 0,					0, 0,			msg_Info,
//		0, 0,					0, 0,			msg_Preferences,
//		0, 0,					0, 0,			msg_ChangeWD,
//		0, 0,					0, 0,			msg_ShowGlossary,
//		0, 0,					0, 0,			msg_Stack,
//		0, 0,					0, 0,			msg_Tile,
		Cmd, 0x55,			0, 0,			msg_Zoom,
		Cmd, 0x1b,			0, 0,			msg_Worksheet
	}
};

Resource 'Bind' (6, "Bindings for Help Menu") {
	{
//		0, 0,					0, 0,			msg_About,
		0, 0x02,				0, 0,			msg_Help
	}
};

Resource 'Bind' (7, "Bindings for BeIDE Menu") {
	{
		Cmd, 0x52,			0, 0,			msg_IDE_Make,
//		0, 0,					0, 0,			msg_IDE_Project2Group,
//		0, 0,					0, 0,			msg_IDE_Remove,
//		0, 0,					0, 0,			msg_IDE_Add,
//		0, 0,					0, 0,			msg_IDE_BringToFront
	}
};

Resource 'Bind' (8, "Miscellaneous Bindings") {
	{
		Ctrl, 0x4e,			0, 0,			msg_CancelCommand,
		Cmd, 0x47,			0, 0,			msg_Execute,
		0, 0x5b,				0, 0,			msg_Execute,
		Cmd, 0x26,			0, 0,			msg_SwitchHeaderSource
	}
};

Resource 'Bind' (9, "Emacs Bindings") {
	{
		Ctrl, 0x2d,			0, 0,			kmsg_NrArgument,
		Ctrl, 0x44,			0, 0,			kmsg_Recenter,
		Ctrl, 0x2f,			0, 0,			kmsg_OpenLine,
		Ctrl, 0x28,			0, 0x01,		kmsg_AppendNextCut,
//		Cmd, 0x26,			0, 0,			kmsg_ClearRegion,	
		0, 0x28,				0, 0x01,		kmsg_CopyRegion,
		Ctrl, 0x28,			0, 0,			kmsg_CutRegion,
		Ctrl, 0x4d,			Ctrl, 0x4d,	kmsg_ExchangeMarkAndPoint,
		Shift, 0x13,			0, 0x01,		kmsg_MarkWord,
		0, 0x41,				Ctrl, 0x4d,	kmsg_MarkAll,
		Ctrl, 0x5e,			0, 0,			kmsg_Mark,
		Ctrl, 0x34,			0, 0,			kmsg_Cut_Word_Backward,
		0, 0x3e,				0, 0x01,		kmsg_Cut_Word,
		Ctrl, 0x43,			0, 0,			kmsg_Cut_to_End_of_Line,
		Ctrl, 0x2c,			0, 0, 			'PSTE',
		0, 0x13,				Ctrl, 0x4d,	kmsg_SplitWindow,
		0, 0x12,				Ctrl, 0x4d,	kmsg_UnsplitWindow,
		0, 0x2f,				Ctrl, 0x4d,	kmsg_SwitchActivePart
	}
};
