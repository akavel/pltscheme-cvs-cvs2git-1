/*
 * File:        wx_mbuf.cc
 * Purpose:     wxMediaBuffer implementation
 * Author:      Matthew Flatt
 * Created:     1995
 * Copyright:   (c) 1995-98, Matthew Flatt

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 */

#define  Uses_wxPrintSetup /* for wx_xt */
#include "wx_utils.h"
#include "wx_win.h"
#include "wx_dcps.h"
#ifndef OLD_WXWINDOWS
# include "wx_cmdlg.h"
#endif
#include "wx_print.h"
#ifdef wx_xt
# include "wx_types.h"
#endif

#include "wx_media.h"
#ifndef OLD_WXWINDOWS
# include "wx_clipb.h"
#else
# include "wx_gclip.h"
#endif
#include "wx_ptreq.h"
#include <ctype.h>
#include <string.h>

#define NUM_MAX_UNDOS 256

#if ALLOW_X_STYLE_SELECTION
Bool wxMediaXSelectionMode = TRUE;
wxMediaBuffer *wxMediaXSelectionOwner = NULL;
wxMediaBuffer *wxMediaXSelectionAllowed = NULL;
static Bool xSelectionCopied = FALSE;
static Bool xClipboardHack = FALSE;
#endif

static void InitCutNPaste(void);

static void MediaStyleNotify(wxStyle *which, wxMediaBuffer *media);

class wxMediaClipboardClient : public wxClipboardClient
{
 public:
  wxMediaClipboardClient();
  char *GetData(char *format, long *size);
  void BeingReplaced(void);
};
static wxMediaClipboardClient *TheMediaClipboardClient;

#if ALLOW_X_STYLE_SELECTION
class wxMediaXClipboardClient : public wxClipboardClient
{
 public:
  wxMediaXClipboardClient();
  char *GetData(char *format, long *size);
  void BeingReplaced(void);
};
static wxMediaXClipboardClient *TheMediaXClipboardClient;
#endif

# define MALLOC_CRP(n) new wxChangeRecord*[n]

// xformer doesn't handle static member variable declarations
#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif

wxBitmap *wxMediaBuffer::bitmap;
wxMemoryDC *wxMediaBuffer::offscreen = NULL;
long wxMediaBuffer::bmHeight, wxMediaBuffer::bmWidth;
Bool wxMediaBuffer::offscreenInUse = FALSE;
wxMediaBuffer *wxMediaBuffer::lastUsedOffscreen = NULL;

#ifdef MZ_PRECISE_GC
END_XFORM_SKIP;
#endif

typedef struct { short type; } Scheme_Object;
extern wxMediaBuffer *objscheme_unbundle_wxMediaBuffer(Scheme_Object *, const char*, int);
extern Scheme_Object *objscheme_bundle_wxMediaBuffer(wxMediaBuffer*);

/******************************************************************/

#ifndef EACH_BUFFER_OWN_OFFSCREEN
static int bcounter = 0;
#endif

wxMediaBuffer::wxMediaBuffer()
 : wxObject(WXGC_NO_CLEANUP)
{
  wxChangeRecord **crpa;

  map = new wxKeymap();
  // AddBufferFunctions(map);

  styleList = new wxStyleList;
  styleList->NewNamedStyle(STD_STYLE, NULL);
  notifyId = styleList->NotifyOnChange((wxStyleNotifyFunc)MediaStyleNotify, 
				       this, 1);

  filename = NULL;

  undomode = redomode = interceptmode = FALSE;
  maxUndos = NUM_MAX_UNDOS;
  crpa = MALLOC_CRP(maxUndos);
  changes = crpa;
  changes_start = changes_end = 0;
  crpa =  MALLOC_CRP(maxUndos);
  redochanges = crpa;
  redochanges_start = redochanges_end = 0;

  customCursor = NULL;

  loadoverwritesstyles = 1;

  noundomode = 0;

  ownCaret = FALSE;
  caretSnip = NULL; 

  pasteTextOnly = 0;

  InitCutNPaste();

  admin = NULL;

#ifdef EACH_BUFFER_OWN_OFFSCREEN
  offscreen = NULL;
#endif

  if (!offscreen) {
    wxREGGLOB(offscreen);
    wxREGGLOB(bitmap);
    wxREGGLOB(lastUsedOffscreen);
    bitmap = NULL;
    offscreen = new wxMemoryDC();
    bmHeight = bmWidth = 0;
#ifndef wx_mac
    offscreen->SetOptimization(TRUE);
#endif
  }

  inactiveCaretThreshold = wxSNIP_DRAW_SHOW_INACTIVE_CARET;

#ifndef EACH_BUFFER_OWN_OFFSCREEN
  bcounter++;
#endif
}

wxMediaBuffer::~wxMediaBuffer()
{
#if ALLOW_X_STYLE_SELECTION
  if (wxMediaXSelectionOwner == this)
    wxMediaXSelectionOwner = NULL;
#endif

  if (map)
    SetKeymap(NULL);

  styleList->ForgetNotification(notifyId);

#ifndef EACH_BUFFER_OWN_OFFSCREEN
  --bcounter;
  if (!bcounter) {
#endif
    offscreen->SelectObject(NULL);
    DELETE_OBJ offscreen;
    offscreen = NULL;
    if (bitmap)
      DELETE_OBJ bitmap;
#ifndef EACH_BUFFER_OWN_OFFSCREEN
  }
#endif

  ClearUndos();
}

/******************************************************************/

void wxMediaBuffer::OnLocalEvent(wxMouseEvent *event)
{
  if (map) {
    Scheme_Object *edit;
    edit = objscheme_bundle_wxMediaBuffer(this);
    if (map->HandleMouseEvent(edit, event))
      return;
    else if (!event->Moving())
      map->BreakSequence();
  }

  OnDefaultEvent(event);
}

void wxMediaBuffer::OnLocalChar(wxKeyEvent *event)
{
  if (map) {
    Scheme_Object *edit;
    edit = objscheme_bundle_wxMediaBuffer(this);
    if (map->HandleKeyEvent(edit, event))
      return;
    else
      map->BreakSequence();
  }

  OnDefaultChar(event);
}

void wxMediaBuffer::OnFocus(Bool WXUNUSED(on))
{
}

/******************************************************************/

void wxMediaBuffer::SetAdmin(wxMediaAdmin *administrator)
{
  SettingAdmin(administrator);

  admin = administrator;
  if (!admin)
    ownCaret = FALSE;

  if (admin)
    InitNewAdmin();
}

void wxMediaBuffer::SettingAdmin(wxMediaAdmin *)
{
}

void wxMediaBuffer::InitNewAdmin()
{
}

wxMediaAdmin *wxMediaBuffer::GetAdmin(void)
{
  return admin;
}

/******************************************************************/

#define ShowsGhostCaret() 1

Bool wxMediaBuffer::DoOwnCaret(Bool ownit)
{
  Bool refresh;

  refresh = (!caretSnip && (((Bool)ownCaret != ownit) || ShowsGhostCaret()));

  ownCaret = ownit;
  if (caretSnip)
    caretSnip->OwnCaret(ownit);
  if (map && !ownit && refresh)
    map->BreakSequence();

#if ALLOW_X_STYLE_SELECTION
  if (ownit && !caretSnip)
    wxMediaXSelectionAllowed = this;
  else if (wxMediaXSelectionAllowed == this)
    wxMediaXSelectionAllowed = NULL;
#endif

  if (admin)
    admin->UpdateCursor();

  return refresh;
}

wxDC *wxMediaBuffer::GetDC()
{
  /* This can be called by snips to get a DC appropriate for
     sizing text, etc., outside of draws. It isn't the destination 
     for draws, though. */
  if (admin)
    return admin->GetDC(NULL, NULL);
  else
    return NULL;
}

void wxMediaBuffer::GetViewSize(float *w, float *h)
{
  if (admin)
    admin->GetView(NULL, NULL, w, h);
  else {
    if (w)
      *w = 0;
    if (h)
      *h = 0;
  }
}

Bool wxMediaBuffer::DoSetCaretOwner(wxSnip *snip, int dist)
{
  Bool hadCaret, visCaret;
  wxSnip *oldCaret;
  Bool refresh;

  if (PTREQ(snip, caretSnip)) {
    if (!admin || (dist == wxFOCUS_IMMEDIATE))
      return FALSE;
    admin->GrabCaret(dist);
  }

  refresh = FALSE;

  visCaret = ownCaret || ShowsGhostCaret();

  if (!snip || !(snip->flags & wxSNIP_HANDLES_EVENTS)) {
    wxSnip *_oldCaret = caretSnip;  
    caretSnip = NULL;
    if (_oldCaret) {
      _oldCaret->OwnCaret(FALSE);
      if (visCaret)
	refresh = TRUE;
    }
#if ALLOW_X_STYLE_SELECTION
    wxMediaXSelectionAllowed = this;
#endif
    if (admin)
      admin->UpdateCursor();
    return refresh;
  }

  if (!GetSnipLocation(snip, NULL, NULL))
    return refresh;

  if (!ownCaret)
    hadCaret = FALSE;
  else
    hadCaret = !caretSnip;

  oldCaret = caretSnip;
  caretSnip = snip;

  BeginEditSequence();
  if (oldCaret)
    oldCaret->OwnCaret(FALSE);
  else if (visCaret)
    refresh = TRUE;
  snip->OwnCaret(ownCaret);
  EndEditSequence();

  if (admin && (dist != wxFOCUS_IMMEDIATE))
    admin->GrabCaret(dist);

  if (admin)
    admin->UpdateCursor();

  return refresh;
}

static void ConvertCoords(wxMediaAdmin *admin, float *x, float *y, int toLocal)
{
  float lx = 0, ly = 0;

  if (admin) {
    if (admin->__type == wxTYPE_MEDIA_SNIP_MEDIA_ADMIN) {
      wxMediaSnip *snip;
      wxSnipAdmin *sa;

      snip = ((wxMediaSnipMediaAdmin *)admin)->GetSnip();
      sa = snip->GetAdmin();

      if (sa) {
	wxMediaBuffer *mbuf;
	mbuf = sa->GetMedia();
	if (mbuf) {
	  float bx = 0, by = 0;
	  int l, t, r, b;
	  
	  mbuf->LocalToGlobal(&bx, &by);
	  mbuf->GetSnipLocation(snip, &lx, &ly, 0);
	  lx += bx;
	  ly += by;

	  snip->GetMargin(&l, &t, &r, &b);
	  lx += l;
	  ly += t;
	}
      }
    } else {
      admin->GetDC(&lx, &ly);
      lx = -lx;
      ly = -ly;
    }
  }

  if (toLocal) {
    if (x)
      *x -= lx;
    if (y)
      *y -= ly;
  } else {
    if (x)
      *x += lx;
    if (y)
      *y += ly;
  }
}

void wxMediaBuffer::GlobalToLocal(float *x, float *y)
{
  ConvertCoords(admin, x, y, 1);
}

void wxMediaBuffer::LocalToGlobal(float *x, float *y)
{
  ConvertCoords(admin, x, y, 0);
}

void wxMediaBuffer::SetCursor(wxCursor *c, Bool override)
{  
  customCursor = c;
  customCursorOverrides = override;

  if (admin)
    admin->UpdateCursor();
}

/******************************************************************/

#ifndef ROUND
#include <math.h>
#define ROUND(x) (int)floor(x)
#endif

#define REDICULOUS_SIZE 2000

Bool wxMediaBuffer::ReadyOffscreen(float width, float height)
{
  if ((width > REDICULOUS_SIZE)
      || (height > REDICULOUS_SIZE))
    return FALSE;

  if (!offscreenInUse && (height > bmHeight || width > bmWidth)) {
    wxBitmap *oldbm = bitmap;

    bmWidth = ROUND(width);
    bmHeight = ROUND(height);

    bitmap = new wxBitmap(bmWidth, bmHeight);

    offscreen->SelectObject(NULL);
    if (oldbm)
      DELETE_OBJ oldbm;
  
    if (bitmap->Ok())
      offscreen->SelectObject(bitmap);

    return TRUE;
  } 
  
  return FALSE;
}

/******************************************************************/

void wxMediaBuffer::SetKeymap(wxKeymap *keymap)
{
  map = keymap;
}

wxKeymap *wxMediaBuffer::GetKeymap(void)
{
  return map;
}

wxStyleList *wxMediaBuffer::GetStyleList(void)
{
  return styleList;
}

void wxMediaBuffer::SetStyleList(wxStyleList *newList)
{
  styleList->ForgetNotification(notifyId);
  notifyId = newList->NotifyOnChange((wxStyleNotifyFunc)MediaStyleNotify, 
				     this, 1);
  styleList = newList;

  if (!styleList->FindNamedStyle(STD_STYLE))
    styleList->NewNamedStyle(STD_STYLE, NULL);
}

static void MediaStyleNotify(wxStyle *which, wxMediaBuffer *media)
{
  if (media)
    media->StyleHasChanged(which);
}

/******************************************************************/

int wxMediaBuffer::AppendEditItems(wxMenu *, int)
{
  return 0;
}

void wxMediaBuffer::DoEdit(int op, Bool recursive, long time)
{  
  if (recursive && caretSnip) {
    caretSnip->DoEdit(op, TRUE, time);
    return;
  }

  switch(op) {
  case wxEDIT_UNDO:
    Undo();
    break;
  case wxEDIT_REDO:
    Redo();
    break;
  case wxEDIT_CLEAR:
    Clear();
    break;
  case wxEDIT_CUT:
    Cut(FALSE, time);
    break;
  case wxEDIT_COPY:
    Copy(FALSE, time);
    break;
  case wxEDIT_PASTE:
    Paste(time);
    break;
  case wxEDIT_KILL:
    Kill(time);
    break;
  case wxEDIT_INSERT_TEXT_BOX:
    InsertBox(wxEDIT_BUFFER);
    break;
  case wxEDIT_INSERT_GRAPHIC_BOX:
    InsertBox(wxPASTEBOARD_BUFFER);
    break;
  case wxEDIT_INSERT_IMAGE:
    InsertImage();
    break;
  case wxEDIT_SELECT_ALL:
    SelectAll();
    break;
  }
}


Bool wxMediaBuffer::CanEdit(int op, Bool recursive)
{ 
  if (recursive && caretSnip) {
    return caretSnip->CanEdit(op, TRUE);
  }

  if (IsLocked()) {
    if ((op != wxEDIT_COPY) && (op != wxEDIT_SELECT_ALL))
      return FALSE;
  }

  switch(op) {
  case wxEDIT_UNDO:
    if (changes_start == changes_end)
      return FALSE;
    break;
  case wxEDIT_REDO:
    if (redochanges_start == redochanges_end)
      return FALSE;
    break;
  }

  return ReallyCanEdit(op);
}

int wxMediaBuffer::AppendFontItems(wxMenu *, int)
{
  return 0;
}

void wxMediaBuffer::DoFont(int, Bool)
{
  return;
}

void wxMediaBuffer::InsertBox(int type)
{
  wxSnip *snip;

  snip = OnNewBox(type);
  if (!snip)
    return;

  BeginEditSequence();
  snip->style = styleList->FindNamedStyle(STD_STYLE);
  if (!snip->style) {
    wxStyle *bs;
    bs = styleList->BasicStyle();
    snip->style = bs;
  }
  Insert(snip);
  SetCaretOwner(snip);
  EndEditSequence();
}

wxSnip *wxMediaBuffer::OnNewBox(int type)
{
  wxMediaSnip *snip;
  wxMediaBuffer *media;

  if (type == wxEDIT_BUFFER)
    media = new wxMediaEdit();
  else
    media = new wxMediaPasteboard();
  snip = new wxMediaSnip(media);

  media->SetKeymap(map);  
  media->SetStyleList(styleList);

  return snip;
}

void wxMediaBuffer::InsertImage(char *filename, long type, Bool relative, Bool inlineImg)
{
  wxImageSnip *snip;
      
  if (!filename) {
    filename = GetFile(NULL);
  }
  
  if (!filename)
    return;

  snip = OnNewImageSnip(filename, type, relative, inlineImg);
  Insert(snip);
}

wxImageSnip *wxMediaBuffer::OnNewImageSnip(char *filename, long type, 
					   Bool relative, Bool inlineImg)
{
  return new wxImageSnip(filename, type, relative, inlineImg);
}

/**********************************************************************/

wxBufferData *wxMediaBuffer::GetSnipData(wxSnip *)
{
  return NULL;
}

void wxMediaBuffer::SetSnipData(wxSnip *, wxBufferData *)
{
}

/**********************************************************************/

Bool wxReadMediaGlobalHeader(wxMediaStreamIn *f)
{
  f->scl->ResetHeaderFlags(f);
  if (!f->scl->Read(f))
    return FALSE;

  wxmbSetupStyleReadsWrites(f);

  return f->bdl->Read(f);
}

Bool wxReadMediaGlobalFooter(wxMediaStreamIn *f)
{
  wxmbDoneStyleReadsWrites(f);
  f->scl->ResetHeaderFlags(f);

  return TRUE;
}

Bool wxWriteMediaGlobalHeader(wxMediaStreamOut *f)
{
  f->scl->ResetHeaderFlags(f);
  if (!f->scl->Write(f))
    return FALSE;

  wxmbSetupStyleReadsWrites(f);

  return f->bdl->Write(f);
}

Bool wxWriteMediaGlobalFooter(wxMediaStreamOut *f)
{
  wxmbDoneStyleReadsWrites(f);
  f->scl->ResetHeaderFlags(f);

  return TRUE;
}

/**********************************************************************/

int wxmeCheckFormatAndVersion(wxMediaStream *s)
{
  if (strcmp(s->read_format, MRED_FORMAT_STR)) {
    wxmeError("Unknown format number.");
    return 0;
  }
  if (strcmp(s->read_version, MRED_VERSION_STR)
      && strcmp(s->read_version, "01")
      && strcmp(s->read_version, "02")) {
    wxmeError("Unknown version number.");
    return 0;
  }

  return 1;
}

Bool wxMediaBuffer::ReadHeaderFromFile(wxMediaStreamIn *, char *headerName)
{
  char buffer[256];

  sprintf(buffer, "Unknown header data: \"%.100s\"."
	  " The file will be loaded anyway.", headerName);

  wxmeError(buffer);

  return TRUE;
}

Bool wxMediaBuffer::ReadFooterFromFile(wxMediaStreamIn *, char *headerName)
{
  char buffer[256];

  sprintf(buffer, "Unknown header data: \"%.100s\"."
	  " The file will be loaded anyway.", headerName);

  wxmeError(buffer);

  return TRUE;
}

Bool wxMediaBuffer::WriteHeadersToFile(wxMediaStreamOut *WXUNUSED(f))
{
  return TRUE;
}

Bool wxMediaBuffer::WriteFootersToFile(wxMediaStreamOut *WXUNUSED(f))
{
  return TRUE;
}

Bool wxMediaBuffer::BeginWriteHeaderFooterToFile(wxMediaStreamOut *f, 
						 char *headerName,
						 long *dataBuffer)
{
  long db;
  db = f->Tell();
  *dataBuffer = db;
  f->PutFixed(0);
  f->Put(headerName);

  return TRUE;
}

Bool wxMediaBuffer::EndWriteHeaderFooterToFile(wxMediaStreamOut *f, 
					       long dataBuffer)
{
  long end, pos;

  end = f->Tell();

  f->JumpTo(dataBuffer);
  f->PutFixed(0);
  pos = f->Tell();
  
  f->JumpTo(dataBuffer);
  f->PutFixed(end - pos);

  f->JumpTo(end);

  numExtraHeaders++;
  
  return TRUE;
}

Bool wxMediaBuffer::ReadHeadersFooters(wxMediaStreamIn *f, Bool headers)
{
  char headerName[256];
  long len, hlen, i, pos, numHeaders;

  f->GetFixed(&numHeaders);
#if 0
  if (numHeaders > 1000) {
    if (wxMessageBox("File contains suspiciously large value for special"
		     " headers/footers. Give up?", "Warning",
		     wxYES_NO | wxCENTRE) == wxYES)
      return FALSE;
  }
#endif

  for (i = 0; i < numHeaders; i++) {
    f->GetFixed(&len);
    if (!f->Ok())
      return FALSE;
    if (len) {
      pos = f->Tell();
      f->SetBoundary(len);
      hlen = 256;
      f->Get((long *)&hlen, (char *)headerName);
      if (headers) {
	if (!ReadHeaderFromFile(f, headerName))
	  return FALSE;
      } else {
	if (!ReadFooterFromFile(f, headerName))
	  return FALSE;
      }
      if (!f->Ok())
	return FALSE;
      f->RemoveBoundary();
      if (len -= (f->Tell() - pos))
	f->Skip(len);
      if (!f->Ok())
	return FALSE;
    }
  }

  return TRUE;
}

Bool wxMediaBuffer::DoWriteHeadersFooters(wxMediaStreamOut *f, Bool headers)
{
  long allStart, allEnd;

  allStart = f->Tell();
  f->PutFixed(0);
  numExtraHeaders = 0;

  if (headers) {
    if (!WriteHeadersToFile(f))
      return FALSE;
  } else {
    if (!WriteFootersToFile(f))
      return FALSE;
  }

  if (numExtraHeaders) {
    allEnd = f->Tell();
    f->JumpTo(allStart);
    f->PutFixed(numExtraHeaders);
    f->JumpTo(allEnd);
  }

  return TRUE;
}

/**********************************************************************/

static wxBufferData *ReadBufferData(wxMediaStreamIn *f)
{
  wxBufferData *data, *newdata;
  wxBufferDataClass *dclass;
  int extraDataIndex;
  long datalen;

  data = NULL;
  do {
    f->Get(&extraDataIndex);
    if (extraDataIndex) {
      dclass = f->bdl->FindByMapPosition(f, extraDataIndex);
      
      if (!dclass || !dclass->required)
	f->Get(&datalen);
      else
	datalen = -1;
      
      if (dclass) {
	long start;
	start = f->Tell();
	if (datalen >= 0)
	  f->SetBoundary(datalen);
	if (!(newdata = dclass->Read(f)))
	  return FALSE;
	newdata->next = data;
	data = newdata;
	if (datalen >= 0) {
	  long rcount;
	  rcount = f->Tell() - start;
	  if (rcount < datalen) {
	    wxmeError("Warning: underread caused by file "
		      "corruption or unknown internal error.");
	    f->Skip(datalen - rcount);
	  }
	  f->RemoveBoundary();
	}
      } else {
	/* Unknown extra data */
	f->Skip(datalen);
      }
      if (!f->Ok())
	return FALSE;
    }
  } while (extraDataIndex);

  return data;
}

Bool wxMediaBuffer::ReadSnipsFromFile(wxMediaStreamIn *f, Bool overwritestylename)
{
  long len, numHeaders, numSnips, i, listId;
  int styleIndex;
  short n;
  wxStyleList *newList;
  wxSnipClass *sclass;
  wxBufferData *data;
  wxSnip *snip;

  if (!ReadHeadersFooters(f, TRUE))
    return FALSE;

  if (!(newList = wxmbReadStylesFromFile(styleList, f, overwritestylename, &listId)))
    return FALSE;

  if (PTRNE(newList, styleList))
    SetStyleList(newList);
  
  f->GetFixed(&numHeaders);
  
#if 0
  if (numHeaders > 100) {
    if (wxMessageBox("File contains suspiciously large value for class"
		     " headers. Give up?", "Warning",
		     wxYES_NO | wxCENTRE) == wxYES)
      return FALSE;
  }
#endif

  for (i = 0; i < numHeaders; i++) {
    f->Get(&n);
    f->GetFixed(&len);
    if (!f->Ok())
      return FALSE;
    if (len) {
      sclass = f->scl->FindByMapPosition(f, n);
      if (sclass) {
	long start, rcount;
	start = f->Tell();

	f->SetBoundary(len);
	if (!sclass->ReadHeader(f))
	  return FALSE;
	if (!f->Ok())
	  return FALSE;
	f->SetHeaderFlag(sclass);

	rcount = f->Tell() - start;
	if (rcount < len) {
	  wxmeError("Warning: underread caused by file "
		    "corruption or unknown internal error.");
	  f->Skip(len - rcount);
	}
	
	f->RemoveBoundary();
      } else {
	f->Skip(len);
      }
      if (!f->Ok())
	return FALSE;
    }
  }

  f->Get(&numSnips);

  for (i = 0; i < numSnips; i++) {
    f->Get(&n);
    if (n >= 0) {
      sclass = f->scl->FindByMapPosition(f, n);
    } else
      sclass = NULL; /* -1 => unknown */
    if (!sclass || !sclass->required)
      f->GetFixed(&len);
    else
      len = -1;
    if (!f->Ok())
      return FALSE;
    if (len) {
      if (sclass) {
	long start;
	start = f->Tell();

	if (len >= 0)
	  f->SetBoundary(len);
	f->Get(&styleIndex);
	if ((snip = sclass->Read(f))) {
	  if (snip->flags & wxSNIP_OWNED)
	    snip->flags -= wxSNIP_OWNED;
	  snip->style = styleList->MapIndexToStyle(f, styleIndex, listId);
	  if (!snip->style) {
	    wxStyle *bs;
	    bs = styleList->BasicStyle();
	    snip->style = bs;
	  }
	  if (!ReadInsert(snip))
	    return FALSE;
	} else
	  return FALSE;

	data = ReadBufferData(f);
	if (!f->Ok())
	  return FALSE;

	if (data)
	  SetSnipData(snip, data);

	if (len >= 0) {
	  long rcount;
	  rcount = f->Tell() - start;
	  if (rcount < len) {
	    wxmeError("Warning: underread caused by file "
		      "corruption or unknown internal error.");
	    f->Skip(len - rcount);
	  }
	  f->RemoveBoundary();
	}
      } else {
	f->Skip(len);
      }
      if (!f->Ok())
	return FALSE;
    }
  }

  if (!ReadHeadersFooters(f, FALSE))
    return FALSE;

  return TRUE;
}

Bool wxmbWriteBufferData(wxMediaStreamOut *f, wxBufferData *data)
{
  long dataPos = 0, dataStart = 0, dataEnd;
  
  while (data) {
    short mp;
    mp = (short)f->MapPosition(data->dataclass);
    f->Put(mp);
    
    if (!data->dataclass->required) {
      dataStart = f->Tell();
      f->PutFixed(0);
      dataPos = f->Tell();
    }

    if (!data->Write(f))
      return FALSE;
    
    if (!data->dataclass->required) {
      dataEnd = f->Tell();
      f->JumpTo(dataStart);
      f->PutFixed(dataEnd - dataPos);
      f->JumpTo(dataEnd);
    }
    data = data->next;
  }

  f->Put(0);

  return TRUE;
}

Bool wxmbWriteSnipsToFile(wxMediaStreamOut *f, 
			  wxStyleList *styleList,
			  wxList *snipList, 
			  wxSnip *startSnip, wxSnip *endSnip,
			  wxList *extraData,
			  wxMediaBuffer *buffer)
{
  long allStart, allEnd, headerPos, headerStart, headerEnd;
  long snipCount, snipPos = 0, snipStart = 0, snipEnd;
  long numHeaders;
  int styleIndex;
  wxNode *node = NULL, *node2;
  wxSnip *snip;
  wxSnipClass *sclass;
  wxBufferData *data;

  if (!wxmbWriteStylesToFile(styleList, f))
    return FALSE;
  
  allStart = f->Tell();
  f->PutFixed(0);

  if (snipList) {
    node = snipList->First();
    if (!node)
      return FALSE;
    startSnip = (wxSnip *)node->Data();
    endSnip = NULL;
  } else
    node = NULL;

  numHeaders = 0;
  snipCount = 0;
  for (snip = startSnip; PTRNE(snip, endSnip); snipCount++) {
    sclass = snip->snipclass;
    if (!sclass) {
      wxmeError("There's a snip without a class."
		" Data will be lost.");
    } else if (!f->GetHeaderFlag(sclass)) {
      short mp;
      mp = (short)f->MapPosition(sclass);
      f->Put(mp);
      headerStart = f->Tell();
      f->PutFixed(0);
      headerPos = f->Tell();
      if (!sclass->WriteHeader(f))
	return FALSE;
      f->SetHeaderFlag(sclass);
      headerEnd = f->Tell();
      f->JumpTo(headerStart);
      f->PutFixed(headerEnd - headerPos);
      f->JumpTo(headerEnd);
      numHeaders++;
      if (!f->Ok())
	return FALSE;
    }

    if (snipList) {
      node = node->Next();
      if (node)
	snip = (wxSnip *)node->Data();
      else
	snip = NULL;
    } else
      snip = snip->next;
  }

  allEnd = f->Tell();
  f->JumpTo(allStart);
  f->PutFixed(numHeaders);
  f->JumpTo(allEnd);

  f->Put(snipCount);

  if (snipList)
    node = snipList->First();
  else
    node = NULL;
  if (extraData)
    node2 = extraData->First();
  else
    node2 = NULL;

  for (snip = startSnip; PTRNE(snip, endSnip); ) {
    sclass = snip->snipclass;

    if (sclass) {
      short mp;
      mp = (short)f->MapPosition(sclass);
      f->Put(mp);
    } else
      f->Put((short)(-1));

    if (!snip->snipclass || !snip->snipclass->required) {
      snipStart = f->Tell();
      f->PutFixed(0);
      snipPos = f->Tell();
    }

    styleIndex = styleList->StyleToIndex(snip->style);
    if (styleIndex < 0) {
      wxmeError("Bad style discovered.");
      styleIndex = 0;
    }
    f->Put(styleIndex);

    snip->Write(f);
    if (node2)
      data = (wxBufferData *)node2->Data();
    else
      data = buffer->GetSnipData(snip);

    if (!wxmbWriteBufferData(f, data))
      return FALSE;

    if (!snip->snipclass || !snip->snipclass->required) {
      snipEnd = f->Tell();
      f->JumpTo(snipStart);
      f->PutFixed(snipEnd - snipPos);
      f->JumpTo(snipEnd);
    }

    if (!f->Ok())
      return FALSE;

    if (snipList) {
      node = node->Next();
      if (node)
	snip = (wxSnip *)node->Data();
      else
	snip = NULL;
    } else
      snip = snip->next;
    if (extraData)
      node2 = node2->Next();
  }

  return TRUE;
}

char *wxMediaBuffer::GetFilename(Bool *temp)
{
  if (temp)
    *temp = tempFilename;
  return filename;
}

wxWindow *wxMediaBuffer::ExtractParent(void)
{
  if (admin && (admin->standard > 0)) {
    wxWindow *w;
    
    w = ((wxCanvasMediaAdmin *)admin)->GetCanvas();
    
    while (w && !wxSubType(w->__type, wxTYPE_FRAME)
	   && !wxSubType(w->__type, wxTYPE_DIALOG_BOX)) {
      w = w->GetParent();
    }
    
    return w;
  } else
    return NULL;
}

#ifndef wx_xt
class wxMediaPrintout : public wxPrintout
{
private:
  wxMediaBuffer *b;
  void *data;

  Bool fitToPage;

public:
  wxMediaPrintout(wxMediaBuffer *buffer, Bool fit);

  Bool HasPage(int page);
  Bool OnPrintPage(int page);
  Bool OnBeginDocument(int startPage, int endPage);
  void OnEndDocument();
};

wxMediaPrintout::wxMediaPrintout(wxMediaBuffer *buffer, Bool fit)
: wxPrintout()
{
  b = buffer;
  fitToPage = fit;
}

Bool wxMediaPrintout::HasPage(int page)
{
  return b->HasPrintPage(GetDC(), page);
}

Bool wxMediaPrintout::OnPrintPage(int page)
{
  b->PrintToDC(GetDC(), page);
  
  return TRUE;
}

Bool wxMediaPrintout::OnBeginDocument(int startPage, int endPage)
{
  if (wxPrintout::OnBeginDocument(startPage, endPage)) {
    b->printing = GetDC();
    data = b->BeginPrint(b->printing, fitToPage);
    return TRUE;
  } else
    return FALSE;
}

void wxMediaPrintout::OnEndDocument()
{
  wxDC *pr = b->printing;
  b->printing = NULL;
  b->EndPrint(pr, data);
  wxPrintout::OnEndDocument();
  b->InvalidateBitmapCache(0, 0, -1, -1);
}
#endif

#ifdef wx_x
# define WXUNUSED_X(x) /* empty */
#else
# define WXUNUSED_X(x) x
#endif

void wxMediaBuffer::Print(Bool interactive, Bool fitToPage, int WXUNUSED_X(output_mode), wxWindow *parent)
{
  int ps;

#ifndef wx_x
  ps = (output_mode == 1);
#else
  ps = 1;
#endif

  if (!parent) {
    parent = ExtractParent();
  }

  if (ps) {
    wxDC *dc;
    void *data;
    
    dc = new wxPostScriptDC(interactive, parent);

    if (dc->Ok()) { 
      dc->StartDoc("Printing buffer");

      printing = dc;
      data = BeginPrint(dc, fitToPage);

      PrintToDC(dc);

      printing = NULL;
      EndPrint(dc, data);

      dc->EndDoc();

      InvalidateBitmapCache(0, 0, -1, -1);
    }

    DELETE_OBJ dc;
    
    return;
  } 

#ifndef wx_x
  wxPrinter *p = new wxPrinter();
  wxPrintout *o = new wxMediaPrintout(this, fitToPage);
  
  p->Print(parent, o, interactive);

  DELETE_OBJ o;
  DELETE_OBJ p;
#endif
}

/****************************************************************/

void wxMediaBuffer::Undo(void)
{
  if (!undomode && !redomode) {
    undomode = TRUE;
    
    PerformUndos(changes, FALSE);
    
    undomode = FALSE;
  }
}

void wxMediaBuffer::Redo(void)
{ 
  if (!undomode && !redomode) {
    redomode = TRUE;
    
    PerformUndos(redochanges, TRUE);
    
    redomode = FALSE;
  }
}

#define delete_cgrec(x)  DELETE_OBJ (x)

static void wxmeClearUndos(wxChangeRecord **changes, int start, int end,
			   int maxUndos)
{
  int i;

  for (i = start; i != end; i = (i + 1) % maxUndos) {
    delete_cgrec(changes[i]);
    changes[i] = NULL;
  }
}

void wxMediaBuffer::AddUndo(wxChangeRecord *rec)
{
  if (interceptmode)
    intercepted->Append((wxObject *)rec);
  else if (undomode)
    AppendUndo(rec, redochanges, TRUE);
  else if (!noundomode) {
    if (!redomode) {
      wxmeClearUndos(redochanges, redochanges_start,
		     redochanges_end, maxUndos);
      redochanges_start = redochanges_end = 0;
    }
    AppendUndo(rec, changes, FALSE);
  } else
    delete_cgrec(rec);
}

void wxMediaBuffer::AddSchemeUndo(void *proc)
{
  wxSchemeModifyRecord *modrec;
  modrec = new wxSchemeModifyRecord(proc);
  AddUndo(modrec);
}

void wxMediaBuffer::AppendUndo(wxChangeRecord *rec, wxChangeRecord **changes, 
			       Bool redos)
{
  if (maxUndos) {
    int start, end;

    if (redos) {
      start = redochanges_start;
      end = redochanges_end;
    } else {
      start = changes_start;
      end = changes_end;
    }

    changes[end] = rec;
    end = (end + 1) % maxUndos;
    if (end == start) {
      delete_cgrec(changes[start]);
      changes[start] = NULL;
      start = (start + 1) % maxUndos;
    }
    
    if (redos) {
      redochanges_start = start;
      redochanges_end = end;
    } else {
      changes_start = start;
      changes_end = end;
    }
  } else
    delete_cgrec(rec);
}

void wxMediaBuffer::PerformUndos(wxChangeRecord **changes, Bool redos)
{
  wxChangeRecord *rec;
  Bool cont;
  int start, end;
  
  BeginEditSequence();

  if (redos) {
    start = redochanges_start;
    end = redochanges_end;
  } else {
    start = changes_start;
    end = changes_end;
  }

  while (start != end) {
    end = (end - 1 + maxUndos) % maxUndos;
    rec = changes[end];
    changes[end] = NULL;

    if (redos) {
      redochanges_start = start;
      redochanges_end = end;
    } else {
      changes_start = start;
      changes_end = end;
    }
    
    cont = rec->Undo(this);
    delete_cgrec(rec);
    if (!cont)
      break;
  }

  EndEditSequence();
}

void wxMediaBuffer::PerformUndoList(wxList *changes)
{
  wxNode *node;
  wxChangeRecord *rec;
  Bool cont = FALSE;
  
  BeginEditSequence();
  do {
    node = changes->Last();
    if (node) {
      rec = (wxChangeRecord *)node->Data();
      cont = rec->Undo(this);
      delete_cgrec(rec);
      changes->DeleteNode(node);
    }
  } while (node && cont);
  EndEditSequence();
}

void wxMediaBuffer::ClearUndos()
{
  wxmeClearUndos(changes, changes_start, changes_end, maxUndos);
  changes_start = changes_end = 0;
  wxmeClearUndos(redochanges, redochanges_start, redochanges_end, maxUndos);
  redochanges_start = redochanges_end = 0;
}

void wxMediaBuffer::SetMaxUndoHistory(int v)
{
  wxChangeRecord **naya;
  int i, j;

  if (undomode || redomode || (v == maxUndos))
    return;

  naya = MALLOC_CRP(v);
  for (j = 0, i = changes_start; 
       (i != changes_end) && (j < v); 
       j++, i = (i + 1) % maxUndos) {
    naya[j] = changes[i];
  }
  for (; i != changes_end; i = (i + 1) % maxUndos) {
    delete_cgrec(changes[i]);
  }
  changes = naya;
  changes_start = 0;
  changes_end = v ? (j % v) : 0;

  naya = MALLOC_CRP(v);
  for (j = 0, i = redochanges_start; 
       (i != redochanges_end) && (j < v); 
       j++, i = (i + 1) % maxUndos) {
    naya[j] = redochanges[i];
  }
  for (; i != redochanges_end; i = (i + 1) % maxUndos) {
    delete_cgrec(redochanges[i]);
  }
  redochanges = naya;
  redochanges_start = 0;
  redochanges_end = v ? (j % v) : v;

  maxUndos = v;
}

int wxMediaBuffer::GetMaxUndoHistory()
{
  return maxUndos;
}

/****************************************************************/

/* Copy and the copy ring: the current clipboard content is stored in
   wxmb_commonCopyBuffer, etc. To implement the copy ring, then when a
   copy is started, we moved the wxmb_commonCopyBuffer, etc. values
   into a copy ring. Yanking from the ring swaps the values in
   wxmb_commonCopyBuffer, etc.  and the ring values and adjust the
   pointer into the ring. */

static int copyDepth = 0;

static int copyRingSize = 30;
static int copyRingPos = 0, copyRingMax = 0, copyRingDest = 0;

static wxList **copyRingBuffer1, **copyRingBuffer2;
static wxStyleList **copyRingStyle;
static wxBufferData **copyRingData;

wxList *wxmb_commonCopyBuffer = NULL;
wxList *wxmb_commonCopyBuffer2 = NULL;
wxStyleList *wxmb_copyStyleList = NULL;
wxBufferData *wxmb_commonCopyRegionData = NULL;

static int copyingSelf;

static void InitCutNPaste()
{
  if (!copyRingBuffer1) {
    wxREGGLOB(copyRingBuffer1);
    wxREGGLOB(copyRingBuffer2);
    wxREGGLOB(copyRingStyle);
    wxREGGLOB(copyRingData);

    copyRingBuffer1 = new wxList*[copyRingSize];
    copyRingBuffer2 = new wxList*[copyRingSize];
    copyRingStyle = new wxStyleList*[copyRingSize];
    copyRingData = new wxBufferData*[copyRingSize];

    copyRingMax = 1;
    copyRingDest = 1;

    wxREGGLOB(wxmb_commonCopyBuffer);
    wxREGGLOB(wxmb_commonCopyBuffer2);
    wxmb_commonCopyBuffer = new wxList(wxKEY_NONE, FALSE);
    wxmb_commonCopyBuffer2 = new wxList(wxKEY_NONE, FALSE);

    wxREGGLOB(wxmb_copyStyleList);
    wxREGGLOB(wxmb_commonCopyRegionData);
  }

  if (!TheMediaClipboardClient) {
    wxREGGLOB(TheMediaClipboardClient);
    TheMediaClipboardClient = new wxMediaClipboardClient;
#if ALLOW_X_STYLE_SELECTION
    wxREGGLOB(TheMediaXClipboardClient);
    wxREGGLOB(wxMediaXSelectionOwner);
    wxREGGLOB(wxMediaXSelectionAllowed);
    TheMediaXClipboardClient = new wxMediaXClipboardClient;
#endif
  }
}

void wxMediaBuffer::CopyRingNext(void)
{
  copyRingBuffer1[copyRingPos] = wxmb_commonCopyBuffer;
  copyRingBuffer2[copyRingPos] =  wxmb_commonCopyBuffer2;
  copyRingData[copyRingPos] = wxmb_commonCopyRegionData;
  copyRingStyle[copyRingPos] = wxmb_copyStyleList;

  copyRingPos = copyRingPos - 1;
  if (copyRingPos < 0)
    copyRingPos = copyRingMax - 1;
  
  wxmb_commonCopyBuffer = copyRingBuffer1[copyRingPos];
  wxmb_commonCopyBuffer2 = copyRingBuffer2[copyRingPos];
  wxmb_commonCopyRegionData = copyRingData[copyRingPos];
  wxmb_copyStyleList = copyRingStyle[copyRingPos];
}

void wxMediaBuffer::BeginCopyBuffer(void)
{
  copyDepth++;
}

void wxMediaBuffer::EndCopyBuffer(void)
{
  --copyDepth;
}

void wxMediaBuffer::FreeOldCopies(void)
{
  if (!wxmb_copyStyleList)
    return;

  if (copyDepth > 1) {
    /* Delete current "ring" occupant: */
    wxmb_commonCopyBuffer->DeleteContents(TRUE);
    DELETE_OBJ wxmb_commonCopyBuffer;
    wxmb_commonCopyBuffer2->DeleteContents(TRUE);
    DELETE_OBJ wxmb_commonCopyBuffer2;

    if (wxmb_commonCopyRegionData)
      DELETE_OBJ wxmb_commonCopyRegionData;

    wxmb_commonCopyBuffer = new wxList(wxKEY_NONE, FALSE);
    wxmb_commonCopyBuffer2 = new wxList(wxKEY_NONE, FALSE);

    wxmb_commonCopyRegionData = NULL;

    wxmb_copyStyleList = NULL;

    return;
  }

  copyRingBuffer1[copyRingPos] = wxmb_commonCopyBuffer;
  copyRingBuffer2[copyRingPos] = wxmb_commonCopyBuffer2;
  copyRingData[copyRingPos] = wxmb_commonCopyRegionData;
  copyRingStyle[copyRingPos] = wxmb_copyStyleList;
  
  if (copyRingMax > copyRingDest) {
    /* No more space: delete current ring occupant: */
    wxList *dl;
    dl = copyRingBuffer1[copyRingDest];
    dl->DeleteContents(TRUE);
    DELETE_OBJ dl;
    dl = copyRingBuffer2[copyRingDest];
    dl->DeleteContents(TRUE);
    DELETE_OBJ dl;
    
    if (copyRingData[copyRingDest]) {
      wxBufferData *data;
      data = copyRingData[copyRingDest];
      DELETE_OBJ data;
    }

    copyRingPos = copyRingDest;
  }

  wxmb_commonCopyBuffer = new wxList(wxKEY_NONE, FALSE);
  wxmb_commonCopyBuffer2 = new wxList(wxKEY_NONE, FALSE);
  wxmb_commonCopyRegionData = NULL;
  wxmb_copyStyleList = NULL;

  copyRingPos = copyRingDest;

  copyRingDest++;
  if (copyRingMax < copyRingDest)
    copyRingMax = copyRingDest;
  if (copyRingDest >= copyRingSize)
    copyRingDest = 0;
}

void wxMediaBuffer::InstallCopyBuffer(long time, wxStyleList *sl)
{
  wxmb_copyStyleList = sl;

  if (copyingSelf != copyDepth) {
#if ALLOW_X_STYLE_SELECTION
    if (!xClipboardHack)
#endif
      wxTheClipboard->SetClipboardClient(TheMediaClipboardClient, time);
  }
}

void wxMediaBuffer::DoBufferPaste(long time, Bool local)
{
  wxClipboardClient *owner;
  wxNode *node, *node2;
  wxSnip *snip;
  wxBufferData *bd;

  owner = wxTheClipboard->GetClipboardClient();
  if (local || (!pasteTextOnly && PTREQ(owner, TheMediaClipboardClient))) {
    copyDepth++;
    for ((node = wxmb_commonCopyBuffer->First(),
	  node2 = wxmb_commonCopyBuffer2->First()); 
	 node; 
	 (node = node->Next(),
	  node2 = node2->Next())) {
      snip = (wxSnip *)node->Data();
      bd = (wxBufferData *)node2->Data();
      snip = snip->Copy();
      InsertPasteSnip(snip, bd);
    }
    --copyDepth;
    if (wxmb_commonCopyRegionData && bufferType == wxEDIT_BUFFER)
      ((wxMediaEdit *)this)->PasteRegionData(wxmb_commonCopyRegionData);
  } else {
    char *str;
    long len;

    if (!pasteTextOnly && (str = wxTheClipboard->GetClipboardData("WXME", &len, time))) {
      wxMediaStreamInStringBase *b;
      wxMediaStreamIn *mf;

      b = new wxMediaStreamInStringBase(str, len);
      mf = new wxMediaStreamIn(b);

      strcpy(mf->read_format, MRED_FORMAT_STR);
      strcpy(mf->read_version, MRED_VERSION_STR);

      if (wxReadMediaGlobalHeader(mf))
	if (mf->Ok())
	  if (ReadFromFile(mf)) {
	    wxBufferData *data;
	    data = ReadBufferData(mf);
	    if (data && bufferType == wxEDIT_BUFFER)
	      ((wxMediaEdit *)this)->PasteRegionData(data);
	  }
      wxReadMediaGlobalFooter(mf);
    } else {
      wxBitmap *bm = NULL;
      
      if (!pasteTextOnly)
	bm = wxTheClipboard->GetClipboardBitmap(time);
      if (bm) { 
	snip = new wxImageSnip(bm);
	InsertPasteSnip(snip, NULL);
      } else {
	str = wxTheClipboard->GetClipboardString(time);
	/* no data => empty string */
	InsertPasteString(str);
      }
    }
  }
}

wxMediaClipboardClient::wxMediaClipboardClient()
{
  formats->Add("TEXT");
  formats->Add("WXME");
}

void wxMediaBuffer::CopySelfTo(wxMediaBuffer *m)
{
  /* Copy all the snips: */
  wxList *saveBuffer, *copySnips;
  wxList *saveBuffer2, *copySnips2;
  wxStyleList *saveStyles;
  wxBufferData *saveData;
  int save_cs;
  Bool t;
  char *f;
  wxNode *node, *node2;

  /* Copy style list */
  m->styleList->Copy(styleList);

  /* Copy all the snips: */
  saveBuffer = wxmb_commonCopyBuffer;
  saveBuffer2 = wxmb_commonCopyBuffer2;
  saveStyles = wxmb_copyStyleList;
  saveData = wxmb_commonCopyRegionData;
  save_cs = copyingSelf;

  m->BeginEditSequence();

  copySnips = new wxList(wxKEY_NONE, FALSE);
  wxmb_commonCopyBuffer = copySnips;
  copySnips2 = new wxList(wxKEY_NONE, FALSE);
  wxmb_commonCopyBuffer2 = copySnips2;
  wxmb_copyStyleList = NULL;
  wxmb_commonCopyRegionData = NULL;
  copyingSelf = copyDepth + 1;
  if (bufferType == wxEDIT_BUFFER) {
    wxMediaEdit *e = (wxMediaEdit *)this;
    int pos;
    pos = e->LastPosition();
    e->Copy(TRUE, 0, 0, pos);
  } else {
    wxMediaPasteboard *pb = (wxMediaPasteboard *)this;
    wxSnip *s;
    wxNode *n;
    wxList *unselect;
    unselect = new wxList(wxKEY_NONE, FALSE);
    
    BeginEditSequence();
    for (s = pb->FindFirstSnip(); s; s = s->Next()) {
      if (!pb->IsSelected(s)) {
        pb->AddSelected(s);
        unselect->Append(s);
      }
    }
    pb->Copy(TRUE, 0);
    for (n = unselect->First(); n; n = n->Next()) {
      wxSnip *snp;
      snp = (wxSnip *)n->Data();
      pb->RemoveSelected(snp);
    }
    EndEditSequence();
  }
  wxmb_commonCopyBuffer = saveBuffer;
  wxmb_commonCopyBuffer2 = saveBuffer2;
  wxmb_copyStyleList = saveStyles;
  wxmb_commonCopyRegionData = saveData;
  copyingSelf = save_cs;

  node = copySnips->First();
  node2 = copySnips2->First();
  for (; node; node = node->Next(), node2 = node2->Next()) {
    wxSnip *s;
    wxBufferData *bfd;
    s = (wxSnip *)node->Data();
    if (m->bufferType == wxEDIT_BUFFER)
      m->Insert(s);
    else {
      wxMediaPasteboard *pb = (wxMediaPasteboard *)m;
      pb->Insert(s, s); /* before itself -> at end */
    }
    bfd = (wxBufferData *)node2->Data();
    m->SetSnipData(s, bfd);
  }

  /* Don't delete the snips themselves, though */
  DELETE_OBJ copySnips;
  DELETE_OBJ copySnips2;

  m->SizeCacheInvalid();

  {
    float mw, mh;
    mw = GetMinWidth();
    m->SetMinWidth(mw);
    mw = GetMaxWidth();
    m->SetMaxWidth(mw);
    mh = GetMinHeight();
    m->SetMinHeight(mh);
    mh = GetMaxHeight();
    m->SetMaxHeight(mh);
  }
 
  f = GetFilename(&t);
  m->SetFilename(f, t);

  m->SetMaxUndoHistory(GetMaxUndoHistory());

  m->SetKeymap(GetKeymap());

  m->SetInactiveCaretThreshold(GetInactiveCaretThreshold());
  m->SetLoadOverwritesStyles(GetLoadOverwritesStyles());

  m->EndEditSequence();
}

char *wxMediaClipboardClient::GetData(char *format, long *size)
{
  wxNode *node;
  wxSnip *snip;
  long l, length = 0, sz = 0;
  char *str, *total = NULL, *old;

  if (!strcmp(format, "TEXT")) {
    for (node = wxmb_commonCopyBuffer->First(); node; node = node->Next()) {
      snip = (wxSnip *)node->Data();

      str = snip->GetText(0, snip->count, TRUE);
      l = strlen(str);

      if (total) {
	if (length + l + 1 >= sz) {
	  sz = (2 * sz) + length + l + 1;
	  old = total;
	  total = new char[sz];
	  memcpy(total, old, length);
	}
	memcpy(total + length, str, l);
      } else
	total = str;
      length += l;
    }
    
    if (!total)
      total = new char[1];
    
    total[length] = 0;
    
    *size = length;

    return total;
  } else if (!strcmp(format, "WXME")) {
    wxMediaStreamOutStringBase *b;
    wxMediaStreamOut *mf;
    char *result;

    b = new wxMediaStreamOutStringBase();
    mf = new wxMediaStreamOut(b);

    wxWriteMediaGlobalHeader(mf);
    if (mf->Ok()) {
      mf->PutFixed(0);
      if (!wxmbWriteSnipsToFile(mf, wxmb_copyStyleList, wxmb_commonCopyBuffer, 
				NULL, NULL, wxmb_commonCopyBuffer2, NULL))
	return FALSE;
      mf->PutFixed(0);
      wxmbWriteBufferData(mf, wxmb_commonCopyRegionData);
    }
    wxWriteMediaGlobalFooter(mf);

    result = b->GetString(size);
    return result;
  } else {
    *size = 0;
    return "";
  }
}  

void wxMediaClipboardClient::BeingReplaced(void)
{
}

#if ALLOW_X_STYLE_SELECTION

wxMediaXClipboardClient::wxMediaXClipboardClient()
{
  formats->Add("TEXT");
  formats->Add("WXME");
}

char *wxMediaXClipboardClient::GetData(char *format, long *size)
{
  if (!xSelectionCopied && !wxMediaXSelectionOwner) {
    *size = 0;
    return "";
  }

  if (!xSelectionCopied || wxMediaXSelectionOwner) {
    xClipboardHack = TRUE;
    wxMediaXSelectionOwner->Copy(FALSE, 0L);
    xClipboardHack = FALSE;
  }

  /* If nothing is copied (e.g., DoCopy is overriden to not copy anything
     or copies directly to clipboard): */
  if (!wxmb_copyStyleList) {
    if (wxTheClipboard->GetClipboardClient() == this)
      return NULL;
    else
      return wxTheClipboard->GetClipboardData(format, size, 0);
  }

  return TheMediaClipboardClient->GetData(format, size);
}

void wxMediaXClipboardClient::BeingReplaced(void)
{
  if (wxMediaXSelectionOwner) {
    wxMediaBuffer *b = wxMediaXSelectionOwner;
    wxMediaXSelectionOwner= NULL;
    xSelectionCopied = FALSE;
    b->OwnXSelection(FALSE, TRUE, FALSE);
  } else
    xSelectionCopied = FALSE;
}

#endif

/****************************************************************/

#if ALLOW_X_STYLE_SELECTION

Bool wxMediaBuffer::DoOwnXSelection(Bool on, Bool force)
{
  if (on) {
    if (!force && wxMediaXSelectionAllowed != this)
      return FALSE;
    if (wxMediaXSelectionOwner) {
      wxMediaXSelectionOwner->OwnXSelection(FALSE, TRUE, FALSE);
      wxMediaXSelectionOwner = NULL; // should be redundant
    }
    xSelectionCopied = FALSE;
    wxTheClipboard->SetClipboardClient(TheMediaXClipboardClient, 0L);
    wxMediaXSelectionOwner = this;
  } else if (this == wxMediaXSelectionOwner) {
    wxMediaXSelectionOwner = NULL;
    if (!xSelectionCopied
	&& PTREQ(wxTheClipboard->GetClipboardClient(), 
		 TheMediaXClipboardClient)) {
      wxTheClipboard->SetClipboardString("", 0L);
    }
  }

  return TRUE;
}

void wxMediaBuffer::CopyOutXSelection(void)
{
  if (this == wxMediaXSelectionOwner) {
    xClipboardHack = TRUE;
    wxMediaXSelectionOwner->Copy(FALSE, 0L);
    xClipboardHack = FALSE;
    xSelectionCopied = TRUE;
  }
}

#endif

void wxMediaSetXSelectionMode(Bool on)
{
#if ALLOW_X_STYLE_SELECTION
  wxMediaXSelectionMode = on;
  if (!on && PTREQ(wxTheClipboard->GetClipboardClient(), 
		   TheMediaXClipboardClient))
    wxTheClipboard->SetClipboardString("", 0L);
#endif
}

Bool wxMediaBuffer::GetPasteTextOnly(void)
{
  return pasteTextOnly;
}

void wxMediaBuffer::SetPasteTextOnly(Bool pto)
{
  pasteTextOnly = pto;
}


/****************************************************************/

void wxMediaBuffer::Lock(Bool lock)
{
  userLocked = lock;
}

Bool wxMediaBuffer::IsLocked()
{
  return userLocked;
}

Bool wxMediaBuffer::Modified(void)
{
  return modified;
}

void wxMediaBuffer::SetModified(Bool mod)
{
  modified = mod;

  if (!mod && !undomode) {
    /* Get rid of undos that reset the modification state. */
    int i;
    for (i = changes_end; i != changes_start; ) {
      wxChangeRecord *cr;
      i = (i - 1 + maxUndos) % maxUndos;
      cr = changes[i];
      cr->DropSetUnmodified();
    }
    for (i = redochanges_end; i != redochanges_start; ) {
      wxChangeRecord *cr;
      i = (i - 1 + maxUndos) % maxUndos;
      cr = redochanges[i];
      cr->DropSetUnmodified();
    }
  }
}

int wxMediaBuffer::GetInactiveCaretThreshold(void)
{
  return inactiveCaretThreshold;
}

void wxMediaBuffer::SetInactiveCaretThreshold(int v)
{
  inactiveCaretThreshold = v;
}


void wxMediaBuffer::OnPaint(Bool WXUNUSED(pre),
			    wxDC *WXUNUSED(dc), 
			    float WXUNUSED(l), float WXUNUSED(t), 
			    float WXUNUSED(r), float WXUNUSED(b), 
			    float WXUNUSED(dx), float WXUNUSED(dy),
			    int WXUNUSED(show_caret))
{
  /* Do nothing */
}

Bool wxMediaBuffer::CanSaveFile(char *WXUNUSED(filename), int WXUNUSED(format))
{
  return TRUE;
}

void wxMediaBuffer::OnSaveFile(char *WXUNUSED(filename), int WXUNUSED(format))
{
  /* do nothing */
}

void wxMediaBuffer::AfterSaveFile(Bool WXUNUSED(success))
{
  /* do nothing */
}

Bool wxMediaBuffer::CanLoadFile(char *WXUNUSED(filename), int WXUNUSED(format))
{
  return TRUE;
}

void wxMediaBuffer::OnLoadFile(char *WXUNUSED(filename), int WXUNUSED(format))
{
  /* do nothing */
}

void wxMediaBuffer::AfterLoadFile(Bool WXUNUSED(success))
{
  /* do nothing */
}

void wxMediaBuffer::OnEditSequence(void)
{
  /* Do nothing */
}

void wxMediaBuffer::AfterEditSequence(void)
{
  /* Do nothing */
}

void wxMediaBuffer::OnDisplaySize(void)
{
  /* Do nothing */
}

#ifdef wx_msw
#define WILDCARD "*.*"
#else
#define WILDCARD "*"
#endif

char *wxMediaBuffer::GetFile(char *path)
{
  return wxFileSelector("Choose a file", path, NULL,
			NULL, WILDCARD, wxOPEN, 
			ExtractParent(), 0, 0);
}
 
char *wxMediaBuffer::PutFile(char *path, char *suggested_name)
{
  return wxFileSelector("Save file as", path,
			suggested_name, NULL, WILDCARD, wxSAVE, 
			ExtractParent(), 0, 0);
}

void wxMediaBuffer::SetLoadOverwritesStyles(Bool b)
{
  loadoverwritesstyles = b;
}

Bool wxMediaBuffer::GetLoadOverwritesStyles()
{
  return loadoverwritesstyles;
}


/****************************************************************/

#define edf(name, action, kname) \
     static Bool ed_##name(void *vb, wxEvent *kname, void *) \
     { wxMediaBuffer *b; \
       b = objscheme_unbundle_wxMediaBuffer((Scheme_Object *)vb, NULL, 0); \
       if (!b) \
        return FALSE; \
       b->action; \
       return TRUE; } \

edf(copy, Copy(FALSE, event->timeStamp), event)
edf(copyappend, Copy(TRUE, event->timeStamp), event)
edf(paste, Paste(event->timeStamp), event)
edf(cut, Cut(FALSE, event->timeStamp), event)
edf(kill, Kill(event->timeStamp), event)
edf(cutappend, Cut(TRUE, event->timeStamp), event)
edf(undo, Undo(), WXUNUSED(event))
edf(redo, Redo(), WXUNUSED(event))
edf(delete, Clear(), WXUNUSED(event))
edf(select_all, SelectAll(), WXUNUSED(event))

void wxMediaBuffer::AddBufferFunctions(wxKeymap *tab)
{
  wxAddMediaBufferFunctions(tab);
}

void wxAddMediaBufferFunctions(wxKeymap *tab)
{
#define setf(name, func) tab->AddFunction(name, ed_##func, NULL)

  setf("copy-clipboard", copy);
  setf("copy-append-clipboard", copyappend);
  setf("cut-clipboard", cut);
  setf("cut-append-clipboard", cutappend);
  setf("paste-clipboard", paste);
  setf("delete-selection", delete);
  setf("clear-selection", delete);
  setf("delete-to-end-of-line", kill);

  setf("undo", undo);
  setf("redo", redo);

  setf("select-all", select_all);
}

/****************************************************************/

#ifdef MEMORY_USE_METHOD
long wxMediaBuffer::MemoryUse(void)
{
  return ((filename ? strlen(filename) + 1 : 0)
	  + (maxUndos * 2 * sizeof(wxChangeRecord*))
	  + wxObject::MemoryUse());
}
#endif

/****************************************************************/

wxStandardSnipAdmin::wxStandardSnipAdmin(wxMediaBuffer *m)  
{
#if USE_OLD_TYPE_SYSTEM
  __type = wxTYPE_MEDIA_SNIP_ADMIN;
#endif

  media = m;
}

wxMediaBuffer *wxStandardSnipAdmin::GetMedia(void)
{
  return media;
}

wxDC *wxStandardSnipAdmin::GetDC()
{
  return media->GetDC();
}

void wxStandardSnipAdmin::GetViewSize(float *w, float *h)
{
  GetView(NULL, NULL, w, h, NULL);
}

void wxStandardSnipAdmin::GetView(float *x, float *y, float *w, float *h, wxSnip *snip)
{
  wxMediaAdmin *admin;
  admin = media->GetAdmin();
    
  if (snip) {
    if (admin) {
      float mx, my, mh, mw, mr, mb, sl, st, sr, sb;

      admin->GetView(&mx, &my, &mw, &mh, FALSE);

      mb = my + mh;
      mr = mx + mw;
      if (media->GetSnipLocation(snip, &sl, &st, FALSE)) {
	float l, t, r, b;

	media->GetSnipLocation(snip, &sr, &sb, TRUE);
	
	l = (mx > sl ? mx : sl);
	t = (my > st ? my : st);
	r = (mr > sr ? sr : mr);
	b = (mb > sb ? sb : mb);
	
	if (x)
	  *x = l - sl;
	if (y)
	  *y = t - st;
	if (w)
	  *w = (r - l);
	if (h)
	  *h = (b - t);
	
	return;
      }
    }
  } else {
    if (admin) {
      admin->GetView(x, y, w, h, TRUE);
      return;
    }
  }

  if (x) *x = 0;
  if (y) *y = 0;
  if (w) *w = 0;
  if (h) *h = 0;
}

Bool wxStandardSnipAdmin::ScrollTo(wxSnip *s, float localx, float localy, 
				   float w, float h, Bool refresh, int bias)
{
  if (s->GetAdmin() == this)
    return media->ScrollTo(s, localx, localy, w, h, refresh, bias);
  else
    return FALSE;
}

void wxStandardSnipAdmin::SetCaretOwner(wxSnip *s, int dist)
{
  if (s->GetAdmin() == this)
    media->SetCaretOwner(s, dist);
}
 
void wxStandardSnipAdmin::Resized(wxSnip *s, Bool redraw_now)
{
  if (s->GetAdmin() == this)
    media->Resized(s, redraw_now);
}

Bool wxStandardSnipAdmin::Recounted(wxSnip *s, Bool redraw_now)
{
  if (s->GetAdmin() == this)
    return media->Recounted(s, redraw_now);
  else
    return FALSE;
}

void wxStandardSnipAdmin::NeedsUpdate(wxSnip *s, float localx, float localy, 
				      float w, float h)
{
  if (s->GetAdmin() == this)
    media->NeedsUpdate(s, localx, localy, w, h);
}

Bool wxStandardSnipAdmin::ReleaseSnip(wxSnip *snip)
{
  if (snip->GetAdmin() == this)
    return media->ReleaseSnip(snip);
  else
    return FALSE;
}

void wxStandardSnipAdmin::UpdateCursor()
{
  if (media->admin)
    media->admin->UpdateCursor();
}

Bool wxStandardSnipAdmin::PopupMenu(void *m, wxSnip *snip, float x, float y)
{
  if (media->admin) {
    float sl, st;
    if (media->GetSnipLocation(snip, &sl, &st, FALSE)) {
      media->admin->PopupMenu(m, x + sl, y + st);
    }
  }
   
  return FALSE;
}

#ifdef wx_mac

#ifndef WX_CARBON
# include <Files.h>
#endif

long wxMediaCreatorId = 'WXME';

void wxMediaSetFileCreatorType(char *file, Bool is_binary)
{
  FSSpec spec;
  FInfo info;
  Str255 filename;

  CopyCStringToPascal(file,filename);

  FSMakeFSSpec(0, 0, filename, &spec);

  FSpGetFInfo(&spec, &info);
  info.fdCreator = wxMediaCreatorId;
  info.fdType = is_binary ? 'WXME' : 'TEXT';
  FSpSetFInfo(&spec, &info);
}

#endif
