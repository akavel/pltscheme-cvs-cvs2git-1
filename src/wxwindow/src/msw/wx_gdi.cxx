/*
 * File:	wx_gdi.cc
 * Purpose:	GDI (Graphics Device Interface) objects and functions
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wx_gdi.cxx,v 1.1 1997/12/22 16:12:03 mflatt Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "%W% %G%"; */

#if defined(_MSC_VER)
# include "wx.h"
#else

#include "wx_setup.h"

#include "wx_utils.h"
#include "wx_main.h"
#include "wx_gdi.h"
#include "wx_list.h"
#include "wx_wmgr.h"

#endif

#if USE_IMAGE_LOADING_IN_MSW
#include "..\..\utils\dib\dib.h"
#endif

#if USE_XPM_IN_MSW
#define FOR_MSW 1
#include "xpm34.h"
#endif
#include "wximgfil.h"
#include "wximgxbm.h"

#if USE_RESOURCE_LOADING_IN_MSW
// Commented out to avoid generated dependency with an absolute path
// #include "..\..\utils\rcparser\src\wxcurico.h"
// #include "..\..\utils\rcparser\src\cric_prv.h"
#endif

Bool wxMakeBitmapAndPalette(LPBITMAPINFOHEADER lpInfo,
			HPALETTE * phPal, HBITMAP * phBitmap);

// #define DEBUG_CREATE
// #define TRACE_GDI

IMPLEMENT_DYNAMIC_CLASS(wxFont, wxObject)

wxFont::wxFont(void)
{
#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxTheFontList->Append(this);
#endif

  Create(12, wxDEFAULT, wxNORMAL, wxNORMAL, FALSE);
}

/* Constructor for a font. Note that the real construction is done
 * in wxDC::SetFont, when information is available about scaling etc.
 */
wxFont::wxFont(int PointSize, int Family, int Style, int Weight, Bool Underlined):
  wxbFont(PointSize, Family, Style, Weight, Underlined)
{
#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxTheFontList->Append(this);
#endif

  Create(PointSize, Family, Style, Weight, Underlined);
}

/* MATTHEW: [4] New font system */
wxFont::wxFont(int PointSize, const char *Face, int Family, int Style, int Weight, Bool Underlined):
  wxbFont(PointSize, Family, Style, Weight, Underlined)
{
#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxTheFontList->Append(this);
#endif

  Create(PointSize, 
	 wxTheFontNameDirectory.FindOrCreateFontId(Face, Family), 
	 Style, Weight, Underlined);
}

/* MATTHEW: [4] New font system */
Bool wxFont::Create(int PointSize, int FontId, int Style, int Weight, Bool Underlined)
{
  fontid = FontId;
  family = wxTheFontNameDirectory.GetFamily(fontid);
  style = Style;
  weight = Weight;
  point_size = PointSize;
  underlined = Underlined;

  temporary = FALSE;
  cfont = NULL;

  return TRUE;
}

wxFont::~wxFont()
{
  if (cfont)
  {
    DeleteObject(cfont);
  }
  cfont = NULL ;
  wxTheFontList->DeleteObject(this);
}

/* MATTHEW: [4] Use wxTheFontNameDirectory */
void wxFont::BuildInternalFont(HDC WXUNUSED(dc))
{
  if (!cfont)
  {
    BYTE ff_italic;
    int ff_weight = 0;
    int ff_family = 0;
    char *ff_face = NULL;
    char *screen;
    
    ff_face = wxTheFontNameDirectory.GetScreenName(fontid, weight, style);
    if (!*ff_face)
      ff_face = NULL;

    switch (family)
    {
      case wxSCRIPT:     ff_family = FF_SCRIPT ;
                         break ;
      case wxDECORATIVE: ff_family = FF_DECORATIVE;
                         break;
		case wxROMAN:      ff_family = FF_ROMAN;
                         break;
      case wxTELETYPE:
      case wxMODERN:     ff_family = FF_MODERN;
                         break;
      case wxSWISS:      ff_family = FF_SWISS;
                         break;
      case wxDEFAULT:
		default:           ff_family = FF_SWISS;
    }

    if (style == wxITALIC || style == wxSLANT)
      ff_italic = 1;
    else
      ff_italic = 0;

    if (weight == wxNORMAL)
      ff_weight = FW_NORMAL;
    else if (weight == wxLIGHT)
      ff_weight = FW_LIGHT;
    else if (weight == wxBOLD)
      ff_weight = FW_BOLD;
/* Always calculate fonts using the screen DC (is this the best strategy?)
 * There may be confusion if a font is selected into a printer
 * DC (say), because the height will be calculated very differently.
    // What sort of display is it?
    int technology = ::GetDeviceCaps(dc, TECHNOLOGY);

    int nHeight;
    
    if (technology != DT_RASDISPLAY && technology != DT_RASPRINTER)
    {
      // Have to get screen DC Caps, because a metafile will return 0.
      HDC dc2 = ::GetDC(NULL);
      nHeight = point_size*GetDeviceCaps(dc2, LOGPIXELSY)/72;
      ::ReleaseDC(NULL, dc2);
    }
    else
    {
      nHeight = point_size*GetDeviceCaps(dc, LOGPIXELSY)/72;
    }
*/
    // Have to get screen DC Caps, because a metafile will return 0.
    HDC dc2 = ::GetDC(NULL);
    int nHeight = point_size*GetDeviceCaps(dc2, LOGPIXELSY)/72;
    ::ReleaseDC(NULL, dc2);

    Bool ff_underline = underlined;
    
	 cfont = CreateFont(nHeight, 0, 0, 0,ff_weight,ff_italic,(BYTE)ff_underline,
					 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					 PROOF_QUALITY, DEFAULT_PITCH | ff_family, ff_face);

	/* MATTHEW: [13] Safety */
	if (!cfont)
		cfont = CreateFont(12, 0, 0, 0,FW_NORMAL,0,(BYTE)0,
					 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					 PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, NULL);

#ifdef DEBUG_CREATE
	 if (cfont==NULL) wxError("Cannot create font","Internal Error") ;
#endif

  }
}

HFONT wxFont::GetInternalFont(HDC dc)
{
  BuildInternalFont(dc) ;
  return cfont ;
}

/*
 * Colour map
 *
 */

IMPLEMENT_DYNAMIC_CLASS(wxColourMap, wxObject)

wxColourMap::wxColourMap(void)
{
  ms_palette = 0;
}

wxColourMap::wxColourMap(const int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue)
{
  Create(n, red, green, blue);
}

wxColourMap::~wxColourMap(void)
{
  if (ms_palette)
  {
    DeleteObject(ms_palette);
  }
}

Bool wxColourMap::Create(const int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue)
{
  if (ms_palette)
    return FALSE;
    
  NPLOGPALETTE npPal = (NPLOGPALETTE)LocalAlloc(LMEM_FIXED, sizeof(LOGPALETTE) + 
                        (WORD)n * sizeof(PALETTEENTRY));
  if (!npPal)
    return(FALSE);

  npPal->palVersion = 0x300;
  npPal->palNumEntries = n;

  int i;
  for (i = 0; i < n; i ++)
  {
    npPal->palPalEntry[i].peRed = red[i];
    npPal->palPalEntry[i].peGreen = green[i];
    npPal->palPalEntry[i].peBlue = blue[i];
    npPal->palPalEntry[i].peFlags = 0;
  }
  ms_palette = CreatePalette((LPLOGPALETTE)npPal);
  LocalFree((HANDLE)npPal);
  return TRUE;
}

int wxColourMap::GetPixel(const unsigned char red, const unsigned char green, const unsigned char blue)
{
  return ::GetNearestPaletteIndex(ms_palette, RGB(red, green, blue));
}

Bool wxColourMap::GetRGB(const int index, unsigned char *red, unsigned char *green, unsigned char *blue)
{
  if (index < 0 || index > 255)
         return FALSE;

  PALETTEENTRY entry;
  if (::GetPaletteEntries(ms_palette, index, 1, &entry))
  {
         *red = entry.peRed;
         *green = entry.peGreen;
         *blue = entry.peBlue;
         return TRUE;
  } else
         return FALSE;
}

// Pens

IMPLEMENT_DYNAMIC_CLASS(wxPen, wxObject)

wxPen::wxPen(void)
{
  stipple = NULL ;
  style = wxSOLID;
  width = 1;
  join = wxJOIN_ROUND ;
  cap = wxCAP_ROUND ;
  nb_dash = 0 ;
  dash = NULL ;
  cpen = NULL;
  my_old_cpen = NULL ;
  old_width = -1 ;
  old_style = -1 ;
  old_join  = -1 ;
  old_cap  = -1 ;
  old_nb_dash  = -1 ;
  old_dash  = NULL ;
  old_color  = 0 ;
  old_stipple = NULL ;

#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxThePenList->AddPen(this);
#endif
}

wxPen::~wxPen()
{
  if (my_old_cpen)
  {
    DeleteObject(my_old_cpen);
  }
  my_old_cpen = NULL ;

  if (cpen)
  {
    DeleteObject(cpen);
  }
  cpen = NULL ;

  wxThePenList->RemovePen(this);
}

wxPen::wxPen(wxColour& col, int Width, int Style)
{
  colour = col;
  stipple = NULL;
  width = Width;
  style = Style;
  join = wxJOIN_ROUND ;
  cap = wxCAP_ROUND ;
  nb_dash = 0 ;
  dash = NULL ;
  cpen = NULL ;
  my_old_cpen = NULL ;
  old_width = -1 ;
  old_style = -1 ;
  old_join  = -1 ;
  old_cap  = -1 ;
  old_nb_dash  = -1 ;
  old_dash  = NULL ;
  old_color  = 0 ;
  old_stipple = NULL ;

#ifndef WIN32
  // In Windows, only a pen of width = 1 can be dotted or dashed!
  if ((Style == wxDOT) || (Style == wxLONG_DASH) ||
      (Style == wxSHORT_DASH) || (Style == wxDOT_DASH) ||
      (Style == wxUSER_DASH))
    width = 1;
#else
/***
  DWORD vers = GetVersion() ;
  WORD  high = HIWORD(vers) ; // high bit=0 for NT, 1 for Win32s
  // Win32s doesn't support wide dashed pens

  if ((high&0x8000)!=0)
***/
  if (wxGetOsVersion()==wxWIN32S)
  {
    // In Windows, only a pen of width = 1 can be dotted or dashed!
    if ((Style == wxDOT) || (Style == wxLONG_DASH) ||
        (Style == wxSHORT_DASH) || (Style == wxDOT_DASH) ||
        (Style == wxUSER_DASH))
      width = 1;
  }
#endif 
  ChangePen() ;

#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxThePenList->AddPen(this);
#endif
}

wxPen::wxPen(const char *col, int Width, int Style)
{
  colour = col;
  stipple = NULL ;
  width = Width;
  style = Style;
  join = wxJOIN_ROUND ;
  cap = wxCAP_ROUND ;
  nb_dash = 0 ;
  dash = NULL ;
  cpen = NULL ;
  my_old_cpen = NULL ;
  old_width = -1 ;
  old_style = -1 ;
  old_join  = -1 ;
  old_cap  = -1 ;
  old_nb_dash  = -1 ;
  old_dash  = NULL ;
  old_color  = 0 ;
  old_stipple = NULL ;

  // In Windows, only a pen of width = 1 can be dotted or dashed!
  if ((Style == wxDOT) || (Style == wxLONG_DASH) || (Style == wxSHORT_DASH) || (Style == wxDOT_DASH))
    width = 1;
    
  ChangePen() ;

#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxThePenList->AddPen(this);
#endif
}

void wxPen::ChangePen(void)
{
  // Handled by wxPen::SelectPen() !!
  if (style==wxTRANSPARENT)
    return ;

  Bool must_change = FALSE ;

  COLORREF ms_colour = 0 ;

  ms_colour = colour.pixel ;

  if (cpen==NULL)
    must_change = TRUE ;
  else
    must_change = !(width==old_width     &&
                    join==old_join       &&
                    cap==old_cap         &&
                    dash==old_dash       &&
                    nb_dash==old_nb_dash &&
                    style==old_style     &&
                    stipple==old_stipple &&
                    old_color==ms_colour) ;

  if (!must_change)
    return ;

  old_width = width ;
  old_join = join ;
  old_cap = cap ;
  old_dash = dash ;
  old_nb_dash = nb_dash ;
  old_style = style ;
  old_stipple = stipple ;
  old_color = ms_colour ;

/**
No! if cpen is currently selected into some DC, this results into
memory leakage with Win32s.
Rather, defers Delete till next selection
  if (cpen)
    DeleteObject(cpen);
  cpen = NULL ;
***/
  if (cpen)
  {
    if (my_old_cpen)
    {
      DeleteObject(my_old_cpen) ; // and hope that's ok...
    }
    my_old_cpen = cpen ;
    cpen = NULL ;
  }

  // Join style, Cap style, Pen Stippling only on Win32.
  // Currently no time to find equivalent on Win3.1, sorry
  // [if such equiv exist!!]
#ifdef WIN32
  if (join==wxJOIN_ROUND        &&
      cap==wxCAP_BUTT          &&
      style!=wxUSER_DASH        &&
      style!=wxSTIPPLE          &&
      style!=wxOPAQUE_STIPPLE
     )
    cpen = CreatePen(wx2msPenStyle(style), width, ms_colour);
  else
  {
    DWORD ms_style = PS_GEOMETRIC|wx2msPenStyle(style) ;

    LOGBRUSH logb ;

    switch(join)
    {
    case wxJOIN_BEVEL: ms_style |= PS_JOIN_BEVEL ; break ;
    case wxJOIN_MITER: ms_style |= PS_JOIN_MITER ; break ;
    default:
    case wxJOIN_ROUND: ms_style |= PS_JOIN_ROUND ; break ;
    }

    switch(cap)
    {
    case wxCAP_PROJECTING: ms_style |= PS_ENDCAP_SQUARE ; break ;
    case wxCAP_BUTT:       ms_style |= PS_ENDCAP_FLAT ;   break ;
    default:
    case wxCAP_ROUND:      ms_style |= PS_ENDCAP_ROUND ;  break ;
    }

    switch(style)
    {
    case wxSTIPPLE:
    case wxOPAQUE_STIPPLE:
      logb.lbStyle = BS_PATTERN ;
      if (stipple)
        logb.lbHatch = (LONG)stipple->ms_bitmap ;
      else
        logb.lbHatch = (LONG)0 ;
    break ;
    case wxBDIAGONAL_HATCH:
      logb.lbStyle = BS_HATCHED ;
      logb.lbHatch = HS_BDIAGONAL ;
    break ;
    case wxCROSSDIAG_HATCH:
      logb.lbStyle = BS_HATCHED ;
      logb.lbHatch = HS_DIAGCROSS ;
    break ;
    case wxFDIAGONAL_HATCH:
      logb.lbStyle = BS_HATCHED ;
      logb.lbHatch = HS_FDIAGONAL ;
    break ;
    case wxCROSS_HATCH:
      logb.lbStyle = BS_HATCHED ;
      logb.lbHatch = HS_CROSS ;
    break ;
    case wxHORIZONTAL_HATCH:
      logb.lbStyle = BS_HATCHED ;
      logb.lbHatch = HS_HORIZONTAL ;
    break ;
    case wxVERTICAL_HATCH:
      logb.lbStyle = BS_HATCHED ;
      logb.lbHatch = HS_VERTICAL ;
    break ;
    default:
      logb.lbStyle = BS_SOLID ;
    break ;
    }
    logb.lbColor = ms_colour ;
    wxDash *real_dash ;
    if (style==wxUSER_DASH&&nb_dash&&dash)
    {
      real_dash = new wxDash[nb_dash] ;
      int i;
      for (i=0;i<nb_dash;i++)
        real_dash[i] = dash[i] * width ;
    }
    else
    real_dash = NULL ;

/***
    DWORD vers = GetVersion() ;
    WORD  high = HIWORD(vers) ; // high bit=0 for NT, 1 for Win32s
    // Win32s doesn't have ExtCreatePen function...

    if ((high&0x8000)==0)
***/
#if 0
    if (wxGetOsVersion()==wxWINDOWS_NT)
#endif
      cpen = ExtCreatePen(ms_style,width,&logb,
                          style==wxUSER_DASH?nb_dash:0,real_dash);
#if 0
    else
      cpen = CreatePen(wx2msPenStyle(style), width, ms_colour);
#endif

    if (real_dash)
      delete [] real_dash ;
  }
#else
    cpen = CreatePen(wx2msPenStyle(style), width, ms_colour);
#endif
#ifdef DEBUG_CREATE
    if (cpen==NULL) wxError("Cannot create pen","Internal error") ;
#endif

  return ;
}

HPEN wxPen::SelectPen(HDC dc)
{
  HPEN prev_pen ;

  if (cpen && style!=wxTRANSPARENT)
  {
    prev_pen = ::SelectObject(dc,cpen) ;
  }
  else
  {
    HPEN nullPen = ::GetStockObject(NULL_PEN) ;
    prev_pen = ::SelectObject(dc , nullPen);
  }
  if (my_old_cpen)
  {
    DeleteObject(my_old_cpen) ; // Now, we can "safely" delete old pen
  }
  my_old_cpen = NULL;           // (We assume that pen belongs to only one DC)
  return prev_pen ;
 
}

int wx2msPenStyle(int wx_style)
{
  int cstyle;
/***
#ifdef WIN32
  DWORD vers = GetVersion() ;
  WORD  high = HIWORD(vers) ; // high bit=0 for NT, 1 for Win32s
#endif
***/
  switch (wx_style)
  {
    case wxDOT:
      cstyle = PS_DOT;
      break;
    case wxSHORT_DASH:
    case wxLONG_DASH:
      cstyle = PS_DASH;
      break;
    case wxTRANSPARENT:
      cstyle = PS_NULL;
      break;
    case wxUSER_DASH:
      // User dash style not supported on Win3.1, sorry...
#ifdef WIN32
      // Win32s doesn't have PS_USERSTYLE
/***
      if ((high&0x8000)==0)
***/
      if (wxGetOsVersion()==wxWINDOWS_NT)
        cstyle = PS_USERSTYLE ;
      else
        cstyle = PS_DOT ; // We must make a choice... This is mine!
#else
      cstyle = PS_DASH ;
#endif
      break ;
    case wxSOLID:
    default:
      cstyle = PS_SOLID;
      break;
  }
  return cstyle;
}


// Brushes
IMPLEMENT_DYNAMIC_CLASS(wxBrush, wxObject)

wxBrush::wxBrush(void)
{
  style = wxSOLID;
  stipple = NULL ;
  cbrush = NULL;
  my_old_cbrush = NULL ;
  old_color = 0 ;
  old_style = -1 ;
  old_stipple = NULL ;

#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxTheBrushList->AddBrush(this);
#endif
}

wxBrush::~wxBrush()
{
  if (my_old_cbrush)
  {
    DeleteObject(my_old_cbrush);
  }
  my_old_cbrush = NULL ;

  if (cbrush)
  {
    DeleteObject(cbrush);
  }
  cbrush = NULL ;

  wxTheBrushList->RemoveBrush(this);
}

wxBrush::wxBrush(wxColour& col, int Style)
{
  colour = col;
  style = Style;
  stipple = NULL ;
  cbrush = NULL;
  my_old_cbrush = NULL;
  old_color = 0 ;
  old_style = -1 ;
  old_stipple = NULL ;

  ChangeBrush();
  
#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxTheBrushList->AddBrush(this);
#endif
}

void wxBrush::ChangeBrush(void) 
{
  // Handled by wxBrush::SelectBrush() !!
  if (style==wxTRANSPARENT)
    return ;

  Bool must_change = FALSE ;

  COLORREF ms_colour = 0 ;

  ms_colour = colour.pixel ;

  if (cbrush==NULL)
    must_change = TRUE ;
  else
    must_change = !(style==old_style     &&
                    stipple==old_stipple &&
                    old_color==ms_colour) ;

  if (!must_change)
    return ;

/***
No! (see ChangePen)
  if (cbrush) DeleteObject(cbrush);
  cbrush = NULL ;
***/

  if (cbrush)
  {
    if (my_old_cbrush)
    {
      DeleteObject(my_old_cbrush) ;
    }
    my_old_cbrush = cbrush ;
    cbrush = NULL ;
  }

  switch (style)
  {
/****
    // Don't reset cbrush, wxTRANSPARENT is handled by wxBrush::SelectBrush()
    // this could save (many) time if frequently switching from
    // wxSOLID to wxTRANSPARENT, because Create... is not always called!!
    //
    case wxTRANSPARENT:
      cbrush = NULL;  // Must always select a suitable background brush
                      // - could choose white always for a quick solution
      break;
***/
    case wxBDIAGONAL_HATCH:
      cbrush = CreateHatchBrush(HS_BDIAGONAL,ms_colour) ;
    break ;
    case wxCROSSDIAG_HATCH:
      cbrush = CreateHatchBrush(HS_DIAGCROSS,ms_colour) ;
    break ;
    case wxFDIAGONAL_HATCH:
      cbrush = CreateHatchBrush(HS_FDIAGONAL,ms_colour) ;
    break ;
    case wxCROSS_HATCH:
      cbrush = CreateHatchBrush(HS_CROSS,ms_colour) ;
    break ;
    case wxHORIZONTAL_HATCH:
      cbrush = CreateHatchBrush(HS_HORIZONTAL,ms_colour) ;
    break ;
    case wxVERTICAL_HATCH:
      cbrush = CreateHatchBrush(HS_VERTICAL,ms_colour) ;
    break ;
    case wxSTIPPLE:
    case wxOPAQUE_STIPPLE:
      if (stipple)
        cbrush = CreatePatternBrush(stipple->ms_bitmap) ;
      else
        cbrush = CreateSolidBrush(ms_colour) ;
      break ;
    case wxSOLID:
    default:
      cbrush = CreateSolidBrush(ms_colour) ;
      break;
  }
  old_style = style ;
  old_stipple = stipple ;
  old_color = ms_colour ;
#ifdef DEBUG_CREATE
    if (cbrush==NULL) wxError("Cannot create brush","Internal error") ;
#endif

}

HBRUSH wxBrush::SelectBrush(HDC dc)
{
  HBRUSH prev_brush ;

  if (cbrush && style!=wxTRANSPARENT)
  {
    prev_brush = ::SelectObject(dc, cbrush);
  }
  else
  {
    HBRUSH nullBrush = ::GetStockObject(NULL_BRUSH);
    prev_brush = ::SelectObject(dc, nullBrush);
  }
  if (my_old_cbrush)
  {
    DeleteObject(my_old_cbrush) ;
  }
  my_old_cbrush = NULL ;
  return(prev_brush) ;

}

wxBrush::wxBrush(const char *col, int Style)
{
  colour = col;
  style = Style;
  stipple = NULL ;
  cbrush = NULL;
  my_old_cbrush = NULL;
  old_color = 0 ;
  old_style = -1 ;
  old_stipple = NULL ;

#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxTheBrushList->AddBrush(this);
#endif
}

// Icons
IMPLEMENT_DYNAMIC_CLASS(wxIcon, wxBitmap)

wxIcon::wxIcon(void) : wxBitmap()
{
  SetupIcon();
//  wxTheIconList->Append(this);
}

wxIcon::wxIcon(char bits[], int width, int height)
: wxBitmap(bits, width, height)
{
  SetupIcon();
}

wxIcon::wxIcon(const char *icon_file, long flags)
: wxBitmap((char *)icon_file, flags)
{
  SetupIcon();
//  wxTheIconList->Append(this);
}

wxIcon::~wxIcon(void)
{
  if (ms_icon)
    DestroyIcon(ms_icon);

//  wxTheIconList->DeleteObject(this);
}

void wxIcon::SetupIcon()
{
  __type = wxTYPE_ICON;
  if (Ok()) {
    ICONINFO info;
    info.fIcon = TRUE;
    info.hbmMask = info.hbmColor = ms_bitmap;
    ms_icon = CreateIconIndirect(&info);
    if (!ms_icon)
      ok = FALSE;
  } else
    ms_icon = NULL;
}

Bool wxIcon::LoadFile(char *icon_file, long flags)
{
  if (selectedIntoDC)
    return FALSE;

  if (ms_icon) {
    DestroyIcon(ms_icon);
    ms_icon = NULL;
  }
  if (ms_bitmap) {
    DeleteObject(ms_bitmap);
    ms_bitmap = NULL;
  }

  if ((flags & wxBITMAP_TYPE_ICO_RESOURCE)
      || (flags & wxBITMAP_TYPE_ICO)) {
    if (flags & wxBITMAP_TYPE_ICO)
      ms_icon = LoadImage(wxhInstance, icon_file, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
    else
      ms_icon = LoadIcon(wxhInstance, icon_file);
    if (ms_icon) 
    {
      ICONINFO info ;
      if (GetIconInfo(ms_icon, &info))
      {
        HBITMAP ms_bitmap = info.hbmMask ;
        if (ms_bitmap)
        {
          BITMAP bm;
          GetObject(ms_bitmap, sizeof(BITMAP), (LPSTR) &bm);
          width = bm.bmWidth;
          height = bm.bmHeight;
        }
        if (info.hbmMask)
          DeleteObject(info.hbmMask) ;
        if (info.hbmColor)
          DeleteObject(info.hbmColor) ;
      }
    }
    ok = (ms_icon != 0);
  } else {
    wxBitmap::LoadFile(icon_file, flags);
    if (Ok())
      SetupIcon();
  }

  return Ok();
}

// Cursors
IMPLEMENT_DYNAMIC_CLASS(wxCursor, wxBitmap)

wxCursor::wxCursor(void)
{
  __type = wxTYPE_CURSOR;
  width = 32; height = 32;
  ms_cursor = NULL ;
  destroyCursor = FALSE;
//  wxTheCursorList->Append(this) ;
}

wxCursor::wxCursor(char WXUNUSED(bits)[], int WXUNUSED(width), int WXUNUSED(height))
{
  __type = wxTYPE_CURSOR;
  ms_cursor = NULL ;
  destroyCursor = FALSE;
//  wxTheCursorList->Append(this) ;
}

wxCursor::wxCursor(const char *cursor_file, long flags, int hotSpotX, int hotSpotY)
{
  __type = wxTYPE_CURSOR;
  destroyCursor = FALSE;
  ms_cursor = 0;
  ok = FALSE;
  if (flags & wxBITMAP_TYPE_CUR_RESOURCE)
  {
    ms_cursor = LoadCursor(wxhInstance, cursor_file);
    if (ms_cursor)
      ok = TRUE;
    else
      ok = FALSE;
  }
  else if (flags & wxBITMAP_TYPE_CUR)
  {
#if USE_RESOURCE_LOADING_IN_MSW
    ms_cursor = ReadCursorFile((char *)cursor_file, wxhInstance, &width, &height);
#endif
  }
  else if (flags & wxBITMAP_TYPE_ICO)
  {
#if USE_RESOURCE_LOADING_IN_MSW
    ms_cursor = IconToCursor((char *)cursor_file, wxhInstance, hotSpotX, hotSpotY, &width, &height);
#endif
  }
  else if (flags & wxBITMAP_TYPE_BMP)
  {
#if USE_RESOURCE_LOADING_IN_MSW
    HBITMAP hBitmap = 0;
    HPALETTE hPalette = 0;
    Bool success = ReadDIB((char *)cursor_file, &hBitmap, &hPalette);
    if (!success)
      return;
    if (hPalette)
      DeleteObject(hPalette);
    POINT pnt;
    pnt.x = hotSpotX;
    pnt.y = hotSpotY;
    ms_cursor = MakeCursorFromBitmap(wxhInstance, hBitmap, &pnt);
    DeleteObject(hBitmap);
    if (ms_cursor)
      ok = TRUE;
#endif
  }
//  wxTheCursorList->Append(this) ;
}

// Cursors by stock number
wxCursor::wxCursor(int cursor_type)
{
  __type = wxTYPE_CURSOR;
  switch (cursor_type)
  {
    case wxCURSOR_WAIT:
  case wxCURSOR_WATCH:
      ms_cursor = LoadCursor(NULL, IDC_WAIT);
      break;
    case wxCURSOR_IBEAM:
      ms_cursor = LoadCursor(NULL, IDC_IBEAM);
      break;
    case wxCURSOR_CROSS:
      ms_cursor = LoadCursor(NULL, IDC_CROSS);
      break;
    case wxCURSOR_SIZENWSE:
      ms_cursor = LoadCursor(NULL, IDC_SIZENWSE);
      break;
    case wxCURSOR_SIZENESW:
      ms_cursor = LoadCursor(NULL, IDC_SIZENESW);
      break;
    case wxCURSOR_SIZEWE:
      ms_cursor = LoadCursor(NULL, IDC_SIZEWE);
      break;
    case wxCURSOR_SIZENS:
      ms_cursor = LoadCursor(NULL, IDC_SIZENS);
      break;
    case wxCURSOR_CHAR:
    {
      ms_cursor = LoadCursor(NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_HAND:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_HAND");
      break;
    }
    case wxCURSOR_BULLSEYE:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_BULLSEYE");
      break;
    }
    case wxCURSOR_PENCIL:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_PENCIL");
      break;
    }
    case wxCURSOR_MAGNIFIER:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_MAGNIFIER");
      break;
    }
    case wxCURSOR_NO_ENTRY:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_NO_ENTRY");
      break;
    }
    case wxCURSOR_LEFT_BUTTON:
    {
      ms_cursor = LoadCursor(NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_RIGHT_BUTTON:
    {
      ms_cursor = LoadCursor(NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_MIDDLE_BUTTON:
    {
      ms_cursor = LoadCursor(NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_SIZING:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_SIZING");
      break;
    }
#if 0
    case wxCURSOR_WATCH:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_WATCH");
      break;
    }
#endif
    case wxCURSOR_SPRAYCAN:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_ROLLER");
      break;
    }
    case wxCURSOR_PAINT_BRUSH:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_PBRUSH");
      break;
    }
    case wxCURSOR_POINT_LEFT:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_PLEFT");
      break;
    }
    case wxCURSOR_POINT_RIGHT:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_PRIGHT");
      break;
    }
    case wxCURSOR_QUESTION_ARROW:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_QARROW");
      break;
    }
    case wxCURSOR_BLANK:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_BLANK");
      break;
    }
    default:
    case wxCURSOR_ARROW:
      ms_cursor = LoadCursor(NULL, IDC_ARROW);
      break;
  }
//  wxTheCursorList->Append(this) ;
  ok = !!ms_cursor;
}

wxCursor::~wxCursor(void)
{
//  wxTheCursorList->DeleteObject(this) ;
}

// Global cursor setting
void wxSetCursor(wxCursor *cursor)
{
  if (cursor && cursor->ms_cursor)
    ::SetCursor(cursor->ms_cursor);

  wxFlushEvents();
}

// Misc. functions

// Return TRUE if we have a colour display
Bool wxColourDisplay(void)
{
  HDC dc = ::GetDC(NULL);
  Bool flag;
  int num = GetDeviceCaps(dc, NUMCOLORS);
  if ((num < 0) || (num > 2))
    flag = TRUE;
  else
    flag = FALSE;
  ReleaseDC(NULL, dc);
  return flag;
}

// Returns depth of screen
int wxDisplayDepth(void)
{
  HDC dc = ::GetDC(NULL);
  int planes = GetDeviceCaps(dc, PLANES);
  int bitsPerPixel = GetDeviceCaps(dc, BITSPIXEL);
  int depth = planes*bitsPerPixel;
  ReleaseDC(NULL, dc);
  return depth;
}

// Get size of display
void wxDisplaySize(int *width, int *height)
{
  RECT r;

  if (SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0)) {
	*width = (r.right - r.left);
	*height = (r.bottom - r.top);
  } else {
	HDC dc = ::GetDC(NULL);
    *width = GetDeviceCaps(dc, HORZRES); *height = GetDeviceCaps(dc, VERTRES);
    ReleaseDC(NULL, dc);
  }
}

IMPLEMENT_DYNAMIC_CLASS(wxBitmap, wxObject)

wxBitmap::wxBitmap(void)
{
  __type = wxTYPE_BITMAP;
  ok = FALSE;
  width = 0;
  height = 0;
  depth = 0;
  ms_bitmap = NULL ;
  selectedInto = NULL;
  numColors = 0;
  bitmapColourMap = NULL;
#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxTheBitmapList->Append(this);
#endif
  WXGC_IGNORE(selectedInto);
}

wxBitmap::wxBitmap(char bits[], int the_width, int the_height, int no_bits)
{
  __type = wxTYPE_BITMAP;
  width = the_width ;
  height = the_height ;
  depth = no_bits = 1;
  numColors = 0;
  bitmapColourMap = NULL;

  ms_bitmap = CreateBitmap(the_width, the_height, no_bits, 1, bits);

  if (ms_bitmap)
    ok = TRUE;
  else
    ok = FALSE;

  selectedInto = NULL;
#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxTheBitmapList->Append(this);
#endif
  WXGC_IGNORE(selectedInto);
}

wxBitmap::wxBitmap(int w, int h, int d)
{
  __type = wxTYPE_BITMAP;
  ok = FALSE;
  width = w;
  height = h;
  depth = d;
  numColors = 0;
  selectedInto = NULL;
  bitmapColourMap = NULL;

  (void)Create(w, h, d);
#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxTheBitmapList->Append(this);
#endif
  WXGC_IGNORE(selectedInto);
}

wxBitmap::wxBitmap(char *bitmap_file, long flags)
{
  __type = wxTYPE_BITMAP;
  ok = FALSE;
  width = 0;
  height = 0;
  depth = 0;
  selectedInto = NULL;
  numColors = 0;
  bitmapColourMap = NULL;

  LoadFile(bitmap_file, (int)flags);
#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxTheBitmapList->Append(this);
#endif
  WXGC_IGNORE(selectedInto);
}

#if USE_XPM_IN_MSW
// Create from data
wxBitmap::wxBitmap(char **data, wxItem *WXUNUSED(anItem))
{
  __type = wxTYPE_BITMAP;
  selectedInto = NULL;
  bitmapColourMap = NULL;

  XImage *ximage;
  int     ErrorStatus;
  XpmAttributes xpmAttr;
  HDC     dc;

  ok = FALSE;
  numColors = 0;

  dc = ::CreateCompatibleDC(NULL);	/* memory DC */

  if (dc)
  {
    xpmAttr.valuemask = XpmReturnInfos;	/* get infos back */
    ErrorStatus = XpmCreateImageFromData(&dc, data,
         &ximage, (XImage **) NULL, &xpmAttr);

    if (ErrorStatus == XpmSuccess)
    {
      /* ximage is malloced and contains bitmap and attributes */
      ms_bitmap = ximage->bitmap;

      BITMAP  bm;
      GetObject(ms_bitmap, sizeof(bm), (LPSTR) & bm);

      width = (bm.bmWidth);
      height = (bm.bmHeight);
      depth = (bm.bmPlanes * bm.bmBitsPixel);
      numColors = xpmAttr.npixels;
      XpmFreeAttributes(&xpmAttr);

      XImageFree(ximage);	// releases the malloc, but does not detroy
			// the bitmap
      ok = TRUE;

    } else
    {
      ok = False;
//  XpmDebugError(ErrorStatus, NULL);
    }
    DeleteDC(dc);
  }
  WXGC_IGNORE(selectedInto);
}
#endif

Bool wxBitmap::Create(int w, int h, int d)
{
  width = w;
  height = h;
  depth = d;

  if (d > 0)
  {
    ms_bitmap = CreateBitmap(w, h, d, 1, NULL);
  }
  else
  {
    HDC dc = GetDC(NULL);
	 ms_bitmap = ::CreateCompatibleBitmap(dc, w, h);
	 ReleaseDC(NULL, dc);
	 depth = wxDisplayDepth();
  }
  if (ms_bitmap)
	 ok = TRUE;
  else
    ok = FALSE;
  return ok;
}

Bool wxBitmap::LoadFile(char *bitmap_file, long flags)  
{
  if (selectedIntoDC)
    return FALSE;

  /* Nevermind the palette */
  flags |= wxBITMAP_DISCARD_COLOURMAP;

  wxMemoryDC *oldSel = (wxMemoryDC *)selectedInto;

  ok = FALSE;
  width = 0;
  height = 0;
  depth = 0;
  
  if (oldSel)
    oldSel->SelectObject(NULL);

  if (ms_bitmap) {
    DeleteObject(ms_bitmap);
    ms_bitmap = NULL;
  }

  if (flags & wxBITMAP_TYPE_BMP_RESOURCE)
  {
    ms_bitmap = LoadBitmap(wxhInstance, bitmap_file);
    if (ms_bitmap)
    {
      ok = TRUE;
      BITMAP bm;
      GetObject(ms_bitmap, sizeof(BITMAP), (LPSTR) &bm);
      width = bm.bmWidth;
      height = bm.bmHeight;
      depth = bm.bmPlanes;
    }
  }

  else if (flags & wxBITMAP_TYPE_XBM)
  {
	  char *c;
	  int w, h;

	  c = wxLoadXBM(bitmap_file, &w, &h);
	  if (c) {
		HDC glob_dc = GetDC(NULL);
	    ms_bitmap = CreateCompatibleBitmap(glob_dc, w, h);
		ReleaseDC(NULL, glob_dc);
		if (ms_bitmap) {
			HDC dc = ::CreateCompatibleDC(NULL);
			
			if (dc)
			{
				HGDIOBJ orig = ::SelectObject(dc, ms_bitmap);
				char *p;
				COLORREF white = RGB(255, 255, 255);
				COLORREF black = RGB(0, 0, 0);
				int i, j;
				
				for (i = 0, p = c; i < h; i++)
				  for (j = 0; j < w; j++, p++)
					::SetPixel(dc, j, i, *p ? black : white);

				::SelectObject(dc, orig);
				DeleteDC(dc);

				ok = TRUE;
				width = w;
				height = h;
				depth = 1;
			} else {
				DeleteObject(ms_bitmap);
				ms_bitmap = NULL;
			}

		}
	  }
  }
#if USE_XPM_IN_MSW
  else if (flags & wxBITMAP_TYPE_XPM)
  {
    XImage *ximage;
    XpmAttributes xpmAttr;
    HDC     dc;

    ok = False;
	 dc = ::CreateCompatibleDC(NULL);
	 if (dc)
	 {
		xpmAttr.valuemask = XpmReturnPixels;
		int errorStatus = XpmReadFileToImage(&dc, bitmap_file, &ximage, (XImage **) NULL, &xpmAttr);
		DeleteDC(dc);
		if (errorStatus == XpmSuccess)
		{
	ms_bitmap = ximage->bitmap;

	BITMAP  bm;
	GetObject(ms_bitmap, sizeof(bm), (LPSTR) & bm);

	width = (bm.bmWidth);
	height = (bm.bmHeight);
	depth = (bm.bmPlanes * bm.bmBitsPixel);
	numColors = xpmAttr.npixels;
		  XpmFreeAttributes(&xpmAttr);
	XImageFree(ximage);
	
	ok = TRUE;
	}
      else
      {
		  ok = FALSE;
      }
    }
  }
#endif
#if USE_IMAGE_LOADING_IN_MSW
  else if ((flags & wxBITMAP_TYPE_BMP) 
	  || (flags & wxBITMAP_TYPE_ANY))
  {
    wxColourMap *cmap = NULL;
    Bool success = FALSE;
    if (flags & wxBITMAP_DISCARD_COLOURMAP)
      success = wxLoadIntoBitmap(bitmap_file, this);
    else
      success = wxLoadIntoBitmap(bitmap_file, this, &cmap);
    if (!success && cmap)
    {
      delete cmap;
		cmap = NULL;
    }
    if (cmap)
      bitmapColourMap = cmap;
  }
#endif
#if 1
  else if (flags & wxBITMAP_TYPE_GIF)
  {
    wxColourMap *cmap = NULL;
    Bool success = FALSE;
    if (flags & wxBITMAP_DISCARD_COLOURMAP)
      success = wxLoadGifIntoBitmap(bitmap_file, this);
    else
      success = wxLoadGifIntoBitmap(bitmap_file, this, &cmap);
    if (!success && cmap)
    {
      delete cmap;
		cmap = NULL;
    }
    if (cmap)
      bitmapColourMap = cmap;
  }
#endif
  
  if (oldSel && ok)
	oldSel->SelectObject(this);

  return ok;
}

wxBitmap::~wxBitmap(void)
{
  if (selectedInto)
  {
	  ((wxMemoryDC *)selectedInto)->SelectObject(NULL);
#if 0
    char buf[200];
    sprintf(buf, "Bitmap %X was deleted without selecting out of wxMemoryDC %X.", this, selectedInto);
    wxFatalError(buf);
#endif
  }
  if (ms_bitmap)
  {
    DeleteObject(ms_bitmap);
  }
  ms_bitmap = NULL ;

  if (bitmapColourMap)
    delete bitmapColourMap;

#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxTheBitmapList->DeleteObject(this);
#endif
}

Bool wxBitmap::SaveFile(char *filename, int typ, wxColourMap *cmap)
{
  if (!ok) return FALSE;

  switch (typ)
  {
#if USE_IMAGE_LOADING_IN_MSW
    case wxBITMAP_TYPE_BMP:
    {
      wxColourMap *actualCmap = cmap;
      if (!actualCmap)
        actualCmap = bitmapColourMap;
      return wxSaveBitmap(filename, this, actualCmap);
      break;
    }
#endif
    case wxBITMAP_TYPE_XBM:
    {
	  char *c, *p;
      HGDIOBJ orig = NULL;
	  int i, j;

	  c = new char[width * height];

	  HDC dc = selectedInto 
		       ? selectedInto->cdc
		       : CreateCompatibleDC(NULL);
			
	  if (dc && !selectedInto) {
	    orig = SelectObject(dc, ms_bitmap);
		if (!orig) {
			DeleteDC(dc);
			dc = NULL;
		}
	  }

	  if (!dc) return FALSE;

	  for (i = 0, p = c; i < height; i++)
		for (j = 0; j < width; j++, p++)
		  *p = (::GetPixel(dc, j, i) ? 1 : 0);
	
	  if (!selectedInto) {
		  SelectObject(dc, orig);
		  DeleteDC(dc);
	  }

      return wxSaveXBM(filename, c, width, height);
      break;
    }
#if USE_XPM_IN_MSW
    case wxBITMAP_TYPE_XPM:
    {
      HGDIOBJ orig = NULL;

      Visual *visual = NULL;
      XImage  ximage;

	  HDC dc = selectedInto 
		       ? selectedInto->cdc
		       : CreateCompatibleDC(NULL);

	  if (dc && !selectedInto) {
	    orig = SelectObject(dc, ms_bitmap);
		if (!orig) {
			DeleteDC(dc);
			dc = NULL;
		}
	  }

	  if (!dc) return FALSE;

	  /* for following SetPixel */
      /* fill the XImage struct 'by hand' */
	  ximage.width = width; ximage.height = height;
	  ximage.depth = depth; ximage.bitmap = ms_bitmap;
	  int errorStatus = XpmWriteFileFromImage(&dc, filename,
						  &ximage, (XImage *) NULL, (XpmAttributes *) NULL);

      if (!selectedInto) {
		 SelectObject(dc, orig);
		 DeleteDC(dc);
	  }

	  if (errorStatus == XpmSuccess)
	    return TRUE;		/* no error */
	  else
	    return FALSE;

      break;
    }
#endif
    default:
      break;
  }
  return FALSE;
}

