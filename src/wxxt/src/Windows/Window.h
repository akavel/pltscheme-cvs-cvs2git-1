/*								-*- C++ -*-
 * $Id: Window.h,v 1.6 1998/10/16 15:55:56 mflatt Exp $
 *
 * Purpose: base class for all windows
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

#ifndef Window_h
#define Window_h

#ifdef __GNUG__
#pragma interface
#endif

#ifdef Have_Xt_Types
class wxWindow_Xintern {
public:
    Widget	  frame;			// frame widgets
    Widget	  scroll;			// optional scrollable widget
    Widget	  handle;			// X implementation of functionality
    Region	  expose_region;		// exposed region
    XEvent*	  expose_event;			// expose event 
    EventMask	  translations_eventmask;	// events selected by widget-translations
    unsigned long last_clicktime; 		// last time and button to compute
    unsigned int  last_clickbutton;		//   if a double click has arrived
};
#else
class wxWindow_Xintern;
#endif

class wxBitmap;
class wxColour;
class wxColourMap;
class wxCommandEvent;
class wxCursor;
class wxFont;
class wxKeyEvent;
class wxLayoutConstraints;
class wxList;
class wxMenu;
class wxMouseEvent;
class wxPanel;

class wxWindow : public wxEvtHandler {
DECLARE_DYNAMIC_CLASS(wxWindow)
public:
    wxWindow(void);
    ~wxWindow(void);

    // access to X private data
    wxWindow_Xintern *GetHandle(void) { return X; }
    // child - parent relationships
    wxChildList *GetChildren(void) { return children; }
    wxWindow  *GetGrandParent(void);
    wxWindow  *GetParent(void) { return parent; }
    void      AddChild(wxWindow *child);
    void      DestroyChildren(void);
    void      RemoveChild(wxWindow *child);
    // label, name, title of wxWindow
    virtual char  *GetLabel(void);
    virtual char  *GetName(void);
    virtual char  *GetTitle(void);
    virtual void  SetLabel(char *label);
    virtual void  SetName(char *name);
    virtual void  SetTitle(char *title);
    // set & query size and position of wxWindow
    virtual void  Centre(int direction = wxBOTH);
    virtual void  ClientToScreen(int *x, int *y);
    virtual void  Configure(int x, int y, int width, int height);
    virtual void  GetClientSize(int *width, int *height);
    virtual void  GetPosition(int *x, int *y);
    virtual void  GetSize(int *width, int *height);
    virtual void  Move(int x, int y)
	{ SetSize(x, y, -1, -1, wxSIZE_USE_EXISTING); }
    virtual void  ScreenToClient(int *x, int *y);
    virtual void  SetClientSize(int width, int height)
	{ SetSize(width, height); }
    virtual void  SetSize(int x, int y, int width, int height, int flags=wxSIZE_AUTO);
    virtual void  SetSize(int width, int height)
	{ SetSize(-1, -1, width, height, wxSIZE_USE_EXISTING); }
    // GDI (colours, colourmap, font, cursor)
    virtual void      ChangeColours(void);
    virtual wxColour  *GetBackgroundColour(void) { return bg; }
    virtual wxColour  *GetForegroundColour(void) { return fg; }
    virtual void      GetTextExtent(const char *string, float *x, float *y,
				    float *descent = NULL,
				    float *externalLeading = NULL,
				    wxFont *theFont = NULL, Bool use16bit=FALSE);
    virtual void      SetBackgroundColour(wxColour *col);
    virtual wxCursor  *SetCursor(wxCursor *cursor);
    virtual void      SetForegroundColour(wxColour *col);
    // Caret
    virtual void  CreateCaret(int WXUNUSED(w), int WXUNUSED(h)) {};
    virtual void  CreateCaret(wxBitmap *WXUNUSED(bitmap)) {};
    virtual void  DestroyCaret(void) {};
    virtual void  ShowCaret(Bool WXUNUSED(show)) {};
    virtual void  SetCaretPos(int WXUNUSED(x), int WXUNUSED(y)) {};
    virtual void  GetCaretPos(int *WXUNUSED(x), int *WXUNUSED(y)) {};
    // Scrolling
    virtual void  EnableScrolling(Bool x, Bool y);
    virtual int   GetScrollPos(int orient);
    virtual int   GetScrollRange(int orient);
    virtual int   GetScrollPage(int orient); /* MATTHEW */
    virtual void  Scroll(int x_pos, int y_pos);
    virtual void  SetScrollArea(int hsize, int vsize);
    virtual void  SetScrollPos(int orient, int pos);
    virtual void  SetScrollRange(int orient, int range);
    virtual void  SetScrollPage(int orient, int range); /* MATTHEW */
    // layout
    virtual wxLayoutConstraints *GetConstraints(void)
        { return constraints; }
    virtual void SetConstraints(wxLayoutConstraints *constr);
    virtual void Layout(void);
    // miscellaneous
    virtual void  AllowDoubleClick(Bool allow) { allow_dclicks = allow; }
    virtual void  CaptureMouse(void);
    virtual void  DragAcceptFiles(Bool accept) { drag_accept = accept; }
    virtual void  Enable(Bool enable);
    virtual void  EnablePainting(Bool enable) { painting_enabled = enable; }
    virtual void  Fit(void) {}
    virtual Bool  GetUserEditMode(void) { return user_edit_mode; }
    virtual long  GetWindowStyleFlag(void) { return style; }
    virtual void  Paint(void) { OnPaint(); }
    virtual Bool  PopupMenu(wxMenu *menu, float x, float y);
    virtual void  Refresh(void);
    virtual void  ReleaseMouse(void);
    virtual void  SetFocus(void);
    virtual void  SetUserEditMode(Bool edit) { user_edit_mode = edit; }
    virtual Bool  Show(Bool show);
    // event handling
    virtual wxEvtHandler *GetEventHandler(void)
	{ return event_handler; }
    virtual void SetEventHandler(wxEvtHandler *handler)
	{ event_handler = handler; }
    virtual void OnChar(wxKeyEvent& event);
    virtual void OnCommand(wxWindow& win, wxCommandEvent& event);
    virtual void OnEvent(wxMouseEvent& event);
    virtual void OnPaint(void);
    virtual void OnScroll(wxScrollEvent& event);
    virtual Bool PreOnChar(wxWindow *, wxKeyEvent *);
    virtual Bool PreOnEvent(wxWindow *, wxMouseEvent *);
    // get the associated device context
    wxWindowDC* GetDC(void);

    /* MATTHEW */
    void MakeModal(int on);
    void InternalEnable(Bool enable, Bool gray = FALSE);
    virtual void ChangeToGray(Bool gray);
    Bool IsGray(void);

    Bool IsShown(void);
    void SetShown(Bool shown);

    void      ForEach(void (*foreach)(wxWindow *w, void *data), void *data);

    Bool GetsFocus(void) { return TRUE; }

protected:
    // create and destroy associated device context
    void CreateDC(void);
    void DestroyDC(void);
    // applies event handlers to window
    void AddEventHandlers(void);
    Bool CallPreOnChar(wxWindow *, wxKeyEvent *);
    Bool CallPreOnEvent(wxWindow *, wxMouseEvent *);
#   ifdef Have_Xt_Types
    // event handlers to care for incoming events
    static void ExposeEventHandler(Widget w, wxWindow **win, XtPointer p_XfwfExposeInfo);
    static void FrameEventHandler(Widget w,   wxWindow **win,
				  XEvent *ev, Boolean *continue_to_dispatch_return);
    static void WindowEventHandler(Widget w,   wxWindow **win,
				   XEvent *ev, Boolean *continue_to_dispatch_return);
    static void ScrollEventHandler(Widget w, wxWindow **win, XtPointer p_XfwfScrollInfo);
#   endif
protected:
    friend void wxXSetBusyCursor(wxWindow *, wxCursor *); /* MATTHEW */
    friend void wxXSetNoCursor(wxWindow *, wxCursor *); /* MATTHEW */
    // X representation
    wxWindow_Xintern *X;
    // device context
    wxWindowDC *dc;
    // child <-> parent relationsship
    wxWindow *parent;
    wxChildList   *children;
    // GDI objects
    wxColour    *fg, *bg;
    wxColourMap *cmap;
    wxCursor    *cursor;
    wxFont      *font;
    // event handler (usually this)
    wxEvtHandler  *event_handler;
    // layout information
    wxLayoutConstraints  *constraints;
    int                  xoff, yoff;
    // misc info
    Bool  allow_dclicks;
    Bool  captured;
    Bool  drag_accept;
    Bool  painting_enabled;
    Bool  user_edit_mode;
    long  style;

    wxWindow **saferef; /* MATTHEW: indirection for safety in callbacks */

    long misc_flags; /* MATTHEW */

    short internal_disabled;  /* MATTHEW */
    short internal_gray_disabled;  /* MATTHEW */

    /* For scrolling with explicit control: */ 
    long hs_pos, vs_pos, hs_page, vs_page, hs_width, vs_width;


    static void FocusChangeCallback(void *, wxWindow **winp, void *on);
};

#endif // Window_h
