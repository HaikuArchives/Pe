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

resource 'MENU' (90, "Font menu") {
	"Font",
	{
	}
};

resource 'MENU' (91, "Language menu") {
	"Language",
	{
	}
};

resource 'MENU' (92, "Saved State") {
	"Language",
	{
		Item		{ "Pe", 'chng', none, noKey },
		Item		{ "BeIDE", 'chng', none, noKey }
	}
};

resource 'MENU' (93, "Language menu") {
	"Language",
	{
		Item		{ "None", 'chng', none, noKey },
		Separator	{}
	}
};

resource 'DLOG' (132, "Preferences") {
	{0, 0, 400, 260},
	"Preferences",
	B_TITLED_WINDOW,
	NORMAL,
	{
		TabbedBook					{{   0,   0, 400, 260 }, "tabB" },
			TabSheet				{ "Colours", "The colours used for syntax colouring and window layout" },
				ColorControl		{{   0,   0, 145,  16 }, "lowc", "Background" },
				ColorControl		{{ 155,   0, 290,  16 }, "txtc", "Normal Text" },
				ColorControl		{{   0,  18, 145,  34 }, "selc", "Selection" },
				ColorControl		{{ 155,  18, 290,  34 }, "mrkc", "Mark" },
				
				Line				{{   0,  39, 290,  40 }},

				ColorControl		{{   0,  45, 145,  61 }, "keyc", "Keywords" },
				ColorControl		{{ 155,  45, 290,  61 }, "cmtc", "Comments" },
				ColorControl		{{   0,  63, 145,  79 }, "strc", "Strings" },
				ColorControl		{{ 155,  63, 290,  79 }, "chcc", "Char Constants" },
				
				Line				{{   0,  84, 290,  85 }},

				ColorControl		{{   0,  90, 145, 106 }, "us1c", "User defined 1" },
				ColorControl		{{ 155,  90, 290, 106 }, "us2c", "User defined 2" },
				ColorControl		{{   0, 108, 145, 124 }, "us3c", "User defined 3" },
				ColorControl		{{ 155, 108, 290, 124 }, "us4c", "User defined 4" },
				
				Line				{{   0, 129, 290, 130 }},

				ColorControl		{{   0, 135, 145, 151 }, "tagc", "HTML Tags" },
				ColorControl		{{ 155, 135, 290, 151 }, "tgsc", "HTML Tagstring" },

//				ColorControl		{{   0, 135, 145, 151 }, "tag color", "HTML Tags" },
//				ColorControl		{{ 155, 135, 290, 151 }, "anchor color", "HTML Anchor" },
//				ColorControl		{{   0, 153, 145, 169 }, "image color", "HTML Image" },
//				ColorControl		{{ 155, 153, 290, 169 }, "tagstring color", "HTML Tagstring" },
			TabSheetEnd				{},

			TabSheet				{ "Connections", "Options that control the connections to other programs" },
				CheckBox			{{   0,   0, 200,  16 }, "idmn", "Show Metrowerks BeIDE Menu" },
				CheckBox			{{   0,  20, 200,  36 }, "mwpl", "Load Metrowerks BeIDE Extensions" },
			TabSheetEnd				{},
			
			TabSheet				{ "Differences", "Options that control the Find Differences command" },
				CheckBox			{{   0,   0, 200,  16 }, "dica", "Ignore Case Change" },
				CheckBox			{{   0,  20, 200,  36 }, "diwh", "Ignore White Space" },
			TabSheetEnd				{},

			TabSheet				{ "Editor", "How the editor should behave" },
				CheckBox			{{   0,   0, 140,  16 }, "stab", "Show Tabstops" },
				Edit				{{ 150,   0, 290,  16 }, "tabs", "Spaces per Tab:", "", "0123456789", 3, 90 },

				CheckBox			{{   0,  20, 140,  36 }, "syco", "Syntax Colouring" },
				CheckBox			{{ 150,  20, 290,  36 }, "auin", "Auto Indent" },

				CheckBox			{{   0,  40, 140,  56 }, "baty", "Balance While Typing" },
				CheckBox			{{ 150,  40, 290,  56 }, "smbr", "Smart Braces" },
				
				CheckBox			{{   0,  60, 140,  76 }, "blcu", "Block Cursor" },
				CheckBox			{{ 150,  60, 290,  76 }, "flcu", "Flashing Cursor" },
				
				CheckBox			{{   0,  80, 140,  96 }, "smwo", "Smart Workspaces" },
				CheckBox			{{ 150,  80, 290,  96 }, "alho", "Alternate Home" },
											
				Line				{{   0, 103, 290, 104 }},

				PopupMenu			{{   0, 111, 190, 127 }, "font", "Font:", 90, 50 },
				Edit				{{ 200, 111, 290, 127 }, "fons", "Size:", "", "0123456789", 2, 40 },

				PopupMenu			{{   0, 131, 190, 147 }, "alfo", "Alt. Font:", 90, 50 },
				Edit				{{ 200, 131, 290, 147 }, "alfs", "Size:", "", "0123456789", 2, 40 },

			TabSheetEnd				{},

			TabSheet				{ "File Types", "These types show up in the Save Dialogbox" },
				StringListBox		{{   0,   0, 290, 120 }, "mimetypes" },
			TabSheetEnd				{},

			TabSheet				{ "Files", "Options for files on disk" },
				Caption				{{   0,   0, 120,  16 }, "sv", "When saving a file" },

				CheckBox			{{ 140,   0, 290,  16 }, "coat", "Restore attributes" },
				CheckBox			{{ 140,  20, 290,  36 }, "maba", "Make backup (filename~)" },
				CheckBox			{{ 140,  40, 290,  56 }, "fonl", "End with newline" },
				
				Line				{{   0,  63, 290,  64 }},
				
				CheckBox			{{   0,  71, 290,  87 }, "vofm", "Verify open files for modification" },
				
				Line				{{   0,  94, 290,  95 }},

				Edit				{{   0, 102, 180, 118 }, "resi", "Nr of Recent Documents", "", "0123456789", 2, 140 },
				CheckBox			{{ 190, 102, 290, 118 }, "sktm", "Ignore tmp files" },
				
				Line				{{   0, 125, 290, 126 }},

				CheckBox			{{   0, 133, 120, 149 }, "zoop", "Zoom Open Panel" },
				CheckBox			{{ 140, 133, 290, 149 }, "fupa", "Full Path in Window Title" },

			TabSheetEnd				{},
			TabSheet				{ "Glossary", "Glossary options" },
				Button				{{   0,   0, 110,  20 }, "reloadglos", "Reload Glossary", 'RelG' },
				CheckBox			{{   0,  30, 290,  46 }, "shgl", "Show Glossary Window at startup" },
				CheckBox			{{   0,  50, 290,  66 }, "scgl", "Act on single click" },
			TabSheetEnd				{},
			
			TabSheet				{ "Grep Patterns", "Standard Grep Patterns to use in the Find Dialog" },
				
				GrepListBox			{{   0,   0, 290, 170 }, "grls" },
				
			TabSheetEnd				{},
			
			TabSheet				{ "Groups", "Group file options" },
				CheckBox			{{   0,   0, 280,  16 }, "sogr", "Sort the files inside a group" },
				CheckBox			{{   0,  20, 280,  36 }, "rlgp", "Store relative paths in a group file" },
			TabSheetEnd				{},
			
			TabSheet				{ "Include Paths", "Paths to use when searching for include files" },
				CheckBox			{{   0,   0, 280,  16 }, "sepa", "Search Parent" },
				CheckBox			{{   0,  20, 280,  36 }, "incl", "Search the paths in $BEINCLUDES" },
				Caption				{{   0,  40, 200,  56 }, "more", "And search the following paths:" },
				PathBox				{{   0,  60, 290, 160 }, "incP" },
			TabSheetEnd				{},
			
			TabSheet				{ "Invisibles", "Options for the way invisible characters are displayed" },
				Edit				{{   0,   0, 190,  16 }, "scca", "Show Control Characters as:", "¿", "", 1, 170 },
				
				Line				{{   0,  21, 290,  22 }},
				
				Caption				{{   0,  27, 290,  43 }, "invc", "Options below are effective only when showing invisibles" },
				
				ColorControl		{{   0,  47, 190,  63 }, "invc", "Invisibles Colour" },
				Edit				{{   0,  67, 190,  83 }, "stca", "Show Tab Character as:", "", "", 1, 170 },
				Edit				{{   0,  87, 190, 103 }, "ssca", "Show Space Character as:", "", "", 1, 170 },
				Edit				{{   0, 107, 190, 123 }, "srca", "Show Return Character as:", "", "", 1, 170 },
			TabSheetEnd				{},
			
			TabSheet				{ "Keybindings", "The bindings between key shortcuts and actions" },

				Caption				{{   0,   0,  40,  16 }, "actcap", "Action:" },
				OutlineList			{{  42,   2, 288, 118 }, "kact" },
				
				Caption				{{   0, 126,  40, 142 }, "keycap", "Keys:" },
				List				{{  42, 128, 168, 166 }, "keys" },

				KeyCapture			{{ 180, 126, 290, 142 }, "kebi" },

				Button				{{ 180, 146, 230, 166 }, "adke", "Add", 'AddK' },
				Button				{{ 240, 146, 290, 166 }, "deke", "Delete", 'DelK' },
			TabSheetEnd				{},
			
			TabSheet				{ "Languages", "Options for Syntax Colouring and the Function Popup" },
				CheckBox			{{   0,   0, 140,  16 }, "shin", "Show Includes" },
				CheckBox			{{   0,  20, 140,  36 }, "shpr", "Show Function Prototypes" },
				CheckBox			{{ 150,   0, 290,  16 }, "shty", "Show Types" },
				CheckBox			{{ 150,  20, 290,  36 }, "sopo", "Sort Alphabetically" },
				
				Line				{{   0,  40, 290,  41 }},
				
				Caption				{{   0,  50, 200,  66 }, "lc", "Suffix Mapping:" },
				PopupMenu			{{   0,  70, 120,  86 }, "lang", "Language:", 91, 55 },
				Edit				{{ 130,  70, 290,  86 }, "sufx", "Suffixes:", "", "", 0, 45 },
				
				Line				{{   0,  93, 290,  94 }},
				
				PopupMenu			{{   0, 101, 200, 117 }, "dela", "Default Language:", 93, 90 },
			TabSheetEnd				{},
			
			TabSheet				{ "Searching", "Paths in the directory popup of the Find Dialog and more" },
				PathBox				{{   0,   0, 290, 100 }, "srcP" },
				
				Line				{{   0, 107, 290, 108 }},
				
				CheckBox			{{   0, 115, 200, 131 }, "isic", "Incremental Search Ignores Case" },
				CheckBox			{{   0, 135, 200, 151 }, "cesf", "Center the String Found" },
				
			TabSheetEnd				{},

			TabSheet				{ "Startup", "What to do when Pe is launched" },
				RadioButton			{{   0,   0, 145,  16 }, "swne", "Create New Document" },
				RadioButton			{{   0,  20, 145,  36 }, "swop", "Show Open Dialog" },
				RadioButton			{{   0,  40, 290,  56 }, "swno", "Do Nothing" },
				Caption				{{  40,  56, 290,  72 }, "s_w_c", " (only effective in combination with a worksheet)" },
				
				Line				{{   0,  79, 290,  80 }},

				CheckBox			{{   0,  87, 290, 103 }, "shtp", "Show HTML Palette at startup" },
			TabSheetEnd				{},

			TabSheet				{ "State", "What to do with state information in saved documents" },
				Caption				{{   0,   0, 140,  16 }, "state", "When opening a file restore" },
				CheckBox			{{   0,  20, 140,  36 }, "repo", "Position" },
				CheckBox			{{   0,  40, 140,  56 }, "refo", "Font Settings" },
				CheckBox			{{   0,  60, 140,  76 }, "rese", "Selection" },
				CheckBox			{{   0,  80, 140,  96 }, "resc", "Scrollbar Settings" },
				CheckBox			{{   0, 100, 140, 116 }, "recw", "Working Directory" },
				PopupMenu			{{   0, 120, 140, 140 }, "sast", "Use state format:", 92, 90 },
			TabSheetEnd				{},

			TabSheet				{ "Worksheet", "Options having effect on worksheets" },
				CheckBox			{{   0,   0, 284,  16 }, "wosh", "Always keep a worksheet open" },
				CheckBox			{{   0,  20, 284,  36 }, "rdse", "Redirect stderr to Error Window" },
				StdErrBox			{{   0,  40, 284, 166 }, "StEb" },
			TabSheetEnd				{},
			
			TabSheet				{ "Wrapping", "Options for softwrapping" },
				CheckBox			{{   0,   0, 100,  16 }, "sowr", "Soft Wrap Files" },
				
				Caption				{{   0,  40, 238,  56 }, "w", "And when wrapping use these settings:" },
				
				RadioButton			{{   0,  60, 100,  76 }, "wrwi", "Window Width" },
				RadioButton			{{   0,  80, 100,  96 }, "wrpa", "Paper Width" },
				RadioButton			{{   0, 100,  98, 116 }, "wrfi", "Fixed Column:" },
				
				Edit				{{ 100, 100, 140, 116 }, "wrft", "", "", "0123456789", 3, 0 },
				
			TabSheetEnd				{},
			
			Button					{{ 330, 224, 390, 244 }, "ok  ", "Apply", 'ok  ' },
			Button					{{ 260, 224, 320, 244 }, "cncl", "Revert", 'cncl' },
		TabbedBookEnd				{}
	}
};
