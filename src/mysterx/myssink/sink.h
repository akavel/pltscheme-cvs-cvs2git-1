// Sink.h : Declaration of the CSink

#ifndef __SINK_H_
#define __SINK_H_

#include "resource.h"       // main symbols
#include "escheme.h"
#include "bstr.h"
#include "sinktbl.h"

typedef struct _event_handler_entry_ { // entry in hash table
  DISPID dispId;
  Scheme_Object *handler;
  FUNCDESC *pFuncDesc;
  _event_handler_entry_ *next;
} EVENT_HANDLER_ENTRY;

const int EVENT_HANDLER_TBL_SIZE = 93;

/////////////////////////////////////////////////////////////////////////////
// CSink
class ATL_NO_VTABLE CSink : 
        public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSink, &CLSID_Sink>,
	public IDispatchImpl<ISink, &IID_ISink, &LIBID_MYSSINKLib>
{

private:
  Scheme_Extension_Table *scheme_extension_table;
  MYSSINK_TABLE *myssink_table;

  Scheme_Object *variantToSchemeObject(VARIANTARG *);
  void unmarshallSchemeObject(Scheme_Object *,VARIANTARG *);
  void handlerUpdateError(char *);

  int getHashValue(DISPID);
  EVENT_HANDLER_ENTRY *newEventHandlerEntry(DISPID,Scheme_Object *,FUNCDESC *);
  EVENT_HANDLER_ENTRY *lookupHandler(DISPID);

  EVENT_HANDLER_ENTRY eventHandlerTable[EVENT_HANDLER_TBL_SIZE];

public:
  CSink() {
  }

DECLARE_REGISTRY_RESOURCEID(IDR_SINK)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CSink)
	COM_INTERFACE_ENTRY(ISink)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// ISink
public:
 STDMETHOD(set_extension_table)(int);
 STDMETHOD(set_myssink_table)(int);
 STDMETHOD(register_handler)(DISPID,int,int); 

//override IDispatch::Invoke()

 STDMETHOD(Invoke)(DISPID,REFIID,LCID,WORD,
		   DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*);
};

#endif //__SINK_H_
