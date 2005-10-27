/*	$Id$
	
	Copyright 2005 Rainer Riedl
	
	Distributed under the MIT License

	Created: 2005-06-04
*/

#ifndef PREFS_H
#define PREFS_H


const char* const prf_X_GrepPatFind				= "regxpatfind";
const char* const prf_X_GrepPatName				= "regxpatname";
const char* const prf_X_GrepPatRepl				= "regxpatrepl";
const char* const prf_X_IncludePath				= "includepath";
const char* const prf_X_Mimetype				= "mimetype";
const char* const prf_X_SearchPath				= "searchpath";
const char* const prf_X_StdErrPattern			= "stderrpattern";

const char* const prf_C_AltComment				= "alt comment color";
const char* const prf_C_AltError				= "alt error color";
const char* const prf_C_AltKeyword				= "alt keyword color";
const char* const prf_C_AltNumber				= "alt number color";
const char* const prf_C_AltOperator				= "alt operator color";
const char* const prf_C_AltSeparator			= "alt separator color";
const char* const prf_C_AltProcessor			= "altprocessor color";
const char* const prf_C_CharConstant			= "char constant color";
const char* const prf_C_Comment					= "comment color";
const char* const prf_C_Error					= "error color";
const char* const prf_C_Invisibles				= "invisibles color";
const char* const prf_C_Keyword					= "keyword color";
const char* const prf_C_Low						= "low color";
const char* const prf_C_Mark					= "mark color";
const char* const prf_C_Number					= "number color";
const char* const prf_C_Operator				= "operator color";
const char* const prf_C_Preprocessor			= "preprocessor color";
const char* const prf_C_Selection				= "selection color";
const char* const prf_C_Separator				= "separator color";
const char* const prf_C_String					= "string color";
const char* const prf_C_SystemIdentifier		= "system identifier color";
const char* const prf_C_Tag						= "tag color";
const char* const prf_C_Tagstring				= "tagstring color";
const char* const prf_C_Text					= "text color";
const char* const prf_C_UserIdentifier			= "user identifier color";
const char* const prf_C_User1					= "user1";
const char* const prf_C_User2					= "user2";
const char* const prf_C_User3					= "user3";
const char* const prf_C_User4					= "user4";

// Todo: There are integer prefs with the same name, error?!
const char* const prf_D_AltFontSize				= "alt font size";
const char* const prf_D_FontSize				= "font size";

const char* const prf_I_SearchBackwards			= "Search Backwards";
const char* const prf_I_SearchBatch				= "Search Batch";
const char* const prf_I_SearchEntireWord		= "Search Entire Word";
const char* const prf_I_SearchIgnoreCase		= "Search Ignore Case";
const char* const prf_I_SearchMultikind			= "Search Multikind";
const char* const prf_I_SearchRecursive			= "Search Recursive";
const char* const prf_I_SearchTextFilesOnly		= "Search Text Files Only";
const char* const prf_I_SearchWhichDir			= "Search Whichdir";
const char* const prf_I_SearchWrap				= "Search Wrap";
const char* const prf_I_SearchWithGrep			= "Search with Grep";
const char* const prf_I_AltFontSize				= "alt font size";
const char* const prf_I_AltHome					= "althome";
const char* const prf_I_AutoIndent				= "auto indent";
const char* const prf_I_AutodetectProjects		= "autodetect projects";
const char* const prf_I_Backup					= "backup";
const char* const prf_I_Balance					= "balance";
const char* const prf_I_BeIncludes				= "beincludes";
const char* const prf_I_BlockCursor				= "block cursor";
const char* const prf_I_CenterFound				= "centerfound";
const char* const prf_I_ContextLines			= "contextlines";
const char* const prf_I_ContextChars			= "contextchars";
const char* const prf_I_DiffCase				= "diffcase";
const char* const prf_I_DiffWhite				= "diffwhite";
const char* const prf_I_FlashCursor				= "flash cursor";
const char* const prf_I_FontSize				= "font size";
const char* const prf_I_FullPath				= "fullpath";
const char* const prf_I_IdeMenu					= "ide menu";
const char* const prf_I_Includes				= "includes";
const char* const prf_I_IsearchIgncase			= "isearch_igncase";
const char* const prf_I_MwPlugins				= "mw plugins";
const char* const prf_I_NlAtEof					= "nl at eof";
const char* const prf_I_Parent					= "parent";
const char* const prf_I_PassiveFtp				= "passive ftp";
const char* const prf_I_Protos					= "protos";
const char* const prf_I_RecentSize				= "recent size";
const char* const prf_I_RedirectStdErr			= "redirect stderr";
const char* const prf_I_RelativeGroupPaths		= "relative group paths";
const char* const prf_I_RestoreCwd				= "restore cwd";
const char* const prf_I_RestoreFont				= "restore font";
const char* const prf_I_RestorePosition			= "restore position";
const char* const prf_I_RestoreScrollbar		= "restore scrollbar";
const char* const prf_I_RestoreSelection		= "restore selection";
const char* const prf_I_SavedState				= "saved state";
const char* const prf_I_ScrollwheelLines		= "scrollwheel lines";	
						// Currently not in GUI
const char* const prf_I_ShowHtmlpaletteForHtml	= "show htmlpalette for html";
const char* const prf_I_ShowHtmlPalette			= "show htmlpalette";
const char* const prf_I_ShowInvisibles			= "show invisibles";
const char* const prf_I_ShowTabs				= "show tabs";
const char* const prf_I_ShowGlossary			= "showglossary";
const char* const prf_I_SingleClickGlossary		= "singleclickglossary";
const char* const prf_I_SkipHtmlExt				= "skiphtmlext";
const char* const prf_I_SkipTmp					= "skiptmp";
const char* const prf_I_SmartBraces				= "smart braces";
const char* const prf_I_SoftWrap				= "softwrap";
const char* const prf_I_SortGroup				= "sortgroup";
const char* const prf_I_SortPopup				= "sortpopup";
const char* const prf_I_SortProject				= "sortproject";
const char* const prf_I_SpacesPerTab			= "spaces per tab";
const char* const prf_I_Startup					= "startup";
const char* const prf_I_StdErrInitEd			= "stderrinited";
const char* const prf_I_SyntaxColoring			= "syntax coloring";
const char* const prf_I_Types					= "types";
const char* const prf_I_Verify					= "verify";
const char* const prf_I_WindowToWorkspace		= "window to workspace";
const char* const prf_I_Worksheet				= "worksheet";
const char* const prf_I_WrapCol					= "wrapcol";
const char* const prf_I_WrapType				= "wraptype";
const char* const prf_I_ZoomOpen				= "zoomopen";

const char* const prf_R_GlossaryPosition		= "GlossaryPosition";
const char* const prf_R_HtmlPalettePos			= "HTMLPalettePos";
const char* const prf_R_DefaultDocumentRect		= "default document rect";

const char* const prf_S_AltFontFamily			= "alt font family";
const char* const prf_S_AltFontStyle			= "alt font style";
const char* const prf_S_ControlChar				= "control char";
const char* const prf_S_DefLang					= "def lang";
const char* const prf_S_FontFamily				= "font family";
const char* const prf_S_FontStyle				= "font style";
const char* const prf_S_LastFindAndOpen			= "last find&open";
const char* const prf_S_LastFtpServer			= "last ftp server";
const char* const prf_S_LastFtpUser				= "last ftp user";
const char* const prf_S_ReturnChar				= "return char";
const char* const prf_S_SpaceChar				= "space char";
const char* const prf_S_TabChar					= "tab char";


#endif // PREFS_H
