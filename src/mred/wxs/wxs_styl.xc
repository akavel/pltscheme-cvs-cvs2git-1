
@INCLUDE prefix.xci

#include "wx_style.h"
#include "wx_mtype.h"

@INCLUDE wxs.xci

@HEADER

@CLASSBASE wxMultColour "mult-color" : "object"
@INTERFACE "mult-color"

@IVAR "r" : float r
@IVAR "g" : float g
@IVAR "b" : float b

@ "get" : void Get(float*,float*,float*);
@ "set" : void Set(float,float,float);

@END

@CLASSBASE wxAddColour "add-color" : "object"
@INTERFACE "add-color"

@IVAR "r" : rint[-1000|1000] r
@IVAR "g" : rint[-1000|1000] g
@IVAR "b" : rint[-1000|1000] b

// short* should really be rshort[-1000|1000]*:
@ "get" : void Get(short*,short*,short*);
@ "set" : void Set(rint[-1000|1000],rint[-1000|1000],rint[-1000|1000]);

@END

@BEGINSYMBOLS family > ONE
@SYM "base" : wxBASE
@SYM "default" : wxDEFAULT
@SYM "decorative" : wxDECORATIVE
@SYM "roman" : wxROMAN
@SYM "script" : wxSCRIPT
@SYM "swiss" : wxSWISS
@SYM "modern" : wxMODERN
@SYM "teletype" : wxTELETYPE
@SYM "system" : wxSYSTEM
@ENDSYMBOLS

@BEGINSYMBOLS weight > ONE
@SYM "base" : wxBASE
@SYM "normal" : wxNORMAL
@SYM "light" : wxLIGHT
@SYM "bold" : wxBOLD
@ENDSYMBOLS

@BEGINSYMBOLS style > ONE
@SYM "base" : wxBASE
@SYM "normal" : wxNORMAL
@SYM "italic" : wxITALIC
@SYM "slant" : wxSLANT
@ENDSYMBOLS

@BEGINSYMBOLS align > ONE
@SYM "base" : wxBASE
@SYM "top" :  wxALIGN_TOP
@SYM "bottom" : wxALIGN_BOTTOM
@SYM "center" : wxALIGN_CENTER
@ENDSYMBOLS

@BEGINSYMBOLS changeNoArg > ONE
@SYM "change-nothing" : wxCHANGE_NOTHING
@SYM "change-normal" : wxCHANGE_NORMAL
@SYM "change-bold" : wxCHANGE_BOLD
@SYM "change-italic" : wxCHANGE_ITALIC
@SYM "change-toggle-underline" : wxCHANGE_TOGGLE_UNDERLINE
@SYM "change-normal-color" : wxCHANGE_NORMAL_COLOUR
@ENDSYMBOLS
@BEGINSYMBOLS changeFam > ONE
@SYM "change-family" : wxCHANGE_FAMILY
@ENDSYMBOLS
@BEGINSYMBOLS changeStyle > ONE
@SYM "change-style" : wxCHANGE_STYLE
@SYM "change-toggle-style" : wxCHANGE_TOGGLE_STYLE
@ENDSYMBOLS
@BEGINSYMBOLS changeWeight > ONE
@SYM "change-weight" : wxCHANGE_WEIGHT
@SYM "change-toggle-weight" : wxCHANGE_TOGGLE_WEIGHT
@ENDSYMBOLS
@BEGINSYMBOLS changeUnderline > ONE
@SYM "change-underline" : wxCHANGE_UNDERLINE
@ENDSYMBOLS
@BEGINSYMBOLS changeSize > ONE
@SYM "change-size" : wxCHANGE_SIZE
@SYM "change-bigger" : wxCHANGE_BIGGER
@SYM "change-smaller" : wxCHANGE_SMALLER
@ENDSYMBOLS
@BEGINSYMBOLS changeAlign > ONE
@SYM "change-alignment" : wxCHANGE_ALIGNMENT
@ENDSYMBOLS

@CLASSBASE wxStyleDelta "style-delta" : "object"

@IVAR "family" : SYM[family] family
@IVAR "face" : nstring face
@IVAR "size-mult" : float sizeMult
@IVAR "size-add" : rint[0|255] sizeAdd
@IVAR "weight-on" : SYM[weight] weightOn
@IVAR "weight-off" : SYM[weight] weightOff
@IVAR "style-on" : SYM[style] styleOn
@IVAR "style-off" : SYM[style] styleOff
@IVAR "underlined-on" : bool underlinedOn
@IVAR "underlined-off" : bool underlinedOff
@IVAR "transparent-text-backing-on" : bool transparentTextBackingOn
@IVAR "transparent-text-backing-off" : bool transparentTextBackingOff
@IVAR r "foreground-mult" : wxMultColour! foregroundMult
@IVAR r "background-mult" : wxMultColour! backgroundMult
@IVAR r "foreground-add" : wxAddColour! foregroundAdd
@IVAR r "background-add" : wxAddColour! backgroundAdd
@IVAR "alignment-on" : SYM[align] alignmentOn
@IVAR "alignment-off" : SYM[align] alignmentOff
  
@CREATOR (SYM[changeNoArg]=wxCHANGE_NOTHING,-int=0); <> no change argument
@CREATORX (SYM[changeFam],SYM[family]); <> family
@CREATORX (SYM[changeStyle],SYM[style]); <> style
@CREATORX (SYM[changeWeight],SYM[weight]); <> weight
@CREATORX (SYM[changeUnderline],bool); <> underline
@CREATORX (SYM[changeSize],rint[0|255]); <> size
@CREATORX (SYM[changeAlign],SYM[align]); <> size

@CLASSID wxTYPE_STYLE_DELTA

@ "set-delta" : wxStyleDelta! SetDelta(SYM[changeNoArg]=wxCHANGE_NOTHING,-int=0); <> no change argument
@ "set-delta" : wxStyleDelta! SetDelta(SYM[changeFam],SYM[family]); <> family
@ "set-delta" : wxStyleDelta! SetDelta(SYM[changeStyle],SYM[style]); <> style
@ "set-delta" : wxStyleDelta! SetDelta(SYM[changeWeight],SYM[weight]); <> weight
@ "set-delta" : wxStyleDelta! SetDelta(SYM[changeUnderline],bool); <> underline
@ "set-delta" : wxStyleDelta! SetDelta(SYM[changeSize],rint[0|255]); <> size
@ "set-delta" : wxStyleDelta! SetDelta(SYM[changeAlign],SYM[align]); <> size

@ "set-delta-face" : wxStyleDelta! SetDeltaFace(string);
@ "set-delta-background" : wxStyleDelta! SetDeltaBackground(string); <> color name
@ "set-delta-background" : wxStyleDelta! SetDeltaBackground(wxColour%); <> colour%
@ "set-delta-foreground" : wxStyleDelta! SetDeltaForeground(string); <> color name
@ "set-delta-foreground" : wxStyleDelta! SetDeltaForeground(wxColour%); <> colour%

@ "equal?" : bool Equal(wxStyleDelta%);
@ "collapse" : bool Collapse(wxStyleDelta%);
@ "copy" : void Copy(wxStyleDelta!);

@END


@CLASSBASE wxStyle "style" : "object"

@CLASSID wxTYPE_STYLE

@ "get-name" : string GetName();
@ "get-family" : SYM[family] GetFamily();
@ "get-face" : nstring GetFace();
@ "get-size" : int GetSize();
@ "get-weight" : SYM[weight] GetWeight();
@ "get-style" : SYM[style] GetStyle();
@ "get-underlined" : bool GetUnderlined();
@ "get-font" : wxFont! GetFont();
@ "get-foreground" : wxColour% GetForeground();
@ "get-background" : wxColour% GetBackground();
@ "get-alignment" : SYM[align] GetAlignment();
@ "get-transparent-text-backing" : bool GetTransparentTextBacking();

@ "get-text-height" : float GetTextHeight(wxDC!);
@ "get-text-descent" : float GetTextDescent(wxDC!);
@ "get-text-space" : float GetTextSpace(wxDC!);
@ "get-text-width" : float GetTextWidth(wxDC!);

@ "get-base-style" : wxStyle^ GetBaseStyle();
@ "set-base-style" : void SetBaseStyle(wxStyle!);

@ "get-delta" : void GetDelta(wxStyleDelta%);
@ "set-delta" : void SetDelta(wxStyleDelta%);

@ "is-join?" : bool IsJoin();

@ "get-shift-style" :  wxStyle^ GetShiftStyle();
@ "set-shift-style" : void SetShiftStyle(wxStyle!);

@ "switch-to" : void SwitchTo(wxDC!, wxStyle^); : : /CHECKOK[0.METHODNAME("style%","switch-to")]

@END


@CLASSBASE wxStyleList "style-list" : "object"

@CREATOR ();

@CLASSID wxTYPE_STYLE_LIST

// @ "clear" : void Clear();
// @ "copy" : void Copy(wxStyleList!);

@ "basic-style" : wxStyle! BasicStyle();

@ "number" : int Number();

@ "find-or-create-style" : wxStyle! FindOrCreateStyle(wxStyle^,wxStyleDelta!);
@ "find-or-create-join-style" : wxStyle! FindOrCreateJoinStyle(wxStyle^,wxStyle!);
@ "find-named-style" : wxStyle^ FindNamedStyle(string);
@ "new-named-style" : wxStyle! NewNamedStyle(string,wxStyle!);
@ "replace-named-style" : wxStyle! ReplaceNamedStyle(string,wxStyle!);

@ "convert" : wxStyle! Convert(wxStyle!);

@MACRO bNegAsFalse = (({x} < 0) ? scheme_false : scheme_make_integer({x}))

@ "index-to-style" : wxStyle^ IndexToStyle(nnint);
@ "style-to-index" : int/bNegAsFalse StyleToIndex(wxStyle!);

@CONSTANT "the-style-list" : wxStyleList! wxTheStyleList

static void NotifyCallbackToScheme(wxStyle *, Scheme_Object *f);

@MACRO ubCallback = (wxStyleNotifyFunc)NotifyCallbackToScheme
@MACRO ubData = (scheme_check_proc_arity(METHODNAME("style-list%","notify-on-change"), 1, 0, 0, p), p[0])
@MACRO spCallback = (wxStyle-object-or-#f -> void)

@MACRO bAnythingFromLong = ((Scheme_Object *){x})
@MACRO ubAnythingToLong = ((long){x})
@MACRO cAnything = 1

@ "notify-on-change" : long/bAnythingFromLong NotifyOnChange(wxStyleNotifyFunc//ubCallback//spCallback,-unknown#void*//ubData)
@ "forget-notification" : void ForgetNotification(long//ubAnythingToLong/cAnything)

@END

static void NotifyCallbackToScheme(wxStyle *s, Scheme_Object *f)
{
  Scheme_Object *p[1];

  p[0] = s ? objscheme_bundle_wxStyle(s) : scheme_false;

  scheme_apply_multi(f, 1, p);
}

