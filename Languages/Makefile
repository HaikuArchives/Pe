#	$Id$
#	
#	Copyright 1996, 1997, 1998, 2002
#	        Hekkelman Programmatuur B.V.  All rights reserved.
#	
#	Redistribution and use in source and binary forms, with or without
#	modification, are permitted provided that the following conditions are met:
#	1. Redistributions of source code must retain the above copyright notice,
#	   this list of conditions and the following disclaimer.
#	2. Redistributions in binary form must reproduce the above copyright notice,
#	   this list of conditions and the following disclaimer in the documentation
#	   and/or other materials provided with the distribution.
#	3. All advertising materials mentioning features or use of this software
#	   must display the following acknowledgement:
#	   
#	    This product includes software developed by Hekkelman Programmatuur B.V.
#	
#	4. The name of Hekkelman Programmatuur B.V. may not be used to endorse or
#	   promote products derived from this software without specific prior
#	   written permission.
#	
#	THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
#	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
#	FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
#	AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#	OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
#	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 	
# 
#	Makefile for making language extensions
#

MACHINE         = $(shell uname -m)
ifeq ($(MACHINE), BePC)
        CPU     = x86
else
        CPU     = ppc
endif

ifeq ($(CPU), x86)
	CFLAGS		= -c -I Sources -I ../Sources -include Sources/CLangIncl.h -O3
	LDFLAGS		= -lbe -lroot -nostart -gdwarf-2
	LIBS		= -lstdc++.r4
	STRIP		= strip
else
	CFLAGS		= -c -ISources -I../Sources -prefix BeHeaders \
				-prefix /boot/home/projects/pe/Languages/Sources/CLangIncl.h -O7
	LDFLAGS		= -lbe -lroot -shared -export pragma -nodup
	STRIP		= ls	# hmm...
endif

CC		= $(BE_C_COMPILER)
REZ		= ../../rez/rez

vpath %.o Sources
vpath %.cpp Sources
vpath %.pch++ Sources
vpath %.headers Sources
vpath %.h Sources
vpath %.rsrc Resources
vpath %.r Resources
vpath %.l Sources

.SUFFIXES: .cpp .o .l .r .rsrc .java .ob2 .pl .py .rez .tex .m

all:	CppExt  JavaExt OberonExt PerlExt PythonExt RezExt TexExt \
	ShellExt HtmlJSExt PascalExt SQLExt # MathematicaExt 

#HtmlExt

clean:
	rm -f Sources/*.o Resources/*.rsrc Sources/*FuncPopup.cpp

CppExt: CLang_cpp.o CcppPopup.o Resources/keywords.cpp.rsrc
	$(CC) -o $@ Sources/CLang_cpp.o Sources/CcppPopup.o $(LIBS) $(LDFLAGS)
	$(STRIP) $@
	xres -o $@ Resources/keywords.cpp.rsrc

PascalExt: CLang_pas.o CpasPopup.o Resources/keywords.pas.rsrc
	$(CC) -o $@ Sources/CLang_pas.o Sources/CpasPopup.o $(LIBS) $(LDFLAGS)
	$(STRIP) $@
	xres -o $@ Resources/keywords.pas.rsrc

SQLExt: CLang_sql.o Resources/keywords.sql.rsrc
	$(CC) -o $@ Sources/CLang_sql.o $(LIBS) $(LDFLAGS)
	$(STRIP) $@
	xres -o $@ Resources/keywords.sql.rsrc
	
JavaExt: CLang_java.o CjavaFuncPopup.o Resources/keywords.java.rsrc
	$(CC) -o $@ Sources/CLang_java.o Sources/CjavaFuncPopup.o $(LIBS) $(LDFLAGS)
	$(STRIP) $@
	xres -o $@ Resources/keywords.java.rsrc

HtmlExt: CLang_html.o ChtmlPopup.o # Resources/keywords.html.rsrc
	cd Resources; $(REZ) -o keywords.html.rsrc -t keywords.html.r
	$(CC) -o $@ Sources/CLang_html.o Sources/ChtmlPopup.o $(LIBS) $(LDFLAGS)
	$(STRIP) $@
	xres -o $@ Resources/keywords.html.rsrc

HtmlJSExt: CLang_html_2.o ChtmlPopup.o # Resources/keywords.html.rsrc
	cd Resources; $(REZ) -o keywords.html-js.rsrc -t keywords.html-js.r
	$(CC) -o $@ Sources/CLang_html_2.o Sources/ChtmlPopup.o $(LIBS) $(LDFLAGS)
	$(STRIP) $@
	xres -o $@ Resources/keywords.html-js.rsrc

OberonExt: CLang_ob2.o Cob2FuncPopup.o Resources/keywords.ob2.rsrc
	$(CC) -o $@ Sources/CLang_ob2.o Sources/Cob2FuncPopup.o $(LIBS) $(LDFLAGS)
	$(STRIP) $@
	xres -o $@ Resources/keywords.ob2.rsrc

PerlExt: CLang_perl.o CperlFuncPopup.o Resources/keywords.pl.rsrc
	$(CC) -o $@ Sources/CLang_perl.o Sources/CperlFuncPopup.o $(LIBS) $(LDFLAGS)
	$(STRIP) $@
	xres -o $@ Resources/keywords.pl.rsrc

PythonExt: CLang_python.o CpythonFuncPopup.o Resources/keywords.py.rsrc
	$(CC) -o $@ Sources/CLang_python.o Sources/CpythonFuncPopup.o $(LIBS) $(LDFLAGS)
	$(STRIP) $@
	xres -o $@ Resources/keywords.py.rsrc

RezExt: CLang_rez.o CrezFuncPopup.o Resources/keywords.rez.rsrc
	$(CC) -o $@ Sources/CLang_rez.o Sources/CrezFuncPopup.o $(LIBS) $(LDFLAGS)
	$(STRIP) $@
	xres -o $@ Resources/keywords.rez.rsrc

TexExt: CLang_tex.o CtexFuncPopup.o Resources/keywords.tex.rsrc
	$(CC) -o $@ Sources/CLang_tex.o Sources/CtexFuncPopup.o $(LIBS) $(LDFLAGS)
	$(STRIP) $@
	xres -o $@ Resources/keywords.tex.rsrc

MathematicaExt: CLang_m.o CmFuncPopup.o Resources/keywords.m.rsrc
	$(CC) -o $@ Sources/CLang_m.o Sources/CmFuncPopup.o $(LIBS) $(LDFLAGS)
	$(STRIP) $@
	xres -o $@ Resources/keywords.m.rsrc

ShellExt: CLang_shell.o Resources/keywords.sh.rsrc
	$(CC) -o $@ Sources/CLang_shell.o $(LIBS) $(LDFLAGS)
	$(STRIP) $@
	xres -o $@ Resources/keywords.sh.rsrc
	

CLang_cpp.o:	CLang_cpp.cpp ../Sources/CLanguageAddOn.h

.cpp.o:
	$(CC) -o $(<D)/$(@F) $(CFLAGS) $<


Resources/keywords.cpp.rsrc: Resources/keywords.cpp Resources/keywords.cpp.r
	cd Resources; $(REZ) -o keywords.cpp.rsrc -t keywords.cpp.r

Resources/keywords.pas.rsrc: Resources/keywords.pas Resources/keywords.pas.r
	cd Resources; $(REZ) -o keywords.pas.rsrc -t keywords.pas.r
	
Resources/keywords.sql.rsrc: Resources/keywords.sql Resources/keywords.sql.r
	cd Resources; $(REZ) -o keywords.sql.rsrc -t keywords.sql.r
	
Resources/keywords.html.rsrc: Resources/keywords.html Resources/keywords.html.r
	cd Resources; $(REZ) -o keywords.html.rsrc -t keywords.html.r

Resources/keywords.java.rsrc: Resources/keywords.java Resources/keywords.java.r
	cd Resources; $(REZ) -o keywords.java.rsrc -t keywords.java.r

Resources/keywords.ob2.rsrc: Resources/keywords.ob2 Resources/keywords.ob2.r
	cd Resources; $(REZ) -o keywords.ob2.rsrc -t keywords.ob2.r

Resources/keywords.m.rsrc: Resources/keywords.m Resources/keywords.m.r
	cd Resources; $(REZ) -o keywords.m.rsrc -t keywords.m.r

Resources/keywords.pl.rsrc: Resources/keywords.pl Resources/keywords.pl.r
	cd Resources; $(REZ) -o keywords.pl.rsrc -t keywords.pl.r

Resources/keywords.py.rsrc: Resources/keywords.py Resources/keywords.py.r
	cd Resources; $(REZ) -o keywords.py.rsrc -t keywords.py.r

Resources/keywords.rez.rsrc: Resources/keywords.rez Resources/keywords.rez.r
	cd Resources; $(REZ) -o keywords.rez.rsrc -t keywords.rez.r

Resources/keywords.tex.rsrc: Resources/keywords.tex Resources/keywords.tex.r
	cd Resources; $(REZ) -o keywords.tex.rsrc -t keywords.tex.r

Resources/keywords.sh.rsrc: Resources/keywords.sh Resources/keywords.sh.r
	cd Resources; $(REZ) -o keywords.sh.rsrc -t keywords.sh.r

Sources/CjavaFuncPopup.cpp: Sources/CjavaFuncPopup.l
	flex -oSources/CjavaFuncPopup.cpp -Pjava_ Sources/CjavaFuncPopup.l

Sources/Cob2FuncPopup.cpp: Sources/Cob2FuncPopup.l
	flex -oSources/Cob2FuncPopup.cpp -Pob2_ Sources/Cob2FuncPopup.l

Sources/CmFuncPopup.cpp: Sources/CmFuncPopup.l
	flex -oSources/CmFuncPopup.cpp -Pm_ Sources/CmFuncPopup.l

Sources/CperlFuncPopup.cpp: Sources/CperlFuncPopup.l
	flex -oSources/CperlFuncPopup.cpp -Pperl_ Sources/CperlFuncPopup.l

Sources/CpythonFuncPopup.cpp: Sources/CpythonFuncPopup.l
	flex -oSources/CpythonFuncPopup.cpp -Ppython_ Sources/CpythonFuncPopup.l

Sources/CrezFuncPopup.cpp: Sources/CrezFuncPopup.l
	flex -i -oSources/CrezFuncPopup.cpp -Prez_ Sources/CrezFuncPopup.l

Sources/CtexFuncPopup.cpp: Sources/CtexFuncPopup.l
	flex -oSources/CtexFuncPopup.cpp -Ptex_ Sources/CtexFuncPopup.l
