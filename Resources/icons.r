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

read 'MICN' (401, "error.micn")
	"error.icon.raw";

//read 'MICN' (404, "idea.micn")
//	"ide a.icon.raw";

read 'MICN' (403, "info.micn")
	"info.icon.raw";

read 'MICN' (402, "warning.micn")
	"warning.icon.raw";

type 'MICN' {
	hexstring;
};

Resource 'MICN' (500, "warning 2")
{
	$"ffffffffffffffffffffffff"
	$"fffffffff9f9f9ffffffffff"
	$"fffff9f9f9f9f9f9f9ffffff"
	$"fffff9f9f9f9f9f9f9ffffff"
	$"fff9f9f9f9f9f9f9f9f9ffff"
	$"fff9f9f9f9f9f9f9f9f9ffff"
	$"fff9f9f9f9f9f9f9f9f9ffff"
	$"fffff9f9f9f9f9f9f9ffffff"
	$"fffff9f9f9f9f9f9f9ffffff"
	$"fffffffff9f9f9ffffffffff"
	$"ffffffffffffffffffffffff"
	$"ffffffffffffffffffffffff"
};

Resource 'MICN' (501, "error 2")
{
	$"ffffffffffffffffffffffff"
	$"ffffffff2d2d2dffffffffff"
	$"ffff2d2d2d2d2d2d2dffffff"
	$"ffff2d2d2d2d2d2d2dffffff"
	$"ff2d2d2d2d2d2d2d2d2dffff"
	$"ff2d2d2d2d2d2d2d2d2dffff"
	$"ff2d2d2d2d2d2d2d2d2dffff"
	$"ffff2d2d2d2d2d2d2dffffff"
	$"ffff2d2d2d2d2d2d2dffffff"
	$"ffffffff2d2d2dffffffffff"
	$"ffffffffffffffffffffffff"
	$"ffffffffffffffffffffffff"
};

Resource 'MICN' (502, "info 2")
{
	$"ffffffffffffffffffffffff"
	$"ffffffff222222ffffffffff"
	$"ffff22222222222222ffffff"
	$"ffff22222222222222ffffff"
	$"ff222222222222222222ffff"
	$"ff222222222222222222ffff"
	$"ff222222222222222222ffff"
	$"ffff22222222222222ffffff"
	$"ffff22222222222222ffffff"
	$"ffffffff222222ffffffffff"
	$"ffffffffffffffffffffffff"
	$"ffffffffffffffffffffffff"
};

//Resource 'MICN' (500, "warning 2")
//{
//	$"ffffffff0000ffffffffffff"
//	$"ffffff00fafa0000ffffffff"
//	$"ffff00fafafafafa00ffffff"
//	$"ffff00f9f9fafa5d00ffffff"
//	$"ffff00f9f9f95d5d00ffffff"
//	$"ffff00f9f9f95d5d00ffffff"
//	$"ffff00f9f9f95d5d00ffffff"
//	$"ffff000000f95d0000ffffff"
//	$"ffff00f9f900005d00ffffff"
//	$"ffff00f9f9f95d5d00ffffff"
//	$"ffffff0000f95d00ffffffff"
//	$"ffffffffff0000ffffffffff"
//};
//
//Resource 'MICN' (501, "error 2")
//{
//	$"ffffffff0000ffffffffffff"
//	$"ffffff002a2a0000ffffffff"
//	$"ffff002a2a2a2a2a00ffffff"
//	$"ffff00ebeb2a2a3000ffffff"
//	$"ffffff06ebeb3000ffffffff"
//	$"ffffff0bebeb300bffffffff"
//	$"ffffffff003000ffffffffff"
//	$"ffffffff000000ffffffffff"
//	$"ffffff002a2d2d00ffffffff"
//	$"ffffff002d2d2f00ffffffff"
//	$"ffffff002d2f2f00ffffffff"
//	$"ffffffff000000ffffffffff"
//};
//
//Resource 'MICN' (502, "info 2")
//{
//	$"ffffffffff000000ffffffff"
//	$"ffffffff0020242600ffffff"
//	$"ffffffff0024242600ffffff"
//	$"ffffffff0026262600ffffff"
//	$"ffffffffff000000ffffffff"
//	$"ffffffff0022222700ffffff"
//	$"ffffffff0022222700ffffff"
//	$"ffffff240022222700ffffff"
//	$"ffffff00222222272700ffff"
//	$"ffffff00002222222700ffff"
//	$"ffffffffff00002200ffffff"
//	$"ffffffffffffff00ffffffff"
//};
