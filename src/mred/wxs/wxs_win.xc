
@INCLUDE prefix.xci 

#include "wx_win.h"
#include "wx_gdi.h"

@INCLUDE wxs.xci

@HEADER

#ifdef wx_mac
#define Move(x, y) SetSize(-1, -1, x, y)
#endif

#if defined(wx_xt)
#define wxPOS_USE_MINUS_ONE 0x4
#endif

static int wxSchemeWindowGetWidth(wxWindow *w)
{
  int x, y;

  w->GetSize(&x, &y);
  
  return x;
}

static int wxSchemeWindowGetHeight(wxWindow *w)
{
  int x, y;

  w->GetSize(&x, &y);
  
  return y;
}

static int wxSchemeWindowGetX(wxWindow *w)
{
  int x, y;

  w->GetPosition(&x, &y);
  
  return x;
}

static int wxSchemeWindowGetY(wxWindow *w)
{
  int x, y;

  w->GetPosition(&x, &y);
  
  return y;
}

@BEGINSYMBOLS sizeMode > ONE
@SYM "auto" : wxSIZE_AUTO
@SYM "use-exsiting" : wxSIZE_USE_EXISTING
@SYM "use-minus-one" : wxPOS_USE_MINUS_ONE
@ENDSYMBOLS

@BEGINSYMBOLS direction > ONE
@SYM "both" : wxBOTH
@SYM "vertical" : wxVERTICAL
@SYM "horizontal" : wxHORIZONTAL
@ENDSYMBOLS

@CLASSBASE wxWindow "wx:window":"wx:object"

@MACRO CHECKCURSOROK[p] = if (x<p> && !x<p>->Ok()) x<p> = wxSTANDARD_CURSOR;

@ "centre" : void Centre(SYM[direction]=wxBOTH);

@ "set-focus" : void SetFocus();
@ "set-size" : void SetSize(int,int,int,int,SYM[sizeMode]=wxSIZE_AUTO); : : /NOZERO[2]|NOZERO[3]
@ "move" : void Move(int,int); 
@ "set-cursor" : wxCursor^ SetCursor(wxCursor^); : : /CHECKCURSOROK[0]
@ "show" : void Show(bool);
@ "is-shown?" : bool IsShown();
@ "fit" : void Fit();
@ "get-size" : void GetSize(int*,int*);
@ "get-client-size" : void GetClientSize(int*,int*);
@ "get-position" : void GetPosition(int*,int*);
@ "enable" : void Enable(bool);

@ "get-name" : nstring GetName();

@ "get-char-height" : float GetCharHeight();
@ "get-char-width" : float GetCharWidth();
@ "client-to-screen" : void ClientToScreen(int*, int*);
@ "screen-to-client" : void ScreenToClient(int*,int*);
@ "refresh" : void Refresh();
@ "get-parent" : wxWindow^ GetParent();
@ "get-text-extent" : void GetTextExtent(string,float*,float*,float?=NULL,float?=NULL,wxFont^=NULL,bool=FALSE);
@ "get-grand-parent" : wxWindow^ GetGrandParent();
@ "get-label" : nstring GetLabel();
@ "center" : void Center(SYM[direction]=wxBOTH);

@ m "get-height" : int wxSchemeWindowGetHeight();
@ m "get-width" : int wxSchemeWindowGetWidth();
@ m "get-x" : int wxSchemeWindowGetX();
@ m "get-y" : int wxSchemeWindowGetY();

@SETMARK w = V
@INCLUDE wxs_win.xci

@END
