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


resource rt_POP (ri_POP_PRF_FONT, "Font menu") {
	"Font",
	{
	}
};

resource rt_POP (ri_POP_PRF_LANG, "Language menu") {
	"Language",
	{
	}
};

resource rt_POP (ri_POP_PRF_STATE, "Saved State") {
	"Language",
	{
		Item		{ "Pe",		'chng',	none,	noKey	},
		Item		{ "BeIDE",	'chng',	none,	noKey	}
	}
};

resource rt_POP (ri_POP_PRF_DEFLANG, "Default Language menu") {
	"Language",
	{
		Item		{ "None",	'chng',	none,	noKey	},
		Separator	{									}
	}
};

resource 'DLOG' (132, "Preferences") {
	{0, 0, 415, 266}, "Preferences", B_TITLED_WINDOW, NORMAL,
	{
		TabbedBook				{{   0,   0, 415, 266 }, "tabB" 															},
			TabSheet			{ "Colours", "The colours used for syntax colouring and window layout"						},
				ColorControl	{{   0,   0, 145,  16 }, "lowc",		"Background"										},
				ColorControl	{{ 155,   0, 290,  16 }, "txtc",		"Normal Text"										},

				ColorControl	{{   0,  16, 145,  32 }, "selc",		"Selection"											},
				ColorControl	{{ 155,  16, 290,  32 }, "mrkc",		"Mark"												},
				
				Line			{{   0,  36, 290,  37 }																		},

				ColorControl	{{   0,  40, 110,  56 }, "keyc",		"Keywords"											},
				ColorControl	{{ 110,  40, 145,  56 }, "ke2c",		""													},
				ColorControl	{{ 155,  40, 255,  56 }, "cmtc",		"Comments"											},
				ColorControl	{{ 255,  40, 290,  56 }, "cm2c",		""													},
				
				ColorControl	{{   0,  56, 110,  72 }, "strc",		"Strings"											},
				ColorControl	{{ 110,  56, 145,  72 }, "tgsc",		""													},
				ColorControl	{{ 155,  56, 255,  72 }, "numc",		"Numbers"											},
				ColorControl	{{ 255,  56, 290,  72 }, "nu2c",		""													},

				ColorControl	{{   0,  72, 110,  88 }, "opec",		"Operators"											},
				ColorControl	{{ 110,  72, 145,  88 }, "op2c",		""													},
				ColorControl	{{ 155,  72, 255,  88 }, "sepc",		"Separators"										},
				ColorControl	{{ 255,  72, 290,  88 }, "se2c",		""													},
				
				ColorControl	{{   0,  88, 110, 104 }, "proc",		"PreProcessor"										},
				ColorControl	{{ 110,  88, 145, 104 }, "pr2c",		""													},
				ColorControl	{{ 155,  88, 255, 104 }, "errc",		"Errors"											},
				ColorControl	{{ 255,  88, 290, 104 }, "er2c",		""													},
				
				Line			{{   0, 108, 290, 109 }},
				
				ColorControl	{{   0, 112, 145, 128 }, "sidc",		"System Identifiers"								},
				ColorControl	{{ 155, 112, 290, 128 }, "chcc",		"Char Constants"									},
				ColorControl	{{   0, 128, 145, 144 }, "uidc",		"User Identifiers"									},
				ColorControl	{{ 155, 128, 290, 144 }, "tagc",		"HTML Tags"											},
				
				Line			{{   0, 148, 290, 149 }																		},

				ColorControl	{{   0, 152, 145, 168 }, "us1c",		"User defined 1"									},
				ColorControl	{{ 155, 152, 290, 168 }, "us2c",		"User defined 2"									},
				ColorControl	{{   0, 168, 145, 184 }, "us3c",		"User defined 3"									},
				ColorControl	{{ 155, 168, 290, 184 }, "us4c",		"User defined 4"									},

			TabSheetEnd			{																							},

			TabSheet			{ "Connections", "Options that control the connections to other programs"					},
				CheckBox		{{   0,   0, 200,  16 }, "idmn",		"Show Metrowerks BeIDE Menu"						},
				CheckBox		{{   0,  20, 200,  36 }, "mwpl",		"Load Metrowerks BeIDE Extensions"					},
				CheckBox		{{   0,  50, 200,  66 }, "pftp",		"Use Passive FTP by default"						},
			TabSheetEnd			{																							},
			
			TabSheet			{ "Differences", "Options that control the Find Differences command"						},
				CheckBox		{{   0,   0, 200,  16 }, "dica",		"Ignore Case Change"								},
				CheckBox		{{   0,  20, 200,  36 }, "diwh",		"Ignore White Space"								},
			TabSheetEnd			{																							},

			TabSheet			{ "Editor", "How the editor should behave"													},
				CheckBox		{{   0,   0, 140,  16 }, "stab",		"Show Tabstops"										},
				Edit			{{ 150,   0, 290,  16 }, "tabs",		"Spaces per Tab:",			"", "0123456789", 3, 90	},

				CheckBox		{{   0,  20, 140,  36 }, "syco",		"Syntax Colouring"									},
				CheckBox		{{ 150,  20, 290,  36 }, "auin",		"Auto Indent"										},

				CheckBox		{{   0,  40, 140,  56 }, "baty",		"Balance While Typing"								},
				CheckBox		{{ 150,  40, 290,  56 }, "smbr",		"Smart Braces"										},
				
				CheckBox		{{   0,  60, 140,  76 }, "blcu",		"Block Cursor"										},
				CheckBox		{{ 150,  60, 290,  76 }, "flcu",		"Flashing Cursor"									},
				
				CheckBox		{{   0,  80, 140,  96 }, "smwo",		"Smart Workspaces"									},
				CheckBox		{{ 150,  80, 290,  96 }, "alho",		"Alternate Home"									},
											
				Line			{{   0, 103, 290, 104 }},

				PopupMenu		{{   0, 111, 190, 127 }, "font",		"Font:",					ri_POP_PRF_FONT, 50		},
				Edit			{{ 200, 111, 290, 127 }, "fons",		"Size:",					"", "0123456789", 2, 40	},

				PopupMenu		{{   0, 131, 190, 147 }, "alfo",		"Alt. Font:",				ri_POP_PRF_FONT, 50		},
				Edit			{{ 200, 131, 290, 147 }, "alfs",		"Size:",					"", "0123456789", 2, 40	},

			TabSheetEnd			{																							},

			TabSheet			{ "File Types", "These types show up in the Save Dialogbox"									},
				StringListBox	{{   0,   0, 290, 120 }, "mimetypes"														},
			TabSheetEnd			{																							},

			TabSheet			{ "Files", "Options for files on disk"														},
				Caption			{{   0,   0, 120,  16 }, "sv",			"When saving a file"								},

				CheckBox		{{ 140,   0, 290,  16 }, "coat",		"Restore attributes"								},
				CheckBox		{{ 140,  20, 290,  36 }, "maba",		"Make backup (filename~)"							},
				CheckBox		{{ 140,  40, 290,  56 }, "fonl",		"End with newline"									},
				
				Line			{{   0,  63, 290,  64 }},
				
				CheckBox		{{   0,  71, 290,  87 }, "vofm",		"Verify open files for modification"				},
				
				Line			{{   0,  94, 290,  95 }},

				Edit			{{   0, 102, 180, 118 }, "resi",		"Nr of Recent Documents", "", "0123456789", 2, 140	},
				CheckBox		{{ 190, 102, 290, 118 }, "sktm",		"Ignore tmp files"									},
				
				Line			{{   0, 125, 290, 126 }},

				CheckBox		{{   0, 133, 120, 149 }, "zoop",		"Zoom Open Panel"									},
				CheckBox		{{ 140, 133, 290, 149 }, "fupa",		"Full Path in Window Title"							},

			TabSheetEnd			{																							},
			TabSheet			{ "Glossary", "Glossary options" },
				Button			{{   0,   0, 110,  20 }, "reloadglos",	"Reload Glossary",						'RelG'		},
				CheckBox		{{   0,  30, 290,  46 }, "shgl",		"Show Glossary Window at startup"					},
				CheckBox		{{   0,  50, 290,  66 }, "scgl",		"Act on single click"								},
			TabSheetEnd			{																							},
			
			TabSheet			{ "Grep Patterns", "Standard Grep Patterns to use in the Find Dialog"						},
				
				GrepListBox		{{   0,   0, 290, 170 }, "grls"																},
				
			TabSheetEnd			{																							},
			
			TabSheet			{ "Groups", "Group file options" },
				CheckBox		{{   0,   0, 280,  16 }, "sogr",		"Sort the files inside a group"						},
				CheckBox		{{   0,  20, 280,  36 }, "rlgp",		"Store relative paths in a group file"				},
			TabSheetEnd			{																							},
			
			TabSheet			{ "Include Paths", "Paths to use when searching for include files"							},
				CheckBox		{{   0,   0, 280,  16 }, "sepa",		"Search Parent"										},
				CheckBox		{{   0,  20, 280,  36 }, "incl",		"Search the paths in $BEINCLUDES"					},
				Caption			{{   0,  40, 200,  56 }, "more",		"And search the following paths:"					},
				PathBox			{{   0,  60, 290, 160 }, "incP" },
			TabSheetEnd			{																							},
			
			TabSheet			{ "Invisibles", "Options for the way invisible characters are displayed"					},
				Edit			{{   0,   0, 190,  16 }, "scca",		"Show Control Characters as:",		"¿", "", 1, 170	},
				
				Line			{{   0,  21, 290,  22 }},
				
				Caption			{{   0,  27, 290,  43 }, "invc",		"Options below are effective only when showing invisibles" },
				
				ColorControl	{{   0,  47, 190,  63 }, "invc",		"Invisibles Colour"									},
				Edit			{{   0,  67, 190,  83 }, "stca",		"Show Tab Character as:",			"", "", 1, 170	},
				Edit			{{   0,  87, 190, 103 }, "ssca",		"Show Space Character as:",			"", "", 1, 170	},
				Edit			{{   0, 107, 190, 123 }, "srca",		"Show Return Character as:",		"", "", 1, 170	},
			TabSheetEnd			{																							},
			
			TabSheet			{ "Keybindings", "The bindings between key shortcuts and actions"							},

				Caption			{{   0,   0,  40,  16 }, "actcap",		"Action:"											},
				OutlineList		{{  42,   2, 288, 131 }, "kact"																},
				
				Caption			{{   0, 139,  40, 155 }, "keycap",		"Keys:"												},
				List			{{  42, 141, 168, 179 }, "keys"																},

				KeyCapture		{{ 180, 139, 290, 155 }, "kebi"																},

				Button			{{ 180, 159, 230, 179 }, "adke",		"Add",							'AddK'				},
				Button			{{ 240, 159, 290, 179 }, "deke",		"Delete",						'DelK'				},
			TabSheetEnd			{																							},
			
			TabSheet			{ "Languages", "Options for Syntax Colouring and the Function Popup"						},
				CheckBox		{{   0,   0, 140,  16 }, "shin",		"Show Includes"										},
				CheckBox		{{   0,  20, 140,  36 }, "shpr",		"Show Function Prototypes"							},
				CheckBox		{{ 150,   0, 290,  16 }, "shty",		"Show Types"										},
				CheckBox		{{ 150,  20, 290,  36 }, "sopo",		"Sort Alphabetically"								},
				
				Line			{{   0,  40, 290,  41 }},
				
				Caption			{{   0,  50, 200,  66 }, "lc",			"Suffix Mapping:"									},
				PopupMenu		{{   0,  70, 120,  86 }, "lang",		"Language:",				ri_POP_PRF_LANG, 55		},
				Edit			{{ 130,  70, 290,  86 }, "sufx",		"Suffixes:",				"", "", 0, 45			},
				
				Line			{{   0,  93, 290,  94 }},
				
				PopupMenu		{{   0, 101, 200, 117 }, "dela",		"Default Language:",		ri_POP_PRF_DEFLANG, 90	},
			TabSheetEnd			{																							},
			
			TabSheet			{ "Projects", "Project window options"														},
				CheckBox		{{   0,   0, 280,  16 }, "prso",		"Sort the files inside a project"					},
				CheckBox		{{   0,  20, 280,  36 }, "prad",		"Autodetect mimetypes of 'Jamfile' & 'Makefile'"	},
			TabSheetEnd			{																							},

			TabSheet			{ "Searching", "Paths in the directory popup of the Find Dialog and more"					},
				PathBox			{{   0,   0, 290, 100 }, "srcP"																},
				
				Line			{{   0, 107, 290, 108 }},
				
				CheckBox		{{   0, 115, 200, 131 }, "isic",		"Incremental Search Ignores Case"					},
				CheckBox		{{   0, 135, 200, 151 }, "cesf",		"Center the String Found"							},
				
			TabSheetEnd			{																							},

			TabSheet			{ "Startup", "What to do when Pe is launched"												},
				RadioButton		{{   0,   0, 145,  16 }, "swne",		"Create New Document"								},
				RadioButton		{{   0,  20, 145,  36 }, "swop",		"Show Open Dialog"									},
				RadioButton		{{   0,  40, 290,  56 }, "swno",		"Do Nothing"										},
				Caption			{{  40,  56, 290,  72 }, "s_w_c",		" (only effective in combination with a worksheet)"	},
				
				Line			{{   0,  79, 290,  80 }},

				CheckBox		{{   0,  87, 290, 103 }, "shtp",		"Show HTML Palette"									},
				CheckBox		{{  20, 107, 290, 123 }, "shtP",		"Only when editing HTML files"						},
			TabSheetEnd			{																							},

			TabSheet			{ "State", "What to do with state information in saved documents"							},
				Caption			{{   0,   0, 140,  16 }, "state",		"When opening a file restore"						},
				CheckBox		{{   0,  20, 140,  36 }, "repo",		"Position"											},
				CheckBox		{{   0,  40, 140,  56 }, "refo",		"Font Settings"										},
				CheckBox		{{   0,  60, 140,  76 }, "rese",		"Selection"											},
				CheckBox		{{   0,  80, 140,  96 }, "resc",		"Scrollbar Settings"								},
				CheckBox		{{   0, 100, 140, 116 }, "recw",		"Working Directory"									},
				PopupMenu		{{   0, 120, 140, 140 }, "sast",		"Use state format:",		ri_POP_PRF_STATE, 90	},
			TabSheetEnd			{																							},

			TabSheet			{ "Worksheet", "Options having effect on worksheets"										},
				CheckBox		{{   0,   0, 284,  16 }, "wosh",		"Always keep a worksheet open"						},
				CheckBox		{{   0,  20, 284,  36 }, "rdse",		"Redirect stderr to Error Window"					},
				StdErrBox		{{   0,  40, 284, 166 }, "StEb"																},
			TabSheetEnd			{																							},
			
			TabSheet			{ "Wrapping", "Options for softwrapping" },
				CheckBox		{{   0,   0, 100,  16 }, "sowr",		"Soft Wrap Files"									},
				
				Caption			{{   0,  40, 238,  56 }, "w",			"And when wrapping use these settings:"				},
				
				RadioButton		{{   0,  60, 100,  76 }, "wrwi",		"Window Width"										},
				RadioButton		{{   0,  80, 100,  96 }, "wrpa",		"Paper Width"										},
				RadioButton		{{   0, 100,  98, 116 }, "wrfi",		"Fixed Column:"										},
				
				Edit			{{ 100, 100, 140, 116 }, "wrft",		"",					"", "0123456789", 3, 0			},
				
			TabSheetEnd			{																							},
			
			Button				{{ 335, 235, 390, 255 }, "ok  ",		"Apply",			'ok  '							},
			Button				{{ 265, 235, 320, 255 }, "cncl",		"Revert",			'cncl'							},
		TabbedBookEnd			{}
	}
};
