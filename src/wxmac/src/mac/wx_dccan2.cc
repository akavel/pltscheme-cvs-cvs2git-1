///////////////////////////////////////////////////////////////////////////////
// File:	wx_dccan2.cc
// Purpose:	Canvas device context implementation (Macintosh version) (part 2)
// Author:	Bill Hale
// Created:	1994
// Updated:	
// Copyright:  (c) 1993-94, AIAI, University of Edinburgh. All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////

static const char sccsid[] = "%W% %G%";

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef OS_X
  #include <QDOffscreen.h>
#endif
#include "wx_dccan.h"
#include "wx_utils.h"
#include "wx_canvs.h"
#include "wx_rgn.h"
#include "wx_privt.h"

//static PixMapHandle	bdiag,
//		cdiag,
//		fdiag,
//		cross,
//		horiz,
//		verti;

extern CGrafPtr wxMainColormap;

// constant to convert radian to degree
#define RAD2DEG 57.2957795131

//-----------------------------------------------------------------------------
void wxCanvasDC::Clear(void)
//-----------------------------------------------------------------------------
{
	if (!Ok()) return;
 
        SetCurrentDC();
	wxMacSetCurrentTool(kNoTool);

	int w, h;
	if (canvas)
		canvas->GetVirtualSize(&w, &h);
	else {
		w = pixmapWidth;
		h = pixmapHeight;
	}

	Rect theClearRect = {0, 0, h, w};
	::EraseRect(&theClearRect); // SET-ORIGIN FLAGGED
}

void wxCanvasDC::GetSize(float *width, float *height)
{
	if (canvas) {
		int w, h;
		canvas->GetVirtualSize(&w, &h);
		*width = w;
		*height = h;
	} else {
		*width = pixmapWidth;
		*height = pixmapHeight;
	}
}

//-----------------------------------------------------------------------------
void wxCanvasDC::CrossHair(float x, float y)
//-----------------------------------------------------------------------------
{
   if (!current_pen || current_pen->GetStyle() == wxTRANSPARENT)
     return;

	SetCurrentDC();

	wxMacSetCurrentTool(kPenTool);
	int xx = XLOG2DEV(x);
	int yy = YLOG2DEV(y);
	float ww, hh;
	GetSize(&ww, &hh) ;
	wxMacDrawLine(0, yy, ww, yy);
	wxMacDrawLine(xx, 0, xx, hh);
}

//-----------------------------------------------------------------------------
void wxCanvasDC::FloodFill(float x, float y, wxColour *col, int style)
//=============================================================================
{
	SetCurrentDC();
}

//-----------------------------------------------------------------------------
Bool wxCanvasDC::GetPixel(float x, float y, wxColour *col)
//=============================================================================
{
	if (!Ok()) return FALSE;
 
        RGBColor rgb;

	SetCurrentDC();

	GetCPixel(XLOG2DEV(x), YLOG2DEV(y), &rgb); // SET-ORIGIN FLAGGED
	col->Set(rgb.red >> 8, rgb.green >> 8, rgb.blue >> 8);

	return TRUE;
}

//-----------------------------------------------------------------------------
void wxCanvasDC::SetPixel(float x, float y, wxColour *col)
//=============================================================================
{
	if (!Ok()) return;
 
        RGBColor rgb;

	SetCurrentDC();

	if ((col->Red() == 255) && (col->Blue() == 255) && (col->Green() == 255)) {
	  rgb.red = 0xFFFF;
	  rgb.green = 0xFFFF;
	  rgb.blue = 0xFFFF;
	} else {
	  rgb.red = col->Red() << 8;
	  rgb.green = col->Green() << 8;
	  rgb.blue = col->Blue() << 8;
	}
	SetCPixel(XLOG2DEV(x), YLOG2DEV(y), &rgb); // SET-ORIGIN FLAGGED
}

//-----------------------------------------------------------------------------
void wxCanvasDC::IntDrawLine(int x1, int y1, int x2, int y2)
//-----------------------------------------------------------------------------
{
    DrawLine(x1, y1, x2, y2);
}

//-----------------------------------------------------------------------------
void wxCanvasDC::DrawLine(float x1, float y1, float x2, float y2)
//-----------------------------------------------------------------------------
{
    if (!Ok()) return;
 
    if (!current_pen || current_pen->GetStyle() == wxTRANSPARENT)
      return;

	SetCurrentDC();
	wxMacSetCurrentTool(kPenTool);
	wxMacDrawLine(XLOG2DEV(x1), YLOG2DEV(y1), XLOG2DEV(x2), YLOG2DEV(y2));
	CalcBoundingBox(x1, y1);
	CalcBoundingBox(x2, y2);
}

//-----------------------------------------------------------------------------
static void FillWithStipple(wxDC *dc, wxRegion *r, wxBrush *brush)
{
  float x, y, w, h, bw, bh;
  int xstart, xend, ystart, yend, i, j;
  wxRegion *old;

  wxBitmap *bm = brush->GetStipple();
  int style = brush->GetStyle();
  wxColour *c = brush->GetColour();

  old = dc->GetClippingRegion();
  if (old) r->Intersect(old);

  r->BoundingBox(&x, &y, &w, &h);
  bw = bm->GetWidth();
  bh = bm->GetHeight();

  x = dc->LogicalToDeviceX(x);
  y = dc->LogicalToDeviceY(y);
  w = dc->LogicalToDeviceXRel(w);
  h = dc->LogicalToDeviceYRel(h);
  
  xstart = (int)floor(x / bw);
  xend = (int)floor((x + w + bw - 0.00001) / bw);

  ystart = (int)floor(y / bh);
  yend = (int)floor((y + h + bh - 0.00001) / bh);

  dc->SetClippingRegion(r);

  for (i = xstart; i < xend; i++)
    for (j = ystart; j < yend; j++)
      dc->Blit(dc->DeviceToLogicalX(i * bw), 
               dc->DeviceToLogicalY(j * bh), 
               dc->DeviceToLogicalXRel(bw), 
               dc->DeviceToLogicalYRel(bh),
               bm, 0, 0, style, c);

  dc->SetClippingRegion(old);
}

wxRegion *wxCanvasDC::BrushStipple()
{
  if (current_brush) {
    wxBitmap *bm = current_brush->GetStipple();
    if (bm && bm->Ok())
      return new wxRegion(this);
  }
  return NULL;
}

void wxCanvasDC::PaintStipple(wxRegion *r)
{
  FillWithStipple(this, r, current_brush);
}

//-----------------------------------------------------------------------------
void wxCanvasDC::DrawArc(float x,float y,float w,float h,float start,float end)
{
    if (!Ok()) return;
 
    SetCurrentDC();

    wxRegion *rgn;
    if (rgn = BrushStipple()) {
      rgn->SetArc(x, y, w, h, start, end);
      PaintStipple(rgn);
    }

    int xx = XLOG2DEV(x); int yy = XLOG2DEV(y);
    int ww = XLOG2DEVREL(w); int hh = XLOG2DEVREL(h);
    
    double degrees1, degrees2;

    degrees1 = start * RAD2DEG;
    degrees2 = end * RAD2DEG;
    
    /* Convert to QD angles for clockwise arc: */
    int alpha1 = (int)(-degrees2 + 90) % 360;
    if (alpha1 < 0)
      alpha1 += 360;
    int alpha2 = (int)(-degrees1 + 90) % 360;
    if (alpha2 < 0)
      alpha2 += 360;
    
    /* Alpha2 should be positive difference: */  
    alpha2 -= alpha1;
    if (alpha2 < 0)
      alpha2 += 360;

	Rect rect;
	rect.left = xx;
	rect.top = yy;
	rect.right = rect.left + ww;
	rect.bottom = rect.top + hh;

    if (current_brush && current_brush->GetStyle() != wxTRANSPARENT) {
      if (!rgn) {
        wxMacSetCurrentTool(kBrushTool);
        PaintArc(&rect, alpha1, alpha2); // SET-ORIGIN FLAGGED
      }
    }
    if (current_pen && current_pen->GetStyle() != wxTRANSPARENT) {
      wxMacSetCurrentTool(kPenTool);
      FrameArc(&rect, alpha1, alpha2); // SET-ORIGIN FLAGGED
    }
	
	CalcBoundingBox(x, y);
    CalcBoundingBox(x + w, y + h);
}

//-----------------------------------------------------------------------------
void wxCanvasDC::DrawPoint(float x, float y)
//-----------------------------------------------------------------------------
{
    if (!Ok()) return;
 
    if (!current_pen || current_pen->GetStyle() == wxTRANSPARENT)
      return;

	SetCurrentDC();
	wxMacSetCurrentTool(kPenTool);
	wxMacDrawPoint(XLOG2DEV(x), YLOG2DEV(y));
	CalcBoundingBox(x, y);
}

//-----------------------------------------------------------------------------
void wxCanvasDC::DrawPolygon(int n, wxPoint points[],
								float xoffset, float yoffset, int fillStyle)
{
	if (!Ok()) return;
 
        SetCurrentDC();
	if (n <= 0) return;

    wxRegion *rgn;
    if (rgn = BrushStipple()) {
      rgn->SetPolygon(n, points, xoffset, yoffset, fillStyle);
      PaintStipple(rgn);
    }



	Point *xpoints1 = new Point[n+1];
	for (int i = 0; i < n; i++)
	{
		xpoints1[i].h = XLOG2DEV(points[i].x + xoffset);
		xpoints1[i].v = YLOG2DEV(points[i].y + yoffset);
		CalcBoundingBox(points[i].x + xoffset, points[i].y + yoffset);
	}

	// Close figure
	xpoints1[n].h = xpoints1[0].h;
	xpoints1[n].v = xpoints1[0].v;

	PolyHandle thePolygon = OpenPoly();
	MoveTo(xpoints1[0].h, xpoints1[0].v); // SET-ORIGIN FLAGGED
	for (int j = 1; j <= n; j++)
	{
		LineTo(xpoints1[j].h, xpoints1[j].v); // SET-ORIGIN FLAGGED
	}
	ClosePoly();

	if (current_brush && current_brush->GetStyle() != wxTRANSPARENT)
	{
	    if (!rgn) {
		if (cMacCurrentTool != kBrushTool) wxMacSetCurrentTool(kBrushTool);
		PaintPoly(thePolygon); // SET-ORIGIN FLAGGED
	    }
	}

	if (current_pen && current_pen->GetStyle() != wxTRANSPARENT)
	{
		wxMacSetCurrentTool(kPenTool);
		FramePoly(thePolygon); // SET-ORIGIN FLAGGED
	}

	delete[] xpoints1;
	KillPoly(thePolygon);
}

//-----------------------------------------------------------------------------
void wxCanvasDC::DrawLines(int n, wxIntPoint points[], int xoffset, int yoffset)
{
	if (!Ok()) return;
 
        if (n <= 0) return;
	if (current_pen && current_pen->GetStyle() != wxTRANSPARENT)
	{
		SetCurrentDC();
		wxMacSetCurrentTool(kPenTool);
	
		Point *xpoints = new Point[n];
	
		for (int i = 0; i < n; i++)
		{
			xpoints[i].h = XLOG2DEV(points[i].x + xoffset);
			xpoints[i].v = YLOG2DEV(points[i].y + yoffset);
			CalcBoundingBox(points[i].x + xoffset, points[i].y + yoffset); // WCH: not in original??
		}
	
		PolyHandle thePolygon = OpenPoly();
		MoveTo(xpoints[0].h, xpoints[0].v); // SET-ORIGIN FLAGGED
		for (int j = 1; j < n; j++)
		{
			LineTo(xpoints[j].h, xpoints[j].v); // SET-ORIGIN FLAGGED
		}
		ClosePoly();
	
		FramePoly(thePolygon); // SET-ORIGIN FLAGGED
	
		delete[] xpoints;
		KillPoly(thePolygon);
	}
}

//-----------------------------------------------------------------------------
void wxCanvasDC::DrawLines(int n, wxPoint points[], float xoffset, float yoffset)
{
	if (!Ok()) return;
 
        if (n <= 0) return;
	if (current_pen && current_pen->GetStyle() != wxTRANSPARENT)
	{
		SetCurrentDC();
		wxMacSetCurrentTool(kPenTool);
	
		Point *xpoints = new Point[n];
	
		for (int i = 0; i < n; i++)
		{
			xpoints[i].h = XLOG2DEV(points[i].x + xoffset);
			xpoints[i].v = YLOG2DEV(points[i].y + yoffset); // WCH: original mistype "h" for "v"
			CalcBoundingBox(points[i].x + xoffset, points[i].y + yoffset); // WCH: not in original??
		}
	
		PolyHandle thePolygon = OpenPoly();
		MoveTo(xpoints[0].h, xpoints[0].v); // SET-ORIGIN FLAGGED
		for (int j = 1; j < n; j++)
		{
			LineTo(xpoints[j].h, xpoints[j].v); // SET-ORIGIN FLAGGED
		}
		ClosePoly();
	
		FramePoly(thePolygon); // SET-ORIGIN FLAGGED
	
		delete[] xpoints;
		KillPoly(thePolygon);
	}
}

//-----------------------------------------------------------------------------
void wxCanvasDC::DrawRectangle(float x, float y, float width, float height)
//-----------------------------------------------------------------------------
{
    if (!Ok()) return;
 
    SetCurrentDC();
	
    wxRegion *rgn;
    if (rgn = BrushStipple()) {
      rgn->SetRectangle(x, y, width, height);
      PaintStipple(rgn);
    }

	int top = YLOG2DEV(y);
	int left = XLOG2DEV(x);
	int bottom = YLOG2DEV(y + height);
	int right = XLOG2DEV(x + width);
	Rect theRect = {top, left, bottom, right};
	if (current_brush && current_brush->GetStyle() != wxTRANSPARENT)
	{
	    if (!rgn) {
		wxMacSetCurrentTool(kBrushTool);
		PaintRect(&theRect); // SET-ORIGIN FLAGGED
	    }
	}

	if (current_pen && current_pen->GetStyle() != wxTRANSPARENT)
	{
		wxMacSetCurrentTool(kPenTool);
		FrameRect(&theRect); // SET-ORIGIN FLAGGED
	}
	CalcBoundingBox(x, y);
	CalcBoundingBox(x + width, y + height);
}

//-----------------------------------------------------------------------------
void wxCanvasDC::DrawRoundedRectangle
					(float x, float y, float width, float height, float radius)
{
    if (!Ok()) return;
 
    SetCurrentDC();
	
    wxRegion *rgn;
    if (rgn = BrushStipple()) {
      rgn->SetRoundedRectangle(x, y, width, height, radius);
      PaintStipple(rgn);
    }

	if (radius < 0.0) {
	  float w = width;
	  if (height < w)
	    w = height;
	  radius = (-radius) * w;
	}
    
	int phys_radius = XLOG2DEVREL(radius);

	int phys_rwidth = phys_radius * 2;
	int phys_rheight = phys_rwidth;

	int top = YLOG2DEV(y);
	int left = XLOG2DEV(x);
	int bottom = YLOG2DEV(y + height);
	int right = XLOG2DEV(x + width);
	Rect theRect = {top, left, bottom, right};

	if (current_brush && current_brush->GetStyle() != wxTRANSPARENT)
	{
	    if (!rgn) {
		wxMacSetCurrentTool(kBrushTool);
		PaintRoundRect(&theRect, phys_rwidth, phys_rheight); // SET-ORIGIN FLAGGED
	    }
	}

	if (current_pen && current_pen->GetStyle() != wxTRANSPARENT)
	{
		wxMacSetCurrentTool(kPenTool);
		FrameRoundRect(&theRect, phys_rwidth, phys_rheight); // SET-ORIGIN FLAGGED
	}

	CalcBoundingBox(x, y);
	CalcBoundingBox(x + width, y + height);
}

//-----------------------------------------------------------------------------
void wxCanvasDC::DrawEllipse(float x, float y, float width, float height)
{
     if (!Ok()) return;
     
     SetCurrentDC();
	
     wxRegion *rgn;
    if (rgn = BrushStipple()) {
      rgn->SetEllipse(x, y, width, height);
      PaintStipple(rgn);
    }

	int top = YLOG2DEV(y);
	int left = XLOG2DEV(x);
	int bottom = YLOG2DEV(y + height);
	int right = XLOG2DEV(x + width);
	Rect theRect = {top, left, bottom, right};
	if (current_brush && current_brush->GetStyle() != wxTRANSPARENT)
	{
	    if (!rgn) {
		wxMacSetCurrentTool(kBrushTool);
		PaintOval(&theRect); // SET-ORIGIN FLAGGED
	    }
	}

	if (current_pen && current_pen->GetStyle() != wxTRANSPARENT)
	{
		wxMacSetCurrentTool(kPenTool);
		FrameOval(&theRect); // SET-ORIGIN FLAGGED
	}
	CalcBoundingBox(x, y);
	CalcBoundingBox(x + width, y + height);
}

/*-----------------------------------
	Ideally, Blit() should munged its args and call the Mac ToolBox 
	CopyBits() function. 
    We also have to be aware that the 'source' arg could actually be a
	wxMemoryDC and we may have to switch GWorlds. 
		I.e if (this->device != source->device)
*/
Bool wxCanvasDC::Blit(float xdest, float ydest, float width, float height,
                wxBitmap *source, float xsrc, float ysrc, int rop, wxColour *c)
{
	if (!Ok() || !source->Ok()) return FALSE;

	SetCurrentDC();
	
	if (source->GetDepth() == 1) {
	  wxMacSetCurrentTool(kColorBlitTool);
	  if (rop == wxSOLID) BackColor(whiteColor);
	  if (c)
	    InstallColor(c, TRUE);
	  else
	    ForeColor(blackColor);
	} else {
	  wxMacSetCurrentTool(kBlitTool);
	  rop = wxSTIPPLE;
	}
	Bool theResult = FALSE;

	if (pixmap)
	{
		int mode;
		switch (rop)
		{       case wxXOR:  
				mode = srcXor; 
				break;
			case wxSOLID: 
				mode = srcOr; 
				break;
			case wxSTIPPLE: /* = opaque */
			default:
				mode = srcCopy;
				break;
		}

		int ixsrc = (int)floor(xsrc);
		int iysrc = (int)floor(ysrc);
		
		if (ixsrc > source->GetWidth())
		  return TRUE;
		if (iysrc > source->GetHeight())
		  return TRUE;

		if (iysrc + height > source->GetHeight())
		  height = source->GetHeight() - iysrc;
		if (ixsrc + width > source->GetWidth())
		  width = source->GetWidth() - ixsrc;

		int h = YLOG2DEVREL(height);
		int w = XLOG2DEVREL(width);
		int x = XLOG2DEV(xdest);
		int y = YLOG2DEV(ydest);
		
		Rect srcr = {iysrc, ixsrc, iysrc + (int)height, ixsrc + (int)width};
		Rect destr = {y, x, y+h, x+w };
		
		CGrafPtr theMacGrafPort = cMacDC->macGrafPort();

                const BitMap *dstbm;
                PixMapHandle destpixh;
                if (! IsPortColor(theMacGrafPort)) {
                    destpixh = NULL;
                    dstbm = GetPortBitMapForCopyBits(theMacGrafPort); // SET-ORIGIN FLAGGED
                } else {
                    destpixh = GetPortPixMap(theMacGrafPort);
                    dstbm = (BitMap *)(* destpixh);
                }

		// Lock PixMaps
		PixMapHandle srpixh = pixmap = ::GetGWorldPixMap(source->x_pixmap);
		// ::LockPixels(srpixh);

		::CopyBits((BitMap *)(*srpixh), dstbm, &srcr, &destr, mode, NULL); // SET-ORIGIN FLAGGED

		// ::UnlockPixels(srpixh);
		CalcBoundingBox(xdest, ydest);
		CalcBoundingBox(xdest + width, ydest + height);
		theResult = TRUE;
	}

  	return theResult;
}

Bool wxCanvasDC::GCBlit(float xdest, float ydest, float width, float height,
			wxBitmap *source, float xsrc, float ysrc)
{
  /* Non-allocating (i.e. no collectable allocation) Blit. Looks like
     the normal one will work. */

  return Blit(xdest, ydest, width, height, source, xsrc, ysrc, wxSTIPPLE, NULL);
}

void wxCanvasDC::TryColour(wxColour *src, wxColour *dest)
{
  if (!Ok()) return;

  SetCurrentDC();

  if (Colour) {
	RGBColor pixel = src->pixel;
	
	Index2Color(Color2Index(&pixel), &pixel);
	
	dest->Set(pixel.red >> 8, pixel.green >> 8, pixel.blue >> 8); 
  } else {
	unsigned char red = src->Red();
	unsigned char blue = src->Blue();
	unsigned char green = src->Green();
	Bool isWhiteColour =
		(red == (unsigned char )255 &&
		 blue == (unsigned char)255 &&
		 green == (unsigned char)255);
	if (isWhiteColour)
		dest->Set(255, 255, 255);
	else
		dest->Set(0, 0, 0);
  }
}
