/*								-*- C++ -*-
 * $Id: Gauge.cc,v 1.1 1997/12/22 17:28:59 mflatt Exp $
 *
 * Purpose: gauge panel item
 *
 * Authors: Markus Holzem and Julian Smart
 *
 * Copyright: (C) 1995, AIAI, University of Edinburgh (Julian)
 * Copyright: (C) 1995, GNU (Markus)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef __GNUG__
#pragma implementation "Gauge.h"
#endif

#define  Uses_XtIntrinsic
#define  Uses_wxGauge
#include "wx.h"
#define  Uses_TraversingEnforcerWidget
#define  Uses_SliderWidget
#include "widgets.h"

//-----------------------------------------------------------------------------
// create and destroy gauge
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGauge, wxItem)

wxGauge::wxGauge(void) : wxItem()
{
    __type = wxTYPE_GAUGE;

    range = value = 0;
}

wxGauge::wxGauge(wxPanel *panel, char *label, int _range,
		 int x, int y, int width, int height, long style, char *name)
{
    __type = wxTYPE_GAUGE;

    range = value = 0;

    Create(panel, label, _range, x, y, width, height, style, name);
}

#define wxDEFAULT_GAUGE_WIDTH		150
#define wxDEFAULT_GAUGE_HEIGHT		 24

Bool wxGauge::Create(wxPanel *panel, char *label, int _range,
		     int x, int y, int width, int height, long style, char *name)
{
    ChainToPanel(panel, style, name);

    Bool vert = (panel->GetLabelPosition() == wxVERTICAL);
    range     = _range;

    label = wxGetCtlLabel(label);

    // create frame
    X->frame = XtVaCreateManagedWidget
	(name, xfwfTraversingEnforcerWidgetClass, parent->GetHandle()->handle,
	 XtNlabel,       label,
	 XtNalignment,   vert ? XfwfTop : XfwfLeft,
	 XtNbackground,  bg->GetPixel(cmap),
	 XtNforeground,  label_fg->GetPixel(cmap),
	 XtNfont,        label_font->GetInternalFont(),
	 XtNtraversalOn, FALSE,
	 XtNframeType,   XfwfSunken,
	 XtNframeWidth,  2,
	 NULL);
    // create the slider widget
    X->handle = XtVaCreateManagedWidget
	("gauge", xfwfSlider2WidgetClass, X->frame,
	 XtNbackground,    bg->GetPixel(cmap),
	 XtNforeground,    fg->GetPixel(cmap),
	 XtNthumbColor,    bg->GetPixel(cmap),
	 XtNminsize,	   0,
	 XtNframeType,     XfwfRaised,
	 XtNframeWidth,    0,
	 XtNhighlightThickness, 0,
	 NULL);
    XtUninstallTranslations(X->handle);
    SetValue(0);
    // set data declared in wxItem
    // panel positioning
    panel->PositionItem(this, x, y, 
			(width  > -1 
			 ? width  
			 : ((style & wxVERTICAL) 
			    ? wxDEFAULT_GAUGE_HEIGHT
			    : wxDEFAULT_GAUGE_WIDTH)),
			(height > -1 
			 ? height 
			 : ((style & wxVERTICAL) 
			    ? wxDEFAULT_GAUGE_WIDTH
			    : wxDEFAULT_GAUGE_HEIGHT)));
    AddEventHandlers();

    return TRUE;
}

//-----------------------------------------------------------------------------
// methods to access internal data
//-----------------------------------------------------------------------------

void wxGauge::SetButtonColour(wxColour *col)
{
    if (col && X->handle)
	XtVaSetValues(X->handle, XtNthumbColor, col->GetPixel(cmap), NULL);
    wxItem::SetButtonColour(col);
}

void wxGauge::SetRange(int new_range)
{
    if (0 <= new_range) {
	range = new_range;
	SetValue(value);
    }
}

void wxGauge::SetValue(int new_value)
{
    if (0 <= new_value && new_value <= range) {
	value = new_value;
	if (style & wxVERTICAL) {
	    XfwfMoveThumb  (X->handle, 0.0, 1.0                      );
	    XfwfResizeThumb(X->handle, 1.0, float(value)/float(range));
	} else {
	    XfwfMoveThumb  (X->handle, 0.0,                       0.0);
	    XfwfResizeThumb(X->handle, float(value)/float(range), 1.0);
	}
    }
}
