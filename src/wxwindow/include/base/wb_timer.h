/*
 * File:	wb_timer.h
 * Purpose:	wxTimer - provides simple timer functionality
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

#ifndef wxb_timerh
#define wxb_timerh

#include "common.h"
#include "wx_obj.h"

class wxTimer;

class wxbTimer: public wxObject
{
 public:
  Bool one_shot;
  int interval;
  unsigned long expiration;
  void *context;
  wxTimer *next, *prev;

  wxbTimer(void);
  ~wxbTimer(void);
  virtual Bool Start(int milliseconds = -1,Bool one_shot=FALSE) = 0; // Start timer
  virtual void Stop(void) = 0;                   // Stop timer
  virtual void Notify(void);                 // Override this member
  virtual int Interval(void) ; // Returns the current interval time (0 if stop)
};

#endif // wxb_timerh
