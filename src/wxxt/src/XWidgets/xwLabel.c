/* Generated by wbuild
 * (generator version 3.2)
 */
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <stdio.h>
#include <xwTabString.h>
#include <./xwLabelP.h>
static void _resolve_inheritance(
#if NeedFunctionPrototypes
WidgetClass
#endif
);
static void set_label(
#if NeedFunctionPrototypes
Widget,String 
#endif
);
static Boolean  set_values(
#if NeedFunctionPrototypes
Widget ,Widget ,Widget,ArgList ,Cardinal *
#endif
);
static void initialize(
#if NeedFunctionPrototypes
Widget ,Widget,ArgList ,Cardinal *
#endif
);
static void destroy(
#if NeedFunctionPrototypes
Widget
#endif
);
static void realize(
#if NeedFunctionPrototypes
Widget,XtValueMask *,XSetWindowAttributes *
#endif
);
static void _expose(
#if NeedFunctionPrototypes
Widget,XEvent *,Region 
#endif
);
static void make_gc(
#if NeedFunctionPrototypes
Widget
#endif
);
static void make_graygc(
#if NeedFunctionPrototypes
Widget
#endif
);
static void count_lines(
#if NeedFunctionPrototypes
Widget
#endif
);
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void make_gc(Widget self)
#else
static void make_gc(self)Widget self;
#endif
{
    XtGCMask mask, fnt = 0;
    XGCValues values;

    if (((XfwfLabelWidget)self)->xfwfLabel.gc != NULL) XtReleaseGC(self, ((XfwfLabelWidget)self)->xfwfLabel.gc);
    values.background = ((XfwfLabelWidget)self)->xfwfLabel.text_bg;
    if (!((XfwfLabelWidget)self)->xfwfLabel.xfont) {
      values.foreground = ((XfwfLabelWidget)self)->xfwfLabel.foreground;
      values.font = ((XfwfLabelWidget)self)->xfwfLabel.font->fid;
      fnt = GCFont;
    } else {
      if (((XfwfLabelWidget)self)->xfwfLabel.pixmap)
	values.foreground = ((XfwfLabelWidget)self)->xfwfLabel.foreground;
      else
	values.foreground = values.background;
    }
    mask = fnt | GCBackground | GCForeground;
    ((XfwfLabelWidget)self)->xfwfLabel.gc = XtGetGC(self, mask, &values);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void make_graygc(Widget self)
#else
static void make_graygc(self)Widget self;
#endif
{
    XtGCMask mask;
    XGCValues values;

    if (((XfwfLabelWidget)self)->xfwfLabel.graygc != NULL) XtReleaseGC(self, ((XfwfLabelWidget)self)->xfwfLabel.graygc);

    if ((((XfwfLabelWidget)self)->xfwfLabel.pixmap != 0) || !wx_enough_colors(XtScreen(self))) {
      /* A GC to draw over bitmaps/text: */
      values.foreground = ((XfwfLabelWidget)self)->xfwfLabel.text_bg;
      values.stipple = GetGray(self);
      values.fill_style = FillStippled;
      mask = GCForeground | GCStipple | GCFillStyle;
    } else {
      /* A GC for drawing gray text: */
      static Pixel color;
      values.background = ((XfwfLabelWidget)self)->xfwfLabel.text_bg;
      ((XfwfLabelWidgetClass)self->core.widget_class)->xfwfCommon_class.darker_color(self, ((XfwfLabelWidget)self)->xfwfLabel.text_bg, &color);
      values.foreground = color;
      mask = GCBackground | GCForeground;
      if (((XfwfLabelWidget)self)->xfwfLabel.font) {
	values.font = ((XfwfLabelWidget)self)->xfwfLabel.font->fid;
	mask |= GCFont;
      }
    }
 
    ((XfwfLabelWidget)self)->xfwfLabel.graygc = XtGetGC(self, mask, &values);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void count_lines(Widget self)
#else
static void count_lines(self)Widget self;
#endif
{
    String p, s;
    int w;

    ((XfwfLabelWidget)self)->xfwfLabel.nlines = 0;
    ((XfwfLabelWidget)self)->xfwfLabel.label_width = 0; ((XfwfLabelWidget)self)->xfwfLabel.label_height = 0; ((XfwfLabelWidget)self)->xfwfLabel.label_depth = 0;
    if (((XfwfLabelWidget)self)->xfwfLabel.label) {
	for (p = ((XfwfLabelWidget)self)->xfwfLabel.label, ((XfwfLabelWidget)self)->xfwfLabel.nlines = 1, s = ((XfwfLabelWidget)self)->xfwfLabel.label; *s; s++) {
	    if (*s == '\n') {
		((XfwfLabelWidget)self)->xfwfLabel.nlines++;
		w = XfwfTextWidth(XtDisplay(self), ((XfwfLabelWidget)self)->xfwfLabel.font, (wxExtFont)((XfwfLabelWidget)self)->xfwfLabel.xfont, p, s - p, ((XfwfLabelWidget)self)->xfwfLabel.tabs);
		p = s + 1;
		if (w > ((XfwfLabelWidget)self)->xfwfLabel.label_width) ((XfwfLabelWidget)self)->xfwfLabel.label_width = w;
	    }
	}
	w = XfwfTextWidth(XtDisplay(self), ((XfwfLabelWidget)self)->xfwfLabel.font, (wxExtFont)((XfwfLabelWidget)self)->xfwfLabel.xfont, p, s - p, ((XfwfLabelWidget)self)->xfwfLabel.tabs);
	if (w > ((XfwfLabelWidget)self)->xfwfLabel.label_width) ((XfwfLabelWidget)self)->xfwfLabel.label_width = w;
	((XfwfLabelWidget)self)->xfwfLabel.label_height = ((XfwfLabelWidget)self)->xfwfLabel.nlines * (wx_ASCENT(((XfwfLabelWidget)self)->xfwfLabel.font, ((wxExtFont)((XfwfLabelWidget)self)->xfwfLabel.xfont)) + wx_DESCENT(((XfwfLabelWidget)self)->xfwfLabel.font, ((wxExtFont)((XfwfLabelWidget)self)->xfwfLabel.xfont)));
    } else if (((XfwfLabelWidget)self)->xfwfLabel.pixmap) {
	Window        root;
	int           x, y;
	unsigned int  width, height, bw, depth;
	XGetGeometry(XtDisplay(self), ((XfwfLabelWidget)self)->xfwfLabel.pixmap, &root,
	             &x, &y, &width, &height, &bw, &depth);
	((XfwfLabelWidget)self)->xfwfLabel.label_width  = (Dimension)width;
	((XfwfLabelWidget)self)->xfwfLabel.label_height = (Dimension)height;
	((XfwfLabelWidget)self)->xfwfLabel.label_depth  = depth;
    }
    /* add border */
    ((XfwfLabelWidget)self)->xfwfLabel.label_width += ((XfwfLabelWidget)self)->xfwfLabel.leftMargin + ((XfwfLabelWidget)self)->xfwfLabel.rightMargin;
    ((XfwfLabelWidget)self)->xfwfLabel.label_height += ((XfwfLabelWidget)self)->xfwfLabel.topMargin + ((XfwfLabelWidget)self)->xfwfLabel.bottomMargin;
}

static XtResource resources[] = {
{XtNlabel,XtCLabel,XtRString,sizeof(((XfwfLabelRec*)NULL)->xfwfLabel.label),XtOffsetOf(XfwfLabelRec,xfwfLabel.label),XtRImmediate,(XtPointer)NULL },
{XtNtablist,XtCTablist,XtRString,sizeof(((XfwfLabelRec*)NULL)->xfwfLabel.tablist),XtOffsetOf(XfwfLabelRec,xfwfLabel.tablist),XtRImmediate,(XtPointer)NULL },
{XtNfont,XtCFont,XtRFontStruct,sizeof(((XfwfLabelRec*)NULL)->xfwfLabel.font),XtOffsetOf(XfwfLabelRec,xfwfLabel.font),XtRString,(XtPointer)XtDefaultFont },
{XtNxfont,XtCXFont,XtRvoid,sizeof(((XfwfLabelRec*)NULL)->xfwfLabel.xfont),XtOffsetOf(XfwfLabelRec,xfwfLabel.xfont),XtRPointer,(XtPointer)NULL },
{XtNpixmap,XtCPixmap,XtRPixmap,sizeof(((XfwfLabelRec*)NULL)->xfwfLabel.pixmap),XtOffsetOf(XfwfLabelRec,xfwfLabel.pixmap),XtRImmediate,(XtPointer)0 },
{XtNforeground,XtCForeground,XtRPixel,sizeof(((XfwfLabelRec*)NULL)->xfwfLabel.foreground),XtOffsetOf(XfwfLabelRec,xfwfLabel.foreground),XtRString,(XtPointer)XtDefaultForeground },
{XtNtext_bg,XtCText_bg,XtRPixel,sizeof(((XfwfLabelRec*)NULL)->xfwfLabel.text_bg),XtOffsetOf(XfwfLabelRec,xfwfLabel.text_bg),XtRPointer,(XtPointer)NULL },
{XtNalignment,XtCAlignment,XtRAlignment,sizeof(((XfwfLabelRec*)NULL)->xfwfLabel.alignment),XtOffsetOf(XfwfLabelRec,xfwfLabel.alignment),XtRImmediate,(XtPointer)0 },
{XtNtopMargin,XtCTopMargin,XtRDimension,sizeof(((XfwfLabelRec*)NULL)->xfwfLabel.topMargin),XtOffsetOf(XfwfLabelRec,xfwfLabel.topMargin),XtRImmediate,(XtPointer)2 },
{XtNbottomMargin,XtCBottomMargin,XtRDimension,sizeof(((XfwfLabelRec*)NULL)->xfwfLabel.bottomMargin),XtOffsetOf(XfwfLabelRec,xfwfLabel.bottomMargin),XtRImmediate,(XtPointer)2 },
{XtNleftMargin,XtCLeftMargin,XtRDimension,sizeof(((XfwfLabelRec*)NULL)->xfwfLabel.leftMargin),XtOffsetOf(XfwfLabelRec,xfwfLabel.leftMargin),XtRImmediate,(XtPointer)2 },
{XtNrightMargin,XtCRightMargin,XtRDimension,sizeof(((XfwfLabelRec*)NULL)->xfwfLabel.rightMargin),XtOffsetOf(XfwfLabelRec,xfwfLabel.rightMargin),XtRImmediate,(XtPointer)2 },
{XtNshrinkToFit,XtCShrinkToFit,XtRBoolean,sizeof(((XfwfLabelRec*)NULL)->xfwfLabel.shrinkToFit),XtOffsetOf(XfwfLabelRec,xfwfLabel.shrinkToFit),XtRImmediate,(XtPointer)False },
{XtNrvStart,XtCRvStart,XtRInt,sizeof(((XfwfLabelRec*)NULL)->xfwfLabel.rvStart),XtOffsetOf(XfwfLabelRec,xfwfLabel.rvStart),XtRImmediate,(XtPointer)0 },
{XtNrvLength,XtCRvLength,XtRInt,sizeof(((XfwfLabelRec*)NULL)->xfwfLabel.rvLength),XtOffsetOf(XfwfLabelRec,xfwfLabel.rvLength),XtRImmediate,(XtPointer)0 },
{XtNtraversalOn,XtCTraversalOn,XtRBoolean,sizeof(((XfwfLabelRec*)NULL)->xfwfCommon.traversalOn),XtOffsetOf(XfwfLabelRec,xfwfCommon.traversalOn),XtRImmediate,(XtPointer)False },
{XtNdrawgray,XtCDrawgray,XtRBoolean,sizeof(((XfwfLabelRec*)NULL)->xfwfLabel.drawgray),XtOffsetOf(XfwfLabelRec,xfwfLabel.drawgray),XtRImmediate,(XtPointer)FALSE },
};

XfwfLabelClassRec xfwfLabelClassRec = {
{ /* core_class part */
/* superclass   	*/  (WidgetClass) &xfwfBoardClassRec,
/* class_name   	*/  "XfwfLabel",
/* widget_size  	*/  sizeof(XfwfLabelRec),
/* class_initialize 	*/  NULL,
/* class_part_initialize*/  _resolve_inheritance,
/* class_inited 	*/  FALSE,
/* initialize   	*/  initialize,
/* initialize_hook 	*/  NULL,
/* realize      	*/  realize,
/* actions      	*/  NULL,
/* num_actions  	*/  0,
/* resources    	*/  resources,
/* num_resources 	*/  17,
/* xrm_class    	*/  NULLQUARK,
/* compres_motion 	*/  True ,
/* compress_exposure 	*/  XtExposeCompressMultiple ,
/* compress_enterleave 	*/  True ,
/* visible_interest 	*/  False ,
/* destroy      	*/  destroy,
/* resize       	*/  XtInheritResize,
/* expose       	*/  XtInheritExpose,
/* set_values   	*/  set_values,
/* set_values_hook 	*/  NULL,
/* set_values_almost 	*/  XtInheritSetValuesAlmost,
/* get_values+hook 	*/  NULL,
/* accept_focus 	*/  XtInheritAcceptFocus,
/* version      	*/  XtVersion,
/* callback_private 	*/  NULL,
/* tm_table      	*/  NULL,
/* query_geometry 	*/  XtInheritQueryGeometry,
/* display_acceleator 	*/  XtInheritDisplayAccelerator,
/* extension    	*/  NULL 
},
{ /* composite_class part */
XtInheritGeometryManager,
XtInheritChangeManaged,
XtInheritInsertChild,
XtInheritDeleteChild,
NULL
},
{ /* XfwfCommon_class part */
XtInherit_compute_inside,
XtInherit_total_frame_width,
_expose,
XtInherit_highlight_border,
XtInherit_unhighlight_border,
XtInherit_hilite_callbacks,
XtInherit_would_accept_focus,
XtInherit_traverse,
XtInherit_lighter_color,
XtInherit_darker_color,
XtInherit_set_color,
/* traversal_trans */  NULL ,
/* traversal_trans_small */  NULL ,
/* travMode */  1 ,
},
{ /* XfwfFrame_class part */
 /* dummy */  0
},
{ /* XfwfBoard_class part */
XtInherit_set_abs_location,
},
{ /* XfwfLabel_class part */
set_label,
},
};
WidgetClass xfwfLabelWidgetClass = (WidgetClass) &xfwfLabelClassRec;
static void _resolve_inheritance(class)
WidgetClass class;
{
  XfwfLabelWidgetClass c = (XfwfLabelWidgetClass) class;
  XfwfLabelWidgetClass super;
  static CompositeClassExtensionRec extension_rec = {
    NULL, NULLQUARK, XtCompositeExtensionVersion,
    sizeof(CompositeClassExtensionRec), True};
  CompositeClassExtensionRec *ext;
  ext = (XtPointer)XtMalloc(sizeof(*ext));
  *ext = extension_rec;
  ext->next_extension = c->composite_class.extension;
  c->composite_class.extension = ext;
  if (class == xfwfLabelWidgetClass) return;
  super = (XfwfLabelWidgetClass)class->core_class.superclass;
  if (c->xfwfLabel_class.set_label == XtInherit_set_label)
    c->xfwfLabel_class.set_label = super->xfwfLabel_class.set_label;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void set_label(Widget self,String  newlabel)
#else
static void set_label(self,newlabel)Widget self;String  newlabel;
#endif
{
    Position x, y;
    int w, h;

    XtFree(((XfwfLabelWidget)self)->xfwfLabel.label);
    ((XfwfLabelWidget)self)->xfwfLabel.label = XtNewString(newlabel);
    count_lines(self);
    if (XtIsRealized(self)) {
	((XfwfLabelWidgetClass)self->core.widget_class)->xfwfCommon_class.compute_inside(self, &x, &y, &w, &h);
	XClearArea(XtDisplay(self), XtWindow(self), x, y, max(w, 0), max(h, 0), True);
	/* $_expose($, NULL, NULL); */
    }
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static Boolean  set_values(Widget  old,Widget  request,Widget self,ArgList  args,Cardinal * num_args)
#else
static Boolean  set_values(old,request,self,args,num_args)Widget  old;Widget  request;Widget self;ArgList  args;Cardinal * num_args;
#endif
{
    Boolean need_redisplay = False, need_count = False;
    Position x, y;

    if (((XfwfLabelWidget)self)->core.background_pixel != ((XfwfLabelWidget)old)->core.background_pixel)
	if (((XfwfLabelWidget)self)->xfwfLabel.graygc) make_graygc(self);

    if (((XfwfLabelWidget)self)->xfwfLabel.tablist != ((XfwfLabelWidget)old)->xfwfLabel.tablist) {
	XtFree((String) ((XfwfLabelWidget)old)->xfwfLabel.tabs);
	((XfwfLabelWidget)self)->xfwfLabel.tabs = XfwfTablist2Tabs(((XfwfLabelWidget)self)->xfwfLabel.tablist);
	if (((XfwfLabelWidget)self)->xfwfLabel.label != NULL) need_count = True;
    }

    if ((((XfwfLabelWidget)self)->xfwfLabel.font != ((XfwfLabelWidget)old)->xfwfLabel.font)
	|| (((XfwfLabelWidget)self)->xfwfLabel.xfont != ((XfwfLabelWidget)old)->xfwfLabel.xfont)) {
	make_gc(self);
	if (((XfwfLabelWidget)self)->xfwfLabel.label != NULL) need_count = True;
    }
    if (((XfwfLabelWidget)self)->xfwfLabel.foreground != ((XfwfLabelWidget)old)->xfwfLabel.foreground
	|| ((XfwfLabelWidget)self)->core.background_pixel != ((XfwfLabelWidget)old)->core.background_pixel) {
	make_gc(self);
	if (((XfwfLabelWidget)self)->xfwfLabel.label != NULL || ((XfwfLabelWidget)self)->xfwfLabel.pixmap != 0) need_redisplay = True;
    }
    if (((XfwfLabelWidget)self)->xfwfLabel.topMargin != ((XfwfLabelWidget)old)->xfwfLabel.topMargin
	|| ((XfwfLabelWidget)self)->xfwfLabel.bottomMargin != ((XfwfLabelWidget)old)->xfwfLabel.bottomMargin
	|| ((XfwfLabelWidget)self)->xfwfLabel.leftMargin != ((XfwfLabelWidget)old)->xfwfLabel.leftMargin
	|| ((XfwfLabelWidget)self)->xfwfLabel.rightMargin != ((XfwfLabelWidget)old)->xfwfLabel.rightMargin
	|| ((XfwfLabelWidget)self)->xfwfLabel.alignment != ((XfwfLabelWidget)old)->xfwfLabel.alignment)
	need_count = True;

    if ((((XfwfLabelWidget)self)->core.sensitive != ((XfwfLabelWidget)old)->core.sensitive)
        || (((XfwfLabelWidget)self)->xfwfLabel.drawgray != ((XfwfLabelWidget)old)->xfwfLabel.drawgray))
	if (((XfwfLabelWidget)self)->xfwfLabel.label != NULL || ((XfwfLabelWidget)self)->xfwfLabel.pixmap != 0) need_redisplay = True;

    if (((XfwfLabelWidget)self)->xfwfLabel.label != ((XfwfLabelWidget)old)->xfwfLabel.label || ((XfwfLabelWidget)self)->xfwfLabel.pixmap != ((XfwfLabelWidget)old)->xfwfLabel.pixmap) {
	XtFree(((XfwfLabelWidget)old)->xfwfLabel.label);
	((XfwfLabelWidget)self)->xfwfLabel.label = XtNewString(((XfwfLabelWidget)self)->xfwfLabel.label);
	need_count = True;
    }
    if (need_count) {
	count_lines(self);
	need_redisplay = True;
    }
    if (need_count && ((XfwfLabelWidget)self)->xfwfLabel.shrinkToFit) {
        int w, h, wd, ht;
	((XfwfLabelWidgetClass)self->core.widget_class)->xfwfCommon_class.compute_inside(self, &x, &y, &w, &h);
	wd = ((XfwfLabelWidget)self)->xfwfLabel.label_width + ((XfwfLabelWidget)self)->core.width - w;
	ht = ((XfwfLabelWidget)self)->xfwfLabel.label_height + ((XfwfLabelWidget)self)->core.height - h;
	if (wd != ((XfwfLabelWidget)self)->core.width || ht != ((XfwfLabelWidget)self)->core.height) {
	    ((XfwfLabelWidgetClass)self->core.widget_class)->xfwfBoard_class.set_abs_location(self, CWWidth | CWHeight, 0, 0, max(1, wd), max(1, ht));
	    need_redisplay = True;
	}
    }

    return need_redisplay;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void initialize(Widget  request,Widget self,ArgList  args,Cardinal * num_args)
#else
static void initialize(request,self,args,num_args)Widget  request;Widget self;ArgList  args;Cardinal * num_args;
#endif
{
    Position x, y;
    int w, h, wd, ht;

    if (((XfwfLabelWidget)self)->xfwfLabel.label) ((XfwfLabelWidget)self)->xfwfLabel.label = XtNewString(((XfwfLabelWidget)self)->xfwfLabel.label);
    count_lines(self);
    ((XfwfLabelWidget)self)->xfwfLabel.gc = NULL;
    ((XfwfLabelWidget)self)->xfwfLabel.graygc = NULL;
    ((XfwfLabelWidget)self)->xfwfLabel.tabs = XfwfTablist2Tabs(((XfwfLabelWidget)self)->xfwfLabel.tablist);
    if (!((XfwfLabelWidget)self)->xfwfLabel.text_bg)
      ((XfwfLabelWidget)self)->xfwfLabel.text_bg = ((XfwfLabelWidget)self)->core.background_pixel;
    if (((XfwfLabelWidget)self)->xfwfLabel.shrinkToFit) {
	((XfwfLabelWidgetClass)self->core.widget_class)->xfwfCommon_class.compute_inside(self, &x, &y, &w, &h);
	wd = ((XfwfLabelWidget)self)->xfwfLabel.label_width + ((XfwfLabelWidget)self)->core.width - w;
	ht = ((XfwfLabelWidget)self)->xfwfLabel.label_height + ((XfwfLabelWidget)self)->core.height - h;
	((XfwfLabelWidgetClass)self->core.widget_class)->xfwfBoard_class.set_abs_location(self, CWWidth | CWHeight, 0, 0, max(1, wd), max(1, ht));
    }
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void destroy(Widget self)
#else
static void destroy(self)Widget self;
#endif
{
  if (((XfwfLabelWidget)self)->xfwfLabel.gc) XtReleaseGC(self, ((XfwfLabelWidget)self)->xfwfLabel.gc); ((XfwfLabelWidget)self)->xfwfLabel.gc = NULL;
  if (((XfwfLabelWidget)self)->xfwfLabel.graygc) XtReleaseGC(self, ((XfwfLabelWidget)self)->xfwfLabel.graygc); ((XfwfLabelWidget)self)->xfwfLabel.graygc = NULL;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void realize(Widget self,XtValueMask * mask,XSetWindowAttributes * attributes)
#else
static void realize(self,mask,attributes)Widget self;XtValueMask * mask;XSetWindowAttributes * attributes;
#endif
{
  xfwfBoardClassRec.core_class.realize(self, mask, attributes);
  make_gc(self);
  /* make_graygc($); - now on demand */
}
#define draw_line(dpy, win, from, to, reg) do {\
	w1 = XfwfTextWidth(dpy, ((XfwfLabelWidget)self)->xfwfLabel.font, (wxExtFont)((XfwfLabelWidget)self)->xfwfLabel.xfont, ((XfwfLabelWidget)self)->xfwfLabel.label + from, to - from, ((XfwfLabelWidget)self)->xfwfLabel.tabs);\
	if (((XfwfLabelWidget)self)->xfwfLabel.alignment & XfwfLeft)\
	    x = rect.x;\
	else if (((XfwfLabelWidget)self)->xfwfLabel.alignment & XfwfRight)\
	    x = rect.x + rect.width - w1;\
	else\
	    x = rect.x + (rect.width - w1)/2;\
	if (w1) {\
	  int grayed;\
	  grayed = ((!((XfwfLabelWidget)self)->core.sensitive || ((XfwfLabelWidget)self)->xfwfLabel.drawgray) && wx_enough_colors(XtScreen(self)));\
	  XfwfDrawImageString(dpy, win, \
			      (((XfwfLabelWidget)self)->xfwfLabel.xfont\
			       ? NULL\
			       : (grayed\
				  ? ((XfwfLabelWidget)self)->xfwfLabel.graygc \
				  : ((XfwfLabelWidget)self)->xfwfLabel.gc)), \
			      x, y, ((XfwfLabelWidget)self)->xfwfLabel.label + from,\
			      to - from, ((XfwfLabelWidget)self)->xfwfLabel.tabs, ((XfwfLabelWidget)self)->xfwfLabel.font,\
			      (wxExtFont)((XfwfLabelWidget)self)->xfwfLabel.xfont, !grayed, reg);\
	}\
    }while (0 )


/*ARGSUSED*/
#if NeedFunctionPrototypes
static void _expose(Widget self,XEvent * event,Region  region)
#else
static void _expose(self,event,region)Widget self;XEvent * event;Region  region;
#endif
{
    Region reg;
    XRectangle rect;
    int baseline;
    int w1;
    int x, y, i, j;

    if (! XtIsRealized(self)) return;

    if (!((XfwfLabelWidget)self)->core.sensitive || ((XfwfLabelWidget)self)->xfwfLabel.drawgray)
      if (!((XfwfLabelWidget)self)->xfwfLabel.graygc) make_graygc(self);

    xfwfBoardClassRec.xfwfCommon_class._expose(self, event, region);
    reg = NULL;
    if (((XfwfLabelWidget)self)->xfwfLabel.label != NULL || ((XfwfLabelWidget)self)->xfwfLabel.pixmap != 0) {
	int w, h;
	((XfwfLabelWidgetClass)self->core.widget_class)->xfwfCommon_class.compute_inside(self, &rect.x, &rect.y, &w, &h);
	rect.x += ((XfwfLabelWidget)self)->xfwfLabel.leftMargin;  w -= ((XfwfLabelWidget)self)->xfwfLabel.leftMargin + ((XfwfLabelWidget)self)->xfwfLabel.rightMargin;
	rect.y += ((XfwfLabelWidget)self)->xfwfLabel.topMargin;  h -= ((XfwfLabelWidget)self)->xfwfLabel.topMargin + ((XfwfLabelWidget)self)->xfwfLabel.bottomMargin;
	rect.width = max(0, w);
	rect.height = max(0, h);
	reg = XCreateRegion();
	XUnionRectWithRegion(&rect, reg, reg);
	if (region != NULL) XIntersectRegion(region, reg, reg);
	XSetRegion(XtDisplay(self), ((XfwfLabelWidget)self)->xfwfLabel.gc, reg);
    }
    if (((XfwfLabelWidget)self)->xfwfLabel.label != NULL) {
        int ascent;
	ascent = wx_ASCENT(((XfwfLabelWidget)self)->xfwfLabel.font, ((wxExtFont)((XfwfLabelWidget)self)->xfwfLabel.xfont));
	baseline = ascent + wx_DESCENT(((XfwfLabelWidget)self)->xfwfLabel.font, ((wxExtFont)((XfwfLabelWidget)self)->xfwfLabel.xfont));
	if (((XfwfLabelWidget)self)->xfwfLabel.alignment & XfwfTop)
	    y = rect.y + ascent;
	else if (((XfwfLabelWidget)self)->xfwfLabel.alignment & XfwfBottom)
	    y = rect.y + rect.height - ((XfwfLabelWidget)self)->xfwfLabel.nlines * baseline + ascent;
	else
	    y = rect.y + (rect.height - ((XfwfLabelWidget)self)->xfwfLabel.nlines * baseline)/2 + ascent;
	for (i = 0, j = 0; ((XfwfLabelWidget)self)->xfwfLabel.label[i]; i++) {
	    if (((XfwfLabelWidget)self)->xfwfLabel.label[i] == '\n') {
		draw_line(XtDisplay(self), XtWindow(self), j, i, reg);
		j = i + 1;
		y += baseline;
	    }
	}
	draw_line(XtDisplay(self), XtWindow(self), j, i, reg);
    } else if (((XfwfLabelWidget)self)->xfwfLabel.pixmap != 0) {
	Dimension width = ((XfwfLabelWidget)self)->xfwfLabel.label_width - ((XfwfLabelWidget)self)->xfwfLabel.leftMargin - ((XfwfLabelWidget)self)->xfwfLabel.rightMargin;
	Dimension height = ((XfwfLabelWidget)self)->xfwfLabel.label_height - ((XfwfLabelWidget)self)->xfwfLabel.topMargin - ((XfwfLabelWidget)self)->xfwfLabel.bottomMargin;
	if (((XfwfLabelWidget)self)->xfwfLabel.alignment & XfwfTop)
	    y = rect.y;
	else if (((XfwfLabelWidget)self)->xfwfLabel.alignment & XfwfBottom)
	    y = rect.y + rect.height - height;
	else
	    y = rect.y + (rect.height - height)/2;
	if (((XfwfLabelWidget)self)->xfwfLabel.alignment & XfwfLeft)
	    x = rect.x;
	else if (((XfwfLabelWidget)self)->xfwfLabel.alignment & XfwfRight)
	    x = rect.x + rect.width - width;
	else
	    x = rect.x + (rect.width - width)/2;
	if (((XfwfLabelWidget)self)->xfwfLabel.label_depth == 1) { /* depth */
	    XCopyPlane(XtDisplay(self), ((XfwfLabelWidget)self)->xfwfLabel.pixmap, XtWindow(self), ((XfwfLabelWidget)self)->xfwfLabel.gc,
		       0, 0, width, height, x, y, 1L);
	} else {
	    XCopyArea(XtDisplay(self), ((XfwfLabelWidget)self)->xfwfLabel.pixmap, XtWindow(self), ((XfwfLabelWidget)self)->xfwfLabel.gc,
		      0, 0, width, height, x, y);
	}
    }

    /* Gray out if not sensitive */
    if (! ((XfwfLabelWidget)self)->core.sensitive || ((XfwfLabelWidget)self)->xfwfLabel.drawgray) {
      if ((((XfwfLabelWidget)self)->xfwfLabel.pixmap != 0) || !wx_enough_colors(XtScreen(self))) {
	    if (!((XfwfLabelWidget)self)->xfwfLabel.graygc) make_graygc(self);
	    XSetRegion(XtDisplay(self), ((XfwfLabelWidget)self)->xfwfLabel.graygc, reg);
	    XFillRectangle(XtDisplay(self), XtWindow(self), ((XfwfLabelWidget)self)->xfwfLabel.graygc, rect.x,
			   rect.y, rect.width, rect.height);
	    XSetClipMask(XtDisplay(self), ((XfwfLabelWidget)self)->xfwfLabel.graygc, None);
	}
    }
    if (((XfwfLabelWidget)self)->xfwfLabel.label != NULL || ((XfwfLabelWidget)self)->xfwfLabel.pixmap != 0) {
      XSetClipMask(XtDisplay(self), ((XfwfLabelWidget)self)->xfwfLabel.gc, None);
    }
    if (reg) XDestroyRegion(reg);
}
