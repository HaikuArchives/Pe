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

#include "RTypes.r"
#include "PMessages.h"

resource 'MENU' (0, "File Menu") {
	"File",
	{
		Item		{ "New", msg_New, none, 'N' },
		Item		{ "New Project", msg_NewProject, none, noKey },
		Item		{ "Open…", msg_Open, none, 'O' },
		Item		{ "Open from Server…", msg_FtpOpen, none, noKey },
		Item		{ "Open Selection", msg_OpenSelected, none, 'D' },
		Submenu		{ 10 },
		Item		{ "Close", msg_Close, none, 'W' },
		Separator	{ },
		Item		{ "Save", msg_Save, none, 'S' },
		Item		{ "Save As…", msg_SaveAs, none, noKey },
		Item		{ "Save on Server…", msg_FtpSave, none, noKey },
		Item		{ "Save a Copy As…", msg_SaveACopy, none, noKey },
		Item		{ "Revert…", msg_Revert, none, noKey },
		Separator	{ },
//		Item		{ "Page Setup…", msg_PageSetup, none, noKey },
//		Item		{ "Print…", msg_Print, none, 'P' },
//		Separator	{ },
		Item		{ "Quit", msg_Quit, none, 'Q' }
	}
};

resource 'MENU' (1, "Edit Menu") {
	"Edit",
	{
		Item		{ "Undo", msg_Undo, none, 'Z' },
		Item		{ "Redo", msg_Redo, shift, 'Z' },
		Separator	{ },
		Item		{ "Cut", 'CCUT', none, 'X' },
		Item		{ "Copy", 'COPY', none, 'C' },
		Item 		{ "Paste", 'PSTE', none, 'V' },
		Item		{ "Clear", msg_Clear, none, noKey },
		Separator	{ },
		Item		{ "Select All", 'SALL', none, 'A' },
		Item		{ "Select Line", msg_SelectLine, none, 'L' },
		Item		{ "Select Paragraph", msg_SelectParagraph, none, noKey }
	}
};

resource 'MENU' (2, "Text Menu") {
	"Text",
	{
		Item		{ "Balance", msg_Balance, none, 'B' },
		Item		{ "Twiddle", msg_Twiddle, none, '\'' },
		Submenu	{ 11 },
		Item		{ "Change Encoding…", msg_ConvertEncoding, none, noKey },
		Separator	{},
		Item		{ "Shift Left", msg_ShiftLeft, none, '[' },
		Item		{ "Shift Right", msg_ShiftRight, none, ']' },
		Separator	{},
		Item		{ "Comment", msg_Comment, none, noKey },
		Item		{ "Uncomment", msg_Uncomment, none, noKey },
		Separator	{},
		Item		{ "Justify", msg_Justify, none, noKey },
		Item		{ "Insert Linebreaks", msg_Wrap, none, noKey },
		Item		{ "Remove Linebreaks", msg_Unwrap, none, noKey },
		Separator	{},
		Item		{ "Show in Browser", msg_ShowInBrowser, shift, 'B' }
	}
};

resource 'MENU' (3, "Search Menu") {
	"Search",
	{
		Item		{ "Find…", msg_FindCmd, none, 'F' },
		Item		{ "Find Again", msg_FindAgain, none, 'G' },
		Item		{ "Find Selection", msg_FindSelection, none, 'H' },
		Item		{ "Enter Search String", msg_EnterSearchString, none, 'E' },
		Separator	{},
		Item		{ "Replace", msg_Replace, none, '=' },
		Item		{ "Replace & Find", msg_ReplaceAndFind, none, 'T' },
		Item		{ "Replace All", msg_ReplaceAll, none, noKey },
		Separator	{},
		Item		{ "Find in Next File", msg_FindInNextFile, none, 'J' },
		Separator	{},
		Item		{ "Incremental Search", msg_IncSearch, none, 'I' },
		Separator	{},
		Item		{ "Jump to Next Error", msg_FindNextError, none, noKey },
		Item		{ "Go To Line…", msg_GoToLine, none, ',' },
		Separator	{},
		Item		{ "Find Differences…", msg_FindDifferences, none, noKey }
	}
};

resource 'MENU' (4, "Window Menu") {
	"Window",
	{
		Item		{ "File Options…", msg_Info, none, noKey },
		Item		{ "Preferences…", msg_Preferences, none, noKey },
		Item		{ "Working Directory…", msg_ChangeWD, none, noKey },
		Item		{ "Glossary", msg_ShowGlossary, none, noKey },
		Item		{ "HTML Palette", msg_ShowHTMLPalette, none, noKey },
		Separator	{},
		Item		{ "Stack", msg_Stack, none, noKey },
		Item		{ "Tile", msg_Tile, none, noKey },
		Item		{ "Zoom", msg_Zoom, none, '/' },
		Separator	{},
		Item		{ "Worksheet", msg_Worksheet, none, '0' },
		Separator	{}
	}
};

resource 'MENU' (5, "Help Menu") {
	"Help",
	{
		Item		{ "Help…", msg_Help, none, noKey },
		Separator	{ },
		Item		{ "About…", msg_About, none, noKey }
	}
};

resource 'MENU' (6, "Extensions") {
	"Extensions",
	{
	}
};

resource 'MENU' (7, "BeIDE") {
	"BeIDE",
	{
		Item		{ "Bring to Front", msg_IDE_BringToFront, none, noKey },
		Separator	{ },
		Item		{ "Add File", msg_IDE_Add, none, noKey },
		Item		{ "Remove File", msg_IDE_Remove, none, noKey },
		Item		{ "Project -> Group", msg_IDE_Project2Group, none, noKey },
		Separator	{ },
		Item		{ "Make", msg_IDE_Make, none, 'M' }
	}
};

resource 'MENU' (10, "Recent Menu") {
	"Open Recent",
	{
	}
};

resource 'MENU' (11, "Change Case Menu") {
	"Change Case",
	{
		Item		{ "Upper Case", msg_ChangeCaseUpper, none, noKey },
		Item		{ "Lower Case", msg_ChangeCaseLower, none, noKey },
		Item		{ "Capitalize Words", msg_ChangeCaseCap, none, noKey },
		Item		{ "Capitalize Lines", msg_ChangeCaseLine, none, noKey },
		Item		{ "Capitalize Sentences", msg_ChangeCaseSent, none, noKey }
	}
};

resource 'MBAR' (0, "Menu Bar") {
	{
		0, 1, 2, 3, 6, 4, 5
	}
};

resource 'MENU' (20, "Mime types") {
	"Mime Type",
	{
	}
};

// Contextual menus
//
resource 'MENU' (30, "Main view") {
	"pop-up",
	{
		Item		{ "Save", msg_Save, none, noKey },
		Separator	{},
		Item		{ "Cut", 'CCUT', none, noKey },
		Item		{ "Copy", 'COPY', none, noKey },
		Item 		{ "Paste", 'PSTE', none, noKey },
		Item		{ "Clear", msg_Clear, none, noKey },
		Item		{ "Select All", 'SALL', none, noKey },
		Separator	{},
		Item		{ "Comment", msg_Comment, none, noKey },
		Item		{ "Uncomment", msg_Uncomment, none, noKey },
		Separator	{},
		Item		{ "Syntax Colouring", msg_ToggleSyntaxColoring, none, noKey },
		Item		{ "Show Invisibles", msg_ToggleShowInvisibles, none, noKey },
		Item		{ "Soft Wrap", msg_SoftWrap, none, noKey },
		Item		{ "Alternate Font", msg_ToggleFont, none, noKey }
	}
};
