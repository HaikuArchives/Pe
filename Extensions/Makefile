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

MACHINE		= $(shell uname -m)
ifeq ($(MACHINE), BePC)
	CPU	= x86
else
	CPU	= ppc
endif

DEBUG		= 0
DEFS		= -DDEBUG=$(DEBUG)
LIBS		= hekkel mail
LIBPATHS	= ..
LDFLAGS		= 
CFLAGS		= -c -I. -I../Sources -I- -I../regex 

ifeq ($(CPU), x86)
	ifeq ($(DEBUG), 1)
		DBG		= -gdwarf-2
		OPT		= -O0
	else
		DBG		= 
		OPT		= -O3
	endif

	CFLAGS	+= -Wall -Wno-multichar -Wno-ctor-dtor-privacy -Wno-sign-compare -fpic
	LDFLAGS	+= -nostart ../_APP_
	LIBS	+= be root stdc++.r4 
	STRIP	= strip
	
else
	ifeq ($(DEBUG), 1)
		DBG		= -sym full
		OPT		= -O0
	else
		DBG		= -sym off
		OPT		= -O7
	endif

	LDFLAGS	+= -shared -export pragma -nodup
	LIBS	+= rx mslcpp_4_0
	STRIP	= ls
endif

CFLAGS		+= $(DBG) $(OPT) $(DEFS)

REZ		= ../rez/rez

# add the -L prefix to all library paths to search
LINK_PATHS = $(foreach path, $(LIBPATHS), $(addprefix -L, $(path)))

# add the -l prefix to all libs to be linked against
LINK_LIBS = $(foreach lib, $(LIBS), $(addprefix -l, $(lib)))

# add to the linker flags 
LDFLAGS += $(LINK_PATHS) $(LINK_LIBS)

.SUFFIXES: .r .rsrc .o .cpp .h

#	/boot/home/projects/lib/MLib \
#	/boot/home/projects/lib/HLib \
#	-lbe -lroot

all: PrefixLines Copy\ Lines\ Containing Cut\ Lines\ Containing Pipe HTMLImage \
	HTMLAnchor ROT13 drieuxCaps HTMLUpdate
	touch extensions

clean:
	rm -f *.o *.rsrc *.xMAP *.dlog.h extensions

PrefixLines: PrefixLines.dlog.h PrefixLines.o
	$(CC) -o $@ PrefixLines.o $(LDFLAGS)
	$(STRIP) $@

PrefixLines.dlog.h: prefix.r
	$(REZ) -h -o $@ $<

.cpp.o:
	$(CC) $(CFLAGS) -o $@ $<

Copy\ Lines\ Containing: CopyContaining.dlog.h CopyContaining.o
	$(CC) -o clc CopyContaining.o $(LDFLAGS)
	$(STRIP) clc
	mv clc '$@'

CopyContaining.dlog.h: copycon.r
	$(REZ) -h -o $@ $<

Cut\ Lines\ Containing: CopyContaining.dlog.h CutContaining.o
	$(CC) -o clc2 CutContaining.o $(LDFLAGS)
	$(STRIP) clc2
	mv clc2 '$@'

Pipe: Pipe.dlog.h Pipe.o
	$(CC) -o $@ Pipe.o $(LDFLAGS)
	$(STRIP) $@

Pipe.dlog.h: Pipe.dlog.r
	$(REZ) -h -o $@ $<

HTMLImage: HTMLImage.dlog.h HTMLImage.o
	$(CC) -o $@ HTMLImage.o $(LDFLAGS)
	$(STRIP) $@

HTMLImage.dlog.h: HTMLImage.dlog.r
	$(REZ) -h -o $@ $<

HTMLAnchor: HTMLAnchor.dlog.h HTMLAnchor.o
	$(CC) -o $@ HTMLAnchor.o $(LDFLAGS)
	$(STRIP) $@

HTMLUpdate: HTMLUpdate.o
	$(CC) -o $@ HTMLUpdate.o $(LDFLAGS)
	$(STRIP) $@

HTMLAnchor.dlog.h: HTMLAnchor.dlog.r
	$(REZ) -h -o $@ $<

ROT13: ROT13.o
	$(CC) -o $@ ROT13.o $(LDFLAGS)
	$(STRIP) $@

drieuxCaps: drieuxCaps.o JRandom.o
	$(CC) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@

HelloWorld: HelloWorld.o
	$(CC) -o $@ HelloWorld.o $(LDFLAGS)
	$(STRIP) $@
