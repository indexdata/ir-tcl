# IRTCL makefile for MS NMAKE 
# $Id: makefile,v 1.4 2003-03-05 22:06:32 adam Exp $
#
# Log at the end of the file
VERSION=1.4.1
 
###########################################################
############### Parameters 
###########################################################

DEBUG=0   # 0 for release, 1 for debug

default: all

all: dirs irtcl

###########################################################
############### Directories
###########################################################
# The current directory is supposed to be something like
# ..../IRTCL/Win, everything is relative to that
ROOTDIR=..   # The home of IRTCL

# TCL include files, libraries, etc.
TCLINCL="c:\program files\tcl\include"
TCLLIB="c:\program files\tcl\lib\tclstub83.lib"

# YAZ include files, libraries, etc.
YAZDIR=$(ROOTDIR)\..\YAZ
YAZLIB=$(YAZDIR)\lib\yaz.lib
YAZINCL=$(YAZDIR)\include

# IRTCL Include files, libraries, programs, etc.
INCLDIR=$(ROOTDIR)          # our includes
LIBDIR=$(ROOTDIR)\lib       # We produce .lib, .exp etc there
BINDIR=$(ROOTDIR)\bin       # We produce exes and dlls there
WINDIR=$(ROOTDIR)\win       # all these Win make things
OBJDIR=$(WINDIR)\obj        # where we store intermediate files
UNIXDIR=$(ROOTDIR)\unix     # corresponding unix things
SRCDIR=$(ROOTDIR)           # for the case we move them under src

# Force temp files in a local temp, easier to clean
# when nmake crashes and leaves a lot of rubbish behind
TMPDIR=$(ROOTDIR)\win\tmp
TMP=$(TMPDIR)
TEMP=$(TMPDIR)

###########################################################
############### Targets - what to make
###########################################################

IRTCLDLL=$(BINDIR)\irtcl.dll
irtcl : $(IRTCLDLL)

###########################################################
############### Compiler and linker options 
###########################################################


### C and CPP compiler  (the same thing)
# Note: $(CPP) has already been defined in the environment
# (if you set things up right!)

COMMON_C_OPTIONS=          \
  /nologo /W3 /GX /FD /c   \
  /D "WIN32" \
  /D "IR_TCL_VERSION=\"$(VERSION)\""  \
  /D USE_TCL_STUBS=1       \
  /FR"$(OBJDIR)\\"         \
  /Fo"$(OBJDIR)\\"         \
  /Fd"$(OBJDIR)\\"

COMMON_C_INCLUDES= \
  /I$(SRCDIR) \
  /I$(YAZINCL) \
  /I$(TCLINCL) \

DEBUG_C_OPTIONS=  \
  /D "_DEBUG"      \
  /MDd  /Od /YX /Zi /Gm

RELEASE_C_OPTIONS=  \
  /D "NDEBUG"        \
  /MD /O2

# /W3  = warning level
# /GX  = Enable exception handling
# /FD  = Generate file dependencies (what ever they are)
# /c   = compile without linking
# /FR  = Generate browse info (.sbr file that gets combined into .bsc)
# /Fo  = object file name (or at least path)
# /Fd  = debug database name (or path)
# /MD  = Runtime library: Multithread DLL
# /MDd = Runtime library: Multithread DLL (debug)
# /Od  = Disable optimising (debug)
# /O2  = Optimize for speed
# /YX  = Automatic use of precomipled headers
# /Gm  = Minimal rebuild (some cpp class stuff)
# /Zi  = Program database for debuggers
# /ZI  = Pgm database with special "edit&continue" stuff - not available in C5


### Linker options
LINK=link.exe

LINK_LIBS= kernel32.lib user32.lib   gdi32.lib   winspool.lib \
           comdlg32.lib advapi32.lib shell32.lib ole32.lib    \
           oleaut32.lib uuid.lib     odbc32.lib  odbccp32.lib \
           wsock32.lib  advapi32.lib

COMMON_LNK_OPTIONS= \
	/nologo \
	/machine:i386 \
	/incremental:no

DEBUG_LNK_OPTIONS= /debug 

RELEASE_LNK_OPTIONS=  /pdb:none

IRTCL_LINK_OPTIONS = /dll 

# Final opt variables
!if $(DEBUG)
COPT=   $(COMMON_C_OPTIONS)   $(DEBUG_C_OPTIONS)     $(COMMON_C_INCLUDES)
MTLOPT= $(COMMON_MTL_OPTIONS) $(DEBUG_MTL_OPTIONS)
RCOPT=  $(COMMON_RC_OPTIONS)  $(DEBUG_RC_OPTIONS)
LNKOPT= $(COMMON_LNK_OPTIONS) $(DEBUG_LNK_OPTIONS)   $(LNK_LIBS)

!else
COPT=   $(COMMON_C_OPTIONS)   $(RELEASE_C_OPTIONS)   $(COMMON_C_INCLUDES) 
MTLOPT= $(COMMON_MTL_OPTIONS) $(RELEASE_MTL_OPTIONS)
RCOPT=  $(COMMON_RC_OPTIONS)  $(RELEASE_RC_OPTIONS)
LNKOPT= $(COMMON_LNK_OPTIONS) $(RELEASE_LNK_OPTIONS) $(LNK_LIBS)
!endif

###########################################################
###############  Source and object modules
###########################################################

# Note: Ordinary source files are not specified here at 
# all, make finds them in suitable dirs. The object modules
# need to be specified, though

IRTCL_OBJS = \
	$(OBJDIR)\ir-tcl.obj \
	$(OBJDIR)\grs.obj \
	$(OBJDIR)\explain.obj \
	$(OBJDIR)\marc.obj \
	$(OBJDIR)\mem.obj \
	$(OBJDIR)\queue.obj \
	$(OBJDIR)\select.obj

ALL_OBJS=$(IRTCL_OBJS)

###########################################################
############### Compiling 
###########################################################

# Note: This defines where to look for the necessary
# source files. Funny way of doing it, but it works.

{$(SRCDIR)}.cpp{$(OBJDIR)}.obj:
	@$(CPP) $(COPT) $<

{$(SRCDIR)}.c{$(OBJDIR)}.obj:
	@$(CPP) $(COPT) $<

###########################################################
############### Resources
###########################################################

### The RC compiler (resource files)
RSC=rc.exe
COMMON_RC_OPTIONS= /l 0x406 /i"$(ROOTDIR)" 
DEBUG_RC_OPTIONS=/d "_DEBUG"
RELEASE_RC_OPTIONS=/d "NDEBUG"

RES=$(OBJDIR)\irtcl.res
RC=$(WINDIR)\irtcl.rc

!if $(DEBUG)
RSOPT=/d_DEBUG
!else
RSOPT=/d_NDEBUG
!endif

$(RES): $(RC)
	$(RSC) $(RSOPT) /fo"$(RES)" $(RC) 

###########################################################
############### Linking
###########################################################

$(IRTCLDLL) : "$(BINDIR)" $(IRTCL_OBJS) $(RES)
	@echo Linking irtcl DLL $(IRTCLDLL)
	$(LINK) $(IRTCL_LINK_OPTIONS) @<<
		/nologo
		$(IRTCL_OBJS) 
		$(RES)
		/out:$(IRTCLDLL) 
		$(YAZLIB)
		$(TCLLIB)
<<


###########################################################
############### Special operations
###########################################################


############## clean
clean:
	-del $(OBJDIR)\*.obj
	-del $(OBJDIR)\*.sbr
	-del $(TMPDIR)\*.
	-del $(IRTCLDLL)

# Because DOS del will only accept one file name to delete,
# the _H_ files work only on sets that have just one file.
# Z3950_H_FILES had to be spelled out. One more point for MS!

########### check directories and create if needed
dirs: $(OBJDIR) $(WINDIR) $(LIBDIR) $(BINDIR) $(TMPDIR)

$(OBJDIR) $(WINDIR) $(LIBDIR) $(BINDIR) $(TMPDIR):
 	if not exist "$@/$(NUL)" mkdir "$@"

###########################################################
############### Explicit dependencies
###########################################################

$(ALL_OBJS): makefile

# force recompilation of everything, if makefile changed

###########################################################
############### Log
###########################################################
#
# $Log: makefile,v $
# Revision 1.4  2003-03-05 22:06:32  adam
# TclStubs on WIN32
#
# Revision 1.3  2003/01/30 13:27:07  adam
# Changed version to 1.4.1. Added WIN32 version resource.
# IrTcl ignores unexpected PDU's, rather than die.
#
# Revision 1.2  2001/12/03 00:31:06  adam
# Towards 1.4. Configure updates.
#
# Revision 1.1  1999/09/10 10:02:29  adam
# Added MS NMAKE files - removed project files.
#

