///////////////////////////////////////////////////////////////////////////////
// File:	wx_win.cc
// Purpose:	wxWindow class implementation (Macintosh version)
// Author:	Bill Hale
// Created:	1994
// Updated:	
// Copyright:  (c) 1993-94, AIAI, University of Edinburgh. All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////

#include "wx_win.h"
#include "wxMacDC.h"
#include "wx_utils.h"
#include "wx_frame.h"
#include "wx_mac_utils.h"
#include "wx_gdi.h"
#include "wx_area.h"
#include "wx_stdev.h"
#include "wx_screen.h"
#include "wxScroll.h"
#include "wx_panel.h"
#include "wx_dialg.h"
#include "wx_dccan.h"
#include "wx_main.h"
#include "wx_menu.h"
#include "wx_rbox.h"
#include "wxTimeScale.h"
#include "wx_macevents.h"
#ifndef WX_CARBON
# include <QuickDraw.h>
#endif

// these offsets are used to eliminate calls to the real SetOrigin
int SetOriginX = 0;
int SetOriginY = 0;

wxWindow* wxWindow::gMouseWindow = NULL; 

static void RestoreNormalBackground(wxBrush *erase);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Geometry methods
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

wxWindow::wxWindow(void)
{
  cWindowX = 0;
  cWindowY = 0;
  cWindowWidth = 0;
  cWindowHeight = 0;
  cStyle = 0;
  cScroll = NULL;
  cAreas = new wxList(wxList::kDestroyData);
  children = new wxChildList();
  cActive = FALSE;
  cEnable = TRUE;

  InitDefaults();

  cParentArea = NULL;

  window_parent = NULL;

  cMacDC = NULL;
  
  cGrandcursor = FALSE;

  cClientArea = new wxArea(this);
}


//-----------------------------------------------------------------------------
wxWindow::wxWindow // Constructor (for screen window)
(
 char*		windowName,
 int 		x,
 int			y,
 int			width,
 int			height,
 long		style
 ) :
 wxbWindow (windowName),
 cStyle (style),
 children (new wxChildList()),
 cScroll (NULL),
 cWindowX (x != -1 ? x : 0),
 cWindowY (y != -1 ? y : 0),
 cWindowHeight (height >= 0 ? height : 0),
 cWindowWidth (width >= 0 ? width : 0)
{
  cAreas = new wxList(wxList::kDestroyData);
  cActive = FALSE;
  cEnable = TRUE;
  
  InitDefaults();

  cParentArea = NULL;

  window_parent = NULL;

  cMacDC = NULL; // WCH: should set to screen grafPort ??

  cGrandcursor = FALSE;

  cClientArea = new wxArea(this);
}

//-----------------------------------------------------------------------------
wxWindow::wxWindow // Constructor (given parentScreen; i.e., this is frame)
(
 char*		windowName,
 wxScreen*	parentScreen,
 int 		x,
 int			y,
 int			width,
 int			height,
 long		style
 ) :
 wxbWindow (windowName),
 cStyle (style),
 children (new wxChildList()),
 cScroll (NULL),
 cWindowX (x != -1 ? x : 0),
 cWindowY (y != -1 ? y : 0),
 cWindowHeight (height >= 0 ? height : 0),
 cWindowWidth (width >= 0 ? width : 0),
 cAreas (new wxList(wxList::kDestroyData))
{
  cActive = FALSE;
  cEnable = TRUE;
  
  if (!parentScreen) wxFatalError("No parent screen for constructing frame.");

  InitDefaults();

  cParentArea = parentScreen->ClientArea();
  cParentArea->Windows()->Append(this);

  window_parent = parentScreen;
  window_parent->AddChild(this);

  // Frames are initially hidden!
  window_parent->GetChildren()->Show(this, FALSE);
  cParentArea->Windows()->Show(this, FALSE);

  cMacDC = NULL; // will set cMacDC later

  cGrandcursor = FALSE;

  cClientArea = new wxArea(this);
}

//-----------------------------------------------------------------------------
wxWindow::wxWindow // Constructor (given parentArea)
(
 char*		windowName,
 wxArea*		parentArea,
 int 		x,
 int			y,
 int			width,
 int			height,
 long		style
 ) :
 wxbWindow (windowName),
 cStyle (style),
 children (new wxChildList()),
 cScroll (NULL),
 cWindowHeight (height >= 0 ? height : 0),
 cWindowWidth (width >= 0 ? width : 0),
 cAreas (new wxList(wxList::kDestroyData))
{
  cActive = FALSE;
  cEnable = TRUE;

  if (!parentArea) wxFatalError("No parent area for constructing window.");

  InitDefaults();

  cParentArea = parentArea;
  cParentArea->Windows()->Append(this);

  window_parent = cParentArea->ParentWindow();

  cMacDC = window_parent->MacDC();

  cGrandcursor = FALSE;

  InitWindowPostion(x, y);
  window_parent->AddChild(this);

  cClientArea = new wxArea(this);
}

//-----------------------------------------------------------------------------
wxWindow::wxWindow // Constructor (given parentWindow)
(
 char*		windowName,
 wxWindow*	parentWindow,
 int 		x,
 int			y,
 int			width,
 int			height,
 long		style
 ) :
 wxbWindow (windowName),
 cStyle (style),
 children (new wxChildList()),
 cScroll (NULL),
 cWindowHeight (height >= 0 ? height : 0),
 cWindowWidth (width >= 0 ? width : 0),
 cAreas (new wxList(wxList::kDestroyData))
{
  cActive = FALSE;
  cEnable = TRUE;

  if (!parentWindow) wxFatalError("No parent window for constructing window.");

  InitDefaults();

  cParentArea = parentWindow->ClientArea();
  cParentArea->Windows()->Append(this);

  window_parent = parentWindow;

  cMacDC = window_parent->MacDC();
  
  cGrandcursor = FALSE;

  InitWindowPostion(x, y);
  window_parent->AddChild(this);

  cClientArea = new wxArea(this);
}

//-----------------------------------------------------------------------------
wxWindow::wxWindow // Constructor (given objectType; i.e., menu or menuBar)
(
 char*		windowName
 ) :
 wxbWindow (windowName),
 cStyle (0),
 children (new wxChildList()),
 cScroll (NULL),
 cWindowX (0),
 cWindowY (0),
 cWindowHeight (0),
 cWindowWidth (0),
 cAreas (new wxList(wxList::kDestroyData))
{
  cActive = FALSE;
  cEnable = TRUE;

  InitDefaults();

  cParentArea = NULL;

  window_parent = NULL;

  cMacDC = NULL;
  
  cGrandcursor = FALSE;

  cClientArea = new wxArea(this);
}

//=============================================================================
// Public destructor
//=============================================================================

static wxWindow *entered = NULL;

void wxRegisterEntered();

void wxRegisterEntered()
{
  wxREGGLOB(entered);
}

//-----------------------------------------------------------------------------
wxWindow::~wxWindow(void) // Destructor
{
  
  wxPanel *panel = (wxPanel *) GetParent ();
  if (panel)
    {
      // parent is not always a wxPanel: can be a wxMenu...
      if (panel->__type == wxTYPE_PANEL)
	{
	  //if (this == parent->last_created)
	  //	parent->last_created = NULL; cjc
	  // tom fettig suggests:
	  panel->new_line = FALSE;
	  panel->label_position = wxHORIZONTAL;
	  panel->hSpacing = PANEL_HSPACING;
	  panel->vSpacing = PANEL_VSPACING;
	  panel->initial_hspacing = panel->hSpacing;
          panel->initial_vspacing = panel->vSpacing;
          panel->current_hspacing = panel->hSpacing;
          panel->current_vspacing = panel->vSpacing;
          panel->OnDeleteChild(this);
	}
    }

  if (cParentArea) 
    cParentArea->OnDeleteChildWindow(this);

  if (window_parent) window_parent->OnDeleteChildWindow(this);

  DestroyChildren();

  delete children;
  if (cScroll)
    delete cScroll;

  delete cAreas;
  
  // don't send leaveEvt messages to this window anymore.
  if (entered == this) entered = NULL; 
}

//=============================================================================
// Private methods
//=============================================================================

//-----------------------------------------------------------------------------
void wxWindow::InitDefaults(void)
{
  cGravitate = 0;
  cJustify = 0;
  cBrush = NULL;
  cEraser = NULL;

  cColour = TRUE; // WCH: must redo this

  cHidden = cUserHidden = FALSE;
  
  WXGC_IGNORE(this, window_parent);
  WXGC_IGNORE(this, cParentArea);
  WXGC_IGNORE(this, cMacDC);
}

//-----------------------------------------------------------------------------
void wxWindow::InitWindowPostion(int x, int y)
{
  if (wxSubType(window_parent->__type, wxTYPE_PANEL) &&
      cParentArea == window_parent->ClientArea())
    {
      wxPanel* parentPanel = (wxPanel*) window_parent;
      int cursorX, cursorY;
      parentPanel->GetCursor(&cursorX, &cursorY);
      cWindowX = (x != -1 ? x : cursorX);
      cWindowY = (y != -1 ? y : cursorY);
    }
  else
    {
      cWindowX = (x != -1 ? x : 0);
      cWindowY = (y != -1 ? y : 0);
    }
}


//-----------------------------------------------------------------------------
int wxWindow::Width(void) { return cWindowWidth; } // mac platform only

//-----------------------------------------------------------------------------
int wxWindow::Height(void) { return cWindowHeight; } // mac platform only

//-----------------------------------------------------------------------------
wxMargin wxWindow::Margin(wxArea* outerArea) // mac platform only
{
  wxMargin result;
  wxArea* parentArea = ParentArea();
  if (parentArea)
    {
      result.SetMargin(cWindowX, Direction::wxLeft);
      result.SetMargin(cWindowY, Direction::wxTop);
      result.SetMargin(parentArea->Width() - cWindowX - cWindowWidth,
		       Direction::wxRight);
      result.SetMargin(parentArea->Height() - cWindowY - cWindowHeight,
		       Direction::wxBottom);

      if (parentArea != outerArea)
	{
	  result += parentArea->Margin(outerArea);
	}
    }

  return result;
}

//-----------------------------------------------------------------------------
wxMargin wxWindow::Margin(wxWindow* outerWindow) // mac platform only
{
  wxMargin result;

  if (outerWindow != this)
    {
      wxArea* parentArea = ParentArea();
      if (parentArea)
	{
	  result =  Margin(parentArea);
	  result += parentArea->Margin(outerWindow);
	}
    }

  return result;
}

//-----------------------------------------------------------------------------
void wxWindow::GetPosition(int* windowX, int* windowY)
{ // Get window position w.r.t. parent area origin
  *windowX = cWindowX;
  *windowY = cWindowY;
}

//-----------------------------------------------------------------------------
void wxWindow::GetSize(int* width, int* height)
{ // Get window size
  *width = cWindowWidth;
  *height = cWindowHeight;
}

//-----------------------------------------------------------------------------
// Get size *available for subwindows* i.e. excluding menu bar etc.
//-----------------------------------------------------------------------------
void wxWindow::GetClientSize(int* width, int* height)
{
  *width = cClientArea->Width();
  *height = cClientArea->Height();
}

//-----------------------------------------------------------------------------
void wxWindow::ClientToScreen(int* x, int* y)
{
  cClientArea->AreaToScreen(x, y);
}

//-----------------------------------------------------------------------------
void wxWindow::ScreenToClient(int* x, int* y)
{
  cClientArea->ScreenToArea(x, y);
}

//-----------------------------------------------------------------------------
void wxWindow::ClientToLogical(int* x, int* y) // mac platform only; testing
{ // Transform point from client c.s. to logical c.s. (virtual canvas, scrolling)
  // default action leaves *x and *y unchanged (logical c.s. same as client c.s.)
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Sizing methods
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//-----------------------------------------------------------------------------
void wxWindow::SetWidthHeight(int width, int height) // mac platform only
{
  SetSize(cWindowX, cWindowY, width, height);
}

//-----------------------------------------------------------------------------
void wxWindow::SetSize(int x, int y, int width, int height, int flags) // mac platform only
{
  int oldWindowX = cWindowX;
  int oldWindowY = cWindowY;
  int oldWindowWidth = cWindowWidth;
  int oldWindowHeight = cWindowHeight;

  if (width == -1)
    width = oldWindowWidth;
  if (height == -1)
    height = oldWindowHeight;
  
  if (!(flags & wxPOS_USE_MINUS_ONE)) {
    if (x == -1)
      x = oldWindowX;
    if (y == -1)
      y = oldWindowY;
  }

  DoSetSize(x, y, width, height);

  int dW = cWindowWidth - oldWindowWidth;
  int dH = cWindowHeight - oldWindowHeight;
  int dX = cWindowX - oldWindowX;
  int dY = cWindowY - oldWindowY;
  OnWindowDSize(dW, dH, dX, dY);
}

//-----------------------------------------------------------------------------
void wxWindow::DoSetSize(int x, int y, int width, int height) // mac platform only
{
  if (x==-1) 
    x= cWindowX;
  if (y==-1) 
    y = cWindowY;
  if (width==-1) 
    width = cWindowWidth;
  if (height==-1) 
    height = cWindowHeight;
  
  Bool xIsChanged = (x != cWindowX);
  Bool yIsChanged = (y != cWindowY);
  Bool widthIsChanged = (width != cWindowWidth);
  Bool heightIsChanged = (height != cWindowHeight);

  if (!cHidden && (xIsChanged || yIsChanged || widthIsChanged || heightIsChanged)) {
    Rect oldWindowRect = { -1, -1, cWindowHeight, cWindowWidth };
    if (SetCurrentMacDCNoMargin()) {
      MacSetBackground();
      OffsetRect(&oldWindowRect,SetOriginX,SetOriginY);
      ::InvalWindowRect(GetWindowFromPort(cMacDC->macGrafPort()),&oldWindowRect);
      ::EraseRect(&oldWindowRect);
      RestoreNormalBackground(cEraser);
    }
  }

  if (xIsChanged) cWindowX = x;
  if (yIsChanged) cWindowY = y;
  if (widthIsChanged) cWindowWidth = width;
  if (heightIsChanged) cWindowHeight = height;

  if (!cHidden && (xIsChanged || yIsChanged || widthIsChanged || heightIsChanged)) {
    Rect newWindowRect = { -1, -1, cWindowHeight, cWindowWidth };
    cMacDC->setCurrentUser(NULL); // macDC no longer valid
    if (SetCurrentMacDCNoMargin()) { // put newClientRect at (SetOriginX,SetOriginY)
      OffsetRect(&newWindowRect,SetOriginX,SetOriginY);
      
      ::InvalWindowRect(GetWindowFromPort(cMacDC->macGrafPort()),&newWindowRect); // force redraw of window
      ::EraseRect(&newWindowRect);
      RestoreNormalBackground(cEraser);
    }
  }
}

void wxWindow::Refresh(void)
{
  Rect theClipRect;
  
  if (cHidden) return;

  GetClipRect(cClientArea, &theClipRect);
  if (SetCurrentMacDC()) {
    // put newClientRect at (SetOriginX, SetOriginY)
    OffsetRect(&theClipRect,SetOriginX,SetOriginY);
    ::InvalWindowRect(GetWindowFromPort(cMacDC->macGrafPort()),&theClipRect); // force redraw of window
  }
}

//-----------------------------------------------------------------------------
void wxWindow::SetClientSize(int newClientWidth, int newClientHeight)
{
  cClientArea->SetSize(newClientWidth, newClientHeight);
}

//-----------------------------------------------------------------------------
void wxWindow::GravitateJustify(Direction gravitate, Direction justify,
				int left, int top, int right, int bottom)
{
  int windowX = cWindowX;
  int windowY = cWindowY;
  int windowWidth = cWindowWidth;
  int windowHeight = cWindowHeight;

  // do gravitate
  if ((int)gravitate & Direction::wxLeft && (int)gravitate & Direction::wxRight)
    windowX = (left + right - cWindowWidth) / 2;
  else if ((int)gravitate & Direction::wxLeft) windowX = left;
  else if ((int)gravitate & Direction::wxRight) windowX = right - cWindowWidth;

  if ((int)gravitate & Direction::wxTop && (int)gravitate & Direction::wxBottom)
    windowY = (top + bottom - cWindowHeight) / 2;
  else if ((int)gravitate & Direction:: wxTop) windowY = top;
  else if ((int)gravitate & Direction::wxBottom) windowY = bottom - cWindowHeight;

  // do justify
  if ((int)justify & Direction::wxLeft)
    {
      windowWidth += windowX - left;
      windowX = left;
    }

  if ((int)justify & Direction::wxTop)
    {
      windowHeight += windowY - top;
      windowY = top;
    }

  if ((int)justify & Direction::wxBottom) windowHeight = bottom - windowY;

  if ((int)justify & Direction::wxRight) windowWidth = right - windowX;

  // do size
  SetSize(windowX, windowY, windowWidth, windowHeight, wxPOS_USE_MINUS_ONE);
}

//-----------------------------------------------------------------------------
void wxWindow::Fit(void)
{ // Resize window to fit exactly around all its client children
  int maxX = 0;
  int maxY = 0;
  wxChildNode* childWindowNode = ClientArea()->Windows()->First();
  while (childWindowNode)
    {
      wxWindow* childWindow = (wxWindow*)childWindowNode->Data();
      int x, y, w, h;
      childWindow->GetPosition(&x, &y);
      childWindow->GetSize(&w, &h);
      if ((x + w) > maxX) maxX = x + w;
      if ((y + h) > maxY) maxY = y + h;
      childWindowNode = childWindowNode->Next();
    }

  SetClientSize(maxX, maxY);
}

//-----------------------------------------------------------------------------
void wxWindow::OnWindowDSize(int dW, int dH, int dX, int dY)
{ 
  OnClientAreaDSize(dW, dH, dX, dY);

  // Resize child areas
  wxNode* areaNode = cAreas->First();
  while (areaNode) {
    wxArea* area = (wxArea*)areaNode->Data();
    if (area == ClientArea()) {
      /* done */
    } else {
      area->OnSiblingDSize(dW, dH, dX, dY);
    }
    areaNode = areaNode->Next();
  }
  
  if (dW || dH && (__type != wxTYPE_FRAME))
    OnSize(cWindowWidth, cWindowHeight);
}

//-----------------------------------------------------------------------------
void wxWindow::OnAreaDSize(int dW, int dH, int dX, int dY)
{
  if (cGravitate || cJustify) {
    int left = cWindowX;
    int top = cWindowY;
    int right = cWindowX + cWindowWidth + dW;
    int bottom = cWindowY + cWindowHeight + dH;
    GravitateJustify(cGravitate, cJustify, left, top, right, bottom);
  }
}

//-----------------------------------------------------------------------------
void wxWindow::OnClientAreaDSize(int dW, int dH, int dX, int dY)
{
  if (__type == wxTYPE_FRAME) {
    ::SizeControl(cMacControl, cWindowWidth, cWindowHeight);
  }

  { // Notify child windows of area resize.
    wxChildNode* childWindowNode = ClientArea()->Windows()->First();
    while (childWindowNode) {
      wxWindow* childWindow = (wxWindow*)childWindowNode->Data();
      if (childWindow) {
	childWindow->OnAreaDSize(dW, dH, dX, dY);
	childWindowNode = childWindowNode->Next();
      }
    }
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Device context methods
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//-----------------------------------------------------------------------------
wxMacDC* wxWindow::MacDC(void) { return cMacDC; } // mac platform only

//-----------------------------------------------------------------------------
int wxWindow::SetCurrentMacDCNoMargin(void) // mac platform only
{
  int vis;

  CGrafPtr theMacGrafPort = cMacDC->macGrafPort();
  
  vis = IsWindowVisible(GetWindowFromPort(theMacGrafPort));
  
  ::SetGWorld(theMacGrafPort, wxGetGDHandle());

  cMacDC->setCurrentUser(NULL); // kludge, since not doing complete setup of DC
  if (cParentArea && !wxSubType(__type, wxTYPE_FRAME)) {
    cParentArea->FrameContentAreaOffset(&SetOriginX, &SetOriginY);
    SetOriginX += cWindowX;
    SetOriginY += cWindowY;
  } else {
    SetOriginX = SetOriginY = 0;
  }
  
  return vis;
}

//-----------------------------------------------------------------------------
int wxWindow::SetCurrentMacDC(void) // mac platform only
{
  CGrafPtr theMacGrafPort = cMacDC->macGrafPort();
  int vis;

  vis = IsWindowVisible(GetWindowFromPort(theMacGrafPort));

  ::SetGWorld(theMacGrafPort, wxGetGDHandle());

  cClientArea->FrameContentAreaOffset(&SetOriginX, &SetOriginY);

  if (cMacDC->currentUser() != this) { // must setup platform
    cMacDC->setCurrentUser(NULL); // kludge, since not doing complete setup of DC
  }

  return vis;
}

//-----------------------------------------------------------------------------
int wxWindow::SetCurrentDC(void) // mac platform only
{
  int vis;
  CGrafPtr theMacGrafPort = cMacDC->macGrafPort();

  vis = IsWindowVisible(GetWindowFromPort(theMacGrafPort));

  ::SetGWorld(theMacGrafPort, wxGetGDHandle());

  cClientArea->FrameContentAreaOffset(&SetOriginX, &SetOriginY);
  
  if (cMacDC->currentUser() != this) { // must setup platform
    cMacDC->setCurrentUser(this);
    Rect theClipRect;
    if (cHidden) {
      theClipRect.top = theClipRect.bottom = 0;
      theClipRect.left = theClipRect.right = 0;
    } else {
      GetClipRect(cClientArea, &theClipRect);
      OffsetRect(&theClipRect,SetOriginX,SetOriginY);
    }

    ::ClipRect(&theClipRect);
    MacSetBackground();
    SetForeground();
    
    PenMode(patCopy);
    SetTextInfo();
  }

  return vis;
}

void wxWindow::ReleaseCurrentDC(int really)
{
#if 1
  {
    if (cMacDC->currentUser() == this) {
      GDHandle dh;
      CGrafPtr g;
      CGrafPtr theMacGrafPort = cMacDC->macGrafPort();
      ::GetGWorld(&g, &dh);
      if (g != theMacGrafPort)
	printf("ReleaseDC: not grafport!\n");	
      if (dh != wxGetGDHandle())
	printf("ReleaseDC: not device!\n");	
    } else
      printf("ReleaseDC: not current!\n");
  }
#endif

  /* This method is here for windows that use a white background, so
     that the background color can be reset.  Currently, that means
     wxListBox only. */
  if (really) {
    if (cMacDC->currentUser() == this) {
      CGrafPtr theMacGrafPort = cMacDC->macGrafPort();
      ::SetGWorld(theMacGrafPort, wxGetGDHandle());
      cMacDC->setCurrentUser(NULL);

      RestoreNormalBackground(wxWHITE_BRUSH);
    }
  }
}

static void RestoreNormalBackground(wxBrush *eraser)
{
  if (eraser == wxWHITE_BRUSH) {
    int depth;
    depth = wxDisplayDepth();
    SetThemeBackground(kThemeBrushDialogBackgroundActive, depth, depth > 1);
  }
}

RgnHandle wxWindow::GetCoveredRegion(int x, int y, int w, int h)
{
  wxWindow *parent = GetParent();
  if (!parent) return NULL;

  int dx, dy, dw, dh;
  GetPosition(&dx, &dy);
  GetSize(&dw, &dh);

  x += dx;
  y += dy;
  w = dw;
  h = dh;

  return parent->GetCoveredRegion(x, y, w, h);
}

//-----------------------------------------------------------------------------
void wxWindow::MacSetBackground(void) // mac platform only
{
  if (cEraser == wxWHITE_BRUSH) {
    BackColor(whiteColor);
    BackPat(GetWhitePattern());
  } else {
    int depth;
    depth = wxDisplayDepth();
    SetThemeBackground(kThemeBrushDialogBackgroundActive, depth, depth > 1);
  }
}

//-----------------------------------------------------------------------------
void wxWindow::SetForeground(void) // mac platform only
{
  if (IsGray()) {
    RGBColor c;
    c.red = c.green = c.blue = 0x7FFF;
    RGBForeColor(&c);
    return;
  }

  if (!cBrush)
    {
      PenPat(GetBlackPattern());
      ForeColor(blackColor);
      return;
    }

  int theBrushStyle = cBrush->GetStyle();
  if (theBrushStyle == wxSOLID)
    PenPat(GetBlackPattern());
  else if (theBrushStyle == wxTRANSPARENT)
    PenPat(GetWhitePattern()); // WCH: does this work??
  else if (IS_HATCH(theBrushStyle))
    {
      macGetHatchPattern(theBrushStyle, &cMacPattern);
      PenPat(&cMacPattern);
    }
  else
    {
      PenPat(GetBlackPattern()); // WCH: must use PenPixPat for stipple
    }

  RGBColor pixel = cBrush->GetColour()->pixel;
  if (cColour)
    RGBForeColor(&pixel);
  else
    {
      unsigned char red = cBrush->GetColour()->Red();
      unsigned char blue = cBrush->GetColour()->Blue();
      unsigned char green = cBrush->GetColour()->Green();
      Bool isWhiteColour =
	(red == (unsigned char )255 &&
	 blue == (unsigned char)255 &&
	 green == (unsigned char)255);
      ForeColor(isWhiteColour ? whiteColor : blackColor);
    }
}

//-----------------------------------------------------------------------------
void wxWindow::SetTextInfo(void) // mac platform only
{
  if (!font) {
    font = wxNORMAL_FONT;
    if (!font) {
      ::TextFont(1);
      ::TextSize(12);
      ::TextFace(0);
      return;
    }
  }

  ::TextFont(font->GetMacFontNum());
  ::TextSize(font->GetPointSize());
  ::TextFace(font->GetMacFontStyle());
}

//-----------------------------------------------------------------------------
void wxWindow::GetClipRect(wxArea* area, Rect* clipRect)
{
  // get clipRect in area c.s.
  ::SetRect(clipRect, 0, 0, area->Width(), area->Height()); // area c.s.

  if (ParentArea()) {
    wxWindow* windowParent = ParentArea()->ParentWindow();
    Rect parentClipRect;
    windowParent->GetClipRect(cParentArea, &parentClipRect);
    wxMargin parentAreaMargin = area->Margin(cParentArea);
    int parentAreaX = parentAreaMargin.Offset(Direction::wxLeft);
    int parentAreaY = parentAreaMargin.Offset(Direction::wxTop);
    ::OffsetRect(&parentClipRect, -parentAreaX, -parentAreaY); // area c.s.
    ::SectRect(&parentClipRect, clipRect, clipRect);
    if (clipRect->top < 0) clipRect->top = 0;
    if (clipRect->left < 0) clipRect->left = 0;
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Tree methods (for windows and areas)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//-----------------------------------------------------------------------------
wxArea* wxWindow::ParentArea(void) { return cParentArea; } // mac platform only

//-----------------------------------------------------------------------------
wxList* wxWindow::Areas(void) { return cAreas; } // mac platform only (kludge)

//-----------------------------------------------------------------------------
wxArea* wxWindow::ClientArea(void) { return cClientArea; } // mac platform only

//-----------------------------------------------------------------------------
wxWindow* wxWindow::GetParent(void) { return window_parent; }

//-----------------------------------------------------------------------------
wxChildList* wxWindow::GetChildren(void) { return children; }

//-----------------------------------------------------------------------------
wxWindow* wxWindow::GetGrandParent(void)
{
  if (window_parent)
    return window_parent->window_parent;
  else return NULL;
}

//-----------------------------------------------------------------------------
/* There are two kinds of root Frames - real root Frames and DialogBox's
   which are 
   */
wxFrame* wxWindow::GetRootFrame(void) // mac platform only
{
  return (wxFrame*)GetParent()->GetRootFrame();
}

ControlHandle wxWindow::GetRootControl(void)
{
  return GetRootFrame()->cMacControl;
}

//-----------------------------------------------------------------------------
void wxWindow::AddChild(wxObject* child) { children->Append(child); }

//-----------------------------------------------------------------------------
void wxWindow::OnDeleteChildWindow(wxWindow* childWindow) // mac platform only
{
  if (children) children->DeleteObject(childWindow);
}

//-----------------------------------------------------------------------------
void wxWindow::OnDeleteChildArea(wxArea* childArea) // mac platform only
{
  cAreas->OnDeleteObject(childArea);
}

//-----------------------------------------------------------------------------
void wxWindow::DestroyChildren(void)
{
  if (children)
    {
      wxChildNode* node = children->First();
      while (node)
	{
	  wxWindow* child = (wxWindow*)node->Data();
	  delete child; // this will also delete current node
	  node = children->First(); // must do since current node was deleted
	}
    }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Scroll methods
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//-----------------------------------------------------------------------------
void wxWindow::AddChildScrollWindow(wxWindow* childScrollWindow) // mac platform only
{
  wxScroll* scroll = GetScroll();
  if (!scroll) wxFatalError("No scroll for AddChildScrollWindow.");
  scroll->AddChildScrollWindow(childScrollWindow);
}

//-----------------------------------------------------------------------------
wxScroll* wxWindow::GetScroll(void) { return cScroll; }

//-----------------------------------------------------------------------------
void wxWindow::SetScrollData // Must override if window scrolls
(
 wxScrollData*		scrollData,
 wxWhatScrollData	whatScrollData,
 wxScrollEvent*		e
 )
{
  // Must override if window scrolls
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Mouse methods
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//-----------------------------------------------------------------------------
void wxWindow::CaptureMouse(void)
     /* Allows events only to this window and its children */
     /* I.e., like X-Windows, not like Windows */
{
  if (gMouseWindow != this) {
    if (gMouseWindow) gMouseWindow->ReleaseMouse();
    gMouseWindow = this;
  }
}

//-----------------------------------------------------------------------------
void wxWindow::ReleaseMouse(void)
{
  if (gMouseWindow == this) {
    gMouseWindow = NULL;
  }
}

//-----------------------------------------------------------------------------
/* Forward decl */
Bool doCallPreMouseEvent(wxWindow *in_win, wxWindow *win, wxMouseEvent *evt);

static void SendEnterLeaveEvent(wxWindow *target, int eventtype, wxWindow *evtsrc, wxMouseEvent *evt)
{
  if (!target->IsHidden()) {
    wxMouseEvent *theMouseEvent = new wxMouseEvent(eventtype);
    theMouseEvent->leftDown = evt->leftDown;
    theMouseEvent->middleDown = evt->middleDown;
    theMouseEvent->rightDown = evt->rightDown;
    theMouseEvent->shiftDown = evt->shiftDown;
    theMouseEvent->controlDown = evt->controlDown;
    theMouseEvent->altDown = evt->altDown;
    theMouseEvent->metaDown = evt->metaDown;
    theMouseEvent->timeStamp = evt->timeStamp;
    
    int clientHitX = (int)(evt->x);
    int clientHitY = (int)(evt->y);
    evtsrc->ClientToScreen(&clientHitX, &clientHitY);
    target->ScreenToClient(&clientHitX, &clientHitY);
    theMouseEvent->x = clientHitX;
    theMouseEvent->y = clientHitY;

    if (!doCallPreMouseEvent(target, target, theMouseEvent))		  
      if (!target->IsGray())
	target->OnEvent(theMouseEvent);
  }
}

static void QueueLeaveEvent(wxWindow *target, wxWindow *evtsrc, wxMouseEvent *evt)
{
  EventRecord e;
  
  int clientHitX = (int)(evt->x);
  int clientHitY = (int)(evt->y);
  evtsrc->ClientToScreen(&clientHitX, &clientHitY);
  target->ScreenToClient(&clientHitX, &clientHitY);
  e.message = (long)target;
  e.where.h = clientHitX;
  e.where.v = clientHitY;
  e.what = 42;
  e.when = UNSCALE_TIMESTAMP(evt->timeStamp);
  e.modifiers = ((evt->shiftDown ? shiftKey : 0)
		 + (evt->metaDown ? cmdKey : 0)
		 + (evt->altDown ? optionKey : 0)
		 + ((evt->leftDown || evt->rightDown) ? btnState : 0)
		 + (evt->rightDown ? controlKey : 0));
  
  QueueMrEdEvent(&e);
}

Bool doCallPreMouseEvent(wxWindow *in_win, wxWindow *_win, wxMouseEvent *evt)
{
  if (_win == in_win 
      && (evt->eventType != wxEVENT_TYPE_ENTER_WINDOW)
      && (evt->eventType != wxEVENT_TYPE_LEAVE_WINDOW)) {
    wxWindow *win = _win->EnterLeaveTarget();

    /* Do enter/leave events */
    if (entered != win) {
      wxWindow *p;
      if (entered) {
	/* Leave in current eventspace? */
	int same = (((wxFrame *)win->GetRootFrame())->context
		    == ((wxFrame *)entered->GetRootFrame())->context);
	
	/* Send/queue leave events to non-common ancestors */
	p = entered;
	while (p) {
	  wxWindow *winp = win;
	  // is p an ancestor of win? If so, break.
	  while (winp && (winp != p))
	    winp = winp->GetParent();
	  if (winp == p)
	    break;
	  
	  if (same)
	    SendEnterLeaveEvent(p, wxEVENT_TYPE_LEAVE_WINDOW, win, evt);
	  else
	    QueueLeaveEvent(p, win, evt);
	  p = p->GetParent();
	}
      } else
	p = win->GetRootFrame()->GetParent();
      
      entered = win;
      
      while (1) {
	wxWindow *winp = win;
	while (winp && (winp->GetParent() != p))
	  winp = winp->GetParent();
	if (!winp)
	  break;
	SendEnterLeaveEvent(winp, wxEVENT_TYPE_ENTER_WINDOW, win, evt);
	if (winp == win)
	  break;
	p = winp;
      }
    }
  }

  wxWindow *p = _win->GetParent();
  return ((p && doCallPreMouseEvent(in_win, p, evt)) || _win->PreOnEvent(in_win, evt));
}

static Bool IsCaptureAncestorArea(wxArea *area)
{
  wxChildNode* childWindowNode = area->Windows()->First();
  while (childWindowNode)
    { 
      wxWindow* p = (wxWindow*)childWindowNode->Data(), *w;
      for (w = wxWindow::gMouseWindow; w; w = w->GetParent()) {
	if (w == p)
	  return TRUE;
      }
      
      childWindowNode = childWindowNode->Next();
    }
  
  return FALSE;
}

Bool wxWindow::SeekMouseEventArea(wxMouseEvent *mouseEvent)
{ // For point expressed in parent area c.s., seek deepest sub-window containing it
  Bool result = FALSE;

  if (!IsEnable())
    return FALSE;

  int hitX = (int)(mouseEvent->x - cWindowX); // window c.s.
  int hitY = (int)(mouseEvent->y - cWindowY); // window c.s.

  int capThis = (wxWindow::gMouseWindow == this);
  wxArea* hitArea = NULL;
  wxNode* areaNode = cAreas->Last();
  while (areaNode && !hitArea) {
    if (!capThis) {
      wxArea* area = (wxArea*)areaNode->Data();
      if ((!wxWindow::gMouseWindow && area->WindowPointInArea(hitX, hitY))
	  || (wxWindow::gMouseWindow && IsCaptureAncestorArea(area)))
	hitArea = area;
      else 
	areaNode = areaNode->Previous();
    }
    
    if (hitArea || capThis) {
      wxMouseEvent *areaMouseEvent = new wxMouseEvent(0);
      *areaMouseEvent = *mouseEvent;
      int hitAreaX, hitAreaY;
      if (hitArea) {
	wxMargin hitAreaMargin = hitArea->Margin(this /* hitArea->ParentWindow() */);
	hitAreaX = hitAreaMargin.Offset(Direction::wxLeft);
	hitAreaY = hitAreaMargin.Offset(Direction::wxTop);
      } else
	hitAreaX = hitAreaY = 0;
      areaMouseEvent->x = hitX - hitAreaX; // hit area c.s.
      areaMouseEvent->y = hitY - hitAreaY; // hit area c.s.
      
      if (!capThis) {
	wxChildNode* childWindowNode = hitArea->Windows()->First();
	while (childWindowNode && !result) {
	  wxWindow* childWindow = (wxWindow*)childWindowNode->Data();
	  result = childWindow->SeekMouseEventArea(areaMouseEvent);
	  if (!result)
	    childWindowNode = childWindowNode->Next();
	}
      }
      
      if (!result) {
	if (capThis || (hitArea == ClientArea() && CanAcceptEvent())) {
	  result = TRUE;
	  int clientHitX = (int)(areaMouseEvent->x);
	  int clientHitY = (int)(areaMouseEvent->y);
	  //ClientToLogical(&clientHitX, &clientHitY); // mouseWindow logical c.s.
	  areaMouseEvent->x = clientHitX; // mouseWindow logical c.s.
	  areaMouseEvent->y = clientHitY; // mouseWindow logical c.s.

	  if (wxSubType(__type, wxTYPE_CANVAS)
	      || wxSubType(__type, wxTYPE_PANEL)) {
	    if (areaMouseEvent->ButtonDown())
	      CaptureMouse();
	    else if (gMouseWindow == this
		     && !areaMouseEvent->Dragging())
	      ReleaseMouse();
	  }

	  if (!doCallPreMouseEvent(this, this, areaMouseEvent)) {
	    if (WantsFocus() && areaMouseEvent->ButtonDown()) {
	      wxFrame *fr = GetRootFrame();
	      if (fr)
		fr->SetFocusWindow(this);
	    }

	    /* PreOnEvent could disable the target... */
	    if (!IsGray())
	      OnEvent(areaMouseEvent);
	  }
	}
      }
    }
    
    if (result)
      break;
  }

  /* Frame/dialog: hande all events, even outside the window */	
  if (!result && (__type == wxTYPE_FRAME || __type == wxTYPE_DIALOG_BOX)) {
    wxMouseEvent *areaMouseEvent = new wxMouseEvent(0);
    *areaMouseEvent = *mouseEvent;
    int clientHitX = (int)(areaMouseEvent->x);
    int clientHitY = (int)(areaMouseEvent->y);
    //ClientToLogical(&clientHitX, &clientHitY); // mouseWindow logical c.s.
    areaMouseEvent->x = clientHitX; // mouseWindow logical c.s.
    areaMouseEvent->y = clientHitY; // mouseWindow logical c.s.
    if (!doCallPreMouseEvent(this, this, areaMouseEvent))
      if (!IsGray())
	OnEvent(areaMouseEvent);
  }
  
  return result;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Keyboard methods
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//-----------------------------------------------------------------------------
void wxWindow::SetFocus(void)
{
  if (WantsFocus() && CanAcceptEvent()) {
    wxFrame* rootFrame = GetRootFrame();
    rootFrame->SetFocusWindow(this);
  }
}

//-----------------------------------------------------------------------------
void wxWindow::OnSetFocus(void)
{
}

//-----------------------------------------------------------------------------
void wxWindow::OnKillFocus(void)
{
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Quill methods
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//-----------------------------------------------------------------------------
void wxWindow::SetFont(wxFont* theFont) // mac platform only
{
  font = theFont;
}

//-----------------------------------------------------------------------------
float wxWindow::GetCharHeight(void)
{
  float theCharHeight;
  if (font)
    theCharHeight = font->GetCharHeight();
  else
    theCharHeight =  0.0;
  return theCharHeight;
}

//-----------------------------------------------------------------------------
float wxWindow::GetCharWidth(void)
{
  float theCharWidth;
  if (font)
    theCharWidth = font->GetCharWidth();
  else
    theCharWidth =  0.0;
  return theCharWidth;
}

//-----------------------------------------------------------------------------
void wxWindow::GetTextExtent(const char* string, float* x, float* y, float* descent,
			     float* externalLeading, wxFont* the_font, Bool use16)
{
  if (the_font)
    the_font->GetTextExtent((char *)string, x, y, descent, externalLeading, use16);
  else if (font)
    font->GetTextExtent((char *)string, x, y, descent, externalLeading, use16);
  else {
    *x = -1;
    *y = -1;
    if (descent) *descent = 0.0;
    if (externalLeading) *externalLeading = 0.0;
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Activate methods
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//-----------------------------------------------------------------------------

void wxWindow::Activate(Bool flag) // mac platform only
{
  cActive = flag;
  ShowAsActive(flag);
  wxNode* areaNode = cAreas->First();
  while (areaNode) {
    wxArea* area = (wxArea*)areaNode->Data();
    wxChildNode* childWindowNode = area->Windows()->First();
    while (childWindowNode) {
      wxWindow* childWindow = (wxWindow*)childWindowNode->Data();
      childWindow->Activate(flag);
      childWindowNode = childWindowNode->Next();
    }
    areaNode = areaNode->Next();
  }
  OnActivate(flag);
}

//-----------------------------------------------------------------------------
void wxWindow::ShowAsActive(Bool flag) // mac platform only
{
  // default is to do nothing
}

//-----------------------------------------------------------------------------
void wxWindow::OnActivate(Bool flag) // mac platform only
{
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Other methods
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//-----------------------------------------------------------------------------
void wxWindow::Paint(void)
{ // Called when needs painting
  if (cHidden) return;

  wxNode* areaNode = cAreas->Last();
  while (areaNode) {
    wxArea* area = (wxArea*)areaNode->Data();
    wxChildNode* childWindowNode = area->Windows()->First();
    while (childWindowNode)
      {
	wxWindow* childWindow = (wxWindow*)childWindowNode->Data();
	if (!childWindow->cHidden)
	  childWindow->Paint();
	childWindowNode = childWindowNode->Next();
      }
    areaNode = areaNode->Previous();
  }
}

//-----------------------------------------------------------------------------

// Enabling Logic:  a window (or control) is shown as OS-activated when
// a) the window is enabled (cEnabled == true)
// b) no enclosing window is disabled (internal_gray == 0)
// These are all local properties of windows.
// So, if we imagine this as a state machine, when does this predicate
// go from true to false or false to true? The least error-prone way to
// do this is simply to compute the predicate given above, and see whether
// it changes.

// The OS_Active predicate tells whether, for the current state of the
// window, it should be displayed as "usable" or not.

Bool wxWindow::OS_Active()
{
  return ((internal_gray == 0) && cEnable);
}

void wxWindow::Enable(Bool Flag) 
     /* Disabling blocks mouse and keyboard events, not update events. */
     /* I.e., like Windows, not like X-Windows */
{
  if (!cEnable != !Flag) {
    Bool current = OS_Active();
  
    cEnable = Flag;
    
    ChildrenInternalGray(Flag);
    
    if (current != OS_Active()) {
      ChangeToGray(!OS_Active());
    } 
  }
}

void wxWindow::InternalGray(Bool gray)
{
  Bool current = OS_Active();

  if (gray) {
    internal_gray += 1;
  } else {
    internal_gray -= 1;
  }
  
  ChildrenInternalGray(gray);
  
  if (current != OS_Active()) {
    ChangeToGray(!OS_Active());
  }
}

// the ChangeToGray procedures doesn't check anything:
// it just changes the window to gray or not-gray

void wxWindow::ChangeToGray(Bool gray)
{
  // Refresh();
  
  if (cMacDC->currentUser() == this)
    /* fg pen needs reset: */
    cMacDC->setCurrentUser(NULL);

  if (gray) {
    wxFrame* frame = GetRootFrame();
    if (this == frame->GetFocusWindow()) {
      frame->SetFocusWindow(NULL);
    }
  }
}

Bool wxWindow::IsGray(void)
{
  return !cEnable || internal_gray;
}

void wxWindow::ChildrenInternalGray(Bool gray)
{
  wxChildNode *node;
  
  for (node = GetChildren()->First(); node; node = node->Next()) {
    wxWindow *w = (wxWindow *)(node->Data());
    w->InternalGray(gray);
  }
}

void wxWindow::InitInternalGray()
{
  wxWindow *p;

  p = GetParent();

  if (!p->cEnable || p->internal_gray) {
    internal_gray = p->internal_gray + (p->cEnable ? 0 : 1);
    ChangeToGray(TRUE);
  }
}

void wxWindow::Highlight(Bool on)
{
}

int wxWindow::Track(Point p)
{
  int on = FALSE;
  Rect r = {0, 0, cWindowHeight, cWindowWidth};
  OffsetRect(&r,SetOriginX,SetOriginY);
  
  while (::StillDown()) {
    GetMouse(&p);
    if (!!PtInRect(p, &r) != on) {
      on = !on;
      Highlight(on);
    }
  }
  
  if (on)
    Highlight(FALSE);
  
  return PtInRect(p, &r);
}

void wxWindow::FlushDisplay()
{
  if (cMacDC) {
    CGrafPtr theMacGrafPort = cMacDC->macGrafPort();
    QDFlushPortBuffer(theMacGrafPort, NULL);
  }
}

//-----------------------------------------------------------------------------
void wxWindow::AddWhiteRgn(RgnHandle r)
{
  wxChildNode *node;
  for (node = GetChildren()->First(); node; node = node->Next()) {
    wxWindow *c = (wxWindow *)(node->Data());
    if (!c->cHidden)
      c->AddWhiteRgn(r);
  }
}

//-----------------------------------------------------------------------------
Bool wxWindow::IsEnable(void) { return cEnable; }

Bool wxWindow::CanAcceptEvent(void)
{
  if (!IsEnable())
    return FALSE;
  if (cHidden)
    return FALSE;

  if (!wxWindow::gMouseWindow)
    return TRUE;

  wxWindow *w = this;
  while (w) {
    if (w == wxWindow::gMouseWindow)
      return TRUE;
    w = w->window_parent;
  }

  return FALSE;
}

void wxWindow::Show(Bool v)
{
  v = !v;
  
  if (v == cUserHidden)
    return;
  
  if (window_parent)
    window_parent->GetChildren()->Show(this, !v);
  if (cParentArea)
    cParentArea->Windows()->Show(this, !v);

  cUserHidden = v;
  DoShow(!v);
}

Bool wxWindow::CanShow(Bool v)
{
  if (v && (cUserHidden 
	    || (window_parent && window_parent->IsHidden())))
    return FALSE;

  v = !v;

  if (v == cHidden)
    return FALSE;
  
  return TRUE;
}

void wxWindow::DoShow(Bool v)
{
  if (!CanShow(v))
    return;

  v = !v;

  if (SetCurrentMacDCNoMargin()) {
    // put newClientRect at (SetOriginX, SetOriginY)
    Rect r = { -1, -1, cWindowHeight, cWindowWidth };
    OffsetRect(&r,SetOriginX,SetOriginY);
    
    if (v) {
      MacSetBackground();
      ::EraseRect(&r);
      RestoreNormalBackground(cEraser);
    }
    
    ::InvalWindowRect(GetWindowFromPort(cMacDC->macGrafPort()),&r);
  }

  cHidden = v;

  if (cHidden) {
    /* Check for focus */
    wxFrame* frame = GetRootFrame();
    if (frame) {
      wxWindow *f = frame->GetFocusWindow();
      if (f == this)
	frame->SetFocusWindow(NULL);
    }
  }
  
}

//-----------------------------------------------------------------------------
Direction wxWindow::GetGravitate(void) { return cGravitate; } // mac platform only

//-----------------------------------------------------------------------------
void wxWindow::SetGravitate(Direction direction) { cGravitate = direction; } // mac platform only

//-----------------------------------------------------------------------------
Direction wxWindow::GetJustify(void) { return cJustify; } // mac platform only

//-----------------------------------------------------------------------------
void wxWindow::SetJustify(Direction direction) { cJustify = direction; } // mac platform only

//-----------------------------------------------------------------------------
void wxWindow::DoPeriodicAction(void) // mac platform only
{
  // default is to do nothing
}

//-----------------------------------------------------------------------------
wxCursor* wxWindow::SetCursor(wxCursor* cursor)
{
  if (cursor && !cursor->Ok())
    cursor = wx_cursor;

  wxCursor* old_cursor = wx_cursor;
  if (old_cursor != cursor) {
    wx_cursor = cursor;
    wxTheApp->AdjustCursor();
  }
  return old_cursor;
}

//-----------------------------------------------------------------------------
void wxWindow::DragAcceptFiles(Bool accept) { } // Not implemented

//-----------------------------------------------------------------------------
Bool wxWindow::IsMacWindow(void) { return FALSE; } // mac platform only

//-----------------------------------------------------------------------------
void wxWindow::SetColourMap(wxColourMap* cmap) { }

//-----------------------------------------------------------------------------
Bool wxWindow::PopupMenu(wxMenu *menu, float x, float y)
{
  MenuHandle m = menu->CreateCopy("popup", FALSE);

  int di;
  if (menu->title) {
    int l = strlen(menu->title);
    char *s = new WXGC_ATOMIC char[l + 3];
    s[0] = s[1] = ' ';
    memcpy(s + 2, menu->title, l + 1);
    InsertMenuItem(m, wxC2P(wxItemStripLabel(s)), 0);
    DisableMenuItem(m, 1);
    InsertMenuItem(m, "\p-", 1);
    di = -2;
  } else
    di = 0;

  if (!SetCurrentDC())
    return FALSE;

  ::InsertMenu(m, -1);
  ::CalcMenuSize(m);
  Point pos = {(short)y + SetOriginY, (short)x + SetOriginX};
  LocalToGlobal(&pos);
  long sel = ::PopUpMenuSelect(m, pos.v, pos.h, 0);

  ReleaseCurrentDC();

  int itemId;

  if (!sel) {
    itemId = 0;
  } else {
    int macMenuId = HiWord(sel);
    int macMenuItemNum = LoWord(sel) + di;

    if (macMenuItemNum <= 0) {
      itemId = 0;
    } else {
      wxMenu *theWxMenu;
      
      if (macMenuId == menu->GetMacMenuId())
	theWxMenu = menu;
      else 
	theWxMenu = menu->wxMacFindSubmenu(macMenuId);
      if (!theWxMenu) wxFatalError("No submenu for menu id.");
      
      wxNode* node = theWxMenu->menuItems->Nth(macMenuItemNum - 1); // counting from 0
      if (!node) wxFatalError("No wxNode for Nth menuItem.");
      
      wxMenuItem* theWxMenuItem = (wxMenuItem*) node->Data();
      if (!theWxMenuItem) wxFatalError("No wxMenuItem for wxNode.");
      
      itemId = theWxMenuItem->itemId;
    }
  }

  wxPopupEvent *event = new wxPopupEvent();
  event->menuId = itemId;

  menu->ProcessCommand(event);

  return TRUE;
}

//-----------------------------------------------------------------------------
void wxWindow::SetEraser(wxBrush* eraser) { cEraser = eraser; }

//-----------------------------------------------------------------------------

Bool wxWindow::AdjustCursor(int mouseX, int mouseY)
{
  Bool result = FALSE;

  // For point expressed in parent area c.s., seek deepest sub-window containing it
  int hitX = mouseX - cWindowX; // window c.s.
  int hitY = mouseY - cWindowY; // window c.s.

  if (wxWindow::gMouseWindow == this) {
    wxCursor *c = GetEffectiveCursor();
    wxSetCursor(c);
    return TRUE;
  }

  if (IsHidden())
    return FALSE;

  wxArea* hitArea = NULL;
  wxNode* areaNode = cAreas->Last();
  while (areaNode && !hitArea) {
    wxArea* area = (wxArea*)areaNode->Data();
    if ((!wxWindow::gMouseWindow && area->WindowPointInArea(hitX, hitY))
	|| (wxWindow::gMouseWindow && IsCaptureAncestorArea(area)))
      hitArea = area;
    else areaNode = areaNode->Previous();
  }

  if (hitArea) {
    wxMargin hitAreaMargin = hitArea->Margin(hitArea->ParentWindow());
    int hitAreaX = hitAreaMargin.Offset(Direction::wxLeft);
    int hitAreaY = hitAreaMargin.Offset(Direction::wxTop);
    int areaX = hitX - hitAreaX; // hit area c.s.
    int areaY = hitY - hitAreaY; // hit area c.s.
    wxChildNode* childWindowNode = hitArea->Windows()->First();
    while (childWindowNode && !result) {
      wxWindow* childWindow = (wxWindow*)childWindowNode->Data();
      result = childWindow->AdjustCursor(areaX, areaY);
      if (!result) childWindowNode = childWindowNode->Next();
    }
    
    if (!result) {
      if (hitArea == ClientArea()) {
	result = TRUE;
	wxCursor *c = GetEffectiveCursor();
	wxSetCursor(c);
      }
    }
  }

  return result;
}

Bool wxWindow::WantsFocus(void)
{
  return FALSE;
}

//-----------------------------------------------------------------------------
// tom: fettig@dfki.uni-sb.de
// to be called by any window, which could have the focus!!
void wxWindow::DestroyFocus() 
{
  wxFrame* root = GetRootFrame();
  if (root->GetFocusWindow()==this)
    root->SetFocusWindow(NULL);	
}


Bool wxWindow::PreOnEvent(wxWindow *, wxMouseEvent *)
{
  return FALSE;
}

Bool wxWindow::PreOnChar(wxWindow *, wxKeyEvent *)
{
  return FALSE;
}

void wxWindow::ForEach(void (*foreach)(wxWindow *w, void *data), void *data)
{
  wxChildNode *node, *next;
  for (node = GetChildren()->First(); node; node = next) {
    wxWindow *c = (wxWindow *)(node->Data());
    next = node->Next();
    if (c) {
      c->ForEach(foreach, data);
    }
  }
  
  foreach(this, data);
}

wxCursor *wxWindow::GetEffectiveCursor(void)
{
  wxWindow *p = this;
  if (!wx_cursor && cGrandcursor) {
    p = p->GetParent();
    if (p)
      p = p->GetParent();
  }
  while (p && !p->wx_cursor)
    p = p->GetParent();
  if (p)
    return p->wx_cursor;
  else
    return NULL;
}

Bool wxWindow::GetsFocus()
{
  return WantsFocus();
}

//-----------------------------------------------------------------------------

void wxWindow::MaybeMoveControls()
{
  // do nothing. This method is overridden for windows that have control children.
}


wxWindow *wxWindow::EnterLeaveTarget()
{
  return this;
}


void wxFlushMacDisplay(void)
{
  QDFlushPortBuffer(wxGetGrafPtr(), NULL);  
}
