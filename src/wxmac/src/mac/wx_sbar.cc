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
#ifndef WX_CARBON
# include <Windows.h>
#endif

pascal void	TrackActionProc(ControlHandle theControl,short partCode);

//	Functions which are called from external scope, but in turn invoke
//	DocWindow methods. These really could be moved t

static ControlActionUPP
TrackActionProcUPP = NewControlActionUPP(TrackActionProc);


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
  InitDefaults(function);
  
  if (label)
    label = wxItemStripLabel(label);

  //////////////////////////////////////////
  // do platform stuff
  //////////////////////////////////////////
  SetCurrentMacDC();
  CGrafPtr theMacGrafPort = cMacDC->macGrafPort();
  int clientWidth = ClientArea()->Width();
  int clientHeight = ClientArea()->Height();
  Rect boundsRect = {0, 0, clientHeight, clientWidth};
  OffsetRect(&boundsRect,SetOriginX,SetOriginY);
  const Bool drawNow = TRUE;
  const short offValue = 0;
  const short minValue = 0;
  const short maxValue = 0;
  long refCon = (long)this;

  if (CreateScrollBarControl(GetWindowFromPort(theMacGrafPort), &boundsRect, 
			     offValue, minValue, maxValue, 1,
			     TRUE, TrackActionProcUPP, &cMacControl))
    cMacControl = NULL;
      
  CheckMemOK(cMacControl);

  SetControlReference(cMacControl, (long)this); /* for TrackControl */
  
  ::EmbedControl(cMacControl, GetRootControl());

  if (GetParent()->IsHidden())
    DoShow(FALSE);
  
  cGrandcursor = TRUE;
}

//-----------------------------------------------------------------------------
void wxScrollBar::InitDefaults(wxFunction function)
{
  Callback(function);

  cStyle = (cStyle & wxHSCROLL ? wxHSCROLL : wxVSCROLL); // kludge
  wxScrollData* scrollData = new wxScrollData;
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
      Rect controlRect = *GetControlBounds(cMacControl,NULL);
      PenState oldPenState;
      ::GetPenState(&oldPenState);
      ::PenNormal();
      Rect r = controlRect;
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
  if (show)
    ::ShowControl(cMacControl);
  else
    ::HideControl(cMacControl);
  wxWindow::DoShow(show);
}

//-----------------------------------------------------------------------------
void wxScrollBar::Enable(Bool enable)
{
  if ((enable != cEnable) && cActive && cMacControl) {
    SetCurrentDC();
    if (enable) {
      ActivateControl(cMacControl);
    }
    else {
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
pascal void TrackActionProc(ControlHandle theControl, short thePart);
pascal void TrackActionProc(ControlHandle theControl, short thePart)
{
  wxScrollBar* scrollBar = (wxScrollBar*) GetControlReference(theControl);
  if (scrollBar) scrollBar->TrackAction(thePart);
}

//-----------------------------------------------------------------------------
void wxScrollBar::OnEvent(wxMouseEvent *event) // mac platform only
{
  if (event->LeftDown()) {
    SetCurrentDC();
    
    int startH, startV;
    event->Position(&startH, &startV); // frame c.s.
    
    Point startPt = {startV + SetOriginY, startH + SetOriginX}; // frame c.s.
    int thePart = ::TestControl(cMacControl, startPt);
    if (thePart) {
      if (thePart == kControlIndicatorPart) {
	if (!StillDown() || TrackControl(cMacControl, startPt, TrackActionProcUPP)) {
	  Bool horizontal = cStyle & wxHSCROLL;
	  wxWhatScrollData positionScrollData =
	    (horizontal ? wxWhatScrollData::wxPositionH : wxWhatScrollData::wxPositionV);
	  int newPosition = GetValue();
	  wxScrollEvent *e = new wxScrollEvent();
	  e->direction = (horizontal ? wxHORIZONTAL : wxVERTICAL);
	  e->pos = GetValue();
	  e->moveType = wxEVENT_TYPE_SCROLL_THUMBTRACK;
	  
	  cScroll->SetScrollData(newPosition, positionScrollData, e);
	}
      } else {
	if (StillDown())
	  ::TrackControl(cMacControl, startPt, TrackActionProcUPP);
      }
    }
  }
}

//-----------------------------------------------------------------------------
void wxScrollBar::TrackAction(short part) // mac platform only
{
  if (part && cScroll) {
    Bool horizontal = cStyle & wxHSCROLL;

    wxScrollData* scrollData = cScroll->GetScrollData();
    int scrollsPerPage = scrollData->GetValue
      (horizontal ? wxWhatScrollData::wxPageW : wxWhatScrollData::wxPageH);
    int maxv = GetMaxValue();
    int mtype = 0;
    
    int delta = 0;
    switch (part)
      {
      case kControlUpButtonPart: delta = -1; mtype = wxEVENT_TYPE_SCROLL_LINEUP; break;
      case kControlDownButtonPart: delta = 1; mtype = wxEVENT_TYPE_SCROLL_LINEDOWN; break;
      case kControlPageUpPart: delta = -scrollsPerPage; mtype = wxEVENT_TYPE_SCROLL_PAGEUP; break;
      case kControlPageDownPart: delta = scrollsPerPage; mtype = wxEVENT_TYPE_SCROLL_PAGEDOWN; break;
      }

    int newPosition = GetValue() + delta;
    if (newPosition < 0) newPosition = 0;
    if (newPosition > maxv) newPosition = maxv;

    wxWhatScrollData positionScrollData =
      (horizontal ? wxWhatScrollData::wxPositionH : wxWhatScrollData::wxPositionV);
    SetValue(newPosition);
    wxScrollEvent *e = new wxScrollEvent();
    e->direction = (horizontal ? wxHORIZONTAL : wxVERTICAL);
    e->pos = GetValue();
    e->moveType = mtype;
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
  // if (this == iniatorWindow) return;

  SetCurrentDC();

  Bool horizontal = cStyle & wxHSCROLL;

  wxWhatScrollData sizeScrollData =
    (horizontal ? wxWhatScrollData::wxSizeW : wxWhatScrollData::wxSizeH);
  if ((long)whatScrollData & (long)sizeScrollData)
    {
      int newSize = scrollData->GetValue(sizeScrollData);
      SetMaxValue(newSize);
    }

  wxWhatScrollData postionScrollData =
    (horizontal ? wxWhatScrollData::wxPositionH : wxWhatScrollData::wxPositionV);
  if ((long)whatScrollData & (long)postionScrollData)
    {
      int newPosition = scrollData->GetValue(postionScrollData);
      SetValue(newPosition);
    }

  wxWhatScrollData pageScrollData =
    (horizontal ? wxWhatScrollData::wxPageW : wxWhatScrollData::wxPageH);
  if ((long)whatScrollData & (long)pageScrollData)
    {
      int newPage = scrollData->GetValue(pageScrollData);
      ::SetControlViewSize(cMacControl, newPage);
    }

  
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Sizing methods
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//-----------------------------------------------------------------------------
void wxScrollBar::OnClientAreaDSize(int dW, int dH, int dX, int dY) // mac platform only
{
  SetCurrentDC();

  Rect bounds;
  GetControlBounds(cMacControl, &bounds);

  dX = bounds.left - SetOriginX;
  dY = bounds.top - SetOriginY;

  if (dW || dH) {
    int clientWidth, clientHeight;
    GetClientSize(&clientWidth, &clientHeight);
    ::SizeControl(cMacControl, clientWidth, clientHeight);
  }

  if (dX || dY)
    ::MoveControl(cMacControl, SetOriginX, SetOriginY);
}


wxWindow *wxScrollBar::EnterLeaveTarget()
{
  return GetParent();
}

wxCursor *wxScrollBar::GetEffectiveCursor(void)
{
  return GetParent()->GetParent()->GetEffectiveCursor();
}
