/*								-*- C++ -*-
 * $Id: Colour.cc,v 1.6 1999/07/19 22:19:22 mflatt Exp $
 *
 * Purpose: classes to cover colours and colourmaps
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
#pragma implementation "Colour.h"
#endif

#define  Uses_XLib
#define  Uses_wxColour
#define  Uses_wxHashTable
#include "wx.h"

#include <ctype.h>

IMPLEMENT_DYNAMIC_CLASS(wxColour, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxColourDatabase, wxList)
IMPLEMENT_DYNAMIC_CLASS(wxColourMap, wxObject)

// shift between wxWindows RGB- and XColor RGB-values
// (necessary because the values specify an intensity)
#define SHIFT (8*(sizeof(short int)-sizeof(char)))

extern "C" { 
#include "XWidgets/wxAllocColor.h"
};

//-----------------------------------------------------------------------------
// private data of wxColour and wxColourMap
//-----------------------------------------------------------------------------

class wxColour_Xintern {
public:
    XColor   xcolor;
    Bool     have_pixel;
    Colormap xcolormap;
};

class wxColourMap_Xintern {
public:
    Colormap xcolormap;
    Bool     priv;
};

//-----------------------------------------------------------------------------
// wxColour
//-----------------------------------------------------------------------------

/* Since destrcutor doesn't do anything: */
#define COLOR_CLEANUP WXGC_NO_CLEANUP

wxColour::wxColour(void)
: wxObject(COLOR_CLEANUP)
{
    __type = wxTYPE_COLOUR;

    X = NULL; // not Ok
    locked = 0;
}

wxColour::wxColour(wxColour& col)
: wxObject(COLOR_CLEANUP)
{
    __type = wxTYPE_COLOUR;

    locked = 0;

    if (col.Ok()) {
	X  = new wxColour_Xintern; // create new X representation
	*X = *(col.X);		   // assign data
    } else
	X = NULL; // not Ok
}

wxColour::wxColour(const char *col)
: wxObject(COLOR_CLEANUP)
{
    __type = wxTYPE_COLOUR;

    locked = 0;

    wxColour *the_colour
	= wxTheColourDatabase->FindColour(col); // find colour by name

    if (the_colour) {
	X  = new wxColour_Xintern; // create new X representation
	*X = *(the_colour->X);	   // assign data
    } else
	X = NULL; // not Ok
}

wxColour::wxColour(unsigned char r, unsigned char g, unsigned char b)
: wxObject(COLOR_CLEANUP)
{
    __type = wxTYPE_COLOUR;

    locked = 0;
    X = NULL; Set(r, g, b); // set RGB-values
}

wxColour::~wxColour(void)
{
  /* If you do anything important here, be sure to change
     COLOR_CLEANUP */

    FreePixel(TRUE);
}

//--- assignment -------------------------------------------------------------

wxColour& wxColour::operator = (wxColour& col)
{
    FreePixel(TRUE); // free pixel before assignment
	
    if (col.Ok()) {
	X  = new wxColour_Xintern; // create new X representation;
	*X = *(col.X);		   // assign data
	X->have_pixel = FALSE;
    }
    return *this;
}

wxColour& wxColour::operator = (const char *col)
{
    FreePixel(TRUE); // free pixel before assignment
    
    wxColour *the_colour
	= wxTheColourDatabase->FindColour(col); // find colour by name

    if (the_colour) {
	X  = new wxColour_Xintern; // create new X representation
	*X = *(the_colour->X);	   // assign data
	X->have_pixel = FALSE;
    }
    return (*this);
}

//--- get and set RGB values --------------------------------------------------

void wxColour::Set(unsigned char r, unsigned char g, unsigned char b)
{
    FreePixel(TRUE); X = new wxColour_Xintern; // create new X representation

    X->xcolor.red   = ((unsigned short)r) << SHIFT; // init XColor structure
    X->xcolor.green = ((unsigned short)g) << SHIFT;
    X->xcolor.blue  = ((unsigned short)b) << SHIFT;
    X->xcolor.flags = DoRed | DoGreen | DoBlue;
    X->have_pixel   = FALSE; // no pixel value assigned
}

void wxColour::Get(unsigned char *r, unsigned char *g, unsigned char *b)
{
    if (X) {
	*r = (unsigned char)(X->xcolor.red   >> SHIFT);
	*g = (unsigned char)(X->xcolor.green >> SHIFT);
	*b = (unsigned char)(X->xcolor.blue  >> SHIFT);
    } else {
	*r = *g = *b = 0;
    }
}

unsigned char wxColour::Red(void)
{
    return ( X ? (unsigned char)(X->xcolor.red >> SHIFT) : 0 );
}

unsigned char wxColour::Green(void)
{
    return ( X ? (unsigned char)(X->xcolor.green >> SHIFT) : 0 );
}

unsigned char wxColour::Blue(void)
{
    return ( X ? (unsigned char)(X->xcolor.blue >> SHIFT) : 0 );
}

//--- allocate and free X pixel values ----------------------------------------

static int alloc_close_color(Display *display, Colormap cmap, XColor *xc)
{
  XColor ctab[256];
  int ncells = DisplayCells(display, DefaultScreen(display)), j;

  ncells = (ncells < 256) ? ncells : 256;
  
  for (j = 0; j < ncells; j++)
    ctab[j].pixel = j;

  XQueryColors(display, cmap, ctab, ncells);

  int           d, mdist, close;
  
  mdist = 0;   close = -1;
  
  for (j = 0; j < ncells; j++) {
    d = (abs((int)(xc->red - ctab[j].red)) +
	 abs((int)(xc->green - ctab[j].green)) +
	 abs((int)(xc->blue - ctab[j].blue)));
    if (!mdist || (d < mdist)) { 
      mdist = d; 
      close = j;
    }
  }

  if (wxAllocColor(display, cmap, &ctab[close])) { 
    static int approxmsg = 1;
    if (approxmsg) {
      wxError("Warning: cannot allocate colour, using approximate match.\n"
	      "(Future allocations may be approximate without report.)");
      
      approxmsg = 0;
    }

    xc->pixel = ctab[close].pixel;
    return 1;
  } else
    return 0;
}

unsigned long wxColour::GetPixel(wxColourMap *cmap, Bool is_color, Bool fg)
{
  if (!is_color) {
    int white;
    if (!X) {
      white = 1;
    } else if (fg) {
      /* foreground: white = white, all else = black */
      white = (((X->xcolor.red >> SHIFT) == 255)
	       && ((X->xcolor.green >> SHIFT) == 255)
	       && ((X->xcolor.blue >> SHIFT) == 255));
    } else {
      /* background: black = black, all else = white */
      white = (X->xcolor.red || X->xcolor.green || X->xcolor.blue);
    }

    if (white)
      return 0; /* WhitePixelOfScreen(wxAPP_SCREEN); */
    else
      return 1; /* BlackPixelOfScreen(wxAPP_SCREEN); */
  }

    if (X) {
	if (!X->have_pixel) {
	    // no pixel value or wrong colourmap
	    FreePixel(FALSE); // free pixel value if any
	    X->xcolormap = GETCOLORMAP(cmap); // colourmap to use
	    // allocate pixel
	    XColor xcol;

	    /* Copy color b/c XAllocColour sets RGB values */
	    xcol.red = X->xcolor.red;
	    xcol.green = X->xcolor.green;
	    xcol.blue = X->xcolor.blue;
	    xcol.flags = DoRed | DoBlue | DoGreen;

	    if (!wxAllocColor(wxAPP_DISPLAY, X->xcolormap, &xcol)
		&& !alloc_close_color(wxAPP_DISPLAY, X->xcolormap, &xcol)) {
	      // failed => used default
	      static int message_printed = FALSE;
	      if (!message_printed) {
		wxError("Colour allocation failed, using black.\n(Future allocations may fail without reports.)", 
			"wxColour");
		message_printed = TRUE;
	      } 
	      return BlackPixelOfScreen(wxAPP_SCREEN);
	    } else {
	      X->xcolor.pixel = xcol.pixel;
	      X->have_pixel = TRUE; // allocation successful
	    }
	}
    } else {
	// use something as a default value
	wxDebugMsg("wxColour: no colour specified, using white\n");
	return(WhitePixelOfScreen(wxAPP_SCREEN));
    }
    return (X->xcolor.pixel);
}

void wxColour::FreePixel(Bool del)
{
    if (X) {
	if (X->have_pixel) {
	    // free allocated colour
	    // -- currently don't free colours, because the ownership of
	    // -- the pixel-value is not specified!
	    // XFreeColors(wxAPP_DISPLAY, X->xcolormap, &(X->xcolor.pixel), 1, 0);
	    X->have_pixel = FALSE;
	}
	if (del) {
	    delete X; // destroy X representation;
	    X = NULL; // not Ok
	}
    }
}

//-----------------------------------------------------------------------------
// wxColourDatabase
//-----------------------------------------------------------------------------

wxColourDatabase::~wxColourDatabase (void)
{
  wxNode *node = First();
  while (node) {
    wxColour *col = (wxColour*)node->Data();
    wxNode *next = node->Next();
    delete col;
    node = next;
  }
}

wxColour *wxColourDatabase::FindColour(const char *colour)
{
  wxNode *node;

  // Force capital so lc matches as in X
  char uc_colour[256];
  int i;

  for (i = 0; colour[i] && i < 255; i++)
    uc_colour[i] = toupper(colour[i]);
  uc_colour[i] = 0;
  colour = uc_colour;

  if ((node = Find(colour)))
    return (wxColour*)node->Data(); // colour already defined

  /* Define the standard set: */
  static wxHashTable *aux = NULL;
  if (!aux) {
    wxColour *tmpc;
    aux = new wxHashTable(wxKEY_STRING, 20);
#define APPEND_C(name, c) tmpc = c; tmpc->Lock(1); aux->Put(name, tmpc);

    APPEND_C ("AQUAMARINE", new wxColour (112, 219, 147));
    APPEND_C ("BLACK", new wxColour (0, 0, 0));
    APPEND_C ("BLUE", new wxColour (0, 0, 255));
    APPEND_C ("BLUE VIOLET", new wxColour (159, 95, 159));
    APPEND_C ("BROWN", new wxColour (165, 42, 42));
    APPEND_C ("CADET BLUE", new wxColour (95, 159, 159));
    APPEND_C ("CORAL", new wxColour (255, 127, 0));
    APPEND_C ("CORNFLOWER BLUE", new wxColour (66, 66, 111));
    APPEND_C ("CYAN", new wxColour (0, 255, 255));
    APPEND_C ("DARK GRAY", new wxColour (47, 47, 47));
    APPEND_C ("DARK GREEN", new wxColour (47, 79, 47));
    APPEND_C ("DARK OLIVE GREEN", new wxColour (79, 79, 47));
    APPEND_C ("DARK ORCHID", new wxColour (153, 50, 204));
    APPEND_C ("DARK SLATE BLUE", new wxColour (107, 35, 142));
    APPEND_C ("DARK SLATE GRAY", new wxColour (47, 79, 79));
    APPEND_C ("DARK TURQUOISE", new wxColour (112, 147, 219));
    APPEND_C ("DIM GRAY", new wxColour (84, 84, 84));
    APPEND_C ("FIREBRICK", new wxColour (142, 35, 35));
    APPEND_C ("FOREST GREEN", new wxColour (35, 142, 35));
    APPEND_C ("GOLD", new wxColour (204, 127, 50));
    APPEND_C ("GOLDENROD", new wxColour (219, 219, 112));
    APPEND_C ("GRAY", new wxColour (192, 192, 192));
    APPEND_C ("GREEN", new wxColour (0, 255, 0));
    APPEND_C ("GREEN YELLOW", new wxColour (147, 219, 112));
    APPEND_C ("INDIAN RED", new wxColour (79, 47, 47));
    APPEND_C ("KHAKI", new wxColour (159, 159, 95));
    APPEND_C ("LIGHT BLUE", new wxColour (191, 216, 216));
    APPEND_C ("LIGHT GRAY", new wxColour (168, 168, 168));
    APPEND_C ("LIGHT STEEL BLUE", new wxColour (143, 143, 188));
    APPEND_C ("LIME GREEN", new wxColour (50, 204, 50));
    APPEND_C ("LIGHT MAGENTA", new wxColour (255, 0, 255));
    APPEND_C ("MAGENTA", new wxColour (255, 0, 255));
    APPEND_C ("MAROON", new wxColour (142, 35, 107));
    APPEND_C ("MEDIUM AQUAMARINE", new wxColour (50, 204, 153));
    APPEND_C ("MEDIUM GRAY", new wxColour (100, 100, 100));
    APPEND_C ("MEDIUM BLUE", new wxColour (50, 50, 204));
    APPEND_C ("MEDIUM FOREST GREEN", new wxColour (107, 142, 35));
    APPEND_C ("MEDIUM GOLDENROD", new wxColour (234, 234, 173));
    APPEND_C ("MEDIUM ORCHID", new wxColour (147, 112, 219));
    APPEND_C ("MEDIUM SEA GREEN", new wxColour (66, 111, 66));
    APPEND_C ("MEDIUM SLATE BLUE", new wxColour (127, 0, 255));
    APPEND_C ("MEDIUM SPRING GREEN", new wxColour (127, 255, 0));
    APPEND_C ("MEDIUM TURQUOISE", new wxColour (112, 219, 219));
    APPEND_C ("MEDIUM VIOLET RED", new wxColour (219, 112, 147));
    APPEND_C ("MIDNIGHT BLUE", new wxColour (47, 47, 79));
    APPEND_C ("NAVY", new wxColour (35, 35, 142));
    APPEND_C ("ORANGE", new wxColour (204, 50, 50));
    APPEND_C ("ORANGE RED", new wxColour (255, 0, 127));
    APPEND_C ("ORCHID", new wxColour (219, 112, 219));
    APPEND_C ("PALE GREEN", new wxColour (143, 188, 143));
    APPEND_C ("PINK", new wxColour (188, 143, 234));
    APPEND_C ("PLUM", new wxColour (234, 173, 234));
    APPEND_C ("PURPLE", new wxColour (176, 0, 255));
    APPEND_C ("RED", new wxColour (255, 0, 0));
    APPEND_C ("SALMON", new wxColour (111, 66, 66));
    APPEND_C ("SEA GREEN", new wxColour (35, 142, 107));
    APPEND_C ("SIENNA", new wxColour (142, 107, 35));
    APPEND_C ("SKY BLUE", new wxColour (50, 153, 204));
    APPEND_C ("SLATE BLUE", new wxColour (0, 127, 255));
    APPEND_C ("SPRING GREEN", new wxColour (0, 255, 127));
    APPEND_C ("STEEL BLUE", new wxColour (35, 107, 142));
    APPEND_C ("TAN", new wxColour (219, 147, 112));
    APPEND_C ("THISTLE", new wxColour (216, 191, 216));
    APPEND_C ("TURQUOISE", new wxColour (173, 234, 234));
    APPEND_C ("VIOLET", new wxColour (79, 47, 79));
    APPEND_C ("VIOLET RED", new wxColour (204, 50, 153));
    APPEND_C ("WHEAT", new wxColour (216, 216, 191));
    APPEND_C ("WHITE", new wxColour (255, 255, 255));
    APPEND_C ("YELLOW", new wxColour (255, 255, 0));
    APPEND_C ("YELLOW GREEN", new wxColour (153, 204, 50));
  }

  XColor xcolor;
  wxColour *col;

  // use wxAPP_DISPLAY and wxAPP_COLOURMAP as default
  if (XParseColor(wxAPP_DISPLAY, GETCOLORMAP(wxAPP_COLOURMAP), colour, &xcolor)) {
    // new colour found: add to list as found, but only if it's in the standard set
    col = (wxColour *)aux->Get(colour);
    if (col) {
      col = DEBUG_NEW wxColour((unsigned char)(xcolor.red >> SHIFT),
			       (unsigned char)(xcolor.green >> SHIFT),
			       (unsigned char)(xcolor.blue >> SHIFT));
      col->Lock(1);
    }
  } else {
    col = (wxColour *)aux->Get(colour);
  }

  if (col)
    Append(colour, col);

  return col;
}

char *wxColourDatabase::FindName(wxColour& colour)
{
  if (colour.Ok()) {
    unsigned char red   = colour.Red();
    unsigned char green = colour.Green();
    unsigned char blue  = colour.Blue();
    
    for (wxNode *node = First(); node; node = node->Next()) {
      wxColour *col = (wxColour*)node->Data ();
      if (col->Red()==red && col->Green()==green && col->Blue()==blue) {
	char *found = node->key.string;
	if (found)
	  return found;
      }
    }
  }
  return NULL;
}

//-----------------------------------------------------------------------------
// wxColourMap
//-----------------------------------------------------------------------------

wxColourMap::wxColourMap(Bool priv)
{
    __type = wxTYPE_COLOURMAP;

    X  = new wxColourMap_Xintern; // create new X representation
    X->xcolormap = DefaultColormapOfScreen(wxAPP_SCREEN);
    X->priv      = priv;
    // if (X->priv) {
    //	    X = NULL; // create colourmap;
    // } else
    //      X->xcolormap = DefaultColormapOfScreen(wxAPP_SCREEN);
}

wxColourMap::~wxColourMap(void)
{
    if (X) {
	if (X->priv) {
	    // free colourmap
	}
	delete X;
    }
}

void *wxColourMap::GetHandle(void)
{
    if (X)
	return (&(X->xcolormap));
    return &(wxAPP_COLOURMAP->X->xcolormap); // just to return somthing
}

