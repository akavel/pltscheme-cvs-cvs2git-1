/* Generated by wbuild
 * (generator version 3.2)
 */
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/bitmaps/gray>
#include <stdio.h>
#include <assert.h>
#include <wxtimeout.h>
#include "xwTools3d.h"
#include <./xwArrowP.h>
static void activate_and_start_timer(
#if NeedFunctionPrototypes
Widget,XEvent*,String*,Cardinal*
#endif
);
static void stop_timer(
#if NeedFunctionPrototypes
Widget,XEvent*,String*,Cardinal*
#endif
);
static void push_up(
#if NeedFunctionPrototypes
Widget,XEvent*,String*,Cardinal*
#endif
);
static void push_down(
#if NeedFunctionPrototypes
Widget,XEvent*,String*,Cardinal*
#endif
);

static XtActionsRec actionsList[] = {
{"activate_and_start_timer", activate_and_start_timer},
{"stop_timer", stop_timer},
{"push_up", push_up},
{"push_down", push_down},
};

static char defaultTranslations[] = "\
<Btn1Down>: push_down() activate_and_start_timer() \n\
<Btn1Up>: push_up() stop_timer() \n\
";
static void _resolve_inheritance(
#if NeedFunctionPrototypes
WidgetClass
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
static Boolean  set_values(
#if NeedFunctionPrototypes
Widget ,Widget ,Widget,ArgList ,Cardinal *
#endif
);
static void _expose(
#if NeedFunctionPrototypes
Widget,XEvent *,Region 
#endif
);
static void  draw_arrow(
#if NeedFunctionPrototypes
Widget,int 
#endif
);
static void timer_callback(
#if NeedFunctionPrototypes
XtPointer ,XtIntervalId *
#endif
);
static void create_arrowgc(
#if NeedFunctionPrototypes
Widget
#endif
);
static void create_arrowlightgc(
#if NeedFunctionPrototypes
Widget
#endif
);
static void create_arrowdarkgc(
#if NeedFunctionPrototypes
Widget
#endif
);
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void timer_callback(XtPointer  client_data,XtIntervalId * timer)
#else
static void timer_callback(client_data,timer)XtPointer  client_data;XtIntervalId * timer;
#endif
{
    Widget self = (Widget) client_data;

    XtCallCallbackList(self, ((XfwfArrowWidget)self)->xfwfArrow.callback, NULL);
    if (((XfwfArrowWidget)self)->xfwfArrow.timer) { /* i.e., no stop issued by callback */
      if (((XfwfArrowWidget)self)->xfwfArrow.timer != 0x1)
        wxRemoveTimeOut(((XfwfArrowWidget)self)->xfwfArrow.timer);
      ((XfwfArrowWidget)self)->xfwfArrow.timer = wxAppAddTimeOut(XtWidgetToApplicationContext(self),
				     ((XfwfArrowWidget)self)->xfwfArrow.repeatDelay, timer_callback, self, self);
    }
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void create_arrowgc(Widget self)
#else
static void create_arrowgc(self)Widget self;
#endif
{
    XtGCMask mask;
    XGCValues values;

    if (((XfwfArrowWidget)self)->xfwfArrow.arrowgc != NULL) XtReleaseGC(self, ((XfwfArrowWidget)self)->xfwfArrow.arrowgc);
    mask = GCForeground;
    values.foreground = ((XfwfArrowWidget)self)->xfwfArrow.foreground;
    ((XfwfArrowWidget)self)->xfwfArrow.arrowgc = XtGetGC(self, mask, &values);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void create_arrowlightgc(Widget self)
#else
static void create_arrowlightgc(self)Widget self;
#endif
{
    XtGCMask mask=0;
    XGCValues values;

    if (((XfwfArrowWidget)self)->xfwfArrow.arrowlightgc != NULL) XtReleaseGC(self, ((XfwfArrowWidget)self)->xfwfArrow.arrowlightgc);
    switch (((XfwfArrowWidget)self)->xfwfFrame.shadowScheme) {
    case XfwfColor:
	mask = GCForeground;
	values.foreground = ((XfwfArrowWidget)self)->xfwfFrame.topShadowColor;
	break;
    case XfwfStipple:
	mask = GCFillStyle | GCStipple | GCForeground | GCBackground;
	values.fill_style = FillOpaqueStippled;
	values.background = ((XfwfArrowWidget)self)->core.background_pixel;
	values.stipple = ((XfwfArrowWidget)self)->xfwfFrame.topShadowStipple;
	values.foreground = WhitePixelOfScreen(XtScreen(self));
	break;
    case XfwfBlack:
    case XfwfAuto:
	if (DefaultDepthOfScreen(XtScreen(self)) > 4
	    && ((XfwfArrowWidgetClass)self->core.widget_class)->xfwfCommon_class.lighter_color(self, ((XfwfArrowWidget)self)->core.background_pixel, &values.foreground)) {
	    mask = GCForeground;
	} else {
	    mask = GCFillStyle | GCBackground | GCForeground | GCStipple;
	    values.fill_style = FillOpaqueStippled;
	    /* values.background = $foreground; */
	    values.background = BlackPixelOfScreen(XtScreen(self));
	    values.foreground = WhitePixelOfScreen(XtScreen(self));
	    values.stipple =
		XCreateBitmapFromData(XtDisplay(self),
				      RootWindowOfScreen(XtScreen(self)),
				      gray_bits, gray_width, gray_height);
	}
	break;
    }
    ((XfwfArrowWidget)self)->xfwfArrow.arrowlightgc = XtGetGC(self, mask, &values);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void create_arrowdarkgc(Widget self)
#else
static void create_arrowdarkgc(self)Widget self;
#endif
{
    XtGCMask mask=0;
    XGCValues values;

    if (((XfwfArrowWidget)self)->xfwfArrow.arrowdarkgc != NULL) XtReleaseGC(self, ((XfwfArrowWidget)self)->xfwfArrow.arrowdarkgc);
    switch (((XfwfArrowWidget)self)->xfwfFrame.shadowScheme) {
    case XfwfColor:
	mask = GCForeground;
	values.foreground = ((XfwfArrowWidget)self)->xfwfFrame.bottomShadowColor;
	break;
    case XfwfStipple:
	mask = GCFillStyle | GCStipple | GCForeground | GCBackground;
	values.fill_style = FillOpaqueStippled;
	values.stipple = ((XfwfArrowWidget)self)->xfwfFrame.bottomShadowStipple;
	values.foreground = BlackPixelOfScreen(XtScreen(self));
	values.background = ((XfwfArrowWidget)self)->core.background_pixel;
	break;
    case XfwfBlack:
    case XfwfAuto:
	if (DefaultDepthOfScreen(XtScreen(self)) > 4
	    && ((XfwfArrowWidgetClass)self->core.widget_class)->xfwfCommon_class.darker_color(self, ((XfwfArrowWidget)self)->core.background_pixel, &values.foreground)) {
	    mask = GCForeground;
	} else {
	    mask = GCFillStyle | GCBackground | GCForeground | GCStipple;
	    values.fill_style = FillOpaqueStippled;
	    /* values.background = $foreground; */
	    values.background = BlackPixelOfScreen(XtScreen(self));
	    values.foreground = WhitePixelOfScreen(XtScreen(self));
	    values.stipple =
		XCreateBitmapFromData(XtDisplay(self),
				      RootWindowOfScreen(XtScreen(self)),
				      gray_bits, gray_width, gray_height);
	}
	break;
    }
    ((XfwfArrowWidget)self)->xfwfArrow.arrowdarkgc = XtGetGC(self, mask, &values);
}

static XtResource resources[] = {
{XtNdirection,XtCDirection,XtRAlignment,sizeof(((XfwfArrowRec*)NULL)->xfwfArrow.direction),XtOffsetOf(XfwfArrowRec,xfwfArrow.direction),XtRImmediate,(XtPointer)XfwfTop },
{XtNforeground,XtCForeground,XtRPixel,sizeof(((XfwfArrowRec*)NULL)->xfwfArrow.foreground),XtOffsetOf(XfwfArrowRec,xfwfArrow.foreground),XtRString,(XtPointer)XtDefaultBackground },
{XtNarrowShadow,XtCArrowShadow,XtRDimension,sizeof(((XfwfArrowRec*)NULL)->xfwfArrow.arrowShadow),XtOffsetOf(XfwfArrowRec,xfwfArrow.arrowShadow),XtRImmediate,(XtPointer)2 },
{XtNrepeat,XtCRepeat,XtRBoolean,sizeof(((XfwfArrowRec*)NULL)->xfwfArrow.repeat),XtOffsetOf(XfwfArrowRec,xfwfArrow.repeat),XtRImmediate,(XtPointer)TRUE },
{XtNinitialDelay,XtCInitialDelay,XtRCardinal,sizeof(((XfwfArrowRec*)NULL)->xfwfArrow.initialDelay),XtOffsetOf(XfwfArrowRec,xfwfArrow.initialDelay),XtRImmediate,(XtPointer)500 },
{XtNrepeatDelay,XtCRepeatDelay,XtRCardinal,sizeof(((XfwfArrowRec*)NULL)->xfwfArrow.repeatDelay),XtOffsetOf(XfwfArrowRec,xfwfArrow.repeatDelay),XtRImmediate,(XtPointer)200 },
{XtNcallback,XtCCallback,XtRCallback,sizeof(((XfwfArrowRec*)NULL)->xfwfArrow.callback),XtOffsetOf(XfwfArrowRec,xfwfArrow.callback),XtRImmediate,(XtPointer)NULL },
};

XfwfArrowClassRec xfwfArrowClassRec = {
{ /* core_class part */
/* superclass   	*/  (WidgetClass) &xfwfBoardClassRec,
/* class_name   	*/  "XfwfArrow",
/* widget_size  	*/  sizeof(XfwfArrowRec),
/* class_initialize 	*/  NULL,
/* class_part_initialize*/  _resolve_inheritance,
/* class_inited 	*/  FALSE,
/* initialize   	*/  initialize,
/* initialize_hook 	*/  NULL,
/* realize      	*/  XtInheritRealize,
/* actions      	*/  actionsList,
/* num_actions  	*/  4,
/* resources    	*/  resources,
/* num_resources 	*/  7,
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
{ /* XfwfArrow_class part */
draw_arrow,
},
};
WidgetClass xfwfArrowWidgetClass = (WidgetClass) &xfwfArrowClassRec;
/*ARGSUSED*/
static void activate_and_start_timer(self,event,params,num_params)Widget self;XEvent*event;String*params;Cardinal*num_params;
{
    if (event->type != ButtonPress) {
        XtWarning("The Arrow activate action isn't bound to a BtnDown event");
	return;
    }
    stop_timer(self, event, params, num_params);
    ((XfwfArrowWidget)self)->xfwfArrow.timer = 0x1; /* During callback, a stop_timer may be evaluated */
    XtCallCallbackList(self, ((XfwfArrowWidget)self)->xfwfArrow.callback, NULL);
    if (((XfwfArrowWidget)self)->xfwfArrow.repeat) {
      if (((XfwfArrowWidget)self)->xfwfArrow.timer) {
        stop_timer(self, event, params, num_params);
	((XfwfArrowWidget)self)->xfwfArrow.timer = wxAppAddTimeOut(XtWidgetToApplicationContext(self),
				 ((XfwfArrowWidget)self)->xfwfArrow.initialDelay, timer_callback, self, self);
      }
    } else
	push_up(self, event, params, num_params);
}

/*ARGSUSED*/
static void stop_timer(self,event,params,num_params)Widget self;XEvent*event;String*params;Cardinal*num_params;
{
    if (((XfwfArrowWidget)self)->xfwfArrow.timer && ((XfwfArrowWidget)self)->xfwfArrow.timer != 0x1)
	wxRemoveTimeOut(((XfwfArrowWidget)self)->xfwfArrow.timer);
    ((XfwfArrowWidget)self)->xfwfArrow.timer = 0;
}

/*ARGSUSED*/
static void push_up(self,event,params,num_params)Widget self;XEvent*event;String*params;Cardinal*num_params;
{
  draw_arrow(self, 0);
}

/*ARGSUSED*/
static void push_down(self,event,params,num_params)Widget self;XEvent*event;String*params;Cardinal*num_params;
{
  draw_arrow(self, 1);
}

static void _resolve_inheritance(class)
WidgetClass class;
{
  XfwfArrowWidgetClass c = (XfwfArrowWidgetClass) class;
  XfwfArrowWidgetClass super;
  static CompositeClassExtensionRec extension_rec = {
    NULL, NULLQUARK, XtCompositeExtensionVersion,
    sizeof(CompositeClassExtensionRec), True};
  CompositeClassExtensionRec *ext;
  ext = (XtPointer)XtMalloc(sizeof(*ext));
  *ext = extension_rec;
  ext->next_extension = c->composite_class.extension;
  c->composite_class.extension = ext;
  if (class == xfwfArrowWidgetClass) return;
  super = (XfwfArrowWidgetClass)class->core_class.superclass;
  if (c->xfwfArrow_class.draw_arrow == XtInherit_draw_arrow)
    c->xfwfArrow_class.draw_arrow = super->xfwfArrow_class.draw_arrow;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void initialize(Widget  request,Widget self,ArgList  args,Cardinal * num_args)
#else
static void initialize(request,self,args,num_args)Widget  request;Widget self;ArgList  args;Cardinal * num_args;
#endif
{
    if (((XfwfArrowWidget)self)->xfwfArrow.direction != XfwfTop && ((XfwfArrowWidget)self)->xfwfArrow.direction != XfwfLeft
	&& ((XfwfArrowWidget)self)->xfwfArrow.direction != XfwfRight && ((XfwfArrowWidget)self)->xfwfArrow.direction != XfwfBottom) {
	XtWarning("direction of Arrow widget incorrect; set to `top'");
	((XfwfArrowWidget)self)->xfwfArrow.direction = XfwfTop;
    }
    ((XfwfArrowWidget)self)->xfwfArrow.arrowgc = NULL; create_arrowgc(self);
    ((XfwfArrowWidget)self)->xfwfArrow.arrowlightgc = NULL; create_arrowlightgc(self);
    ((XfwfArrowWidget)self)->xfwfArrow.arrowdarkgc = NULL; create_arrowdarkgc(self);
    ((XfwfArrowWidget)self)->xfwfArrow.timer = 0;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void destroy(Widget self)
#else
static void destroy(self)Widget self;
#endif
{
   if (((XfwfArrowWidget)self)->xfwfArrow.timer) {
      if (((XfwfArrowWidget)self)->xfwfArrow.timer != 0x1)
        wxRemoveTimeOut(((XfwfArrowWidget)self)->xfwfArrow.timer);
      ((XfwfArrowWidget)self)->xfwfArrow.timer = 0;
   }
   if (((XfwfArrowWidget)self)->xfwfArrow.arrowgc) XtReleaseGC(self, ((XfwfArrowWidget)self)->xfwfArrow.arrowgc); ((XfwfArrowWidget)self)->xfwfArrow.arrowgc = NULL;
   if (((XfwfArrowWidget)self)->xfwfArrow.arrowlightgc) XtReleaseGC(self, ((XfwfArrowWidget)self)->xfwfArrow.arrowlightgc); ((XfwfArrowWidget)self)->xfwfArrow.arrowlightgc = NULL;
   if (((XfwfArrowWidget)self)->xfwfArrow.arrowdarkgc) XtReleaseGC(self, ((XfwfArrowWidget)self)->xfwfArrow.arrowdarkgc); ((XfwfArrowWidget)self)->xfwfArrow.arrowdarkgc = NULL;   
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static Boolean  set_values(Widget  old,Widget  request,Widget self,ArgList  args,Cardinal * num_args)
#else
static Boolean  set_values(old,request,self,args,num_args)Widget  old;Widget  request;Widget self;ArgList  args;Cardinal * num_args;
#endif
{
    Boolean need_redisplay = False;

    if (((XfwfArrowWidget)self)->xfwfArrow.direction != XfwfTop && ((XfwfArrowWidget)self)->xfwfArrow.direction != XfwfLeft
	&& ((XfwfArrowWidget)self)->xfwfArrow.direction != XfwfRight && ((XfwfArrowWidget)self)->xfwfArrow.direction != XfwfBottom) {
	XtWarning("direction of Arrow widget incorrect; set to `top'");
	((XfwfArrowWidget)self)->xfwfArrow.direction = XfwfTop;
    }
    if (((XfwfArrowWidget)old)->xfwfArrow.direction != ((XfwfArrowWidget)self)->xfwfArrow.direction)
	need_redisplay = True;
    if (((XfwfArrowWidget)old)->xfwfArrow.foreground != ((XfwfArrowWidget)self)->xfwfArrow.foreground) {
	create_arrowgc(self);
	need_redisplay = True;
    }
    if (((XfwfArrowWidget)old)->xfwfArrow.arrowShadow != ((XfwfArrowWidget)self)->xfwfArrow.arrowShadow)
	need_redisplay = True;
    if (((XfwfArrowWidget)self)->xfwfFrame.shadowScheme != ((XfwfArrowWidget)old)->xfwfFrame.shadowScheme) {
	create_arrowdarkgc(self);
	create_arrowlightgc(self);
	need_redisplay = True;
    } else if (((XfwfArrowWidget)self)->xfwfFrame.shadowScheme == XfwfColor) {
	if (((XfwfArrowWidget)self)->xfwfFrame.topShadowColor != ((XfwfArrowWidget)old)->xfwfFrame.topShadowColor) {
	    create_arrowlightgc(self);
	    need_redisplay = True;
	}
	if (((XfwfArrowWidget)self)->xfwfFrame.bottomShadowColor != ((XfwfArrowWidget)old)->xfwfFrame.bottomShadowColor) {
	    create_arrowdarkgc(self);
	    need_redisplay = True;
	}
    } else if (((XfwfArrowWidget)self)->xfwfFrame.shadowScheme == XfwfStipple) {
	if (((XfwfArrowWidget)self)->xfwfFrame.topShadowStipple != ((XfwfArrowWidget)old)->xfwfFrame.topShadowStipple) {
	    create_arrowlightgc(self);
	    need_redisplay = True;
	}
	if (((XfwfArrowWidget)self)->xfwfFrame.bottomShadowStipple != ((XfwfArrowWidget)old)->xfwfFrame.bottomShadowStipple) {
	    create_arrowdarkgc(self);
	    need_redisplay = True;
	}
    }
    return need_redisplay;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void _expose(Widget self,XEvent * event,Region  region)
#else
static void _expose(self,event,region)Widget self;XEvent * event;Region  region;
#endif
{
    assert(((XfwfArrowWidget)self)->xfwfArrow.direction == XfwfTop || ((XfwfArrowWidget)self)->xfwfArrow.direction == XfwfLeft
	   || ((XfwfArrowWidget)self)->xfwfArrow.direction == XfwfRight || ((XfwfArrowWidget)self)->xfwfArrow.direction == XfwfBottom);

    if (! XtIsRealized(self)) return;

    xfwfBoardClassRec.xfwfCommon_class._expose(self, event, region);

    if (region != NULL) {
	XSetRegion(XtDisplay(self), ((XfwfArrowWidget)self)->xfwfArrow.arrowgc, region);
	XSetRegion(XtDisplay(self), ((XfwfArrowWidget)self)->xfwfArrow.arrowlightgc, region);
	XSetRegion(XtDisplay(self), ((XfwfArrowWidget)self)->xfwfArrow.arrowdarkgc, region);
    }
	
    draw_arrow(self, 0);

    if (region != NULL) {
	XSetClipMask(XtDisplay(self), ((XfwfArrowWidget)self)->xfwfArrow.arrowgc, None);
	XSetClipMask(XtDisplay(self), ((XfwfArrowWidget)self)->xfwfArrow.arrowlightgc, None);
	XSetClipMask(XtDisplay(self), ((XfwfArrowWidget)self)->xfwfArrow.arrowdarkgc, None);
    }
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void  draw_arrow(Widget self,int  on)
#else
static void  draw_arrow(self,on)Widget self;int  on;
#endif
{
    Position x, y;
    int  width, height, dir;

    ((XfwfArrowWidgetClass)self->core.widget_class)->xfwfCommon_class.compute_inside(self, &x, &y, &width, &height);

    if (((XfwfArrowWidget)self)->xfwfArrow.arrowShadow) {
      XDrawLine(XtDisplay(self), XtWindow(self),
		(on ? ((XfwfArrowWidget)self)->xfwfArrow.arrowdarkgc : ((XfwfArrowWidget)self)->xfwfArrow.arrowlightgc),
		x, y, x + width, y);
      XDrawLine(XtDisplay(self), XtWindow(self),
		(on ? ((XfwfArrowWidget)self)->xfwfArrow.arrowdarkgc : ((XfwfArrowWidget)self)->xfwfArrow.arrowlightgc),
		x, y, x, y + height);
      XDrawLine(XtDisplay(self), XtWindow(self),
		(on ? ((XfwfArrowWidget)self)->xfwfArrow.arrowlightgc : ((XfwfArrowWidget)self)->xfwfArrow.arrowdarkgc),
		x + width - 1, y, x + width - 1, y + height);
      XDrawLine(XtDisplay(self), XtWindow(self),
		(on ? ((XfwfArrowWidget)self)->xfwfArrow.arrowlightgc : ((XfwfArrowWidget)self)->xfwfArrow.arrowdarkgc),
		x, y + height - 1, x + width, y + height - 1);
      width--;
      height--;
    }

    switch (((XfwfArrowWidget)self)->xfwfArrow.direction) {
    case XfwfTop:
      dir = UP;
      --y;
      break;
    case XfwfBottom:
      dir = DOWN;
      y++;
      break;
    case XfwfLeft:
      x++;
      dir = LEFT;
      break;
    default:
    case XfwfRight:
      --x;
      dir = RIGHT;
      break;
    }

    x += ((XfwfArrowWidget)self)->xfwfArrow.arrowShadow;
    y += ((XfwfArrowWidget)self)->xfwfArrow.arrowShadow;
    width -= 2 * ((XfwfArrowWidget)self)->xfwfArrow.arrowShadow;
    height -= 2 * ((XfwfArrowWidget)self)->xfwfArrow.arrowShadow;
    width = max(1, width);
    height = max(1, height);

    Xaw3dDrawArrow(XtDisplay(self), XtWindow(self),
		   ((XfwfArrowWidget)self)->xfwfArrow.arrowlightgc, ((XfwfArrowWidget)self)->xfwfArrow.arrowdarkgc,
		   ((XfwfArrowWidget)self)->xfwfArrow.arrowgc, ((XfwfArrowWidget)self)->xfwfArrow.arrowgc,
		   x, y, width, height,
		   0, dir, on);
}
