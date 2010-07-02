
# Be sure to modify the definitions in this file to agree with your
# systems installation.
#  NOTE: be sure that the install directories use '\' not '/' for paths.


# MSVC install directories
LIBINSTALL     = \msdev\lib
INCLUDEINSTALL = \msdev\include\GL

# Win95 dll directory
DLLINSTALL     = \windows\system

# WinNT dll directory
#DLLINSTALL     = \winnt\system32

# Microsoft OpenGL libraries
#
GLU    = glu32.lib
OPENGL = opengl32.lib
GLUT   = $(TOP)/lib/glut/glut32.lib
GLUTLIB = glut32.lib
GLUTDLL = glut32.dll

# SGI OpenGL for Windows libraries (formerly Cosmo OpenGL)
# >> To use, uncomment lines below and comment out the similiar
# >> lines above.  You can download SGI OpenGL for Windows for
# >> free from http://www.meer.net/~gold/OpenGL/opengl2.exe
#
#GLU     = \oglsdk\lib\glu.lib
#OPENGL  = \oglsdk\lib\opengl.lib
#GLUT    = $(TOP)/lib/glut/glut.lib
#GLUTLIB = glut.lib
#GLUTDLL = glut.dll

# The Micro UI lib
MUI     = $(TOP)/lib/mui/mui.lib

# The OpenGL Extrusion and Tubing lib
GLE     = $(TOP)/lib/gle/gle.lib

# common definitions used by all makefiles
CFLAGS	= $(cflags) $(cdebug) $(EXTRACFLAGS) -DWIN32 -I$(TOP)/include
LIBS	= $(lflags) $(ldebug) $(EXTRALIBS) $(GLUT) $(GLU) $(OPENGL) $(guilibs)
EXES	= $(SRCS:.c=.exe)

# default rule
default	: $(EXES)


# cleanup rules
clean	:
	@del *.obj
	@del *.pdb
	@del *.ilk
	@del *.ncb
	@del *~
	@del *.exp

clobber	: clean
	@del *.exe
	@del *.dll
	@del *.lib

# inference rules
$(EXES)	: $*.obj
        $(link) -out:$@ $** $(LIBS)
.c.obj	: 
	$(CC) $(CFLAGS) $<
