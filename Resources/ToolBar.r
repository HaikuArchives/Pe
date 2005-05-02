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
	
	Created: 02/20/98 19:12:29
*/

#include "RButtonBar.r"
#include "../Sources/PMessages.h"

resource 'BtnB' (0) {
	0,
	{
		new, msg_New, 0, "New Document",
		open, msg_BtnOpen, menu, "Open Document",
		14, msg_Save, 0, "Save Document",
		0, 0, space, "",
		11, msg_Execute, toggle, "Execute Command",
		10, msg_FuncPopup, menu, "Function Popupmenu",
		32, msg_HeaderPopup, menu, "Header Popupmenu",
		0, 0, space, "",
		17, msg_FindCmd, 0, "Find",
		13, msg_IncSearchBtn, toggle, "Incremental Search",
		0, 0, space, "",
		15, msg_ReadOnly, dual, "Read Only",
		12, msg_Info, 0, "File Options…",
		18, msg_SoftWrap, dual, "Softwrap",
		0, 0, space, "",
		31, msg_EditAsPrj, 0, "Edit As Project"
	}
};

type 'MICN' {
	hexstring;
};

/* Save Document */
resource 'MICN' (14) {
	$"ffffffffffffffffffffffffffffffff"
	$"ffffffffffffffffffffffffffffffff"
	$"ffffffffffffffffffffffffffffffff"
	$"ffffffffffffffff0effffffffffffff"
	$"ffffffffff0000000e0e0effffffffff"
	$"ffffffff000b002d00000e0e0effffff"
	$"ffffff000b003f2deb2d00000e0e0eff"
	$"ffff000b003f3f3f3fd2d2d200000e0e"
	$"ff000b0b0b00003f3f3f3f000b000e0e"
	$"000b0b00000b0b00003f000b00000e0e"
	$"000b00171700000b0b000b00000e0eff"
	$"0015003f3f1717000b0b00000e0effff"
	$"ff000015153f3f000b00000e0effffff"
	$"ffffff000015000b00000e0effffffff"
	$"ffffffffff000000000e0effffffffff"
	$"ffffffffffffff000e0effffffffffff"
};

/* Function Popupmenu */
resource 'MICN' (10) {
	$"ffffffffffffffffffffffffffffffff"
	$"ffffffffffffffffffffffffffffffff"
	$"ffffffffffffffffffffffffffffffff"
	$"ffffff180000ffffffffffffffffffff"
	$"ffffff0014ffffffffffffffffffffff"
	$"ffff1800ffffffffffffffffffffffff"
	$"ff000000000d180affffffffffff0a18"
	$"ffff1800ffff0a11ff00181800ff110a"
	$"ffff110affff0018ff18060618ff1800"
	$"ffff0a11ffff00ffffff0a0affffff00"
	$"ff180018ffff00ffff18060618ffff00"
	$"ff140018ffff0018ff00181800ff1800"
	$"ff0d00ffffff0a11ffffffffffff110a"
	$"000018ffffff180affffffffffff0a18"
	$"ffffffffffffffffffffffffffffffff"
	$"ffffffffffffffffffffffffffffffff"
};

/* Header Popupmenu */
resource 'MICN' (32) {
	$"ffffffffffffffffffffffffffffffff"
	$"ffffffffffffffffffffffffffffffff"
	$"ffffffffffffffffffffffffffffffff"
	$"ffffffffff1400ffffffffffffffffff"
	$"ffffffffff0000ffffffffffffffffff"
	$"ffffffffffff00ffffffffffffffffff"
	$"ffffffffffff00ff000014ffffffffff"
	$"ffffffffffff000aff1400ffffffffff"
	$"ffffffffffff00ffffff00ffffffffff"
	$"ffffffffffff00ffffff01ffffffffff"
	$"ffffffffffff00ffffff00ffffffffff"
	$"ffffffffff180017ff180017ffffffff"
	$"ffffff00ff000000ff000000ffffffff"
	$"ffffffffffffffffffffffffffffffff"
	$"ffffffffffffffffffffffffffffffff"
	$"ffffffffffffffffffffffffffffffff"
};

/* Execute Command */
resource 'MICN' (11) {
	$"ffffffffffffffffffffff0000ffffff"
	$"ffffffffffffffffffff00d8d800ffff"
	$"ffffffffffffffffffff00fad8d800ff"
	$"ffffffffffffffffff00f9f9fa7d00ff"
	$"ffffffffffffffffff00bdbd7d00ffff"
	$"ffffffffffffffff00f9bdbd7d00ffff"
	$"ffffff0000ffffff00bdf97d00ffffff"
	$"ffff00d8d800ff00f9f97d7d00ffffff"
	$"ff00d8d8fa00ff00bdf97d00ffffffff"
	$"ff00f8bdfad800f9f9bd7d00ffffffff"
	$"ffff00f8bdfaf9bdf97d00ffffffffff"
	$"ffff00f8bdfaf9f87d7d00ffffffffff"
	$"ffffff00f8bdf9bd7d00ffffffffffff"
	$"ffffff00f8bdf97d7d00ffffffffffff"
	$"ffffffff00f8f97d00ffffffffffffff"
	$"ffffffff00000000ffffffffffffffff"
};

/* Find */
resource 'MICN' (17) {
	$"ffffffffffffff0000000000ffffffff"
	$"ffffffffff00003f3f3f3f3f0000ffff"
	$"ffffffff003f3f00000000003f3f00ff"
	$"ffffff003f0e008787878787010f3f00"
	$"ffffff003f00aeaeaeaeaeaeae003f00"
	$"ffffff003f0eae8686868686ae101f00"
	$"ffffff00003f0e00000000000f3f0000"
	$"ffffff008700003f3f3f3f3f00008700"
	$"ffffff00870017000000000087878700"
	$"ffffffff00173f00603f8787870000ff"
	$"ffffff00043f00000000000000ffffff"
	$"ffff00000e00000fffffffffffffffff"
	$"ff00000e00000fffffffffffffffffff"
	$"00000e00000fffffffffffffffffffff"
	$"000000000fffffffffffffffffffffff"
	$"ff00000fffffffffffffffffffffffff"
};

/* Incremental Search */
resource 'MICN' (13) {
	$"ffffffffffffff0000000000ffffffff"
	$"ffffffffff00003f3f3f3f3f0000ffff"
	$"ffffffff003f3f00000000003f3f00ff"
	$"ffffff003f0e008787878787010f3f00"
	$"ffffff003f00aeaeaeaeaeaeae003f00"
	$"ffffff003f0eae8686868686ae101f00"
	$"ffffff00003f0e00000000000f3f0000"
	$"ffffff008700003f3f3f3f3f00008700"
	$"ffffff00870017000000000087878700"
	$"ffffffff00173f00603f8787870000ff"
	$"ffffff00043f00000000000000ffffff"
	$"ffff00000e00000fffffffffffffffff"
	$"ff00000e00000fffffffffffffffffff"
	$"00000e00000fffffff00ffffff00ffff"
	$"000000000fffffff000000ff000000ff"
	$"ff00000fffffffffff00ffffff00ffff"
};

/* Read Only Off */
resource 'MICN' (15) {
	$"ffffffffffffffffffff0b0000ffffff"
	$"ffffffffffffffffff000b3f5a00ffff"
	$"ffffffffffffffff005c0b3f5a5a00ff"
	$"ffffffffffffff005cfd0b3f5a5a00ff"
	$"ffffffffffff005cfd113f085a5a00ff"
	$"ffffffffff005cfd113ffd08080900ff"
	$"ffffffff0a5cfd113ffd085c5c00ffff"
	$"ffffff0a5cfd113ffa085c5c00ffffff"
	$"ffff110afd113ffa085c5c00ffffffff"
	$"ffff0a0b0a3ffa085c5c00ffffffffff"
	$"ff170a3f0afa115c5c00ffffffffffff"
	$"ff110b3f110a001100ffffffffffffff"
	$"170a3f5a5b5b5b00ffffffffffffffff"
	$"110a005b5b0000ffffffffffffffffff"
	$"0a00000000ffffffffffffffffffffff"
	$"0a000affffffffffffffffffffffffff"
};

/* Read Only On */
resource 'MICN' (16) {
	$"ff2dffffffffffffffff0b0000ffffff"
	$"2d2d2dffffffffffff000b3f5a00ffff"
	$"ff2d2d2dffffffff005c0b3f5a5a00ff"
	$"ffff2d2d2dffff005cfd0b3f5a5a00ff"
	$"ffffff2d2d2d005cfd113f085a5a00ff"
	$"ffffffff2d2d2dfd113ffd08080900ff"
	$"ffffffff0a2d2d2d3ffd085c5c00ffff"
	$"ffffff0a5cfd2d2d2d085c5c00ffffff"
	$"ffff110afd113f2d2d2d5c00ffffffff"
	$"ffff0a0b0a3ffa082d2d2dffffffffff"
	$"ff170a3f0afa115c5c2d2d2dffffffff"
	$"ff110b3f110a001100ff2d2d2dffffff"
	$"170a3f5a5b5b5b00ffffff2d2d2dffff"
	$"110a005b5b0000ffffffffff2d2d2dff"
	$"0a00000000ffffffffffffffff2d2d2d"
	$"0a000affffffffffffffffffffff2dff"
};

/* File Options */
resource 'MICN' (12) {
	$"ffffffffff000000ffffffffffffffff"
	$"ffffffff00f2232400ffffffffffffff"
	$"ffffff00f23ff2232400ffffffffffff"
	$"ffffff00f2f2f2232400ffffffffffff"
	$"ffffff00232323242400ffffffffffff"
	$"ffffffff0024242400ffffffffffffff"
	$"ffffff00920000005e00ffffffffffff"
	$"ffffff0c2192d25e2500ffffffffffff"
	$"ffffffff002121252500ffffffffffff"
	$"ffffff0000212125250013ffffffffff"
	$"ffff005e002121252500001313ffffff"
	$"ffff002121212125f2f22500131313ff"
	$"ffff130021212121f22525001313ffff"
	$"ffffffff000021212125001313ffffff"
	$"ffffffffffff000021001313ffffffff"
	$"ffffffffffffffff001313ffffffffff"
};

/* Softwrap On */
Resource 'MICN' (19) {
	$"ffffffffffffffffffffffffffffffff"
	$"ff00000000000000ff9a9affffffffff"
	$"ffffffffffffffffff9a9affffffffff"
	$"ff0000ff00000000009a9affffffffff"
	$"ffffffffffffffffff9a9affffffffff"
	$"ff00000000000000ff9a9affffffffff"
	$"ffffffffffffffffff9a9affffffffff"
	$"ff00000000000000009a9affffffffff"
	$"ffffffffffffffffff9a9affffffffff"
	$"ff00000000ff0000009a9affffffffff"
	$"ffffffffffffffffff9a9affffff9aff"
	$"ff0000ff00000000ff9a9affff9a9aff"
	$"ffffffffffffffffff9a9aff9a9a9aff"
	$"ff0000000000ff00009a9affff9a9aff"
	$"ffffffffffffffffff9a9affffff9aff"
	$"ffffffffffffffffffffffffffffffff"
};

/* Softwrap Off */
Resource 'MICN' (18) {
	$"ffffffffffffffffffffffffffffffff"
	$"ff00000000000000ff9a9a000000ffff"
	$"ffffffffffffffffff9a9affffffffff"
	$"ff0000ff00000000009a9a0000ffffff"
	$"ffffffffffffffffff9a9affffffffff"
	$"ff00000000000000ff9a9a000000ffff"
	$"ffffffffffffffffff9a9affffffffff"
	$"ff00000000000000009a9a000000ffff"
	$"ffffffffffffffffff9a9affffffffff"
	$"ff00000000ff0000009a9aff000000ff"
	$"ffffffffffffffffff9a9affffffffff"
	$"ff0000ff00000000ff9a9a000000ffff"
	$"ffffffffffffffffff9a9affffffffff"
	$"ff0000000000ff00009a9a0000ffffff"
	$"ffffffffffffffffff9a9affffffffff"
	$"ffffffffffffffffffffffffffffffff"  
};

/* Edit As Project */
Resource 'MICN' (31) {
	$"fffffafafafafafafafaffffffffffff"
	$"fffffafafafafafafafaffffffffffff" 
	$"ffff090909090909090909090909ffff" 
	$"ffff091919191919191919191909ffff" 
	$"ffff09193f3f3f3f3f3f3f3f1909ffff" 
	$"ffff09193f0000000000003f1909ffff" 
	$"ffff09193f3f3f3f3f3f3f3f1909ffff" 
	$"ffff09193f3f3f3f3f3f3f3f1909ffff" 
	$"ffff09193f0000000000003f1909ffff" 
	$"ffff09193f3f3f3f3f3f3f3f1909ffff" 
	$"ffff09193f3f3f3f3f3f3f3f1909ffff" 
	$"ffff09193f0000000000003f1909ffff" 
	$"ffff09193f3f3f3f3f3f3f3f1909ffff" 
	$"ffff09193f3f3f3f3f3f3f3f1909ffff" 
	$"ffff091919191919191919191909ffff" 
	$"ffff090909090909090909090909ffff"
};
