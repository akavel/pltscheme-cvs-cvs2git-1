/* Generated by wbuild
 * (generator version 3.2)
 */
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <./xwCanvasP.h>
static void _resolve_inheritance(
#if NeedFunctionPrototypes
WidgetClass
#endif
);
#line 22 "XWidgets/xwCanvas.w"
static void realize(
#if NeedFunctionPrototypes
Widget,XtValueMask *,XSetWindowAttributes *
#endif
);
#line 38 "XWidgets/xwCanvas.w"
static Boolean  set_values(
#if NeedFunctionPrototypes
Widget ,Widget ,Widget,ArgList ,Cardinal *
#endif
);

static XtResource resources[] = {
#line 16 "XWidgets/xwCanvas.w"
{XtNbackingStore,XtCBackingStore,XtRInt,sizeof(((XfwfCanvasRec*)NULL)->xfwfCanvas.backingStore),XtOffsetOf(XfwfCanvasRec,xfwfCanvas.backingStore),XtRImmediate,(XtPointer)NotUseful },
};

XfwfCanvasClassRec xfwfCanvasClassRec = {
{ /* core_class part */
/* superclass   	*/  (WidgetClass) &xfwfBoardClassRec,
/* class_name   	*/  "XfwfCanvas",
/* widget_size  	*/  sizeof(XfwfCanvasRec),
/* class_initialize 	*/  NULL,
/* class_part_initialize*/  _resolve_inheritance,
/* class_inited 	*/  FALSE,
/* initialize   	*/  NULL,
/* initialize_hook 	*/  NULL,
/* realize      	*/  realize,
/* actions      	*/  NULL,
/* num_actions  	*/  0,
/* resources    	*/  resources,
/* num_resources 	*/  1,
/* xrm_class    	*/  NULLQUARK,
/* compres_motion 	*/  True ,
/* compress_exposure 	*/  XtExposeCompressMultiple ,
/* compress_enterleave 	*/  True ,
/* visible_interest 	*/  False ,
/* destroy      	*/  NULL,
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
XtInherit__expose,
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
{ /* XfwfCanvas_class part */
 /* dummy */  0
},
};
WidgetClass xfwfCanvasWidgetClass = (WidgetClass) &xfwfCanvasClassRec;
static void _resolve_inheritance(class)
WidgetClass class;
{
  XfwfCanvasWidgetClass c = (XfwfCanvasWidgetClass) class;
  XfwfCanvasWidgetClass super;
  static CompositeClassExtensionRec extension_rec = {
    NULL, NULLQUARK, XtCompositeExtensionVersion,
    sizeof(CompositeClassExtensionRec), True};
  CompositeClassExtensionRec *ext;
  ext = (XtPointer)XtMalloc(sizeof(*ext));
  *ext = extension_rec;
  ext->next_extension = c->composite_class.extension;
  c->composite_class.extension = ext;
  if (class == xfwfCanvasWidgetClass) return;
  super = (XfwfCanvasWidgetClass)class->core_class.superclass;
}
#line 22 "XWidgets/xwCanvas.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 22 "XWidgets/xwCanvas.w"
static void realize(Widget self,XtValueMask * mask,XSetWindowAttributes * attributes)
#else
#line 22 "XWidgets/xwCanvas.w"
static void realize(self,mask,attributes)Widget self;XtValueMask * mask;XSetWindowAttributes * attributes;
#endif
#line 23 "XWidgets/xwCanvas.w"
{
    if ((((XfwfCanvasWidget)self)->xfwfCanvas.backingStore == Always)
    ||  (((XfwfCanvasWidget)self)->xfwfCanvas.backingStore == NotUseful)
    ||  (((XfwfCanvasWidget)self)->xfwfCanvas.backingStore == WhenMapped)) {
	*mask |= CWBackingStore;
	attributes->backing_store = ((XfwfCanvasWidget)self)->xfwfCanvas.backingStore;
    } else {
	*mask &= ~CWBackingStore;
    }
    /* chain to parent method */
    xfwfBoardClassRec.core_class.realize(self, mask, attributes);
}
#line 38 "XWidgets/xwCanvas.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 38 "XWidgets/xwCanvas.w"
static Boolean  set_values(Widget  old,Widget  request,Widget self,ArgList  args,Cardinal * num_args)
#else
#line 38 "XWidgets/xwCanvas.w"
static Boolean  set_values(old,request,self,args,num_args)Widget  old;Widget  request;Widget self;ArgList  args;Cardinal * num_args;
#endif
#line 39 "XWidgets/xwCanvas.w"
{
    if (((XfwfCanvasWidget)old)->xfwfCanvas.backingStore != ((XfwfCanvasWidget)self)->xfwfCanvas.backingStore) {
	if ((((XfwfCanvasWidget)self)->xfwfCanvas.backingStore == Always)
	||  (((XfwfCanvasWidget)self)->xfwfCanvas.backingStore == NotUseful)
	||  (((XfwfCanvasWidget)self)->xfwfCanvas.backingStore == WhenMapped)) {
	    XSetWindowAttributes attributes;
	    unsigned long	 mask = CWBackingStore;

	    attributes.backing_store = ((XfwfCanvasWidget)self)->xfwfCanvas.backingStore;
	    XChangeWindowAttributes(XtDisplay(self), XtWindow(self), mask, &attributes);
	}
    }

    return FALSE; /* there is no need to redraw */
}
