# srpersist.mak

all : srpersist.dll

clean :
	-@erase srpersist.obj
	-@erase srptypes.obj
	-@erase srpbuffer.obj
	-@erase srpersist.dll

CPP=cl.exe
CPP_FLAGS=/I"../mzscheme/include" /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /c

.cxx.obj::
   $(CPP) $(CPP_FLAGS) $< 

MZC="D:\PLT\mzc"
	
LINK32=$(MZC)
LINK32_FLAGS=
LINK32_LIBS= \
	kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib \
	advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib \
	odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib \
	comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib \
	uuid.lib odbc32.lib odbccp32.lib 
LINK32_OBJS= \
	srpersist.obj srptypes.obj srpbuffer.obj

srpersist.dll : $(DEF_FILE) $(LINK32_OBJS)
	$(LINK32) $(LINK32_FLAGS) --ld srpersist.dll $(LINK32_OBJS) $(LINK32_LIBS)
	copy srpersist.dll ..\..\collects\srpersist\compiled\native

srpersist.obj : srpersist.cxx srpersist.h srptypes.h srpprims.tbl srpconsts.tbl srpinfo.tbl srpstructs.tbl

srptypes.obj : srptypes.cxx srpersist.h srptypes.h

srpbuffer.obj : srpbuffer.cxx srpersist.h srpbuffer.h

