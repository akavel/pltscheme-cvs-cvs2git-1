/*
 * File:      wx_gdi.cc
 * Purpose:     GDI (Graphics Device Interface) objects and functions (X version)
 * Author:      Julian Smart
 * Created:     1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wx_gdi.cxx,v 1.7 1999/01/14 22:25:08 mflatt Exp $
 * Copyright:   (c) 1993, AIAI, University of Edinburgh
 */

static const char sccsid[] = "@(#)wx_gdi.cc	1.2 5/9/94";

#ifdef __GNUG__
#pragma implementation
#endif

#include <iostream.h>
#include "common.h"
#include "wx_utils.h"
#include "wx_main.h"
#include "wx_gdi.h"
#include "wx_dcmem.h"

#if USE_XPM_IN_X
#include "wx_item.h"
#ifdef VMS
#include "[--.contrib.wxxpm.libxpm.34b.lib]xpm34.h"
#else
extern "C" {
#include "../../contrib/wxxpm/libxpm.34b/lib/xpm34.h"
}
#endif
#endif
#if USE_IMAGE_LOADING_IN_X
#ifdef VMS
#include "[--.utils.image.src]wx_image.h"
#else
#include "../../utils/image/src/wx_image.h"
#endif
#endif

const DISPLAY_DPI = 100;

XFontPool *wxFontPool = NULL;
#include <X11/cursorfont.h>

#include "wx_frame.h"


#if !USE_IMAGE_PIXMAP_METHOD
Pixmap
XCreateInsensitivePixmap( Display *display, Pixmap pixmap );
#endif

char *wxDecorativeFontName = copystring("helvetica");
char *wxRomanFontName = copystring("times");
char *wxModernFontName = copystring("courier");
char *wxSwissFontName = copystring("lucida");
char *wxScriptFontName = copystring("zapfchancery");
char *wxTeletypeFontName = copystring("lucidatypewriter");
char *wxDefaultFontName = copystring("*");

/* MATTHEW: [4] Display-specific font */
IMPLEMENT_DYNAMIC_CLASS(wxXFont, wxObject)

wxXFont::~wxXFont()
{
  if (mFont)
    XmFontListFree (mFont);
  mFont = NULL;
}

IMPLEMENT_DYNAMIC_CLASS(wxFont, wxObject)

wxFont::wxFont (void)
{
  fontid = wxDEFAULT;
  family = wxTheFontNameDirectory.GetFamily(fontid);
  style = wxNORMAL;
  weight = wxNORMAL;
  point_size = 12;
  underlined = FALSE;

#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: Bad policy in general; worse in GC */
  wxTheFontList->Append (this);
#endif
}

/* Constructor for a font. Note that the real construction is done
 * in wxDC::SetFont, when information is available about scaling etc.
 */
/* MATTHEW: [4] New font system */
wxFont::wxFont (int PointSize, int FontId, int Style, int Weight, Bool Underlined):
wxbFont (PointSize, FontId, Style, Weight, Underlined)
{
  fontid = FontId;
  family = wxTheFontNameDirectory.GetFamily(FontId);
  style = Style;
  weight = Weight;
  point_size = PointSize;
  underlined = Underlined;

#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: Bad policy in general; worse in GC */
  wxTheFontList->Append (this);
#endif
}

/* MATTHEW: [4] New font system */
wxFont::wxFont (int PointSize, const char *Face, int Family, int Style, int Weight, 
		Bool Underlined):
wxbFont (PointSize, Family, Style, Weight, Underlined)
{
  fontid = wxTheFontNameDirectory.FindOrCreateFontId(Face, Family);
  family = wxTheFontNameDirectory.GetFamily(fontid);
  style = Style;
  weight = Weight;
  point_size = PointSize;
  underlined = Underlined;

#if !WXGARBAGE_COLLECTION_ON
  wxTheFontList->Append (this);
#endif  
}

wxFont::~wxFont ()
{
  /* MATTHEW: [4] moved work to display-specific */
  xfonts.DeleteContents(TRUE);
#if !WXGARBAGE_COLLECTION_ON
  wxTheFontList->DeleteObject (this);
#endif
}

/* MATTHEW: [4] Display argument, new implementation */
XmFontList wxFont::GetInternalFont (Display *display, XFontStruct **fs)
{
  wxNode *node;

  for (node = xfonts.First(); node; node = node->Next()) {
    wxXFont *xf = (wxXFont *)node->Data();
    if (xf->display == display) {
      if (fs)
	*fs = xf->xFont;
      return xf->mFont;
    }
  }

  int res_x = DISPLAY_DPI;
  int res_y = DISPLAY_DPI;
  int scaled_size = (int) (10 * point_size);

  wxXFont *xf;

  xf = new wxXFont;
  
  xf->xFont = wxFontPool->FindNearestFont (fontid, style,
					   weight, scaled_size,
					   underlined, res_x, res_y,
					   display);
  
  xf->mFont = XmFontListCreate (xf->xFont, XmSTRING_DEFAULT_CHARSET);

  xfonts.Append(xf);

  if (fs)
    *fs = xf->xFont;

  return xf->mFont;
}

#ifdef wx_x
/* MATTHEW: [4] Use display */
XFontInfo::XFontInfo (int the_fontid, int the_style, int the_weight, int the_point_size,
	   Bool is_underlined, Display *a_display,
	   XFontStruct * the_font)
{
  fontid = the_fontid;
  style = the_style;
  weight = the_weight;
  point_size = the_point_size;
  font = the_font;
  underlined = is_underlined;
  display = a_display; /* MATTHEW: [4] Remember display */
}

XFontPool::XFontPool (void)
{
  cache = NULL;
}


void XFontPool::
/* MATTHEW: [4] Display argument */
AddFont (int fontid, int style, int weight, int point_size,
	 Bool is_underlined, Display *display,
	 XFontStruct * font)
{
  /* MATTHEW: [4] Use Display argument */
  XFontInfo *info = new XFontInfo (fontid, style, weight, point_size, is_underlined, display, font);
  Append (info);
}


XFontStruct *
  XFontPool::
/* MATTHEW: [4] Display argument */
FindFont (int fontid, int style, int weight, int point_size, Bool is_underlined, Display *display)
{
  if (cache && cache->fontid == fontid && cache->style == style && cache->weight == weight
      && cache->point_size == point_size && cache->underlined == is_underlined
      && cache->display == display)
    return cache->font;

  wxNode *node = First ();
  XFontStruct *found = 0;
  while (node && !found) {
    XFontInfo *info = (XFontInfo *) node->Data ();
    if (info->fontid == fontid && info->style == style && info->weight == weight
	&& info->point_size == point_size && info->underlined == is_underlined
	&& info->display == display) {
      found = info->font;
      cache = info;
    }
    node = node->Next ();
  }

  return found;
}

// This is really just CreateFont now...
XFontStruct *
XFontPool::
FindOrCreateFont (int fontid, int style, int weight,
		  int point_size, Bool is_underlined, 
		  int point_size_to_store, int xres, int yres,
		  Display *display)
{
  XFontStruct *font;
  int try_again = 2;
  char *name;
  
  while (try_again--) {
    name = wxTheFontNameDirectory.GetScreenName(fontid, weight, style);
    if (!name)
      name = "-*-*-*-*-*-*-*-%d-*-*-*-*-*-*";
    
    sprintf(wxBuffer, name, point_size);
    
    font = XLoadQueryFont(display, wxBuffer);

    if (font || (style == wxDEFAULT))
      try_again = 0;
    else {
      if (style == wxSLANT)
	style = wxITALIC;
      else
	style = wxSLANT;
    }
  }

  return font;
}

XFontStruct *
  XFontPool::
FindNearestFont (int fontid, int style, int weight, int point_size,
		 Bool is_underlined, int xres, int yres,
		 Display *display)
{
  XFontStruct *font;

  font = FindFont(fontid, style, weight, point_size, is_underlined, display);

  if (font)
    return font;

  font = FindOrCreateFont (fontid, style, weight, point_size, is_underlined, 
			   point_size, xres, yres, display);

  if (!font) {
    
#define DELTA 10

    const int delta = DELTA;
    /* The weights below might need to be "tuned" @@@@ */
    const int max_size = point_size + 2 * delta * (1 + (point_size / (18 * delta)));
    const int min_size = point_size - 2 * delta * (1 + (point_size / (18 * delta)));
    int i;
    
    /* Search for another size (approx.) */
    /* Smaller */
    for (i = point_size - delta; !font && i >= delta && i >= min_size; i -= delta)
      font = FindOrCreateFont (fontid, style, weight, i, is_underlined, 
			       point_size, xres, yres, display);

    /* Larger */
    for (i = point_size + delta; !font && i <= max_size; i += delta)
      font = FindOrCreateFont (fontid, style, weight, i, is_underlined, point_size, 
			       xres, yres, display);
    
    /* Try Default fontid */
    if (!font && fontid != wxDEFAULT)
      font = FindOrCreateFont (wxDEFAULT, style, weight, point_size, is_underlined, 
			       point_size, xres, yres, display);

    /* Bogus font */
    if (!font)
      {
	int bog_standard = 120;

	// For next time this happens, remember we just use a bog standard one
	
	font = FindOrCreateFont (wxDEFAULT, wxNORMAL, wxNORMAL, bog_standard,
				 is_underlined, point_size, DISPLAY_DPI, DISPLAY_DPI,
				 display);
	/* MATTHEW: [4] If above still found nothing, last ditch effort: */
	if (!font) {
	  if (!font) {
	    /* last-ditch efforst to find a font */
	    sprintf (wxBuffer, "-*-*-*-*-*-*-*-%d-*-*-*-*-*-*", 
		     bog_standard);
	    font = XLoadQueryFont (display, wxBuffer);
	  }
	}
      }
  }

  // Remember which font to use for next time
  if (font)
    AddFont(fontid, style, weight, point_size, is_underlined, display, font);

  return font;
}

#endif


/*
 * Colour map
 *
 * When constructed with the default constructor, we start from
 * the wxMainColormap, allocating additional read-only cells
 * in Create().  The cells are freed on the next call to Create()
 * or when the destructor is called.
 */
 
/* Wolfram Gloger <u7y22ab@sunmail.lrz-muenchen.de>
I have implemented basic colormap support for the X11 versions of
wxWindows, notably wxColourMap::Create().  The way I did it is to
allocate additional read-only color cells in the default colormap.  In
general you will get arbitrary pixel values assigned to these new
cells and therefore I added a method wxColourMap::TransferBitmap()
which maps the pixel values 0..n to the real ones obtained with
Create().  This is only implemented for the popular case of 8-bit
depth.

Allocating read-write color cells would involve installing a private
X11 colormap for a particular window, and AFAIK this is not
recommended; only the window manager should do this...  Also, it is
not the functionality that wxColourMap::Create() aims to provide.
 */

/* MATTHEW: [4] Display-specific colormap */
Colormap wxGetMainColormap(Display *display)
{
  static Colormap def_cmap;

  if (!display) /* Must be called first with non-NULL display */
    return def_cmap;

  Colormap c = 
    DefaultColormapOfScreen(XScreenOfDisplay(display, 
					     DefaultScreen(display)));

  if (!def_cmap)
    def_cmap = c;

  return c;
}

IMPLEMENT_DYNAMIC_CLASS(wxColourMap, wxObject)

wxColourMap::wxColourMap (void)
{
}

wxColourMap::~wxColourMap (void)
{
  /* MATTHEW: [4] Flush cmap by creating empty */
  Create(0, NULL, NULL, NULL);
}

/* MATTHEW: [4] Re-wrote to handle multiple displays */
Bool
wxColourMap::Create(const int n,
		    const unsigned char *red,
		    const unsigned char *green,
		    const unsigned char *blue)
{
  XColor xcol;
  Display *display;

  wxNode *node, *next;
  
  for (node = xcolormaps.First(); node; node = next) {
    wxXColormap *c = (wxXColormap *)node->Data();
    unsigned long *pix_array = c->pix_array;
    Colormap cmap = c->cmap;
    Bool destroyable = c->destroyable;
    int pix_array_n = c->pix_array_n;
    display = c->display;

    if (pix_array_n > 0) {
      XFreeColors(display, cmap, pix_array, pix_array_n, 0);
      delete [] pix_array;
    }

    if (destroyable)
      XFreeColormap(display, cmap);

    next = node->Next();
    xcolormaps.DeleteNode(node);
    delete c;
  }
  
  if (!n)
    return FALSE;

  display = wxGetDisplay();

  unsigned long *pix_array;
  Colormap cmap;
  int pix_array_n;

  cmap = wxGetMainColormap(display);

  pix_array = new unsigned long[n];
  if (!pix_array)
    return FALSE;

  pix_array_n = n;
  xcol.flags = DoRed | DoGreen | DoBlue;
  for(int i = 0; i < n; i++) {
    xcol.red = (unsigned short)red[i] << 8;
    xcol.green = (unsigned short)green[i] << 8;
    xcol.blue = (unsigned short)blue[i] << 8;
    pix_array[i] = (wxAllocColor(display, cmap, &xcol) == 0) ? 0 : xcol.pixel;
  }

  wxXColormap *c = new wxXColormap;

  c->pix_array_n = pix_array_n;
  c->pix_array = pix_array;
  c->cmap = cmap;
  c->display = display;
  c->destroyable = FALSE;
  xcolormaps.Append(c);

  return TRUE;
}

Bool
wxColourMap::TransferBitmap(void *data, int depth, int size)
{
  switch(depth) {
  case 8:
    {
	unsigned char *uptr = (unsigned char *)data;
	int pix_array_n;
	unsigned long *pix_array = GetXPixArray(wxGetDisplay(), &pix_array_n);
	while(size-- > 0) {
	  if((int)*uptr < pix_array_n)
	    *uptr = (unsigned char)pix_array[*uptr];
	  uptr++;
	}

	return TRUE;
     }
  default:
	return FALSE;
}
}

int wxColourMap::GetPixel(const unsigned char red, const unsigned char green, const unsigned char blue)
{
  return 0;
}

Bool wxColourMap::GetRGB(const int pixel, unsigned char *red, unsigned char *green, unsigned char *blue)
{
  return FALSE;
}

/* MATTHEW: [4] Get display-specific cmap */
Colormap wxColourMap::GetXColormap(Display *display)
{
  if (!xcolormaps.Number())
    return wxGetMainColormap(display);

  wxNode *node;

  for (node = xcolormaps.First(); node; node = node->Next()) {
    wxXColormap *c = (wxXColormap *)node->Data();
    if (c->display == display)
      return c->cmap;
  }

  /* Make a new one: */
  wxXColormap *c = new wxXColormap;
  wxXColormap *first = (wxXColormap *)xcolormaps.First()->Data();
  XColor xcol;
  int pix_array_n = first->pix_array_n;
  
  c->pix_array_n = pix_array_n;
  c->pix_array = new unsigned long[pix_array_n];
  c->display = display;
  c->cmap = wxGetMainColormap(display);
  c->destroyable = FALSE;

  xcol.flags = DoRed | DoGreen | DoBlue;
  for (int i = 0; i < pix_array_n; i++) {
    xcol.pixel = first->pix_array[i];
    XQueryColor(first->display, first->cmap, &xcol);
    c->pix_array[i] = 
      (wxAllocColor(display, c->cmap, &xcol) == 0) ? 0 : xcol.pixel;
  }

  xcolormaps.Append(c);

  return c->cmap;
}

/* MATTHEW: [4] Get display-specific pix_array */
unsigned long *wxColourMap::GetXPixArray(Display *display, int *n)
{
  wxNode *node;

  for (node = xcolormaps.First(); node; node = node->Next()) {
    wxXColormap *c = (wxXColormap *)node->Data();
    if (c->display == display) {
      if (n)
	*n = c->pix_array_n;
      return c->pix_array;
    }
  }
  
  /* Not found; call GetXColormap, which will create it, then this again */
  (void)GetXColormap(display);
  return GetXPixArray(display, n);
}

void wxColourMap::PutXColormap(Display *display, Colormap cm, Bool dp)
{
  Create(0, NULL, NULL, NULL);
  wxXColormap *c = new wxXColormap;

  c->pix_array_n = 0;
  c->pix_array = NULL;
  c->display = display;
  c->cmap = cm;
  c->destroyable = dp;

  xcolormaps.Append(c);
}

// Pens

IMPLEMENT_DYNAMIC_CLASS(wxPen, wxObject)

wxPen::wxPen (void)
{
  stipple = NULL;
  style = wxSOLID;
  join = wxJOIN_ROUND;
  cap = wxCAP_ROUND;
  nb_dash = 0;
  dash = NULL;
  width = 1;
}

wxPen::~wxPen ()
{
}

wxPen::wxPen (wxColour & col, int Width, int Style):
wxbPen (col, Width, Style)
{
  colour = col;

  stipple = NULL;
  width = Width;
  style = Style;
  join = wxJOIN_ROUND;
  cap = wxCAP_ROUND;
  nb_dash = 0;
  dash = NULL;
}

wxPen::wxPen (char *col, int Width, int Style):
wxbPen (col, Width, Style)
{
  colour = col;
  stipple = NULL;
  width = Width;
  style = Style;
  join = wxJOIN_ROUND;
  cap = wxCAP_ROUND;
  nb_dash = 0;
  dash = NULL;
}

// Brushes

IMPLEMENT_DYNAMIC_CLASS(wxBrush, wxObject)

wxBrush::wxBrush (void)
{
  style = wxSOLID;
  stipple = NULL;
}

wxBrush::~wxBrush ()
{
}

wxBrush::wxBrush (wxColour & col, int Style):
wxbBrush (col, Style)
{
  colour = col;
  style = Style;
  stipple = NULL;
}

wxBrush::wxBrush (char *col, int Style):
wxbBrush (col, Style)
{
  colour = col;
  style = Style;
  stipple = NULL;
}


// Cursors
IMPLEMENT_DYNAMIC_CLASS(wxCursor, wxBitmap)

/* MATTHEW: [4] Display-specific cursor for Motif */
IMPLEMENT_DYNAMIC_CLASS(wxXCursor, wxObject)

wxCursor::wxCursor (void)
{
  __type = wxTYPE_CURSOR;
  width = 0;
  height = 0;
  numColors = 0;
  x_pixmap = 0;
  ok = FALSE;
  image = 0;
  cursor_id = 0; /* MATTHEW: [4]  Init cursor_id */
}

wxCursor::wxCursor (char bits[], int w, int h)
{
  __type = wxTYPE_CURSOR;
  width = 0;
  height = 0;
  numColors = 0;
  x_pixmap = 0; 
  ok = FALSE;
  image = 0;
  cursor_id = 0; /* MATTHEW: [4]  Init cursor_id */
}


wxCursor::wxCursor (const char *cursor_file, long flags, int hotSpotX, int hotSpotY)
{
  __type = wxTYPE_CURSOR;
  width = 0;
  height = 0;
  numColors = 0;
  x_pixmap = 0;
  image = 0;
}

/* MATTHEW: [4] Abstract cursor lookup: (Don't miss Motif I-beam change) */

static
Cursor
 XMakeCursor(int cursor_type, Bool& use_raw_x_cursor, Display *dpy)
{
  Cursor x_cursor = 0;
  
  switch (cursor_type)
    {
    case wxCURSOR_WAIT:
      {
	x_cursor = XCreateFontCursor (dpy, XC_watch);
	break;
      }
    case wxCURSOR_CROSS:
      {
	x_cursor = XCreateFontCursor (dpy, XC_crosshair);
	break;
      }
    case wxCURSOR_CHAR:
      {
	// Nothing
	break;
      }
    case wxCURSOR_HAND:
      {
	x_cursor = XCreateFontCursor (dpy, XC_hand1);
	break;
      }
    case wxCURSOR_BULLSEYE:
      {
	x_cursor = XCreateFontCursor (dpy, XC_target);
	break;
      }
    case wxCURSOR_PENCIL:
      {
	x_cursor = XCreateFontCursor (dpy, XC_pencil);
	break;
      }
    case wxCURSOR_MAGNIFIER:
      {
	x_cursor = XCreateFontCursor (dpy, XC_sizing);
	break;
      }
    case wxCURSOR_IBEAM:
      {
	/* MATTHEW: [4] I-beam in Motif */
	x_cursor = XCreateFontCursor (dpy, XC_xterm);
	break;
      }
    case wxCURSOR_NO_ENTRY:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_pirate);
	break;
      }

    case wxCURSOR_LEFT_BUTTON:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_leftbutton);
	break;
      }
    case wxCURSOR_RIGHT_BUTTON:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_rightbutton);
	break;
      }
    case wxCURSOR_MIDDLE_BUTTON:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_middlebutton);
	break;
      }
    case wxCURSOR_QUESTION_ARROW:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_question_arrow);
	break;
      }
    case wxCURSOR_SIZING:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_sizing);
	break;
      }
    case wxCURSOR_WATCH:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_watch);
	break;
      }
    case wxCURSOR_SPRAYCAN:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_spraycan);
	break;
      }
    case wxCURSOR_PAINT_BRUSH:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_spraycan);
	break;
      }
    case wxCURSOR_SIZENWSE:
    case wxCURSOR_SIZENESW:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_circle);
	break;
      }
    case wxCURSOR_SIZEWE:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_sb_h_double_arrow);
	break;
      }
    case wxCURSOR_SIZENS:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_sb_v_double_arrow);
	break;
      }
    case wxCURSOR_POINT_LEFT:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_sb_left_arrow);
	break;
      }
    case wxCURSOR_POINT_RIGHT:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_sb_right_arrow);
	break;
      }
    // (JD Huggins) added more stock cursors for X
    // X-only cursors BEGIN
    case wxCURSOR_CROSS_REVERSE:
    {
      use_raw_x_cursor = TRUE;
      x_cursor = XCreateFontCursor(dpy, XC_cross_reverse);
      break;
    }
    case wxCURSOR_DOUBLE_ARROW:
    {
      use_raw_x_cursor = TRUE;
      x_cursor = XCreateFontCursor(dpy, XC_double_arrow);
      break;
    }
    case wxCURSOR_BASED_ARROW_UP:
    {
      use_raw_x_cursor = TRUE;
      x_cursor = XCreateFontCursor(dpy, XC_based_arrow_up);
      break;
    }
    case wxCURSOR_BASED_ARROW_DOWN:
    {
      use_raw_x_cursor = TRUE;
      x_cursor = XCreateFontCursor(dpy, XC_based_arrow_down);
      break;
    }
    // X-only cursors END
    default:
    case wxCURSOR_ARROW:
      {
	x_cursor = XCreateFontCursor (dpy, XC_top_left_arrow);
	break;
      }
    case wxCURSOR_BLANK:
      {
	GC gc;
	XGCValues gcv;
	Pixmap empty_pixmap;
	XColor blank_color;

	use_raw_x_cursor = TRUE;
	empty_pixmap = XCreatePixmap (dpy, RootWindow (dpy, DefaultScreen (dpy)),
				      16, 16, 1);
	gcv.function = GXxor;
	gc = XCreateGC (dpy,
			empty_pixmap,
			GCFunction,
			&gcv);
	XCopyArea (dpy,
		   empty_pixmap,
		   empty_pixmap,
		   gc,
		   0, 0,
		   16, 16,
		   0, 0);
	XFreeGC (dpy, gc);
	x_cursor = XCreatePixmapCursor (dpy,
					empty_pixmap,
					empty_pixmap,
					&blank_color,
					&blank_color,
					8, 8);

	break;
      }
    }

  return x_cursor;
}

/* MATTHEW: [4] This used XMakeCursor, now */
// Cursors by stock number
wxCursor::wxCursor (int cursor_type)
{
  __type = wxTYPE_CURSOR;
  width = 0;
  height = 0;
  ok = TRUE;

  use_raw_x_cursor = FALSE;

  image = 0;
  x_pixmap = 0;

  cursor_id = cursor_type;
}

Cursor
  wxCursor::GetXCursor(Display *display)
{
  wxNode *node;

  for (node = xcursors.First(); node; node = node->Next()) {
    wxXCursor *c = (wxXCursor *)node->Data();
    
    if (c->display == display)
      return c->x_cursor;
  }

  Bool dummy;
  Cursor x_cursor = XMakeCursor(cursor_id, dummy, display);
  if (x_cursor) {
    wxXCursor *c = new wxXCursor;

    c->x_cursor = x_cursor;
    c->display = display;
    xcursors.Append(c);
  }

  return x_cursor;
}

wxCursor::~wxCursor (void)
{
  xcursors.DeleteContents(TRUE);
}

// Global cursor setting
void 
wxSetCursor (wxCursor * cursor)
{
  wxFlushEvents ();
}

// Misc. functions

// Return TRUE if we have a colour display
Bool 
wxColourDisplay (void)
{
  Display *dpy = wxGetDisplay();

  if (DefaultDepth (dpy, DefaultScreen (dpy)) < 2)
    return FALSE;
  else
    return TRUE;
}

// Returns depth of screen
int 
wxDisplayDepth (void)
{
  Display *dpy = wxGetDisplay();
  return DefaultDepth (dpy, DefaultScreen (dpy));
}

// Get size of display
void 
wxDisplaySize (int *width, int *height)
{
  Display *dpy = wxGetDisplay();
  
  if (wxTheApp->topLevel)
    {
      *width = DisplayWidth (dpy, DefaultScreen (dpy));
      *height = DisplayHeight (dpy, DefaultScreen (dpy));
    }
  else
    {
      // A good bet!
      *width = 1024;
      *height = 768;
    }
}

IMPLEMENT_DYNAMIC_CLASS(wxBitmap, wxObject)

wxBitmap::wxBitmap (void)
{
  __type = wxTYPE_BITMAP;
  ok = FALSE;
  width = 0;
  height = 0;
  depth = 0;
  numColors = 0;
  bitmapColourMap = NULL;
  x_pixmap = 0;
  freePixmap = FALSE;

  free_colors = NULL;
  free_colors_num = 0;

  insensPixmap = 0;
  labelPixmap = 0;
  armPixmap = 0;
  insensImage = 0;
  image = 0;

#if !WXGARBAGE_COLLECTION_ON
  wxTheBitmapList->Append (this);
#endif
}


wxBitmap::wxBitmap (char bits[], int the_width, int the_height)
{
  __type = wxTYPE_BITMAP;
  depth = 1;
  width = the_width;
  height = the_height;
  numColors = 0;
  bitmapColourMap = NULL;
  free_colors = NULL;
  free_colors_num = 0;

  insensPixmap = 0;
  labelPixmap = 0;
  armPixmap = 0;
  insensImage = 0;
  image = 0;

  freePixmap = TRUE;
  Display *dpy = display = wxGetDisplay(); /* MATTHEW: [4] Use wxGetDisplay */
  x_pixmap = XCreateBitmapFromData (dpy, RootWindow (dpy, DefaultScreen (dpy)), bits, width, height);
  if (x_pixmap)
    ok = TRUE;
  else
    ok = FALSE;

#if !WXGARBAGE_COLLECTION_ON
  wxTheBitmapList->Append (this);
#endif

  WXGC_IGNORE(selectedTo);
}

wxBitmap::wxBitmap (char *bitmap_file, long flags)
{
  __type = wxTYPE_BITMAP;
  free_colors = NULL;
  free_colors_num = 0;
  numColors = 0;
  bitmapColourMap = NULL;
  display = wxGetDisplay(); /* MATTHEW: [4] Display */
  insensPixmap = 0;
  labelPixmap = 0;
  armPixmap = 0;
  insensImage = 0;
  image = 0;
  bitmapColourMap = NULL;
  width = 0;
  height = 0;
  depth = 0;
  freePixmap = FALSE;

  (void)LoadFile(bitmap_file, (int)flags);

#if !WXGARBAGE_COLLECTION_ON
  wxTheBitmapList->Append (this);
#endif

  WXGC_IGNORE(selectedTo);
}

#if USE_XPM_IN_X
wxBitmap::wxBitmap(char **data, wxItem *anItem)
{
  __type = wxTYPE_BITMAP;
  free_colors = NULL;
  free_colors_num = 0;
  numColors = 0;
  bitmapColourMap = NULL;
  insensPixmap = 0;
  labelPixmap = 0;
  armPixmap = 0;
  insensImage = 0;
  image = 0;
  width = 0;
  height = 0;
  depth = 0;
  freePixmap = FALSE;

  XpmAttributes xpmAttr;

  ok = FALSE;

  xpmAttr.valuemask = XpmReturnInfos;	/* nothing yet, but get infos back */
  Display *dpy = wxGetDisplay();

  display = dpy; /* MATTHEW: [4] Remember the display */

  XpmColorSymbol symbolicColors[4];
  if (anItem && anItem->handle) {
		symbolicColors[0].name = "foreground";
		symbolicColors[0].value = NULL;
		symbolicColors[1].name = "background";
		symbolicColors[1].value = NULL;
		XtVaGetValues((Widget)anItem->handle,
			XmNforeground,  &symbolicColors[0].pixel,
			XmNbackground,  &symbolicColors[1].pixel,NULL);
		xpmAttr.numsymbols = 2;
		xpmAttr.colorsymbols = symbolicColors;
		xpmAttr.valuemask |= XpmColorSymbols;	// add flag
	}

  Pixmap  pixmap;
  int ErrorStatus = XpmCreatePixmapFromData(dpy, RootWindow(dpy, DefaultScreen(dpy)),
               data, &pixmap, (Pixmap *) NULL, &xpmAttr);
  if (ErrorStatus == XpmSuccess)
  {
    // Set attributes
    width=xpmAttr.width;
    height = xpmAttr.height;
    if ( xpmAttr.npixels > 2 )
    {
	depth = 8;	// next time not just a guess :-) ...
    } else
    {
	depth = 1;	// mono	
    }
    numColors = xpmAttr.npixels;
    XpmFreeAttributes(&xpmAttr);
    ok = TRUE;
    x_pixmap = pixmap;
  }
  else
  {
//		XpmDebugError(ErrorStatus, NULL);
		ok = False;
  }

  WXGC_IGNORE(selectedTo);
}
#endif

/*
 * Create a new bitmap of a given size and depth
 *
 */

wxBitmap::wxBitmap (int w, int h, Bool b_and_w)
{
  __type = wxTYPE_BITMAP;
  numColors = 0;
  bitmapColourMap = NULL;
  free_colors = NULL;
  free_colors_num = 0;
  bitmapColourMap = NULL;
  (void)Create(w, h, b_and_w ? 1 : -1);

  WXGC_IGNORE(selectedTo);
}

wxBitmap::~wxBitmap (void)
{
  FreeBitmapImage();
}

void wxBitmap::SetupBitmapImage()
{
  // code for wxItem
  XImage *src_image = XGetImage(display, x_pixmap, 0, 0, width, height, AllPlanes, ZPixmap);

  int bpl = (width + 7) >> 3, ht;
  char *data = new WXGC_ATOMIC char[height * bpl];
  char *bits = new WXGC_ATOMIC char[height * bpl], *bptr;

  memset(data, 0, height * bpl);
  memset(bits, 0, height * bpl);

  for (bptr = bits, ht = 0; ht < height; ht++, bptr += bpl) {
    int wd, bit = 0x01, byte = 0, index = 0;
    for (wd = 0; wd < width; wd++) {
      if (XGetPixel(src_image, wd, ht))
	byte |= bit;
      bit = bit << 1;
      if (bit > 0x80) {
	bptr[index++] = byte;
	byte = 0;
	bit = 0x01;
      }
    }
    if (byte)
      bptr[index] = byte;
  }

  XDestroyImage(src_image);  

  image = new XImage;
  image->width = width;
  image->height = height;
  image->data = bits;
  image->depth = 1;
  image->xoffset = 0;
  image->format = XYBitmap;
  image->byte_order = LSBFirst;
  image->bitmap_unit = 8;
  image->bitmap_bit_order = LSBFirst;
  image->bitmap_pad = 8;
  image->bytes_per_line = bpl;

  char tmp[128];
  sprintf (tmp, "Im%x", (unsigned int)image);
  XmInstallImage (image, tmp);

  // Build our manually stipped pixmap.

  for (int i = 0; i < height; i++)
    {
      int mask = i % 2 ? 0x55 : 0xaa;
      for (int j = 0; j < bpl; j++)
	data[i * bpl + j] = bits[i * bpl + j] & mask;
    }
  insensImage = new XImage;
  insensImage->width = width;
  insensImage->height = height;
  insensImage->data = data;
  insensImage->depth = 1;
  insensImage->xoffset = 0;
  insensImage->format = XYBitmap;
  insensImage->byte_order = LSBFirst;
  insensImage->bitmap_unit = 8;
  insensImage->bitmap_bit_order = LSBFirst;
  insensImage->bitmap_pad = 8;
  insensImage->bytes_per_line = bpl;

  sprintf (tmp, "Not%x", (unsigned int)insensImage);
  XmInstallImage (insensImage, tmp);
}

void wxBitmap::FreeBitmapImage()
{
  if (x_pixmap && freePixmap)
    XFreePixmap (display, x_pixmap);

  if (free_colors)
    {
      int screen = DefaultScreen(display);
      Colormap cmp = DefaultColormap(display,screen);
      long llp;
      for(llp = 0;llp < free_colors_num;llp++)
	XFreeColors(display, cmp, &free_colors[llp], 1, 0L);
      delete free_colors;
      free_colors = NULL;
    };

  if (bitmapColourMap) {
    delete bitmapColourMap;
    bitmapColourMap = NULL;
  }

  free_colors = NULL;
  free_colors_num = 0;

  if (labelPixmap) {
    XmDestroyPixmap (DefaultScreenOfDisplay (display), labelPixmap);
    labelPixmap = 0;
  }

  if (armPixmap) {
    XmDestroyPixmap (DefaultScreenOfDisplay (display), armPixmap);
    armPixmap = 0;
  }

  if (insensPixmap) {
    XmDestroyPixmap (DefaultScreenOfDisplay (display), insensPixmap);
    insensPixmap = 0;
  }

  if (image)
    {
      XmUninstallImage (image);
      // delete[]image->data;
      // XtFree ((char *)image);
      image = NULL;
    }

  if (insensImage)
    {
      XmUninstallImage (insensImage);
      // delete[]insensImage->data;
      // XtFree ((char *) insensImage);
      insensImage = NULL;
    }
}

static int errorFlagged;
static int FlagError(Display*, XErrorEvent*)
{
  errorFlagged = 1;
  return 0;
}

Bool wxBitmap::Create(int w, int h, int d)
{
  Display *dpy = wxGetDisplay();

  width = w;
  height = h;
  depth = d;
  if (d < 1)
    depth = DefaultDepth(dpy, DefaultScreen(dpy)); /* MATTHEW: [13] */

  freePixmap = TRUE;

  insensPixmap = 0;
  labelPixmap = 0;
  armPixmap = 0;
  insensImage = 0;
  image = 0;

  display = dpy; /* MATTHEW: [4] Remember the display */

  int (*old_handler)(Display*, XErrorEvent*);
  old_handler = XSetErrorHandler(FlagError);
  errorFlagged = 0;

  x_pixmap = XCreatePixmap (dpy, RootWindow (dpy, DefaultScreen (dpy)),
			    width, height, depth);

  XSync(dpy, FALSE);

  if (errorFlagged)
    x_pixmap = 0;
  
  XSetErrorHandler(old_handler);

  if (x_pixmap)
    ok = TRUE;
  else
    ok = FALSE;
  return ok;
}

Bool wxBitmap::LoadFile(char *filename, long flags)
{
  if (selectedIntoDC)
    return FALSE;

  FreeBitmapImage();

  if (flags & wxBITMAP_TYPE_XBM)
  {
    insensPixmap = 0;
    labelPixmap = 0;
    armPixmap = 0;
    insensImage = 0;
    image = 0;
    width = 0;
    height = 0;
    
    freePixmap = TRUE;
    int hotX, hotY;
    unsigned int w, h;
    Display *dpy = display; /* MATTHEW: [4] Use display */
    int value = XReadBitmapFile (dpy, RootWindow (dpy, DefaultScreen (dpy)),
                        filename, &w, &h, &x_pixmap, &hotX, &hotY);
    width = w;
    height = h;
    depth = 1;
    if ((value == BitmapFileInvalid) ||
        (value == BitmapOpenFailed) ||
        (value == BitmapNoMemory))
      {
        ok = FALSE;
        x_pixmap = 0;
      }
    else {
      ok = TRUE;
    }
  }
#if USE_XPM_IN_X
  else if (flags & wxBITMAP_TYPE_XPM)
  {
    Display *dpy = display; /* MATTHEW: [4] Use display */

    XpmAttributes xpmAttr;
    Pixmap  pixmap;

    ok = FALSE;
    xpmAttr.valuemask = XpmReturnInfos | XpmCloseness | XpmDepth;
    xpmAttr.closeness = 40000;
    xpmAttr.depth = DefaultDepth(dpy, DefaultScreen(dpy));
    int errorStatus = XpmReadFileToPixmap(dpy,
        RootWindow(dpy, DefaultScreen(dpy)), filename,
        &pixmap, (Pixmap *) NULL, &xpmAttr);
    if (errorStatus == XpmSuccess)
    {
        x_pixmap = pixmap;

	width=xpmAttr.width;
	height = xpmAttr.height;
        depth = xpmAttr.depth;

	numColors = xpmAttr.npixels;

	XpmFreeAttributes(&xpmAttr);

        ok = TRUE;
    } else
    {
//      XpmDebugError(errorStatus, filename);
      ok = FALSE;
      return FALSE;
    }
    return ok;
  }
#endif
#if USE_IMAGE_LOADING_IN_X
  else if ((flags & wxBITMAP_TYPE_ANY) || (flags & wxBITMAP_TYPE_BMP) ||
           (flags & wxBITMAP_TYPE_GIF))
  {
    // BUGBUG: no depth information returned!!!
    wxColourMap *cmap = NULL;
    Bool success = FALSE;
    if (flags & wxBITMAP_DISCARD_COLOURMAP)
      success = wxLoadIntoBitmap(filename, this);
    else
      success = wxLoadIntoBitmap(filename, this, &cmap);
    if (!success && cmap)
    {
      delete cmap;
      cmap = NULL;
    }
    if (cmap)
      bitmapColourMap = cmap;
    return success;
  }
#endif
  return FALSE;
}

static int write_pixmap_as_bitmap(Display *display, Pixmap pm, char *fname, 
				   int width, int height)
{
  char *data, *pos;
  int rw, ok, i, j;

  XImage *img = XGetImage(display, pm, 0, 0, width, height, AllPlanes, ZPixmap);

  rw = ((width + 1) >> 3);

  data = new char[rw * height];

  pos = data;
  for (j = 0; j < height; j++, pos += rw) {
    int bit = 0x01, v = 0, count = 0;
    char *row = pos;

    for (i = 0; i < width; i++) {
      XColor xcol;
      unsigned long pixel;

      pixel = XGetPixel(img, i, j);
      if (xcol.pixel != pixel) {
	xcol.pixel = pixel;
	
	XQueryColor(display, 
		    DefaultColormapOfScreen(DefaultScreenOfDisplay(display)), 
		    &xcol);
      }

      if ((xcol.red >> 8) != 255
	  || (xcol.green >> 8) != 255
	  || (xcol.blue >> 8) != 255)
	v += bit;

      bit = bit << 1;
      count++;
      if (count == 8) {
	*(row++) = v;
	v = 0;
	bit = 0x01;
	count = 0;
      }
    }
    if (bit != 0x01)
      *row = v;
  }

  Pixmap bm = XCreateBitmapFromData(display, pm, data, width, height);

  ok = (XWriteBitmapFile(display, fname, bm, width, height, 0, 0)
	== BitmapSuccess);

  XFreePixmap(display, bm);

  XDestroyImage(img);

  return ok;
} 

Bool wxBitmap::SaveFile(char *filename, int typ, wxColourMap *cmap)
{
  if (selectedTo) 
    selectedTo->EndSetPixel();
  
  switch (typ)
  {
#if USE_IMAGE_LOADING_IN_X
/* IMAGE SAVING NOT IN X YET
    case wxBITMAP_TYPE_BMP:
    {
      wxColourMap *actualCmap = cmap;
      if (!actualCmap)
        actualCmap = bitmapColourMap;
      return wxSaveBitmap(filename, this, actualCmap);
      break;
    }
 */
#endif
    case wxBITMAP_TYPE_XBM: 
    {
      if (ok && x_pixmap) {
	if (depth == 1)
	  return (XWriteBitmapFile(display, filename, x_pixmap,
				   width, height,
				   -1, -1)
		  == BitmapSuccess);
	else {
	  return write_pixmap_as_bitmap( display, x_pixmap,  filename, 
					width, height);
	}
      }
      return FALSE;
    }
    case wxBITMAP_TYPE_XPM:
    {
      if (ok && x_pixmap)
      {
        Display *dpy =  display; /* MATTHEW: [4] Use display */
        int errorStatus = XpmWriteFileFromPixmap(dpy, filename,
          x_pixmap, (Pixmap) NULL, (XpmAttributes *) NULL);
        if (errorStatus == XpmSuccess)
          return TRUE;
        else
          return FALSE;
      }
      else
        return FALSE;
      break;
    }
    default:
      break;
  }
  return FALSE;
}

Pixmap wxBitmap::GetLabelPixmap (Widget w)
{
  if (!image && (depth == 1))
    SetupBitmapImage();

  if (image == 0)
    return x_pixmap;

  if (labelPixmap)
    return labelPixmap;

  Display *dpy = display; /* MATTHEW: [4] Use display */

#ifdef FOO
/*
   If we do:
   if (labelPixmap) return labelPixmap;
   things can be wrong, because colors can have been changed.

   If we do:
   if (labelPixmap)
   XmDestroyPixmap(DefaultScreenOfDisplay(dpy),labelPixmap) ;
   we got BadDrawable if the pixmap is referenced by multiples widgets

   this is a catch22!!

   So, before doing thing really clean, I just do nothing; if the pixmap is
   referenced by many widgets, Motif performs caching functions.
   And if pixmap is referenced with multiples colors, we just have some
   memory leaks... I hope we can deal with them...
 */
  // Must be destroyed, because colours can have been changed!
  if (labelPixmap)
    XmDestroyPixmap (DefaultScreenOfDisplay (dpy), labelPixmap);
#endif

  char tmp[128];
  sprintf (tmp, "Im%x", (unsigned int)image);

  Pixel fg, bg;

  while (XmIsGadget (w))
    w = XtParent (w);
  XtVaGetValues(w, XmNbackground, &bg, XmNforeground, &fg, NULL);

  labelPixmap = XmGetPixmap (DefaultScreenOfDisplay (dpy), tmp, fg, bg);

  return labelPixmap;
}

Pixmap wxBitmap::GetArmPixmap (Widget w)
{
  if (!image && (depth == 1))
    SetupBitmapImage();

  if (image == 0)
    return x_pixmap;

  if (armPixmap)
    return armPixmap;

  Display *dpy =display; /* MATTHEW: [4] Use display */
#ifdef FOO
  See GetLabelPixmap () comment
  // Must be destroyed, because colours can have been changed!
  if (armPixmap)
      XmDestroyPixmap (DefaultScreenOfDisplay (dpy), armPixmap);
#endif

  char tmp[128];
  sprintf (tmp, "Im%x", (unsigned int)image);

  Pixel fg, bg;

  XtVaGetValues (w, XmNarmColor, &bg, NULL);
  while (XmIsGadget (w))
    w = XtParent (w);
  XtVaGetValues (w, XmNforeground, &fg, NULL);

  armPixmap = XmGetPixmap (DefaultScreenOfDisplay (dpy), tmp, fg, bg);

  return armPixmap;
}

Pixmap wxBitmap::GetInsensPixmap (Widget w)
{
  Display *dpy = display;  /* MATTHEW: [4] Use display */

  if (!image && (depth == 1))
    SetupBitmapImage();

  if (insensImage == 0)
    return x_pixmap;

  if (insensPixmap)
    return insensPixmap;

#ifdef FOO
  See GetLabelPixmap () comment
  // Must be destroyed, because colours can have been changed!
  if (insensPixmap)
      XmDestroyPixmap (DefaultScreenOfDisplay (dpy), insensPixmap);
#endif

  char tmp[128];
  sprintf (tmp, "Not%x", (unsigned int)insensImage);

  Pixel fg, bg;

  while (XmIsGadget (w))
    w = XtParent (w);
  XtVaGetValues (w, XmNbackground, &bg, XmNforeground, &fg, NULL);

  insensPixmap = XmGetPixmap (DefaultScreenOfDisplay (dpy), tmp, fg, bg);

  return insensPixmap;
}

// We may need this sometime...
#if 0
#ifdef wx_motif
/****************************************************************************

NAME
	XCreateInsensitivePixmap - create a grayed-out copy of a pixmap

SYNOPSIS
	Pixmap XCreateInsensitivePixmap( Display *display, Pixmap pixmap )

DESCRIPTION
	This function creates a grayed-out copy of the argument pixmap, suitable
	for use as a XmLabel's XmNlabelInsensitivePixmap resource.

RETURN VALUES
	The return value is the new Pixmap id or zero on error.  Errors include
	a NULL display argument or an invalid Pixmap argument.

ERRORS
	If one of the XLib functions fail, it will produce a X error.  The
	default X error handler prints a diagnostic and calls exit().

SEE ALSO
	XCopyArea(3), XCreateBitmapFromData(3), XCreateGC(3), XCreatePixmap(3),
	XFillRectangle(3), exit(2)

AUTHOR
	John R Veregge - john@puente.jpl.nasa.gov
	Advanced Engineering and Prototyping Group (AEG)
	Information Systems Technology Section (395)
	Jet Propulsion Lab - Calif Institute of Technology

*****************************************************************************/

Pixmap
XCreateInsensitivePixmap( Display *display, Pixmap pixmap )

{
static
	char	stipple_data[] = 
		{
			0x55, 0x55, 0xAA, 0xAA, 0x55, 0x55, 0xAA, 0xAA,
			0x55, 0x55, 0xAA, 0xAA, 0x55, 0x55, 0xAA, 0xAA,
			0x55, 0x55, 0xAA, 0xAA, 0x55, 0x55, 0xAA, 0xAA,
			0x55, 0x55, 0xAA, 0xAA, 0x55, 0x55, 0xAA, 0xAA 
		};
    GC		gc;
    Pixmap	ipixmap, stipple;
    unsigned	width, height, depth;

    Window	window;	/* These return values */
    unsigned	border;	/* from XGetGeometry() */
    int		x, y;	/* are not needed.     */

    ipixmap = 0;

    if ( NULL == display || 0 == pixmap )
	return ipixmap;

    if ( 0 == XGetGeometry( display, pixmap, &window, &x, &y,
			    &width, &height, &border, &depth )
       )
	return ipixmap; /* BadDrawable: probably an invalid pixmap */

    /* Get the stipple pixmap to be used to 'gray-out' the argument pixmap.
    */
    stipple = XCreateBitmapFromData( display, pixmap, stipple_data, 16, 16 );
    if ( 0 != stipple )
    {
	gc = XCreateGC( display, pixmap, (XtGCMask)0, (XGCValues*)NULL );
	if ( NULL != gc )
	{
	    /* Create an identical copy of the argument pixmap.
	    */
	    ipixmap = XCreatePixmap( display, pixmap, width, height, depth );
	    if ( 0 != ipixmap )
	    {
		/* Copy the argument pixmap into the new pixmap.
		*/
		XCopyArea( display, pixmap, ipixmap,
			   gc, 0, 0, width, height, 0, 0 );

		/* Refill the new pixmap using the stipple algorithm/pixmap.
		*/
		XSetStipple( display, gc, stipple );
		XSetFillStyle( display, gc, FillStippled );
		XFillRectangle( display, ipixmap, gc, 0, 0, width, height );
	    }
	    XFreeGC( display, gc );
	}
	XFreePixmap( display, stipple );
    }
    return ipixmap;
}
#endif
#endif

