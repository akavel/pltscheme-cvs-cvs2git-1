/*
 * File:	wx_dccan.h
 * Purpose:	Canvas device context declaration
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

#ifndef wx_dccanh
#define wx_dccanh

#include "wx_gdi.h"
#include "wb_dccan.h"

class wxCanvasDC: public wxbCanvasDC
{
 public:
  wxCanvasDC(void);

  void TryColour(wxColour *src, wxColour *dest);

  // Create a DC corresponding to a canvas
  wxCanvasDC(wxCanvas *canvas);
  void GetClippingBox(float *x,float *y,float *w,float *h) ;
  /* MATTHEW: [8] */
  void GetClippingRegion(float *x, float *y, float *width, float *height);

  void GetSize(float *width, float *height);

  Bool GCBlit(float xdest, float ydest, float width, float height,
	      wxBitmap *source, float xsrc, float ysrc);

  ~wxCanvasDC(void);
};

// This class specific to Windows 3.1
class wxScreenDC: public wxCanvasDC
{
 public:
  // Create a DC representing the whole screen
  wxScreenDC(void);
  ~wxScreenDC(void);
};

#endif // wx_dccan

