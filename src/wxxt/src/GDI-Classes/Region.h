
#ifndef wb_rgnh
#define wb_rgnh

#include "wx_dc.h"

class wxPSRgn;

#ifdef UseXtRegions
typedef Region XtRegion;
#else
typedef void *XtRegion;
#endif

class wxRegion : public wxObject 
{
 public:
#ifdef wx_msw
  HRGN rgn;
#endif
#ifdef wx_x
  XtRegion rgn;
#endif
  wxDC *dc;
  wxPSRgn *ps;
  int is_ps;

  wxRegion(wxDC *dc, wxRegion *r = NULL);
  ~wxRegion();

  inline wxDC *GetDC() { return dc; }
  
  void SetRectangle(float x, float y, float width, float height);
  void SetRoundedRectangle(float x, float y, float width, float height, float radius = 20.0);
  void SetEllipse(float x, float y, float width, float height);
  void SetPolygon(int n, wxPoint points[], float xoffset = 0, float yoffset = 0, 
		  int fillStyle=wxODDEVEN_RULE);
  void SetArc(float x, float y, float w, float h, float start, float end);

  void Union(wxRegion *);
  void Intersect(wxRegion *);
  void Subtract(wxRegion *);

  void BoundingBox(float *x, float *y, float *w, float *h);

  Bool Empty();
  
  void Cleanup();

  /* PS Stuff */
  wxRegion& operator<<(const char *s);
  wxRegion& operator<<(double d);
};

class wxPSRgn : public wxObject
{
 public:
  int is_intersect;
  wxPSRgn() { is_intersect = 0; }
  virtual char *GetString() = 0;
  virtual wxPSRgn *Lift() = 0;
};

class wxPSRgn_Atomic : public wxPSRgn
{
 public:
  char *s;
  wxPSRgn_Atomic(char *ps) { s = ps; }
  char *GetString() { return s; }
  wxPSRgn *Lift() { return this; }
};

class wxPSRgn_Composite : public wxPSRgn
{
 public:
  wxPSRgn *a, *b;
  char *MakeString(const char *prefix, const char *infix, const char *suffix);

  int FlattenIntersects(wxPSRgn **l, wxPSRgn *r, int i);
};

class wxPSRgn_Union : public wxPSRgn_Composite
{
 public:
  wxPSRgn_Union(wxPSRgn *ra, wxPSRgn *rb) { a = ra; b = rb; }
  char *GetString();
  wxPSRgn *Lift();
};

class wxPSRgn_Intersect : public wxPSRgn_Composite
{
 public:
  wxPSRgn_Intersect(wxPSRgn *ra, wxPSRgn *rb) { a = ra; b = rb; is_intersect = 1; }
  char *GetString();
  wxPSRgn *Lift();
};

class wxPSRgn_Diff : public wxPSRgn_Composite
{
 public:
  wxPSRgn_Diff(wxPSRgn *ra, wxPSRgn *rb) { a = ra; b = rb; }
  char *GetString();
  wxPSRgn *Lift();
};

#endif
