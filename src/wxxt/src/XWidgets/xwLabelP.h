/* Generated by wbuild
 * (generator version 3.2)
 */
#ifndef ___XWLABELP_H
#define ___XWLABELP_H
#include <./xwBoardP.h>
#include <./xwLabel.h>
_XFUNCPROTOBEGIN
typedef void (*set_label_Proc)(
#if NeedFunctionPrototypes
Widget,String 
#endif
);
#define XtInherit_set_label ((set_label_Proc) _XtInherit)

typedef struct {
/* methods */
set_label_Proc set_label;
/* class variables */
} XfwfLabelClassPart;

typedef struct _XfwfLabelClassRec {
CoreClassPart core_class;
CompositeClassPart composite_class;
XfwfCommonClassPart xfwfCommon_class;
XfwfFrameClassPart xfwfFrame_class;
XfwfBoardClassPart xfwfBoard_class;
XfwfLabelClassPart xfwfLabel_class;
} XfwfLabelClassRec;

typedef struct {
/* resources */
String  label;
String  tablist;
XFontStruct * font;
void * xfont;
Pixmap  pixmap;
Pixel  foreground;
Alignment  alignment;
Dimension  topMargin;
Dimension  bottomMargin;
Dimension  leftMargin;
Dimension  rightMargin;
Boolean  shrinkToFit;
int  rvStart;
int  rvLength;
Boolean  drawgray;
/* private state */
int  nlines;
int * tabs;
GC  gc;
GC  graygc;
Dimension  label_width;
Dimension  label_height;
unsigned  int  label_depth;
} XfwfLabelPart;

typedef struct _XfwfLabelRec {
CorePart core;
CompositePart composite;
XfwfCommonPart xfwfCommon;
XfwfFramePart xfwfFrame;
XfwfBoardPart xfwfBoard;
XfwfLabelPart xfwfLabel;
} XfwfLabelRec;

externalref XfwfLabelClassRec xfwfLabelClassRec;

_XFUNCPROTOEND
#endif /* ___XWLABELP_H */
