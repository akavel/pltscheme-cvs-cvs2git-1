/********************************************************/
/*                       Regions                        */
/********************************************************/

#ifndef wxPI
# define wxPI 3.141592653589793
#endif

#define CAIRO_DEV ((cairo_t *)target)

#ifdef wx_mac
typedef struct {
  CGMutablePathRef path;
  CGMutablePathRef *paths;
  Bool *oes;
  int npaths, apaths;
} PathTarget;
# define CGPATH ((PathTarget *)target)->path
# define PathTargetPath_t CGMutablePathRef
# define CGXFORM (&current_xform)
# define PATHPATH ((CGMutablePathRef)target)
#endif

#ifdef wx_msw
typedef struct {
  GraphicsPath *path;
  GraphicsPath **paths;
  int npaths, apaths;
} PathTarget;
# define GP ((PathTarget *)target)->path
# define PathTargetPath_t GraphicsPath*
# define CURRENT_GP current_path
# define PATH_GP ((GraphicsPath *)target)
#endif

wxRegion::wxRegion(wxDC *_dc, wxRegion *r, Bool _no_prgn)
{
  dc = _dc;
  is_ps = wxSubType(dc->__type, wxTYPE_DC_POSTSCRIPT);
  locked = 0;
 
#ifdef wx_msw
  rgn = NULL;
#endif
#ifdef wx_x
  rgn = NULL;
#endif
#ifdef wx_mac
  rgn = NULL;
#endif
  prgn = NULL;
  no_prgn = _no_prgn;
  if (r) Union(r);
}

wxRegion::~wxRegion()
{
  Cleanup();
}

void wxRegion::Cleanup()
{  
#ifdef wx_msw
  if (rgn) {
    DeleteObject(rgn);
    rgn = NULL;
  }
#endif
#ifdef wx_x
  if (rgn) {
    XDestroyRegion(rgn);
    rgn = NULL;
  }
#endif
#ifdef wx_mac
  if (rgn) {
    DisposeRgn(rgn);
    rgn = NULL;
  }
#endif
  if (!no_prgn) {
    prgn = NULL;
# if defined(wx_mac) || defined(wx_msw)
    if (paths) {
      int i;
      for (i = 0; i < npaths; i++) {
#  ifdef wx_mac
	CGPathRelease(paths[i]);
#  endif
#  ifdef wx_msw
	wxGPathRelease(paths[i]);
#  endif
      }
      paths = NULL;
#  ifdef wx_mac
      oes = NULL;
#  endif
    }
# endif
  }
}

void wxRegion::SetRectangle(double x, double y, double width, double height)
{
  double xw, yh;
  int ix, iy, iw, ih;

  Cleanup();

  if (!no_prgn) {
    prgn = new wxRectanglePathRgn(dc, x, y, width, height);
  }

  xw = x + width;
  yh = y + height;
  x = dc->FLogicalToDeviceX(x);
  y = dc->FLogicalToDeviceY(y);
  xw = dc->FLogicalToDeviceX(xw);
  width = xw - x;
  yh = dc->FLogicalToDeviceY(yh);
  height = yh - y;

  if (is_ps) {
    /* So bitmap-based region is right */
    height = -height;
    y  = -y;
  }

  ix = (int)floor(x);
  iy = (int)floor(y);
  iw = ((int)floor(x + width)) - ix;
  ih = ((int)floor(y + height)) - iy;

#ifdef wx_msw
  rgn = CreateRectRgn(ix, iy, ix + iw, iy + ih);
#endif
#ifdef wx_x
  {
    XRectangle r;
    rgn = XCreateRegion();
    r.x = ix;
    r.y = iy;
    r.width = iw;
    r.height = ih;
    XUnionRectWithRegion(&r, rgn, rgn);
  }
#endif
#ifdef wx_mac
  rgn = NewRgn();
  SetRectRgn(rgn, ix, iy, ix + iw, iy + ih);
#endif
}

void wxRegion::SetRoundedRectangle(double x, double y, double width, double height, double radius)
{
#ifdef wx_xt
  wxRegion *lt, *rt, *lb, *rb, *w, *h, *r;
#endif
#if defined(wx_msw) || defined(wx_mac)
  double xw, yh;
  int ix, iy, iw, ih;
  int xradius, yradius;
#endif

  Cleanup();

  // A negative radius value is interpreted to mean
  // 'the proportion of the smallest X or Y dimension'
  if (radius < 0.0) {
    double smallest = 0.0;
    if (width < height)
      smallest = width;
    else
      smallest = height;
    radius = (double)(- radius * smallest);
  } else
    radius = dc->FLogicalToDeviceXRel(radius);

  if (!no_prgn) {
    prgn = new wxRoundedRectanglePathRgn(dc, x, y, width, height, radius);
  }

#ifdef wx_x
  lt = new wxRegion(dc, NULL, TRUE);
  rt = new wxRegion(dc, NULL, TRUE);
  lb = new wxRegion(dc, NULL, TRUE);
  rb = new wxRegion(dc, NULL, TRUE);
  w = new wxRegion(dc, NULL, TRUE);
  h = new wxRegion(dc, NULL, TRUE);

  lt->SetEllipse(x, y, 2 * radius, 2 * radius);
  rt->SetEllipse(x + width - 2 * radius, y, 2 * radius, 2 * radius);
  rb->SetEllipse(x + width - 2 * radius, y + height - 2 * radius, 2 * radius, 2 * radius);
  lb->SetEllipse(x, y + height - 2 * radius, 2 * radius, 2 * radius);

  w->SetRectangle(x, y + radius, width, height - 2 * radius);
  h->SetRectangle(x + radius, y, width - 2 * radius, height);

  r = lt;
  r->Union(rt);
  r->Union(lb);
  r->Union(rb);
  r->Union(w);
  r->Union(h);

  /* A little hack: steal rgn from r: */
  rgn = r->rgn;
  r->rgn = NULL;
#else
  /* Windows and Mac */
  xw = x + width;
  yh = y + height;
  x = dc->FLogicalToDeviceX(x);
  y = dc->FLogicalToDeviceY(y);
  width = dc->FLogicalToDeviceX(xw) - x;
  height = dc->FLogicalToDeviceY(yh) - y;
  xradius = (int)(dc->FLogicalToDeviceXRel(radius));
  yradius = (int)(dc->FLogicalToDeviceYRel(radius));

  ix = (int)floor(x);
  iy = (int)floor(y);
  iw = ((int)floor(x + width)) - ix;
  ih = ((int)floor(y + height)) - iy;

  if (is_ps) {
    /* So bitmap-based region is right */
    height = -height;
    y = -y;
  }

# ifdef wx_msw
  rgn = CreateRoundRectRgn(ix, iy, ix + iw, iy + ih, xradius, yradius); // SET-ORIGIN FLAGGED
# endif
# ifdef wx_mac
  /* This code uses the current port. We don't know what the current
     port might be, so we have to pick one to be sure that QuickDraw
     is allowed. */
  {
    CGrafPtr savep;
    GDHandle savegd;
    
    ::GetGWorld(&savep, &savegd);  
    ::SetGWorld(wxGetGrafPtr(), GetMainDevice());

    rgn = NewRgn();
    OpenRgn();
    {
      Rect r2;
      SetRect(&r2, ix, iy, ix + iw, iy + ih);
      FrameRoundRect(&r2, xradius, yradius); // SET-ORIGIN FLAGGED
      CloseRgn(rgn);
    }

    ::SetGWorld(savep, savegd);
  }    
# endif
#endif
}

void wxRegion::SetEllipse(double x, double y, double width, double height)
{
  double xw, yh;
#if defined(wx_msw) || defined(wx_mac)
  int ix, iy, iw, ih;
#endif

  Cleanup();

  if (!no_prgn) {
    prgn = new wxArcPathRgn(dc, x, y, width, height, 0, 2 * wxPI);
  }

  xw = x + width;
  yh = y + height;
  x = dc->FLogicalToDeviceX(x);
  y = dc->FLogicalToDeviceY(y);
  width = dc->FLogicalToDeviceX(xw) - x;
  height = dc->FLogicalToDeviceY(yh) - y;

  if (is_ps) {
    /* So bitmap-based region is right */
    height = -height;
    y = -y;
  }

#if defined(wx_msw) || defined(wx_mac)
  ix = (int)floor(x);
  iy = (int)floor(y);
  iw = ((int)floor(x + width)) - ix;
  ih = ((int)floor(y + height)) - iy;
#endif

#ifdef wx_msw
  rgn = CreateEllipticRgn(ix, iy, ix + iw, iy + ih);
#endif
#ifdef wx_mac
  /* This code uses the current port. We don't know what the current
     port might be, so we have to pick one to be sure that QuickDraw
     is allowed. */
  {
    CGrafPtr savep;
    GDHandle savegd;
    
    ::GetGWorld(&savep, &savegd);  
    ::SetGWorld(wxGetGrafPtr(), GetMainDevice());

    rgn = NewRgn();
    OpenRgn();
    {
      Rect r;
      SetRect(&r, ix, iy, ix + iw, iy + ih);
      FrameOval(&r);
      CloseRgn(rgn);
    }

    ::SetGWorld(savep, savegd);
  }
#endif

#ifdef wx_x
  {
    int npoints;
    XPoint *p;
    p = wxEllipseToPolygon(width, height, x, y, &npoints);
    rgn = XPolygonRegion(p, npoints - 1, WindingRule);
  }
#endif
}

#ifdef wx_x
# define POINT XPoint
#endif
#ifdef wx_mac
# define POINT MyPoint
  typedef struct { int x, y; } MyPoint;
#endif

typedef struct { double x, y; } FPoint;

void wxRegion::SetPolygon(int n, wxPoint points[], double xoffset, double yoffset, int fillStyle, int delta)
{
  POINT *cpoints;
  FPoint *fpoints;
  int i, v;
  double vf;

  Cleanup();

  if (n < 2)
    return;

  if (!no_prgn) {
    prgn = new wxPolygonPathRgn(dc, n, points, xoffset, yoffset, fillStyle);
  }

  cpoints = new POINT[n];
  fpoints = (is_ps ? new FPoint[n] : (FPoint *)NULL);
  for (i = 0; i < n; i++) {
    v = dc->LogicalToDeviceX(points[i+delta].x + xoffset);
    cpoints[i].x = v;
    v = dc->LogicalToDeviceY(points[i+delta].y + yoffset);
    cpoints[i].y = v;
    if (fpoints) {
      vf = dc->FLogicalToDeviceX(points[i+delta].x + xoffset);
      fpoints[i].x = vf;
      vf = dc->FLogicalToDeviceY(points[i+delta].y + yoffset);
      fpoints[i].y = vf;
    }
  }

  if (is_ps) {
    /* So bitmap-based region is right */
    for (i = 0; i < n; i++) {
      cpoints[i].y = -cpoints[i].y;
    }
  }

#ifdef wx_msw
  rgn = CreatePolygonRgn(cpoints, n, (fillStyle == wxODDEVEN_RULE) ? ALTERNATE : WINDING);
#endif
#ifdef wx_x
  rgn = XPolygonRegion(cpoints, n, (fillStyle == wxODDEVEN_RULE) ? EvenOddRule : WindingRule);
#endif
#ifdef wx_mac
  /* This code uses the current port. We don't know what the current
     port might be, so we have to pick one to be sure that QuickDraw
     is allowed. */
  {
    CGrafPtr savep;
    GDHandle savegd;
    
    ::GetGWorld(&savep, &savegd);  
    ::SetGWorld(wxGetGrafPtr(), GetMainDevice());

    rgn = NewRgn();
    OpenRgn();
    MoveTo(cpoints[0].x, cpoints[0].y);
    for (i = 0; i < n; i++) {
      LineTo(cpoints[i].x, cpoints[i].y);
    }
    LineTo(cpoints[0].x, cpoints[0].y);
    CloseRgn(rgn);

    ::SetGWorld(savep, savegd);
  }
#endif
}

void wxRegion::SetPath(wxPath *p, double xoffset, double yoffset, int fillStyle)
{
  double **ptss, xs, ys;
  int *lens, cnt, i, total_cnt, j, k;
  wxPoint *a;

  Cleanup();

  if (!no_prgn) {
    prgn = new wxPathPathRgn(dc, p, xoffset, yoffset, fillStyle);
    no_prgn = 1;
  }

  dc->GetUserScale(&xs, &ys);
  cnt = p->ToPolygons(&lens, &ptss, xs, ys);

  if (!cnt)
    return;
  
  total_cnt = 0;
  for (i = 0; i < cnt; i++) {
    total_cnt += (lens[i] / 2);
  }

#ifdef MZ_PRECISE_GC
  a = (wxPoint *)GC_malloc_atomic(sizeof(wxPoint) * total_cnt);
#else
  a = new wxPoint[total_cnt];
#endif

  for (i = 0, k = 0; i < cnt; i++) {
    for (j = 0; j < lens[i]; j += 2) {
      a[k].x = ptss[i][j] + xoffset;
      a[k].y = ptss[i][j+1] + yoffset;
      k++;
    }
  }

  if (cnt == 1) {
    SetPolygon(total_cnt, a, xoffset, yoffset, fillStyle, 0);
  } else {
    for (i = 0, k = 0; i < cnt; i++) {
      j = (lens[i] / 2);
      if (i == 0)
	SetPolygon(j, a, xoffset, yoffset, fillStyle, k);
      else {
	wxRegion *r;
	r = new wxRegion(dc, NULL, 1);
	r->SetPolygon(j, a, xoffset, yoffset, fillStyle, k);
	Xor(r);
	DELETE_OBJ r;
      }
      k += j;
    }
  }      
  
  no_prgn = 0;
}

void wxRegion::SetArc(double x, double y, double w, double h, double start, double end)
{
  wxRegion *r;
  static double pi;
  int saw_start = 0, saw_end = 0, closed = 0;
  double cx, cy;
  wxPoint *a;
  int n;
  char save_no_prgn;

#ifdef MZ_PRECISE_GC
  a = (wxPoint *)GC_malloc_atomic(sizeof(wxPoint) * 20);
#else
  a = new wxPoint[20];
#endif

  save_no_prgn = no_prgn;
  if (!no_prgn) {
    prgn = new wxArcPathRgn(dc, x, y, w, h, start, end);
    no_prgn = 1;
  }

  SetEllipse(x, y, w, h);

  if (start == end) return;

  r = new wxRegion(dc, NULL, TRUE);

  if (!pi)
    pi = 2 * asin((double)1.0);

  start = fmod((double)start, 2*pi);
  end = fmod((double)end, 2*pi);
  if (start < 0)
    start += 2*pi;
  if (end < 0)
    end += 2*pi;

  cx = x + w/2;
  cy = y + h/2;

  a[0].x = ((w+2) / 2) * cos(end) + cx;
  a[0].y = ((h+2) / 2) * (-sin(end)) + cy;

  a[1].x = cx;
  a[1].y = cy;

  a[2].x = ((w+2) / 2) * cos(start) + cx;
  a[2].y = ((h+2) / 2) * (-sin(start)) + cy;

  n = 3;

  if (!saw_start && (start < (pi / 2)))
    saw_start = 1;
  if (!saw_end && (end > start) && (end < (pi / 2)))
    saw_end = 1;
  if (saw_start && !closed) {
    a[n].x = x + w + 2;
    a[n++].y = y - 2;
  }
  if (saw_start && !saw_end) {
    a[n].x = cx;
    a[n++].y = y - 2;
  } else
    closed = saw_start;

  if (!saw_start && (start < pi))
    saw_start = 1;
  if (!saw_end && (end > start) && (end < pi))
    saw_end = 1;
  if (saw_start && !closed) {
    a[n].x = x - 2;
    a[n++].y = y - 2;
  }
  if (saw_start && !saw_end) {
    a[n].x = x - 2;
    a[n++].y = cy;
  } else
    closed = saw_start;

  if (!saw_start && (start < (1.5 * pi)))
    saw_start = 1;
  if (!saw_end && (end > start) && (end < (1.5 * pi)))
    saw_end = 1;
  if (saw_start && !closed) {
    a[n].x = x - 2;
    a[n++].y = y + h + 2;
  }
  if (saw_start && !saw_end) {
    a[n].x = cx;
    a[n++].y = y + h + 2;
  } else
    closed = saw_start;

  saw_start = 1;
  saw_end = (end > start);
  
  if (saw_start && !closed) {
    a[n].x = x + w + 2;
    a[n++].y = y + h + 2;
  }
  if (saw_start && !saw_end) {
    a[n].x = x + w + 2;
    a[n++].y = cy;    
  } else
    closed = saw_start;

  if (!saw_end && (end < (pi / 2)))
    saw_end = 1;
  if (saw_start && !closed) {
    a[n].x = x + w + 2;
    a[n++].y = y - 2;
  }
  if (saw_start && !saw_end) {
    a[n].x = cx;
    a[n++].y = y - 2; 
  } else
    closed = saw_start;
  
  if (!saw_end && (end < pi))
    saw_end = 1;
  if (saw_start && !closed) {
    a[n].x = x - 2;
    a[n++].y = y - 2;
  }
  if (saw_start && !saw_end) {
    a[n].x = x - 2;
    a[n++].y = cy;    
  } else
    closed = saw_start;

  if (!saw_end && (end < (1.5 * pi)))
    saw_end = 1;
  if (saw_start && !closed) {
    a[n].x = x - 2;
    a[n++].y = y + h + 2;
  } 
  if (saw_start && !saw_end) {
    a[n].x = cx;
    a[n++].y = y + h + 2;
  } else
    closed = saw_start;

  if (!closed) {
    a[n].x = x + w + 2;
    a[n++].y = y + h + 2;
  }

  r->SetPolygon(n, a);

  Intersect(r);

  no_prgn = save_no_prgn;
}

void wxRegion::Union(wxRegion *r)
{
  if (r->dc != dc) return;
  if (r->ReallyEmpty()) return;

  if (!no_prgn) {
    if (!r->prgn) abort();
    if (!prgn)
      prgn = r->prgn;
    else {
      wxPathRgn *pr;
      pr = new wxUnionPathRgn(prgn, r->prgn);
      prgn = pr;
    }
  }

#ifdef wx_msw
  if (!rgn) {
    rgn = CreateRectRgn(0, 0, 1, 1); // SET-ORIGIN FLAGGED
    CombineRgn(rgn, r->rgn, rgn, RGN_COPY);
  } else
    CombineRgn(rgn, r->rgn, rgn, RGN_OR);
#endif
#ifdef wx_x
  if (!rgn) {
    rgn = XCreateRegion();
  }
  XUnionRegion(rgn, r->rgn, rgn);
#endif
#ifdef wx_mac
  if (!rgn) {
    rgn = NewRgn();
  }
  UnionRgn(rgn, r->rgn, rgn);
#endif
}

void wxRegion::Intersect(wxRegion *r)
{
  if (r->dc != dc) return;
  if (r->ReallyEmpty()) {
    Cleanup();
    return;
  }

  if (!no_prgn) {
    wxPathRgn *pr;
    if (!r->prgn) abort();
    pr = new wxIntersectPathRgn(prgn, r->prgn);
    prgn = pr;
  }

#ifdef wx_msw
  if (!rgn) return;
  CombineRgn(rgn, r->rgn, rgn, RGN_AND);
#endif
#ifdef wx_x
  if (!rgn) return;
  XIntersectRegion(rgn, r->rgn, rgn);
#endif
#ifdef wx_mac
  if (!rgn) return;
  SectRgn(rgn, r->rgn, rgn);
#endif

  if (ReallyEmpty()) {
    Cleanup();
  }
}

void wxRegion::Subtract(wxRegion *r)
{
  if (r->dc != dc) return;
  if (r->ReallyEmpty()) return;

  if (!no_prgn) {
    /* wxDiffPathRgn is only half a subtract; the result must be intersected with the first part */
    wxPathRgn *pr;
    if (!r->prgn) abort();
    pr = new wxDiffPathRgn(prgn, r->prgn);
    pr = new wxIntersectPathRgn(prgn, pr);
    prgn = pr;
  }

#ifdef wx_msw
  if (!rgn) return;
  CombineRgn(rgn, rgn, r->rgn, RGN_DIFF);
#endif
#ifdef wx_x
  if (!rgn) return;
  XSubtractRegion(rgn, r->rgn, rgn);
#endif
#ifdef wx_mac
  if (!rgn) return;
  DiffRgn(rgn, r->rgn, rgn);
#endif

  if (ReallyEmpty()) {
    Cleanup();
    return;
  }
}
  
void wxRegion::Xor(wxRegion *r)
{
  if (r->dc != dc) return;
  if (r->ReallyEmpty()) return;

  if (!no_prgn) {
    wxPathRgn *pr;
    if (!r->prgn) abort();
    if (!prgn)
      pr = r->prgn;
    else
      pr = new wxDiffPathRgn(prgn, r->prgn);
    prgn = pr;
  }

#ifdef wx_msw
  if (!rgn) return;
  CombineRgn(rgn, rgn, r->rgn, RGN_XOR);
#endif
#ifdef wx_x
  if (!rgn) return;
  XXorRegion(rgn, r->rgn, rgn);
#endif
#ifdef wx_mac
  if (!rgn) return;
  XorRgn(rgn, r->rgn, rgn);
#endif

  if (ReallyEmpty()) {
    Cleanup();
    return;
  }
}
  
void wxRegion::BoundingBox(double *x, double *y, double *w, double *h)
{
  if (Empty()) {
    *x = *y = *w = *h = 0;
    return;
  } else {
    double v;
#ifdef wx_msw
    RECT r;

    GetRgnBox(rgn, &r);
  
    *x = r.left;
    *y = r.top;
    *w = r.right - r.left;
    *h = r.bottom - r.top;
#endif
#ifdef wx_x
    XRectangle r;
    
    XClipBox(rgn, &r);
    
    *x = r.x;
    *y = r.y;
    *w = r.width;
    *h = r.height;
#endif
#ifdef wx_mac
    {
      Rect r;
      GetRegionBounds(rgn,&r);
      *x = r.left;
      *y = r.top;
      *w = r.right - *x;
      *h = r.bottom - *y;
    }
#endif

    if (is_ps) {
      /* Bitmap-based region is stored upside-down */
      *y = -(*y);
    }
    
    v = dc->DeviceToLogicalX((int)*x);
    *x = v;
    v = dc->DeviceToLogicalY((int)*y);
    *y = v;
    v = dc->DeviceToLogicalXRel((int)*w);
    *w = v;
    v = dc->DeviceToLogicalYRel((int)*h);
    *h = v;
  }
}

Bool wxRegion::Empty()
{
#ifdef wx_msw
  RECT r;
  if (!rgn) return TRUE;

  return (GetRgnBox(rgn, &r) == NULLREGION);
#endif
#ifdef wx_x
  if (!rgn) return TRUE;
  return XEmptyRegion(rgn);
#endif
#ifdef wx_mac
  if (!rgn) return TRUE;
  return EmptyRgn(rgn);
#endif
}

Bool wxRegion::ReallyEmpty()
{
  return Empty() && !prgn;
}

Bool wxRegion::IsInRegion(double x, double y)
{
  int ix, iy;

  if (!rgn) return FALSE;

  x = dc->FLogicalToDeviceX(x);
  y = dc->FLogicalToDeviceY(y);
  

  ix = (int)floor(x);
  iy = (int)floor(y);

#ifdef wx_xt
  return XPointInRegion(rgn, ix, iy);
#endif
#ifdef wx_msw
  return PtInRegion(rgn, ix, iy);
#endif
#ifdef wx_mac
  {
    Point p;
    p.h = ix;
    p.v = iy;
    return PtInRgn(p, rgn);
  }
#endif
}

void wxRegion::Install(long target)
{
  if (prgn) {
    Bool oe;
    wxPathRgn *do_p;

#ifdef WX_USE_CAIRO
    cairo_init_clip(CAIRO_DEV);
    cairo_new_path(CAIRO_DEV);
#endif
#ifdef wx_mac
    CGContextRef cg = (CGContextRef)target;
    PathTarget *t;
    CGMutablePathRef path;
    int i;
    
    if (paths) {
      for (i = 0; i < npaths; i++) {
	CGContextBeginPath(cg);
	CGContextAddPath(cg, paths[i]);
	if (oes[i])
	  CGContextEOClip(cg);
	else
	  CGContextClip(cg);
      }
      return;
    }
  
    path = CGPathCreateMutable();
  
    t = (PathTarget *)malloc(sizeof(PathTarget));
    t->path = path;

    t->paths = NULL;
    t->oes = NULL;
    t->npaths = 0;
    t->apaths = 0;
    
    target = (long)t;
#endif
#ifdef wx_msw
    Graphics *g = (Graphics *)target;
    GraphicsPath *gp;
    PathTarget *t;
    int i;

    if (paths) {
      wxGSetClip(g, paths[0], CombineModeReplace);
      for (i = 1; i < npaths; i++) {
	wxGSetClip(g, paths[i], CombineModeIntersect);
      }
      return;
    }
    
    gp = wxGPathNew(FillModeAlternate);

    t = (PathTarget *)malloc(sizeof(PathTarget));
    t->path = gp;
    t->npaths = 0;
    t->apaths = 0;

    target = (long)t;
#endif

    do_p = prgn->Lift();
    oe = do_p->Install(target, 0);

#ifdef WX_USE_CAIRO
    if (oe)
      cairo_set_fill_rule(CAIRO_DEV, CAIRO_FILL_RULE_EVEN_ODD);
    cairo_clip(CAIRO_DEV);
    if (oe)
      cairo_set_fill_rule(CAIRO_DEV, CAIRO_FILL_RULE_WINDING);
    cairo_new_path(CAIRO_DEV);
#endif
#if defined(wx_mac) || defined(wx_msw)
    npaths = t->npaths + 1;
    {
# ifdef MZ_PRECISE_GC
      PathTargetPath_t *ps;
      ps = (PathTargetPath_t *)scheme_malloc_atomic(sizeof(PathTargetPath_t) * npaths);
      paths = ps;
# else
      paths = new WXGC_ATOMIC PathTargetPath_t[npaths];
# endif
    }
# ifdef wx_mac
    oes = new WXGC_ATOMIC Bool[npaths];
# endif
    for (i = 0; i < npaths - 1; i++) {
      paths[i] = t->paths[i];
# ifdef wx_mac
      oes[i] = t->oes[i];
# endif
    }
    paths[npaths - 1] = t->path;
# ifdef wx_mac
    oes[i] = oe;
# endif

    free(t);
# ifdef wx_mac
    for (i = 0; i < npaths; i++) {
      CGContextBeginPath(cg);
      CGContextAddPath(cg, paths[i]);
      if (oe)
	CGContextEOClip(cg);
      else
	CGContextClip(cg);
    }
# endif
# ifdef wx_msw
    wxGSetClip(g, paths[0], CombineModeReplace);
    for (i = 1; i < npaths; i++) {
      wxGSetClip(g, paths[i], CombineModeIntersect);
    }
# endif
#endif
  } else {
    /* Empty region: */
#ifdef WX_USE_CAIRO
    cairo_new_path(CAIRO_DEV);
    cairo_clip(CAIRO_DEV);
#endif
#ifdef wx_mac
    {
      CGContextRef cg = (CGContextRef)target;
      CGRect r;
      r.origin.x = 0;
      r.origin.y = 0;
      r.size.width = 0;
      r.size.height = 0;
      CGContextClipToRect(cg, r);
    }
#endif
#ifdef wx_msw
    {
      GraphicsPath *gp;
      Graphics *g = (Graphics *)target;
      gp = wxGPathNew(FillModeAlternate);
      wxGSetClip(g, gp, CombineModeReplace);
      wxGPathRelease(gp);
    }
#endif
  }
}

void wxRegion::InstallPS(wxPostScriptDC *dc, wxPSStream *s)
{
  Bool oe;
  wxPathRgn *do_p;

  do_p = prgn->Lift();
  s->Out("newpath\n");

  oe = do_p->InstallPS(dc, s);

  if (oe)
    s->Out("eoclip\n");
  else
    s->Out("clip\n");
}

/***************************************************************************************/

#ifdef wx_mac
static CGAffineTransform current_xform;
#endif
#ifdef wx_msw
static GraphicsPath *current_path;
#endif

wxPathRgn::wxPathRgn(wxDC *dc)
: wxObject(FALSE)
{
  if (dc) {
    double x, y, xs, ys;
    dc->GetDeviceOrigin(&x, &y);
    dc->GetUserScale(&xs, &ys);
    ox = x;
    oy = y;
    sx = xs;
    sy = ys;
  } else {
    ox = oy = 0.0;
    sx = sy = 1.0;
  }
}

wxPathRgn::~wxPathRgn()
{ 
  /* If anything important is added here, change constructor chaining
     to wxObject from FALSE to TRUE. Beware that wxPaths can share
     wxPathRgns. */
}

long wxPathRgn::PrepareScale(long target, Bool oe)
{
#ifdef WX_USE_CAIRO
  cairo_matrix_t *m;
  m = cairo_matrix_create();
  cairo_current_matrix(CAIRO_DEV, m);
  cairo_default_matrix(CAIRO_DEV);
  cairo_translate(CAIRO_DEV, ox, oy);
  cairo_scale(CAIRO_DEV, sx, sy);
  return (long)m;
#endif
#ifdef wx_mac
  current_xform = CGAffineTransformMakeTranslation(ox, oy);
  current_xform = CGAffineTransformScale(current_xform, sx, sy);
  return 0;
#endif
#ifdef wx_msw
  current_path = wxGPathNew(oe ?  FillModeAlternate : FillModeWinding);
  return 0;
#endif
}

void wxPathRgn::RestoreScale(long target, long v)
{
#ifdef WX_USE_CAIRO
  cairo_matrix_t *m = (cairo_matrix_t *)v;
  cairo_set_matrix(CAIRO_DEV, m);
  cairo_matrix_destroy(m);
#endif
#ifdef wx_mac
#endif
#ifdef wx_msw
  Matrix *m;
  m = wxGMatrixNew();
  wxGMatrixTranslate(m, ox, oy);
  wxGMatrixScale(m, sx, sy);
  wxGPathTransform(current_path, m);
  wxGMatrixRelease(m);
  wxGPathAddPath(GP, current_path, TRUE);
  wxGPathRelease(current_path);
#endif
}


wxRectanglePathRgn::wxRectanglePathRgn(wxDC *dc_for_scale, double _x, double _y, double _width, double _height)
: wxPathRgn(dc_for_scale)
{
  x = _x;
  y = _y;
  width = _width;
  height = _height;
}

Bool wxRectanglePathRgn::Install(long target, Bool reverse)
{
  long m;
  m = PrepareScale(target, TRUE);

#ifdef WX_USE_CAIRO
  cairo_move_to(CAIRO_DEV, x, y);
  if (reverse) {
    cairo_rel_line_to(CAIRO_DEV, 0, height);
    cairo_rel_line_to(CAIRO_DEV, width, 0);
    cairo_rel_line_to(CAIRO_DEV, 0, -height);
  } else {
    cairo_rel_line_to(CAIRO_DEV, width, 0);
    cairo_rel_line_to(CAIRO_DEV, 0, height);
    cairo_rel_line_to(CAIRO_DEV, -width, 0);
  }
  cairo_close_path(CAIRO_DEV);
#endif
#ifdef wx_mac
  CGPathMoveToPoint(CGPATH, CGXFORM, x, y);
  if (reverse) {
    CGPathAddLineToPoint(CGPATH, CGXFORM, x, y + height);
    CGPathAddLineToPoint(CGPATH, CGXFORM, x + width, y + height);
    CGPathAddLineToPoint(CGPATH, CGXFORM, x + width, y);
  } else {
    CGPathAddLineToPoint(CGPATH, CGXFORM, x + width, y);
    CGPathAddLineToPoint(CGPATH, CGXFORM, x + width, y + height);
    CGPathAddLineToPoint(CGPATH, CGXFORM, x, y + height);
  }
  CGPathCloseSubpath(CGPATH);
#endif
#ifdef wx_msw
  if (reverse) {
    wxGPathAddLine(CURRENT_GP, x, y, x, y + height);
    wxGPathAddLine(CURRENT_GP, x, y + height, x + width, y + height);
    wxGPathAddLine(CURRENT_GP, x + width, y + height, x + width, y);
  } else {
    wxGPathAddLine(CURRENT_GP, x, y, x + width, y);
    wxGPathAddLine(CURRENT_GP, x + width, y, x + width, y + height);
    wxGPathAddLine(CURRENT_GP, x + width, y + height, x, y + height);
  }
  wxGPathCloseFigure(CURRENT_GP);
#endif

  RestoreScale(target, m);
  
  return FALSE;
}

Bool wxRectanglePathRgn::InstallPS(wxPostScriptDC *dc, wxPSStream *s)
{
  double xx, yy, ww, hh;

  xx = dc->FsLogicalToDeviceX(x, ox, sx);
  yy = dc->FsLogicalToDeviceY(y, oy, sy);
  ww = dc->FsLogicalToDeviceXRel(width, ox, sx);
  hh = dc->FsLogicalToDeviceYRel(height, oy, sy);

  s->Out(xx); s->Out(" "); s->Out(yy); s->Out(" moveto\n");
  s->Out(xx + ww); s->Out(" "); s->Out(yy); s->Out(" lineto\n");
  s->Out(xx + ww); s->Out(" "); s->Out(yy - hh); s->Out(" lineto\n");
  s->Out(xx); s->Out(" "); s->Out(y - hh); s->Out(" lineto\n");
  s->Out("closepath\n");

  return FALSE;
}

wxRoundedRectanglePathRgn::wxRoundedRectanglePathRgn(wxDC *dc_for_scale, 
						     double _x, double _y, double _width, double _height, double _radius)
: wxPathRgn(dc_for_scale)
{
  x = _x;
  y = _y;
  width = _width;
  height = _height;
  radius = _radius;

  if (radius < 0) {
    if (width > height)
      radius = radius * height;
    else
      radius = radius * width;
  }
}

Bool wxRoundedRectanglePathRgn::Install(long target, Bool reverse)
{
  long m;
  m = PrepareScale(target, TRUE);

#ifdef WX_USE_CAIRO
  {
    double w = width, h = height;
    if (reverse) {
      cairo_move_to(CAIRO_DEV, x, y + radius);
      cairo_line_to(CAIRO_DEV, x, y + h - radius);
      cairo_arc_negative(CAIRO_DEV, x + radius, y + h - radius, radius, wxPI, 0.5 * wxPI);
      cairo_line_to(CAIRO_DEV, x + w - radius, y + h);
      cairo_arc_negative(CAIRO_DEV, x + w - radius, y + h - radius, radius, 0.5 * wxPI, 0);
      cairo_line_to(CAIRO_DEV, x + w, y + radius);
      cairo_arc_negative(CAIRO_DEV, x + w - radius, y + radius, radius, 2 * wxPI, 1.5 * wxPI);
      cairo_line_to(CAIRO_DEV, x + radius, y);
      cairo_arc_negative(CAIRO_DEV, x + radius, y + radius, radius, 1.5 * wxPI, wxPI);
      cairo_line_to(CAIRO_DEV, x, y + radius);
    } else {
      cairo_move_to(CAIRO_DEV, x, y + radius);
      cairo_arc(CAIRO_DEV, x + radius, y + radius, radius, wxPI, 1.5 * wxPI);
      cairo_line_to(CAIRO_DEV, x + w - radius, y);
      cairo_arc(CAIRO_DEV, x + w - radius, y + radius, radius, 1.5 * wxPI, 2 * wxPI);
      cairo_line_to(CAIRO_DEV, x + w, y + h - radius);
      cairo_arc(CAIRO_DEV, x + w - radius, y + h - radius, radius, 0, 0.5 * wxPI);
      cairo_line_to(CAIRO_DEV, x + radius, y + h);
      cairo_arc(CAIRO_DEV, x + radius, y + h - radius, radius, 0.5 * wxPI, wxPI);
      cairo_line_to(CAIRO_DEV, x, y + radius);
    }
    cairo_close_path(CAIRO_DEV);
  }
#endif
#ifdef wx_mac
  if (reverse) {
    CGPathMoveToPoint(CGPATH, CGXFORM, x + radius, y);
    CGPathAddArc(CGPATH, CGXFORM, x + radius, y + radius, radius, 1.5 * wxPI, 1.0 * wxPI, TRUE);
    CGPathAddLineToPoint(CGPATH, CGXFORM, x, y + height - radius);
    CGPathAddArc(CGPATH, CGXFORM, x + radius, y + height - radius, radius, 1.0 * wxPI, 0.5 * wxPI, TRUE);
    CGPathAddLineToPoint(CGPATH, CGXFORM, x + width - radius, y + height);
    CGPathAddArc(CGPATH, CGXFORM, x + width - radius, y + height - radius, radius, 0.5 * wxPI, 0, TRUE);
    CGPathAddLineToPoint(CGPATH, CGXFORM, x + width, y + radius);
    CGPathAddArc(CGPATH, CGXFORM, x + width - radius, y + radius, radius, 2 * wxPI, 1.5 * wxPI, TRUE);
  } else {
    CGPathMoveToPoint(CGPATH, CGXFORM, x + radius, y);
    CGPathAddLineToPoint(CGPATH, CGXFORM, x + width - radius, y);
    CGPathAddArc(CGPATH, CGXFORM, x + width - radius, y + radius, radius, 1.5 * wxPI, 2 * wxPI, FALSE);
    CGPathAddLineToPoint(CGPATH, CGXFORM, x + width, y + height - radius);
    CGPathAddArc(CGPATH, CGXFORM, x + width - radius, y + height - radius, radius, 0, 0.5 * wxPI, FALSE);
    CGPathAddLineToPoint(CGPATH, CGXFORM, x + radius, y + height);
    CGPathAddArc(CGPATH, CGXFORM, x + radius, y + height - radius, radius, 0.5 * wxPI, 1.0 * wxPI, FALSE);
    CGPathAddLineToPoint(CGPATH, CGXFORM, x, y + radius);
    CGPathAddArc(CGPATH, CGXFORM, x + radius, y + radius, radius, 1.0 * wxPI, 1.5 * wxPI, FALSE);
  }
  CGPathCloseSubpath(CGPATH);
#endif
#ifdef wx_msw
  if (reverse) {
    wxGPathAddArc(CURRENT_GP, x, y, radius * 2, radius * 2, 270, -90);
    wxGPathAddLine(CURRENT_GP, x, y + radius, x, y + height - radius);
    wxGPathAddArc(CURRENT_GP, x, y + height - 2 * radius, 2 * radius, 2 * radius, 180, -90);
    wxGPathAddLine(CURRENT_GP, x + radius, y + height, x + width - radius, y + height);
    wxGPathAddArc(CURRENT_GP, x + width - 2 * radius, y + height - 2 * radius, 2 * radius, 2 * radius, 90, -90);
    wxGPathAddLine(CURRENT_GP, x + width, y + height - radius, x + width, y + radius);
    wxGPathAddArc(CURRENT_GP, x + width - 2 * radius, y, radius * 2, radius * 2, 360, -90);
  } else {
    wxGPathAddArc(CURRENT_GP, x, y, radius * 2, radius * 2, 180, 90);
    wxGPathAddLine(CURRENT_GP, x + radius, y, x + width - radius, y);
    wxGPathAddArc(CURRENT_GP, x + width - 2 * radius, y, radius * 2, radius * 2, 270, 90);
    wxGPathAddLine(CURRENT_GP, x + width, y + radius, x + width, y + height - radius);
    wxGPathAddArc(CURRENT_GP, x + width - 2 * radius, y + height - 2 * radius, 2 * radius, 2 * radius, 0, 90);
    wxGPathAddLine(CURRENT_GP, x + width - radius, y + height, x + radius, y + height);
    wxGPathAddArc(CURRENT_GP, x, y + height - 2 * radius, 2 * radius, 2 * radius, 90, 90);
  }
  wxGPathCloseFigure(CURRENT_GP);
#endif

  RestoreScale(target, m);

  return FALSE;
}

Bool wxRoundedRectanglePathRgn::InstallPS(wxPostScriptDC *dc, wxPSStream *s)
{
  double xx, yy, ww, hh, rr;

  xx = dc->FsLogicalToDeviceX(x, ox, sx);
  yy = dc->FsLogicalToDeviceY(y, oy, sy);
  ww = dc->FsLogicalToDeviceXRel(width, ox, sx);
  hh = dc->FsLogicalToDeviceYRel(height, oy, sy);
  if (sx > sy)
    rr = dc->FsLogicalToDeviceYRel(radius, oy, sy);
  else
    rr = dc->FsLogicalToDeviceXRel(radius, ox, sx);

  hh = -hh;

  s->Out(xx + rr); s->Out(" "); 
  s->Out(yy); s->Out(" moveto\n");
  
  s->Out(xx + rr); s->Out(" ");
  s->Out(yy - rr); s->Out(" "); 
  s->Out(rr); s->Out(" 90 180 arc\n");
  
  s->Out(xx + rr); s->Out(" ");
  s->Out(yy + hh + rr); s->Out(" "); 
  s->Out(rr); s->Out(" 180 270 arc\n");
  
  s->Out(xx + ww - rr); s->Out(" ");
  s->Out(yy + hh + rr); s->Out(" "); 
  s->Out(rr); s->Out(" 270 0 arc\n");
  
  s->Out(xx + ww - rr); s->Out(" ");
  s->Out(yy - rr); s->Out(" "); 
  s->Out(rr); s->Out(" 0 90 arc\n");

  s->Out("closepath\n");

  return FALSE;
}

wxPolygonPathRgn::wxPolygonPathRgn(wxDC *dc_for_scale,
				   int _n, wxPoint _points[], double _xoffset, double _yoffset, int _fillStyle)
: wxPathRgn(dc_for_scale)
{
  n = _n;
  points = _points;
  xoffset = _xoffset;
  yoffset = _yoffset;
  fillStyle = _fillStyle;
}

Bool wxPolygonPathRgn::Install(long target, Bool reverse)
{
  int i;
  long m;
  m = PrepareScale(target, fillStyle == wxODDEVEN_RULE);

#ifdef WX_USE_CAIRO
  if (reverse) {
    cairo_move_to(CAIRO_DEV, points[n-1].x + xoffset, points[n-1].y + yoffset);
    for (i = n-1; i--; ) {
      cairo_line_to(CAIRO_DEV, points[i].x + xoffset, points[i].y + yoffset);
    }
  } else {
    cairo_move_to(CAIRO_DEV, points[0].x + xoffset, points[0].y + yoffset);
    for (i = 1; i < n; i++) {
      cairo_line_to(CAIRO_DEV, points[i].x + xoffset, points[i].y + yoffset);
    }
  }
  cairo_close_path(CAIRO_DEV);
#endif
#ifdef wx_mac
  if (reverse) {
    CGPathMoveToPoint(CGPATH, CGXFORM, points[n-1].x + xoffset, points[n-1].y + yoffset);
    for (i = n-1; i--; ) {
      CGPathAddLineToPoint(CGPATH, CGXFORM, points[i].x + xoffset, points[i].y + yoffset);
    }
  } else {
    CGPathMoveToPoint(CGPATH, CGXFORM, points[0].x + xoffset, points[0].y + yoffset);
    for (i = 1; i < n; i++) {
      CGPathAddLineToPoint(CGPATH, CGXFORM, points[i].x + xoffset, points[i].y + yoffset);
    }
  }
  CGPathCloseSubpath(CGPATH);
#endif
#ifdef wx_msw
  if (reverse) {
    for (i = n - 1; i--; ) {
      wxGPathAddLine(CURRENT_GP,
		     points[i+1].x + xoffset, points[i+1].y + yoffset,
		     points[i].x + xoffset, points[i].y + yoffset);
    }
  } else {
    for (i = 0; i < n - 1; i++) {
      wxGPathAddLine(CURRENT_GP,
		     points[i].x + xoffset, points[i].y + yoffset,
		     points[i+1].x + xoffset, points[i+1].y + yoffset);
    }
  }
  wxGPathCloseFigure(CURRENT_GP);
#endif

  RestoreScale(target, m);

  return (fillStyle == wxODDEVEN_RULE);
}

Bool wxPolygonPathRgn::InstallPS(wxPostScriptDC *dc, wxPSStream *s)
{
  double xx, yy;
  int i;

  xx = dc->FsLogicalToDeviceX(points[0].x, ox, sx);
  yy = dc->FsLogicalToDeviceY(points[0].y, oy, sy);
  s->Out(xx); s->Out(" "); 
  s->Out(yy); s->Out(" moveto\n");

  for (i = 1; i < n; i++) {
    xx = dc->FsLogicalToDeviceX(points[i].x, ox, sx);
    yy = dc->FsLogicalToDeviceY(points[i].y, oy, sy);
    s->Out(xx); s->Out(" "); 
    s->Out(yy); s->Out(" lineto\n");
  }
  s->Out("closepath\n");

  return (fillStyle == wxODDEVEN_RULE);
}


wxPathPathRgn::wxPathPathRgn(wxDC *dc_for_scale,
			     wxPath *_p, double _xoffset, double _yoffset, int _fillStyle)
: wxPathRgn(dc_for_scale)
{
  p = new wxPath();
  p->AddPath(_p);
  xoffset = _xoffset;
  yoffset = _yoffset;
  fillStyle = _fillStyle;
}

Bool wxPathPathRgn::Install(long target, Bool reverse)
{
  wxPath *q;
  long m;

  m = PrepareScale(target, fillStyle == wxODDEVEN_RULE);

  if (reverse) {
    q = new wxPath();
    q->AddPath(p);
    q->Reverse();
  } else
    q = p;

#ifdef WX_USE_CAIRO
  q->Install(target, 0, 0);
#endif
#ifdef wx_mac
  q->Install((long)CGPATH, 0, 0);
#endif
#ifdef wx_msw
  q->Install((long)CURRENT_GP, 0, 0);
#endif

  RestoreScale(target, m);

  return (fillStyle == wxODDEVEN_RULE);
}

Bool wxPathPathRgn::InstallPS(wxPostScriptDC *dc, wxPSStream *s)
{
  p->InstallPS(dc, s, 0, 0);
  return (fillStyle == wxODDEVEN_RULE);
}

wxArcPathRgn::wxArcPathRgn(wxDC *dc_for_scale,
			   double _x, double _y, double _w, double _h, double _start, double _end)
: wxPathRgn(dc_for_scale)
{
  x = _x;
  y = _y;
  w = _w;
  h = _h;
  start = _start;
  end = _end;
}

Bool wxArcPathRgn::Install(long target, Bool reverse)
{
  long m;

  m = PrepareScale(target, TRUE);

#ifdef WX_USE_CAIRO
  {
    cairo_translate(CAIRO_DEV, x, y);
    cairo_scale(CAIRO_DEV, w, h);
    if ((start != 0.0) || (end != (2 * wxPI)))
      cairo_move_to(CAIRO_DEV, 0.5, 0.5);
    if (!reverse)
      cairo_arc(CAIRO_DEV, 0.5, 0.5, 0.5, -end, -start);
    else
      cairo_arc_negative(CAIRO_DEV, 0.5, 0.5, 0.5, -start, -end);
    cairo_close_path(CAIRO_DEV);
  }
#endif
#ifdef wx_mac
  {
    CGAffineTransform xform;
    xform = CGAffineTransformTranslate(*CGXFORM, x, y);
    xform = CGAffineTransformScale(xform, w, h);
    if ((start != 0.0) || (end != (2 * wxPI)))
      CGPathMoveToPoint(CGPATH, &xform, 0.5, 0.5);
    if (!reverse)
      CGPathAddArc(CGPATH, &xform, 0.5, 0.5, 0.5, (2 * wxPI) - end, (2 * wxPI) - start, FALSE);
    else
      CGPathAddArc(CGPATH, &xform, 0.5, 0.5, 0.5, (2 * wxPI) - start, (2 * wxPI) - end, TRUE);
    CGPathCloseSubpath(CGPATH);
  }
#endif
#ifdef wx_msw
  {
    double init, span;
    if ((start == 0.0) && (end == 2 * wxPI)) {
      if (reverse) {
	wxGPathAddArc(CURRENT_GP, x, y, w, h, 360.0, -360.0);
      } else {
	wxGPathAddArc(CURRENT_GP, x, y, w, h, 0.0, 360.0);
      }
    } else {
      init = (2 * wxPI - start) * 180 / wxPI;
      init = fmod(init, 360.0);
      if (init < 0.0)
	init += 360.0;
      
      span = (start - end) * 180 / wxPI;
      span = fmod(span, 360.0);
      if (span > 0)
	span -= 360.0;
      if (reverse) {
	wxGPathAddPie(CURRENT_GP, x, y, w, h, init + span, -span);
      } else {
	wxGPathAddPie(CURRENT_GP, x, y, w, h, init, span);
      }
    }
    wxGPathCloseFigure(CURRENT_GP);
  }
#endif

  RestoreScale(target, m);

  return FALSE;
}

Bool wxArcPathRgn::InstallPS(wxPostScriptDC *dc, wxPSStream *s)
{
  double xx, yy, ww, hh;

  xx = dc->FsLogicalToDeviceX(x, ox, sx);
  yy = dc->FsLogicalToDeviceY(y, oy, sy);
  ww = dc->FsLogicalToDeviceXRel(w, ox, sx);
  hh = dc->FsLogicalToDeviceYRel(h, oy, sy);

  s->Out("matrix currentmatrix ");
  s->Out(xx + ww/2); s->Out(" "); s->Out(yy - hh/2); s->Out(" translate ");
  s->Out(ww); s->Out(" "); s->Out(hh); s->Out(" scale\n");
  if ((start != 0) || (end != 2 * wxPI)) {
    s->Out("0 0 moveto\n");
  }
  s->Out("0 0 0.5 "); 
  s->Out(start * 180 / wxPI); s->Out(" "); 
  s->Out(end * 180 / wxPI); s->Out(" arc setmatrix closepath\n");
  
  return FALSE;
}


wxUnionPathRgn::wxUnionPathRgn(wxPathRgn *_f, wxPathRgn *_s)
: wxPathRgn(NULL)
{
  if (!_f || !_s)
    abort();
  a = _f;
  b = _s;
}

Bool wxUnionPathRgn::Install(long target, Bool reverse)
{
  Bool aoe, boe;

  aoe = a->Install(target, reverse);
  boe = b->Install(target, reverse);

  return aoe || boe;
}

Bool wxUnionPathRgn::InstallPS(wxPostScriptDC *dc, wxPSStream *s)
{
  Bool aoe, boe;

  aoe = a->InstallPS(dc, s);
  boe = b->InstallPS(dc, s);

  return aoe || boe;
}

wxIntersectPathRgn::wxIntersectPathRgn(wxPathRgn *_f, wxPathRgn *_s)
: wxPathRgn(NULL)
{
  if (!_f || !_s)
    abort();
  a = _f;
  b = _s;
}

Bool wxIntersectPathRgn::Install(long target, Bool reverse)
{
  Bool aoe;

  aoe = a->Install(target, reverse);
#ifdef WX_USE_CAIRO
  if (aoe)
    cairo_set_fill_rule(CAIRO_DEV, CAIRO_FILL_RULE_EVEN_ODD);
  cairo_clip(CAIRO_DEV);
  if (aoe)
    cairo_set_fill_rule(CAIRO_DEV, CAIRO_FILL_RULE_WINDING);
  cairo_new_path(CAIRO_DEV);
#endif
#if defined(wx_mac) || defined(wx_msw)
  {
    PathTarget *t = (PathTarget *)target;
    PathTargetPath_t path;
    int i;

    if (t->npaths + 1 >= t->apaths) {
      PathTargetPath_t *naya;
# ifdef wx_mac
      Bool *naya_oes;
# endif
      int n = (t->apaths + 5) * 2;
      
# ifdef MZ_PRECISE_GC
      naya = (PathTargetPath_t *)scheme_malloc_atomic(sizeof(PathTargetPath_t) * n);
# else
      naya = new WXGC_ATOMIC PathTargetPath_t[n];
# endif
# ifdef wx_mac
      naya_oes = new WXGC_ATOMIC Bool[n];
# endif
      for (i = 0; i < t->npaths; i++) {
	naya[i] = t->paths[i];
# ifdef wx_mac
	naya_oes[i] = t->oes[i];
# endif
      }

      t->paths = naya;
# ifdef wx_mac
      t->oes = naya_oes;
# endif
      t->apaths = n;
    }
# ifdef wx_mac
    t->oes[t->npaths] = aoe;
# endif
    t->paths[t->npaths++] = t->path;
# ifdef wx_mac
    path = CGPathCreateMutable();
# endif
# ifdef wx_msw
    path = wxGPathNew(FillModeAlternate);
# endif
    t->path = path;
  }
#endif
  return b->Install(target, reverse);
}

Bool wxIntersectPathRgn::InstallPS(wxPostScriptDC *dc, wxPSStream *s)
{
  Bool aoe;

  aoe = a->InstallPS(dc, s);
  if (aoe)
    s->Out("eoclip\n");
  else
    s->Out("clip\n");
  
  return b->InstallPS(dc, s);
}
  

wxDiffPathRgn::wxDiffPathRgn(wxPathRgn *_f, wxPathRgn *_s)
: wxPathRgn(NULL)
{
  if (!_f || !_s)
    abort();
  a = _f;
  b = _s;
}

Bool wxDiffPathRgn::Install(long target, Bool reverse)
{
  Bool aoe, boe;

  aoe = a->Install(target, reverse);
  boe = b->Install(target, !reverse);

  return aoe || boe;
}

Bool wxDiffPathRgn::InstallPS(wxPostScriptDC *dc, wxPSStream *s)
{
  Bool aoe, boe;

  aoe = a->InstallPS(dc, s);
  s->Out("reversepath\n");
  boe = b->InstallPS(dc, s);
  s->Out("reversepath\n");

  return aoe || boe;
}

wxPathRgn *wxPathRgn::Lift()
{
  return this;
}

Bool wxPathRgn::IsIntersect()
{
  return FALSE;
}

int wxPathRgn::FlattenIntersects(wxPathRgn **l, wxPathRgn *r, int i)
{
  if (r->IsIntersect())
    return FlattenIntersects(l, ((wxIntersectPathRgn *)r)->b, 
			     FlattenIntersects(l, ((wxIntersectPathRgn *)r)->a, i));
  
  if (l)
    l[i] = r;

  return i + 1;
}

wxPathRgn *wxUnionPathRgn::Lift()
{
  wxPathRgn *la, *lb;
  wxPathRgn *r = NULL, **al, **bl;
  int na, nb, i, j;

  la = a->Lift();
  lb = b->Lift();

  if (!la->IsIntersect()
      && !lb->IsIntersect()
      && (a == la) && (b == lb))
    return this;

  /* (A n B) U (C n D) = (A U C) n (A U D) n (B U C) n (B U D) */

  /* count: */
  na = FlattenIntersects(NULL, la, 0);
  nb = FlattenIntersects(NULL, lb, 0);

  al = new wxPathRgn*[na];
  bl = new wxPathRgn*[nb];

  /* flatten: */
  FlattenIntersects(al, la, 0);
  FlattenIntersects(bl, lb, 0);

  for (i = 0; i < na; i++) {
    for (j = 0; j < nb; j++) {
      wxPathRgn *c;
      c = new wxUnionPathRgn(al[i], bl[j]);
      if (r)
	r = new wxIntersectPathRgn(r, c);
      else
	r = c;
    }
  }

  return r;
}

wxPathRgn *wxIntersectPathRgn::Lift()
{
  wxPathRgn *la, *lb;

  la = a->Lift();
  lb = b->Lift();

  if ((la == a) && (lb == b))
    return this;
  else
    return new wxIntersectPathRgn(la, lb);
}

Bool wxIntersectPathRgn::IsIntersect()
{
  return TRUE;
}

wxPathRgn *wxDiffPathRgn::Lift()
{
  wxPathRgn *la, *lb;
  wxPathRgn *r = NULL, **al, **bl;
  int na, nb, i;

  la = a->Lift();
  lb = b->Lift();

  if (!la->IsIntersect()
      && !lb->IsIntersect()
      && (a == la) && (b == lb))
    return this;

  if (lb->IsIntersect()) {
    /* A - (B n C) = (A - B) u (A - C) */
    nb = FlattenIntersects(NULL, lb, 0);
    bl = new wxPathRgn*[nb];
    FlattenIntersects(bl, lb, 0);
    
    for (i = 0; i < nb; i++) {
      wxPathRgn *s;
      s = new wxDiffPathRgn(la, bl[i]);
      if (r) {
	r = new wxUnionPathRgn(r, s);
      } else
	r = s;
    }

    return r->Lift(); /* Handles intersections in la */
  } else {
    /* (A n B) - C = (A - C) n (B - C)   [note: C has no intersections] */
    na = FlattenIntersects(NULL, la, 0);
    al = new wxPathRgn*[na];
    FlattenIntersects(al, la, 0);
    
    for (i = 0; i < na; i++) {
      wxPathRgn *s;
      s = new wxDiffPathRgn(al[i], lb);
      if (r) {
	r = new wxIntersectPathRgn(r, s);
      } else
	r = s;
    }

    return r;
  }
}

/********************************************************/
/*                        Paths                         */
/********************************************************/

#define CMD_CLOSE        1.0
#define CMD_MOVE         2.0
#define CMD_LINE         3.0
#define CMD_CURVE        4.0

#define ROTATE_XY(x, y) { xtmp1 = x; ytmp1 = y;             \
                          xtmp2 = (xx * xtmp1) + (xy * ytmp1); \
                          ytmp2 = (yy * ytmp1) + (yx * xtmp1); \
                          x = xtmp2; y = ytmp2; }

wxPath::wxPath()
{
  Reset();
}

wxPath::~wxPath()
{
  Reset();
}

void wxPath::Reset()
{
  ClearCache();
  cmd_size = 0;
  alloc_cmd_size = 0;
  cmds = NULL;
  last_cmd = -1;
}

void wxPath::ClearCache()
{
  poly_pts = NULL;
}

void wxPath::MakeRoom(int n)
{
  ClearCache();
  if (cmd_size + n > alloc_cmd_size) {
    double *a;
    int s;
    s = 2 * (alloc_cmd_size + n);
    a = new WXGC_ATOMIC double[s];
    memcpy(a, cmds, sizeof(double) * cmd_size);
    cmds = a;
    alloc_cmd_size = s;
  }
}

Bool wxPath::IsOpen()
{
  return ((last_cmd > -1) && (cmds[last_cmd] != CMD_CLOSE));
}

void wxPath::Close()
{
  if ((last_cmd > -1) && (cmds[last_cmd] != CMD_CLOSE)) {
    MakeRoom(1);
    last_cmd = cmd_size;
    cmds[cmd_size++] = CMD_CLOSE;
  }
}

void wxPath::MoveTo(double x, double y)
{
  Close();

  MakeRoom(3);
  last_cmd = cmd_size;
  cmds[cmd_size++] = CMD_MOVE;
  cmds[cmd_size++] = x;
  cmds[cmd_size++] = y;
}

void wxPath::LineTo(double x, double y)
{
  MakeRoom(3);
  last_cmd = cmd_size;
  cmds[cmd_size++] = CMD_LINE;
  cmds[cmd_size++] = x;
  cmds[cmd_size++] = y;
}

void wxPath::Arc(double x, double y, double w, double h, double start, double end, Bool ccw)
{
  double delta, angle, rotate;
  double x0, y0, x1, y1, x2, y2, x3, y3;
  double xx, xy, yy, yx, xtmp1, ytmp1, xtmp2, ytmp2;
  int did_one = 0, start_cmd = cmd_size, start_open;

  start_open = IsOpen();

  /* The arc below is backwards from the MrEd API.... */
  {
    double s;
    s = start;
    start = end;
    end = s;
    ccw = !ccw;
  }

  if (ccw) {
    double s;
    s = start;
    start = end;
    end = s;
  }

  delta = end - start;
  if (delta > 2 * wxPI)
    delta = 2 * wxPI;
  else if (delta < 0) {
    delta = fmod(delta, 2 * wxPI);
    delta += 2 * wxPI;
  }

  /* At this point, delta is between 0 and 2pi */
  
  if (delta == 2 * wxPI)
    start = 0;

  /* Change top-left to center: */
  x += w/2;
  y += h/2;

  /* Make up to 4 curves to represent the arc. */
  do {
    if (delta > (wxPI / 2))
      angle = (wxPI / 2);
    else
      angle = delta;

    /* First generate points for an arc
       of `angle' length from -angle/2 to
       +angle/2. */

    x0  = cos(angle / 2);
    y0  = sin(angle / 2);
    x1 = (4 - x0) / 3;
    y1 = ((1 - x0) * (3 - x0)) / (3 * y0);
    x2 = x1;
    y2 = -y1;
    x3 = x0;
    y3 = -y0;
    
    /* Rotate to start: */
    rotate = start + (angle / 2);
    xx = cos(rotate);
    xy = sin(rotate);
    yy = xx;
    yx = -xy;
    ROTATE_XY(x0, y0);
    ROTATE_XY(x1, y1);
    ROTATE_XY(x2, y2);
    ROTATE_XY(x3, y3);

    /* Scale and move to match ellipse: */
    x0 = (x0 * w/2) + x;
    x1 = (x1 * w/2) + x;
    x2 = (x2 * w/2) + x;
    x3 = (x3 * w/2) + x;

    y0 = (y0 * h/2) + y;
    y1 = (y1 * h/2) + y;
    y2 = (y2 * h/2) + y;
    y3 = (y3 * h/2) + y;

    if (!did_one) {
      if (IsOpen()) {
	LineTo(x0, y0);
      } else {
	MoveTo(x0, y0);
      }
    }

    if (angle)
      CurveTo(x1, y1, x2, y2, x3, y3);
    else
      LineTo(x3, y3);
    
    start += angle;
    delta -= angle;
    did_one = 1;
  } while (delta > 0);

  if (!ccw) {
    Reverse(start_cmd, start_open);
  }
}

void wxPath::CurveTo(double x1, double y1, double x2, double y2, double x3, double y3)
{
  MakeRoom(7);
  last_cmd = cmd_size;
  cmds[cmd_size++] = CMD_CURVE;
  cmds[cmd_size++] = x1;
  cmds[cmd_size++] = y1;
  cmds[cmd_size++] = x2;
  cmds[cmd_size++] = y2;
  cmds[cmd_size++] = x3;
  cmds[cmd_size++] = y3;
}

void wxPath::Translate(double x, double y)
{
  int i = 0;
  while (i < cmd_size) {
    if (cmds[i] == CMD_CLOSE) {
      i += 1;
    } else if ((cmds[i] == CMD_MOVE)
	       || (cmds[i] == CMD_LINE)) {
      cmds[i+1] += x;
      cmds[i+2] += y;
      i += 3;
    } else if (cmds[i] == CMD_CURVE) {
      cmds[i+1] += x;
      cmds[i+2] += y;
      cmds[i+3] += x;
      cmds[i+4] += y;
      cmds[i+5] += x;
      cmds[i+6] += y;
      i += 7;
    }
  }
}

void wxPath::Scale(double x, double y)
{
  int i = 0;
  while (i < cmd_size) {
    if (cmds[i] == CMD_CLOSE) {
      i += 1;
    } else if ((cmds[i] == CMD_MOVE)
	       || (cmds[i] == CMD_LINE)) {
      cmds[i+1] *= x;
      cmds[i+2] *= y;
      i += 3;
    } else if (cmds[i] == CMD_CURVE) {
      cmds[i+1] *= x;
      cmds[i+2] *= y;
      cmds[i+3] *= x;
      cmds[i+4] *= y;
      cmds[i+5] *= x;
      cmds[i+6] *= y;
      i += 7;
    }
  }
}

void wxPath::Rotate(double a)
{
  double xx, xy, yy, yx, xtmp1, ytmp1, xtmp2, ytmp2;
  int i = 0;

  xx = cos(a);
  xy = sin(a);
  yy = xx;
  yx = -xy;

  while (i < cmd_size) {
    if (cmds[i] == CMD_CLOSE) {
      i += 1;
    } else if ((cmds[i] == CMD_MOVE)
	       || (cmds[i] == CMD_LINE)) {
      ROTATE_XY(cmds[i+1], cmds[i+2]);
      i += 3;
    } else if (cmds[i] == CMD_CURVE) {
      ROTATE_XY(cmds[i+1], cmds[i+2]);
      ROTATE_XY(cmds[i+3], cmds[i+4]);
      ROTATE_XY(cmds[i+5], cmds[i+6]);
      i += 7;
    }
  }
}

void wxPath::Reverse(int start_cmd, Bool start_with_line)
{
  int e, i, j, pos, n, *cs, controls;
  double *a;

  while (start_cmd < cmd_size) {
    /* Find next starting point: */
    if (cmds[start_cmd] == CMD_CLOSE) {
      start_cmd += 1;
    }

    i = start_cmd;
    n = 0;
    while (i < cmd_size) {
      if (cmds[i] == CMD_CLOSE) {
	break;
      } else {
	n++;
	if (cmds[i] == CMD_MOVE) {
	  i += 3;
	} else if (cmds[i] == CMD_LINE) {
	  i += 3;
	} else if (cmds[i] == CMD_CURVE) {
	  i += 7;
	}
      }
    }
    e = i;

    /* Reverse sub-path in [start_cmd, e) */

    a = new WXGC_ATOMIC double[e - start_cmd];
    cs = new WXGC_ATOMIC int[n];

    /* Traverse again to find command starts: */
    n = 0;
    i = start_cmd;
    while (i < e) {
      cs[n++] = i;
      if (cmds[i] == CMD_MOVE) {
	i += 3;
      } else if (cmds[i] == CMD_LINE) {
	i += 3;
      } else if (cmds[i] == CMD_CURVE) {
	i += 7;
      }
    }

    /* Reverse */
    controls = -1;
    pos = 0;
    for (j = n; j--; ) {
      i = cs[j];
      if (!start_with_line && (j == n - 1)) {
	a[pos++] = CMD_MOVE;
      } else if (controls >= 0) {
	a[pos++] = CMD_CURVE;
	a[pos++] = cmds[controls+3];
	a[pos++] = cmds[controls+4];
	a[pos++] = cmds[controls+1];
	a[pos++] = cmds[controls+2];
      } else {
	a[pos++] = CMD_LINE;
      }

      if ((cmds[i] == CMD_MOVE)
	  || (cmds[i] == CMD_LINE)) {
	a[pos++] = cmds[i+1];
	a[pos++] = cmds[i+2];
	controls = -1;
      } else if (cmds[i] == CMD_CURVE) {
	a[pos++] = cmds[i+5];
	a[pos++] = cmds[i+6];
	controls = i;
      }
    }

    memcpy(cmds + start_cmd, a, (e - start_cmd) * sizeof(double));

    start_cmd = e;
  }
}

void wxPath::AddPath(wxPath *p)
{
  int i, closed_n;

  if (!IsOpen()) {
    /* Simple case: this path is closed, so just append p */
    MakeRoom(p->cmd_size);
    last_cmd = cmd_size + p->last_cmd;
    for (i = 0; i < p->cmd_size; i++) {
      cmds[cmd_size++] = p->cmds[i];
    }
  } else {
    /* Put closed paths in p on the front of this path,
       and add unclosed paths to this path's unclosed
       path. */
    if (p->IsOpen()) {
      for (i = 0; i < p->cmd_size; i++) {
	if (p->cmds[i] == CMD_CLOSE)
	  break;
	else if (cmds[i] == CMD_CURVE)
	  i += 7;
	else
	  i += 3;
      }
      
      if (i < p->cmd_size) {
	closed_n = i + 1;
      } else {
	closed_n = 0;
      }
    } else {
      /* No open path in p */
      closed_n = p->cmd_size;
    }
    
    MakeRoom(p->cmd_size);
    memmove(cmds + closed_n, cmds, cmd_size * sizeof(double));
    memcpy(cmds, p->cmds, closed_n * sizeof(double));
    if (closed_n  < p->cmd_size) {
      /* There was an open path in p... */
      memcpy(cmds + cmd_size + closed_n, p->cmds + closed_n, (p->cmd_size - closed_n) * sizeof(double));

      /* p's open path must start with CMD_MOVE; change it to CMD_LINE */
      cmds[closed_n + cmd_size] = CMD_LINE;
    
      last_cmd = cmd_size + p->last_cmd;
    } else {
      /* No open path in p, so just adjust last_cmd */
      last_cmd += closed_n;
    }
    cmd_size += p->cmd_size;
  }
}

void wxPath::Install(long target, double dx, double dy)
{
  int i = 0;
  double lx = 0.0, ly = 0.0;

#ifdef WX_USE_CAIRO
  cairo_new_path(CAIRO_DEV);
#endif

  while (i < cmd_size) {
    if (cmds[i] == CMD_CLOSE) {
#ifdef WX_USE_CAIRO
      cairo_close_path(CAIRO_DEV);
#endif
#ifdef wx_mac
      CGPathCloseSubpath(PATHPATH);;
#endif
#ifdef wx_msw
      wxGPathCloseFigure(PATH_GP);
#endif
      i += 1;
    } else if (cmds[i] == CMD_MOVE) {
#ifdef WX_USE_CAIRO
      cairo_move_to(CAIRO_DEV, cmds[i+1]+dx, cmds[i+2]+dy);
#endif
#ifdef wx_mac
      CGPathMoveToPoint(PATHPATH, NULL, cmds[i+1]+dx, cmds[i+2]+dy);
#endif
      lx = cmds[i+1];
      ly = cmds[i+2];
      i += 3;
    } else if (cmds[i] == CMD_LINE) {
      if ((cmds[i+1] != lx) || (cmds[i+2] != ly)) {
#ifdef WX_USE_CAIRO
	cairo_line_to(CAIRO_DEV, cmds[i+1]+dx, cmds[i+2]+dy);
#endif
#ifdef wx_mac
	CGPathAddLineToPoint(PATHPATH, NULL, cmds[i+1]+dx, cmds[i+2]+dy);
#endif
#ifdef wx_msw
	wxGPathAddLine(PATH_GP, lx+dx, ly+dy, cmds[i+1]+dx, cmds[i+2]+dy);
#endif
      }
      lx = cmds[i+1];
      ly = cmds[i+2];
      i += 3;
    } else if (cmds[i] == CMD_CURVE) {
      if ((cmds[i+5] != lx) || (cmds[i+6] != ly)) {
#ifdef WX_USE_CAIRO
	cairo_curve_to(CAIRO_DEV, 
		       cmds[i+1]+dx, cmds[i+2]+dy, 
		       cmds[i+3]+dx, cmds[i+4]+dy, 
		       cmds[i+5]+dx, cmds[i+6]+dy);
#endif
#ifdef wx_mac
	CGPathAddCurveToPoint(PATHPATH, NULL, 
			      cmds[i+1]+dx, cmds[i+2]+dy, 
			      cmds[i+3]+dx, cmds[i+4]+dy, 
			      cmds[i+5]+dx, cmds[i+6]+dy);
#endif
#ifdef wx_msw
	wxGPathAddBezier(PATH_GP, lx+dx, ly+dy, 
			 cmds[i+1]+dx, cmds[i+2]+dy, 
			 cmds[i+3]+dx, cmds[i+4]+dy, 
			 cmds[i+5]+dx, cmds[i+6]+dy);
#endif
      }
      lx = cmds[i+5];
      ly = cmds[i+6];
      i += 7;
    }
  }
}

void wxPath::InstallPS(wxPostScriptDC *dc, wxPSStream *s, double dx, double dy)
{
  int i = 0;

  while (i < cmd_size) {
    if (cmds[i] == CMD_CLOSE) {
      s->Out("closepath\n");
      i += 1;
    } else if ((cmds[i] == CMD_MOVE) 
	       || (cmds[i] == CMD_LINE)) {
      double x, y;
      x = dc->FLogicalToDeviceX(cmds[i+1]+ dx);
      y = dc->FLogicalToDeviceY(cmds[i+2]+ dy);
      s->Out(x); s->Out(" "); s->Out(y);
      if (cmds[i] == CMD_LINE)
	s->Out(" lineto\n");
      else
	s->Out(" moveto\n");
      i += 3;
    } else if (cmds[i] == CMD_CURVE) {
      double x1, y1, x2, y2, x3, y3;
      x1 = dc->FLogicalToDeviceX(cmds[i+1] + dx);
      y1 = dc->FLogicalToDeviceY(cmds[i+2] + dy);
      x2 = dc->FLogicalToDeviceX(cmds[i+3] + dx);
      y2 = dc->FLogicalToDeviceY(cmds[i+4] + dy);
      x3 = dc->FLogicalToDeviceX(cmds[i+5] + dx);
      y3 = dc->FLogicalToDeviceY(cmds[i+6] + dy);
      s->Out(x1); s->Out(" "); s->Out(y1); s->Out(" "); 
      s->Out(x2); s->Out(" "); s->Out(y2); s->Out(" "); 
      s->Out(x3); s->Out(" "); s->Out(y3); s->Out(" "); 
      s->Out("curveto\n");
      i += 7;
    }
  }
}

int wxPath::ToPolygons(int **_lens, double ***_ptss, double sx, double sy)
{
  int i, cnt, *lens, len, alloc_len, need_len;
  double lx, ly, **ptss, *pts, *naya;

  cnt = 0;
  for (i = 0; i < cmd_size; ) {
    if (cmds[i] == CMD_CLOSE) {
      cnt++;
      i += 1;
    } else if (cmds[i] == CMD_MOVE) {
      i += 3;
    } else if (cmds[i] == CMD_LINE) {
      i += 3;
    } else if (cmds[i] == CMD_CURVE) {
      i += 7;
    }
  }

  if (IsOpen())
    cnt++;

  ptss = new double*[cnt];
  lens = new int[cnt];
  cnt = 0;

  pts = NULL;
  len = 0;
  alloc_len = 0;
  lx = ly = 0;

  for (i = 0; i < cmd_size; ) {
    if (cmds[i] == CMD_CLOSE) {
      ptss[cnt] = pts;
      lens[cnt] = len;
      cnt++;

      len = 0;
      alloc_len = 0;
      pts = NULL;
      lx = ly = 0;

      i += 1;
    } else {
      if ((cmds[i] == CMD_MOVE)
	  || (cmds[i] == CMD_LINE)) {
	need_len = 1;
      } else if (cmds[i] == CMD_CURVE) {
	double dx, dy;
	dx = sx * (lx - cmds[i + 5]);
	dy = sy * (ly - cmds[i + 6]);
	if (dx < 0) dx = -dx;
	if (dy < 0) dy = -dy;
	if (dx > dy)
	  need_len = (int)ceil(dx);
	else
	  need_len = (int)ceil(dy);
	need_len += 1;
      } else {
	need_len = 0;
      }

      if (len + (2 * need_len) > alloc_len) {
	int l;
	l = (len + (2 * need_len)) * 2;
	naya = new WXGC_ATOMIC double[l];
	memcpy(naya, pts, len * sizeof(double));
	pts = naya;
	alloc_len = l;
      }

      if ((cmds[i] == CMD_MOVE)
	  || (cmds[i] == CMD_LINE)) {
	lx = cmds[i+1];
	ly = cmds[i+2];
	pts[len++] = lx;
	pts[len++] = ly;
	i += 3;
      } else if (cmds[i] == CMD_CURVE) {
	int d;
	double x0 = lx, x1 = cmds[i+1], x2 = cmds[i+3], x3 = cmds[i+5];
	double y0 = ly, y1 = cmds[i+2], y2 = cmds[i+4], y3 = cmds[i+6];
	double ax = (((x3 - (x2 * 3)) + (x1 * 3)) - x0);
	double ay = (((y3 - (y2 * 3)) + (y1 * 3)) - y0);
	double bx = (((x2 * 3) - (x1 * 6)) + (x0 * 3));
	double by = (((y2 * 3) - (y1 * 6)) + (y0 * 3));
	double cx = ((x1 * 3) - (x0 * 3));
	double cy = ((y1 * 3) -  (y0 * 3));
	double dx = x0, dy = y0, tt, x, y;

	for (d = 0; d < need_len; d++) {
	  tt = ((double)d / (double)(need_len - 1));
	  x = ((((((tt * ax) + bx) * tt) + cx) * tt) + dx);
	  y = ((((((tt * ay) + by) * tt) + cy) * tt) + dy);
	  pts[len++] = x;
	  pts[len++] = y;
	}

	lx = x3;
	ly = y3;

	i += 7;
      }
    }
  }

  if (IsOpen()) {
    ptss[cnt] = pts;
    lens[cnt] = len;
    cnt++;
  }

  *_lens = lens;
  *_ptss = ptss;

  return cnt;
}

void wxPath::BoundingBox(double *_x1, double *_y1, double *_x2, double *_y2)
{
  double x1, x2, y1, y2;
  int i;

  if (cmd_size) {
    /* First command must be move-to: */
    x1 = cmds[1];
    y1 = cmds[2];
    x2 = x1;
    y2 = y1;
    for (i = 3; i < cmd_size; ) {
      if (cmds[i] == CMD_CLOSE) {
	i += 1;
      } else if ((cmds[i] == CMD_MOVE)
		 || (cmds[i] == CMD_LINE)) {
	if (cmds[i+1] < x1)
	  x1 = cmds[i+1];
	if (cmds[i+1] > x2)
	  x2 = cmds[i+1];
	if (cmds[i+2] < y1)
	  y1 = cmds[i+2];
	if (cmds[i+2] > y2)
	  y2 = cmds[i+2];
	i += 3;
      } else if (cmds[i] == CMD_CURVE) {
	int j;
	for (j = 0; j < 6; j += 2) {
	  if (cmds[i+j+1] < x1)
	    x1 = cmds[i+j+1];
	  if (cmds[i+j+1] > x2)
	    x2 = cmds[i+j+1];
	  if (cmds[i+j+2] < y1)
	    y1 = cmds[i+j+2];
	  if (cmds[i+j+2] > y2)
	    y2 = cmds[i+j+2];
	}
	i += 7;
      }
    }
  } else {
    x1 = y1 = x2 = y2 = 0.0;
  }

  *_x1 = x1;
  *_x2 = x2;
  *_y1 = y1;
  *_y2 = y2;
}
