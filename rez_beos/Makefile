# Copyright 1996, 1997, 1998
#         Hekkelman Programmatuur B.V.  All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. All advertising materials mentioning features or use of this software
#    must display the following acknowledgement:
#    
#     This product includes software developed by Hekkelman Programmatuur B.V.
# 
# 4. The name of Hekkelman Programmatuur B.V. may not be used to endorse or
#    promote products derived from this software without specific prior
#    written permission.
# 
# THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
# AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 

MACHINE		= $(shell uname -m)
ifeq ($(MACHINE), BePC)
	CPU	= x86
else
	CPU	= ppc
endif

TARGET		= rez
# DEBUG should be either 1 or 0
DEBUG		= 1
DEFINES		= # BETA DEMO
BUILDFIRST	= 
#BUILDLAST	=
LIBS		= 
LIBPATHS	= 
INCPATHS	= 

ifeq ($(CPU), x86)
	WARNINGS= # -Wall -Wno-multichar -Wno-ctor-dtor-privacy -Wno-sign-compare
	LIBS	+= be stdc++.r4
	CFLAGS	= 
	LDFLAGS	=
else
	WARNINGS= -w all -w nonotinlined -w noimplicit
	LIBS	+= mslcpp_4_0
	CFLAGS	= -prefix /boot/develop/headers/be/precompiled/BeHeaders
	LDFLAGS	= -map rez.xMAP -sym full -nodup
endif

#%{
# Pe generated file list:
Sources = \
	./Sources/rez_parser.cpp \
	./Sources/rez_scanner.cpp \
	./Sources/RState.cpp \
	./Sources/SymbolTable.cpp \
	./Sources/rez.cpp \
	./Sources/REval.cpp \
	./Sources/RElem.cpp \

Others = \
	./Sources/rez_parser.y \
	./Sources/rez_scanner.l \
#%}

define SOURCES
	$(Sources)
endef

define RESOURCES
	$(Resources)
endef

ifeq ($(CPU), x86)
	ifeq ($(DEBUG), 1)
		DBG	= -gdwarf-2
		OPT	= -O0
		OBJDIR	= Obj.Debug
	else
		DBG	= 
		OPT	= -O3
		OBJDIR	= Obj.NoDebug
	endif
else
	ifeq ($(DEBUG), 1)
		DBG	= -g
		OPT	= -O0
		OBJDIR	= Obj.Debug
	else
		DBG	= 
		OPT	= -O7
		OBJDIR	= Obj.NoDebug
	endif
endif

define SRC_TO_OBJ
	$(addprefix $(OBJDIR)/, $(addsuffix .o, $(foreach file, $(SOURCES), $(basename $(notdir $(file))))))
endef

define R_TO_RSRC
	$(addprefix $(OBJDIR)/, $(addsuffix .rsrc, $(foreach file, $(RESOURCES), $(basename $(notdir $(file))))))
endef

OBJS = $(SRC_TO_OBJ)
RSRC += $(R_TO_RSRC)

# create a unique list of paths to our sources
SRC_PATHS += $(sort $(foreach file, $(SOURCES), $(dir $(file))))
R_PATHS += $(sort $(foreach file, $(RESOURCES), $(dir $(file))))

VPATH :=
# add the list of source paths to VPATH if not already present
VPATH += $(addprefix :, $(subst  ,:, $(filter-out $($(subst, :, ,$(VPATH))), $(SRC_PATHS))))
VPATH += $(addprefix :, $(subst  ,:, $(filter-out $($(subst, :, ,$(VPATH))), $(R_PATHS))))

# add the list of source paths to INLCUDES if not already present
INCLUDES = $(foreach path, $(INCPATHS) $(SRC_PATHS), $(addprefix -I, $(path)))
RINCLUDES = $(foreach path, $(RINCPATHS) $(R_PATHS), $(addprefix -I, $(path)))

# add the -L prefix to all library paths to search
LINK_PATHS = $(foreach path, $(LIBPATHS), $(addprefix -L, $(path)))

# add the -l prefix to all libs to be linked against
LINK_LIBS = $(foreach lib, $(LIBS), $(addprefix -l, $(lib)))

# add to the linker flags 
LDFLAGS += $(LINK_PATHS) $(LINK_LIBS)

CFLAGS	+= $(DBG) $(OPT)
ifeq ($(CPU), x86)
	CFLAGSi += -fpic
endif
CFLAGS	+= $(foreach define, $(DEFINES), $(addprefix -D, $(define)))

all: $(TARGET) $(BUILDLAST)

$(TARGET): $(BUILDFIRST) $(OBJDIR) $(OBJS) $(RSRC)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)
ifeq ($(DEBUG), 0)
	strip $(TARGET)
endif
#	xres -o $(TARGET) $(OBJDIR)/*.rsrc
	mimeset -f $(TARGET)

clean:
	rm -rf Obj.Debug Obj.NoDebug

$(OBJDIR)/%.rsrc: %.r
	@ echo "-> "$(@F)
	@ rez -o $@ -t $(RINCLUDES) $<

$(OBJDIR):
	@ test -d $(OBJDIR) || mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.cpp
	@ echo "-> "$(@F)
	@ $(CC) -c $< -o $@ $(INCLUDES) $(CFLAGS)

include $(OBJS:%.o=%.d)

$(OBJDIR)/%.d:%.cpp
	@ test -d $(OBJDIR) || mkdir $(OBJDIR)
	@ echo "-> "$(@F)
ifeq ($(CPU), x86)
	@ $(SHELL) -ec '$(CC) -M $(INCLUDES) $< | sed '\''s/\(.*\.o\)[ :]*/$(OBJDIR)\/\1 $(@F): /g'\'' > $@'
else
	@ $(SHELL) -ec '$(CC) -prefix BeHeaders -make $(INCLUDES) $< | sed '\''s/\(.*\.o\)[ :]*/$(OBJDIR)\/\1 $(@F): /g'\'' > $@'
endif

FORCE:

Sources/rez_parser.cpp rez_parser.cpp: Sources/rez_parser.y
	bison -d -o Sources/rez_parser.cpp $<

Sources/rez_scanner.cpp rez_scanner.cpp: Sources/rez_scanner.l
	flex -i -oSources/rez_scanner.cpp $<

dist:
	mkdir dist
	cp -r Sources/ Resources/ Makefile COPYRIGHT dist
	tar cvzf rez_dist_`date | awk '{print $$2"-"$$3}'`.tgz dist/
	rm -r dist
