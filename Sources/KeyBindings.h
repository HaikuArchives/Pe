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

	Created: 02-06-02
*/

#ifndef KEYBINDINGS_H
#define KEYBINDINGS_H

//#define kmsg_OpenHeaderSource			'OpnH'
//#define kmsg_Abort_Shell_Command		'amsg'
#define kmsg_Move_Character_Left								'Mchl'
#define kmsg_Move_Character_Right							'Mchr'
#define kmsg_Move_Word_Left									'Mwdl'
#define kmsg_Move_Word_Right								'Mwdr'
#define kmsg_Move_SubWord_Left								'Mswl'
#define kmsg_Move_SubWord_Right								'Mswr'
#define kmsg_Move_to_Beginning_of_Line						'Mbol'
#define kmsg_Move_to_End_of_Line							'Meol'
#define kmsg_Move_to_Previous_Line							'Mprl'
#define kmsg_Move_to_Next_Line								'Mnxl'
#define kmsg_Move_to_Top_of_Page							'Mbop'
#define kmsg_Move_to_Bottom_of_Page						'Meop'
#define kmsg_Move_to_Beginning_of_File						'Mbof'
#define kmsg_Move_to_End_of_File								'Meof'
#define kmsg_Delete_Character_Left							'Dchl'
#define kmsg_Delete_Character_Right							'Dchr'
#define kmsg_Delete_to_Beginning_of_Line					'Dbol'
#define kmsg_Delete_to_End_of_Line							'Deol'
#define kmsg_Delete_to_End_of_File							'Deof'
#define kmsg_Extend_Selection_with_Character_Left		'Schl'
#define kmsg_Extend_Selection_with_Character_Right		'Schr'
#define kmsg_Extend_Selection_with_Previous_Word			'Swdl'
#define kmsg_Extend_Selection_with_Next_Word				'Swdr'
#define kmsg_Extend_Selection_with_Previous_SubWord			'Sswl'
#define kmsg_Extend_Selection_with_Next_SubWord				'Sswr'
#define kmsg_Extend_Selection_to_Current_Line				'Slin'
#define kmsg_Extend_Selection_to_Previous_Line			'Sprl'
#define kmsg_Extend_Selection_to_Next_Line					'Snxl'
#define kmsg_Extend_Selection_to_Beginning_of_Line		'Sbol'
#define kmsg_Extend_Selection_to_End_of_Line				'Seol'
#define kmsg_Extend_Selection_to_Beginning_of_Page		'Sbop'
#define kmsg_Extend_Selection_to_End_of_Page				'Seop'
#define kmsg_Extend_Selection_to_Beginning_of_File		'Sbof'
#define kmsg_Extend_Selection_to_End_of_File				'Seof'
#define kmsg_Scroll_One_Line_Up								'Rlnu'
#define kmsg_Scroll_One_Line_Down								'Rlnd'
#define kmsg_Scroll_Page_Up										'Rpgu'
#define kmsg_Scroll_Page_Down									'Rpgd'
#define kmsg_Scroll_to_Start_of_File							'Rbof'
#define kmsg_Scroll_to_End_of_File								'Reof'

// for emacs users:
#define kmsg_OpenLine												'OpnL'

#define kmsg_Mark													'Mark'
#define kmsg_MarkAll												'MAll'
#define kmsg_MarkWord												'MWrd'
#define kmsg_ExchangeMarkAndPoint							'ExMP'

#define kmsg_CutRegion											'CutR'
#define kmsg_CopyRegion											'CopR'
#define kmsg_ClearRegion											'ClrR'

#define kmsg_Cut_to_End_of_Line								'CutL'
#define kmsg_Cut_Word											'CutW'
#define kmsg_Cut_Word_Backward								'CtWB'
#define kmsg_Cut_Sentence										'CutS'

#define kmsg_AppendNextCut										'ApCt'

#define kmsg_Recenter												'Rcnt'

#define kmsg_NrArgument											'Arg '

#define kmsg_SwitchActivePart									'SwAP'
#define kmsg_SplitWindow											'Splt'
#define kmsg_UnsplitWindow										'DelP'


#endif // KEYBINDINGS_H
