/*
 * File:	wb_win.h
 * Purpose:	
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wb_win.h	1.2 5/9/94" */

/*
 * Purpose:  wxWindow class declaration. Base class for all windows and
 *           panel items.
 */


#ifndef wxb_winh
#define wxb_winh

#include "common.h"
#include "wx_obj.h"
#include "wx_stdev.h"
#include "wx_list.h"

#ifdef IN_CPROTO
typedef       void    *wxFunction ;
typedef       void    *wxbWindow ;
#else

class wxCursor;
class wxFont;
class wxIcon;
class wxColourMap;
class wxMenu;
class wxWindow;
class wxRectangle;
class wxBitmap;
class wxItem;
class wxLayoutConstraints;

// Callback function type definition
typedef void (*wxFunction) (wxObject*, wxEvent*);

/*
 * Event handler: windows have themselves as their event handlers
 * by default, but their event handlers could be set to another
 * object entirely. This separation can reduce the amount of
 * derivation required, and allow alteration of a window's functionality
 * (e.g. by a resource editor that temporarily switches event handlers).
 */
 
class wxEvtHandler: public wxObject
{
 public:
  wxEvtHandler(void);
  ~wxEvtHandler(void);

  virtual void OnMenuCommand(long WXUNUSED(cmd)) {};
  virtual void OnMenuSelect(long WXUNUSED(cmd)) {};
  inline virtual void OnCommand(wxWindow *WXUNUSED(win), wxCommandEvent *WXUNUSED(event)) {};
                                                 // Called if child control has no
                                                 // callback function
  virtual void OnScroll(wxScrollEvent *WXUNUSED(event)) {};
  inline virtual void OnPaint(void) {};                 // Called when needs painting
  virtual void OnSize(int WXUNUSED(width), int WXUNUSED(height)) {};           // Called on resize
  inline virtual void OnMove(int WXUNUSED(x), int WXUNUSED(y)) {};          // Called on move
  inline virtual void OnEvent(wxMouseEvent *WXUNUSED(event)) {};  // Called on mouse event
  inline virtual void OnChar(wxKeyEvent *WXUNUSED(event)) {};     // Called on character event
  // Under Windows, we can intercept character input per dialog or frame
  virtual inline Bool OnCharHook(wxKeyEvent *WXUNUSED(event)) { return FALSE; }
  inline virtual Bool OnClose(void) { return TRUE; };  // Delete window if returns TRUE
  inline virtual void OnActivate(Bool WXUNUSED(active)) {};       // Called on window activation (MSW)
  inline virtual void OnSetFocus(void) {};              // Called on setting focus
  inline virtual void OnKillFocus(void) {};             // Called on killing focus
  inline virtual void OnDropFile(char *WXUNUSED(file)) {};

  // If clicked on panel
  inline virtual void OnLeftClick(int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(keys)) {};
  inline virtual void OnRightClick(int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(keys)) {};

  virtual void OnItemEvent(wxItem *WXUNUSED(item), wxMouseEvent *WXUNUSED(event)) {};
  virtual void OnSelect(Bool WXUNUSED(select)) {};

  virtual void OnDefaultAction(wxItem *WXUNUSED(initiatingItem)) {};
  virtual void OnChangeFocus(wxItem *WXUNUSED(from), wxItem *WXUNUSED(to)) {};
  virtual Bool OnFunctionKey(wxKeyEvent *WXUNUSED(event)) { return FALSE; };
};

/*
 * Base class for frame, panel, canvas, panel items, dialog box.
 *
 */

class wxWindow;
class wxMenu;
class wxSizer;
class wxbWindow: public wxEvtHandler
{
 protected:
  short editUIMode;  // TRUE if user can edit interface
  short internal_disabled;
  long windowStyle; // Store the window's style
  wxEvtHandler *windowEventHandler; // Usually is 'this'
  wxFont *font;                               // Window's font
  wxChildList *children;                           // Window's children
  wxWindow *window_parent;                     // Each window always knows its parent
  Bool is_shown;

 public:
  int doubleClickAllowed ;
  // Font - created on demand, not deleted with window
  wxCursor *wx_cursor;                        // Window's cursor

  Bool paintingEnabled;
  Bool winCaptured;
  char *handle;                                // Pointer to real window
  char *windowName;                            // Window name

  wxFunction callback;                         // Callback associated with the window
  virtual void Callback(wxFunction);           // Adds callback

  // Constructors/Destructors
  wxbWindow(void);
  virtual ~wxbWindow(void);

  virtual void GetSize(int *width, int *height) = 0;
  virtual void GetPosition(int *x, int *y) = 0;
  virtual void GetClientSize(int *width, int *height) = 0; // Size client can use
  virtual void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO) = 0;
  inline virtual void SetSize(int width, int height) { SetSize(-1, -1, width, height, wxSIZE_USE_EXISTING); }
  inline virtual void Move(int x, int y) { SetSize(x, y, -1, -1, wxSIZE_USE_EXISTING); }
  virtual void SetClientSize(int width, int size) = 0;
  virtual void ClientToScreen(int *x, int *y) = 0;
  virtual void ScreenToClient(int *x, int *y) = 0;
  virtual void Enable(Bool enable) = 0;
  virtual void SetFocus(void) = 0;
  virtual void CaptureMouse(void) = 0;
  virtual void ReleaseMouse(void) = 0;
  virtual void DragAcceptFiles(Bool accept) = 0;
  virtual void MakeModal(Bool modal);

  virtual char *GetHandle(void);
  virtual wxWindow *GetParent(void);
  virtual inline void SetParent(wxWindow *p) { window_parent = p; }
  virtual wxWindow *GetGrandParent(void);
  inline wxChildList *GetChildren() { return children; }

  virtual Bool Show(Bool show) = 0;
  virtual wxCursor *SetCursor(wxCursor *cursor) = 0;
  virtual void SetColourMap(wxColourMap *cmap) = 0;

  virtual float GetCharWidth(void) = 0;
  virtual float GetCharHeight(void) = 0;
  /* MATTHEW: [2] 16-bit fonts */
  inline virtual void GetTextExtent(const char *WXUNUSED(string), float *WXUNUSED(x), float *WXUNUSED(y),
       float *WXUNUSED(descent) = NULL, float *WXUNUSED(externalLeading) = NULL, wxFont *WXUNUSED(theFont) = NULL, Bool WXUNUSED(use16) = FALSE) {};

  // Font
  inline virtual void SetFont(wxFont *f) { font = f; }
  inline virtual wxFont *GetFont(void) { return font; }

  inline virtual void SetTitle(char *WXUNUSED(title)) {};      // Set window title
  inline virtual char *GetTitle(void) { return NULL; }; // Set window title
  // Most windows have the concept of a label; for frames, this is the
  // title; for items, this is the label or button text.
  inline virtual char *GetLabel(void) { return GetTitle(); }

  inline virtual char *GetName(void) { return windowName; }
  virtual void SetName(char *name);

  inline virtual void Fit(void) {};                  // Size window to fit contents
  inline virtual void Centre(int WXUNUSED(direction)) {};      // Centre item on panel,
                                               // or frame on screen
  // Renamed from GetWindowStyle since it clashed with a
  // macro in windowsx.h.
  inline long GetWindowStyleFlag(void) { return windowStyle; }
  // A concession to our friends across the pond
  inline void Center(int direction = wxHORIZONTAL) { Centre(direction); }

  inline virtual void EnablePainting(Bool enable) { paintingEnabled = enable; }
  inline virtual void Paint(void) { OnPaint(); }

  virtual Bool PopupMenu(wxMenu *menu, float x, float y) = 0;

  // Scrolling
  virtual void SetScrollPos(int WXUNUSED(orient), int WXUNUSED(pos)) {};
  virtual void SetScrollRange(int WXUNUSED(orient), int WXUNUSED(range)) {};
  virtual int GetScrollPos(int WXUNUSED(orient)) {return 0;};
  virtual int GetScrollRange(int WXUNUSED(orient)) {return 0;};

  // Event handlers that do something by default
  virtual void OnSize(int width, int height);
  virtual void OnMenuSelect(long WXUNUSED(cmd)) {};
  virtual void OnCommand(wxWindow *win, wxCommandEvent *event);

  // Caret
  virtual void CreateCaret(int WXUNUSED(w), int WXUNUSED(h)) {};
  virtual void CreateCaret(wxBitmap *WXUNUSED(bitmap)) {};
  virtual void DestroyCaret(void) {};
  virtual void ShowCaret(Bool WXUNUSED(show)) {};
  virtual void SetCaretPos(int WXUNUSED(x), int WXUNUSED(y)) {};
  virtual void GetCaretPos(int *WXUNUSED(x), int *WXUNUSED(y)) {};

  // INTERNAL FUNCTIONS
  virtual void AddChild(wxObject *child);         // Adds reference to the child object
  virtual void RemoveChild(wxObject *child);   // Removes reference to child
                                       // (but doesn't delete the child object)
  virtual void DestroyChildren(void);  // Removes and destroys all children

  // Mode for manipulating the user interface
  virtual inline void SetUserEditMode(Bool edit) { editUIMode = edit; }
  virtual inline Bool GetUserEditMode(void) { return editUIMode; }

  virtual void SetEventHandler(wxEvtHandler *handler);
  virtual wxEvtHandler *GetEventHandler(void);

  Bool IsShown();
  void SetShown(Bool s);

  void ForEach(void (*foreach)(wxWindow *w, void *data), void *data);

  Bool GetsFocus();
};

extern wxChildList *wxGetTopLevelWindowsList(wxObject *);
#define wxTopLevelWindows(w) (wxGetTopLevelWindowsList(w))

extern wxWindow *wxGetModalWindow(wxObject*);
extern void wxPushModalWindow(wxObject*,wxWindow*);
extern void wxPopModalWindow(wxObject*,wxWindow*);

extern void *wxGetContextForFrame();

class wxRectangle: public wxObject
{
 public:
  int x;
  int y;
  int width;
  int height;
  inline wxRectangle(void) { x = 0; y = 0; width = 0; height = 0; }
};

#endif // IN_CPROTO
#endif // wxb_winh
