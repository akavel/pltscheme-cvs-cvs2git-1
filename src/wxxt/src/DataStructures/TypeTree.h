/*								-*- C++ -*-
 * $Id: TypeTree.h,v 1.1.1.1 1997/12/22 17:28:45 mflatt Exp $
 *
 * Purpose: type tree (type keys defined in common.h)
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

#ifndef TypeTree_h
#define TypeTree_h

#ifdef __GNUG__
#pragma interface
#endif

void  wxInitStandardTypes(void);

Bool  wxSubType(WXTYPE type1, WXTYPE type2);
char  *wxGetTypeName(WXTYPE type);

class wxTypeDef : public wxObject {
DECLARE_DYNAMIC_CLASS(wxTypeDef)
public:
    inline wxTypeDef(void) : wxObject(FALSE)  { name = NULL; __type = wxTYPE_TYPEDEF; }
    inline ~wxTypeDef(void)  { if (name) delete name; }
private:
    friend Bool wxSubType(WXTYPE type1, WXTYPE type2);
    friend char *wxGetTypeName(WXTYPE type);
    friend class wxTypeTree;

    char    *name;
    WXTYPE  type;
    WXTYPE  parent;
#ifdef MEMORY_USE_METHOD
  inline long MemoryUse(void) { return strlen(name) + wxObject::MemoryUse(); }
#endif
};

class wxTypeTree : public wxHashTable {
DECLARE_DYNAMIC_CLASS(wxTypeTree)
public:
    wxTypeTree(void);
    ~wxTypeTree(void);

    void  AddType(WXTYPE t, WXTYPE parent, char *name);
    char  *GetName(WXTYPE t) { return wxGetTypeName(t); }
};

#endif // TypeTree_h
