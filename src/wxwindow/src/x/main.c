/*
 * File:	main.c
 * Purpose:	Stub for calling main wxWindows entry point
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	1995
 * RCS_ID:      $Id: main.c,v 1.1 1997/12/22 16:12:08 mflatt Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

extern int wxEntry(int argc, char *argv[]);

#ifdef _LINUX_DLL
#define main __linux_dll_main
#endif

/* Can include some code to setup the
   environment and do a few things "before"
   firing up wxWindows
*/

#if !(defined(AIX) || defined(AIX4))

/* Main Stub */
int
main(int argc, char *argv[])
{
  /* Main entry stub */
  return wxEntry(argc, argv);
}

#endif
 /* End AIX */
