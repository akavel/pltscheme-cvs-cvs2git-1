/*								-*- C++ -*-
 * $Id: Error.cc,v 1.1.1.1 1997/12/22 17:28:56 mflatt Exp $
 *
 * Purpose: error routines
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

#include "wx.h"

#include <stdarg.h>

void wxDebugMsg(const char *fmt, ...)
{
#if 0
    if (!wxAPP_DEBUGOUTPUT)
	return ;
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
#endif
}

void wxError(const char *msg, const char *title)
{
    fprintf(stderr, "%s: %s\n", title, msg);
}

void wxFatalError(const char *msg, const char *title)
{
    fprintf(stderr, "%s: %s\n", title, msg);
    exit(1);
}
