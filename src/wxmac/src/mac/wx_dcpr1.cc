///////////////////////////////////////////////////////////////////////////////
// File:	wx_dcpr1.cc
// Purpose:	Print Canvas device context implementation (Macintosh version) (part 1)
// Author:	Lou Birk (copied from wx_dccan)
// Created:	1995
// Updated:	
///////////////////////////////////////////////////////////////////////////////

static const char sccsid[] = "%W% %G%";

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <QuickDraw.h>
#include <Printing.h>
#include "wx_dcpr.h"
//#include "wx_canvs.h"
#include "wx_privt.h"
#include "wx_utils.h"
#include "wx_mac_utils.h"
#include "wx_area.h"

// Declarations local to this file
#define YSCALE(y) (yorigin - (y))
#define     wx_round(a)    (int)((a)+.5)

extern CGrafPtr wxMainColormap;

void InstallColor(wxColour &c, int fg, Bool Colour);

//-----------------------------------------------------------------------------
// Default constructor
//-----------------------------------------------------------------------------
#ifdef OS_X
wxPrinterDC::wxPrinterDC(wxWindow *parent) : wxCanvasDC()
{
  PMPrintSettings printSettings;
  PMPageFormat pageFormat;
  OSErr err;
  
  ok = true;
  
  if (wxPMBegin() != noErr) {
    ok = false;
    return;
  }

  if (PMCreatePrintSettings(&printSettings) != noErr) {
    ok = false;
    return;
  }
  
  if (PMDefaultPrintSettings(printSettings) != noErr) {
    ok = false;
    PMDisposePrintSettings(printSettings);
    return;
  }
  
  if (PMCreatePageFormat(&pageFormat) != noErr) {
    ok = false;
    PMDisposePrintSettings(printSettings);
    return;
  }
  
  if (PMDefaultPageFormat(pageFormat) != noErr) {
    ok = false;
    PMDisposePrintSettings(printSettings);
    PMDisposePageFormat(pageFormat);
    return;
  }

  close_handle = 1;
  Create(printSettings,pageFormat);
}
#else
wxPrinterDC::wxPrinterDC(wxWindow *parent) : wxCanvasDC()
{
  THPrint pr;

  ok = TRUE;

  wxPrOpen();

  pr = (THPrint)NewHandleClear(sizeof(TPrint));

  PrintDefault(pr);
  if (PrError() != fnfErr) {
    if (!PrJobDialog(pr)) {
      ok = FALSE;
    }
    if (PrError())
      ok = FALSE;
  } else
    ok = FALSE;

  if (ok) {
    close_handle = 1;
    Create(pr);
  } else {
    DisposeHandle((Handle)pr);
  }
}
#endif


//-----------------------------------------------------------------------------
#ifdef OS_X
wxPrinterDC::wxPrinterDC(PMPrintSettings printSettings, PMPageFormat pageFormat) : wxCanvasDC()
#else
wxPrinterDC::wxPrinterDC(THPrint pData) : wxCanvasDC()
#endif
{
  ok = true;
  close_handle = 0;
  wxPrOpen();
#ifdef OS_X
  Create(pData);
#else
  Create(printSettings,pageFormat);
#endif
}

#ifdef OS_X
void wxPrinterDC::Create(PMPrintSettings printSettings, PMPageFormat pageFormat)
#else
void wxPrinterDC::Create(THPrint pData)
#endif
{
    GrafPtr grafPtr;
    __type = wxTYPE_DC_PRINTER;

#ifdef OS_X
    cPrintSettings = printSettings;
    cPageFormat = pageFormat;
    
    if (PMOpenDocument(cPrintSettings,cPageFormat,&cPrintContext) != noErr) {
      ok = false;
      return;
    }  
    
    GrafPtr grafPtr;
    
    if (PMGetGrafPtr(cPrintContext, &grafPtr) != noErr) {
      ok = false;
      return;
    }

#else    
    prRecHandle = pData;

    prPort = PrOpenDoc(prRecHandle, 0, 0);

    if (PrError()) {
      PrCloseDoc(prPort);
      PrClose();
      ok = FALSE;
      return;
    }

    grafPtr = &(prPort->gPort);
#endif

    cMacDC = new wxMacDC((CGrafPtr)grafPtr);
    CGrafPtr theMacGrafPort = cMacDC->macGrafPort();
	
    cMacDoingDrawing = FALSE;

    clipping = FALSE;
    selected_pixmap = NULL;

    current_reg = NULL ;
    onpaint_reg = NULL ;

    min_x = 0; min_y = 0;
    max_x = 0; max_y = 0;

  pixmapWidth = (**prRecHandle).prInfo.rPage.right;
  pixmapHeight = (**prRecHandle).prInfo.rPage.bottom;

  device = wxDEVICE_CANVAS;
  font = wxNORMAL_FONT;

  logical_origin_x = 0;
  logical_origin_y = 0;

  device_origin_x = 0;
  device_origin_y = 0;

  logical_scale_x = 1.0;
  logical_scale_y = 1.0;

  user_scale_x = 1.0;
  user_scale_y = 1.0;

  mapping_mode = MM_TEXT;

  title = NULL;


  ok = TRUE;
  current_pen_join = -1 ;
  current_pen_cap = -1 ;
  current_pen_nb_dash = -1 ;
  current_pen_dash = NULL ;
  current_stipple = NULL ;

  Colour = wxColourDisplay();

  current_pen = NULL;
  current_brush = NULL;
  current_text_foreground = new wxColour(wxBLACK);
//  current_text_background = NULL;
  SetBrush(wxWHITE_BRUSH);
  SetPen(wxBLACK_PEN);

  int clientWidth, clientHeight;
  //the_canvas->GetClientSize(&clientWidth, &clientHeight);
  clientWidth = pixmapWidth;
  clientHeight = pixmapHeight; // paper
  Rect paintRect = {0, 0, clientHeight, clientWidth};
  SetPaintRegion(&paintRect);

}

//-----------------------------------------------------------------------------
wxPrinterDC::~wxPrinterDC(void)
{
  if (ok) {
    PrCloseDoc(prPort);
    if (close_handle)
      DisposeHandle((Handle)prRecHandle);
  }

  wxPrClose();
}

//-----------------------------------------------------------------------------
Bool wxPrinterDC::StartDoc(char *message) { return TRUE; }

//-----------------------------------------------------------------------------
void wxPrinterDC::EndDoc(void) { }

//-----------------------------------------------------------------------------
void wxPrinterDC::StartPage(void)
{
  if (prPort)
    PrOpenPage(prPort, 0); 
}

//-----------------------------------------------------------------------------
void wxPrinterDC::EndPage(void)
{
  if (prPort)
    PrClosePage(prPort);
}
