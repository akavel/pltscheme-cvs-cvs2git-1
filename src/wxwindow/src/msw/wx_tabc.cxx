/*
 * File:	wx_tabc.cc
 * Purpose:	Tab choice implementation
 * Author:	Matthew
 * Created:	2002
 * Copyright:	(c) 2002, PLT
 */

#include "wx.h"

#include "wx_tabc.h"

#include <commctrl.h>

BOOL wxTabChoice::MSWCommand(UINT WXUNUSED(param), WORD WXUNUSED(id))
{
  wxCommandEvent *event;
  event = new wxCommandEvent(wxEVENT_TYPE_TAB_CHOICE_COMMAND);
  ProcessCommand(event);
  return TRUE;
}


wxTabChoice::wxTabChoice(wxPanel *panel, wxFunction func, char *label,
			 int n, char **choices)
  : wxItem(panel)
{
  int x = 0, y = 0, i;
  wxWnd *cparent = NULL;
  TCITEM tie;
  INITCOMMONCONTROLSEX icex;

  icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
  icex.dwICC  = ICC_TAB_CLASSES;
  InitCommonControlsEx(&icex);

  panel->AddChild(this);
  wxWinType = wxTYPE_HWND;
  windowStyle = 0;
  cparent = (wxWnd *)panel->handle;

  panel->GetValidPosition(&x, &y);

  windows_id = (int)NewId(this);

  HWND hwndTab;
  int width, height;

  {
    int cx, cy;
    float current_width, cyf, total_width = 0;

    wxGetCharSize(cparent->handle, &cx, &cy, buttonFont);
    
    for (i = 0; i < n; i++) {
      GetTextExtent(wxStripMenuCodes(choices[i]), &current_width, &cyf, NULL, NULL, buttonFont);
      if (current_width < 40)
	current_width = 40;
      total_width += current_width + cy;
    }

    width = (int)total_width;
    height = 2 * cy;
  }

  hwndTab = CreateWindow(WC_TABCONTROL, "", 
			 WS_CHILD | WS_CLIPSIBLINGS,
			 0, 0, width, height,
			 cparent->handle, NULL, wxhInstance, NULL);
 
  // Add tabs for each day of the week. 
  tie.mask = TCIF_TEXT;
 
  for (i = 0; i < n; i++) { 
    tie.pszText = choices[i];
    TabCtrl_InsertItem(hwndTab, i, &tie);
  } 

  SubclassControl(hwndTab);

  ms_handle = (HANDLE)hwndTab;

  if (buttonFont) {
    HDC the_dc = GetWindowDC((HWND)ms_handle);
    if (buttonFont->GetInternalFont(the_dc))
      SendMessage((HWND)ms_handle,WM_SETFONT,
		  (WPARAM)buttonFont->GetInternalFont(the_dc),0L);
    ReleaseDC((HWND)ms_handle,the_dc);
  }

  RECT prc;
  prc.left = prc.top = prc.right = prc.bottom = 0;
  TabCtrl_AdjustRect(hwndTab, TRUE, &prc);
    
  SetSize(x, y, width, prc.bottom - prc.top);

  ShowWindow(hwndTab, SW_SHOW);
  panel->AdvanceCursor(this);
  Callback(func);
}
wxTabChoice::~wxTabChoice()
{
  
}

int wxTabChoice::GetSelection(void) { 
  return 0;
}
int wxTabChoice::Number(void) { 
  return 0;
}
void wxTabChoice::SetSelection(int n) { 
}
void wxTabChoice::Enable(Bool enable) { 
}

void wxTabChoice::SetSize(int x, int y, int width, int height, int WXUNUSED(sizeFlags))
{
  int currentX, currentY;
  GetPosition(&currentX, &currentY);
  if (x == -1)
    x = currentX;
  if (y == -1)
    y = currentY;

  char buf[300];

  float current_width;

  int cx;
  int cy;
  float cyf;

  if (width < 0)
    GetSize(&width, &height);

  MoveWindow((HWND)ms_handle, x, y, width, height, TRUE);

  OnSize(width, height);
}
