
@INCLUDE prefix.xci

#include "wx_gdi.h"

@INCLUDE wxs.xci

@HEADER

@INCLUDE wxs_bmt.xci

@CLASSBASE wxBitmap "wx:bitmap" : "wx:object"

@SET TYPE = char
@SET NOTEST = 1
@INCLUDE list.xci

@MACRO ZEROERR[p.woh] = if ((x<p> < 1) || (x<p> > 100000)) scheme_signal_error("wx:bitmap%%::initialization: bad " <woh> ": %d", x<p>);

@MACRO NONZERODEPTH = if (x3 != 1) scheme_signal_error("wx:bitmap%%::initialization: depth %d is illegal (only depth 1 is supported)", x3);
@MACRO LISTENOUGH = if (scheme_proper_list_length(p[0]) < (((x1 * x2) >> 3) * x3)) scheme_signal_error("wx:bitmap%%::initialization: byte list too short");
	
@CREATOR (char[]/bList/ubList/cList,int,int,int=1); : : /ZEROERR[1."width"]|ZEROERR[2."height"]|NONZERODEPTH|LISTENOUGH|glueUncountedListSet[char.0.0."wx:bitmap%::initialization"]// <> character list
@CREATOR (int,int,int=-1); : : /ZEROERR[0."width"]|ZEROERR[1."height"] <> width/height
@CREATOR (pathname,SYM[bitmapType]); <> pathname

@ "get-depth" : int GetDepth();
@ "get-height" : int GetHeight();
@ "get-width" : int GetWidth();
@ "ok?" : bool Ok();

@ "load-file" : void LoadFile(pathname,SYM[bitmapType]);
@ "save-file" : void SaveFile(pathname,int,wxColourMap^=NULL);

@ "set-colour-map" : void SetColourMap(wxColourMap^);

@END

@CLASSBASE wxIcon "wx:icon" : "wx:bitmap"

@CREATOR (string, SYM[bitmapType]);

@END

