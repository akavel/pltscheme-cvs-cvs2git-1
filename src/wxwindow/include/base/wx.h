/*
 * File:	wx.h
 * Purpose:	Window library main include file
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 *
 * Renovated by Matthew for MrEd, 1995-2000
 */

/* sccsid[] = "@(#)wx.h	1.2 5/9/94" */

#ifndef wx_wxh
#define wx_wxh

#include "wx_setup.h"           // Which features to include - user editable
#include "common.h"

#include "wx_win.h"
#include "wx_frame.h"
#include "wx_dc.h"
#include "wx_dccan.h"
#include "wx_dcmem.h"
#include "wx_dcps.h"
#include "wx_canvs.h"
#include "wx_panel.h"
#include "wx_dcpan.h"
#include "wx_buttn.h"
#include "wx_check.h"
#include "wx_choic.h"
#include "wx_messg.h"
#include "wx_lbox.h"
#include "wx_rbox.h"
#include "wx_slidr.h"
#include "wx_gauge.h"
#include "wx_menu.h"
#include "wx_main.h"
#include "wx_stdev.h"
#include "wx_list.h"
#include "wx_gdi.h"
#include "wx_dialg.h"
#include "wx_utils.h"
#include "wx_cmdlg.h"
#include "wx_res.h"

#ifdef wx_msw
#include "wx_wmgr.h"
#include "wx_privt.h"
#include "wx_itemp.h"

/* I don't know where this is coming from, but it makes a mess. */
#undef GetStyle
#undef SetStyle
#undef GetFont
#undef SetFont
#undef SetBitmap

#endif

#endif // wx_wxh
