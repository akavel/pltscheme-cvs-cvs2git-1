/*								-*- C++ -*-
 * $Id: MessageBox.cc,v 1.2 1998/09/06 01:54:01 mflatt Exp $
 *
 * Purpose: message box
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

#define  Uses_wxDialogBase
#include "wx.h"

extern int wxsMessageBox(char *message, char *caption, long style, wxWindow *parent);

int wxMessageBox(char *message, char *caption, long style, wxWindow *parent, int, int)
{
  return wxsMessageBox(message, caption, style, parent);
}
