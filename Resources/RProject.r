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
	
	Created: 11/21/98 20:33:44
*/

#include "RButtonBar.r"
#include "RTypes.r"
#include "PMessages.h"

Resource 'MENU' (200)
{
	"File",
	{
		Item		{ "Close", msg_Close, none, 'W' },
		Separator	{ },
		Item		{ "Save", msg_Save, none, 'S' },
		Item		{ "Save As…", msg_SaveAs, none, noKey },
		Item		{ "Revert…", msg_Revert, none, noKey },
		Separator	{ },
		Item		{ "Quit", msg_Quit, none, 'Q' }
	}
};

Resource 'MENU' (201)
{
	"Edit",
	{
		Item		{ "New Group", msg_NewGroup, none, noKey },
		Item		{ "Edit as Text", msg_EditAsText, none, noKey }
	}
};

Resource 'MBAR' (200)
{	// group file
	{ 200, 201 }
};

Resource 'MBAR' (201)
{	// project file
	{ 200 }
};

Resource 'BtnB' (1) {	// group file
	acceptFirstClick,
	{
		20, 'Add ', 0, "Add File",
		21, 'Rmve', 0, "Remove File",
		0, 0, space, "",
		14, msg_Save, 0, "Save Document"
	}
};

Resource 'BtnB' (1) {	// Project file
	0,
	{
		20, 'Add ', 0, "Add File",
		21, 'Rmve', 0, "Remove File",
		0, 0, space, "",
		14, msg_Save, 0, "Save Document",
		0, 0, space, "",
		22, msg_EditAsText, 0, "Edit Project as Text"
	}
};

Resource 'MICN' (20) {
	$"ffffffffffffffff0000000000ffffff"
	$"ffffffffffffffff00faf95d00ffffff"
	$"ffffffffffffffff00faf95d00ffffff"
	$"ffffffffff00000000faf95d00000000"
	$"ffffffffff00fafafafaf9f9fafafa00"
	$"ffffffff0000f9f9f9f9f9f9f9f9f900"
	$"ffffff003f005d5d5df9f95d5d5d5d00"
	$"ffff00414100000000faf95d00000000"
	$"ff00414141b7b79000faf95d00ffffff"
	$"004141424141b7b700faf95d00ffffff"
	$"b7000042424242420000000000ffffff"
	$"ffb7000000434343434343009000b7b7"
	$"ffffb700900000454545000000b7ffff"
	$"ffffffb7006890004500b7b7ffffffff"
	$"ffffffffb700000000b7b7ffffffffff"
	$"ffffffffffb700b7ffb7ffffffffffff"
};

Resource 'MICN' (21) {
	$"ffffffffffffffffffffffffffffffff"
	$"ffffffffffffffffffffffffffffffff"
	$"ffffffffffffffffffffffffffffffff"
	$"ffffffffff0000000000000000000000"
	$"ffffffffff00fafafafafafafafafa00"
	$"ffffffff0000f9f9f9f9f9f9f9f9f900"
	$"ffffff003f005d5d5d5d5d5d5d5d5d00"
	$"ffff0041410000000000000000000000"
	$"ff00414141b7b790b768689041410000"
	$"004141424141b7b7b790b741410000b7"
	$"b700004242424242b7b7424229900000"
	$"ffb7000000434343434343009000b7b7"
	$"ffffb700900000454545000000b7ffff"
	$"ffffffb7006890004500b7b7ffffffff"
	$"ffffffffb700000000b7b7ffffffffff"
	$"ffffffffffb700b7ffb7ffffffffffff"
};

Resource 'MICN' (22) {
	$"ffffffffffffffffffffffff0a0b0aff"
	$"ffffffffffffffffffffff0a24858500"
	$"ffffffffffffffffffff0a2486858501"
	$"ffffffffffff00ffff0a24863f200000"
	$"ffffffffff003f000a24863f202000ff"
	$"ffffffff003f400a24863f202000ffff"
	$"ffffff003f40400a863f202000ffffff"
	$"ffff004141410a640b202000400000ff"
	$"ff00414141410a646400004141410000"
	$"00414142410a290000919191910100b7"
	$"b7000042420000b7b7b7b7b700910000"
	$"ffb7000000434343434343009000b7b7"
	$"ffffb700900000454545000000b7ffff"
	$"ffffffb7006890004500b7b7ffffffff"
	$"ffffffffb700000000b7b7ffffffffff"
	$"ffffffffffb700b7ffb7ffffffffffff"
};
