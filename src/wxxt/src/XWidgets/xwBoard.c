/* Generated by wbuild
 * (generator version 3.2)
 */
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#line 477 "XWidgets/xwBoard.w"
#include <stdio.h>
#line 478 "XWidgets/xwBoard.w"
#include <X11/Shell.h>
#line 479 "XWidgets/xwBoard.w"
#include <ctype.h>
#include <./xwBoardP.h>
static void _resolve_inheritance(
#if NeedFunctionPrototypes
WidgetClass
#endif
);
#line 144 "XWidgets/xwBoard.w"
static Boolean  set_values(
#if NeedFunctionPrototypes
Widget ,Widget ,Widget,ArgList ,Cardinal *
#endif
);
#line 194 "XWidgets/xwBoard.w"
static void initialize(
#if NeedFunctionPrototypes
Widget ,Widget,ArgList ,Cardinal *
#endif
);
#line 217 "XWidgets/xwBoard.w"
static void set_abs_location(
#if NeedFunctionPrototypes
Widget,unsigned  int ,int ,int ,int ,int 
#endif
);
#line 236 "XWidgets/xwBoard.w"
static void resize(
#if NeedFunctionPrototypes
Widget
#endif
);
#line 261 "XWidgets/xwBoard.w"
static XtGeometryResult  query_geometry(
#if NeedFunctionPrototypes
Widget,XtWidgetGeometry *,XtWidgetGeometry *
#endif
);
#line 272 "XWidgets/xwBoard.w"
static XtGeometryResult  geometry_manager(
#if NeedFunctionPrototypes
Widget ,XtWidgetGeometry *,XtWidgetGeometry *
#endif
);
#line 295 "XWidgets/xwBoard.w"
static void change_managed(
#if NeedFunctionPrototypes
Widget
#endif
);
#line 309 "XWidgets/xwBoard.w"
#define ceil(r) (-(int )(-(r )))


#line 314 "XWidgets/xwBoard.w"
static void generate_location(
#if NeedFunctionPrototypes
Widget
#endif
);
#line 335 "XWidgets/xwBoard.w"
static void get_core_geometry(
#if NeedFunctionPrototypes
Widget,Position *,Position *,Dimension *,Dimension *
#endif
);
#line 365 "XWidgets/xwBoard.w"
static void set_location(
#if NeedFunctionPrototypes
Widget,unsigned  int 
#endif
);
#line 406 "XWidgets/xwBoard.w"
#define skip_blanks(s) while (isspace (*s ))s ++


#line 412 "XWidgets/xwBoard.w"
#define strtonum(t, n) do {\
        while ((*(t)) && !isdigit(*(t)) && *(t) != '.') (t)++;\
        for ((n) = 0; isdigit(*t); (t)++)\
            (n) = 10 * (n) + *(t) - '0';\
    }while (0 )


#line 424 "XWidgets/xwBoard.w"
#define strtofrac(t, n, factor) for ((factor )=1.0 ,(n )=0 ,(t )++;isdigit (*(t ));(t )++,(factor )/=10.0 )(n )=10 *(n )+*(t )-'0'


#line 428 "XWidgets/xwBoard.w"
static String  scan(
#if NeedFunctionPrototypes
String ,Position *,float *
#endif
);
#line 462 "XWidgets/xwBoard.w"
static void interpret_location(
#if NeedFunctionPrototypes
Widget
#endif
);
#line 314 "XWidgets/xwBoard.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 314 "XWidgets/xwBoard.w"
static void generate_location(Widget self)
#else
#line 314 "XWidgets/xwBoard.w"
static void generate_location(self)Widget self;
#endif
#line 315 "XWidgets/xwBoard.w"
{
    char tmp[100];

    (void) sprintf(tmp, "%d+%f %d+%f %d+%f %d+%f",
                   ((XfwfBoardWidget)self)->xfwfBoard.abs_x, ((XfwfBoardWidget)self)->xfwfBoard.rel_x, ((XfwfBoardWidget)self)->xfwfBoard.abs_y, ((XfwfBoardWidget)self)->xfwfBoard.rel_y, ((XfwfBoardWidget)self)->xfwfBoard.abs_width, ((XfwfBoardWidget)self)->xfwfBoard.rel_width,
                   ((XfwfBoardWidget)self)->xfwfBoard.abs_height, ((XfwfBoardWidget)self)->xfwfBoard.rel_height);
    XtFree(((XfwfBoardWidget)self)->xfwfBoard.location);
    ((XfwfBoardWidget)self)->xfwfBoard.location = XtNewString(tmp);
}
#line 335 "XWidgets/xwBoard.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 335 "XWidgets/xwBoard.w"
static void get_core_geometry(Widget self,Position * x,Position * y,Dimension * width,Dimension * height)
#else
#line 335 "XWidgets/xwBoard.w"
static void get_core_geometry(self,x,y,width,height)Widget self;Position * x;Position * y;Dimension * width;Dimension * height;
#endif
#line 336 "XWidgets/xwBoard.w"
{
    Widget parent;
    Position px, py;
    Dimension pw, ph, minsize;
    float h;

    parent = ((XfwfBoardWidget)self)->core.parent;
    if (XtIsSubclass(((XfwfBoardWidget)self)->core.parent, xfwfBoardWidgetClass))
        ((XfwfBoardWidgetClass)parent->core.widget_class)->xfwfCommon_class.compute_inside(parent, &px, &py, &pw, &ph);
    else {
        px = 0;
        py = 0;
        pw = ((XfwfBoardWidget)parent)->core.width;
        ph = ((XfwfBoardWidget)parent)->core.height;
    }

    *x = ceil(((XfwfBoardWidget)self)->xfwfBoard.rel_x * pw + ((XfwfBoardWidget)self)->xfwfBoard.abs_x * ((XfwfBoardWidget)self)->xfwfBoard.hunit) + px;
    *y = ceil(((XfwfBoardWidget)self)->xfwfBoard.rel_y * ph + ((XfwfBoardWidget)self)->xfwfBoard.abs_y * ((XfwfBoardWidget)self)->xfwfBoard.vunit) + py;
    minsize = 2 * ((XfwfBoardWidgetClass)self->core.widget_class)->xfwfCommon_class.total_frame_width(self);
    minsize = max(1, minsize);
    h = ceil(((XfwfBoardWidget)self)->xfwfBoard.rel_width * pw + ((XfwfBoardWidget)self)->xfwfBoard.abs_width * ((XfwfBoardWidget)self)->xfwfBoard.hunit);
    *width = h < minsize ? minsize : h;
    h = ceil(((XfwfBoardWidget)self)->xfwfBoard.rel_height * ph + ((XfwfBoardWidget)self)->xfwfBoard.abs_height * ((XfwfBoardWidget)self)->xfwfBoard.vunit);
    *height = h < minsize ? minsize : h;
}
#line 365 "XWidgets/xwBoard.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 365 "XWidgets/xwBoard.w"
static void set_location(Widget self,unsigned  int  flags)
#else
#line 365 "XWidgets/xwBoard.w"
static void set_location(self,flags)Widget self;unsigned  int  flags;
#endif
#line 366 "XWidgets/xwBoard.w"
{
    Widget parent;
    Position px, py;
    Dimension pw, ph;

    parent = ((XfwfBoardWidget)self)->core.parent;
    if (XtIsSubclass(((XfwfBoardWidget)self)->core.parent, xfwfBoardWidgetClass))
        ((XfwfBoardWidgetClass)parent->core.widget_class)->xfwfCommon_class.compute_inside(parent, &px, &py, &pw, &ph);
    else {
        px = 0;
        py = 0;
        pw = ((XfwfBoardWidget)parent)->core.width;
        ph = ((XfwfBoardWidget)parent)->core.height;
    }
    if (flags & CWX) {
        ((XfwfBoardWidget)self)->xfwfBoard.rel_x = 0.0;
        ((XfwfBoardWidget)self)->xfwfBoard.abs_x = ceil((((XfwfBoardWidget)self)->core.x - px)/((XfwfBoardWidget)self)->xfwfBoard.hunit);
    }
    if (flags & CWY) {
        ((XfwfBoardWidget)self)->xfwfBoard.rel_y = 0.0;
        ((XfwfBoardWidget)self)->xfwfBoard.abs_y = ceil((((XfwfBoardWidget)self)->core.y - py)/((XfwfBoardWidget)self)->xfwfBoard.vunit);
    }
    if (flags & CWWidth) {
        ((XfwfBoardWidget)self)->xfwfBoard.rel_width = 0.0;
        ((XfwfBoardWidget)self)->xfwfBoard.abs_width = ceil(((XfwfBoardWidget)self)->core.width/((XfwfBoardWidget)self)->xfwfBoard.hunit);
    }
    if (flags & CWHeight) {
        ((XfwfBoardWidget)self)->xfwfBoard.rel_height = 0.0;
        ((XfwfBoardWidget)self)->xfwfBoard.abs_height = ceil(((XfwfBoardWidget)self)->core.height/((XfwfBoardWidget)self)->xfwfBoard.vunit);
    }
}
#line 428 "XWidgets/xwBoard.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 428 "XWidgets/xwBoard.w"
static String  scan(String  s,Position * absval,float * relval)
#else
#line 428 "XWidgets/xwBoard.w"
static String  scan(s,absval,relval)String  s;Position * absval;float * relval;
#endif
#line 429 "XWidgets/xwBoard.w"
{
    String p;
    char c;
    long n;
    float fract, factor;

    *absval = 0;
    *relval = 0.0;
    p = s;
    strtonum(p, n);
    if (*p != '.') {                            /* 1st number is integer */
        *absval = n;
        skip_blanks(p);
        c = *p;
        if (c != '+' && c != '-') return p;     /* No second number */
        s = p;
        strtonum(p, n);
        if (*p != '.') return s;                /* This is an error... */
        strtofrac(p, fract, factor);
        *relval = c == '-' ? -fract * factor - n : fract * factor + n;
        return p;
    } else {                                    /* 1st number is float */
        strtofrac(p, fract, factor);
        *relval = fract * factor + n;
        skip_blanks(p);
        c = *p;
        if (c != '+' && c != '-') return p;     /* No second number */
        strtonum(p, n);
        *absval = c == '-' ? -n : n;
        return p;
    }
}
#line 462 "XWidgets/xwBoard.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 462 "XWidgets/xwBoard.w"
static void interpret_location(Widget self)
#else
#line 462 "XWidgets/xwBoard.w"
static void interpret_location(self)Widget self;
#endif
#line 463 "XWidgets/xwBoard.w"
{
    char *s;

    s = ((XfwfBoardWidget)self)->xfwfBoard.location;
    s = scan(s, &((XfwfBoardWidget)self)->xfwfBoard.abs_x, &((XfwfBoardWidget)self)->xfwfBoard.rel_x);
    s = scan(s, &((XfwfBoardWidget)self)->xfwfBoard.abs_y, &((XfwfBoardWidget)self)->xfwfBoard.rel_y);
    s = scan(s, &((XfwfBoardWidget)self)->xfwfBoard.abs_width, &((XfwfBoardWidget)self)->xfwfBoard.rel_width);
    s = scan(s, &((XfwfBoardWidget)self)->xfwfBoard.abs_height, &((XfwfBoardWidget)self)->xfwfBoard.rel_height);
}

static XtResource resources[] = {
#line 59 "XWidgets/xwBoard.w"
{XtNabs_x,XtCAbs_x,XtRPosition,sizeof(((XfwfBoardRec*)NULL)->xfwfBoard.abs_x),XtOffsetOf(XfwfBoardRec,xfwfBoard.abs_x),XtRImmediate,(XtPointer)0 },
#line 60 "XWidgets/xwBoard.w"
{XtNrel_x,XtCRel_x,XtRFloat,sizeof(((XfwfBoardRec*)NULL)->xfwfBoard.rel_x),XtOffsetOf(XfwfBoardRec,xfwfBoard.rel_x),XtRString,(XtPointer)"0.0"},
#line 61 "XWidgets/xwBoard.w"
{XtNabs_y,XtCAbs_y,XtRPosition,sizeof(((XfwfBoardRec*)NULL)->xfwfBoard.abs_y),XtOffsetOf(XfwfBoardRec,xfwfBoard.abs_y),XtRImmediate,(XtPointer)0 },
#line 62 "XWidgets/xwBoard.w"
{XtNrel_y,XtCRel_y,XtRFloat,sizeof(((XfwfBoardRec*)NULL)->xfwfBoard.rel_y),XtOffsetOf(XfwfBoardRec,xfwfBoard.rel_y),XtRString,(XtPointer)"0.0"},
#line 70 "XWidgets/xwBoard.w"
{XtNx,XtCX,XtRPosition,sizeof(((XfwfBoardRec*)NULL)->core.x),XtOffsetOf(XfwfBoardRec,core.x),XtRImmediate,(XtPointer)MAGICNUM },
#line 71 "XWidgets/xwBoard.w"
{XtNy,XtCY,XtRPosition,sizeof(((XfwfBoardRec*)NULL)->core.y),XtOffsetOf(XfwfBoardRec,core.y),XtRImmediate,(XtPointer)MAGICNUM },
#line 79 "XWidgets/xwBoard.w"
{XtNabs_width,XtCAbs_width,XtRPosition,sizeof(((XfwfBoardRec*)NULL)->xfwfBoard.abs_width),XtOffsetOf(XfwfBoardRec,xfwfBoard.abs_width),XtRImmediate,(XtPointer)0 },
#line 80 "XWidgets/xwBoard.w"
{XtNrel_width,XtCRel_width,XtRFloat,sizeof(((XfwfBoardRec*)NULL)->xfwfBoard.rel_width),XtOffsetOf(XfwfBoardRec,xfwfBoard.rel_width),XtRString,(XtPointer)"1.0"},
#line 81 "XWidgets/xwBoard.w"
{XtNabs_height,XtCAbs_height,XtRPosition,sizeof(((XfwfBoardRec*)NULL)->xfwfBoard.abs_height),XtOffsetOf(XfwfBoardRec,xfwfBoard.abs_height),XtRImmediate,(XtPointer)0 },
#line 82 "XWidgets/xwBoard.w"
{XtNrel_height,XtCRel_height,XtRFloat,sizeof(((XfwfBoardRec*)NULL)->xfwfBoard.rel_height),XtOffsetOf(XfwfBoardRec,xfwfBoard.rel_height),XtRString,(XtPointer)"1.0"},
#line 87 "XWidgets/xwBoard.w"
{XtNwidth,XtCWidth,XtRDimension,sizeof(((XfwfBoardRec*)NULL)->core.width),XtOffsetOf(XfwfBoardRec,core.width),XtRImmediate,(XtPointer)MAGICNUM },
#line 88 "XWidgets/xwBoard.w"
{XtNheight,XtCHeight,XtRDimension,sizeof(((XfwfBoardRec*)NULL)->core.height),XtOffsetOf(XfwfBoardRec,core.height),XtRImmediate,(XtPointer)MAGICNUM },
#line 94 "XWidgets/xwBoard.w"
{XtNhunit,XtCHunit,XtRFloat,sizeof(((XfwfBoardRec*)NULL)->xfwfBoard.hunit),XtOffsetOf(XfwfBoardRec,xfwfBoard.hunit),XtRString,(XtPointer)"1.0"},
#line 95 "XWidgets/xwBoard.w"
{XtNvunit,XtCVunit,XtRFloat,sizeof(((XfwfBoardRec*)NULL)->xfwfBoard.vunit),XtOffsetOf(XfwfBoardRec,xfwfBoard.vunit),XtRString,(XtPointer)"1.0"},
#line 118 "XWidgets/xwBoard.w"
{XtNlocation,XtCLocation,XtRString,sizeof(((XfwfBoardRec*)NULL)->xfwfBoard.location),XtOffsetOf(XfwfBoardRec,xfwfBoard.location),XtRImmediate,(XtPointer)NULL },
};

XfwfBoardClassRec xfwfBoardClassRec = {
{ /* core_class part */
/* superclass   	*/  (WidgetClass) &xfwfFrameClassRec,
/* class_name   	*/  "XfwfBoard",
/* widget_size  	*/  sizeof(XfwfBoardRec),
/* class_initialize 	*/  NULL,
/* class_part_initialize*/  _resolve_inheritance,
/* class_inited 	*/  FALSE,
/* initialize   	*/  initialize,
/* initialize_hook 	*/  NULL,
/* realize      	*/  XtInheritRealize,
/* actions      	*/  NULL,
/* num_actions  	*/  0,
/* resources    	*/  resources,
/* num_resources 	*/  15,
/* xrm_class    	*/  NULLQUARK,
/* compres_motion 	*/  True ,
/* compress_exposure 	*/  XtExposeCompressMultiple ,
/* compress_enterleave 	*/  True ,
/* visible_interest 	*/  False ,
/* destroy      	*/  NULL,
/* resize       	*/  resize,
/* expose       	*/  XtInheritExpose,
/* set_values   	*/  set_values,
/* set_values_hook 	*/  NULL,
/* set_values_almost 	*/  XtInheritSetValuesAlmost,
/* get_values+hook 	*/  NULL,
/* accept_focus 	*/  XtInheritAcceptFocus,
/* version      	*/  XtVersion,
/* callback_private 	*/  NULL,
/* tm_table      	*/  NULL,
/* query_geometry 	*/  query_geometry,
/* display_acceleator 	*/  XtInheritDisplayAccelerator,
/* extension    	*/  NULL 
},
{ /* composite_class part */
geometry_manager,
change_managed,
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
set_abs_location,
},
};
WidgetClass xfwfBoardWidgetClass = (WidgetClass) &xfwfBoardClassRec;
static void _resolve_inheritance(class)
WidgetClass class;
{
  XfwfBoardWidgetClass c = (XfwfBoardWidgetClass) class;
  XfwfBoardWidgetClass super;
  static CompositeClassExtensionRec extension_rec = {
    NULL, NULLQUARK, XtCompositeExtensionVersion,
    sizeof(CompositeClassExtensionRec), True};
  CompositeClassExtensionRec *ext;
  ext = (XtPointer)XtMalloc(sizeof(*ext));
  *ext = extension_rec;
  ext->next_extension = c->composite_class.extension;
  c->composite_class.extension = ext;
  if (class == xfwfBoardWidgetClass) return;
  super = (XfwfBoardWidgetClass)class->core_class.superclass;
  if (c->xfwfBoard_class.set_abs_location == XtInherit_set_abs_location)
    c->xfwfBoard_class.set_abs_location = super->xfwfBoard_class.set_abs_location;
}
#line 144 "XWidgets/xwBoard.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 144 "XWidgets/xwBoard.w"
static Boolean  set_values(Widget  old,Widget  request,Widget self,ArgList  args,Cardinal * num_args)
#else
#line 144 "XWidgets/xwBoard.w"
static Boolean  set_values(old,request,self,args,num_args)Widget  old;Widget  request;Widget self;ArgList  args;Cardinal * num_args;
#endif
#line 145 "XWidgets/xwBoard.w"
{
    XtWidgetGeometry reply;
    int i;

    if (((XfwfBoardWidget)self)->xfwfBoard.location != ((XfwfBoardWidget)old)->xfwfBoard.location) {
        XtFree(((XfwfBoardWidget)old)->xfwfBoard.location);
        ((XfwfBoardWidget)self)->xfwfBoard.location = XtNewString(((XfwfBoardWidget)self)->xfwfBoard.location);
        interpret_location(self);
        get_core_geometry(self, &((XfwfBoardWidget)self)->core.x, &((XfwfBoardWidget)self)->core.y, &((XfwfBoardWidget)self)->core.width, &((XfwfBoardWidget)self)->core.height);
    } else if (ceil(((XfwfBoardWidget)self)->xfwfBoard.abs_x*((XfwfBoardWidget)self)->xfwfBoard.hunit) != ceil(((XfwfBoardWidget)old)->xfwfBoard.abs_x*((XfwfBoardWidget)old)->xfwfBoard.hunit)
               || ceil(((XfwfBoardWidget)self)->xfwfBoard.abs_width*((XfwfBoardWidget)self)->xfwfBoard.hunit) != ceil(((XfwfBoardWidget)old)->xfwfBoard.abs_width*((XfwfBoardWidget)old)->xfwfBoard.hunit)
               || ceil(((XfwfBoardWidget)self)->xfwfBoard.abs_y*((XfwfBoardWidget)self)->xfwfBoard.vunit) != ceil(((XfwfBoardWidget)old)->xfwfBoard.abs_y*((XfwfBoardWidget)old)->xfwfBoard.vunit)
               || ceil(((XfwfBoardWidget)self)->xfwfBoard.abs_height*((XfwfBoardWidget)self)->xfwfBoard.vunit) != ceil(((XfwfBoardWidget)old)->xfwfBoard.abs_height*((XfwfBoardWidget)old)->xfwfBoard.vunit)
               || ((XfwfBoardWidget)self)->xfwfBoard.rel_x != ((XfwfBoardWidget)old)->xfwfBoard.rel_x
               || ((XfwfBoardWidget)self)->xfwfBoard.rel_y != ((XfwfBoardWidget)old)->xfwfBoard.rel_y
               || ((XfwfBoardWidget)self)->xfwfBoard.rel_width != ((XfwfBoardWidget)old)->xfwfBoard.rel_width
               || ((XfwfBoardWidget)self)->xfwfBoard.rel_height != ((XfwfBoardWidget)old)->xfwfBoard.rel_height) {
        get_core_geometry(self, &((XfwfBoardWidget)self)->core.x, &((XfwfBoardWidget)self)->core.y, &((XfwfBoardWidget)self)->core.width, &((XfwfBoardWidget)self)->core.height);
        generate_location(self);
    } else if (((XfwfBoardWidget)self)->core.x != ((XfwfBoardWidget)old)->core.x
               || ((XfwfBoardWidget)self)->core.y != ((XfwfBoardWidget)old)->core.y
               || ((XfwfBoardWidget)self)->core.width != ((XfwfBoardWidget)old)->core.width
               || ((XfwfBoardWidget)self)->core.height != ((XfwfBoardWidget)old)->core.height) {
      set_location(self, (((((XfwfBoardWidget)self)->core.x != ((XfwfBoardWidget)old)->core.x) ? CWX : 0)
		       | ((((XfwfBoardWidget)self)->core.y != ((XfwfBoardWidget)old)->core.y) ? CWY : 0)
		       | ((((XfwfBoardWidget)self)->core.width != ((XfwfBoardWidget)old)->core.width) ? CWWidth : 0)
		       | ((((XfwfBoardWidget)self)->core.height != ((XfwfBoardWidget)old)->core.height) ? CWHeight : 0)));
      generate_location(self);
    }
    if (((XfwfBoardWidgetClass)self->core.widget_class)->xfwfCommon_class.total_frame_width(old) != ((XfwfBoardWidgetClass)self->core.widget_class)->xfwfCommon_class.total_frame_width(self)) {
        for (i = 0; i < ((XfwfBoardWidget)self)->composite.num_children; i++) {
            (void) XtQueryGeometry(((XfwfBoardWidget)self)->composite.children[i], NULL, &reply);
            XtConfigureWidget(((XfwfBoardWidget)self)->composite.children[i], reply.x, reply.y, reply.width,
                              reply.height, reply.border_width);
        }
    }
    return False;
}
#line 194 "XWidgets/xwBoard.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 194 "XWidgets/xwBoard.w"
static void initialize(Widget  request,Widget self,ArgList  args,Cardinal * num_args)
#else
#line 194 "XWidgets/xwBoard.w"
static void initialize(request,self,args,num_args)Widget  request;Widget self;ArgList  args;Cardinal * num_args;
#endif
#line 195 "XWidgets/xwBoard.w"
{
    if (((XfwfBoardWidget)self)->xfwfBoard.location != NULL) {
        ((XfwfBoardWidget)self)->xfwfBoard.location = XtNewString(((XfwfBoardWidget)self)->xfwfBoard.location);
        interpret_location(self);
        get_core_geometry(self, &((XfwfBoardWidget)self)->core.x, &((XfwfBoardWidget)self)->core.y, &((XfwfBoardWidget)self)->core.width, &((XfwfBoardWidget)self)->core.height);
    } else if (((XfwfBoardWidget)self)->core.x != MAGICNUM || ((XfwfBoardWidget)self)->core.y != MAGICNUM
               || ((XfwfBoardWidget)self)->core.width != MAGICNUM || ((XfwfBoardWidget)self)->core.height != MAGICNUM) {
        set_location(self, CWX | CWY | CWWidth | CWHeight);
        generate_location(self);
    } else {
        generate_location(self);
        get_core_geometry(self, &((XfwfBoardWidget)self)->core.x, &((XfwfBoardWidget)self)->core.y, &((XfwfBoardWidget)self)->core.width, &((XfwfBoardWidget)self)->core.height);
    }
}
#line 217 "XWidgets/xwBoard.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 217 "XWidgets/xwBoard.w"
static void set_abs_location(Widget self,unsigned  int  flags,int  x,int  y,int  w,int  h)
#else
#line 217 "XWidgets/xwBoard.w"
static void set_abs_location(self,flags,x,y,w,h)Widget self;unsigned  int  flags;int  x;int  y;int  w;int  h;
#endif
#line 218 "XWidgets/xwBoard.w"
{
    if ((flags & (CWX | CWY | CWWidth | CWHeight)) == 0) return;
    if (flags & CWX) ((XfwfBoardWidget)self)->core.x = x;
    if (flags & CWY) ((XfwfBoardWidget)self)->core.y = y;
    if (flags & CWWidth) ((XfwfBoardWidget)self)->core.width = w;
    if (flags & CWHeight) ((XfwfBoardWidget)self)->core.height = h;
    set_location(self, flags);
    generate_location(self);
}
#line 236 "XWidgets/xwBoard.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 236 "XWidgets/xwBoard.w"
static void resize(Widget self)
#else
#line 236 "XWidgets/xwBoard.w"
static void resize(self)Widget self;
#endif
#line 237 "XWidgets/xwBoard.w"
{
    int i;
    XtWidgetGeometry reply;
    Widget child;

    for (i = 0; i < ((XfwfBoardWidget)self)->composite.num_children; i++) {
        child = ((XfwfBoardWidget)self)->composite.children[i];
        (void) XtQueryGeometry(child, NULL, &reply);
        XtConfigureWidget(child, reply.x, reply.y, reply.width,
                          reply.height, reply.border_width);
    }
}
#line 261 "XWidgets/xwBoard.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 261 "XWidgets/xwBoard.w"
static XtGeometryResult  query_geometry(Widget self,XtWidgetGeometry * request,XtWidgetGeometry * reply)
#else
#line 261 "XWidgets/xwBoard.w"
static XtGeometryResult  query_geometry(self,request,reply)Widget self;XtWidgetGeometry * request;XtWidgetGeometry * reply;
#endif
#line 262 "XWidgets/xwBoard.w"
{
    reply->request_mode = CWX | CWY | CWWidth | CWHeight;
    get_core_geometry(self, &reply->x, &reply->y,
                      &reply->width, &reply->height);
    return XtGeometryAlmost;
}
#line 272 "XWidgets/xwBoard.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 167 "XWidgets/xwBoard.w"
static XtGeometryResult  geometry_manager(Widget  child,XtWidgetGeometry * request,XtWidgetGeometry * reply)
#else
#line 167 "XWidgets/xwBoard.w"
static XtGeometryResult  geometry_manager(child,request,reply)Widget  child;XtWidgetGeometry * request;XtWidgetGeometry * reply;
#endif
{ Widget self = XtParent(child); {
    /* Widget $ = XtParent(child); */
    Dimension wd, ht, bw;
    Position x, y;

    /* Get complete geometry, from request or current value */
    x = request->request_mode & CWX ? request->x : ((XfwfBoardWidget)child)->core.x;
    y = request->request_mode & CWY ? request->y : ((XfwfBoardWidget)child)->core.y;
    wd = request->request_mode & CWWidth ? request->width : ((XfwfBoardWidget)child)->core.width;
    ht = request->request_mode & CWHeight ? request->height : ((XfwfBoardWidget)child)->core.height;
    bw = request->request_mode & CWBorderWidth ? request->border_width
        : ((XfwfBoardWidget)child)->core.border_width;

    if (wd <= 0) wd = 1; if (ht <= 0) ht = 1; /* MATTHEW: [5] */
    XtConfigureWidget(child, x, y, wd, ht, bw);
    return XtGeometryDone;
}
}
#line 295 "XWidgets/xwBoard.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 295 "XWidgets/xwBoard.w"
static void change_managed(Widget self)
#else
#line 295 "XWidgets/xwBoard.w"
static void change_managed(self)Widget self;
#endif
#line 296 "XWidgets/xwBoard.w"
{
#if 1
    Widget top = self, w;

    while (!XtIsSubclass(top, shellWidgetClass)) top = XtParent(top) ;
    for (w = self; w != top; w = XtParent(w)) XtInstallAllAccelerators(w, top);
#endif
}
