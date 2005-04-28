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
	$"ffffffffffffff000e0effffffffffff "
};

resource 'MICN' (10) {
	$"1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b"
	$"1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b"
	$"1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b"
	$"1b1b1b1800001b1b1b1b1b1b1b1b1b1b"
	$"1b1b1b00141b1b1b1b1b1b1b1b1b1b1b"
	$"1b1b18001b1b1b1b1b1b1b1b1b1b1b1b"
	$"1b000000000d180a1b1b1b1b1b1b0a18"
	$"1b1b18001b1b0a111b001818001b110a"
	$"1b1b110a1b1b00181b180606181b1800"
	$"1b1b0a111b1b001b1b1b0a0a1b1b1b00"
	$"1b1800181b1b001b1b180606181b1b00"
	$"1b1400181b1b00181b001818001b1800"
	$"1b0d001b1b1b0a111b1b1b1b1b1b110a"
	$"0000181b1b1b180a1b1b1b1b1b1b0a18"
	$"1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b"
	$"1b1b1b1b1b1b1b1b1b1b1b1b1b1b1bff"
};

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


resource 'MICN' (13) {
	$"ffffff1814141418ff1000000010ffff"
	$"ffff141467676714000d4646460d00ff"
	$"ff181467676767100d403f3f46520d10"
	$"ff1467676767670052403f3f46526c00"
	$"ff146767676767004c46404046526c00"
	$"ff146767676767006c6c4652526c6c00"
	$"ff181467676767100d6c52526c6c0d10"
	$"ffff141467676718000d6c6c6c0d00ff"
	$"1414141814140000001000000010ffff"
	$"838414ffff0098ab00ffffffffffffff"
	$"848514ff0098abd100ffffffffffffff"
	$"8514ff0098abd100ffffffffffffffff"
	$"14ff0098abd100ffff00ffffff00ffff"
	$"ff0098abd100ffff000000ff000000ff"
	$"ff00abd100ffffffff00ffffff00ffff"
	$"ffff0000ffffffffffffffffffffffff"
};

resource 'MICN' (15) {
	$"ffffffffffffffffffffffff0000ffff"
	$"ffffffffffffffffffffff002c5a00ff"
	$"ffffffffffffffffffff00002c2c5a00"
	$"ffffffffffffffffff00fae500ebeb00"
	$"ffffffffffffffff00fae5e5f90000ff"
	$"ffffffffffffff00fae5e5f9bd00ffff"
	$"ffffffffffff00fae5e5f9bd00ffffff"
	$"ffffffffff00fae5e5f9bd00ffffffff"
	$"ffffffff00fae5e5f9bd00ffffffffff"
	$"ffffff00fae5e5f9bd00ffffffffffff"
	$"ffff00fae5e5f9bd00ffffffffffffff"
	$"ffff00e5e5f9bd00ffffffffffffffff"
	$"ff005a5af9bd00ffffffffffffffffff"
	$"ff005a5a0000ffffffffffffffffffff"
	$"00000000ffffffffffffffffffffffff"
	$"0000ffffffffffffffffffffffffffff "
};

resource 'MICN' (16) {
	$"2d2dffffffffffffffffffff0000ffff"
	$"2d2d2dffffffffffffffff002c5a00ff"
	$"ff2d2d2dffffffffffff00002c2c5a00"
	$"ffff2d2d2dffffffff00fae500ebeb00"
	$"ffffff2d2d2dffff00fae5e5f90000ff"
	$"ffffffff2d2d2d00fae5e5f9bd00ffff"
	$"ffffffffff2d2d2de5e5f9bd00ffffff"
	$"ffffffffff002d2d2df9bd00ffffffff"
	$"ffffffff00fae52d2d2d00ffffffffff"
	$"ffffff00fae5e5f92d2d2dffffffffff"
	$"ffff00fae5e5f9bd002d2d2dffffffff"
	$"ffff00e5e5f9bd00ffff2d2d2dffffff"
	$"ff005a5af9bd00ffffffff2d2d2dffff"
	$"ff005a5a0000ffffffffffff2d2d2dff"
	$"00000000ffffffffffffffffff2d2d2d"
	$"0000ffffffffffffffffffffffff2d2d "
};

resource 'MICN' (17) {
	$"ffffffffffffffffff1000000010ffff"
	$"ffffffffffffffff000d4646460d00ff"
	$"ffffffffffffff100d403f3f46520d10"
	$"ffffffffffffff0052403f3f46526c00"
	$"ffffffffffffff004c46404046526c00"
	$"ffffffffffffff006c6c4652526c6c00"
	$"ffffffffffffff100d6c52526c6c0d10"
	$"ffffffffffffffff000d6c6c6c0d00ff"
	$"ffffffffffff0000001000000010ffff"
	$"ffffffffff0098ab00ffffffffffffff"
	$"ffffffff0098abd100ffffffffffffff"
	$"ffffff0098abd100ffffffffffffffff"
	$"ffff0098abd100ffffffffffffffffff"
	$"ff0098abd100ffffffffffffffffffff"
	$"ff00abd100ffffffffffffffffffffff"
	$"ffff0000ffffffffffffffffffffffff"
};

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
	$"ffffffffffffffffffffffffffffffff "
};

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
