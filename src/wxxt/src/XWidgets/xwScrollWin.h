/* Generated by wbuild
 * (generator version 3.2)
 */
#ifndef ___XWSCROLLWIN_H
#define ___XWSCROLLWIN_H
#include <./xwBoard.h>
_XFUNCPROTOBEGIN
void xws_set_scroll_direct(
#if NeedFunctionPrototypes
Widget,long ,long ,long ,long ,long ,long 
#endif
);
void xws_get_scroll_pos(
#if NeedFunctionPrototypes
Widget,double *,double *
#endif
);
void xws_get_scroll_area(
#if NeedFunctionPrototypes
Widget,Dimension *,Dimension *
#endif
);
void ScrollWinViewableHeight(
#if NeedFunctionPrototypes
Widget,int *
#endif
);
#ifndef XtNautoAdjustScrollbars
#define XtNautoAdjustScrollbars "autoAdjustScrollbars"
#endif
#ifndef XtCAutoAdjustScrollbars
#define XtCAutoAdjustScrollbars "AutoAdjustScrollbars"
#endif
#ifndef XtRBoolean
#define XtRBoolean "Boolean"
#endif

#ifndef XtNtraverseToChild
#define XtNtraverseToChild "traverseToChild"
#endif
#ifndef XtCTraverseToChild
#define XtCTraverseToChild "TraverseToChild"
#endif
#ifndef XtRBoolean
#define XtRBoolean "Boolean"
#endif

#ifndef XtNspacing
#define XtNspacing "spacing"
#endif
#ifndef XtCSpacing
#define XtCSpacing "Spacing"
#endif
#ifndef XtRDimension
#define XtRDimension "Dimension"
#endif

#ifndef XtNscrollbarWidth
#define XtNscrollbarWidth "scrollbarWidth"
#endif
#ifndef XtCScrollbarWidth
#define XtCScrollbarWidth "ScrollbarWidth"
#endif
#ifndef XtRDimension
#define XtRDimension "Dimension"
#endif

#ifndef XtNshadowWidth
#define XtNshadowWidth "shadowWidth"
#endif
#ifndef XtCShadowWidth
#define XtCShadowWidth "ShadowWidth"
#endif
#ifndef XtRDimension
#define XtRDimension "Dimension"
#endif

#ifndef XtNhideHScrollbar
#define XtNhideHScrollbar "hideHScrollbar"
#endif
#ifndef XtCHideHScrollbar
#define XtCHideHScrollbar "HideHScrollbar"
#endif
#ifndef XtRBoolean
#define XtRBoolean "Boolean"
#endif

#ifndef XtNhideVScrollbar
#define XtNhideVScrollbar "hideVScrollbar"
#endif
#ifndef XtCHideVScrollbar
#define XtCHideVScrollbar "HideVScrollbar"
#endif
#ifndef XtRBoolean
#define XtRBoolean "Boolean"
#endif

#ifndef XtNvScrollAmount
#define XtNvScrollAmount "vScrollAmount"
#endif
#ifndef XtCVScrollAmount
#define XtCVScrollAmount "VScrollAmount"
#endif
#ifndef XtRInt
#define XtRInt "Int"
#endif

#ifndef XtNhScrollAmount
#define XtNhScrollAmount "hScrollAmount"
#endif
#ifndef XtCHScrollAmount
#define XtCHScrollAmount "HScrollAmount"
#endif
#ifndef XtRInt
#define XtRInt "Int"
#endif

#ifndef XtNinitialX
#define XtNinitialX "initialX"
#endif
#ifndef XtCInitialX
#define XtCInitialX "InitialX"
#endif
#ifndef XtRPosition
#define XtRPosition "Position"
#endif

#ifndef XtNinitialY
#define XtNinitialY "initialY"
#endif
#ifndef XtCInitialY
#define XtCInitialY "InitialY"
#endif
#ifndef XtRPosition
#define XtRPosition "Position"
#endif

#ifndef XtNdrawgrayScrollWin
#define XtNdrawgrayScrollWin "drawgrayScrollWin"
#endif
#ifndef XtCDrawgrayScrollWin
#define XtCDrawgrayScrollWin "DrawgrayScrollWin"
#endif
#ifndef XtRBoolean
#define XtRBoolean "Boolean"
#endif

#ifndef XtNdoScroll
#define XtNdoScroll "doScroll"
#endif
#ifndef XtCDoScroll
#define XtCDoScroll "DoScroll"
#endif
#ifndef XtRBoolean
#define XtRBoolean "Boolean"
#endif

#ifndef XtNscrollCallback
#define XtNscrollCallback "scrollCallback"
#endif
#ifndef XtCScrollCallback
#define XtCScrollCallback "ScrollCallback"
#endif
#ifndef XtRCallback
#define XtRCallback "Callback"
#endif

#ifndef XtNscrollResponse
#define XtNscrollResponse "scrollResponse"
#endif
#ifndef XtCScrollResponse
#define XtCScrollResponse "ScrollResponse"
#endif
#ifndef XtRXTCallbackProc
#define XtRXTCallbackProc "XTCallbackProc"
#endif

typedef struct _XfwfScrolledWindowClassRec *XfwfScrolledWindowWidgetClass;
typedef struct _XfwfScrolledWindowRec *XfwfScrolledWindowWidget;
externalref WidgetClass xfwfScrolledWindowWidgetClass;
_XFUNCPROTOEND
#endif /* ___XWSCROLLWIN_H */
