/*
 * File:	wx_dcmem.h
 * Purpose:	Memory device context declaration
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993-94, AIAI, University of Edinburgh. All Rights Reserved.
 */

/* sccsid[] = "%W% %G%" */


#ifndef wx_dcmemh
#define wx_dcmemh

#include "wb_dcmem.h"
#ifndef OS_X
#include <QDOffscreen.h>
#endif

#ifdef IN_CPROTO
typedef       void    *wxMemoryDC ;
#else

class wxMemoryDC: public wxbMemoryDC
{
 public:
  Bool read_only;
  wxMemoryDC(Bool read_only = FALSE);

  ~wxMemoryDC(void);
  virtual void SelectObject(wxBitmap *bitmap);
  wxBitmap *GetObject();
};

#endif // IN_CPROTO
#endif // wx_dcmemh

