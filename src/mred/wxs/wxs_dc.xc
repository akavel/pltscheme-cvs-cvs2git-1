
@INCLUDE prefix.xci

#include "wx_dccan.h"
#include "wx_dcmem.h"
#include "wx_dcps.h"
#ifndef wx_mac
#include "wx_dcpan.h"
#endif
#ifdef wx_msw
#include "wx_mf.h"
#endif
#include "wx_types.h"
#ifdef wx_mac
#include "wx_dcpr.h"
#endif
#include "wx_rgn.h"

@INCLUDE wxs.xci

@HEADER

@BEGINSYMBOLS solidity > ONE
@SYM "transparent" : wxTRANSPARENT
@SYM "solid" : wxSOLID
@ENDSYMBOLS

@BEGINSYMBOLS bitmapDrawStyle > ONE
@SYM "solid" : wxSOLID
@SYM "opaque" : wxSTIPPLE
@SYM "xor" : wxXOR
@ENDSYMBOLS

@INCLUDE wxs_drws.xci

@MACRO CastToSO = (Scheme_Object*){x}
@MACRO spAnything = _

static wxColour* dcGetTextBackground(wxDC *dc)
{
  wxColour *c = new wxColour();
  *c = dc->GetTextBackground();
  return c;
}

static wxColour* dcGetTextForeground(wxDC *dc)
{
  wxColour *c = new wxColour();
  *c = dc->GetTextForeground();
  return c;
}

static bool DrawBitmap(wxDC *dc, wxBitmap *bm, float x, float y, int mode, wxColour *c)
{
  if (bm->Ok()) {
    return dc->Blit(x, y, bm->GetWidth(), bm->GetHeight(), bm, 0, 0, mode, c);
  } else
    return FALSE;
}

static bool DrawBitmapRegion(wxDC *dc, wxBitmap *bm, float x, float y, float dx, float dy, float dw, float dh, int mode, wxColour *c)
{
  if (bm->Ok()) {
    return dc->Blit(x, y, dw, dh, bm, dx, dy, mode, c);
  } else
    return FALSE;
}

static void* MyTextExtent(wxDC *dc, char *s, wxFont *f, bool big)
{
  float w, h, d, asc;
  Scheme_Object *a[4];

  dc->GetTextExtent(s, &w, &h, &d, &asc, f, big);

  a[0] = scheme_make_double(w);
  a[1] = scheme_make_double(h);
  a[2] = scheme_make_double(d);
  a[3] = scheme_make_double(asc);

  return scheme_values(4, a);
}

static void* MyGetSize(wxDC *dc)
{
  float w, h;
  Scheme_Object *a[2];

  dc->GetSize(&w, &h);

  a[0] = scheme_make_double(w);
  a[1] = scheme_make_double(h);

  return scheme_values(2, a);
}

@CLASSBASE wxDC "dc":"object"
@INTERFACE "dc"

@CLASSID wxTYPE_DC

@SETMARK Q = U
@INCLUDE wxs_draw.xci

// Also in wxWindow:
@ m "get-text-extent" : void[]/CastToSO//spAnything MyTextExtent(string,wxFont^=NULL,bool=FALSE); : : /CheckOk
@ Q "get-char-height" : float GetCharHeight();
@ Q "get-char-width" : float GetCharWidth();

@MACRO rZERO = return 0;
@MACRO rFALSE = return FALSE;

#ifndef wx_mac
#define HIDETHISSTATEMENT(x) x
#else
#define HIDETHISSTATEMENT(x) 
#endif

@MACRO IFNOTMAC = HIDETHISSTATEMENT(
@MACRO ENDIF = )

#ifndef wx_mac
#define CHECKTHISONE(x) x
#else
#define CHECKTHISONE(x) 1
#endif

@ m "draw-bitmap-section" : bool DrawBitmapRegion(wxBitmap!,float,float,float,float,nnfloat,nnfloat,SYM[bitmapDrawStyle]=wxSOLID,wxColour!=NULL); : : /CheckOkFalse|CheckFalse[0] : : rFALSE <> with size
@ m "draw-bitmap" : bool DrawBitmap(wxBitmap!,float,float,SYM[bitmapDrawStyle]=wxSOLID,wxColour!=NULL);

@ Q "try-color" : void TryColour(wxColour!,wxColour!);

@ Q "set-background-mode" : void SetBackgroundMode(SYM[solidity]); :  : /CheckOk
@ Q "set-scale" : void SetUserScale(nnfloat,nnfloat); : : /CheckOk
@ Q "set-origin" : void SetDeviceOrigin(float,float); : : /CheckOk

@ q "get-background" : wxColour! GetBackground();
@ q "get-background-mode" : SYM[solidity] GetBackgroundMode();
@ q "get-brush" : wxBrush! GetBrush();
@ q "get-font" : wxFont! GetFont();
@ q "get-pen" : wxPen! GetPen();
@ m "get-text-background" : wxColour! dcGetTextBackground();
@ m "get-text-foreground" : wxColour! dcGetTextForeground();

@ m "get-size" : void[]/CastToSO//spAnything MyGetSize();

@ q "ok?" : bool Ok();

@ Q "start-doc" : bool StartDoc(string); : : : rFALSE
@ Q "start-page" : void StartPage();
@ Q "end-doc" : void EndDoc();
@ Q "end-page" : void EndPage();

@END

@CLASSBASE wxMemoryDC "bitmap-dc":"dc"

@CLASSID wxTYPE_DC_MEMORY

@CREATOR ()

@ "get-pixel" : bool GetPixel(float,float,wxColour^)
@ "set-pixel" : void SetPixel(float,float,wxColour^)

@ "set-bitmap" : void SelectObject(wxBitmap^);
@ "get-bitmap" : wxBitmap^ GetObject();

@END

@CLASSBASE wxPostScriptDC "post-script-dc":"dc"

@CLASSID wxTYPE_DC_POSTSCRIPT

@INCLUDE wxs_dorf.xci

@CREATOR (bool=TRUE)

@END

#ifdef wx_x

class basePrinterDC : public wxObject
{
public:
  basePrinterDC()
  {
    scheme_raise_exn(MZEXN_MISC_UNSUPPORTED,
		     "%s", 
		     METHODNAME("printer-dc%","initialization")": not supported for X Windows");
  }
};

#else

class basePrinterDC : public wxPrinterDC
{
public:
  basePrinterDC()
    : wxPrinterDC( )
  {
  }
};

#endif

@CLASSBASE basePrinterDC "printer-dc":"dc"

@CLASSID wxTYPE_DC_PRINTER

@CREATOR ();

@END




#if 0

#ifdef wx_msw

class baseMetaFileDC : public wxMetaFileDC {
public:
  baseMetaFileDC(char *s = NULL);

  baseMetaFile* baseClose() { return (baseMetaFile *)Close(); }
};

baseMetaFileDC::baseMetaFileDC(char *s)
    : wxMetaFileDC(s)
{
}

#else

class baseMetaFileDC : public wxObject 
{
public:
  baseMetaFileDC(char * = NULL) {
    scheme_raise_exn(MZEXN_MISC_UNSUPPORTED,
		     "%s", 
		     METHODNAME("meta-file-dc%","initialization")": only supported for Windows");
  }

  baseMetaFile* baseClose() { return NULL; }
};

#endif

@CLASSBASE baseMetaFileDC "meta-file-dc":"dc"

@CLASSID wxTYPE_DC_METAFILE

@CREATOR (string=NULL);

@ "close" : baseMetaFile! baseClose()

@END

#endif
