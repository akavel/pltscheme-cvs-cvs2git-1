/*								-*- C++ -*-
 * $Id: ListBox.h,v 1.5 1998/12/05 01:08:21 mflatt Exp $
 *
 * Purpose: list box panel item
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

#ifndef ListBox_h
#define ListBox_h

#ifdef __GNUG__
#pragma interface
#endif

class wxPanel;
class wxStringList;

class wxListBox : public wxItem {
DECLARE_DYNAMIC_CLASS(wxListBox)
public:
    wxListBox(void);
    wxListBox(wxPanel *panel, wxFunction func, char *title,
	      Bool Multiple = FALSE,
	      int x = -1, int y = -1, int width = -1, int height = -1,
	      int n = 0, char **Choices = NULL,
	      long style = 0, char *name = "listBox");
    ~wxListBox(void);

    // override parent methods
    virtual void ChangeColours(void);
    virtual void SetSize(int width, int height) { SetSize(-1, -1, width, height); }
    virtual void SetSize(int x, int y, int width, int height, int flags=wxSIZE_AUTO);
    // handle the contents of the list
    void  Append(char *item);
    void  Append(char *item, char *client_data);
    void  Clear(void);
    Bool  Create(wxPanel *panel, wxFunction func, char *title,
		 Bool multiple = FALSE,
		 int x = -1, int y = -1, int width = -1, int height = -1,
		 int n = 0, char **choices = NULL,
		 long style = 0, char *name = "listBox");
    void  Delete(int n);
    void  Deselect(int n);
    int   FindString(char *s);
    char  *GetClientData(int n);
    int   GetSelection(void);
    int   GetSelections(int **list_selections);
    char  *GetString(int n);
    char  *GetStringSelection(void);
    void  SetString(int n, char *choice);
    void  InsertItems(int n_items, char **items, int pos);
    int   Number(void);
    Bool  Selected(int n);
    void  Set(int n, char *choices[]);
    void  Set(wxStringList *slist);
    void  SetClientData(int n, char *client_data);
    void  SetClientData(char *data) { wxItem::SetClientData(data); }
    void  SetFirstItem(int n);
    void  SetFirstItem(char *s);
    void  SetSelection(int c, Bool select = TRUE);
    void  SetOneSelection(int c);
    Bool  SetStringSelection(char *s);
    int   GetFirstItem();
    int   NumberOfVisibleItems();
    void  Command(wxCommandEvent &event);
    void  OnChar(wxKeyEvent &e);
private:
#   ifdef Have_Xt_Types
    static void EventCallback(Widget w, XtPointer, XtPointer);
#   endif
    void  SetInternalData(void);

    char    **choices;
    char    **client_data;
    int     num_choices, num_free;
};

#endif // ListBox_h
