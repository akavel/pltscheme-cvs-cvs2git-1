///////////////////////////////////////////////////////////////////////////////
// File:	wx_dc.cc
// Purpose:	Device context implementation (Macintosh version)
// Author:	Bill Hale
// Created:	1994
// Updated:	
// Copyright:  (c) 1993-94, AIAI, University of Edinburgh. All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef OS_X
# include <QuickDraw.h>
#endif
#include "wx_gdi.h"
#include "wx_dc.h"

//-----------------------------------------------------------------------------
wxDC::wxDC(void)
     //-----------------------------------------------------------------------------
{
  cMacDC = NULL;
  cMacCurrentTool = kNoTool;
}

//-----------------------------------------------------------------------------
wxDC::~wxDC(void)
     //-----------------------------------------------------------------------------
{
}

//-----------------------------------------------------------------------------
void wxDC::wxMacDrawPoint(int x1, int y1)
     //-----------------------------------------------------------------------------
{
  MoveTo(x1 + SetOriginX, y1 + SetOriginY);
  Line(0, 0);
}

//-----------------------------------------------------------------------------
void wxDC::wxMacDrawLine(int x1, int y1, int x2, int y2)
     //-----------------------------------------------------------------------------
{
  MoveTo(x1 + SetOriginX, y1 + SetOriginY);
  LineTo(x2 + SetOriginX, y2 + SetOriginY);
}

void wxDC::SetTextForeground(wxColour *colour)
{
  wxbDC::SetTextForeground(colour);
  ToolChanged(kTextTool);
}

void wxDC::SetTextBackground(wxColour *colour)
{
  wxbDC::SetTextBackground(colour);
  ToolChanged(kTextTool);
}

void wxDC::SetBackgroundMode(int mode)
{
  wxbDC::SetBackgroundMode(mode);
  ToolChanged(kTextTool);
}

void wxDC::ToolChanged(wxMacToolType tool)
{
  if ((tool == kNoTool) || (cMacCurrentTool == tool))
    cMacCurrentTool = kNoTool;
}
