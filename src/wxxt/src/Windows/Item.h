/*								-*- C++ -*-
 *
 * Purpose: base class for all panel items
 *
 * Authors: Markus Holzem and Julian Smart
 *
 * Copyright: (C) 2005 PLT Scheme, Inc.
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

#ifndef Item_h
#define Item_h

#ifdef __GNUG__
#pragma interface
#endif

class wxCommandEvent;
class wxColour;
class wxFont;
class wxPanel;
class wxBitmap;

class wxItem : public wxWindow {
public:
  wxItem(void);
  
  // chain panel<->item, copy colours, fonts, and style
  void ChainToPanel(wxPanel *parent, long style=0, char *name=NULL);
  virtual void  Command(wxCommandEvent *event);
  void  ProcessCommand(wxCommandEvent *event);
  
  wxBitmap *CheckMask(wxBitmap *bm);

protected:
    wxFunction callback;
    wxFont     *label_font;
};

extern char *wxGetCtlLabel(char *label);

#endif // Item_h
