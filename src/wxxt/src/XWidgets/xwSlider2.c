/* Generated by wbuild
 * (generator version 3.2)
 */
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/bitmaps/gray>
#include <stdio.h>
#include <./xwSlider2P.h>
static void start(
#if NeedFunctionPrototypes
Widget,XEvent*,String*,Cardinal*
#endif
);
static void finish(
#if NeedFunctionPrototypes
Widget,XEvent*,String*,Cardinal*
#endif
);
static void drag(
#if NeedFunctionPrototypes
Widget,XEvent*,String*,Cardinal*
#endif
);

static XtActionsRec actionsList[] = {
{"start", start},
{"finish", finish},
{"drag", drag},
};

static char defaultTranslations[] = "\
<Btn1Down>: start() \n\
<Btn1Motion>: drag() \n\
<Btn1Up>: finish() \n\
";
static void _resolve_inheritance(
#if NeedFunctionPrototypes
WidgetClass
#endif
);
static void compute_thumb(
#if NeedFunctionPrototypes
Widget,Position *,Position *,Dimension *,Dimension *
#endif
);
static void compute_inside(
#if NeedFunctionPrototypes
Widget,Position *,Position *,int *,int *
#endif
);
static void _expose(
#if NeedFunctionPrototypes
Widget,XEvent *,Region 
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
static void move_thumb(
#if NeedFunctionPrototypes
Widget,int ,int ,int ,int ,int ,int 
#endif
);
static void compute_info(
#if NeedFunctionPrototypes
Widget,Position *,Position *,Dimension *,Dimension *,float *,float *,float *,float *
#endif
);
static Boolean  set_values(
#if NeedFunctionPrototypes
Widget ,Widget ,Widget,ArgList ,Cardinal *
#endif
);
static void scroll_response(
#if NeedFunctionPrototypes
Widget ,XtPointer ,XtPointer 
#endif
);
static void create_gc(
#if NeedFunctionPrototypes
Widget
#endif
);
static void create_graygc(
#if NeedFunctionPrototypes
Widget
#endif
);
static void create_thumbgc(
#if NeedFunctionPrototypes
Widget
#endif
);
static void create_thumblightgc(
#if NeedFunctionPrototypes
Widget
#endif
);
static void create_thumbdarkgc(
#if NeedFunctionPrototypes
Widget
#endif
);
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void create_gc(Widget self)
#else
static void create_gc(self)Widget self;
#endif
{
    XtGCMask mask;
    XGCValues values;

    if (((XfwfSlider2Widget)self)->xfwfLabel.gc != NULL) XtReleaseGC(self, ((XfwfSlider2Widget)self)->xfwfLabel.gc);
    values.background = ((XfwfSlider2Widget)self)->xfwfSlider2.thumbColor;
    values.foreground = ((XfwfSlider2Widget)self)->xfwfLabel.foreground;
    values.font = ((XfwfSlider2Widget)self)->xfwfLabel.font->fid;
    mask = GCFont | GCBackground | GCForeground;
    ((XfwfSlider2Widget)self)->xfwfLabel.gc = XtGetGC(self, mask, &values);

    if (((XfwfSlider2Widget)self)->xfwfLabel.rv_gc != NULL) XtReleaseGC(self, ((XfwfSlider2Widget)self)->xfwfLabel.rv_gc);
    values.foreground = ((XfwfSlider2Widget)self)->xfwfSlider2.thumbColor;
    values.background = ((XfwfSlider2Widget)self)->xfwfLabel.foreground;
    values.font = ((XfwfSlider2Widget)self)->xfwfLabel.font->fid;
    mask = GCFont | GCBackground | GCForeground;
    ((XfwfSlider2Widget)self)->xfwfLabel.rv_gc = XtGetGC(self, mask, &values);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void create_graygc(Widget self)
#else
static void create_graygc(self)Widget self;
#endif
{
    XtGCMask mask;
    XGCValues values;

    if (((XfwfSlider2Widget)self)->xfwfLabel.graygc != NULL) XtReleaseGC(self, ((XfwfSlider2Widget)self)->xfwfLabel.graygc);
    values.foreground = ((XfwfSlider2Widget)self)->xfwfSlider2.thumbColor;
    values.stipple =
	XCreateBitmapFromData(XtDisplay(self),
			      RootWindowOfScreen(XtScreen(self)),
			      gray_bits, gray_width, gray_height);
    values.fill_style = FillStippled;
    mask = GCForeground | GCStipple | GCFillStyle;
    ((XfwfSlider2Widget)self)->xfwfLabel.graygc = XtGetGC(self, mask, &values);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void create_thumbgc(Widget self)
#else
static void create_thumbgc(self)Widget self;
#endif
{
    XtGCMask mask;
    XGCValues values;

    if (((XfwfSlider2Widget)self)->xfwfSlider2.thumbgc != NULL) XtReleaseGC(self, ((XfwfSlider2Widget)self)->xfwfSlider2.thumbgc);
    if (((XfwfSlider2Widget)self)->xfwfSlider2.thumbPixmap != 0) {
	mask = GCTile | GCFillStyle;
	values.tile = ((XfwfSlider2Widget)self)->xfwfSlider2.thumbPixmap;
	values.fill_style = FillTiled;
    } else {
	mask = GCForeground;
	values.foreground = ((XfwfSlider2Widget)self)->xfwfSlider2.thumbColor;
    }
    ((XfwfSlider2Widget)self)->xfwfSlider2.thumbgc = XtGetGC(self, mask, &values);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void create_thumblightgc(Widget self)
#else
static void create_thumblightgc(self)Widget self;
#endif
{
    XtGCMask mask=0;
    XGCValues values;

    if (((XfwfSlider2Widget)self)->xfwfSlider2.thumblightgc != NULL) XtReleaseGC(self, ((XfwfSlider2Widget)self)->xfwfSlider2.thumblightgc);
    switch (((XfwfSlider2Widget)self)->xfwfFrame.shadowScheme) {
    case XfwfColor:
	mask = GCForeground;
	values.foreground = ((XfwfSlider2Widget)self)->xfwfFrame.topShadowColor;
	break;
    case XfwfStipple:
	mask = GCFillStyle | GCStipple | GCForeground | GCBackground;
	values.fill_style = FillOpaqueStippled;
	values.background = ((XfwfSlider2Widget)self)->xfwfSlider2.thumbColor;
	values.stipple = ((XfwfSlider2Widget)self)->xfwfFrame.topShadowStipple;
	values.foreground = WhitePixelOfScreen(XtScreen(self));
	break;
    case XfwfBlack:
    case XfwfAuto:
	if (DefaultDepthOfScreen(XtScreen(self)) > 4
	    && ((XfwfSlider2WidgetClass)self->core.widget_class)->xfwfCommon_class.lighter_color(self, ((XfwfSlider2Widget)self)->xfwfSlider2.thumbColor, &values.foreground)) {
	    mask = GCForeground;
	} else {
	    mask = GCFillStyle | GCBackground | GCForeground | GCStipple;
	    values.fill_style = FillOpaqueStippled;
	    /* values.background = $thumbColor; */
	    values.background = BlackPixelOfScreen(XtScreen(self));
	    values.foreground = WhitePixelOfScreen(XtScreen(self));
	    values.stipple =
		XCreateBitmapFromData(XtDisplay(self),
				      RootWindowOfScreen(XtScreen(self)),
				      gray_bits, gray_width, gray_height);
	}
	break;
    }
    ((XfwfSlider2Widget)self)->xfwfSlider2.thumblightgc = XtGetGC(self, mask, &values);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void create_thumbdarkgc(Widget self)
#else
static void create_thumbdarkgc(self)Widget self;
#endif
{
    XtGCMask mask=0;
    XGCValues values;

    if (((XfwfSlider2Widget)self)->xfwfSlider2.thumbdarkgc != NULL) XtReleaseGC(self, ((XfwfSlider2Widget)self)->xfwfSlider2.thumbdarkgc);
    switch (((XfwfSlider2Widget)self)->xfwfFrame.shadowScheme) {
    case XfwfColor:
	mask = GCForeground;
	values.foreground = ((XfwfSlider2Widget)self)->xfwfFrame.bottomShadowColor;
	break;
    case XfwfStipple:
	mask = GCFillStyle | GCStipple | GCForeground | GCBackground;
	values.fill_style = FillOpaqueStippled;
	values.stipple = ((XfwfSlider2Widget)self)->xfwfFrame.bottomShadowStipple;
	values.foreground = BlackPixelOfScreen(XtScreen(self));
	values.background = ((XfwfSlider2Widget)self)->xfwfSlider2.thumbColor;
	break;
    case XfwfBlack:
    case XfwfAuto:
	if (DefaultDepthOfScreen(XtScreen(self)) > 4
	    && ((XfwfSlider2WidgetClass)self->core.widget_class)->xfwfCommon_class.darker_color(self, ((XfwfSlider2Widget)self)->xfwfSlider2.thumbColor, &values.foreground)) {
	    mask = GCForeground;
	} else {
	    mask = GCFillStyle | GCBackground | GCForeground | GCStipple;
	    values.fill_style = FillOpaqueStippled;
	    /* values.background = $thumbColor; */
	    values.background = BlackPixelOfScreen(XtScreen(self));
	    values.foreground = WhitePixelOfScreen(XtScreen(self));
	    values.stipple =
		XCreateBitmapFromData(XtDisplay(self),
				      RootWindowOfScreen(XtScreen(self)),
				      gray_bits, gray_width, gray_height);
	}
	break;
    }
    ((XfwfSlider2Widget)self)->xfwfSlider2.thumbdarkgc = XtGetGC(self, mask, &values);
}

static XtResource resources[] = {
{XtNthumbColor,XtCThumbColor,XtRPixel,sizeof(((XfwfSlider2Rec*)NULL)->xfwfSlider2.thumbColor),XtOffsetOf(XfwfSlider2Rec,xfwfSlider2.thumbColor),XtRString,(XtPointer)XtDefaultBackground },
{XtNthumbPixmap,XtCThumbPixmap,XtRPixmap,sizeof(((XfwfSlider2Rec*)NULL)->xfwfSlider2.thumbPixmap),XtOffsetOf(XfwfSlider2Rec,xfwfSlider2.thumbPixmap),XtRImmediate,(XtPointer)NULL },
{XtNminsize,XtCMinsize,XtRDimension,sizeof(((XfwfSlider2Rec*)NULL)->xfwfSlider2.minsize),XtOffsetOf(XfwfSlider2Rec,xfwfSlider2.minsize),XtRImmediate,(XtPointer)20 },
{XtNthumbFrameWidth,XtCThumbFrameWidth,XtRDimension,sizeof(((XfwfSlider2Rec*)NULL)->xfwfSlider2.thumbFrameWidth),XtOffsetOf(XfwfSlider2Rec,xfwfSlider2.thumbFrameWidth),XtRImmediate,(XtPointer)2 },
{XtNthumbFrameType,XtCThumbFrameType,XtRFrameType,sizeof(((XfwfSlider2Rec*)NULL)->xfwfSlider2.thumbFrameType),XtOffsetOf(XfwfSlider2Rec,xfwfSlider2.thumbFrameType),XtRImmediate,(XtPointer)XfwfRaised },
{XtNscrollCallback,XtCScrollCallback,XtRCallback,sizeof(((XfwfSlider2Rec*)NULL)->xfwfSlider2.scrollCallback),XtOffsetOf(XfwfSlider2Rec,xfwfSlider2.scrollCallback),XtRImmediate,(XtPointer)NULL },
{XtNscrollResponse,XtCScrollResponse,XtRXTCallbackProc,sizeof(((XfwfSlider2Rec*)NULL)->xfwfSlider2.scrollResponse),XtOffsetOf(XfwfSlider2Rec,xfwfSlider2.scrollResponse),XtRImmediate,(XtPointer)scroll_response },
{XtNframeWidth,XtCFrameWidth,XtRDimension,sizeof(((XfwfSlider2Rec*)NULL)->xfwfFrame.frameWidth),XtOffsetOf(XfwfSlider2Rec,xfwfFrame.frameWidth),XtRImmediate,(XtPointer)2 },
{XtNframeType,XtCFrameType,XtRFrameType,sizeof(((XfwfSlider2Rec*)NULL)->xfwfFrame.frameType),XtOffsetOf(XfwfSlider2Rec,xfwfFrame.frameType),XtRImmediate,(XtPointer)XfwfSunken },
};

XfwfSlider2ClassRec xfwfSlider2ClassRec = {
{ /* core_class part */
/* superclass   	*/  (WidgetClass) &xfwfLabelClassRec,
/* class_name   	*/  "XfwfSlider2",
/* widget_size  	*/  sizeof(XfwfSlider2Rec),
/* class_initialize 	*/  NULL,
/* class_part_initialize*/  _resolve_inheritance,
/* class_inited 	*/  FALSE,
/* initialize   	*/  initialize,
/* initialize_hook 	*/  NULL,
/* realize      	*/  XtInheritRealize,
/* actions      	*/  actionsList,
/* num_actions  	*/  3,
/* resources    	*/  resources,
/* num_resources 	*/  9,
/* xrm_class    	*/  NULLQUARK,
/* compres_motion 	*/  True ,
/* compress_exposure 	*/  XtExposeCompressMultiple |XtExposeGraphicsExpose ,
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
/* tm_table      	*/  defaultTranslations,
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
compute_inside,
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
XtInherit_set_label,
},
{ /* XfwfSlider2_class part */
compute_thumb,
move_thumb,
compute_info,
scroll_response,
},
};
WidgetClass xfwfSlider2WidgetClass = (WidgetClass) &xfwfSlider2ClassRec;
/*ARGSUSED*/
static void start(self,event,params,num_params)Widget self;XEvent*event;String*params;Cardinal*num_params;
{
    Dimension w, h;
    Position x, y;
    XfwfScrollInfo info;
    Boolean outside = False;

    if (event->type != ButtonPress && event->type != ButtonRelease
	&& event->type != MotionNotify)
	XtError("The start action must be bound to a mouse event");
    ((XfwfSlider2WidgetClass)self->core.widget_class)->xfwfSlider2_class.compute_thumb(self, &x, &y, &w, &h);
    if (event->xbutton.x < x) {			/* Left of thumb */
	info.reason = XfwfSPageLeft;
	info.flags = XFWF_HPOS;			/* Suggest a value: */
	info.hpos = max(0.0, ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_x - ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_wd);
	outside = True;
	XtCallCallbackList(self, ((XfwfSlider2Widget)self)->xfwfSlider2.scrollCallback, &info);
    }
    if (event->xbutton.x >= x + w) {		/* Right of thumb */
	info.reason = XfwfSPageRight;
	info.flags = XFWF_HPOS;			/* Suggest a value: */
	info.hpos = min(1.0, ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_x + ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_wd);
	outside = True;
	XtCallCallbackList(self, ((XfwfSlider2Widget)self)->xfwfSlider2.scrollCallback, &info);
    }
    if (event->xbutton.y < y) {			/* Above thumb */
	info.reason = XfwfSPageUp;
	info.flags = XFWF_VPOS;			/* Suggest a value: */
	info.vpos = max(0.0, ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_y - ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_ht);
	outside = True;
	XtCallCallbackList(self, ((XfwfSlider2Widget)self)->xfwfSlider2.scrollCallback, &info);
    }
    if (event->xbutton.y >= y + h) {		/* Below thumb */
	info.reason = XfwfSPageDown;
	info.flags = XFWF_VPOS;			/* Suggest a value: */
	info.vpos = min(1.0, ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_y + ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_ht);
	outside = True;
	XtCallCallbackList(self, ((XfwfSlider2Widget)self)->xfwfSlider2.scrollCallback, &info);
    }
    if (! outside) {				/* Inside the thumb */
	((XfwfSlider2Widget)self)->xfwfSlider2.drag_in_progress = True;
	((XfwfSlider2Widget)self)->xfwfSlider2.m_delta_x = x - event->xbutton.x;
	((XfwfSlider2Widget)self)->xfwfSlider2.m_delta_y = y - event->xbutton.y;
    }
}

/*ARGSUSED*/
static void finish(self,event,params,num_params)Widget self;XEvent*event;String*params;Cardinal*num_params;
{
    XfwfScrollInfo info;

    if (((XfwfSlider2Widget)self)->xfwfSlider2.drag_in_progress) {
	((XfwfSlider2Widget)self)->xfwfSlider2.drag_in_progress = False;
	info.reason = XfwfSMove;
	info.flags = XFWF_VPOS | XFWF_HPOS;
	info.hpos = ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_x;
	info.vpos = ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_y;
	XtCallCallbackList(self, ((XfwfSlider2Widget)self)->xfwfSlider2.scrollCallback, &info);
    }
}

/*ARGSUSED*/
static void drag(self,event,params,num_params)Widget self;XEvent*event;String*params;Cardinal*num_params;
{
    XfwfScrollInfo info;
    Dimension wd, ht;
    Position oldx, oldy, newx, newy;
    float dum1, dum2;

    if (! ((XfwfSlider2Widget)self)->xfwfSlider2.drag_in_progress) return;
    if (event->type != ButtonPress && event->type != ButtonRelease
	&& event->type != MotionNotify)
	XtError("The drag action must be bound to a mouse event");
    ((XfwfSlider2WidgetClass)self->core.widget_class)->xfwfSlider2_class.compute_thumb(self, &oldx, &oldy, &wd, &ht);
    newx = event->xbutton.x + ((XfwfSlider2Widget)self)->xfwfSlider2.m_delta_x;
    newy = event->xbutton.y + ((XfwfSlider2Widget)self)->xfwfSlider2.m_delta_y;
    ((XfwfSlider2WidgetClass)self->core.widget_class)->xfwfSlider2_class.compute_info(self, &newx, &newy, &wd, &ht, &((XfwfSlider2Widget)self)->xfwfSlider2.thumb_x, &((XfwfSlider2Widget)self)->xfwfSlider2.thumb_y,&dum1,&dum2);
    ((XfwfSlider2WidgetClass)self->core.widget_class)->xfwfSlider2_class.move_thumb(self, oldx, oldy, wd, ht, newx, newy);
    info.reason = XfwfSDrag;
    info.flags = XFWF_VPOS | XFWF_HPOS;
    info.hpos = ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_x;
    info.vpos = ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_y;
    XtCallCallbackList(self, ((XfwfSlider2Widget)self)->xfwfSlider2.scrollCallback, &info);
}

static void _resolve_inheritance(class)
WidgetClass class;
{
  XfwfSlider2WidgetClass c = (XfwfSlider2WidgetClass) class;
  XfwfSlider2WidgetClass super;
  static CompositeClassExtensionRec extension_rec = {
    NULL, NULLQUARK, XtCompositeExtensionVersion,
    sizeof(CompositeClassExtensionRec), True};
  CompositeClassExtensionRec *ext;
  ext = (XtPointer)XtMalloc(sizeof(*ext));
  *ext = extension_rec;
  ext->next_extension = c->composite_class.extension;
  c->composite_class.extension = ext;
  if (class == xfwfSlider2WidgetClass) return;
  super = (XfwfSlider2WidgetClass)class->core_class.superclass;
  if (c->xfwfSlider2_class.compute_thumb == XtInherit_compute_thumb)
    c->xfwfSlider2_class.compute_thumb = super->xfwfSlider2_class.compute_thumb;
  if (c->xfwfSlider2_class.move_thumb == XtInherit_move_thumb)
    c->xfwfSlider2_class.move_thumb = super->xfwfSlider2_class.move_thumb;
  if (c->xfwfSlider2_class.compute_info == XtInherit_compute_info)
    c->xfwfSlider2_class.compute_info = super->xfwfSlider2_class.compute_info;
  if (c->xfwfSlider2_class.scroll_response == XtInherit_scroll_response)
    c->xfwfSlider2_class.scroll_response = super->xfwfSlider2_class.scroll_response;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void compute_thumb(Widget self,Position * x,Position * y,Dimension * width,Dimension * height)
#else
static void compute_thumb(self,x,y,width,height)Widget self;Position * x;Position * y;Dimension * width;Dimension * height;
#endif
{
    Position fx, fy;
    int fw, fh;

    xfwfLabelClassRec.xfwfCommon_class.compute_inside(self, &fx, &fy, &fw, &fh);
    fw = max(0, fw);
    fh = max(0, fh);
    *width = ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_wd * fw + 0.5;
    *height = ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_ht * fh + 0.5;
    if (*width < ((XfwfSlider2Widget)self)->xfwfSlider2.minsize) *width = min(fw, ((XfwfSlider2Widget)self)->xfwfSlider2.minsize);
    if (*height < ((XfwfSlider2Widget)self)->xfwfSlider2.minsize) *height = min(fh, ((XfwfSlider2Widget)self)->xfwfSlider2.minsize);
    *x = fx + ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_x * (fw - *width) + 0.5;
    *y = fy + ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_y * (fh - *height) + 0.5;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void compute_inside(Widget self,Position * x,Position * y,int * w,int * h)
#else
static void compute_inside(self,x,y,w,h)Widget self;Position * x;Position * y;int * w;int * h;
#endif
{
    Dimension ww, hh;

    ((XfwfSlider2WidgetClass)self->core.widget_class)->xfwfSlider2_class.compute_thumb(self, x, y, &ww, &hh);
    *x += ((XfwfSlider2Widget)self)->xfwfSlider2.thumbFrameWidth;
    *y += ((XfwfSlider2Widget)self)->xfwfSlider2.thumbFrameWidth;
    *w = (int)ww - 2 * ((XfwfSlider2Widget)self)->xfwfSlider2.thumbFrameWidth;
    *h = (int)hh - 2 * ((XfwfSlider2Widget)self)->xfwfSlider2.thumbFrameWidth;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void _expose(Widget self,XEvent * event,Region  region)
#else
static void _expose(self,event,region)Widget self;XEvent * event;Region  region;
#endif
{
    Position x, y;
    Dimension wd, ht, d;

    if (! XtIsRealized(self)) return;
    if (region != NULL) {
	XSetRegion(XtDisplay(self), ((XfwfSlider2Widget)self)->xfwfSlider2.thumbgc, region);
	XSetRegion(XtDisplay(self), ((XfwfSlider2Widget)self)->xfwfSlider2.thumbdarkgc, region);
	XSetRegion(XtDisplay(self), ((XfwfSlider2Widget)self)->xfwfSlider2.thumblightgc, region);
    }
    ((XfwfSlider2WidgetClass)self->core.widget_class)->xfwfSlider2_class.compute_thumb(self, &x, &y, &wd, &ht);
    d = ((XfwfSlider2Widget)self)->xfwfSlider2.thumbFrameWidth;
    XFillRectangle(XtDisplay(self), XtWindow(self), ((XfwfSlider2Widget)self)->xfwfSlider2.thumbgc, x + d, y + d, wd - 2 * d, ht - 2 * d);
    XfwfDrawFrame(self, x, y, wd, ht, ((XfwfSlider2Widget)self)->xfwfSlider2.thumbFrameType, d,
	       ((XfwfSlider2Widget)self)->xfwfSlider2.thumblightgc, ((XfwfSlider2Widget)self)->xfwfSlider2.thumbdarkgc);
    if (region != NULL) {
	XSetClipMask(XtDisplay(self), ((XfwfSlider2Widget)self)->xfwfSlider2.thumbgc, None);
	XSetClipMask(XtDisplay(self), ((XfwfSlider2Widget)self)->xfwfSlider2.thumbdarkgc, None);
	XSetClipMask(XtDisplay(self), ((XfwfSlider2Widget)self)->xfwfSlider2.thumblightgc, None);
    }
    xfwfLabelClassRec.xfwfCommon_class._expose(self, event, region);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void initialize(Widget  request,Widget self,ArgList  args,Cardinal * num_args)
#else
static void initialize(request,self,args,num_args)Widget  request;Widget self;ArgList  args;Cardinal * num_args;
#endif
{
    ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_x = ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_y = 0.0;
    ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_wd = ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_ht = 1.0;
    ((XfwfSlider2Widget)self)->xfwfSlider2.drag_in_progress = False;
    create_thumbgc(self);
    create_gc(self);
    create_graygc(self);
    ((XfwfSlider2Widget)self)->xfwfSlider2.thumblightgc = NULL; create_thumblightgc(self);
    ((XfwfSlider2Widget)self)->xfwfSlider2.thumbdarkgc = NULL; create_thumbdarkgc(self);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void destroy(Widget self)
#else
static void destroy(self)Widget self;
#endif
{
  if (((XfwfSlider2Widget)self)->xfwfLabel.gc) XtReleaseGC(self, ((XfwfSlider2Widget)self)->xfwfLabel.gc); ((XfwfSlider2Widget)self)->xfwfLabel.gc = NULL;
  if (((XfwfSlider2Widget)self)->xfwfLabel.rv_gc) XtReleaseGC(self, ((XfwfSlider2Widget)self)->xfwfLabel.rv_gc); ((XfwfSlider2Widget)self)->xfwfLabel.rv_gc = NULL;
  if (((XfwfSlider2Widget)self)->xfwfLabel.graygc) XtReleaseGC(self, ((XfwfSlider2Widget)self)->xfwfLabel.graygc); ((XfwfSlider2Widget)self)->xfwfLabel.graygc = NULL;
  if (((XfwfSlider2Widget)self)->xfwfSlider2.thumblightgc) XtReleaseGC(self, ((XfwfSlider2Widget)self)->xfwfSlider2.thumblightgc); ((XfwfSlider2Widget)self)->xfwfSlider2.thumblightgc = NULL;
  if (((XfwfSlider2Widget)self)->xfwfSlider2.thumbdarkgc) XtReleaseGC(self, ((XfwfSlider2Widget)self)->xfwfSlider2.thumbdarkgc); ((XfwfSlider2Widget)self)->xfwfSlider2.thumbdarkgc = NULL;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void move_thumb(Widget self,int  oldx,int  oldy,int  wd,int  ht,int  newx,int  newy)
#else
static void move_thumb(self,oldx,oldy,wd,ht,newx,newy)Widget self;int  oldx;int  oldy;int  wd;int  ht;int  newx;int  newy;
#endif
{
    int h, ah;

    XCopyArea(XtDisplay(self), XtWindow(self), XtWindow(self),
	      DefaultGCOfScreen(XtScreen(self)),
	      oldx, oldy, wd, ht, newx, newy);
    /* First check if the old and new areas do not overlap */
    if (newx + wd <= oldx || oldx + wd <= newx
	|| newy + ht <= oldy || oldy + ht <= newy) {
	XClearArea(XtDisplay(self), XtWindow(self), oldx, oldy, wd, ht, False);
	return;
    } else {					/* They do overlap */
	h = oldy - newy;
	if (h > 0)
	    XClearArea(XtDisplay(self), XtWindow(self), oldx, newy + ht, wd,h,False);
	else if (h < 0)
	    XClearArea(XtDisplay(self), XtWindow(self), oldx, oldy, wd, -h, False);
	ah = (h < 0) ? -h : h;
	if (newx < oldx)
	    XClearArea(XtDisplay(self), XtWindow(self), newx + wd,
		       max(oldy, newy), oldx - newx, ht - ah, False);
	else if (oldx < newx)
	    XClearArea(XtDisplay(self), XtWindow(self), oldx, max(oldy, newy),
		       newx - oldx, ht - ah, False);
    }
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void compute_info(Widget self,Position * x,Position * y,Dimension * w,Dimension * h,float * thumb_x,float * thumb_y,float * thumb_wd,float * thumb_ht)
#else
static void compute_info(self,x,y,w,h,thumb_x,thumb_y,thumb_wd,thumb_ht)Widget self;Position * x;Position * y;Dimension * w;Dimension * h;float * thumb_x;float * thumb_y;float * thumb_wd;float * thumb_ht;
#endif
{
    int fw, fh;
    Position fx, fy;

    xfwfLabelClassRec.xfwfCommon_class.compute_inside(self, &fx, &fy, &fw, &fh);
    fw = max(0, fw);
    fh = max(0, fh);
    *w = min(fw, max(((XfwfSlider2Widget)self)->xfwfSlider2.minsize, *w));
    *h = min(fh, max(((XfwfSlider2Widget)self)->xfwfSlider2.minsize, *h));
    *x = min(fx + fw - *w, max(fx, *x));
    *y = min(fy + fh - *h, max(fy, *y));
    *thumb_wd = ((float) *w)/fw;
    *thumb_ht = ((float) *h)/fh;
    *thumb_x = (*w == fw) ? 0.0 : ((float) (*x - fx))/(fw - *w);
    *thumb_y = (*h == fh) ? 0.0 : ((float) (*y - fy))/(fh - *h);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static Boolean  set_values(Widget  old,Widget  request,Widget self,ArgList  args,Cardinal * num_args)
#else
static Boolean  set_values(old,request,self,args,num_args)Widget  old;Widget  request;Widget self;ArgList  args;Cardinal * num_args;
#endif
{
    Boolean need_redisplay = False;
    Position x, y;
    Dimension w, h;

    if (((XfwfSlider2Widget)self)->xfwfSlider2.thumbPixmap != ((XfwfSlider2Widget)old)->xfwfSlider2.thumbPixmap) {
	create_thumbgc(self);
	create_thumblightgc(self);
	create_thumbdarkgc(self);
	need_redisplay = True;
    } else if (((XfwfSlider2Widget)self)->xfwfSlider2.thumbColor != ((XfwfSlider2Widget)old)->xfwfSlider2.thumbColor) {
	((XfwfSlider2Widget)self)->xfwfSlider2.thumbPixmap = 0;
	create_thumbgc(self);
	create_thumblightgc(self);
	create_thumbdarkgc(self);
	need_redisplay = True;
    }
    if (((XfwfSlider2Widget)self)->xfwfSlider2.thumbFrameWidth != ((XfwfSlider2Widget)old)->xfwfSlider2.thumbFrameWidth)
	need_redisplay = True;
    if (((XfwfSlider2Widget)self)->xfwfSlider2.thumbFrameType != ((XfwfSlider2Widget)old)->xfwfSlider2.thumbFrameType)
	need_redisplay = True;
    if (((XfwfSlider2Widget)self)->xfwfSlider2.minsize != ((XfwfSlider2Widget)old)->xfwfSlider2.minsize) {
	compute_thumb(old, &x, &y, &w, &h);
	if (w < ((XfwfSlider2Widget)self)->xfwfSlider2.minsize || h < ((XfwfSlider2Widget)self)->xfwfSlider2.minsize) need_redisplay = True;
    }
    if (((XfwfSlider2Widget)self)->xfwfSlider2.scrollResponse != ((XfwfSlider2Widget)old)->xfwfSlider2.scrollResponse) {
	((XfwfSlider2Widget)self)->xfwfSlider2.scrollResponse = ((XfwfSlider2Widget)old)->xfwfSlider2.scrollResponse;
	XtWarning("scrollResponse resource may only be queried, not set");
    }

    return need_redisplay;
}
#define range(x) (0.0 <=(x )&&(x )<=1.0 )


/*ARGSUSED*/
#if NeedFunctionPrototypes
static void scroll_response(Widget  wdg,XtPointer  client_data,XtPointer  call_data)
#else
static void scroll_response(wdg,client_data,call_data)Widget  wdg;XtPointer  client_data;XtPointer  call_data;
#endif
{
    Widget self = (Widget) client_data;
    XfwfScrollInfo *inf = (XfwfScrollInfo *)call_data;
    XfwfScrollInfo new_info;
    float x, y, w, h;
    Position newx, newy, oldx, oldy;
    Dimension newwd, newht, oldwd, oldht, wd, ht;
    XEvent event;
    XRectangle rect;
    Region clip;
    Display *dpy = XtDisplay(self);

    x = (inf->flags&XFWF_HPOS) && range(inf->hpos) ? inf->hpos : ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_x;
    y = (inf->flags&XFWF_VPOS) && range(inf->vpos) ? inf->vpos : ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_y;
    w = (inf->flags&XFWF_HSIZE) && range(inf->hsize) ? inf->hsize : ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_wd;
    h = (inf->flags&XFWF_VSIZE) && range(inf->vsize) ? inf->vsize : ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_ht;

    if (((XfwfSlider2Widget)self)->xfwfSlider2.thumb_wd != w || ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_ht != h) {	/* Size changed */
	if (XtIsRealized(self))
	    ((XfwfSlider2WidgetClass)self->core.widget_class)->xfwfSlider2_class.compute_thumb(self, &oldx, &oldy, &oldwd, &oldht);
	((XfwfSlider2Widget)self)->xfwfSlider2.thumb_wd = w;
	((XfwfSlider2Widget)self)->xfwfSlider2.thumb_ht = h;
	((XfwfSlider2Widget)self)->xfwfSlider2.thumb_x = x;
	((XfwfSlider2Widget)self)->xfwfSlider2.thumb_y = y;
	if (XtIsRealized(self)) {
	    ((XfwfSlider2WidgetClass)self->core.widget_class)->xfwfSlider2_class.compute_thumb(self, &newx, &newy, &newwd, &newht);
	    if ((oldwd > newwd) || (oldht > newht))
	      XClearArea(dpy, XtWindow(self), oldx, oldy, oldwd, oldht, False);
	    event.xexpose.x = rect.x = newx;
	    event.xexpose.y = rect.y = newy;
	    event.xexpose.width = rect.width = newwd;
	    event.xexpose.height = rect.height = newht;
	    clip = XCreateRegion();
	    XUnionRectWithRegion(&rect, clip, clip);
	    ((XfwfSlider2WidgetClass)self->core.widget_class)->xfwfCommon_class._expose(self, &event, clip);
	    XDestroyRegion(clip);
	}
    } else if (((XfwfSlider2Widget)self)->xfwfSlider2.thumb_x != x || ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_y != y) { /* Only position changed */
	if (XtIsRealized(self))
	    ((XfwfSlider2WidgetClass)self->core.widget_class)->xfwfSlider2_class.compute_thumb(self, &oldx, &oldy, &wd, &ht);
	((XfwfSlider2Widget)self)->xfwfSlider2.thumb_x = x;
	((XfwfSlider2Widget)self)->xfwfSlider2.thumb_y = y;
	if (XtIsRealized(self)) {
	    ((XfwfSlider2WidgetClass)self->core.widget_class)->xfwfSlider2_class.compute_thumb(self, &newx, &newy, &wd, &ht);
	    ((XfwfSlider2WidgetClass)self->core.widget_class)->xfwfSlider2_class.move_thumb(self, oldx, oldy, wd, ht, newx, newy);
	}
    }

    if (inf->reason != XfwfSNotify) {
	new_info = *inf;
	new_info.reason = XfwfSNotify;
	XtCallCallbackList(self, ((XfwfSlider2Widget)self)->xfwfSlider2.scrollCallback, &new_info);
    }
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
void XfwfGetThumb(Widget self,XfwfScrollInfo * info)
#else
void XfwfGetThumb(self,info)Widget self;XfwfScrollInfo * info;
#endif
{
    if (! XtIsSubclass(self, xfwfSlider2WidgetClass))
	XtError("XfwfGetThumb called with incorrect widget type");
    info->reason = XfwfSNotify;
    info->flags = XFWF_VPOS | XFWF_VSIZE | XFWF_HPOS | XFWF_HSIZE;
    info->vpos = ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_y;
    info->vsize = ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_ht;
    info->hpos = ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_x;
    info->hsize = ((XfwfSlider2Widget)self)->xfwfSlider2.thumb_wd;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
void XfwfMoveThumb(Widget self,double  x,double  y)
#else
void XfwfMoveThumb(self,x,y)Widget self;double  x;double  y;
#endif
{
    XfwfScrollInfo info;

    if (! XtIsSubclass(self, xfwfSlider2WidgetClass))
	XtError("XfwfMoveThumb called with incorrect widget type");
    if (x < 0.0 || x > 1.0 || y < 0.0 || y > 1.0)
	XtError("XfwfMoveThumb called with incorrect arguments");
    info.flags = XFWF_VPOS | XFWF_HPOS;
    info.reason = XfwfSNotify;
    info.vpos = y;
    info.hpos = x;
    ((XfwfSlider2WidgetClass)self->core.widget_class)->xfwfSlider2_class.scroll_response(NULL, self, &info);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
void XfwfResizeThumb(Widget self,double  wd,double  ht)
#else
void XfwfResizeThumb(self,wd,ht)Widget self;double  wd;double  ht;
#endif
{
    XfwfScrollInfo info;

    if (! XtIsSubclass(self, xfwfSlider2WidgetClass))
	XtError("XfwfResizeThumb called with incorrect widget type");
    if (wd < 0.0 || wd > 1.0 || ht < 0.0 || ht > 1.0)
	XtError("XfwfResizeThumb called with incorrect arguments");

    info.reason = XfwfSNotify;
    info.flags = XFWF_VSIZE | XFWF_HSIZE;
    info.vsize = ht;
    info.hsize = wd;
    ((XfwfSlider2WidgetClass)self->core.widget_class)->xfwfSlider2_class.scroll_response(NULL, self, &info);
}
