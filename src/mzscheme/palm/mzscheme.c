/* Main code for PLT MzScheme */

#include <Pilot.h>
#include <string.h>
#include "callback.h"

#include "mzschemeRsc.h"

static Boolean MainFormHandleEvent (EventPtr e)
{
    Boolean handled = false;
    FormPtr frm;
    
    CALLBACK_PROLOGUE

    switch (e->eType) {
    case frmOpenEvent:
	frm = FrmGetActiveForm();
	FrmDrawForm(frm);
	handled = true;
	break;

    case menuEvent:
	MenuEraseStatus(NULL);

	switch(e->data.menu.itemID) {
	}

    	handled = true;
	break;

    case ctlSelectEvent:
	switch(e->data.ctlSelect.controlID) {
	}
	break;

    default:
        break;
    }

    CALLBACK_EPILOGUE

    return handled;
}

static Boolean ApplicationHandleEvent(EventPtr e)
{
    FormPtr frm;
    Word    formId;
    Boolean handled = false;

    if (e->eType == frmLoadEvent) {
	formId = e->data.frmLoad.formID;
	frm = FrmInitForm(formId);
	FrmSetActiveForm(frm);

	switch(formId) {
	case MainForm:
	    FrmSetEventHandler(frm, MainFormHandleEvent);
	    break;
	}
	handled = true;
    } else if (e->eType == ctlEnterEvent) {
      FieldPtr input, output;
      Handle itext, otext, old;
      Word size;

      frm = FrmGetActiveForm();
      input = (FieldPtr)FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, ExprField));
      output = (FieldPtr)FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, ResultField));
      
      itext = FldGetTextHandle(input);
      size = MemHandleSize((VoidHand)itext);

      otext = (Handle)MemHandleNew(size);
      (void)memcpy(*otext, *itext, size);
      
      old = FldGetTextHandle(output);
      FldSetTextHandle(output, otext);
      // MemHandleFree((VoidHand)old);

      **itext = (sizeof(int) + '0');

      FrmDrawForm(frm);
    }

    return handled;
}

/* Get preferences, open (or create) app database */
static Word StartApplication(void)
{
    FrmGotoForm(MainForm);
    return 0;
}

/* Save preferences, close forms, close app database */
static void StopApplication(void)
{
    FrmSaveAllForms();
    FrmCloseAllForms();
}

/* The main event loop */
static void EventLoop(void)
{
    Word err;
    EventType e;

    do {
	EvtGetEvent(&e, evtWaitForever);
	if (! SysHandleEvent (&e))
	    if (! MenuHandleEvent (NULL, &e, &err))
		if (! ApplicationHandleEvent (&e))
		    FrmDispatchEvent (&e);
    } while (e.eType != appStopEvent);
}

/* Main entry point; it is unlikely you will need to change this except to
   handle other launch command codes */
DWord PilotMain(Word cmd, Ptr cmdPBP, Word launchFlags)
{
    Word err;

    if (cmd == sysAppLaunchCmdNormalLaunch) {

	err = StartApplication();
	if (err) return err;

	EventLoop();
	StopApplication();

    } else {
	return sysErrParamErr;
    }

    return 0;
}
