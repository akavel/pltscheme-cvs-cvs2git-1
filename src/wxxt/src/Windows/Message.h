/*								-*- C++ -*-
 * $Id: Message.h,v 1.2 1999/11/04 17:25:38 mflatt Exp $
 *
 * Purpose: message panel item
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

#ifndef Message_h
#define Message_h

#ifdef __GNUG__
#pragma interface
#endif

class wxBitmap;
class wxPanel;

class wxMessage: public wxItem {
public:
    wxMessage(void);
    ~wxMessage(void);

    wxMessage(wxPanel *panel, char *label, int x=-1, int y=-1,
	     long style=0, char *name="message");
    wxMessage(wxPanel *panel, wxBitmap *bitmap, int x=-1, int y=-1,
	     long style=0, char *name = "message");
    
    Bool Create(wxPanel *panel, char *label, int x=-1, int y=-1,
		long style=0, char *name="message");
    Bool Create(wxPanel *panel, wxBitmap *bitmap, int x=-1, int y=-1,
		long style=0, char *name="message");

    void AllowResize(Bool allow);
    void SetAlignment(long alignment);
    void SetLabel(char *message);
    void SetLabel(wxBitmap *bitmap);

    virtual char  *GetLabel(void);

private:
  wxBitmap *bm_label;
};

#endif // Message_h
