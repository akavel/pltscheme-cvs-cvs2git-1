/*								-*- C++ -*-
 * $Id: GroupBox.h,v 1.1 1997/12/22 17:28:59 mflatt Exp $
 *
 * Purpose: group box choice panel item
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

#ifndef GroupBox_h
#define GroupBox_h

#ifdef __GNUG__
#pragma interface
#endif

class wxPanel;

class wxGroupBox : public wxItem {
DECLARE_DYNAMIC_CLASS(wxGroupBox)
public:
    wxGroupBox(void);
    wxGroupBox(wxPanel *panel, char *label,
	       int x = -1, int y = -1, int width = -1, int height = -1,
	       long style = 0, char *name = "group");

    Bool Create(wxPanel *panel, char *label,
		int x = -1, int y = -1, int width = -1, int height = -1,
		long style = 0, char *name = "group");
};

#endif // GroupBox_h
