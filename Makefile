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

TARGET			= pe
# DEBUG should be either 1 or 0
DEBUG			= 1
DEFINES			= # BETA DEMO
LIBS			= hekkel tracker devel mail textencoding
LIBPATHS		= .
INCPATHS		= 
ifeq ($(CPU), x86)
	WARNINGS	=  -Wall -Wno-multichar -Wno-ctor-dtor-privacy -Wno-sign-compare
	LIBS		+= be root stdc++.r4 
	PREFIXFILE	= 
else
	WARNINGS	= -w all -w nonotinlined -w noimplicit
	LIBS		+= mslcpp_4_0 rx
	INCPATHS	+= ./regex/
	PREFIXFILE	= $(OBJDIR)/pe.headers
	PREFIX		= -prefix $(PREFIXFILE)
	LDFLAGS		= -map pe.xMAP -sym full -nodup -export all
endif
REZ				= ./rez/rez

#%{
# Pe generated file list:
Sources = \
	./Sources/Benaphore.cpp \
	./Sources/CCharObject.cpp \
	./Sources/CConvertDialog.cpp \
	./Sources/CDiff.cpp \
	./Sources/CDiffWindow.cpp \
	./Sources/CDoc.cpp \
	./Sources/CFindDialog.cpp \
	./Sources/CFtpDialog.cpp \
	./Sources/CFtpListItem.cpp \
	./Sources/CFtpStream.cpp \
	./Sources/CGlossary.cpp \
	./Sources/CGoToLine.cpp \
	./Sources/CGrepBox.cpp \
	./Sources/CHTMLBar.cpp \
	./Sources/CInfoDialog.cpp \
	./Sources/CKeyCapturer.cpp \
	./Sources/CKeyMapper.cpp \
	./Sources/CKeywords.cpp \
	./Sources/CLangIntf.cpp \
	./Sources/CLanguageAddOn.cpp \
	./Sources/CLineObject.cpp \
	./Sources/CListBox.cpp \
	./Sources/CLogin.cpp \
	./Sources/CMenuItem.cpp \
	./Sources/CMessageItem.cpp \
	./Sources/CMimeBox.cpp \
	./Sources/COpenSelection.cpp \
	./Sources/CPathsBox.cpp \
	./Sources/CPrefsDialog.cpp \
	./Sources/CStdErrBox.cpp \
	./Sources/CStdErrParser.cpp \
	./Sources/CUrlOpener.cpp \
	./Sources/FontStyle.cpp \
	./Sources/PAbout.cpp \
	./Sources/PApp.cpp \
	./Sources/PCmd.cpp \
	./Sources/PDoc.cpp \
	./Sources/PErrorWindow.cpp \
	./Sources/PGlossyWindow.cpp \
	./Sources/PGroupWindow.cpp \
	./Sources/PItalicMenuItem.cpp \
	./Sources/PKeyDownFilter.cpp \
	./Sources/PLongAction.cpp \
	./Sources/PMessageWindow.cpp \
	./Sources/PProjectWindow.cpp \
	./Sources/PScrollBar.cpp \
	./Sources/PSplitter.cpp \
	./Sources/PStatus.cpp \
	./Sources/PText.cpp \
	./Sources/PTextBuffer.cpp \
	./Sources/PToolBar.cpp \
	./Sources/PTypeAHeadList.cpp \
	./Sources/Sstdio.cpp \
	./Sources/Utils.cpp \
	./Sources/utf-support.cpp \

HSources = \
	./Sources/HAppResFile.cpp \
	./Sources/HBox.cpp \
	./Sources/HButtonBar.cpp \
	./Sources/HColorControl.cpp \
	./Sources/HColorPicker2.cpp \
	./Sources/HColorUtils.cpp \
	./Sources/HDialog.cpp \
	./Sources/HDlogView.cpp \
	./Sources/HError.cpp \
	./Sources/HHelpWindow.cpp \
	./Sources/HLazyObject.cpp \
	./Sources/HPreferences.cpp \
	./Sources/HStream.cpp \
	./Sources/HTabSheet.cpp \
	./Sources/MAlert.cpp \
	./Sources/MTextAddOn.cpp \
	./Sources/MThread.cpp \

Resources = \
	./Resources/menus.r \
	./Resources/fonttabs.r \
	./Resources/find.r \
	./Resources/prefs.r \
	./Resources/wrap.r \
	./Resources/icons.r \
	./Resources/colorpicker.r \
	./Resources/openfile.r \
	./Resources/gotoline.r \
	./Resources/changecase.r \
	./Resources/info.r \
	./Resources/convert.r \
	./Resources/Bindings.r \
	./Resources/UTF-Tables.r \
	./Resources/UTF-Mappings.r \
	./Resources/ToolBar.r \
	./Resources/WordBreakTable.r \
	./Resources/RHTMLBar.r \
	./Resources/RDiffToolbar.r \
	./Resources/ftp.r \
	./Resources/login.r \
	./Resources/RCursor.r \
	./Resources/RFtpOpenDialog.r \
	./Resources/RProject.r \
	./Resources/RMimeinfo.r \
#%}

define SOURCES
	$(Sources)
endef

define RESOURCES
	$(Resources)
endef

ifeq ($(CPU), x86)
	ifeq ($(DEBUG), 1)
		DBG		= -gdwarf-2
		OPT		= -O0
		OBJDIR	= Obj.Debug
	else
		DBG		= 
		OPT		= -O3
		OBJDIR	= Obj.NoDebug
	endif
else
	ifeq ($(DEBUG), 1)
		DBG		= -g
		OPT		= -O0
		OBJDIR	= Obj.Debug
	else
		DBG		= 
		OPT		= -O7
		OBJDIR	= Obj.NoDebug
	endif
endif

define SRC_TO_OBJ
	$(addprefix $(OBJDIR)/, $(addsuffix .o, $(foreach file, $(SOURCES), $(basename $(notdir $(file))))))
endef

define HSRC_TO_OBJ
	$(addprefix $(OBJDIR)/, $(addsuffix .o, $(foreach file, $(HSources), $(basename $(notdir $(file))))))
endef

define R_TO_RSRC
	$(addprefix $(OBJDIR)/, $(addsuffix .rsrc, $(foreach file, $(RESOURCES), $(basename $(notdir $(file))))))
endef


OBJS = $(SRC_TO_OBJ)
HOBJS = $(HSRC_TO_OBJ)
RSRC = $(R_TO_RSRC)

# create a unique list of paths to our sources
SRC_PATHS += $(sort $(foreach file, $(SOURCES), $(dir $(file))))
R_PATHS += $(sort $(foreach file, $(RESOURCES), $(dir $(file))))

VPATH :=
# add the list of source paths to VPATH if not already present
VPATH += $(addprefix :, $(subst  ,:, $(filter-out $($(subst, :, ,$(VPATH))), $(SRC_PATHS))))
VPATH += $(addprefix :, $(subst  ,:, $(filter-out $($(subst, :, ,$(VPATH))), $(R_PATHS))))

# add the list of source paths to INLCUDES if not already present
INCLUDES = $(foreach path, $(SRC_PATHS) $(INCPATHS), $(addprefix -I, $(path)))
RINCLUDES = $(foreach path, $(R_PATHS) $(RINCPATHS), $(addprefix -I, $(path)))

# add the -L prefix to all library paths to search
LINK_PATHS = $(foreach path, $(LIBPATHS), $(addprefix -L, $(path)))

# add the -l prefix to all libs to be linked against
LINK_LIBS = $(foreach lib, $(LIBS), $(addprefix -l, $(lib)))

# add to the linker flags 
LDFLAGS += $(LINK_PATHS) $(LINK_LIBS)

CFLAGS	+= $(DBG) $(OPT)
ifeq ($(CPU), x86)
CFLAGS	+= -fpic
endif
CFLAGS	+= $(foreach define, $(DEFINES), $(addprefix -D, $(define)))

all: rez_compiler librx $(TARGET) lang ext

$(TARGET): $(OBJDIR) libhekkel.a $(OBJS) $(RSRC)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)
ifeq ($(DEBUG), 0)
	strip $(TARGET)
endif
	xres -o $(TARGET) $(RSRC)
	$(REZ) -o $(TARGET) Resources/Now.r
	mimeset -f $(TARGET)

clean:
	rm -rf Obj.* *.xSYM *.xMAP

$(OBJDIR)/%.rsrc: %.r
	@ echo "-> "$(@F)
ifeq ($(CPU), x86)
	@ gcc -E -x c -o - $(RINCLUDES) $(INCLUDES) $< | sed 's/^#.*//g' > $@_tmp
else
	@ mwcc -preprocess -o $@_tmp $(RINCLUDES) $(INCLUDES) $<
endif
	@ $(REZ) -o $@ -t $(RINCLUDES) $@_tmp

$(OBJDIR):
	@ test -d $(OBJDIR) || mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.cpp $(PREFIXFILE)
	@ echo "-> "$(@F)
	@ $(CC) -c $< -o $@ $(INCLUDES) $(PREFIX) $(CFLAGS)

include $(OBJS:%.o=%.d)

$(PREFIXFILE): Sources/pe.pch++
ifeq ($(CPU),ppc)
	mwcc -precompile $(PREFIXFILE) -lang cplus Sources/pe.pch++ $(INCLUDES)
endif

$(OBJDIR)/%.d: %.cpp $(PREFIXFILE)
	@ test -d $(OBJDIR) || mkdir $(OBJDIR)
	@ echo "-> "$(@F)
ifeq ($(CPU), x86)
	@ $(SHELL) -ec '$(CC) -M $(INCLUDES) $< | sed '\''s/\(.*\.o\)[ :]*/$(OBJDIR)\/\1 $(@F): /g'\'' > $@'
else
	@ $(SHELL) -ec '$(CC) -prefix $(PREFIXFILE) -make $(INCLUDES) $< | sed '\''s/\(.*\.o\)[ :]*/$(OBJDIR)\/\1 $(@F): /g'\'' > $@'
endif

FORCE:

rez_compiler:
	cd ./rez && $(MAKE)

libhekkel.a: $(HOBJS)
	rm -rf $@
	ar -cru $@ $(HOBJS)

librx:
ifeq ($(CPU), ppc)
	cd regex && $(MAKE) librx.so
	mv regex/librx.so .
endif

lang:
	cd Languages && $(MAKE)

ext: pe libhekkel.a
ifeq ($(CPU), x86)
	cp pe _APP_
endif
	cd Extensions && $(MAKE)

