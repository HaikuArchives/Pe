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

/*	This is the resource file that generates the HTML Palette for Pe.
	
	You can modify this palette if you want to and replace the default
	palette with your own customized version. To do so, select the following
	lines and execute them:

cd $PE_DIR/Goodies
rez -o "HTML Palette" RHTMLPalette.r
mv "HTML Palette" $PE_SETTINGS_DIR
	
	This will compile the RHTMLBar.r file in the goodies directory, that's the
	file you're reading now. The result will be moved to the settingsdir of Pe.
	Now if you quit Pe and relaunch it you will see it has a new palette (if you
	changed it) containing the new items you added or changed.
*/

//#include "PMessages.h"
#include "ResourcesMisc.h"

#define msg_ShowInBrowser		'Brws'
#define msg_HtmlNew				'HNew'


type rtyp_Htmp {
	cstring;									// Bar name
	longint;									// Flags (unused for now)
	longint;									// Width of the bar
	longint = $$CountOf(items);
	array items {
		switch {
			case Simple:
				key longint = 'btn ';
				cstring;						// Label
				cstring;						// Glossary
			case Menu:
				key longint = 'menu';
				cstring;						// Label
				array {
					cstring;					// MenuItemLabel
					cstring;					// Glossary
				};
				byte = 0;
			case Dialog:
				key longint = 'dlog';
				cstring;						// Label
				cstring;						// Extension name
			case Command:
				key longint = 'cmnd';
				cstring;						// Label
				longint;						// Command
		}
	};
};

resource rtyp_Htmp (rid_Htmp_HtmlPalette) {
	"Html Palette",
	0,
	85,
	{
		Command	{ "New", msg_HtmlNew },
		Menu		{ "Font",
						{
							"Teletype",		"<tt>$selection$$insert$</tt>",
							"Italic",		"<i>$selection$$insert$</i>",
							"Bold",			"<b>$selection$$insert$</b>",
							"Big",			"<big>$selection$$insert$</big>",
							"Small",		"<small>$selection$$insert$</small>",
							"Superscript",	"<sup>$selection$$insert$</sup>",
							"Subscript",	"<sub>$selection$$insert$</sub>",
						}
					},
		Menu		{ "Structure",
						{
							"Emphasis",		"<em>$selection$$insert$</em>",
							"Strong",		"<strong>$selection$$insert$</strong>",
							"Citation",		"<cite>$selection$$insert$</cite>",
							"Definition",	"<dfn>$selection$$insert$</dfn>",
							"Emphasis",		"<em>$selection$$insert$</em>",
							"Code",			"<code>$selection$$insert$</code>",
							"Sample",		"<samp>$selection$$insert$</samp>",
							"Keyboard",		"<kbd>$selection$$insert$</kbd>",
							"Variable",		"<var>$selection$$insert$</var>",
							"Abbreviated",	"<abbr>$selection$$insert$</abbr>",
							"Acronym",		"<acronym>$selection$$insert$</acronym>",
							"Preformatted",	"<pre>$selection$$insert$</pre>",
						}
					},
		Menu		{ "Heading",
						{
							"Heading 1",	"<h1>$selection$$insert$</h1>",
							"Heading 2",	"<h2>$selection$$insert$</h2>",
							"Heading 3",	"<h3>$selection$$insert$</h3>",
							"Heading 4",	"<h4>$selection$$insert$</h4>",
							"Heading 5",	"<h5>$selection$$insert$</h5>",
							"Heading 6",	"<h6>$selection$$insert$</h6>"
						}
					},
		Menu		{ "Quotation",
						{
							"Blockquote",	"<blockquote>\n\t<p>$selection$$insert$\n</blockquote>\n",
							"Quote",		"<q>$selection$$insert$</q>"
						}
					},
		Menu		{ "Lists",
						{
							"Ordered",		"<ol>\n\t<li>$selection$$insert$\n</ol>",
							"Unordered",	"<ul>\n\t<li>$selection$$insert$\n</ul>",
							"Definition",	"<dl>\n\t<dt>$selection$$insert$\n\t<dd>\n</dl>"
						}
					},
		Dialog	{ "Image…",		"HTMLImage" },
		Dialog	{ "Anchor…",	"HTMLAnchor" },
		Dialog	{ "Update",		"HTMLUpdate" },
		Simple	{ "Paragraph",	"<p>$selection$$insert$\n</p>\n" },
		Simple	{ "Line break",	"<br>\n" },
		Command	{ "Preview",	msg_ShowInBrowser }
	}
};
