///////////////////////////////////////////////////////////////////////////////
// File:	wx_dcmem.cc
// Purpose:	Memory device context implementation (Macintosh version)
// Author:	Bill Hale
// Created:	1994
// Updated:	
// Copyright:  (c) 1993-94, AIAI, University of Edinburgh. All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "wx_dcmem.h"
#include "wx_utils.h"
#include "wx_canvs.h"

/* 
   A wxMemoryDC is a pointer to a bitmap, which is an offscreen GWorld. 
   */
extern CGrafPtr wxMainColormap;

wxMemoryDC::wxMemoryDC(Bool ro)
{
  Init(NULL);
  
  __type = wxTYPE_DC_MEMORY;
  device = wxDEVICE_PIXMAP;

  read_only = ro;
  
  ok = FALSE;
  title = NULL;

  selected_pixmap = NULL;
}

wxMemoryDC::~wxMemoryDC(void)
{
  if (selected_pixmap) {
    if (!read_only) {
      selected_pixmap->selectedInto = NULL;
      selected_pixmap->selectedIntoDC = 0;
    }
    selected_pixmap = NULL;
  }
  
  if (cMacDC) {
    DELETE_OBJ cMacDC;
    cMacDC = NULL;
  }
}

void wxMemoryDC::SelectObject(wxBitmap *bitmap)
{
  if (selected_pixmap == bitmap) {
    // set cMacDC ??
    return;
  }
  if (!read_only) {
    if (bitmap && bitmap->selectedIntoDC)
      // This bitmap is selected into a different memoryDC
      return;
  }
  
  if (selected_pixmap) {
    if (!read_only) {
      selected_pixmap->selectedInto = NULL;
      selected_pixmap->selectedIntoDC = 0;
    }
  }

  if (cMacDC) {
    DELETE_OBJ cMacDC;
    cMacDC = NULL;
  }
  ok = FALSE;
  selected_pixmap = bitmap;
  if (bitmap == NULL) {	// deselect a bitmap
    pixmapWidth = 0;
    pixmapHeight = 0;
    return;
  }
  if (!read_only) {
    bitmap->selectedInto = this;
    bitmap->selectedIntoDC = -1;
  }
  pixmapWidth = bitmap->GetWidth();
  pixmapHeight = bitmap->GetHeight();
  if (bitmap->Ok()) {
    if (bitmap->x_pixmap) {
      cMacDC = new wxMacDC((CGrafPtr)bitmap->x_pixmap);
      // bitmap->DrawMac(0, 0);
      ok = TRUE;
      
      Colour = (bitmap->GetDepth() > 1);

      SetCurrentDC();
      InstallColor(current_background_color, FALSE);
      PenMode(patCopy);
      ToolChanged(kNoTool);
      ReleaseCurrentDC();
    }
  }
}

wxBitmap* wxMemoryDC::GetObject()
{
  return selected_pixmap;
}
