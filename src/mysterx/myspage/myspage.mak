# myspage.mak

CPP=cl.exe
CPP_FLAGS=/I"F:/SBN/Include" /I"../../mzscheme/include" /MT /W3 /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /c

.cxx.obj::
   $(CPP) $(CPP_FLAGS) $<

MTL=midl.exe
MTL_SWITCHES=/tlb ".\myspage.tlb" /h "myspage.h" /iid "myspage_i.c" /Oicf 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"myspage.res"
REGSVR32=regsvr32	

LINK32=link.exe
LINK32_FLAGS=d:\plt\collects\mzscheme\lib\win32\i386\msvc\mzdyn.obj kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib f:\SBN\Lib\shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no /machine:I386 /def:myspage.def /out:myspage.dll
DEF_FILE= myspage.def
LINK32_OBJS= dhtmlpage.obj event.obj eventqueue.obj myspage.obj stdafx.obj \
	myspage.res

all : myspage.tlb myspage.dll myspage.h myspage_i.c 

myspage.dll : $(DEF_FILE) $(LINK32_OBJS) 
	$(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS) 
	copy myspage.dll ..\..\..\collects\mysterx\compiled\native
	$(REGSVR32) /s ..\..\..\collects\mysterx\compiled\native\myspage.dll

clean :
	-@erase DHTMLPage.obj
	-@erase Event.obj
	-@erase EventQueue.obj
	-@erase myspage.obj
	-@erase myspage.res
	-@erase StdAfx.obj
	-@erase myspage.dll
	-@erase myspage.exp
	-@erase myspage.ilk
	-@erase myspage.lib
	-@erase myspage.h
	-@erase myspage.tlb
	-@erase myspage_i.c

.cxx{$(INTDIR)}.obj::
   $(CPP) $(CPP_FLAGS) $< 

dhtmlpage.obj : dhtmlpage.cxx dhtmlpage.h dhtmlpageui.htm stdafx.h

event.obj : event.cxx event.h stdafx.h

eventqueue.obj : eventqueue.cxx eventqueue.h stdafx.h

myspage.obj : myspage.cxx myspage.h stdafx.h

stdafx.obj : stdafx.cxx stdafx.h

myspage.res : myspage.rc myspage.tlb
	$(RSC) $(RSC_PROJ) myspage.rc

myspage.tlb myspage.h myspage_i.c : myspage.idl
	$(MTL) $(MTL_SWITCHES) myspage.idl



