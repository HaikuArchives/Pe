
/*  A Bison parser, made from Sources/rez_parser.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	ALIGN	258
#define	ARRAY	259
#define	AS	260
#define	BINARY	261
#define	BITSTRING	262
#define	BOOLEAN	263
#define	BYTE	264
#define	CASE	265
#define	CHANGE	266
#define	CHAR	267
#define	CSTRING	268
#define	DECIMAL	269
#define	DELETE	270
#define	DEREZ	271
#define	FILL	272
#define	HEX	273
#define	INTEGER	274
#define	LITERAL	275
#define	LONGINT	276
#define	OCTAL	277
#define	INCLUDE	278
#define	POINT	279
#define	PSTRING	280
#define	READ	281
#define	RECT	282
#define	RES	283
#define	REZ	284
#define	STRING	285
#define	SWITCH	286
#define	rTYPE	287
#define	WSTRING	288
#define	LABEL	289
#define	IDENT	290
#define	REZVAR	291
#define	NUMBER	292
#define	AND	293
#define	OR	294
#define	EQ	295
#define	NE	296
#define	LT	297
#define	LE	298
#define	GT	299
#define	GE	300
#define	SHR	301
#define	SHL	302
#define	STR_CONST	303
#define	WIDE	304
#define	BIT	305
#define	NIBBLE	306
#define	WORD	307
#define	LONG	308
#define	KEY	309
#define	HEXSTRING	310
#define	UNSIGNED	311
#define	rEOF	312
#define	COUNTOF	313
#define	ARRAYINDEX	314
#define	LBITFIELD	315
#define	LBYTE	316
#define	FORMAT	317
#define	RESSIZE	318
#define	RESOURCE	319
#define	LLONG	320
#define	SHELL	321
#define	LWORD	322
#define	HEX_CONST	323
#define	NEGATE	324
#define	FLIP	325
#define	NOT	326

#line 1 "Sources/rez_parser.y"


/*	$Id$
	
	Copyright 1996, 1997, 1998
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

	Created: 12/02/98 15:37:42
*/

#include "rez.h"
#include "RTypes.h"
#include "RState.h"
#include "REval.h"

#include <cstdio>
#include <List.h>
#include <ByteOrder.h>

//#define alloca malloc
#define YYDEBUG 1

#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL

extern FILE *yyin;

extern int yylex();

#define yyerror(s)	error(s)

static RState *sState;
static RElem *head;

#define RS(s)	((RState *)(s))
#define RSV(s)	((RSValue *)(s))
#define LST(s)	((BList *)(s))
#define RE(s)	((REval *)(s))

#define CHECKSTATE	if (!sState) error("Internal error (sState == NULL on line %d)", __LINE__)
#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		328
#define	YYFLAG		-32768
#define	YYNTBASE	92

#define YYTRANSLATE(x) ((unsigned)(x) <= 326 ? yytranslate[x] : 131)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    91,     2,     2,     2,    76,    71,     2,    88,
    89,    74,    72,    87,    73,     2,    75,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    86,    82,     2,
    83,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    84,     2,    85,    70,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    80,    69,    81,    90,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
    56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
    66,    67,    68,    77,    78,    79
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     8,    10,    12,    14,    16,    20,    27,    33,
    35,    37,    41,    43,    45,    47,    49,    51,    53,    55,
    57,    59,    61,    62,    64,    68,    71,    73,    76,    79,
    83,    87,    92,    97,   103,   106,   110,   114,   119,   121,
   123,   125,   127,   129,   131,   136,   138,   140,   142,   144,
   148,   151,   153,   158,   162,   169,   172,   178,   180,   182,
   184,   186,   188,   190,   196,   199,   201,   207,   210,   215,
   221,   227,   234,   242,   251,   256,   259,   261,   267,   271,
   273,   275,   277,   282,   284,   286,   288,   290,   292,   294,
   297,   303,   305,   307,   309,   311,   313,   316,   318,   320,
   322,   324,   326,   330,   332,   336,   342,   346,   350,   354,
   358,   362,   366,   370,   374,   378,   382,   386,   389,   392,
   395,   399,   403,   407,   411,   415,   419,   423,   427,   429,
   431,   433,   438,   447,   452,   457,   462,   470,   476,   486,
   494,   498,   502,   504,   506,   508,   510,   514,   518,   519,
   521,   524,   526,   529,   531,   534,   539,   544,   548,   554,
   562,   572,   584,   598,   614,   632,   652,   674,   676
};

static const short yyrhs[] = {    94,
     0,   121,    80,   123,    81,    82,     0,   122,     0,    93,
     0,     1,     0,    57,     0,    23,   128,    82,     0,    32,
    37,    80,    95,    81,    82,     0,    32,    37,     5,    37,
    82,     0,    96,     0,    34,     0,    95,    82,    96,     0,
    97,     0,   101,     0,    98,     0,   102,     0,   104,     0,
   105,     0,   106,     0,   107,     0,   113,     0,   115,     0,
     0,     8,     0,     8,    83,   119,     0,     8,   117,     0,
   100,     0,    56,   100,     0,    99,   100,     0,    56,    99,
   100,     0,   100,    83,   119,     0,    56,   100,    83,   119,
     0,    99,   100,    83,   119,     0,    56,    99,   100,    83,
   119,     0,   100,   117,     0,    56,   100,   117,     0,    99,
   100,   117,     0,    56,    99,   100,   117,     0,    18,     0,
    14,     0,    22,     0,     6,     0,    20,     0,     7,     0,
     7,    84,   119,    85,     0,     9,     0,    19,     0,    21,
     0,    12,     0,    12,    83,   119,     0,    12,   117,     0,
   103,     0,   103,    84,   119,    85,     0,   103,    83,   128,
     0,   103,    84,   119,    85,    83,   128,     0,   103,   117,
     0,   103,    84,   119,    85,   117,     0,    30,     0,    55,
     0,    25,     0,    33,     0,    13,     0,    24,     0,    24,
    83,   125,   123,    81,     0,    24,   117,     0,    27,     0,
    27,    83,   125,   123,    81,     0,    27,   117,     0,     4,
    80,    95,    81,     0,    49,     4,    80,    95,    81,     0,
     4,    35,    80,    95,    81,     0,    49,     4,    35,    80,
    95,    81,     0,     4,    84,   119,    85,    80,    95,    81,
     0,    49,     4,    84,   119,    85,    80,    95,    81,     0,
    31,    80,   108,    81,     0,   108,   109,     0,   109,     0,
    10,    35,    86,   110,    82,     0,   110,    82,   111,     0,
   111,     0,    96,     0,    34,     0,    54,   112,    83,   119,
     0,     8,     0,    12,     0,    24,     0,    27,     0,   100,
     0,   103,     0,    17,   114,     0,    17,   114,    84,   119,
    85,     0,    50,     0,    51,     0,     9,     0,    52,     0,
    53,     0,     3,   116,     0,    51,     0,     9,     0,    52,
     0,    53,     0,   118,     0,   117,    87,   118,     0,    35,
     0,    35,    83,   119,     0,    35,    83,   125,   123,    81,
     0,    88,   119,    89,     0,   119,    72,   119,     0,   119,
    73,   119,     0,   119,    74,   119,     0,   119,    75,   119,
     0,   119,    76,   119,     0,   119,    47,   119,     0,   119,
    46,   119,     0,   119,    71,   119,     0,   119,    69,   119,
     0,   119,    70,   119,     0,    90,   119,     0,    91,   119,
     0,    73,   119,     0,   119,    38,   119,     0,   119,    39,
   119,     0,   119,    42,   119,     0,   119,    44,   119,     0,
   119,    43,   119,     0,   119,    45,   119,     0,   119,    40,
   119,     0,   119,    41,   119,     0,    37,     0,    35,     0,
   120,     0,    58,    88,    35,    89,     0,    60,    88,    35,
    87,    37,    87,    37,    89,     0,    61,    88,    35,    89,
     0,    67,    88,    35,    89,     0,    65,    88,    35,    89,
     0,    28,    37,    88,    37,    87,   128,    89,     0,    28,
    37,    88,    37,    89,     0,    26,    37,    88,    37,    87,
   128,    89,   128,    82,     0,    26,    37,    88,    37,    89,
   128,    82,     0,   123,    87,   124,     0,   123,    82,   124,
     0,   124,     0,   119,     0,   128,     0,   127,     0,   126,
   123,    81,     0,   125,   123,    81,     0,     0,    80,     0,
    35,    80,     0,    68,     0,   127,    68,     0,    48,     0,
   128,    48,     0,    66,    88,    48,    89,     0,    62,    88,
   129,    89,     0,    48,    87,   130,     0,    48,    87,   130,
    87,   130,     0,    48,    87,   130,    87,   130,    87,   130,
     0,    48,    87,   130,    87,   130,    87,   130,    87,   130,
     0,    48,    87,   130,    87,   130,    87,   130,    87,   130,
    87,   130,     0,    48,    87,   130,    87,   130,    87,   130,
    87,   130,    87,   130,    87,   130,     0,    48,    87,   130,
    87,   130,    87,   130,    87,   130,    87,   130,    87,   130,
    87,   130,     0,    48,    87,   130,    87,   130,    87,   130,
    87,   130,    87,   130,    87,   130,    87,   130,    87,   130,
     0,    48,    87,   130,    87,   130,    87,   130,    87,   130,
    87,   130,    87,   130,    87,   130,    87,   130,    87,   130,
     0,    48,    87,   130,    87,   130,    87,   130,    87,   130,
    87,   130,    87,   130,    87,   130,    87,   130,    87,   130,
    87,   130,     0,    48,     0,    37,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    92,    93,    94,    95,    96,    97,   100,   103,   104,   107,
   108,   109,   112,   113,   114,   115,   116,   117,   118,   119,
   120,   121,   122,   125,   126,   127,   130,   131,   132,   133,
   134,   135,   136,   137,   138,   139,   140,   141,   144,   145,
   146,   147,   148,   151,   152,   153,   154,   155,   158,   159,
   160,   163,   164,   165,   166,   167,   168,   171,   172,   173,
   174,   175,   178,   179,   180,   183,   184,   185,   188,   189,
   190,   191,   192,   193,   196,   199,   200,   203,   206,   207,
   210,   211,   212,   215,   216,   217,   218,   219,   220,   223,
   224,   239,   240,   241,   242,   243,   246,   249,   250,   251,
   252,   255,   256,   259,   260,   261,   264,   265,   266,   267,
   268,   269,   270,   271,   272,   273,   274,   275,   276,   277,
   278,   279,   280,   281,   282,   283,   284,   285,   286,   287,
   288,   291,   292,   294,   295,   296,   299,   310,   323,   325,
   329,   330,   331,   334,   336,   337,   338,   339,   340,   343,
   346,   349,   350,   367,   368,   377,   378,   381,   382,   383,
   384,   385,   386,   387,   388,   389,   390,   393,   394
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","ALIGN",
"ARRAY","AS","BINARY","BITSTRING","BOOLEAN","BYTE","CASE","CHANGE","CHAR","CSTRING",
"DECIMAL","DELETE","DEREZ","FILL","HEX","INTEGER","LITERAL","LONGINT","OCTAL",
"INCLUDE","POINT","PSTRING","READ","RECT","RES","REZ","STRING","SWITCH","rTYPE",
"WSTRING","LABEL","IDENT","REZVAR","NUMBER","AND","OR","EQ","NE","LT","LE","GT",
"GE","SHR","SHL","STR_CONST","WIDE","BIT","NIBBLE","WORD","LONG","KEY","HEXSTRING",
"UNSIGNED","rEOF","COUNTOF","ARRAYINDEX","LBITFIELD","LBYTE","FORMAT","RESSIZE",
"RESOURCE","LLONG","SHELL","LWORD","HEX_CONST","'|'","'^'","'&'","'+'","'-'",
"'*'","'/'","'%'","NEGATE","FLIP","NOT","'{'","'}'","';'","'='","'['","']'",
"':'","','","'('","')'","'~'","'!'","s","incl","type","datadecl","datatype",
"booleantype","numerictype","radix","numericsize","chartype","stringtype","stringspecifier",
"pointtype","recttype","arraytype","switchtype","casestmts","casestmt","casebody",
"caseline","keytype","filltype","fillsize","aligntype","alignsize","symboliclist",
"symbolicvalue","e","f","resheader","readheader","datalist","data","dataarray",
"switchdata","hexconst","strconst","fmt","farg", NULL
};
#endif

static const short yyr1[] = {     0,
    92,    92,    92,    92,    92,    92,    93,    94,    94,    95,
    95,    95,    96,    96,    96,    96,    96,    96,    96,    96,
    96,    96,    96,    97,    97,    97,    98,    98,    98,    98,
    98,    98,    98,    98,    98,    98,    98,    98,    99,    99,
    99,    99,    99,   100,   100,   100,   100,   100,   101,   101,
   101,   102,   102,   102,   102,   102,   102,   103,   103,   103,
   103,   103,   104,   104,   104,   105,   105,   105,   106,   106,
   106,   106,   106,   106,   107,   108,   108,   109,   110,   110,
   111,   111,   111,   112,   112,   112,   112,   112,   112,   113,
   113,   114,   114,   114,   114,   114,   115,   116,   116,   116,
   116,   117,   117,   118,   118,   118,   119,   119,   119,   119,
   119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
   119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
   119,   120,   120,   120,   120,   120,   121,   121,   122,   122,
   123,   123,   123,   124,   124,   124,   124,   124,   124,   125,
   126,   127,   127,   128,   128,   128,   128,   129,   129,   129,
   129,   129,   129,   129,   129,   129,   129,   130,   130
};

static const short yyr2[] = {     0,
     1,     5,     1,     1,     1,     1,     3,     6,     5,     1,
     1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     0,     1,     3,     2,     1,     2,     2,     3,
     3,     4,     4,     5,     2,     3,     3,     4,     1,     1,
     1,     1,     1,     1,     4,     1,     1,     1,     1,     3,
     2,     1,     4,     3,     6,     2,     5,     1,     1,     1,
     1,     1,     1,     5,     2,     1,     5,     2,     4,     5,
     5,     6,     7,     8,     4,     2,     1,     5,     3,     1,
     1,     1,     4,     1,     1,     1,     1,     1,     1,     2,
     5,     1,     1,     1,     1,     1,     2,     1,     1,     1,
     1,     1,     3,     1,     3,     5,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     2,     2,     2,
     3,     3,     3,     3,     3,     3,     3,     3,     1,     1,
     1,     4,     8,     4,     4,     4,     7,     5,     9,     7,
     3,     3,     1,     1,     1,     1,     3,     3,     0,     1,
     2,     1,     2,     1,     2,     4,     4,     3,     5,     7,
     9,    11,    13,    15,    17,    19,    21,     1,     1
};

static const short yydefact[] = {     0,
     5,     0,     0,     0,     0,     6,     4,     1,     0,     3,
   154,     0,     0,     0,     0,     0,     0,   149,     0,     0,
   155,     7,     0,     0,     0,    23,   130,   129,     0,     0,
     0,     0,     0,   152,     0,   150,     0,     0,     0,   144,
   131,     0,   143,   149,   149,   146,   145,     0,     0,     0,
     0,     0,     0,     0,     0,    42,    44,    24,    46,    49,
    62,    40,     0,    39,    47,    43,    48,    41,    63,    60,
    66,    58,     0,    61,    11,     0,    59,     0,     0,    10,
    13,    15,     0,    27,    14,    16,    52,    17,    18,    19,
    20,    21,    22,   151,     0,     0,     0,     0,     0,   130,
   120,     0,   118,   119,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   149,   149,     0,     0,   153,     0,   157,
   156,     0,     0,     0,   138,     9,    99,    98,   100,   101,
    97,     0,    23,     0,     0,   104,     0,    26,   102,     0,
    51,    94,    92,    93,    95,    96,    90,     0,    65,     0,
    68,     0,     0,     0,    28,     0,    23,    29,     0,    35,
     0,     0,    56,     0,     0,     0,     0,     0,   107,   121,
   122,   127,   128,   123,   125,   124,   126,   114,   113,   116,
   117,   115,   108,   109,   110,   111,   112,     2,   142,   141,
   148,   147,   169,   168,   158,     0,     0,     0,    23,     0,
     0,     0,     0,    25,     0,    50,     0,   149,   149,     0,
     0,    77,     0,    23,     0,    30,     0,    36,     8,    12,
     0,    37,    31,    54,     0,   132,     0,   134,   136,   135,
     0,     0,   140,   137,     0,    69,     0,    45,   105,   149,
   103,     0,     0,     0,     0,    75,    76,    23,     0,     0,
     0,    38,    32,    33,    53,     0,   159,     0,    71,    23,
     0,    91,    64,    67,    23,     0,    70,     0,    34,     0,
    57,     0,     0,   139,     0,   106,    82,     0,    81,     0,
    80,    72,    23,    55,     0,   160,    73,    84,    85,    86,
    87,    88,    89,     0,    78,     0,   133,     0,     0,    79,
    74,   161,    83,     0,   162,     0,   163,     0,   164,     0,
   165,     0,   166,     0,   167,     0,     0,     0
};

static const short yydefgoto[] = {   326,
     7,     8,    79,    80,    81,    82,    83,    84,    85,    86,
    87,    88,    89,    90,    91,   221,   222,   290,   291,   304,
    92,   157,    93,   141,   148,   149,    40,    41,     9,    10,
    42,    43,    44,    45,    46,    47,    49,   205
};

static const short yypact[] = {    11,
-32768,   -21,    -6,     1,    22,-32768,-32768,-32768,   -45,-32768,
-32768,   -42,    24,   -25,    27,    40,     9,   221,    85,    92,
-32768,-32768,   100,   112,   121,   398,    87,-32768,    77,   107,
   111,   113,   120,-32768,    99,-32768,    99,    99,    99,   693,
-32768,    63,-32768,   221,   221,   105,   132,   101,   133,   139,
   -61,   -29,   161,    90,   -33,-32768,   165,   -19,-32768,   -16,
-32768,-32768,    56,-32768,-32768,-32768,-32768,-32768,   -14,-32768,
    13,-32768,   175,-32768,-32768,   248,-32768,   312,    65,-32768,
-32768,-32768,    23,    17,-32768,-32768,   -22,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   230,   235,   239,   245,   256,-32768,
-32768,   448,-32768,-32768,    99,    99,    99,    99,    99,    99,
    99,    99,    99,    99,    99,    99,    99,    99,    99,    99,
    99,    99,   211,   221,   221,    81,    89,-32768,    -8,-32768,
-32768,   -21,   -21,   -21,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   177,   398,    99,    99,   212,    99,   213,-32768,    99,
   213,-32768,-32768,-32768,-32768,-32768,   218,   223,   213,   223,
   213,   294,    14,    23,    18,   224,   452,    19,    99,   213,
   -21,    99,   213,   216,   220,   219,   225,   228,-32768,   523,
   703,   569,   569,   131,   131,   131,   131,   188,   188,   682,
   740,    80,   222,   222,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   236,   -23,    15,     2,   398,   103,
   515,   563,   255,   693,   275,   693,    99,   221,   221,   289,
    12,-32768,   247,   398,    99,    20,    99,   213,-32768,-32768,
    99,   213,   693,   132,   580,-32768,   288,-32768,-32768,-32768,
    -8,   -21,-32768,-32768,   137,-32768,   249,-32768,   693,   221,
-32768,   628,   128,   130,   250,-32768,-32768,   398,   151,   645,
    99,   213,   693,   693,    21,   251,   252,    47,-32768,   398,
   172,-32768,-32768,-32768,   344,   164,-32768,   257,   693,   -21,
   213,   303,    -8,-32768,   186,-32768,-32768,   519,-32768,   259,
-32768,-32768,   398,   132,   253,   262,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   261,   217,   195,-32768,    -8,    99,-32768,
-32768,   267,   693,    -8,   268,    -8,   272,    -8,   273,    -8,
   280,    -8,   283,    -8,-32768,   372,   373,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,  -119,  -157,-32768,-32768,   298,   -72,-32768,-32768,
    91,-32768,-32768,-32768,-32768,-32768,   159,-32768,    76,-32768,
-32768,-32768,-32768,-32768,   -51,   167,   -34,-32768,-32768,-32768,
   -37,   160,  -143,-32768,-32768,    -2,-32768,  -122
};


#define	YYLAST		816


static const short yytable[] = {    14,
   101,   142,   102,   103,   104,   165,   126,   127,   151,   230,
   168,     1,   146,    25,   218,   146,   219,   159,   146,   161,
   146,   220,    21,   210,    21,   132,    11,   133,   203,    57,
    15,    59,   170,     2,    18,   173,     3,    16,     4,   204,
    12,    65,     5,    67,    13,    19,   143,   146,   223,    21,
   144,   146,   146,   146,   146,   146,    22,   134,    17,   135,
   171,   172,    21,   147,   152,   242,   150,     6,   158,   250,
   180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
   190,   191,   192,   193,   194,   195,   196,   197,    26,   245,
   244,   226,   256,   224,    21,   160,   243,   225,   137,   169,
   227,   231,   261,   280,   259,   153,   154,   155,   156,   211,
   212,    20,   214,   228,    23,   216,   232,   289,   267,   107,
   108,   109,   110,   111,   112,   113,   114,    24,   284,   206,
   207,   208,    48,   100,   233,    28,    51,   235,   276,    50,
   138,   139,   140,   123,   124,   166,   167,   289,    52,   125,
   285,   118,   119,   120,   121,   122,    29,    53,    30,    31,
   296,   201,   124,    32,    95,    33,    94,   125,   234,   202,
   124,    35,   128,   306,   262,   125,   113,   114,   249,    21,
   253,   254,   252,   246,   167,   312,    37,   129,    38,    39,
   260,   315,   263,   317,    96,   319,   264,   321,    97,   323,
    98,   325,   118,   119,   120,   121,   122,    99,   273,   124,
   274,   124,   271,   281,   125,   302,   125,   269,   167,    54,
    55,   130,    56,    57,    58,    59,   279,   131,    60,    61,
    62,   277,   167,    63,    64,    65,    66,    67,    68,   268,
    69,    70,   136,    71,   292,   167,    72,    73,   145,    74,
   287,   163,   286,   124,   162,    27,   209,    28,   125,   118,
   119,   120,   121,   122,   174,    76,   297,   167,    11,   175,
   288,    77,    78,   176,   313,   311,   167,   294,    29,   177,
    30,    31,    12,   199,   200,    32,    13,    33,    34,   100,
   178,    28,   198,    35,   213,   120,   121,   122,   -23,   215,
    36,   217,    36,   220,   236,   229,   237,   238,    37,   146,
    38,    39,    29,   239,    30,    31,   240,    56,    57,    32,
    59,    33,   241,   255,   266,    62,   258,    35,   270,    64,
    65,    66,    67,    68,    36,   275,   293,   282,   283,   295,
   305,   307,    37,   309,    38,    39,    54,    55,   308,    56,
    57,    58,    59,   314,   316,    60,    61,    62,   318,   320,
    63,    64,    65,    66,    67,    68,   322,    69,    70,   324,
    71,   327,   328,    72,    73,   164,    74,   287,   303,   257,
   310,   251,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    76,     0,     0,     0,     0,   288,    77,    78,
    54,    55,     0,    56,    57,    58,    59,     0,     0,    60,
    61,    62,     0,     0,    63,    64,    65,    66,    67,    68,
     0,    69,    70,     0,    71,     0,     0,    72,    73,     0,
    74,    75,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    76,     0,     0,     0,
     0,     0,    77,    78,    54,    55,     0,    56,    57,    58,
    59,     0,     0,    60,    61,    62,     0,     0,    63,    64,
    65,    66,    67,    68,     0,    69,    70,     0,    71,     0,
     0,    72,    73,     0,    74,   105,   106,   107,   108,   109,
   110,   111,   112,   113,   114,     0,     0,     0,     0,     0,
    76,     0,     0,     0,     0,     0,    77,    78,     0,     0,
     0,     0,     0,     0,     0,     0,   115,   116,   117,   118,
   119,   120,   121,   122,     0,    57,   298,    59,     0,     0,
   299,    61,     0,     0,     0,     0,   179,    65,     0,    67,
     0,     0,   300,    70,     0,   301,     0,     0,    72,     0,
     0,    74,   105,   106,   107,   108,   109,   110,   111,   112,
   113,   114,   107,   108,   109,   110,   111,   112,   113,   114,
     0,     0,     0,    77,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   115,   116,   117,   118,   119,   120,   121,
   122,   115,   116,   117,   118,   119,   120,   121,   122,   247,
   105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
   109,   110,   111,   112,   113,   114,     0,   105,   106,   107,
   108,   109,   110,   111,   112,   113,   114,     0,     0,     0,
     0,   115,   116,   117,   118,   119,   120,   121,   122,     0,
   118,   119,   120,   121,   122,     0,     0,   248,   115,   116,
   117,   118,   119,   120,   121,   122,     0,     0,     0,     0,
     0,     0,     0,     0,   265,   105,   106,   107,   108,   109,
   110,   111,   112,   113,   114,     0,     0,     0,     0,     0,
     0,     0,   105,   106,   107,   108,   109,   110,   111,   112,
   113,   114,     0,     0,     0,     0,   115,   116,   117,   118,
   119,   120,   121,   122,     0,     0,     0,     0,     0,     0,
     0,     0,   272,   115,   116,   117,   118,   119,   120,   121,
   122,   107,   108,   109,   110,   111,   112,   113,   114,   278,
   105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
   105,     0,   107,   108,   109,   110,   111,   112,   113,   114,
     0,   116,   117,   118,   119,   120,   121,   122,     0,     0,
     0,   115,   116,   117,   118,   119,   120,   121,   122,     0,
     0,   115,   116,   117,   118,   119,   120,   121,   122,   107,
   108,   109,   110,   111,   112,   113,   114,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   117,   118,   119,   120,   121,   122
};

static const short yycheck[] = {     2,
    35,    35,    37,    38,    39,    78,    44,    45,    60,   167,
    83,     1,    35,     5,   158,    35,   160,    69,    35,    71,
    35,    10,    48,   143,    48,    87,    48,    89,    37,     7,
    37,     9,    84,    23,    80,    87,    26,    37,    28,    48,
    62,    19,    32,    21,    66,    88,    80,    35,    35,    48,
    84,    35,    35,    35,    35,    35,    82,    87,    37,    89,
    83,    84,    48,    83,     9,    89,    83,    57,    83,   213,
   105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
   115,   116,   117,   118,   119,   120,   121,   122,    80,   209,
    89,   164,    81,    80,    48,    83,    82,    84,     9,    83,
    83,    83,    83,    83,   224,    50,    51,    52,    53,   144,
   145,    88,   147,   165,    88,   150,   168,   275,   241,    40,
    41,    42,    43,    44,    45,    46,    47,    88,    82,   132,
   133,   134,    48,    35,   169,    37,    37,   172,   258,    48,
    51,    52,    53,    81,    82,    81,    82,   305,    37,    87,
   270,    72,    73,    74,    75,    76,    58,    37,    60,    61,
   283,    81,    82,    65,    88,    67,    80,    87,   171,    81,
    82,    73,    68,   293,   226,    87,    46,    47,   213,    48,
   218,   219,   217,    81,    82,   308,    88,    87,    90,    91,
   225,   314,   227,   316,    88,   318,   231,   320,    88,   322,
    88,   324,    72,    73,    74,    75,    76,    88,    81,    82,
    81,    82,   250,   265,    87,   288,    87,    81,    82,     3,
     4,    89,     6,     7,     8,     9,   261,    89,    12,    13,
    14,    81,    82,    17,    18,    19,    20,    21,    22,   242,
    24,    25,    82,    27,    81,    82,    30,    31,    84,    33,
    34,     4,    81,    82,    80,    35,    80,    37,    87,    72,
    73,    74,    75,    76,    35,    49,    81,    82,    48,    35,
    54,    55,    56,    35,   309,    81,    82,   280,    58,    35,
    60,    61,    62,   124,   125,    65,    66,    67,    68,    35,
    35,    37,    82,    73,    83,    74,    75,    76,    82,    87,
    80,    84,    80,    10,    89,    82,    87,    89,    88,    35,
    90,    91,    58,    89,    60,    61,    89,     6,     7,    65,
     9,    67,    87,    35,    37,    14,    80,    73,    80,    18,
    19,    20,    21,    22,    80,    86,    80,    87,    87,    37,
    82,    89,    88,    83,    90,    91,     3,     4,    87,     6,
     7,     8,     9,    87,    87,    12,    13,    14,    87,    87,
    17,    18,    19,    20,    21,    22,    87,    24,    25,    87,
    27,     0,     0,    30,    31,    78,    33,    34,   288,   221,
   305,   215,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    49,    -1,    -1,    -1,    -1,    54,    55,    56,
     3,     4,    -1,     6,     7,     8,     9,    -1,    -1,    12,
    13,    14,    -1,    -1,    17,    18,    19,    20,    21,    22,
    -1,    24,    25,    -1,    27,    -1,    -1,    30,    31,    -1,
    33,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    49,    -1,    -1,    -1,
    -1,    -1,    55,    56,     3,     4,    -1,     6,     7,     8,
     9,    -1,    -1,    12,    13,    14,    -1,    -1,    17,    18,
    19,    20,    21,    22,    -1,    24,    25,    -1,    27,    -1,
    -1,    30,    31,    -1,    33,    38,    39,    40,    41,    42,
    43,    44,    45,    46,    47,    -1,    -1,    -1,    -1,    -1,
    49,    -1,    -1,    -1,    -1,    -1,    55,    56,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    69,    70,    71,    72,
    73,    74,    75,    76,    -1,     7,     8,     9,    -1,    -1,
    12,    13,    -1,    -1,    -1,    -1,    89,    19,    -1,    21,
    -1,    -1,    24,    25,    -1,    27,    -1,    -1,    30,    -1,
    -1,    33,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    40,    41,    42,    43,    44,    45,    46,    47,
    -1,    -1,    -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    69,    70,    71,    72,    73,    74,    75,
    76,    69,    70,    71,    72,    73,    74,    75,    76,    85,
    38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
    42,    43,    44,    45,    46,    47,    -1,    38,    39,    40,
    41,    42,    43,    44,    45,    46,    47,    -1,    -1,    -1,
    -1,    69,    70,    71,    72,    73,    74,    75,    76,    -1,
    72,    73,    74,    75,    76,    -1,    -1,    85,    69,    70,
    71,    72,    73,    74,    75,    76,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    85,    38,    39,    40,    41,    42,
    43,    44,    45,    46,    47,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    -1,    -1,    -1,    -1,    69,    70,    71,    72,
    73,    74,    75,    76,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    85,    69,    70,    71,    72,    73,    74,    75,
    76,    40,    41,    42,    43,    44,    45,    46,    47,    85,
    38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
    38,    -1,    40,    41,    42,    43,    44,    45,    46,    47,
    -1,    70,    71,    72,    73,    74,    75,    76,    -1,    -1,
    -1,    69,    70,    71,    72,    73,    74,    75,    76,    -1,
    -1,    69,    70,    71,    72,    73,    74,    75,    76,    40,
    41,    42,    43,    44,    45,    46,    47,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    71,    72,    73,    74,    75,    76
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/etc/bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#if defined (__GNUC__) && !defined (C_ALLOCA)
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (__BEOS__) && defined (__MWERKS__) && !defined (C_ALLOCA)
#include <alloca.h>
#else /* Not Metroworks on BeOS or else using C_ALLOCA */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not Metroworks on BeOS */
#endif /* not sparc.  */
#endif /* not GNU C and not C_ALLOCA.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 196 "/etc/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 92 "Sources/rez_parser.y"
{ YYACCEPT; ;
    break;}
case 2:
#line 93 "Sources/rez_parser.y"
{ if (head) head->Write(); WriteResource(yyvsp[-4]); YYACCEPT; ;
    break;}
case 3:
#line 94 "Sources/rez_parser.y"
{ YYACCEPT; ;
    break;}
case 4:
#line 95 "Sources/rez_parser.y"
{ YYACCEPT; ;
    break;}
case 5:
#line 96 "Sources/rez_parser.y"
{ return 1; ;
    break;}
case 6:
#line 97 "Sources/rez_parser.y"
{ return 1; ;
    break;}
case 7:
#line 100 "Sources/rez_parser.y"
{ Include((char *)yyvsp[-1]); ;
    break;}
case 8:
#line 103 "Sources/rez_parser.y"
{ RState::FinishType(yyvsp[-4], RS(yyvsp[-2])); ;
    break;}
case 9:
#line 104 "Sources/rez_parser.y"
{ RState::CopyType(yyvsp[-3], yyvsp[-1]); ;
    break;}
case 12:
#line 109 "Sources/rez_parser.y"
{ yyval = yyvsp[-2]; if (yyvsp[0]) RS(yyvsp[-2])->SetNext(RS(yyvsp[0])); ;
    break;}
case 23:
#line 122 "Sources/rez_parser.y"
{ yyval = 0; ;
    break;}
case 24:
#line 125 "Sources/rez_parser.y"
{ yyval = (int)new RSNrValue(8); ;
    break;}
case 25:
#line 126 "Sources/rez_parser.y"
{ yyval = (int)new RSNrValue(8); RSV(yyval)->SetDefaultValue(yyvsp[0]); ;
    break;}
case 26:
#line 127 "Sources/rez_parser.y"
{ yyval = (int)new RSNrValue(8); RSV(yyval)->AddIdentifiers(LST(yyvsp[0])); ;
    break;}
case 28:
#line 131 "Sources/rez_parser.y"
{ yyval = yyvsp[0]; ;
    break;}
case 29:
#line 132 "Sources/rez_parser.y"
{ yyval = yyvsp[0]; ;
    break;}
case 30:
#line 133 "Sources/rez_parser.y"
{ yyval = yyvsp[0]; ;
    break;}
case 31:
#line 134 "Sources/rez_parser.y"
{ yyval = yyvsp[-2]; RSV(yyval)->SetDefaultValue(yyvsp[0]); ;
    break;}
case 32:
#line 135 "Sources/rez_parser.y"
{ yyval = yyvsp[-2]; RSV(yyval)->SetDefaultValue(yyvsp[0]); ;
    break;}
case 33:
#line 136 "Sources/rez_parser.y"
{ yyval = yyvsp[-2]; RSV(yyval)->SetDefaultValue(yyvsp[0]); ;
    break;}
case 34:
#line 137 "Sources/rez_parser.y"
{ yyval = yyvsp[-2]; RSV(yyval)->SetDefaultValue(yyvsp[0]); ;
    break;}
case 35:
#line 138 "Sources/rez_parser.y"
{ yyval = yyvsp[-1]; RSV(yyval)->AddIdentifiers(LST(yyvsp[0])); ;
    break;}
case 36:
#line 139 "Sources/rez_parser.y"
{ yyval = yyvsp[-1]; RSV(yyval)->AddIdentifiers(LST(yyvsp[0])); ;
    break;}
case 37:
#line 140 "Sources/rez_parser.y"
{ yyval = yyvsp[-1]; RSV(yyval)->AddIdentifiers(LST(yyvsp[0])); ;
    break;}
case 38:
#line 141 "Sources/rez_parser.y"
{ yyval = yyvsp[-1]; RSV(yyval)->AddIdentifiers(LST(yyvsp[0])); ;
    break;}
case 44:
#line 151 "Sources/rez_parser.y"
{ yyval = (int)new RSNrValue(32); ;
    break;}
case 45:
#line 152 "Sources/rez_parser.y"
{ yyval = (int)new RSNrValue(REvaluate(RE(yyvsp[-1]), head)); ;
    break;}
case 46:
#line 153 "Sources/rez_parser.y"
{ yyval = (int)new RSNrValue(8); ;
    break;}
case 47:
#line 154 "Sources/rez_parser.y"
{ yyval = (int)new RSNrValue(16); ;
    break;}
case 48:
#line 155 "Sources/rez_parser.y"
{ yyval = (int)new RSNrValue(32); ;
    break;}
case 49:
#line 158 "Sources/rez_parser.y"
{ yyval = (int)new RSNrValue(8); ;
    break;}
case 50:
#line 159 "Sources/rez_parser.y"
{ yyval = (int)new RSNrValue(8); RSV(yyval)->SetDefaultValue(yyvsp[0]); ;
    break;}
case 51:
#line 160 "Sources/rez_parser.y"
{ yyval = (int)new RSNrValue(8); RSV(yyval)->AddIdentifiers(LST(yyvsp[0])); ;
    break;}
case 52:
#line 163 "Sources/rez_parser.y"
{ yyval = (int)new RSStringValue(yyvsp[0], 0); ;
    break;}
case 53:
#line 164 "Sources/rez_parser.y"
{ yyval = (int)new RSStringValue(yyvsp[-3], REvaluate(RE(yyvsp[-1]), head)); ;
    break;}
case 54:
#line 165 "Sources/rez_parser.y"
{ yyval = (int)new RSStringValue(yyvsp[-2], 0); RSV(yyval)->SetDefaultValue(yyvsp[0]); ;
    break;}
case 55:
#line 166 "Sources/rez_parser.y"
{ yyval = (int)new RSStringValue(yyvsp[-5], REvaluate(RE(yyvsp[-3]), head)); RSV(yyval)->SetDefaultValue(yyvsp[0]); ;
    break;}
case 56:
#line 167 "Sources/rez_parser.y"
{ yyval = (int)new RSStringValue(yyvsp[-1], 0); RSV(yyval)->AddIdentifiers(LST(yyvsp[0])); ;
    break;}
case 57:
#line 168 "Sources/rez_parser.y"
{ yyval = (int)new RSStringValue(yyvsp[-4], REvaluate(RE(yyvsp[-2]), head)); RSV(yyval)->AddIdentifiers(LST(yyvsp[0])); ;
    break;}
case 58:
#line 171 "Sources/rez_parser.y"
{ yyval = RSStringValue::skStr; ;
    break;}
case 59:
#line 172 "Sources/rez_parser.y"
{ yyval = RSStringValue::skHex; ;
    break;}
case 60:
#line 173 "Sources/rez_parser.y"
{ yyval = RSStringValue::skPStr; ;
    break;}
case 61:
#line 174 "Sources/rez_parser.y"
{ yyval = RSStringValue::skWStr; ;
    break;}
case 62:
#line 175 "Sources/rez_parser.y"
{ yyval = RSStringValue::skCStr; ;
    break;}
case 63:
#line 178 "Sources/rez_parser.y"
{ yyval = (int)new RSArray(new RSNrValue(16), 0, 2); ;
    break;}
case 64:
#line 179 "Sources/rez_parser.y"
{ error("Unimplemented constant declaration"); ;
    break;}
case 65:
#line 180 "Sources/rez_parser.y"
{ error("Unimplemented constant declaration"); ;
    break;}
case 66:
#line 183 "Sources/rez_parser.y"
{ yyval = (int)new RSArray(new RSNrValue(16), 0, 4); ;
    break;}
case 67:
#line 184 "Sources/rez_parser.y"
{ error("Unimplemented constant declaration"); ;
    break;}
case 68:
#line 185 "Sources/rez_parser.y"
{ error("Unimplemented constant declaration"); ;
    break;}
case 69:
#line 188 "Sources/rez_parser.y"
{ yyval = (int)new RSArray(RS(yyvsp[-1])); ;
    break;}
case 70:
#line 189 "Sources/rez_parser.y"
{ yyval = (int)new RSArray(RS(yyvsp[-1])); ;
    break;}
case 71:
#line 190 "Sources/rez_parser.y"
{ yyval = (int)new RSArray(RS(yyvsp[-1]), yyvsp[-3]); ;
    break;}
case 72:
#line 191 "Sources/rez_parser.y"
{ yyval = (int)new RSArray(RS(yyvsp[-1]), yyvsp[-3]); ;
    break;}
case 73:
#line 192 "Sources/rez_parser.y"
{ yyval = (int)new RSArray(RS(yyvsp[-1]), 0, yyvsp[-4]); ;
    break;}
case 74:
#line 193 "Sources/rez_parser.y"
{ yyval = (int)new RSArray(RS(yyvsp[-1]), 0, yyvsp[-5]); ;
    break;}
case 75:
#line 196 "Sources/rez_parser.y"
{ yyval = (int)new RSSwitch(LST(yyvsp[-1])); ;
    break;}
case 76:
#line 199 "Sources/rez_parser.y"
{ yyval = yyvsp[-1]; LST(yyvsp[-1])->AddItem(RS(yyvsp[0])); ;
    break;}
case 77:
#line 200 "Sources/rez_parser.y"
{ yyval = (int)new BList; LST(yyval)->AddItem(RS(yyvsp[0])); ;
    break;}
case 78:
#line 203 "Sources/rez_parser.y"
{ yyval = (int)new RCase(yyvsp[-3], RS(yyvsp[-1])); ;
    break;}
case 79:
#line 206 "Sources/rez_parser.y"
{ yyval = yyvsp[-2]; RS(yyvsp[-2])->SetNext(RS(yyvsp[0])); ;
    break;}
case 83:
#line 212 "Sources/rez_parser.y"
{ yyval = yyvsp[-2]; RSV(yyval)->SetDefaultValue(yyvsp[0]); ;
    break;}
case 90:
#line 223 "Sources/rez_parser.y"
{ yyval = (int)new RSNrValue(yyvsp[0]); RSV(yyval)->SetDefaultValue((int)RValue(0)); ;
    break;}
case 91:
#line 224 "Sources/rez_parser.y"
{
																int cnt = REvaluate(RE(yyvsp[-1]), head);
																RSNrValue *s, *t = NULL;

																while (cnt--)
																{
																	s = new RSNrValue(yyvsp[-3]);
																	s->SetDefaultValue((int)RValue(0));
																	s->SetNext(t);
																	t = s;
																}
																yyval = (int)s;
															;
    break;}
case 92:
#line 239 "Sources/rez_parser.y"
{ yyval = 1; ;
    break;}
case 93:
#line 240 "Sources/rez_parser.y"
{ yyval = 4; ;
    break;}
case 94:
#line 241 "Sources/rez_parser.y"
{ yyval = 8; ;
    break;}
case 95:
#line 242 "Sources/rez_parser.y"
{ yyval = 16; ;
    break;}
case 96:
#line 243 "Sources/rez_parser.y"
{ yyval = 32; ;
    break;}
case 102:
#line 255 "Sources/rez_parser.y"
{ yyval = (int)new BList; LST(yyval)->AddItem(RS(yyvsp[0])); ;
    break;}
case 103:
#line 256 "Sources/rez_parser.y"
{ yyval = yyvsp[-2]; LST(yyvsp[-2])->AddItem(RS(yyvsp[0])); ;
    break;}
case 104:
#line 259 "Sources/rez_parser.y"
{ yyval = (int)new RSymbol(yyvsp[0], 0); ;
    break;}
case 105:
#line 260 "Sources/rez_parser.y"
{ yyval = (int)new RSymbol(yyvsp[-2], REvaluate(RE(yyvsp[0]), head)); ;
    break;}
case 106:
#line 261 "Sources/rez_parser.y"
{ yyval = (int)new RSymbol(yyvsp[-4], 0); ;
    break;}
case 107:
#line 264 "Sources/rez_parser.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 108:
#line 265 "Sources/rez_parser.y"
{ yyval = (long)RBinaryOp(RE(yyvsp[-2]), RE(yyvsp[0]), reoPlus); ;
    break;}
case 109:
#line 266 "Sources/rez_parser.y"
{ yyval = (long)RBinaryOp(RE(yyvsp[-2]), RE(yyvsp[0]), reoMinus); ;
    break;}
case 110:
#line 267 "Sources/rez_parser.y"
{ yyval = (long)RBinaryOp(RE(yyvsp[-2]), RE(yyvsp[0]), reoMultiply); ;
    break;}
case 111:
#line 268 "Sources/rez_parser.y"
{ yyval = (long)RBinaryOp(RE(yyvsp[-2]), RE(yyvsp[0]), reoDivide); ;
    break;}
case 112:
#line 269 "Sources/rez_parser.y"
{ yyval = (long)RBinaryOp(RE(yyvsp[-2]), RE(yyvsp[0]), reoModulus); ;
    break;}
case 113:
#line 270 "Sources/rez_parser.y"
{ yyval = (long)RBinaryOp(RE(yyvsp[-2]), RE(yyvsp[0]), reoSHL); ;
    break;}
case 114:
#line 271 "Sources/rez_parser.y"
{ yyval = (long)RBinaryOp(RE(yyvsp[-2]), RE(yyvsp[0]), reoSHR); ;
    break;}
case 115:
#line 272 "Sources/rez_parser.y"
{ yyval = (long)RBinaryOp(RE(yyvsp[-2]), RE(yyvsp[0]), reoBitAnd); ;
    break;}
case 116:
#line 273 "Sources/rez_parser.y"
{ yyval = (long)RBinaryOp(RE(yyvsp[-2]), RE(yyvsp[0]), reoBitOr); ;
    break;}
case 117:
#line 274 "Sources/rez_parser.y"
{ yyval = (long)RBinaryOp(RE(yyvsp[-2]), RE(yyvsp[0]), reoXPwrY); ;
    break;}
case 118:
#line 275 "Sources/rez_parser.y"
{ yyval = (long)RUnaryOp(RE(yyvsp[0]), reoFlip); ;
    break;}
case 119:
#line 276 "Sources/rez_parser.y"
{ yyval = (long)RUnaryOp(RE(yyvsp[0]), reoNot); ;
    break;}
case 120:
#line 277 "Sources/rez_parser.y"
{ yyval = (long)RUnaryOp(RE(yyvsp[0]), reoNegate); ;
    break;}
case 121:
#line 278 "Sources/rez_parser.y"
{ yyval = (long)RBinaryOp(RE(yyvsp[-2]), RE(yyvsp[0]), reoAnd); ;
    break;}
case 122:
#line 279 "Sources/rez_parser.y"
{ yyval = (long)RBinaryOp(RE(yyvsp[-2]), RE(yyvsp[0]), reoOr); ;
    break;}
case 123:
#line 280 "Sources/rez_parser.y"
{ yyval = (long)RBinaryOp(RE(yyvsp[-2]), RE(yyvsp[0]), reoLT); ;
    break;}
case 124:
#line 281 "Sources/rez_parser.y"
{ yyval = (long)RBinaryOp(RE(yyvsp[-2]), RE(yyvsp[0]), reoGT); ;
    break;}
case 125:
#line 282 "Sources/rez_parser.y"
{ yyval = (long)RBinaryOp(RE(yyvsp[-2]), RE(yyvsp[0]), reoLE); ;
    break;}
case 126:
#line 283 "Sources/rez_parser.y"
{ yyval = (long)RBinaryOp(RE(yyvsp[-2]), RE(yyvsp[0]), reoGE); ;
    break;}
case 127:
#line 284 "Sources/rez_parser.y"
{ yyval = (long)RBinaryOp(RE(yyvsp[-2]), RE(yyvsp[0]), reoEQ); ;
    break;}
case 128:
#line 285 "Sources/rez_parser.y"
{ yyval = (long)RBinaryOp(RE(yyvsp[-2]), RE(yyvsp[0]), reoNE); ;
    break;}
case 129:
#line 286 "Sources/rez_parser.y"
{ yyval = (long)RValue(yyvsp[0]); ;
    break;}
case 130:
#line 287 "Sources/rez_parser.y"
{ yyval = (long)RIdentifier(yyvsp[0]); ;
    break;}
case 132:
#line 291 "Sources/rez_parser.y"
{ yyval = (long)RFunction(refCountOf, yyvsp[-1]); ;
    break;}
case 133:
#line 293 "Sources/rez_parser.y"
{ yyval = (long)RFunction(refCopyBits, yyvsp[-5], yyvsp[-3], yyvsp[-1]); ;
    break;}
case 134:
#line 294 "Sources/rez_parser.y"
{ yyval = (long)RFunction(refCopyBits, yyvsp[-1], 0, 8); ;
    break;}
case 135:
#line 295 "Sources/rez_parser.y"
{ yyval = (long)RFunction(refCopyBits, yyvsp[-1], 0, 16); ;
    break;}
case 136:
#line 296 "Sources/rez_parser.y"
{ yyval = (long)RFunction(refCopyBits, yyvsp[-1], 0, 32); ;
    break;}
case 137:
#line 300 "Sources/rez_parser.y"
{
																sState = RState::FirstState(yyvsp[-5]);
																if (!sState)
																{
																	int t = ntohl(yyvsp[-5]);
																	error("Undefined resource type: %4.4s", &t);
																}
																head = NULL;
																yyval = (int)new ResHeader(yyvsp[-5], yyvsp[-3], yyvsp[-1]);
															;
    break;}
case 138:
#line 311 "Sources/rez_parser.y"
{
																sState = RState::FirstState(yyvsp[-3]);
																if (!sState)
																{
																	int t = ntohl(yyvsp[-3]);
																	error("Undefined resource type: %4.4s", &t);
																}
																head = NULL;
																yyval = (int)new ResHeader((int)yyvsp[-3], (int)yyvsp[-1], (int)NULL);
															;
    break;}
case 139:
#line 324 "Sources/rez_parser.y"
{ WriteResource((char *)yyvsp[-1], yyvsp[-7], yyvsp[-5], (char *)yyvsp[-3]); ;
    break;}
case 140:
#line 326 "Sources/rez_parser.y"
{ WriteResource((char *)yyvsp[-1], yyvsp[-5], yyvsp[-3], NULL); ;
    break;}
case 144:
#line 334 "Sources/rez_parser.y"
{ CHECKSTATE; sState = sState->Shift(yyvsp[0], tInt, &head); ;
    break;}
case 145:
#line 336 "Sources/rez_parser.y"
{ CHECKSTATE; sState = sState->Shift(yyvsp[0], tString, &head); free((char *)yyvsp[0]); ;
    break;}
case 146:
#line 337 "Sources/rez_parser.y"
{ CHECKSTATE; sState = sState->Shift(yyvsp[0], tRaw, &head); free((char *)yyvsp[0]); ;
    break;}
case 148:
#line 339 "Sources/rez_parser.y"
{ CHECKSTATE; sState = sState->Shift(0, tArrayEnd, &head); ;
    break;}
case 150:
#line 343 "Sources/rez_parser.y"
{ CHECKSTATE; sState = sState->Shift(0, tArray, &head); ;
    break;}
case 151:
#line 346 "Sources/rez_parser.y"
{ CHECKSTATE; sState = sState->Shift(yyvsp[-1], tCase, &head); ;
    break;}
case 153:
#line 350 "Sources/rez_parser.y"
{ char *t, *a, *b;
															  long sa, sb;

															  a = (char *)yyvsp[-1];	sa = *(long *)a;
															  b = (char *)yyvsp[0];	sb = *(long *)b;

															  t = (char *)malloc(sa + sb + sizeof(long));
															  if (!t) error("insufficient memory");

															  memcpy(t + sizeof(long), a + sizeof(long), sa);
															  memcpy(t + sizeof(long) + sa, b + sizeof(long), sb);
															  *(long*)t = sa + sb;

															  free(a); free(b);
															  yyval = (int)t; ;
    break;}
case 155:
#line 368 "Sources/rez_parser.y"
{ char *t, *a, *b;
															  a = (char *)yyvsp[-1];
															  b = (char *)yyvsp[0];
															  t = (char *)malloc(strlen(a) + strlen(b) + 1);
															  if (!t) error("insufficient memory");
															  strcpy(t, a);
															  strcat(t, b);
															  free(a); free(b);
															  yyval = (int)t; ;
    break;}
case 156:
#line 377 "Sources/rez_parser.y"
{ yyval = (int)strdup(getenv((char *)yyvsp[-1])); ;
    break;}
case 157:
#line 378 "Sources/rez_parser.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 158:
#line 381 "Sources/rez_parser.y"
{ char b[1024]; sprintf(b, (char *)yyvsp[-2], yyvsp[0]); yyval = (int)strdup(b); ;
    break;}
case 159:
#line 382 "Sources/rez_parser.y"
{ char b[1024]; sprintf(b, (char *)yyvsp[-4], yyvsp[-2], yyvsp[0]); yyval = (int)strdup(b); ;
    break;}
case 160:
#line 383 "Sources/rez_parser.y"
{ char b[1024]; sprintf(b, (char *)yyvsp[-6], yyvsp[-4], yyvsp[-2], yyvsp[0]); yyval = (int)strdup(b); ;
    break;}
case 161:
#line 384 "Sources/rez_parser.y"
{ char b[1024]; sprintf(b, (char *)yyvsp[-8], yyvsp[-6], yyvsp[-4], yyvsp[-2], yyvsp[0]); yyval = (int)strdup(b); ;
    break;}
case 162:
#line 385 "Sources/rez_parser.y"
{ char b[1024]; sprintf(b, (char *)yyvsp[-10], yyvsp[-8], yyvsp[-6], yyvsp[-4], yyvsp[-2], yyvsp[0]); yyval = (int)strdup(b); ;
    break;}
case 163:
#line 386 "Sources/rez_parser.y"
{ char b[1024]; sprintf(b, (char *)yyvsp[-12], yyvsp[-10], yyvsp[-8], yyvsp[-6], yyvsp[-4], yyvsp[-2], yyvsp[0]); yyval = (int)strdup(b); ;
    break;}
case 164:
#line 387 "Sources/rez_parser.y"
{ char b[1024]; sprintf(b, (char *)yyvsp[-14], yyvsp[-12], yyvsp[-10], yyvsp[-8], yyvsp[-6], yyvsp[-4], yyvsp[-2], yyvsp[0]); yyval = (int)strdup(b); ;
    break;}
case 165:
#line 388 "Sources/rez_parser.y"
{ char b[1024]; sprintf(b, (char *)yyvsp[-16], yyvsp[-14], yyvsp[-12], yyvsp[-10], yyvsp[-8], yyvsp[-6], yyvsp[-4], yyvsp[-2], yyvsp[0]); yyval = (int)strdup(b); ;
    break;}
case 166:
#line 389 "Sources/rez_parser.y"
{ char b[1024]; sprintf(b, (char *)yyvsp[-18], yyvsp[-16], yyvsp[-14], yyvsp[-12], yyvsp[-10], yyvsp[-8], yyvsp[-6], yyvsp[-4], yyvsp[-2], yyvsp[0]); yyval = (int)strdup(b); ;
    break;}
case 167:
#line 390 "Sources/rez_parser.y"
{ char b[1024]; sprintf(b, (char *)yyvsp[-20], yyvsp[-18], yyvsp[-16], yyvsp[-14], yyvsp[-12], yyvsp[-10], yyvsp[-8], yyvsp[-6], yyvsp[-4], yyvsp[-2], yyvsp[0]); yyval = (int)strdup(b); ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 498 "/etc/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 397 "Sources/rez_parser.y"


