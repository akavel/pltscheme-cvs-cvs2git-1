/*								-*- C++ -*-
 * $Id: AppMain.h,v 1.1 1997/12/22 17:28:45 mflatt Exp $
 *
 * Purpose: wxWindows application and main loop
 *
 * Authors: Markus Holzem and Julian Smart
 *
 * Copyright: (C) 1995, AIAI, University of Edinburgh (Julian)
 * Copyright: (C) 1995, GNU (Markus)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef AppMain_h
#define AppMain_h

#ifdef __GNUG__
#pragma interface
#endif

class wxFrame;

class wxApp : public wxObject {
DECLARE_DYNAMIC_CLASS(wxApp)
public:
    // functions
    wxApp(wxlanguage_t language = wxLANGUAGE_DEFAULT);

    virtual void    Dispatch(void);
            Bool    Initialized(void) { return initialized; };
    virtual int     MainLoop(void);
    virtual void    ExitMainLoop(void) { keep_going = False; }
    virtual wxFrame *OnInit(void) { return 0; };
    virtual int     OnExit(void) { return 0; };
    virtual Bool    Pending(void);

    char* GetAppName(void)	   { return wxAPP_NAME; }
    void  SetAppName(char *name)   { wxAPP_NAME = name; }
    char* GetClassName(void)	   { return wxAPP_CLASS; }
    void  SetClassName(char *name) { wxAPP_CLASS = name; }
    // data
    int    argc;
    char** argv;
    static Bool&  wantDebugOutput;
    static char*& wx_class;
    static char*& appName;
private:
    friend int wxEntry(int argc, char **argv);

    Bool  initialized;
public:
    Bool  keep_going;
};

void wxInitNewToplevel(void);
extern void wxDoNextEvent();
extern int wxEventReady();
extern void wxDoEvents();

// Here's a macro you can use if your compiler
// really, really wants main() to be in your main program
#if defined(AIX) || defined(AIX4)
#define IMPLEMENT_WXWIN_MAIN \
     int main(int argc, char *argv[]) { return wxEntry(argc, argv); }
#else
#define IMPLEMENT_WXWIN_MAIN
#endif

#endif // AppMain_h
