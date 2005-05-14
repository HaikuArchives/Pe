/*	$Id$
	
	Copyright 2005 Rainer Riedl
	
	Distributed under the MIT License

	Created: 2005-05-14
*/

#ifndef RESOURCESMENUS_H
#define RESOURCESMENUS_H


/*** RESOURCE-TYPES ******************/
#define rt_MBR					'MBAR'
#define rt_MEN					'MENU'
#define rt_POP					'MENU'


/*** RESOURCE-IDs ********************/

// Menubars
#define ri_MBR_DOCUMENT_WIN			0
#define ri_MBR_GROUP_WIN			1
#define ri_MBR_PROJECT_WIN			2

// Menus: Document Window
#define ri_MEN_DW_FILE				0
#define ri_MEN_DW_EDIT				1
#define ri_MEN_DW_TEXT				2
#define ri_MEN_DW_SEARCH			3
#define ri_MEN_DW_WINDOW			4
#define ri_MEN_DW_HELP				5
#define ri_MEN_DW_EXTENSIONS		6
#define ri_MEN_DW_BEIDE				7
#define ri_MEN_DW_RECENT			10
#define ri_MEN_DW_CHANGECASE		11
// Menus: File Panel (Document Window)
#define ri_MEN_FP_MIMETYPES			20
// Menus: Project/Group Window
#define ri_MEN_PW_FILE				200
#define ri_MEN_PW_EDIT				201

// Popupmenus: Document Window
#define ri_POP_CTX_TEXT				30
// Popupmenus: Find-Dialog
#define ri_POP_FND_METHOD			81
#define ri_POP_FND_DIRNAME			82
#define ri_POP_FND_FILENAME			83
#define ri_POP_FND_GREPPATTERN		84
// Popupmenus: Prefs
#define ri_POP_PRF_FONT				90
#define ri_POP_PRF_LANG				91
#define ri_POP_PRF_STATE			92
#define ri_POP_PRF_DEFLANG			93
// Popupmenus: Change Encoding
#define ri_POP_ENC_CONV				111
// Popupmenus: FTP-Open
#define ri_POP_FTP_PATH				128
// Popupmenus: Info
#define ri_POP_INF_MIMETYPES		101
#define ri_POP_INF_FONT				400
#define ri_POP_INF_ENCODING			401
#define ri_POP_INF_LINEBREAKS		402
#define ri_POP_INF_LANGUAGE			403


#endif // RESOURCESMENUS_H
