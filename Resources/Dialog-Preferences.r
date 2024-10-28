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
#include "PrefControls.h"


resource rtyp_Popu (rid_Popu_PrfFont, "Font menu") {
	"Font",
	{
	}
};

resource rtyp_Popu (rid_Popu_PrfLang, "Language menu") {
	"Language",
	{
	}
};

resource rtyp_Popu (rid_Popu_PrfState, "Saved state") {
	"Language",
	{
		Item		{ "Pe",		'chng',	none,	noKey	},
		Item		{ "BeIDE",	'chng',	none,	noKey	}
	}
};

resource rtyp_Popu (rid_Popu_PrfDefaultLang, "Default language menu") {
	"Language",
	{
		Item		{ "None",	'chng',	none,	noKey	},
		Separator	{									}
	}
};


resource 'DLOG' (132, "Preferences") {
	{0, 0, 415, 276}, "Preferences", B_TITLED_WINDOW, NORMAL,
	{
		TabbedBook				{{   0,   0, 415, 276 }, "tabB" 															},
			TabSheet			{ "Colors", "The colors used for syntax coloring and window layout"						},
				ColorControl	{{   0,   0, 145,  16 }, pnm_Col_C_Low,					"Background"						},
				ColorControl	{{ 155,   0, 290,  16 }, pnm_Col_C_Text,				"Normal text"						},

				ColorControl	{{   0,  16, 145,  32 }, pnm_Col_C_Selection,			"Selection"							},
				ColorControl	{{ 155,  16, 290,  32 }, pnm_Col_C_Mark,				"Mark"								},

				ColorControl	{{   0,  32, 145,  48 }, pnm_Col_C_Highlight,			"Highlight"							},

				Line			{{   0,  52, 290,  53 }																		},

				ColorControl	{{   0,  56, 110,  72 }, pnm_Col_C_Keyword1,			"Keywords"							},
				ColorControl	{{ 110,  56, 145,  72 }, pnm_Col_C_Keyword2,			""									},
				ColorControl	{{ 155,  56, 255,  72 }, pnm_Col_C_Comment1,			"Comments"							},
				ColorControl	{{ 255,  56, 290,  72 }, pnm_Col_C_Comment2,			""									},

				ColorControl	{{   0,  72, 110,  88 }, pnm_Col_C_String1,				"Strings"							},
				ColorControl	{{ 110,  72, 145,  88 }, pnm_Col_C_String2,				""									},
				ColorControl	{{ 155,  72, 255,  88 }, pnm_Col_C_Number1,				"Numbers"							},
				ColorControl	{{ 255,  72, 290,  88 }, pnm_Col_C_Number2,				""									},

				ColorControl	{{   0,  88, 110, 104 }, pnm_Col_C_Operator1,			"Operators"							},
				ColorControl	{{ 110,  88, 145, 104 }, pnm_Col_C_Operator2,			""									},
				ColorControl	{{ 155,  88, 255, 104 }, pnm_Col_C_Separator1,			"Separators"						},
				ColorControl	{{ 255,  88, 290, 104 }, pnm_Col_C_Separator2,			""									},

				ColorControl	{{   0, 104, 110, 120 }, pnm_Col_C_Preprocessor1,		"Pre-processor"						},
				ColorControl	{{ 110, 104, 145, 120 }, pnm_Col_C_Preprocessor2,		""									},
				ColorControl	{{ 155, 104, 255, 120 }, pnm_Col_C_Error1,				"Errors"							},
				ColorControl	{{ 255, 104, 290, 120 }, pnm_Col_C_Error2,				""									},

				Line			{{   0, 124, 290, 115 }},

				ColorControl	{{   0, 128, 145, 144 }, pnm_Col_C_IdentifierSystem,	"System identifiers"				},
				ColorControl	{{ 155, 128, 290, 144 }, pnm_Col_C_CharConst,			"Char constants"					},
				ColorControl	{{   0, 144, 145, 160 }, pnm_Col_C_IdentifierUser,		"User identifiers"					},
				ColorControl	{{ 155, 144, 255, 160 }, pnm_Col_C_Tag,					"Tags/Attribut"						},
				ColorControl	{{ 255, 144, 290, 160 }, pnm_Col_C_Attribute,			""									},

				Line			{{   0, 164, 290, 165 }																		},

				ColorControl	{{   0, 168, 145, 184 }, pnm_Col_C_UserSet1,			"User defined 1"					},
				ColorControl	{{ 155, 168, 290, 184 }, pnm_Col_C_UserSet2,			"User defined 2"					},
				ColorControl	{{   0, 184, 145, 200 }, pnm_Col_C_UserSet3,			"User defined 3"					},
				ColorControl	{{ 155, 184, 290, 200 }, pnm_Col_C_UserSet4,			"User defined 4"					},

			TabSheetEnd			{																							},

			TabSheet			{ "Connections", "Options that control the connections to other programs"					},
#ifdef __BEOS__
				CheckBox		{{   0,   0, 200,  16 }, pnm_Con_X_ShowBeIdeMenu,		"Show Metrowerks BeIDE menu"		},
				CheckBox		{{   0,  20, 200,  36 }, pnm_Con_X_LoadBeIdeExt,		"Load Metrowerks BeIDE extensions"	},
				CheckBox		{{   0,  50, 200,  66 }, pnm_Con_X_PassiveFtp,			"Use passive FTP by default"		},
#else
				CheckBox		{{   0,   0, 200,  16 }, pnm_Con_X_ShowBeIdeMenu,		"Show Paladin IDE menu"				},
				CheckBox		{{   0,  30, 200,  46 }, pnm_Con_X_PassiveFtp,			"Use passive FTP by default"		},
#endif
			TabSheetEnd			{																							},

			TabSheet			{ "Differences", "Options that control the 'Find differences' command"						},
				CheckBox		{{   0,   0, 200,  16 }, pnm_Dif_X_DiffCaseInsensitive,	"Ignore case change"				},
				CheckBox		{{   0,  20, 200,  36 }, pnm_Dif_X_IgnoreWhiteSpace,	"Ignore white space"				},
			TabSheetEnd			{																							},

			TabSheet			{ "Editor", "How the editor should behave"													},
				CheckBox		{{   0,   0, 140,  16 }, pnm_Edi_X_ShowTabs,			"Show tabstops"						},
				Edit			{{ 150,   0, 290,  16 }, pnm_Edi_T_SpacesPerTab,		"Spaces per tab:",		"", "0123456789", 3, 90		},

				CheckBox		{{   0,  20, 140,  36 }, pnm_Edi_X_SyntaxColoring,		"Syntax coloring"					},
				CheckBox		{{ 150,  20, 290,  36 }, pnm_Edi_X_AutoIndent,			"Auto-indent"						},

				CheckBox		{{   0,  40, 140,  56 }, pnm_Edi_X_BalanceWhileTyping,	"Balance while typing"				},
				CheckBox		{{ 150,  40, 290,  56 }, pnm_Edi_X_SmartBraces,			"Smart braces"						},

				CheckBox		{{   0,  60, 140,  76 }, pnm_Edi_X_BlockCursor,			"Block cursor"						},
				CheckBox		{{ 150,  60, 290,  76 }, pnm_Edi_X_FlashingCursor,		"Flashing cursor"					},

				CheckBox		{{   0,  80, 140,  96 }, pnm_Edi_X_SmartWorkspaces,		"Smart workspaces"					},
				CheckBox		{{ 150,  80, 290,  96 }, pnm_Edi_X_AlternateHome,		"Alternate home"					},

				Edit			{{   0, 100, 290, 117 }, pnm_Edi_T_ContextLines,
									"Context lines at top/bottom edge:",	"", "0123456789", 2, 240
								},

				Edit			{{   0, 120, 290, 137 }, pnm_Edi_T_ContextChars,
									"Context chars at left/right edge:",	"", "0123456789", 2, 240		},

				Line			{{   0, 143, 290, 144 }},

				PopupMenu		{{   0, 151, 190, 167 }, pnm_Edi_P_Font1Face,			"Font:",				rid_Popu_PrfFont, 50		},
				Edit			{{ 200, 151, 290, 167 }, pnm_Edi_T_Font1Size,			"Size:",				"", "0123456789", 2, 40		},

				PopupMenu		{{   0, 171, 190, 187 }, pnm_Edi_P_Font2Face,			"Alt. font:",			rid_Popu_PrfFont, 50		},
				Edit			{{ 200, 171, 290, 187 }, pnm_Edi_T_Font2Size,			"Size:",				"", "0123456789", 2, 40		},

			TabSheetEnd			{																							},

			TabSheet			{ "File types", "These types show up in the 'Save' dialog"									},
				StringListBox	{{   0,   0, 290, 120 }, pnm_Typ_L_Mimetypes												},
			TabSheetEnd			{																							},

			TabSheet			{ "Files", "Options for files on disk"														},
				Caption			{{   0,   0, 120,  16 }, "sv",							"When saving a file"				},

				CheckBox		{{ 140,   0, 290,  16 }, pnm_Fil_X_MakeBackup,			"Make backup (filename~)"			},
				CheckBox		{{ 140,  20, 290,  36 }, pnm_Fil_X_EndWithNewline,		"End with newline"					},
				CheckBox		{{ 140,  40, 290,  56 }, pnm_Fil_X_DiscardTrailingSpace,"Discard trailing space"			},

				Line			{{   0,  63, 290,  64 }},

				CheckBox		{{   0,  71, 290,  87 }, pnm_Fil_X_VerifyOpenFiles,		"Verify open files for modification"},

				Line			{{   0,  94, 290,  95 }},

				Edit			{{   0, 102, 180, 118 }, pnm_Fil_T_NrOfRecentDocs,		"Nr of recent documents", "", "0123456789", 2, 140	},
				CheckBox		{{ 190, 102, 290, 118 }, pnm_Fil_X_SkipTmpFiles,		"Ignore tmp files"					},

				Line			{{   0, 125, 290, 126 }},

				CheckBox		{{   0, 133, 120, 149 }, pnm_Fil_X_ZoomOpenPanel,		"Zoom 'Open' dialog"					},
				CheckBox		{{ 140, 133, 290, 149 }, pnm_Fil_X_FullPathInTitle,		"Full path in window title"			},

			TabSheetEnd			{																							},

			TabSheet			{ "Glossary", "Glossary options" },
				Button			{{   0,   0, 110,  20 }, "reloadglos",					"Reload glossary",		'RelG'		},
				CheckBox		{{   0,  30, 290,  46 }, pnm_Glo_X_ShowGlossary,		"Show glossary window at startup"	},
				CheckBox		{{   0,  50, 290,  66 }, pnm_Glo_X_SingleClickGlossary,	"Act on single click"				},
			TabSheetEnd			{																							},

			TabSheet			{ "Grep patterns", "Standard grep patterns to use in the 'Find' dialog"						},

				GrepListBox		{{   0,   0, 290, 170 }, "grls"																},

			TabSheetEnd			{																							},

			TabSheet			{ "Groups", "Group file options" },
				CheckBox		{{   0,   0, 280,  16 }, pnm_Grp_X_SortGroup,			"Sort the files inside a group"		},
				CheckBox		{{   0,  20, 280,  36 }, pnm_Grp_X_RelativeGroupPaths,	"Store relative paths in a group file"},
			TabSheetEnd			{																							},

			TabSheet			{ "Include paths", "Paths to use when searching for include files"							},
				CheckBox		{{   0,   0, 280,  16 }, pnm_Inc_X_SearchParent,		"Search parent"						},
				CheckBox		{{   0,  20, 280,  36 }, pnm_Inc_X_BeIncludes,			"Search the paths in $BEINCLUDES"	},
				Caption			{{   0,  40, 200,  56 }, "more",						"And search the following paths:"	},
				PathBox			{{   0,  60, 290, 160 }, "incP" },
			TabSheetEnd			{																							},

			TabSheet			{ "Invisibles", "Options for the way invisible characters are displayed"					},
				Edit			{{   0,   0, 190,  16 }, pnm_Inv_T_ControlChar,		"Show control characters as:",		"¿", "", 1, 170	},

				Line			{{   0,  21, 290,  22 }},

				Caption			{{   0,  27, 290,  43 }, "invccap",		"Options below are effective only when showing invisibles" },

				ColorControl	{{   0,  47, 190,  63 }, pnm_Inv_C_Invisibles,			"Invisibles color:"					},
				Edit			{{   0,  67, 190,  83 }, pnm_Inv_T_TabChar,				"Show tab character as:",		"", "", 1, 170	},
				Edit			{{   0,  87, 190, 103 }, pnm_Inv_T_SpaceChar,			"Show space character as:",		"", "", 1, 170	},
				Edit			{{   0, 107, 190, 123 }, pnm_Inv_T_ReturnChar,			"Show return character as:",	"", "", 1, 170	},
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

			TabSheet			{ "Languages", "Options for syntax coloring and the function popup"							},
				CheckBox		{{   0,   0, 160,  16 }, pnm_Lng_X_ShowIncludes,		"Show includes"						},
				CheckBox		{{   0,  20, 160,  36 }, pnm_Lng_X_ShowPrototypes,		"Show function prototypes"			},
				CheckBox		{{ 170,   0, 290,  16 }, pnm_Lng_X_ShowTypes,			"Show types"						},
				CheckBox		{{ 170,  20, 290,  36 }, pnm_Lng_X_SortPopup,			"Sort alphabetically"				},

				Line			{{   0,  40, 290,  41 }},

				Caption			{{   0,  50, 200,  66 }, "lc",							"Suffix mapping:"					},
				PopupMenu		{{   0,  70, 120,  86 }, pnm_Lng_P_Language,			"Language:",			rid_Popu_PrfLang, 55},
				Edit			{{ 130,  70, 290,  86 }, pnm_Lng_T_Suffixes,			"Suffixes:",			"", "", 0, 45},

				Line			{{   0,  93, 290,  94 }},

				PopupMenu		{{   0, 101, 200, 117 }, pnm_Lng_P_DefaultLanguage,		"Default language:",	rid_Popu_PrfDefaultLang, 100	},
			TabSheetEnd			{																							},

			TabSheet			{ "Projects", "Project window options"														},
				CheckBox		{{   0,   0, 280,  16 }, pnm_Prj_X_SortProjectFiles,	"Sort the files inside a project"	},
				CheckBox		{{   0,  20, 280,  36 }, pnm_Prj_X_AutodetectProjects,	"Autodetect mimetypes of 'Jamfile' & 'Makefile'"},
			TabSheetEnd			{																							},

			TabSheet			{ "Searching", "Paths in the directory popup of the 'Find' Dialog etc."						},
				PathBox			{{   0,   0, 290, 100 }, "srcP"																},

				Line			{{   0, 107, 290, 108 }},

				CheckBox		{{   0, 115, 200, 131 }, pnm_Src_X_InclSearchIgnoreCase,"Incremental search ignores case"	},
				CheckBox		{{   0, 135, 200, 151 }, pnm_Src_X_CenterFoundString,	"Center the string found"			},

			TabSheetEnd			{																							},

			TabSheet			{ "Startup", "What to do when Pe is launched"												},
				RadioButton		{{   0,   0, 145,  16 }, pnm_Sup_R_CreateNewDocument,	"Create new document"				},
				RadioButton		{{   0,  20, 145,  36 }, pnm_Sup_R_ShowOpenDialog,		"Show 'Open' dialog"				},
				RadioButton		{{   0,  40, 290,  56 }, pnm_Sup_R_DoNothing,			"Do nothing"						},
				Caption			{{  40,  56, 290,  72 }, "s_w_c",		" (only effective in combination with a worksheet)"	},

				Line			{{   0,  79, 290,  80 }},

				CheckBox		{{   0,  87, 290, 103 }, pnm_Sup_X_ShowHtmlPalette,		"Show HTML palette"					},
				CheckBox		{{  20, 107, 290, 123 }, pnm_Sup_X_ShowPaletteForHtml,	"Only when editing HTML files"		},
			TabSheetEnd			{																							},

			TabSheet			{ "State", "What to do with state information in saved documents"							},
				Caption			{{   0,   0, 140,  16 }, "state",						"When opening a file restore"		},
				CheckBox		{{   0,  20, 140,  36 }, pnm_Sta_X_RestorePosition,		"Position"							},
				CheckBox		{{   0,  40, 140,  56 }, pnm_Sta_X_RestoreFont,			"Font settings"						},
				CheckBox		{{   0,  60, 140,  76 }, pnm_Sta_X_RestoreSelection,	"Selection"							},
				CheckBox		{{   0,  80, 140,  96 }, pnm_Sta_X_RestoreScrollbar,	"Scrollbar settings"				},
				CheckBox		{{   0, 100, 140, 116 }, pnm_Sta_X_RestoreCwd,			"Working directory"					},
				PopupMenu		{{   0, 120, 140, 140 }, pnm_Sta_P_UseStateFormat,		"Use state format:",	rid_Popu_PrfState, 90	},
			TabSheetEnd			{																							},

			TabSheet			{ "Worksheet", "Options having effect on worksheets"										},
				CheckBox		{{   0,   0, 284,  16 }, pnm_Wor_X_KeepWorksheetOpen,	"Always keep a worksheet open"		},
				CheckBox		{{   0,  20, 284,  36 }, pnm_Wor_X_RedirectStderr,		"Redirect stderr to error Window"	},
				StdErrBox		{{   0,  40, 284, 166 }, "StEb"																},
			TabSheetEnd			{																							},

			TabSheet			{ "Wrapping", "Options for soft-wrapping" },
				CheckBox		{{   0,   0, 100,  16 }, pnm_Wrp_X_SoftWrapFiles,		"Soft-wrap files"					},

				Caption			{{   0,  40, 238,  56 }, "w",			"And when wrapping use these settings:"				},

				RadioButton		{{   0,  60, 100,  76 }, pnm_Wrp_R_WindowWidth,			"Window width"						},
				RadioButton		{{   0,  80, 100,  96 }, pnm_Wrp_R_PaperWidth,			"Paper width"						},
				RadioButton		{{   0, 100,  98, 116 }, pnm_Wrp_R_FixedColumn,			"Fixed column:"						},

				Edit			{{ 100, 100, 140, 116 }, pnm_Wrp_T_FixedColumn,			"",		"", "0123456789", 3, 0		},

			TabSheetEnd			{																							},

			Button				{{ 335, 245, 390, 265 }, "ok  ",		"Apply",			'ok  '							},
			Button				{{ 265, 245, 320, 265 }, "cncl",		"Revert",			'cncl'							},
		TabbedBookEnd			{}
	}
};
