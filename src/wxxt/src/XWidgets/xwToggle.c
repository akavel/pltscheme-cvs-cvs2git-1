/* Generated by wbuild
 * (generator version 3.2)
 */
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <xwTools3d.h>
#include <./xwToggleP.h>
static void toggle(
#if NeedFunctionPrototypes
Widget,XEvent*,String*,Cardinal*
#endif
);
static void switch_on(
#if NeedFunctionPrototypes
Widget,XEvent*,String*,Cardinal*
#endif
);
static void switch_off(
#if NeedFunctionPrototypes
Widget,XEvent*,String*,Cardinal*
#endif
);

static XtActionsRec actionsList[] = {
{"toggle", toggle},
{"switch_on", switch_on},
{"switch_off", switch_off},
};

static char defaultTranslations[] = "\
<Btn1Up>: toggle() \n\
<Key>Return: toggle() \n\
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
static void realize(
#if NeedFunctionPrototypes
Widget,XtValueMask *,XSetWindowAttributes *
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
static void create_indicator_gc(
#if NeedFunctionPrototypes
Widget
#endif
);
static void compute_indicatorcolor(
#if NeedFunctionPrototypes
Widget,int ,XrmValue *
#endif
);
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void create_indicator_gc(Widget self)
#else
static void create_indicator_gc(self)Widget self;
#endif
{
    XtGCMask mask = 0;
    XGCValues values;

    if (((XfwfToggleWidget)self)->xfwfToggle.indicator_gc != NULL) XtReleaseGC(self, ((XfwfToggleWidget)self)->xfwfToggle.indicator_gc);
    switch (((XfwfToggleWidget)self)->xfwfFrame.shadowScheme) {
    case XfwfColor:
        mask = GCForeground;
        values.foreground = ((XfwfToggleWidget)self)->xfwfFrame.topShadowColor;
        break;
    case XfwfStipple:
        mask = GCFillStyle | GCStipple | GCForeground | GCBackground;
        values.fill_style = FillOpaqueStippled;
        values.background = BlackPixelOfScreen(XtScreen(self));
        values.foreground = WhitePixelOfScreen(XtScreen(self));
        values.stipple    = GetGray(self);
        break;
    case XfwfBlack:
    case XfwfAuto:
        if (DefaultDepthOfScreen(XtScreen(self)) > 4
            && ((XfwfToggleWidgetClass)self->core.widget_class)->xfwfCommon_class.set_color(self, ((XfwfToggleWidget)self)->core.background_pixel, &values.foreground)) {
            mask = GCForeground;
        } else {
            mask = GCFillStyle | GCBackground | GCForeground | GCStipple;
            values.fill_style = FillOpaqueStippled;
            values.background = BlackPixelOfScreen(XtScreen(self));
            values.foreground = WhitePixelOfScreen(XtScreen(self));
            values.stipple    = GetGray(self);
        }
        break;
    }
    ((XfwfToggleWidget)self)->xfwfToggle.indicator_gc = XtGetGC(self, mask, &values);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void compute_indicatorcolor(Widget self,int  offset,XrmValue * value)
#else
static void compute_indicatorcolor(self,offset,value)Widget self;int  offset;XrmValue * value;
#endif
{
    static Pixel color;
#if 1
    ((XfwfToggleWidgetClass)self->core.widget_class)->xfwfCommon_class.set_color(self, ((XfwfToggleWidget)self)->core.background_pixel, &color);
#else
    (void) choose_color(self, 0.85, ((XfwfToggleWidget)self)->core.background_pixel, &color);
#endif
    value->addr = (XtPointer) &color;
}

static XtResource resources[] = {
{XtNonCallback,XtCOnCallback,XtRCallback,sizeof(((XfwfToggleRec*)NULL)->xfwfToggle.onCallback),XtOffsetOf(XfwfToggleRec,xfwfToggle.onCallback),XtRImmediate,(XtPointer)NULL },
{XtNoffCallback,XtCOffCallback,XtRCallback,sizeof(((XfwfToggleRec*)NULL)->xfwfToggle.offCallback),XtOffsetOf(XfwfToggleRec,xfwfToggle.offCallback),XtRImmediate,(XtPointer)NULL },
{XtNon,XtCOn,XtRBoolean,sizeof(((XfwfToggleRec*)NULL)->xfwfToggle.on),XtOffsetOf(XfwfToggleRec,xfwfToggle.on),XtRImmediate,(XtPointer)False },
{XtNindicatorSize,XtCIndicatorSize,XtRDimension,sizeof(((XfwfToggleRec*)NULL)->xfwfToggle.indicatorSize),XtOffsetOf(XfwfToggleRec,xfwfToggle.indicatorSize),XtRImmediate,(XtPointer)0 },
{XtNindicatorType,XtCIndicatorType,XtRIndicatorType,sizeof(((XfwfToggleRec*)NULL)->xfwfToggle.indicatorType),XtOffsetOf(XfwfToggleRec,xfwfToggle.indicatorType),XtRImmediate,(XtPointer)XfwfSquareIndicator },
{XtNindicatorColor,XtCIndicatorColor,XtRPixel,sizeof(((XfwfToggleRec*)NULL)->xfwfToggle.indicatorColor),XtOffsetOf(XfwfToggleRec,xfwfToggle.indicatorColor),XtRCallProc,(XtPointer)compute_indicatorcolor },
{XtNframeWidth,XtCFrameWidth,XtRDimension,sizeof(((XfwfToggleRec*)NULL)->xfwfFrame.frameWidth),XtOffsetOf(XfwfToggleRec,xfwfFrame.frameWidth),XtRImmediate,(XtPointer)0 },
};

XfwfToggleClassRec xfwfToggleClassRec = {
{ /* core_class part */
/* superclass   	*/  (WidgetClass) &xfwfButtonClassRec,
/* class_name   	*/  "XfwfToggle",
/* widget_size  	*/  sizeof(XfwfToggleRec),
/* class_initialize 	*/  NULL,
/* class_part_initialize*/  _resolve_inheritance,
/* class_inited 	*/  FALSE,
/* initialize   	*/  initialize,
/* initialize_hook 	*/  NULL,
/* realize      	*/  realize,
/* actions      	*/  actionsList,
/* num_actions  	*/  3,
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
{ /* XfwfLabel_class part */
XtInherit_set_label,
},
{ /* XfwfButton_class part */
 /* dummy */  0
},
{ /* XfwfToggle_class part */
 /* dummy */  0
},
};
WidgetClass xfwfToggleWidgetClass = (WidgetClass) &xfwfToggleClassRec;
/*ARGSUSED*/
static void toggle(self,event,params,num_params)Widget self;XEvent*event;String*params;Cardinal*num_params;
{
    XtVaSetValues(self, "on", !((XfwfToggleWidget)self)->xfwfToggle.on, NULL);
    XtCallCallbackList(self, ((XfwfToggleWidget)self)->xfwfToggle.on ? ((XfwfToggleWidget)self)->xfwfToggle.onCallback : ((XfwfToggleWidget)self)->xfwfToggle.offCallback, event);
}

/*ARGSUSED*/
static void switch_on(self,event,params,num_params)Widget self;XEvent*event;String*params;Cardinal*num_params;
{
    if (! ((XfwfToggleWidget)self)->xfwfToggle.on) {
        XtVaSetValues(self, "on", True, NULL);
        XtCallCallbackList(self, ((XfwfToggleWidget)self)->xfwfToggle.onCallback, event);
    }
}

/*ARGSUSED*/
static void switch_off(self,event,params,num_params)Widget self;XEvent*event;String*params;Cardinal*num_params;
{
    if (((XfwfToggleWidget)self)->xfwfToggle.on) {
        XtVaSetValues(self, "on", False, NULL);
        XtCallCallbackList(self, ((XfwfToggleWidget)self)->xfwfToggle.offCallback, event);
    }
}

static void _resolve_inheritance(class)
WidgetClass class;
{
  XfwfToggleWidgetClass c = (XfwfToggleWidgetClass) class;
  XfwfToggleWidgetClass super;
  static CompositeClassExtensionRec extension_rec = {
    NULL, NULLQUARK, XtCompositeExtensionVersion,
    sizeof(CompositeClassExtensionRec), True};
  CompositeClassExtensionRec *ext;
  ext = (XtPointer)XtMalloc(sizeof(*ext));
  *ext = extension_rec;
  ext->next_extension = c->composite_class.extension;
  c->composite_class.extension = ext;
  if (class == xfwfToggleWidgetClass) return;
  super = (XfwfToggleWidgetClass)class->core_class.superclass;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void initialize(Widget  request,Widget self,ArgList  args,Cardinal * num_args)
#else
static void initialize(request,self,args,num_args)Widget  request;Widget self;ArgList  args;Cardinal * num_args;
#endif
{
    ((XfwfToggleWidget)self)->xfwfToggle.saveLeftMargin = ((XfwfToggleWidget)self)->xfwfLabel.leftMargin;

    if (!((XfwfToggleWidget)self)->xfwfToggle.indicatorSize || ((XfwfToggleWidget)self)->xfwfToggle.indicatorSize > ((XfwfToggleWidget)self)->xfwfLabel.font->ascent + 2)
	((XfwfToggleWidget)self)->xfwfToggle.indicatorSize = ((XfwfToggleWidget)self)->xfwfLabel.font->ascent + 2;

    ((XfwfToggleWidget)self)->xfwfToggle.indicator_gc = NULL;

    XtVaSetValues(self, XtNleftMargin, 2 * ((XfwfToggleWidget)self)->xfwfLabel.leftMargin + ((XfwfToggleWidget)self)->xfwfToggle.indicatorSize, NULL);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void destroy(Widget self)
#else
static void destroy(self)Widget self;
#endif
{
   if (((XfwfToggleWidget)self)->xfwfToggle.indicator_gc) XtReleaseGC(self, ((XfwfToggleWidget)self)->xfwfToggle.indicator_gc); ((XfwfToggleWidget)self)->xfwfToggle.indicator_gc = NULL;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void realize(Widget self,XtValueMask * mask,XSetWindowAttributes * attributes)
#else
static void realize(self,mask,attributes)Widget self;XtValueMask * mask;XSetWindowAttributes * attributes;
#endif
{
    xfwfButtonClassRec.core_class.realize(self, mask, attributes);
    create_indicator_gc(self);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static Boolean  set_values(Widget  old,Widget  request,Widget self,ArgList  args,Cardinal * num_args)
#else
static Boolean  set_values(old,request,self,args,num_args)Widget  old;Widget  request;Widget self;ArgList  args;Cardinal * num_args;
#endif
{
    Boolean redraw = False;

    if (!XtIsRealized(self))
	return False;

    if (((XfwfToggleWidget)self)->xfwfToggle.on != ((XfwfToggleWidget)old)->xfwfToggle.on
    ||  ((XfwfToggleWidget)self)->xfwfToggle.indicatorType != ((XfwfToggleWidget)old)->xfwfToggle.indicatorType) {
        redraw = True;
    }
    if (((XfwfToggleWidget)self)->xfwfFrame.shadowScheme != ((XfwfToggleWidget)old)->xfwfFrame.shadowScheme) {
        create_indicator_gc(self);
        redraw = True;
    } else if (((XfwfToggleWidget)self)->xfwfFrame.shadowScheme == XfwfColor
           &&  ((XfwfToggleWidget)self)->xfwfToggle.indicatorColor != ((XfwfToggleWidget)old)->xfwfToggle.indicatorColor) {
	create_indicator_gc(self);
        redraw = True;
    }
    if (((XfwfToggleWidget)self)->xfwfToggle.indicatorSize != ((XfwfToggleWidget)old)->xfwfToggle.indicatorSize) {
	/* Compute new left margin */
	Dimension w = ((XfwfToggleWidget)self)->xfwfLabel.leftMargin + 2 * ((XfwfToggleWidget)self)->xfwfToggle.saveLeftMargin;
	/* change left margin */
	XtVaSetValues(self, XtNleftMargin, w, NULL);
	redraw = True;
    }
    return redraw;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void _expose(Widget self,XEvent * event,Region  region)
#else
static void _expose(self,event,region)Widget self;XEvent * event;Region  region;
#endif
{
    Position x, y;
    int w, h;

    if (! XtIsRealized(self)) return;
    xfwfButtonClassRec.xfwfCommon_class._expose(self, event, region);
    ((XfwfToggleWidgetClass)self->core.widget_class)->xfwfCommon_class.compute_inside(self, &x, &y, &w, &h);
    x += ((XfwfToggleWidget)self)->xfwfToggle.saveLeftMargin;
    y += (h - ((XfwfToggleWidget)self)->xfwfToggle.indicatorSize) / 2;
    switch (((XfwfToggleWidget)self)->xfwfToggle.indicatorType) {
    default:
	XtWarning("XfwfToggle has wrong indicatorType, using square!");
    case XfwfSquareIndicator:
	Xaw3dDrawToggle(XtDisplay(self), XtWindow(self),
	   	        ((XfwfToggleWidget)self)->xfwfFrame.lightgc, ((XfwfToggleWidget)self)->xfwfFrame.darkgc, ((XfwfToggleWidget)self)->xfwfToggle.indicator_gc, NULL, ((XfwfToggleWidget)self)->xfwfLabel.gc,
		        x, y, ((XfwfToggleWidget)self)->xfwfToggle.indicatorSize, 2, ((XfwfToggleWidget)self)->xfwfToggle.on);
	break;
    case XfwfDiamondIndicator:
	Xaw3dDrawRadio(XtDisplay(self), XtWindow(self),
	   	       ((XfwfToggleWidget)self)->xfwfFrame.lightgc, ((XfwfToggleWidget)self)->xfwfFrame.darkgc, ((XfwfToggleWidget)self)->xfwfToggle.indicator_gc, NULL, ((XfwfToggleWidget)self)->xfwfLabel.gc,
		       x, y, ((XfwfToggleWidget)self)->xfwfToggle.indicatorSize, 2, ((XfwfToggleWidget)self)->xfwfToggle.on);
	break;
    }
}
