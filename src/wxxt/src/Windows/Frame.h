/*								-*- C++ -*-
 * $Id: Frame.h,v 1.2 1998/08/18 20:03:40 mflatt Exp $
 *
 * Purpose: base class for all frames
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

#ifndef Frame_h
#define Frame_h

#ifdef __GNUG__
#pragma interface
#endif

class wxIcon;
class wxMenuBar;
class wxMessage;
class wxToolBar;

class wxFrame : public wxPanel {
DECLARE_DYNAMIC_CLASS(wxFrame)
public:
    wxFrame(void);
    wxFrame(wxFrame *parent, char *title, int x=-1, int y=-1,
	    int width=-1, int height=-1, int style=wxDEFAULT_FRAME,
	    char *name="frame");
    ~wxFrame(void);
    // frame creation
    Bool  Create(wxFrame *parent, char *title, int x=-1, int y=-1,
		 int width=-1, int height=-1, int style=wxDEFAULT_FRAME,
		 char *name="frame");
    // leave place for menubar and statusline
    virtual void  Fit(void);
    virtual void  Layout(void);
    virtual void  GetClientSize(int *width, int *height);
    virtual void  SetClientSize(int width, int height);
    virtual void  SetSize(int width, int height)
		{ Configure(-1, -1, width, height); }
    virtual void  SetSize(int x, int y, int width, int height,
			  int WXUNUSED(flags=wxSIZE_AUTO))
		{ Configure(x, y, width, height); }
    // status line
    /* MATTHEW: */
    void  CreateStatusLine(int number=1, char *name = "status_line");
    void  SetStatusText(char *text, int number=0);
    Bool  StatusLineExists(void);
    // change and query state of frame
    void  Iconize(Bool iconize);
    Bool  Iconized(void);
    void  Maximize(Bool maximize);
    // associated GDI objects
    wxMenuBar *GetMenuBar(void);
    void      SetIcon(wxIcon *icon);
    void      SetMenuBar(wxMenuBar *menubar);
    // miscellaneous
    void  Command(int id);
    void  LoadAccelerators(char *WXUNUSED(resource)) {}
    Bool  Show(Bool show);
    // virtual event functions
    virtual void  OnMenuSelect(long id);

    void SetToolBar(wxToolBar*);
    wxToolBar *GetToolBar(void);

    /* MATTHEW */
    void *context;

protected:
    wxMenuBar  *menubar;
    wxMessage  **status;
    int        num_status;
    Bool       other; /* This field is unused */
    Bool       being_destroyed;
};

#endif // Frame_h
