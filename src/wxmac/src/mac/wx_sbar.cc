///////////////////////////////////////////////////////////////////////////////
// File:	wx_sbar.cc
// Purpose:	Macintosh Scrollbar implementation (Macintosh version)
// Author:	Bill Hale
// Created:	1994
// Updated:	
// Copyright:  (c) 1993-94, AIAI, University of Edinburgh. All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////

#include "wx_item.h"
#include "wx_sbar.h"
#include "wxScroll.h"
#include "wx_utils.h"
#include "wx_mac_utils.h"
#include "wxMacDC.h"
#include "wx_stdev.h"
#include "wx_area.h"
#include "wx_frame.h"
#include "wx_het.h"

//	Functions which are called from external scope, but in turn invoke
//	DocWindow methods. These really could be moved t

static pascal void TrackActionProc(ControlHandle theControl, short thePart);
static ControlActionUPP TrackActionProcUPP = NewControlActionUPP(TrackActionProc);


//=============================================================================
// Public constructors
//=============================================================================

//-----------------------------------------------------------------------------
wxScrollBar::wxScrollBar // Constructor (given parentArea)
(
 wxArea*		parentArea,
 wxFunction	function,
 char*		label,
 int 		x,
 int			y,
 int			width,
 int			height,
 long		style,
 char*		windowName,
 WXTYPE		objectType
 ) :
 wxWindow (windowName, parentArea, x, y, width, height, style)
{
  CreateWxScrollBar(function, label);
}

//-----------------------------------------------------------------------------
wxScrollBar::wxScrollBar // Constructor (given parentWindow)
(
 wxWindow*	parentWindow,
 wxFunction	function,
 char*		label,
 int 		x,
 int			y,
 int			width,
 int			height,
 long		style,
 char*		windowName,
 WXTYPE		objectType
 ) :
 wxWindow (windowName, parentWindow, x, y, width, height, style)
{
  CreateWxScrollBar(function, label);
}

//=============================================================================
// Public destructor
//=============================================================================

//-----------------------------------------------------------------------------
wxScrollBar::~wxScrollBar(void)
{
  ::DisposeControl(cMacControl);
}

//=============================================================================
// Private methods
//=============================================================================

//-----------------------------------------------------------------------------
void wxScrollBar::CreateWxScrollBar // common constructor initialization
(
 wxFunction function,
 char* label
 )
{
  const short offValue = 0;
  const short minValue = 0;
  const short maxValue = 0;
  CGrafPtr theMacGrafPort;
  int clientWidth, clientHeight;
  wxArea *carea;
  Rect boundsRect = {0, 0, 0, 0};
  
  InitDefaults(function);
  
  if (label)
    label = wxItemStripLabel(label);

  //////////////////////////////////////////
  // do platform stuff
  //////////////////////////////////////////
  SetCurrentMacDC();
  theMacGrafPort = cMacDC->macGrafPort();
  carea = ClientArea();
  clientWidth = carea->Width();
  clientHeight = carea->Height();
  boundsRect.bottom = clientHeight;
  boundsRect.right = clientWidth;
  OffsetRect(&boundsRect,SetOriginX,SetOriginY);

  if (CreateScrollBarControl(GetWindowFromPort(theMacGrafPort), &boundsRect, 
			     offValue, minValue, maxValue, 1,
			     TRUE, TrackActionProcUPP, &cMacControl))
    cMacControl = NULL;
      
  CheckMemOK(cMacControl);

  {
    void *rc;
    rc = WRAP_SAFEREF(this);
    refcon = rc;
  }
  SetControlReference(cMacControl, (long)refcon); /* for TrackControl */
  
  ::EmbedControl(cMacControl, GetRootControl());

  {
    wxWindow *p;
    p = GetParent();
    if (p->IsHidden())
      DoShow(FALSE);
  }

  cGrandcursor = TRUE;
}

//-----------------------------------------------------------------------------
void wxScrollBar::InitDefaults(wxFunction function)
{
  wxScrollData* scrollData;

  Callback(function);

  cStyle = (cStyle & wxHSCROLL ? wxHSCROLL : wxVSCROLL); // kludge
  scrollData = new wxScrollData;
  cScroll = new wxScroll(this, scrollData);
}

//-----------------------------------------------------------------------------
void wxScrollBar::ChangeColour(void)
{
}

//-----------------------------------------------------------------------------
char* wxScrollBar::GetLabel()
{
  Str255	pLabel;

  ::GetControlTitle(cMacControl, pLabel);
  ::CopyPascalStringToC(pLabel, wxBuffer);
  return wxBuffer;
}

//-----------------------------------------------------------------------------
void wxScrollBar::SetLabel(char* label)
{
}

//-----------------------------------------------------------------------------
void wxScrollBar::SetValue(int val)
{
  SetCurrentDC();
  ::SetControl32BitValue(cMacControl, val);
}

//-----------------------------------------------------------------------------
int wxScrollBar::GetValue(void)
{
  return ::GetControl32BitValue(cMacControl);
}

//-----------------------------------------------------------------------------
void wxScrollBar::SetMaxValue(int maxValue)
{
  SetCurrentDC();
  ::SetControl32BitMaximum(cMacControl, maxValue);
}

//-----------------------------------------------------------------------------
int wxScrollBar::GetMaxValue(void)
{
  return ::GetControl32BitMaximum(cMacControl);
}


//-----------------------------------------------------------------------------
void wxScrollBar::Paint(void)
{
  if (cHidden) return;
  
  SetCurrentDC();
  // GRW
  if (IsControlVisible(cMacControl))
    {
      ::Draw1Control(cMacControl);
    }
  else
    {
      // Draw outline of hidden scrollbar (since we're clipping DrawGrowIcon)
      Rect controlRect;
      PenState oldPenState;
      Rect r;
      
      controlRect = *GetControlBounds(cMacControl,NULL);
      ::GetPenState(&oldPenState);
      ::PenNormal();
      r = controlRect;
      OffsetRect(&r,SetOriginX,SetOriginY);
      ::FrameRect(&r);
      ::SetPenState(&oldPenState);
    }
  // GRW
}

//-----------------------------------------------------------------------------
void wxScrollBar::DoShow(Bool show)
{
  if (!CanShow(show)) return;

  SetCurrentDC();
  if (show) {
    ::ShowControl(cMacControl);
  } else {
    ::HideControl(cMacControl);
  }
  wxWindow::DoShow(show);
}

//-----------------------------------------------------------------------------
void wxScrollBar::Enable(Bool enable)
{
  if ((enable != cEnable) && cActive && cMacControl) {
    SetCurrentDC();
    if (enable) {
      ActivateControl(cMacControl);
    } else {
      DeactivateControl(cMacControl);
    }
  }
  wxWindow::Enable(enable);
}

//-----------------------------------------------------------------------------
void wxScrollBar::ShowAsActive(Bool flag)
{
  if (cEnable && cMacControl) {
    SetCurrentDC();
    if (flag) {
      ActivateControl(cMacControl);
    } else {
      DeactivateControl(cMacControl);
    }
  }
}

//-----------------------------------------------------------------------------
static pascal void TrackActionProc(ControlHandle theControl, short thePart)
{
  wxScrollBar* scrollBar;
  void *rc;

  rc = (void *)GetControlReference(theControl);
  scrollBar = (wxScrollBar*)GET_SAFEREF(rc);
  if (scrollBar) {
    /* Must queue callbacks only: */
    scrollBar->TrackAction(thePart);

    while (wxHETYield(scrollBar) && StillDown()) { }
  }

#ifdef MZ_PRECISE_GC
  /* Restore variable stack. */
  GC_variable_stack = (void **)__gc_var_stack__[0];
#endif
}

//-----------------------------------------------------------------------------
void wxScrollBar::OnEvent(wxMouseEvent *event) // mac platform only
{
  if (event->LeftDown()) {
    int startH, startV;
    Point startPt;
    int thePart;

    SetCurrentDC();
    
    event->Position(&startH, &startV); // frame c.s.
    
    startPt.v = startV + SetOriginY; // frame c.s.
    startPt.h = startH + SetOriginX;
    thePart = ::TestControl(cMacControl, startPt);
    if (thePart) {
      int down;
      down = StillDown();
	
      if (thePart == kControlIndicatorPart) {
	if (!down || wxHETTrackControl(cMacControl, startPt, TrackActionProcUPP)) {
	  Bool horizontal = cStyle & wxHSCROLL;
	  wxWhatScrollData positionScrollData =
	    (horizontal ? wxWhatScrollData::wxPositionH : wxWhatScrollData::wxPositionV);
	  int newPosition;
	  wxScrollEvent *e;

	  newPosition = GetValue();
	   e = new wxScrollEvent();
	  e->direction = (horizontal ? wxHORIZONTAL : wxVERTICAL);
	  e->pos = GetValue();
	  e->moveType = wxEVENT_TYPE_SCROLL_THUMBTRACK;
	  
	  cScroll->SetScrollData(newPosition, positionScrollData, e);
	}
      } else {
	if (down) {
	  ::wxHETTrackControl(cMacControl, startPt, TrackActionProcUPP);
	}
      }
    }
  }
}

//-----------------------------------------------------------------------------
void wxScrollBar::TrackAction(short part) // mac platform only
{
  /* This code must not call Scheme. */

  if (part && cScroll) {
    Bool horizontal = cStyle & wxHSCROLL;
    wxScrollData* scrollData;
    int scrollsPerPage;
    int maxv;
    int mtype, delta;
    int newPosition;
    wxWhatScrollData positionScrollData;
    wxScrollEvent *e;

    scrollData = cScroll->GetScrollData();
    scrollsPerPage = scrollData->GetValue(horizontal 
					  ? wxWhatScrollData::wxPageW
					  : wxWhatScrollData::wxPageH);
    maxv = GetMaxValue();
    mtype = 0;
    
    delta = 0;
    switch (part)
      {
      case kControlUpButtonPart: delta = -1; mtype = wxEVENT_TYPE_SCROLL_LINEUP; break;
      case kControlDownButtonPart: delta = 1; mtype = wxEVENT_TYPE_SCROLL_LINEDOWN; break;
      case kControlPageUpPart: delta = -scrollsPerPage; mtype = wxEVENT_TYPE_SCROLL_PAGEUP; break;
      case kControlPageDownPart: delta = scrollsPerPage; mtype = wxEVENT_TYPE_SCROLL_PAGEDOWN; break;
      }

    newPosition = GetValue() + delta;
    if (newPosition < 0) newPosition = 0;
    if (newPosition > maxv) newPosition = maxv;

    positionScrollData = (horizontal 
			  ? wxWhatScrollData::wxPositionH 
			  : wxWhatScrollData::wxPositionV);
    SetValue(newPosition);
    e = new wxScrollEvent();
    e->direction = (horizontal ? wxHORIZONTAL : wxVERTICAL);
    e->pos = GetValue();
    e->moveType = mtype;
    /* This code must not call Scheme, either, though it can
       install callbacks: */
    cScroll->SetScrollData(newPosition, positionScrollData, e);

    SetCurrentDC(); // must reset cMacDC (kludge)
  }
}

//-----------------------------------------------------------------------------
void wxScrollBar::SetScrollData // adjust scrollBar to match scroll data setting
(
 wxScrollData*		scrollData,
 wxWhatScrollData	whatScrollData,
 wxScrollEvent*		e
 )
{
  /* This function must not call Scheme. */

  // if (this == iniatorWindow) return;
  Bool horizontal = cStyle & wxHSCROLL;
  wxWhatScrollData sizeScrollData;
  wxWhatScrollData postionScrollData;
  wxWhatScrollData pageScrollData;

  SetCurrentDC();

  sizeScrollData = (horizontal ? wxWhatScrollData::wxSizeW : wxWhatScrollData::wxSizeH);
  if ((long)whatScrollData & (long)sizeScrollData)
    {
      int newSize;
      newSize = scrollData->GetValue(sizeScrollData);
      SetMaxValue(newSize);
    }

  postionScrollData =
    (horizontal ? wxWhatScrollData::wxPositionH : wxWhatScrollData::wxPositionV);
  if ((long)whatScrollData & (long)postionScrollData)
    {
      int newPosition;
      newPosition = scrollData->GetValue(postionScrollData);
      SetValue(newPosition);
    }

  pageScrollData = (horizontal ? wxWhatScrollData::wxPageW : wxWhatScrollData::wxPageH);
  if ((long)whatScrollData & (long)pageScrollData)
    {
      int newPage;
      newPage = scrollData->GetValue(pageScrollData);
      ::SetControlViewSize(cMacControl, newPage);
    }

  
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Sizing methods
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//-----------------------------------------------------------------------------
void wxScrollBar::OnClientAreaDSize(int dW, int dH, int dX, int dY) // mac platform only
{
  Rect bounds;

  SetCurrentDC();

  GetControlBounds(cMacControl, &bounds);

  dX = bounds.left - SetOriginX;
  dY = bounds.top - SetOriginY;

  if (dW || dH) {
    int clientWidth, clientHeight;
    GetClientSize(&clientWidth, &clientHeight);
    ::SizeControl(cMacControl, clientWidth, clientHeight);
  }

  if (dX || dY) {
    ::MoveControl(cMacControl, SetOriginX, SetOriginY);
  }
}


wxWindow *wxScrollBar::EnterLeaveTarget()
{
  return GetParent();
}

wxCursor *wxScrollBar::GetEffectiveCursor(void)
{
  wxWindow *p;
  p = GetParent();
  p = p->GetParent();
  return p->GetEffectiveCursor();
}
