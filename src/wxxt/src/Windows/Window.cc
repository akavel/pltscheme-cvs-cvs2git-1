/*								-*- C++ -*-
 * $Id: Window.cc,v 1.32 1999/11/10 03:46:20 mflatt Exp $
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

#ifdef __GNUG__
#pragma implementation "Window.h"
#endif

#define  Uses_XtIntrinsicP
#define  Uses_wxGDI
#define  Uses_wxLayout
#define  Uses_wxMenu
#define  Uses_wxMenuBar
#define  Uses_wxTypeTree
#define  Uses_wxWindow
#define  Uses_wxDialogBox
#define  Uses_wxItem
#include "wx.h"
#define  Uses_ScrollWinWidget
#define  Uses_ShellWidget
#define  Uses_SimpleWidget
#define  Uses_EnforcerWidget
#define  Uses_LabelWidget
#define  Uses_MultiListWidget
#include "widgets.h"

#include <X11/keysym.h> // needed for IsFunctionKey, etc.

#define MALLOC_SAFEREF(s) malloc(s)
#define FREE_SAFEREF(x) free(x)

extern void wxSetSensitive(Widget, Bool enabled);

//-----------------------------------------------------------------------------
// wxWindow constructor
//-----------------------------------------------------------------------------

#define ACTIVE_VIA_POINTER_FLAG 0x1
#define DISABLED_FLAG 0x2
#define SHOWN_FLAG 0x4
#define NO_AUTO_SCROLL_FLAG 0x8
#define FOCUS_FLAG 0x10
#define REPORT_ZERO_WIDTH_FLAG 0x20
#define REPORT_ZERO_HEIGHT_FLAG 0x40
#define LAST_WAS_ALT_DOWN_FLAG 0x80

wxWindow::wxWindow(void)
{ 
    __type = wxTYPE_WINDOW;

    // X representation
    X = new wxWindow_Xintern;
    X->frame = X->handle = X->scroll = NULL;
    X->translations_eventmask = 0;
    X->last_clicktime = 0;
    X->last_clickbutton = 0;
    // device context
    dc = NULL;
    // child <-> parent relationships
    parent   = NULL;
    children = DEBUG_NEW wxChildList;
    // event handler
    event_handler = this;
    // layout information
    constraints = DEBUG_NEW wxLayoutConstraints;
    constraints->left.Absolute(0);
    constraints->top.Absolute(0);
    constraints->width.AsIs();
    constraints->height.AsIs();
    xoff = yoff = 0;
    // GDI objects
    fg     = wxBLACK;
    bg     = wxGREY;
    cmap   = wxAPP_COLOURMAP;
    cursor = NULL /* wxSTANDARD_CURSOR */;
    font   = wxSYSTEM_FONT;
    // misc info
    allow_dclicks    = FALSE;
    captured         = FALSE;
    drag_accept      = FALSE;
    painting_enabled = TRUE;
    style            = 0;
    user_edit_mode   = FALSE;

    /* MATTHEW */
    saferef = (wxWindow **)MALLOC_SAFEREF(sizeof(wxWindow *));
    *saferef = this;
    misc_flags = 0;
    /* except for frames, windows start out shown: */
    if (!wxSubType(__type, wxTYPE_FRAME))
      misc_flags |= SHOWN_FLAG;
    internal_disabled = 0;

    WXGC_IGNORE(parent);
}

wxWindow::~wxWindow(void)
{
    // destroy children
    DestroyChildren(); delete children; children = NULL;
    // destroy device context
    if (dc) delete dc; dc = NULL;
    // remove from parents list
    if (parent)	parent->RemoveChild(this); parent = NULL;
    // destroy widgets
    wxSetSensitive(X->frame, TRUE);

    *saferef = NULL; /* MATTHEW */

    if (X->frame) XtDestroyWidget(X->frame); X->frame = X->handle = X->scroll = NULL;
    // delete contraints
    delete constraints; constraints = NULL;
    // delete X internal data
    delete X; X = NULL;
}

//-----------------------------------------------------------------------------
// child - parent relationships
//-----------------------------------------------------------------------------

wxWindow *wxWindow::GetGrandParent(void)
{
    return (parent ? parent->parent : (wxWindow*)NULL);
}

void wxWindow::AddChild(wxWindow *child)
{
  /* Propagate busy cursor flag */
  child->user_edit_mode = user_edit_mode;

  children->Append(child);
}

void wxWindow::DestroyChildren(void)
{
  wxChildNode *node;
  while ( (node=children->First()) != NULL ) {
    wxWindow *child;
    child = (wxWindow*)(node->Data());
    if (child) {
      delete child;
    }
  }
}

void wxWindow::RemoveChild(wxWindow *child)
{
    children->DeleteObject(child);
}

//-----------------------------------------------------------------------------
// label, name, title of wxWindow
//-----------------------------------------------------------------------------

char *wxWindow::GetLabel(void)
{
  char *label = NULL;

  if (!X->frame) // forbid, if no widget associated
    return NULL;
  
  XtVaGetValues(X->frame, XtNlabel, &label, NULL);
  return label;
}

char *wxWindow::GetName(void)
{
    if (!X->handle) // forbid, if no widget associated
	return NULL;

    return XtName(X->handle);
}

char *wxWindow::GetTitle(void)
{
  char *title = NULL;
  
  if (!X->frame) // forbid, if no widget associated
    return NULL;
  
  XtVaGetValues(X->frame, XtNtitle, &title, NULL);
  return title;
}

void wxWindow::SetLabel(char *label)
{
  char *oldlabel = NULL;

  if (!X->frame) // forbid, if no widget associated
    return;

  XtVaGetValues(X->frame, XtNlabel, &oldlabel, NULL);
  if (oldlabel) {
    label = wxGetCtlLabel(label);
    XtVaSetValues(X->frame, XtNlabel, label, NULL);
  }
}

void wxWindow::SetName(char *name)
{
  // overwrite quark computed on widget creation
  XrmQuark q;
  q = XrmStringToName((name != NULL) ? name : "");
  X->handle->core.xrm_name = q;
}

void wxWindow::SetTitle(char *title)
{
    if (!X->frame) // forbid, if no widget associated
	return;

    XtVaSetValues(X->frame, XtNtitle, title, XtNiconName, title, NULL);
}

//-----------------------------------------------------------------------------
// set & query size and position of wxWindow
//-----------------------------------------------------------------------------

void wxWindow::Centre(int direction)
{
    int x=-1,       y=-1,       width=0,      height=0,
	parent_x=0, parent_y=0, parent_width, parent_height;

    // get position and width of parent
    if (parent) {
	if ( wxSubType(__type, wxTYPE_FRAME) )
	    parent->GetPosition(&parent_x, &parent_y);
	parent->GetClientSize(&parent_width, &parent_height);
    } else {
	wxDisplaySize(&parent_width, &parent_height);
    }
    // get position and size of THIS window
    GetPosition(&x, &y); GetSize(&width, &height);

    if (direction & wxCENTRE_TOPLEFT) {
	x = parent_x + parent_width / 2;
	y = parent_y + parent_height / 2;
    } else {
	// compute centered position
	if (direction & wxHORIZONTAL) {
	    x = parent_x + (parent_width - width) / 2;
	    if (x < 0)
	      x = 0;
	}
	if (direction & wxVERTICAL) {
	    y = parent_y + (parent_height - height) / 2;
	    if (y < 0)
	      y = 0;
       }
    }
    // move window
    Move(x, y);
}

void wxWindow::ClientToScreen(int *x, int *y)
{
  if (!X->handle) // forbid, if no widget associated
    return;

  {
    Display *dpy  = XtDisplay(X->handle);
    Screen  *scn  = XtScreen(X->handle);
    Window  root  = RootWindowOfScreen(scn);
    Window  win   = XtWindow(X->handle);
    Window  child;
    int xx = *x;
    int yy = *y;
    XTranslateCoordinates(dpy, win, root, xx, yy, x, y, &child);
  }
}

void wxWindow::Configure(int x, int y, int width, int height)
{
    Arg    args[4];
    int    i = 0;
    int _xoff = 0, _yoff = 0;
    Position cx, cy;
    Dimension cw, ch;

    if (!X->frame) // forbid, if no widget associated
	return;

    if (!wxSubType(__type, wxTYPE_FRAME) && parent) {
	_xoff = parent->xoff;
	_yoff = parent->yoff;
    }

    /* MATTHEW: [5] Make sure width, height != 0 */
    if (!width) {
      width = 1;
      misc_flags |= REPORT_ZERO_WIDTH_FLAG;
    } else
      misc_flags -= (misc_flags & REPORT_ZERO_WIDTH_FLAG);

    if (!height) {
      height = 1;
      misc_flags |= REPORT_ZERO_HEIGHT_FLAG;
    } else
      misc_flags -= (misc_flags & REPORT_ZERO_HEIGHT_FLAG);

    XtVaGetValues(X->frame, XtNx, &cx, XtNy, &cy, XtNwidth, &cw, XtNheight, &ch, NULL);

    if ((x > -1) && ((Position)(x + _xoff) != cx))
      { args[i].name = XtNx; args[i].value = (Position)(x+_xoff); ++i; }
    if ((y > -1) && ((Position)(y + _yoff) != cy))
      { args[i].name = XtNy; args[i].value = (Position)(y+_yoff); ++i; }
    if ((width > -1) && ((Dimension)width != cw))
      { args[i].name = XtNwidth; args[i].value = (Dimension)width; ++i; }
    if ((height > -1) && ((Dimension)height != ch))
      { args[i].name = XtNheight; args[i].value = (Dimension)height; ++i; }

    if (i) {
      XtSetValues(X->frame, args, i);
      GetEventHandler()->OnSize(width, height);
    }
}

void wxWindow::GetPosition(int *x, int *y)
{
    int _xoff = 0, _yoff = 0;
    Position xx, yy;

    if (!X->frame) // forbid, if no widget associated
	return;

    if (!wxSubType(__type, wxTYPE_FRAME) && parent) {
	_xoff = parent->xoff;
	_yoff = parent->yoff;
    }

    XtVaGetValues(X->frame, XtNx, &xx, XtNy, &yy, NULL);
    *x = (int)xx - _xoff; *y = (int)yy - _yoff;
}

void wxWindow::GetSize(int *width, int *height)
{
    Dimension ww, hh;

    if (!X->frame) // forbid, if no widget associated
	return;

    XtVaGetValues(X->frame, XtNwidth, &ww, XtNheight, &hh, NULL);
    *width = ww; *height = hh;

    if (misc_flags & REPORT_ZERO_WIDTH_FLAG)
      *width = 0;
    if (misc_flags & REPORT_ZERO_HEIGHT_FLAG)
      *height = 0;
}

/* MATTHEW: Client size is different from size */
void wxWindow::GetClientSize(int *width, int *height)
{
    Dimension dww, dhh, fw, fh;
    int ww, hh;

    if (!X->handle) // forbid, if no widget associated
	return;

    XtVaGetValues(X->handle, XtNwidth, &dww, XtNheight, &dhh, NULL);
    if (X->scroll && !(misc_flags & NO_AUTO_SCROLL_FLAG)) {
      /* Handle window is possibly bigger than actual visible area */
      Dimension xs, ys;
      xws_get_scroll_area(X->scroll, &xs, &ys);
      if (wxSubType(__type, wxTYPE_LIST_BOX)) {
	dww = xs;
	dhh = ys;
      } else {
	if (xs < dww)
	  dww = xs;
	if (ys < dhh)
	  dhh = ys;
      }
    }

    XtVaGetValues(X->frame, XtNwidth, &fw, XtNheight, &fh, NULL);

    /* If frame < contained, don't believe the number! */
    if (fw < dww)
      dww = 0;
    if (fh < dhh)
      dhh = 0;

    ww = (int)dww;
    hh = (int)dhh;

    if (XtIsSubclass(X->handle, xfwfFrameWidgetClass)) {
      Dimension fw, oo, io;

      XtVaGetValues(X->handle, 
		    XtNframeWidth, &fw, 
		    XtNouterOffset, &oo, 
		    XtNinnerOffset, &io, 
		    NULL);

      ww = ww - fw - oo - io;
      hh = hh - fw - oo - io;
    }

    *width = ww; *height = hh;
}

void wxWindow::ScreenToClient(int *x, int *y)
{
  if (!X->handle) // forbid, if no widget associated
    return;
    
  {
    Display *dpy  = XtDisplay(X->handle);
    Screen  *scn  = XtScreen(X->handle);
    Window  root  = RootWindowOfScreen(scn);
    Window  win   = XtWindow(X->handle);
    Window  child;
    int xx = *x;
    int yy = *y;
    XTranslateCoordinates(dpy, root, win, xx, yy, x, y, &child);
  }
}

void wxWindow::SetSize(int x, int y, int width, int height, int WXUNUSED(flags))
{
    if (x > -1)		constraints->left.Absolute(x);
    if (y > -1)		constraints->top.Absolute(y);
    if (width > -1)	constraints->width.Absolute(width);
    else		constraints->width.AsIs();
    if (height > -1)	constraints->height.Absolute(height);
    else		constraints->height.AsIs();

    Configure(x, y, width, height);
}

//-----------------------------------------------------------------------------
// GDI objects (colours, colourmap, font, cursor)
//-----------------------------------------------------------------------------

void wxWindow::ChangeColours(void)
{
  if (X->frame) {
    if (bg)
      XtVaSetValues(X->frame, XtNbackground,
		    bg->GetPixel(cmap), NULL);
    if (fg)
      XtVaSetValues(X->frame, XtNforeground,
		    fg->GetPixel(cmap), NULL);
  }
  if (X->handle) {
    if (bg)
      XtVaSetValues(X->handle, XtNbackground,
		    bg->GetPixel(cmap), NULL);
    if (fg)
      XtVaSetValues(X->handle, XtNforeground,
		    fg->GetPixel(cmap), NULL);
  }
}

// merged with DC-method: float wxWindow::GetCharHeight(void)
// merged with DC-method: float wxWindow::GetCharWidth(void)
// merged with DC-method: wxFont *GetFont(void)
// merged with DC-method: void wxWindow::GetTextExtent()

void wxWindow::SetBackgroundColour(wxColour *col)
{
    bg = col; ChangeColours();
}

// merged with DC-method: void wxWindow::SetColourMap(wxColourMap *new_cmap)

wxCursor *wxWindow::SetCursor(wxCursor *new_cursor)
{
  if (!X->handle) // forbid, if no widget associated
    return NULL;
  
  wxCursor *previous = cursor;
  
  if (!new_cursor || (new_cursor && new_cursor->Ok())) {
    cursor = new_cursor;
    if (!user_edit_mode) { /* really indicates busy_cursor */
      XtVaSetValues(X->handle, XtNcursor, new_cursor ? GETCURSOR(new_cursor) : None, NULL);
      if (__type == wxTYPE_LIST_BOX) {
	/* Yuck. Set cursor for total client area of listbox */
	XtVaSetValues(XtParent(X->handle), XtNcursor, new_cursor ? GETCURSOR(new_cursor) : None, NULL);
      }
    }
  }
  
  return previous;
}

// merged with DC-method: void wxWindow::SetFont(wxFont *new_font)

void wxWindow::SetForegroundColour(wxColour *col)
{
    fg = col; ChangeColours();
}

//-----------------------------------------------------------------------------
// layout
//-----------------------------------------------------------------------------

void wxWindow::SetConstraints(wxLayoutConstraints *constr)
{
    if (constr) {
	delete constraints;
	constraints = constr;
    }
}

// void wxWindow::Layout(void)
// --> wxLayout.cc

//-----------------------------------------------------------------------------
// Scrolling
//-----------------------------------------------------------------------------

void wxWindow::EnableScrolling(Bool x, Bool y)
{
    if (X->scroll)
	XtVaSetValues(X->scroll, XtNhideHScrollbar, !x, XtNhideVScrollbar, !y, NULL);
}

int wxWindow::GetScrollPos(int orient)
{
    if (!X->scroll) return 0; // window is not scrollable

    if (!(misc_flags & NO_AUTO_SCROLL_FLAG))
      return 0;
 
    if (misc_flags & NO_AUTO_SCROLL_FLAG) {
      return orient == wxHORIZONTAL ? hs_pos : vs_pos;
    } else {
      Position pos;
      XtVaGetValues(X->handle, orient == wxHORIZONTAL ? XtNx : XtNy, &pos, NULL);
      return -pos;
    }
}

int wxWindow::GetScrollRange(int orient)
{
    if (!X->scroll) return 0; // window is not scrollable

    if (!(misc_flags & NO_AUTO_SCROLL_FLAG))
      return 0;
 
    return orient == wxHORIZONTAL ? hs_width : vs_width;
}

int wxWindow::GetScrollPage(int orient)
{
  if (!X->scroll) return 0; // window is not scrollable
  
    if (!(misc_flags & NO_AUTO_SCROLL_FLAG))
      return 0;
 
  if (orient == wxHORIZONTAL) {
    if (!hs_width)
      return 0;
    return hs_page;
  } else {
    if (!vs_width)
      return 0;
    return vs_page;
  }
}

void wxWindow::Scroll(int x_pos, int y_pos)
{
    if (!X->scroll)
	return; // don't want to scroll or window not scrollable

    if (misc_flags & NO_AUTO_SCROLL_FLAG) {
      if (x_pos >= 0) {
	hs_pos = x_pos;
	if (hs_pos > hs_width)
	  hs_pos = hs_width;
      }
      if (y_pos >= 0) {
	vs_pos = y_pos;
	if (vs_pos > vs_width)
	  vs_pos = vs_width;
      }

      xws_set_scroll_direct(X->scroll, hs_width, hs_page, hs_pos, vs_width, vs_page, vs_pos);
    } else {
      Position dummy; int wd, ht; Dimension gwd, ght;
      // size of view port
      XfwfCallComputeInside(X->scroll, &dummy, &dummy, &wd, &ht);
      // size of scrollable window
      XtVaGetValues(X->handle, XtNheight, &ght, XtNwidth, &gwd, NULL);
      // get missing position if any (x_pos <0 || y_pos < 0)
      if (x_pos < 0) { XtVaGetValues(X->handle, XtNx, &dummy, NULL); x_pos = -dummy; }
      if (y_pos < 0) { XtVaGetValues(X->handle, XtNy, &dummy, NULL); y_pos = -dummy; }
      // compute correct (x,y)-position - 0<=x<=gwd-wd, 0<=y<=ght-ht
      x_pos = min(x_pos, gwd-wd); x_pos = max(0, x_pos);
      y_pos = min(y_pos, ght-ht); y_pos = max(0, y_pos); 
      // set position
      XtVaSetValues(X->handle, XtNx, (Position)(-x_pos), XtNy, (Position)(-y_pos), NULL);
    }
}

void wxWindow::SetScrollArea(int gwd, int ght)
{
    Dimension d;
    int wd, ht; 
    Position p, x, y;

    if ((gwd <= 0 && ght <= 0) || !X->scroll)
	return; // don't want to resize or window not scrollable

    // position of scrollable window
    XtVaGetValues(X->handle, XtNx, &x, XtNy, &y, NULL);
    // size of viewport
    XfwfCallComputeInside(X->scroll, &p, &p, &wd, &ht);
    // get missing sizes if any (gwd <0 || ght < 0)
    if (gwd < 0) { XtVaGetValues(X->handle, XtNwidth, &d, NULL);  gwd = d; }
    if (!gwd) gwd = 1; /* MATTHEW: [5] */
    if (ght < 0) { XtVaGetValues(X->handle, XtNheight, &d, NULL); ght = d; }
    if (!ght) ght = 1; /* MATTHEW: [5] */
    // compute correct (x,y)-position - 0<=x<=gwd-wd, 0<=y<=ght-ht
    x = min(x, gwd-wd); x = max(0, x);
    y = min(y, ght-ht); y = max(0, y); 
    // set size and reposition if necessary (x,y changed)
    XtVaSetValues(X->handle,
		  XtNx,     x,              XtNy,      y,
		  XtNwidth, (Dimension)gwd, XtNheight, (Dimension)ght,
		  NULL);
}

void wxWindow::SetScrollPos(int orient, int pos)
{
  if (!(misc_flags & NO_AUTO_SCROLL_FLAG))
    return;
  
  if (orient == wxHORIZONTAL)	wxWindow::Scroll(pos < 0 ? 0 : pos, -1);
  else			        wxWindow::Scroll(-1, pos < 0 ? 0 : pos);
}

void wxWindow::SetScrollRange(int orient, int range)
{
  if (!(misc_flags & NO_AUTO_SCROLL_FLAG))
    return;

  if (orient == wxHORIZONTAL) {
    hs_width = range;
    if (hs_pos > hs_width)
      hs_pos = hs_width;
  } else {
    vs_width = range;
    if (vs_pos > vs_width)
      vs_pos = vs_width;
  }
  
  xws_set_scroll_direct(X->scroll, hs_width, hs_page, hs_pos, vs_width, vs_page, vs_pos);
}

void wxWindow::SetScrollPage(int orient, int range)
{
  if (!(misc_flags & NO_AUTO_SCROLL_FLAG))
    return;
  
  if (range <= 0)
    range = 1;
  if (orient == wxHORIZONTAL) {
    if (!hs_width)
      hs_page = 1;
    else
      hs_page = range;
  } else {
    if (!vs_width)
      vs_page = 1;
    else
      vs_page = range;
  }

  xws_set_scroll_direct(X->scroll, hs_width, hs_page, hs_pos, vs_width, vs_page, vs_pos);
}

//-----------------------------------------------------------------------------
// miscellaneous
//-----------------------------------------------------------------------------

void wxWindow::CaptureMouse(void)
{
    if (!X->frame) // forbid, if no widget associated
	return;

    if (!captured) {
	XtAddGrab(X->frame, TRUE, FALSE);
	captured = TRUE;
    }
};

void wxWindow::ChangeToGray(Bool gray)
{
  if (XtIsSubclass(X->handle, xfwfLabelWidgetClass)
      || XtIsSubclass(X->handle, xfwfMultiListWidgetClass))
    XtVaSetValues(X->handle, XtNdrawgray, (Boolean)gray, NULL);

  if (XtIsSubclass(X->frame, xfwfEnforcerWidgetClass))
    XtVaSetValues(X->frame, XtNdrawgray, (Boolean)gray, NULL);

  /* If disabling and this window has the focus, get rid of it: */
  if (gray && (misc_flags & FOCUS_FLAG)) {
    wxWindow *p = GetParent();
    while (p) {
      if (wxSubType(p->__type, wxTYPE_FRAME)) {
	p->SetFocus();
	break;
      }
      p = p->GetParent();
    }
  }
}

Bool wxWindow::IsGray(void)
{
  return (misc_flags & DISABLED_FLAG) || internal_gray_disabled;
}

void wxWindow::InternalEnable(Bool enable, Bool gray)
{
  Bool do_something;
  short start_igd = internal_gray_disabled;
  
  if (!X->frame || !X->handle) // forbid, if no widget associated
    return;
  
  if (!enable) {
    do_something = !internal_disabled;
    internal_disabled++;
    if (gray)
      internal_gray_disabled++;
  } else { 
    --internal_disabled;
    do_something = !internal_disabled;
    if (gray)
      --internal_gray_disabled;
  }

  if (do_something && !(misc_flags & DISABLED_FLAG))
    wxSetSensitive(X->frame, enable);

  if ((!!internal_gray_disabled != !!start_igd) && !(misc_flags & DISABLED_FLAG))
    ChangeToGray(!!internal_gray_disabled);
}

void wxWindow::Enable(Bool enable)
{
  Bool orig_enabled = !(misc_flags & DISABLED_FLAG);

  if (!X->frame || !X->handle) // forbid, if no widget associated
    return;
  
  if (orig_enabled == !!enable)
    return;

  if (!enable)
    misc_flags |= DISABLED_FLAG;
  else
    misc_flags -= DISABLED_FLAG;

  if (!internal_disabled)
    wxSetSensitive(X->frame, enable);

  /* Doing handle sensitive makes it gray: */
  if (!internal_gray_disabled)
    ChangeToGray(!enable);
}

Bool wxWindow::PopupMenu(wxMenu *menu, float x, float y)
{
  int dev_x = (int)x;
  int dev_y = (int)y;
  
  if (!X->frame || !X->handle) // forbid, if no widget associated
    return FALSE;

  ClientToScreen(&dev_x, &dev_y);
  menu->PopupMenu(X->frame, dev_x, dev_y);
  return TRUE;
}

void wxWindow::GetRefreshSize(int *w, int *h)
{
  GetSize(w, h);
}

void wxWindow::Refresh(void)
{
    XExposeEvent  dummyEvent;
    int           width, height;

    if (!X->handle) // forbid, if no widget associated
	return;

    GetRefreshSize(&width, &height);

    dummyEvent.type	  = Expose;
    dummyEvent.display	  = XtDisplay(X->handle);
    dummyEvent.send_event = True;
    dummyEvent.window	  = XtWindow(X->handle);
    dummyEvent.x	  = 0;
    dummyEvent.y	  = 0;
    dummyEvent.width	  = width;
    dummyEvent.height	  = height;
    dummyEvent.count	  = 0;

    XSendEvent(XtDisplay(X->handle), XtWindow(X->handle),
	       False, ExposureMask, (XEvent*)&dummyEvent);
}

void wxWindow::ReleaseMouse(void)
{
    if (!X->frame) // forbid, if no widget associated
	return;

    if (captured) {
	XtRemoveGrab(X->frame);
	captured = FALSE;
    }
};

void wxWindow::SetFocus(void)
{
  wxWindow *win;

  if (!X->frame) // forbid, if no widget associated
    return;

  if (IsGray())
    return;

  if (misc_flags & FOCUS_FLAG)
    /* focus is already here */
    return;

  // search for the frame of this widget
  win = this;
  for (/*wxWindow *win = this*/; win; win = win->parent) {
    if (wxSubType(win->__type, wxTYPE_FRAME))
      break;
  }
  
#if 0
  /* MATTHEW: Is the frame currently active? */
  if (win && (win->misc_flags & FOCUS_FLAG)) {
    /* MATTHEW: Avoids trying to set focus when it's already there: */
    if (XtIsSubclass(X->frame, xfwfCommonWidgetClass)) {
      Time time = CurrentTime;
      
      XtCallAcceptFocus(X->frame, &time);
      
      return;
    }
  }
#endif

  // if found: set focus
  if (win)
    XtSetKeyboardFocus(win->X->frame, X->frame);
}

Bool wxWindow::Show(Bool show)
{
    if (parent)
      parent->GetChildren()->Show(this, show);

    if (!X->handle) // forbid, if no widget associated
      return TRUE;

    /* MATTHEW: [5] Get rid of or restore focus traversal */
    if (XtIsSubclass(X->frame, xfwfCommonWidgetClass))
      XtVaSetValues(X->frame, XtNtraversalOn, (Boolean)show, NULL);

    /* MATTHEW: [6] Show/hide frame */
    if (show)
      XtManageChild(X->frame);
    else
      XtUnmanageChild(X->frame);

    SetShown(show);

    return TRUE;
}

Bool wxWindow::IsShown(void)
{
  return !!(misc_flags & SHOWN_FLAG);
}

void wxWindow::SetShown(Bool shown)
{
  if (shown)
    misc_flags |= SHOWN_FLAG;
  else
    misc_flags -= (misc_flags & SHOWN_FLAG);
}
    

//-----------------------------------------------------------------------------
// virtual event functions, that implement the default behaviour
//-----------------------------------------------------------------------------

//-- DIRTY -- DIRTY -- DIRTY -- DIRTY -- DIRTY --
//
_XFUNCPROTOBEGIN
extern EventMask _XtConvertTypeToMask(int type);	  // internal Xt function
extern void      _XtTranslateEvent(Widget w, XEvent *ev); // internal Xt function
_XFUNCPROTOEND
//
// I've used the following way to intercept the incomming events:
// - first Xt calls the expose method of the widget
// - second it calls all event handlers installed by XtAddEventHandler
// - third it evaluates the the widget's translation table
// --> I forbid the evaluation of the translation table and call
// 	_XtTranslateEvent by myself. 
//
//-- DIRTY -- DIRTY -- DIRTY -- DIRTY -- DIRTY --

void wxWindow::OnChar(wxKeyEvent& wxevent)
{
    XEvent onstack;
    XEvent *xev = (XEvent*)wxevent.eventHandle; // X event
    if (!xev) {
     xev = &onstack;
     xev->xkey.type = KeyPress;
     xev->xkey.display = wxAPP_DISPLAY;
     xev->xkey.window = XtWindow(X->handle);
     xev->xkey.root = RootWindowOfScreen(wxAPP_SCREEN);
     xev->xkey.subwindow = XtWindow(X->handle);
     xev->xkey.time = 0L;
     xev->xkey.send_event = 0;
     xev->xkey.same_screen = 0;
     xev->xkey.serial = 0;
    }

    // check if widget has translations and if this event is selected by the widget
    if (X->handle->core.tm.translations
	&& (X->translations_eventmask & _XtConvertTypeToMask(xev->type))) {
      // translate wxKeyEvent to XEvent
      KeySym keysym = CharCodeWXToX(wxevent.keyCode);
      if (keysym != 0) {
	xev->xkey.keycode = XKeysymToKeycode(xev->xkey.display, keysym);
	xev->xkey.x	 = (int)wxevent.x;
	xev->xkey.y	 = (int)wxevent.y;
	xev->xkey.state &= ~(ShiftMask | ControlMask | Mod1Mask | Mod3Mask);
	xev->xkey.state |= (wxevent.altDown     ? Mod3Mask    : 0) |
			   (wxevent.controlDown ? ControlMask : 0) |
			   (wxevent.metaDown    ? Mod1Mask    : 0) |
			   (wxevent.shiftDown   ? ShiftMask   : 0);
	// call Widget methods to handle this event
	_XtTranslateEvent(X->handle, xev);
      }
    }
}

void wxWindow::OnCommand(wxWindow& win, wxCommandEvent& event)
{
    // OnCommand events are routed to the parent by default
    if (parent && parent->GetEventHandler())
	parent->GetEventHandler()->OnCommand(win, event);
}

void wxWindow::OnEvent(wxMouseEvent& wxevent)
{
  EventMask  mask;
  XEvent    *xev = (XEvent*)wxevent.eventHandle; // X event

  if (!xev) return;

  mask = _XtConvertTypeToMask(xev->xany.type); // eventmask of event

  // adapt converted mask (Xt error????)
  if (mask & ButtonMotionMask)
    mask |= Button1MotionMask | Button2MotionMask | Button3MotionMask |
      Button4MotionMask | Button5MotionMask;
  // check if widget has translations and if this event is selected by the widget
  if (X->handle->core.tm.translations && (X->translations_eventmask & mask)) {
    // no translation of wxMouseEvents to XEvents as for OnChar
    // --- may be added on request ---
    // call Widget methods to handle this event
    _XtTranslateEvent(X->handle, xev);
  }
}

Bool wxWindow::PreOnChar(wxWindow *, wxKeyEvent *)
{
  return FALSE;
}

Bool wxWindow::PreOnEvent(wxWindow *, wxMouseEvent *)
{
  return FALSE;
}

Bool wxWindow::CallPreOnChar(wxWindow *win, wxKeyEvent *event)
{
  wxWindow *p = win->GetParent();

  if (wxSubType(win->__type, wxTYPE_MENU_BAR)
      || wxSubType(win->__type, wxTYPE_MENU))
    return FALSE;

  if (wxSubType(win->__type, wxTYPE_FRAME)
      || wxSubType(win->__type, wxTYPE_DIALOG_BOX))
    p = NULL;

  return ((p && CallPreOnChar(p, event))
	  || win->IsGray()
	  || win->PreOnChar(this, event));
}

Bool wxWindow::CallPreOnEvent(wxWindow *win, wxMouseEvent *event)
{
  wxWindow *p = win->GetParent();

  if (wxSubType(win->__type, wxTYPE_MENU_BAR)
      || wxSubType(win->__type, wxTYPE_MENU))
    return FALSE;

  if (wxSubType(win->__type, wxTYPE_FRAME)
      || wxSubType(win->__type, wxTYPE_DIALOG_BOX))
    p = NULL;

  return ((p && CallPreOnEvent(p, event)) 
	  || win->IsGray()
	  || win->PreOnEvent(this, event));
}

void wxWindow::OnPaint(void)
{
    // This works only for subclasses of the xfwfCommonWidgetClass
    XfwfCallExpose(X->handle, X->expose_event, X->expose_region);
}

void wxWindow::OnScroll(wxScrollEvent&)
{
}

//-----------------------------------------------------------------------------
// apply event handling to a wxWindow
//-----------------------------------------------------------------------------

static void FreeSaferef(Widget WXUNUSED(w), wxWindow** winp,
			XtPointer WXUNUSED(null))
{
  FREE_SAFEREF((char *)winp);
}

void wxWindow::FocusChangeCallback(void*,
				   wxWindow **winp,
				   void*on)
{
  wxWindow *win = *winp;
  if (!win)
    return;

  if (on) {
    win->misc_flags |= FOCUS_FLAG;
    win->GetEventHandler()->OnSetFocus();
  } else { 
    win->misc_flags -= (win->misc_flags & FOCUS_FLAG);
    win->GetEventHandler()->OnKillFocus();
  }
}

void wxWindow::RegisterAll(Widget ww)
{
  XtInsertEventHandler
    (ww,
     ButtonPressMask |	// for OnEvent
     ButtonReleaseMask |
     ButtonMotionMask |
     PointerMotionMask |
     PointerMotionHintMask,
     FALSE,
     (XtEventHandler)wxWindow::WindowEventHandler,
     (XtPointer)saferef,
     XtListHead);
  
  if (XtIsComposite(ww)) {
    Widget *w;
    Cardinal c, i;

    XtVaGetValues(ww, XtNchildren, &w, XtNnumChildren, &c, NULL);
    for (i = 0; i < c; i++)
      RegisterAll(w[i]);
  }
}

void wxWindow::AddEventHandlers(void)
{
    if (!X->frame || !X->handle) // forbid, if no widget associated
	return;

    // event handler for frame
    XtInsertEventHandler(X->frame,		// handle events for frame widget
			 StructureNotifyMask |	// for OnSize, OnEvent
			 SubstructureNotifyMask,// for adding of window-eventhandler
			 TRUE,			// for OnClose
			 (XtEventHandler)wxWindow::FrameEventHandler,
			 (XtPointer)saferef,
			 XtListHead);
    // handle expose events (works only for subclasses of xfwfCommonWidgetClass)
    if (XtIsSubclass(X->handle, xfwfCommonWidgetClass)) {
	XtAddCallback(X->handle, XtNexposeCallback,
		      (XtCallbackProc)wxWindow::ExposeEventHandler,
		      (XtPointer)saferef);
	XtVaSetValues(X->handle, XtNuseExposeCallback, TRUE, NULL);
      XtAddCallback(X->handle, XtNfocusHiliteChange,
		    (XtCallbackProc)FocusChangeCallback, 
		    (XtPointer)saferef);
    }
    // handle scroll events (works only for scrollable widgets)
    if (X->scroll) {
      XtAddCallback(X->scroll, XtNscrollCallback,
		    (XtCallbackProc)wxWindow::ScrollEventHandler,
		    (XtPointer)saferef);
      if (XtIsSubclass(X->scroll, xfwfCommonWidgetClass))
	XtAddCallback(X->scroll, XtNfocusHiliteChange,
		      (XtCallbackProc)FocusChangeCallback, 
		      (XtPointer)saferef);
    }

    if (XtIsSubclass(X->frame, xfwfCommonWidgetClass)) {
      XtAddCallback(X->frame, XtNonDestroy,
		    (XtCallbackProc)FreeSaferef,
		    (XtPointer)saferef);
      XtAddCallback(X->frame, XtNfocusHiliteChange,
		    (XtCallbackProc)FocusChangeCallback, 
		    (XtPointer)saferef);
    }

    wxWindow *win = this;

    win->X->translations_eventmask = XtBuildEventMask(win->X->handle);
    XtInsertEventHandler
      (win->X->handle,	// handle events for client area widget
       KeyPressMask |	// for OnChar
       KeyReleaseMask |
       ButtonPressMask |	// for OnEvent
       ButtonReleaseMask |
       ButtonMotionMask |
       PointerMotionMask |
       PointerMotionHintMask |
       EnterWindowMask |
       LeaveWindowMask |
       (XtIsSubclass(win->X->handle, xfwfCommonWidgetClass) ?
	NoEventMask : ExposureMask), // for OnPaint (non-xfwfCommonWidget-subclasses)
       FALSE,
       (XtEventHandler)wxWindow::WindowEventHandler,
       (XtPointer)saferef,
       XtListHead);

    if (__type == wxTYPE_LIST_BOX) {
      /* Yuck. Get Mouse-moved events in total client area of listbox */
      XtInsertEventHandler
	(XtParent(win->X->handle),
	 ButtonPressMask |	// for OnEvent
	 ButtonReleaseMask |
	 ButtonMotionMask |
	 PointerMotionMask |
	 PointerMotionHintMask,
	 FALSE,
	 (XtEventHandler)wxWindow::WindowEventHandler,
	 (XtPointer)saferef,
	 XtListHead);
    }

    if (win->X->scroll)
      RegisterAll(win->X->scroll);

    XtInsertEventHandler
      (win->X->frame,	// handle events for frame widget
       EnterWindowMask |
       LeaveWindowMask |
       FocusChangeMask | // for OnKillFocus, OnSetFocus
       /* Yucky hack to make PreOnChar work for messages, sliders, and gauges: */
       ((wxSubType(win->__type, wxTYPE_MESSAGE) 
	 || wxSubType(win->__type, wxTYPE_SLIDER) 
	 || wxSubType(win->__type, wxTYPE_GAUGE))
	? (KeyPressMask | KeyReleaseMask) : NoEventMask),
       FALSE,
       (XtEventHandler)wxWindow::WindowEventHandler,
       (XtPointer)saferef,
       XtListHead);
}

void wxWindow::ExposeEventHandler(Widget     WXUNUSED(w),
				  wxWindow** winp,
				  XtPointer  p_XfwfExposeInfo)
{
  XfwfExposeInfo *einfo;
  wxWindow *win = *winp;

  if (!win)
    return;

    einfo = (XfwfExposeInfo*)p_XfwfExposeInfo;

    if (win->painting_enabled) { // painting is allowed
	if (win->dc) {
	    if (!(win->dc->ok)) { // setup drawable of dc on first expose
		win->dc->X->draw_window = win->dc->X->drawable = XtWindow(win->X->handle);
		win->dc->SetBackground(&win->dc->current_background_color);
		win->dc->Clear();
		win->dc->ok = TRUE;
	    }
	    // setup clipping region
	    win->dc->X->expose_reg = einfo->region;
	    win->dc->SetCanvasClipping();
	}
	// call refresh method
	win->X->expose_region = einfo->region;
	win->X->expose_event  = einfo->event;
	win->GetEventHandler()->OnPaint();
	if (win->dc) {
	    // reset clipping region
	    win->dc->X->expose_reg = NULL;
	    win->dc->SetCanvasClipping();
	}
    }
}

void wxWindow::FrameEventHandler(Widget w,
				 wxWindow **winp,
				 XEvent *xev,
				 Boolean *WXUNUSED(continue_to_dispatch_return))
{
  /* MATTHEW: */
  wxWindow *win = *winp;
  if (!win)
    return;

    switch (xev->xany.type) {
    case ClientMessage:
// 	printf("ClientMessage <%s> for %s\n",
// 	       XGetAtomName(XtDisplay(w), xev->xclient.message_type),
// 	       win->GetName());
	// only wxFrames have a WM_DELETE_WINDOW property
	if(!strcmp(XGetAtomName(XtDisplay(w),xev->xclient.message_type),"WM_PROTOCOLS")
	&& !strcmp(XGetAtomName(XtDisplay(w),xev->xclient.data.l[0]),"WM_DELETE_WINDOW")){
	    // I've reveived a WM_DELETE_WINDOW message for win
	    wxWindow *current_modal = wxGetModalWindow(win);
	    if (current_modal && (current_modal != win))
	      return;
#if 0
	    if (wxModalShowingStack.Number()>0 && wxModalFrames.First()->Data()!=win)
		// forbid closing if there is a top-modal frame other than this
		return;
#endif
	    
	    // remove any focus.
	    /* MATTHEW: [3] No! */
	    /* XSetInputFocus(XtDisplay(w), None, RevertToNone, CurrentTime); */
	    /* wxFlushEvents(); */

	    // delete frame, if allowed
	    if (win->GetEventHandler()->OnClose())
	      /* MATTHEW: [3] In GC mode, hide instead of destroy */
#if !WXGARBAGE_COLLECTION_ON
		delete win;
#else
	        win->Show(FALSE);
#endif
	}
	break;
    case CreateNotify:
	/* MATTHEW: was here */
	break;
    case ConfigureNotify:
	// layout window
	win->Layout();
	// notify size and position change
	win->GetEventHandler()->OnMove(xev->xconfigure.width, xev->xconfigure.height);
	win->GetEventHandler()->OnSize(xev->xconfigure.width, xev->xconfigure.height);
	break;
    case UnmapNotify:
        if (wxSubType(win->__type, wxTYPE_DIALOG_BOX)) {
	  /* Check for a frame in the parent hierarchy: */
	  wxWindow *p = win->GetParent();
	  while (p) {
	    if (!wxSubType(p->__type, wxTYPE_DIALOG_BOX))
	      break;
	    p = p->GetParent();
	  }
	  /* No parent? Can't iconize. */
	  if (!p) {
            if (win->IsShown()) {
              ((wxDialogBox *)win)->Iconize(FALSE);
	    }
	  }
	}
	break;
    }
}

void wxWindow::ScrollEventHandler(Widget    WXUNUSED(w),
				  wxWindow  **winp,
				  XtPointer p_XfwfScrollInfo)
{
  XfwfScrollInfo *sinfo = (XfwfScrollInfo*)p_XfwfScrollInfo;
  
  wxScrollEvent *_wxevent = new wxScrollEvent();
  wxScrollEvent &wxevent = *_wxevent;
  
  wxWindow *win = *winp;
  if (!win)
    return;

  if (win->misc_flags & NO_AUTO_SCROLL_FLAG) {
    int dir;
    switch (sinfo->reason) {
    case XfwfSUp:	
      win->SetScrollPos(dir = wxVERTICAL, win->vs_pos - 1);
      break;
    case XfwfSLeft:
      win->SetScrollPos(dir = wxHORIZONTAL, win->hs_pos - 1);
      break;
    case XfwfSDown:
      win->SetScrollPos(dir = wxVERTICAL, win->vs_pos + 1);
      break;
    case XfwfSRight:
      win->SetScrollPos(dir = wxHORIZONTAL, win->hs_pos + 1);
      break;
    case XfwfSPageUp:
      win->SetScrollPos(dir = wxVERTICAL, win->vs_pos - win->vs_page);
      break;
    case XfwfSPageLeft:
      win->SetScrollPos(dir = wxHORIZONTAL, win->hs_pos - win->hs_page);
      break;
    case XfwfSPageDown:
      win->SetScrollPos(dir = wxVERTICAL, win->vs_pos + win->vs_page);
      break;
    case XfwfSPageRight:
      win->SetScrollPos(dir = wxHORIZONTAL, win->hs_pos + win->hs_page);
      break;
    case XfwfSTop:
    case XfwfSLeftSide:
    case XfwfSBottom:
    case XfwfSRightSide:
      dir = wxHORIZONTAL;
      break;
    case XfwfSDrag:
      { 
	double x, y;
	xws_get_scroll_pos(win->X->scroll, &x, &y);
	win->wxWindow::Scroll((int)(win->hs_width * x), (int)(win->vs_width * y));
	if (sinfo->flags & XFWF_VPOS)
	  dir = wxVERTICAL;
	else
	  dir = wxHORIZONTAL;
      }
      break;
    default:
      dir = wxHORIZONTAL;
      break;
    }
    wxevent.pos = win->GetScrollPos(dir);
  } else {
    // sinfo->gx and sinfo->gy are set by the ScrolledWindow widget
    XtMoveWidget(win->X->handle, sinfo->gx, sinfo->gy);
    win->Refresh();
  }
  
  if (win->misc_flags & NO_AUTO_SCROLL_FLAG) {
    wxevent.eventHandle = (char*)p_XfwfScrollInfo;
    WXSCROLLORIENT(wxevent) = wxHORIZONTAL;
    switch (sinfo->reason) {
    case XfwfSUp:	WXSCROLLORIENT(wxevent) = wxVERTICAL;
    case XfwfSLeft:	WXSCROLLPOS(wxevent) = wxEVENT_TYPE_SCROLL_LINEUP;
      break;
    case XfwfSDown:	WXSCROLLORIENT(wxevent) = wxVERTICAL;
    case XfwfSRight:	WXSCROLLPOS(wxevent) = wxEVENT_TYPE_SCROLL_LINEDOWN;
      break;
    case XfwfSPageUp:	WXSCROLLORIENT(wxevent) = wxVERTICAL;
    case XfwfSPageLeft:	WXSCROLLPOS(wxevent) = wxEVENT_TYPE_SCROLL_PAGEUP;
      break;
    case XfwfSPageDown:	WXSCROLLORIENT(wxevent) = wxVERTICAL;
    case XfwfSPageRight:WXSCROLLPOS(wxevent) = wxEVENT_TYPE_SCROLL_PAGEDOWN;
      break;
    case XfwfSTop:	WXSCROLLORIENT(wxevent) = wxVERTICAL;
    case XfwfSLeftSide:	WXSCROLLPOS(wxevent) = wxEVENT_TYPE_SCROLL_TOP;
      break;
    case XfwfSBottom:	WXSCROLLORIENT(wxevent) = wxVERTICAL;
    case XfwfSRightSide:WXSCROLLPOS(wxevent) = wxEVENT_TYPE_SCROLL_BOTTOM;
      break;
    case XfwfSDrag:	WXSCROLLPOS(wxevent) = wxEVENT_TYPE_SCROLL_THUMBTRACK;
    default:
      break;
    }

    win->GetEventHandler()->OnScroll(wxevent);

    wxevent.eventHandle = NULL;
  }
}

extern Bool wxIsAlt(KeySym key_sym);

void wxWindow::WindowEventHandler(Widget w,
				  wxWindow **winp,
				  XEvent *xev,
				  Boolean *continue_to_dispatch_return)
{
  Bool subWin;
  wxWindow *win = *winp;
  Bool Enter=FALSE, Press=FALSE;

  if (!win) {
    *continue_to_dispatch_return = FALSE;
    return;
  }

  subWin = (w != win->X->handle) && (w != win->X->frame);

    switch (xev->xany.type) {
    case KeyPress: {
	wxKeyEvent *wxevent = new wxKeyEvent(wxEVENT_TYPE_CHAR);

	KeySym	   keysym;
	(void)XLookupString(&(xev->xkey), NULL, 0, &keysym, NULL);

	if (wxIsAlt(keysym) && !(xev->xkey.state & (ShiftMask | ControlMask)))
	  win->misc_flags |= LAST_WAS_ALT_DOWN_FLAG;
	else if (win->misc_flags & LAST_WAS_ALT_DOWN_FLAG)
	  win->misc_flags -= LAST_WAS_ALT_DOWN_FLAG;

	// set wxWindows event structure
	wxevent->eventHandle	= (char*)xev;
	wxevent->keyCode	= CharCodeXToWX(keysym);
	wxevent->x		= xev->xkey.x;
	wxevent->y		= xev->xkey.y;
	wxevent->altDown	= /* xev->xkey.state & Mod3Mask */ FALSE;
	wxevent->controlDown	= xev->xkey.state & ControlMask;
	wxevent->metaDown	= xev->xkey.state & Mod1Mask;
	wxevent->shiftDown	= xev->xkey.state & ShiftMask;
	wxevent->timeStamp      = xev->xkey.time; /* MATTHEW */
	*continue_to_dispatch_return = FALSE;
	if (!win->CallPreOnChar(win, wxevent)) {
	  /* hack: ignore SubWin for a choice item key event: */
	  if (subWin && (win->__type == wxTYPE_CHOICE))
	    subWin = 0;

	  if (subWin)
	    *continue_to_dispatch_return = TRUE;
	  else
	    win->GetEventHandler()->OnChar(*wxevent);
	}
	wxevent->eventHandle = NULL; /* MATTHEW: [5] */
        /* Event was handled by OnFunctionKey and/or OnChar */ }
	break;
    case KeyRelease:
        {
	  KeySym	   keysym;

	  *continue_to_dispatch_return = FALSE;
	  if (win->misc_flags & LAST_WAS_ALT_DOWN_FLAG) {
	    win->misc_flags -= LAST_WAS_ALT_DOWN_FLAG;

	    (void)XLookupString(&(xev->xkey), NULL, 0, &keysym, NULL);
	    if (wxIsAlt(keysym)) {
	      /* Find frame. */
	      wxWindow *p = win;
	      while (p) {
		if (wxSubType(p->__type, wxTYPE_FRAME)) {
		  wxMenuBar *mb = ((wxFrame *)p)->GetMenuBar();
		  if (mb)
		    mb->SelectAMenu();
		  break;
		}
		p = p->GetParent();
	      }
	    }
	  }
	}
        break;
    case ButtonPress:
	Press = TRUE;
    case ButtonRelease: 
      if (win->misc_flags & LAST_WAS_ALT_DOWN_FLAG)
	win->misc_flags -= LAST_WAS_ALT_DOWN_FLAG;
      {
        wxMouseEvent *wxevent = new wxMouseEvent;

	switch (xev->xbutton.button) {
	case Button1: wxevent->eventType = wxEVENT_TYPE_LEFT;   break;
	case Button2: wxevent->eventType = wxEVENT_TYPE_MIDDLE; break;
	case Button3: wxevent->eventType = wxEVENT_TYPE_RIGHT;  break;
	}
	if (Press) {
	  // button is down
	  wxevent->eventType |= wxEVENT_TYPE_DOWN;
	  if (win->allow_dclicks) { // doubleclick handling wanted?
	    if (xev->xbutton.button == win->X->last_clickbutton
		&&  (xev->xbutton.time - win->X->last_clicktime
		     <= (unsigned int)XtGetMultiClickTime(wxAPP_DISPLAY))) {
	      // double click has arrived
	      wxevent->eventType |= wxEVENT_TYPE_DOUBLE;
	      win->X->last_clicktime = 0; 
	    } else {
	      // single click has arrived
	      win->X->last_clickbutton = xev->xbutton.button;
	      win->X->last_clicktime   = xev->xbutton.time;
	    }
	  }
	}
	// set wxWindows event structure
	wxevent->eventHandle	= (char*)xev;
	wxevent->x		= xev->xbutton.x;
	wxevent->y		= xev->xbutton.y;
	wxevent->altDown		= /* xev->xbutton.state & Mod3Mask */ FALSE;
	wxevent->controlDown	= xev->xbutton.state & ControlMask;
	wxevent->metaDown	= xev->xbutton.state & Mod1Mask;
	wxevent->shiftDown	= xev->xbutton.state & ShiftMask;
	wxevent->leftDown	= ((wxevent->eventType == wxEVENT_TYPE_LEFT_DOWN)
				   || (xev->xbutton.state & Button1Mask));
	wxevent->middleDown	= ((wxevent->eventType == wxEVENT_TYPE_MIDDLE_DOWN)
				   || (xev->xbutton.state & Button2Mask));
	wxevent->rightDown	= ((wxevent->eventType == wxEVENT_TYPE_RIGHT_DOWN)
				   || (xev->xbutton.state & Button3Mask));
	wxevent->timeStamp       = xev->xbutton.time; /* MATTHEW */
	*continue_to_dispatch_return = FALSE;
	if (!win->CallPreOnEvent(win, wxevent)) {
	  if (subWin) {
	    *continue_to_dispatch_return = TRUE;
	  } else {
	    if (Press 
		&& !wxSubType(win->__type, wxTYPE_MENU_BAR)
		&& !wxSubType(win->__type, wxTYPE_PANEL))
	      win->SetFocus();
	    win->GetEventHandler()->OnEvent(*wxevent);
	  }
	}
	wxevent->eventHandle = NULL; /* MATTHEW: [5] */
        }
	break;
    case EnterNotify:
      Enter = TRUE;
    case LeaveNotify: 
      if (win->misc_flags & LAST_WAS_ALT_DOWN_FLAG)
	win->misc_flags -= LAST_WAS_ALT_DOWN_FLAG;
      if (w == win->X->frame) {
	/* If Focus == PointerRoot, manage activation */
	if (xev->xcrossing.detail != NotifyInferior) {
	  Window current;
	  int old_revert;
	  XGetInputFocus(XtDisplay(win->X->frame), &current, &old_revert);
	  if (current == PointerRoot) {
	    if (Enter)
	      win->misc_flags |= ACTIVE_VIA_POINTER_FLAG;
	    else
	      win->misc_flags -= (win->misc_flags & ACTIVE_VIA_POINTER_FLAG);
	    win->GetEventHandler()->OnActivate(Enter);
	  }
	}
      } else {
        wxMouseEvent *wxevent = new wxMouseEvent(Enter 
						 ? wxEVENT_TYPE_ENTER_WINDOW 
						 : wxEVENT_TYPE_LEAVE_WINDOW);

	// set wxWindows event structure
	wxevent->eventHandle	= (char*)xev;
	wxevent->x		= xev->xcrossing.x;
	wxevent->y		= xev->xcrossing.y;
	wxevent->altDown		= /* xev->xcrossing.state & Mod3Mask */ FALSE;
	wxevent->controlDown	= xev->xcrossing.state & ControlMask;
	wxevent->metaDown	= xev->xcrossing.state & Mod1Mask;
	wxevent->shiftDown	= xev->xcrossing.state & ShiftMask;
	wxevent->leftDown	= xev->xcrossing.state & Button1Mask;
	wxevent->middleDown	= xev->xcrossing.state & Button2Mask;
	wxevent->rightDown	= xev->xcrossing.state & Button3Mask;
	wxevent->timeStamp       = xev->xbutton.time; /* MATTHEW */
	*continue_to_dispatch_return = FALSE; /* Event was handled by OnEvent */ 
	if (!win->CallPreOnEvent(win, wxevent))
	  win->GetEventHandler()->OnEvent(*wxevent);
	wxevent->eventHandle = NULL; /* MATTHEW: [5] */
      }
      break;
    case MotionNotify: {
	wxMouseEvent *wxevent = new wxMouseEvent(wxEVENT_TYPE_MOTION);

	if (xev->xmotion.is_hint == NotifyHint) {
	    // hints need a XQueryPointer
	    Window root, child;
	    XQueryPointer(XtDisplay(w), XtWindow(w), &root, &child,
			  &(xev->xmotion.x_root), &(xev->xmotion.y_root),
			  &(xev->xmotion.x),      &(xev->xmotion.y),
			  &(xev->xmotion.state));
	}
	// set wxWindows event structure
	wxevent->eventHandle	= (char*)xev;
	wxevent->x		= xev->xmotion.x;
	wxevent->y		= xev->xmotion.y;
	wxevent->altDown		= /* xev->xmotion.state & Mod3Mask */ FALSE;
	wxevent->controlDown	= xev->xmotion.state & ControlMask;
	wxevent->metaDown	= xev->xmotion.state & Mod1Mask;
	wxevent->shiftDown	= xev->xmotion.state & ShiftMask;
	wxevent->leftDown	= xev->xmotion.state & Button1Mask;
	wxevent->middleDown	= xev->xmotion.state & Button2Mask;
	wxevent->rightDown	= xev->xmotion.state & Button3Mask;
	wxevent->timeStamp       = xev->xbutton.time; /* MATTHEW */
	*continue_to_dispatch_return = FALSE; /* Event was handled by OnEvent */
	if (!win->CallPreOnEvent(win, wxevent)) {
	  if (subWin)
	    *continue_to_dispatch_return = TRUE;
	  else
	    win->GetEventHandler()->OnEvent(*wxevent);
	}
	wxevent->eventHandle = NULL; /* MATTHEW: [5] */
      }
	break;
      /* MATTHEW : [5] Use focus in/out for OnActivate */
    case FocusIn:
        Enter = TRUE;
    case FocusOut:
      if (win->misc_flags & LAST_WAS_ALT_DOWN_FLAG)
	win->misc_flags -= LAST_WAS_ALT_DOWN_FLAG;
      if (xev->xfocus.detail != NotifyInferior) {
	Window current;
	if (xev->xfocus.detail == NotifyPointer) {
	  /* NotifyPointer is meaningful if the focus is PointerRoot
	     or we're active via the pointer */
	  if (!Enter && (win->misc_flags & ACTIVE_VIA_POINTER_FLAG)) {
	    current = PointerRoot;
	  } else {
	    int old_revert;
	    XGetInputFocus(XtDisplay(win->X->frame), &current, &old_revert);
	  }
	} else
	  current = PointerRoot;

	if (current == PointerRoot) {
	  if (xev->xfocus.detail == NotifyPointer) {
	    if (Enter)
	      win->misc_flags |= ACTIVE_VIA_POINTER_FLAG;
	    else
	      win->misc_flags -= (win->misc_flags & ACTIVE_VIA_POINTER_FLAG);
	  }
	  win->GetEventHandler()->OnActivate(Enter);
	}
      }
      break;
    case Expose: // arrives for non-xfwfCommonWidget-subclasses only
	if (win->dc && win->painting_enabled) { // expose only if DC available
	    // setup drawable of dc if dc available
	    if (!(win->dc->ok)) { // first expose call
		win->dc->X->draw_window = win->dc->X->drawable = XtWindow(win->X->handle);
		win->dc->SetBackground(&win->dc->current_background_color);
		win->dc->Clear();
		win->dc->ok = TRUE;
	    }
	    // call refresh method
	    win->GetEventHandler()->OnPaint();
	}
        break;
    }
}

//-----------------------------------------------------------------------------
// create and destroy associated device context
//-----------------------------------------------------------------------------

void wxWindow::CreateDC(void)
{
    if (dc) return; // only create once!

    dc = DEBUG_NEW wxWindowDC;
    // Initialize wxWindowDC
    wxWindowDC_Xinit init;
    init.dpy      = wxAPP_DISPLAY; // display is global to application
    init.scn      = wxAPP_SCREEN;  //  screen is global to application
    init.owner    = this;
    init.drawable = XtWindow(X->handle);
    dc->ok = TRUE;
    
    dc->Initialize(&init);

    dc->X->is_window = TRUE;
}

void wxWindow::DestroyDC(void)
{
    if (!dc) return; // no DC to destroy
    // destroy device context
    delete dc;
    dc = NULL;
}

wxWindowDC *wxWindow::GetDC(void)
{ 
  if (!dc) {
    if (!(style & wxNO_DC))
      CreateDC();
  }
  
  return dc; 
}

void wxWindow::GetTextExtent(const char *s, float *w, float *h, float *descent,
			     float *ext_leading, wxFont *theFont,
			     Bool use16bit)
{
    int direction, ascent, descent2;
    XCharStruct overall;

    if (dc) {
      dc->GetTextExtent(s, w, h, descent, ext_leading, theFont, use16bit);
      return;
    }

    if (!theFont) theFont = font;
    XTextExtents((XFontStruct*)theFont->GetInternalFont(), s, strlen(s),
		 &direction, &ascent, &descent2, &overall);
    *w = (float)(overall.width);
    *h = (float)(ascent + descent2);
    if (descent) *descent = (float)descent2;
    if (ext_leading) *ext_leading = 0.0;
}


void wxWindow::ForEach(void (*foreach)(wxWindow *w, void *data), void *data)
{
  wxChildNode *node, *next;
  for (node = children->First(); node; node = next) {
    next = node->Next();
    wxWindow *child = (wxWindow*)(node->Data());
    if (child) {
      child->ForEach(foreach, data);
    }
  }

  foreach(this, data);
}

