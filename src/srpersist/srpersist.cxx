// srpersist.cxx

#include "stdafx.h"

#include <limits.h>
#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#include "escheme.h"

#include "srptypes.h"
#include "srpbuffer.h"
#include "srpersist.h"

#include "srpprims.tbl"
#include "srpstructs.tbl"
#include "srpconsts.tbl"
#include "srpinfo.tbl"
#include "srpbitmask.tbl"

#ifndef WIN32
typedef boolean BOOL
#endif

static Scheme_Unit *srp_unit;  // the unit returned by the extension 

static SRP_BUFFER_TBL_ENTRY *bufferTable[BUFFER_TBL_SIZE];

/* NOTE

   When we wish to return a Scheme string, and a string length
   is available from ODBC, we use scheme_make_sized_string.
   Drivers need not return NULL-terminated strings.

*/

int keyConstCmp(char *s,SRP_NAMED_CONSTANT *p) {
  return stricmp(s,p->scheme_name);
}

int keySmallConstCmp(char *s,SRP_NAMED_SMALL_CONSTANT *p) {
  return stricmp(s,p->scheme_name);
} 

int keyTypedConstCmp(char *s,SRP_NAMED_TYPED_CONSTANT *p) {
  return stricmp(s,p->scheme_name);
}

int keyBitsDictCmp(char *s,SRP_NAMED_BITS_DICT *p) {
  return stricmp(s,p->scheme_name);
}

int namedBitsDictCmp(SRP_NAMED_BITS_DICT *p1,SRP_NAMED_BITS_DICT *p2) {
  return stricmp(p1->scheme_name,p2->scheme_name);
}

char *findBitByValueInDict(SQLUINTEGER value,SRP_NAMED_CONSTANT *entry,size_t numBits) {
  size_t i;

  for (i = 0; i < numBits; i++,entry++) {
    if (entry->val == value) {
      return entry->scheme_name;
    }
  } 

  return NO_BIT_NAME;
}

SQLUINTEGER findBitByNameInDict(char *intName,SRP_NAMED_CONSTANT *entry,size_t numBits) {
  size_t i;

  for (i = 0; i < numBits; i++,entry++) {
    if (strcmpi(entry->scheme_name,intName) == 0) {
      return entry->val;
    }
  } 

  return -1;  // appears to be unused in ODBC header files
}

char *findSmallIntName(char *name,SQLUSMALLINT value,
		       SRP_NAMED_BITS_DICT *dict,size_t dictsize) {
  SRP_NAMED_BITS_DICT *p;

  p = namedBitsDictSearch(name,dict,dictsize);

  if (p == NULL) {
    scheme_signal_error("Unknown constant dictionary entry: %s",name);
  }

  return findBitByValueInDict(value,p->bits,p->numBits);
}

char *findIntegerName(char *name,SQLUINTEGER value,
		      SRP_NAMED_BITS_DICT *dict,size_t dictsize) {
  SRP_NAMED_BITS_DICT *p;

  p = namedBitsDictSearch(name,dict,dictsize);

  if (p == NULL) {
    scheme_signal_error("Unknown constant dictionary entry: %s",name);
  }

  return findBitByValueInDict(value,p->bits,p->numBits);
}

SQLUINTEGER findNamedInteger(char *name,char *intName,
			     SRP_NAMED_BITS_DICT *dict,size_t dictsize) {
  SRP_NAMED_BITS_DICT *p;

  p = namedBitsDictSearch(name,dict,dictsize);

  if (p == NULL) {
    scheme_signal_error("Unknown constant dictionary entry: %s",name);
  }

  return findBitByNameInDict(intName,p->bits,p->numBits);
}

Scheme_Object *bitsListFromBitMask(char *name,SQLUINTEGER bits) {
  Scheme_Object *retval;
  SRP_NAMED_BITS_DICT *p;
  size_t numBits;
  SRP_NAMED_CONSTANT *q;
  size_t i;

  p = namedBitsDictSearch(name,bitMaskTable,sizeray(bitMaskTable));

  if (p == NULL) {
    scheme_signal_error("Unknown constant dictionary entry: %s",name);
  }

  numBits = p->numBits;

  retval = scheme_null;

  for (i = 0, q = p->bits; i < numBits; i++,q++) {
    if (q->val & bits) {
      retval = scheme_make_pair(scheme_intern_symbol(q->scheme_name),
				retval);
    }
  }

  return retval;
}

void SchemeObjectToHandle(Scheme_Object *obj,
			  HANDLE *handle,SQLSMALLINT *handleType) {

  if (SQL_HENVP(obj)) {
    *handle = SQL_HENV_VAL(obj);
    *handleType = SQL_HANDLE_ENV;
  }
  else if (SQL_HDBCP(obj)) {
    *handle = SQL_HDBC_VAL(obj);
    *handleType = SQL_HANDLE_DBC;
  }
  else if (SQL_HSTMTP(obj)) {
    *handle = SQL_HSTMT_VAL(obj);
    *handleType = SQL_HANDLE_STMT;
  }
  else if (SQL_HDESCP(obj)) {
    *handle = SQL_HDESC_VAL(obj);
    *handleType = SQL_HANDLE_DESC;
  }
}

int sizeofCDataType(SQLSMALLINT type) {
  switch (type) {
  case SQL_C_CHAR :
    return sizeof(unsigned char);
  case SQL_C_SSHORT :
    return sizeof(short int);
  case SQL_C_USHORT :
    return sizeof(unsigned short int);
  case SQL_C_SLONG :
    return sizeof(long int);
  case SQL_C_ULONG :
    // SQL_C_BOOKMARK is same value
    return sizeof(unsigned long int);
  case SQL_C_FLOAT :
    return sizeof(float);
  case SQL_C_DOUBLE :
    return sizeof(double);
  case SQL_C_BIT :
    return sizeof(unsigned char);
  case SQL_C_STINYINT :
    return sizeof(signed char);
  case SQL_C_UTINYINT :
    return sizeof(unsigned char);
  case SQL_C_SBIGINT :
    return sizeof(_int64);
  case SQL_C_UBIGINT :
    return sizeof(unsigned _int64);
  case SQL_C_BINARY :
    // SQL_C_VARBOOKMARK has same value
    return sizeof(unsigned char *);
  case SQL_C_TYPE_DATE :
    return sizeof(DATE_STRUCT);
  case SQL_C_TYPE_TIME :
    return sizeof(TIME_STRUCT);
  case SQL_C_TYPE_TIMESTAMP :
    return sizeof(TIMESTAMP_STRUCT);
  case SQL_C_NUMERIC :
    return sizeof(SQL_NUMERIC_STRUCT);
  case SQL_C_GUID :
    return sizeof(SQLGUID);
  case SQL_C_INTERVAL_YEAR :
  case SQL_C_INTERVAL_MONTH :
  case SQL_C_INTERVAL_DAY :
  case SQL_C_INTERVAL_HOUR :
  case SQL_C_INTERVAL_MINUTE :
  case SQL_C_INTERVAL_SECOND :
  case SQL_C_INTERVAL_YEAR_TO_MONTH :
  case SQL_C_INTERVAL_DAY_TO_HOUR :
  case SQL_C_INTERVAL_DAY_TO_MINUTE :
  case SQL_C_INTERVAL_DAY_TO_SECOND :
  case SQL_C_INTERVAL_HOUR_TO_MINUTE :
  case SQL_C_INTERVAL_HOUR_TO_SECOND :
  case SQL_C_INTERVAL_MINUTE_TO_SECOND :
    return sizeof(SQL_INTERVAL_STRUCT);
  }
  
  scheme_signal_error("Unknown C data type constant: %X",(int)type);

  return 0;  // unreachable
}

unsigned short getHashValue(void *address) {
  return ((unsigned short)address >> 4) % BUFFER_TBL_SIZE;
}

void addToBufferTable(void *address,SRP_SQL_BUFFER *buffer) {
  unsigned short hashVal;
  SRP_BUFFER_TBL_ENTRY *pEntry,*p;

  pEntry = (SRP_BUFFER_TBL_ENTRY *)scheme_malloc(sizeof(SRP_BUFFER_TBL_ENTRY));
  pEntry->address = address;
  pEntry->buffer = buffer;
  
  hashVal = getHashValue(address);
  
  p = bufferTable[hashVal];
  
  if (p == NULL) {
    bufferTable[hashVal] = pEntry;
  }
  else {
    while (p->next != NULL) {
      p = p->next;
    }
    p->next = pEntry; 
  }
}

SRP_SQL_BUFFER *lookupBufferFromAddress(void *address) {
  unsigned short hashVal;
  SRP_BUFFER_TBL_ENTRY *p;

  hashVal = getHashValue(address);
  
  p = bufferTable[hashVal];
  
  while (p) {
    if (p->address == address) {
      return p->buffer;
    }
    p = p->next;
  }

  return NULL;
}

char *rowStatusToString(SQLUSMALLINT rowStatus) {
  switch (rowStatus) {

  case SQL_ROW_DELETED :
    return "sql-row-deleted";

  case SQL_ROW_ERROR :
    return "sql-row-error";

  case SQL_ROW_SUCCESS :
    return "sql-row-success";

  case SQL_ROW_UPDATED :
    return "sql-row-updated";
  }

  return "?";
}

// utilities

Scheme_Object *srp_make_length(int argc,Scheme_Object **argv) {
  SRP_SQL_LENGTH *retval;

  retval = (SRP_SQL_LENGTH *)scheme_malloc_eternal(sizeof(SRP_SQL_LENGTH));

  retval->type = sql_length_type; 
  retval->value = 0;

  return (Scheme_Object *)retval;
}

Scheme_Object *srp_make_indicator(int argc,Scheme_Object **argv) {
  SRP_SQL_INDICATOR *retval;

  retval = (SRP_SQL_INDICATOR *)scheme_malloc_eternal(sizeof(SRP_SQL_INDICATOR));

  retval->type = sql_indicator_type; 
  retval->value = 0;

  return (Scheme_Object *)retval;
}

Scheme_Object *srp_read_op_parms(int argc,Scheme_Object **argv) {
  SQLUINTEGER i;
  SQLUSMALLINT *values;
  Scheme_Object *retval,*symbol;
  
  if (SQL_OP_PARMSP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-read-op-parms","sql-op-parms",0,argc,argv);
  }

  values = SQL_OP_PARMS_VAL(argv[0]);

  if (values == NULL) {
    return scheme_null;
  }

  retval = scheme_null;

  for (i = SQL_OP_PARMS_LEN(argv[0]) - 1; i >= 0; i--) {
    switch(values[i]) {
    case SQL_PARAM_PROCEED :
      symbol = scheme_intern_symbol("sql-param-proceed");
      break;
    case SQL_PARAM_IGNORE :
      symbol = scheme_intern_symbol("sql-param-ignore");
      break;
    default :
      scheme_signal_error("sql-read-op-parms: unknown operation parameter: %X",
			  (unsigned int)(values[i]));
    }
    retval = scheme_make_pair(symbol,retval);
  }

  return retval;

}

Scheme_Object *srp_read_indicator(int argc,Scheme_Object **argv) {
  SQLINTEGER value;
  Scheme_Object *value_object;

  if (SQL_INDICATORP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-read-indicator","sql-indicator",0,argc,argv);
  }

  value = SQL_INDICATOR_VAL(argv[0]);

  switch(value) {
  case SQL_NO_TOTAL :
    return scheme_intern_symbol("sql-no-total");
  case SQL_NULL_DATA :
    return scheme_intern_symbol("sql-null-data");
  case SQL_NTS :
    return scheme_intern_symbol("sql-nts");
  case SQL_COLUMN_IGNORE :
    return scheme_intern_symbol("sql-column-ignore");
  case SQL_DATA_AT_EXEC :
    return scheme_intern_symbol("sql-data-at-exec");
  }

  value_object = scheme_make_integer_value(value);

  if (value <= SQL_LEN_DATA_AT_EXEC_OFFSET) {
    return scheme_make_pair(scheme_intern_symbol("sql-len-data-at-exec"),
			    scheme_make_pair(value_object,scheme_null));
  }

  return value_object;
}

Scheme_Object *srp_read_length(int argc,Scheme_Object **argv) {
  if (SQL_LENGTHP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-read-length","sql-length",0,argc,argv);
  }

  return scheme_make_integer_value(SQL_LENGTH_VAL(argv[0]));
}

Scheme_Object *srp_set_indicator(int argc,Scheme_Object **argv) {
  char *lenString;
  SRP_SQL_INDICATOR *indicator;

  if (SQL_INDICATORP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-set-indicator","sql-indicator",0,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-set-indicator","symbol",1,argc,argv);
  }

  indicator = (SRP_SQL_INDICATOR *)(argv[0]);

  lenString = SCHEME_SYM_VAL(argv[1]);

  if (strcmpi(lenString,"sql-len-data-at-exec") == 0) {
    int execVal;

    if (argc != 3) {
      scheme_signal_error("sql-set-indicator: "
			  "%s requires additional integer argument",lenString);
    }

    if (SCHEME_INTP(argv[2]) == FALSE) {
      scheme_wrong_type("sql-set-indicator","int",2,argc,argv);
    }

    execVal = SCHEME_INT_VAL(argv[2]);

    indicator->value = SQL_LEN_DATA_AT_EXEC(execVal);

    return scheme_void;
  }

  if (argc != 2) {
      scheme_signal_error("sql-set-indicator: "
			  "too many arguments for indicator value %s",
			  lenString);
  }

  if (strcmpi(lenString,"sql-nts") == 0) {
    indicator->value = SQL_NTS;
  }
  else if (strcmpi(lenString,"sql-null-data") == 0) {
    indicator->value = SQL_NULL_DATA;
  }
  else if (strcmpi(lenString,"sql-column-ignore") == 0) {
    indicator->value = SQL_COLUMN_IGNORE;
  }
  else if (strcmpi(lenString,"sql-data-at-exec") == 0) {
    indicator->value = SQL_DATA_AT_EXEC;
  }
  else {
    scheme_signal_error("sql-set-indicator: "
			"unknown indicator value %s",
			lenString);

  }

  return scheme_void;
}


Scheme_Object *srp_read_row_status(int argc,Scheme_Object **argv) {
  SQLUSMALLINT *values;
  SQLUINTEGER numRows;
  Scheme_Object *retval;
  Scheme_Object *symbol;
  long i;

  if (SQL_ROW_STATUSP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-read-row-status","sql-row-status",0,argc,argv);
  }

  values = SQL_ROW_STATUS_VAL(argv[0]);
  numRows = SQL_ROW_STATUS_LEN(argv[0]);

  retval = scheme_null;

  for (i = numRows-1; i >= 0; i--) {
    symbol = scheme_intern_symbol(rowStatusToString(values[i]));
    retval = scheme_make_pair(symbol,retval);
  }

  return retval;

}

char *APDArrayStatusToString(SQLUSMALLINT status) {
  switch(status) {
  case SQL_PARAM_PROCEED :
    return "sql-param-proceed";
  case SQL_PARAM_IGNORE :
    return "sql-param-ignore";
  default :
    scheme_signal_error("Unknown array status (%X) for APD descriptor",status);
  }
  return NULL;
}

char *IPDArrayStatusToString(SQLUSMALLINT status) {
  switch(status) {
  case SQL_PARAM_SUCCESS :
    return "sql-param-success";
  case SQL_PARAM_SUCCESS_WITH_INFO :
    return "sql-param-success-with-info";
  case SQL_PARAM_ERROR :
    return "sql-param-error";
  case SQL_PARAM_UNUSED :
    return "sql-param-unused";
  case SQL_PARAM_DIAG_UNAVAILABLE :
    return "sql-param-unavailable";
  default :
    scheme_signal_error("Unknown IPD array status (%X)",status);
  }
  return NULL;
}

char *ARDArrayStatusToString(SQLUSMALLINT status) {
  switch(status) {
  case SQL_ROW_PROCEED :
    return "sql-row-proceed";
  case SQL_ROW_IGNORE :
    return "sql-row-ignore";
  default :
    scheme_signal_error("Unknown ARD array status (%X)",status);
  }
  return NULL;
}

char *IRDArrayStatusToString(SQLUSMALLINT status) {
  switch(status) {
  case SQL_ROW_SUCCESS :
    return "sql-row-success";
  case SQL_ROW_SUCCESS_WITH_INFO :
    return "sql-row-success-with-info";
  case SQL_ROW_ERROR :
    return "sql-row-error";
  case SQL_ROW_UPDATED :
    return "sql-row-updated";
  case SQL_ROW_DELETED :
    return "sql-row-deleted";
  case SQL_ROW_ADDED :
    return "sql-row-added";
  case SQL_ROW_NOROW :
    return "sql-row-norow";
  default :
    scheme_signal_error("Unknown IRD array status (%X)",status);
  }
  return NULL;
}

Scheme_Object *srp_read_array_status(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLUSMALLINT *values;
  DESCTYPE descType;
  SQLHDESC hdesc;
  SQLUINTEGER arraySize;
  SQLSMALLINT descCount;
  SQLUINTEGER *pRowsCount;
  SQLINTEGER actualLen;
  Scheme_Object *retval;
  long i;

  if (SQL_ARRAY_STATUSP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-read-array-status","sql-array-status",0,argc,argv);
  }

  values = SQL_ARRAY_STATUS_VAL(argv[0]);
  descType = SQL_ARRAY_STATUS_DESCTYPE(argv[0]);
  hdesc = SQL_ARRAY_STATUS_HDESC(argv[0]);

  if (values == NULL) {
    return scheme_null;
  }

  retval = scheme_null;

  switch(descType) {
  case APD :

    sr = SQLGetDescField(hdesc,1,SQL_DESC_COUNT,
			 &descCount,sizeof(descCount),&actualLen);

    if (sr != SQL_SUCCESS) {
      scheme_signal_error("Can't get array size for APD descriptor");
    }

    retval = scheme_null;

    for (i = descCount - 1; i >= 0; i--) {
      retval = scheme_make_pair(scheme_intern_symbol(APDArrayStatusToString(values[i])),
				retval);
    }

    return retval;

  case ARD :

    sr = SQLGetDescField(hdesc,1,SQL_DESC_COUNT,
			 &arraySize,sizeof(arraySize),&actualLen);

    if (sr != SQL_SUCCESS) {
      scheme_signal_error("Can't get rowset size for ARD descriptor");
    }

    retval = scheme_null;

    for (i = arraySize - 1; i >= 0; i--) {
      retval = scheme_make_pair(scheme_intern_symbol(ARDArrayStatusToString(values[i])),
				retval);
    }

    return retval;

  case IPD :

    sr = SQLGetDescField(hdesc,1,SQL_DESC_COUNT,
			 &descCount,sizeof(descCount),&actualLen);

    if (sr != SQL_SUCCESS) {
      scheme_signal_error("Can't get parameter count for IPD descriptor");
    }

    retval = scheme_null;

    for (i = descCount - 1; i >= 0; i--) {
      retval = scheme_make_pair(scheme_intern_symbol(IPDArrayStatusToString(values[i])),
				retval);
    }

    return retval;

  case IRD :

    sr = SQLGetDescField(hdesc,1,SQL_DESC_ROWS_PROCESSED_PTR,
			 &pRowsCount,sizeof(pRowsCount),&actualLen);

    if (sr != SQL_SUCCESS) {
      scheme_signal_error("Can't get rowset size for IRD descriptor");
    }

    retval = scheme_null;

    for (i = *pRowsCount - 1; i >= 0; i--) {
      retval = scheme_make_pair(scheme_intern_symbol(IRDArrayStatusToString(values[i])),
				retval);
    }

    return retval;

  case EXPLICIT :

    scheme_signal_error("Can't get array status for explicitly-allocated descriptor");
  } 

  return retval;

}

Scheme_Object *srp_make_buffer(int argc,Scheme_Object **argv) {
  SRP_SQL_BUFFER *retval;
  char *typeName;
  SRP_NAMED_CONSTANT *p;
  long numElts;

  if (SCHEME_SYMBOLP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-make-buffer","symbol",0,argc,argv);
  }

  if (SCHEME_EXACT_INTEGERP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-make-buffer","int",1,argc,argv);
  }

  typeName = SCHEME_SYM_VAL(argv[0]);

  p = namedConstSearch(typeName,CDataTypes);
  
  if (p == NULL) {
    scheme_signal_error("sql-make-buffer: invalid C type: %s",typeName);
  }

  if (scheme_get_int_val(argv[1],&numElts) == 0) {
    scheme_signal_error("sql-make-buffer: too many elements requested");
  }

  retval = (SRP_SQL_BUFFER *)scheme_malloc_eternal(sizeof(SRP_SQL_BUFFER));
  retval->type = sql_buffer_type;

  retval->numElts = numElts;

  retval->CDataType = (SQLSMALLINT)(p->val);

  retval->eltSize = sizeofCDataType(retval->CDataType);

  // buffers might be relinquished by Scheme, 
  // but still bound to OBDC columns
  // so make actual storage uncollectable

  retval->storage = scheme_malloc_eternal(retval->numElts * sizeof(retval->eltSize));

  // need to be able to recover <sql-buffer> from storage address
  // for use by SQLParamData()

  addToBufferTable(retval->storage,retval);

  return (Scheme_Object *)retval;
}

Scheme_Object *srp_read_buffer(int argc,Scheme_Object **argv) {
  SQLSMALLINT CDataType;
  void *buffer;
  unsigned long numElts;

  if (SQL_BUFFERP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-read-buffer","int",0,argc,argv);
  } 

  CDataType = SQL_BUFFER_CTYPE(argv[0]);
  buffer = SQL_BUFFER_VAL(argv[0]);
  numElts = SQL_BUFFER_NUMELTS(argv[0]);

  switch(CDataType) {
  case SQL_C_CHAR :
    return readCharBuffer((char *)buffer,numElts);
  case SQL_C_SLONG :
  case SQL_C_LONG :
    return readLongBuffer((long *)buffer,numElts);
  case SQL_C_ULONG :
    // SQL_C_BOOKMARK is the same
    return readULongBuffer((unsigned long *)buffer,numElts);
  case SQL_C_SSHORT :
  case SQL_C_SHORT :
    return readShortBuffer((short *)buffer,numElts);
  case SQL_C_USHORT :
    return readUShortBuffer((unsigned short *)buffer,numElts);
  case SQL_C_FLOAT :
    return readFloatBuffer((float *)buffer,numElts);
  case SQL_C_DOUBLE :
    return readDoubleBuffer((double *)buffer,numElts);
  case SQL_C_NUMERIC :
    return readNumericBuffer((SQL_NUMERIC_STRUCT *)buffer,numElts);
  case SQL_C_DATE :
  case SQL_C_TYPE_DATE :
    return readDateBuffer((DATE_STRUCT *)buffer,numElts);
  case SQL_C_TIME :
  case SQL_C_TYPE_TIME :
    return readTimeBuffer((TIME_STRUCT *)buffer,numElts);
  case SQL_C_TIMESTAMP :
  case SQL_C_TYPE_TIMESTAMP :
    return readTimeStampBuffer((TIMESTAMP_STRUCT *)buffer,numElts);
  case SQL_C_INTERVAL_YEAR :
    return readIntervalYearBuffer((SQL_INTERVAL_STRUCT *)buffer,numElts);
  case SQL_C_INTERVAL_MONTH :
    return readIntervalMonthBuffer((SQL_INTERVAL_STRUCT *)buffer,numElts);
  case SQL_C_INTERVAL_DAY :
    return readIntervalDayBuffer((SQL_INTERVAL_STRUCT *)buffer,numElts);
  case SQL_C_INTERVAL_HOUR :
    return readIntervalHourBuffer((SQL_INTERVAL_STRUCT *)buffer,numElts);
  case SQL_C_INTERVAL_MINUTE :
    return readIntervalMinuteBuffer((SQL_INTERVAL_STRUCT *)buffer,numElts);
  case SQL_C_INTERVAL_SECOND :
    return readIntervalSecondBuffer((SQL_INTERVAL_STRUCT *)buffer,numElts);
  case SQL_C_INTERVAL_YEAR_TO_MONTH :
    return readIntervalYearMonthBuffer((SQL_INTERVAL_STRUCT *)buffer,numElts);
  case SQL_C_INTERVAL_DAY_TO_HOUR :
    return readIntervalDayHourBuffer((SQL_INTERVAL_STRUCT *)buffer,numElts);
  case SQL_C_INTERVAL_DAY_TO_MINUTE :
    return readIntervalDayMinuteBuffer((SQL_INTERVAL_STRUCT *)buffer,numElts);
  case SQL_C_INTERVAL_DAY_TO_SECOND :
    return readIntervalDaySecondBuffer((SQL_INTERVAL_STRUCT *)buffer,numElts);
  case SQL_C_INTERVAL_HOUR_TO_MINUTE :
    return readIntervalHourMinuteBuffer((SQL_INTERVAL_STRUCT *)buffer,numElts);
  case SQL_C_INTERVAL_HOUR_TO_SECOND :
    return readIntervalHourSecondBuffer((SQL_INTERVAL_STRUCT *)buffer,numElts);
  case SQL_C_INTERVAL_MINUTE_TO_SECOND :
    return readIntervalMinuteSecondBuffer((SQL_INTERVAL_STRUCT *)buffer,numElts);
  case SQL_C_BINARY :
    // SQL_C_VARBOOKMARK is the same
    return readBinaryBuffer((char *)buffer,numElts);
  case SQL_C_BIT :
    return readBitBuffer((unsigned char *)buffer,numElts);
  case SQL_C_SBIGINT :
    return readBigIntBuffer((_int64 *)buffer,numElts);
  case SQL_C_UBIGINT :
    return readUBigIntBuffer((unsigned _int64 *)buffer,numElts);
  case SQL_C_STINYINT :
  case SQL_C_TINYINT :
    return readTinyBuffer((char *)buffer,numElts);
  case SQL_C_UTINYINT :
    return readUTinyBuffer((unsigned char *)buffer,numElts);
  case SQL_C_GUID :
    return readGuidBuffer((SQLGUID *)buffer,numElts);
  }

  scheme_signal_error("Unknown buffer C data type: %X",CDataType);

  return scheme_void; // unreachable

}

BOOL schemeIntP(Scheme_Object *o) {
  return SCHEME_INTP(o);
}

BOOL schemeExactIntegerP(Scheme_Object *o) {
  return SCHEME_EXACT_INTEGERP(o);
}

BOOL schemeFloatP(Scheme_Object *o) {
  return SCHEME_FLOATP(o);
}

BOOL schemeDoubleP(Scheme_Object *o) {
  return SCHEME_DBLP(o);
}

BOOL schemeNumericP(Scheme_Object *o) {
  return (scheme_is_struct_instance(NUMERIC_STRUCT_TYPE,o));
}

BOOL schemeDateP(Scheme_Object *o) {
  return (scheme_is_struct_instance(DATE_STRUCT_TYPE,o));
}

BOOL schemeTimeP(Scheme_Object *o) {
  return (scheme_is_struct_instance(TIME_STRUCT_TYPE,o));
}

BOOL schemeTimeStampP(Scheme_Object *o) {
  return (scheme_is_struct_instance(TIMESTAMP_STRUCT_TYPE,o));
}

BOOL schemeGuidP(Scheme_Object *o) {
  return (scheme_is_struct_instance(GUID_STRUCT_TYPE,o));
}

BOOL schemeIntervalIntegerP(Scheme_Object *o) {
  Scheme_Object *currList;
  Scheme_Object *sign,*theInt;

  if (scheme_proper_list_length(o) != 2) {
    return FALSE;
  }

  currList = o;

  sign = SCHEME_CAR(currList);

  if (SCHEME_SYMBOLP(sign) == FALSE) {
    return FALSE;
  }

  if (strcmp(SCHEME_SYM_VAL(sign),"+") && 
      strcmp(SCHEME_SYM_VAL(sign),"-")) {
    return FALSE;
  }

  theInt = SCHEME_CADR(currList);

  return isUnsignedInt(theInt);
}

BOOL checkIsPredList(Scheme_Object *o,BOOL (*p)(Scheme_Object *),long numElts) {
  Scheme_Object *currList,*currVal;
  long count;

  if (SCHEME_PAIRP(o) == FALSE) {
    return FALSE;
  }

  currList = o;
  count = 0;

  while (currList != scheme_null) {

    currVal = SCHEME_CAR(currList);

    if (++count > numElts) {
      scheme_signal_error("sql-write-buffer: too many elements to fit in buffer");
    }

    if ((*p)(currVal) == FALSE) {
      return FALSE;
    }

    currList = SCHEME_CDR(currList);
  }

  return TRUE;
}

typedef unsigned long *(*INTERVAL_FIELD_ACCESSOR)(SQL_INTERVAL_STRUCT *); 

void writeIntervalToBuff(void *buffer,Scheme_Object *currList,long numElts,
			 SQLINTERVAL intervalType,
			 short numFields,
			 Scheme_Object *signProc,
			 Scheme_Object **intProc,
			 INTERVAL_FIELD_ACCESSOR *fieldFromInterval) {
  Scheme_Object *currVal;
  Scheme_Object *currSign,*currInt;
  char *signStr;
  SQL_INTERVAL_STRUCT *pInterval;
  long i,j;

  checkIsPredList(currList,schemeIntervalIntegerP,numElts);

  for (i = 0; currList != scheme_null; i++) {

    currVal = SCHEME_CAR(currList);

    pInterval = (SQL_INTERVAL_STRUCT *)buffer + i;

    currSign = scheme_apply(signProc,1,&currVal);

    pInterval->interval_type = intervalType;

    signStr = SCHEME_SYM_VAL(currSign);
    pInterval->interval_sign = 
      (*signStr == '+') ? SQL_FALSE : SQL_TRUE;

    for (j = 0; j < numFields; j++) {

      currInt = scheme_apply(intProc[j],1,&currVal);

      /* this depends on sizeof(long) == sizeof(int) */

      if (scheme_get_unsigned_int_val(currInt,fieldFromInterval[j](pInterval)) == 0) {
	scheme_signal_error("sql-write-buffer: interval too big");
      }
    }

    currList = SCHEME_CDR(currList);
  }
}

unsigned long *getIntervalYear(SQL_INTERVAL_STRUCT *p) {
  return &p->intval.year_month.year;
} 

unsigned long *getIntervalMonth(SQL_INTERVAL_STRUCT *p) {
  return &p->intval.year_month.month;
} 

unsigned long *getIntervalDay(SQL_INTERVAL_STRUCT *p) {
  return &p->intval.day_second.day;
} 

unsigned long *getIntervalHour(SQL_INTERVAL_STRUCT *p) {
  return &p->intval.day_second.hour;
} 

unsigned long *getIntervalMinute(SQL_INTERVAL_STRUCT *p) {
  return &p->intval.day_second.minute;
} 

unsigned long *getIntervalSecond(SQL_INTERVAL_STRUCT *p) {
  return &p->intval.day_second.second;
} 

Scheme_Object *srp_write_buffer(int argc,Scheme_Object **argv) {
  SQLSMALLINT CDataType;
  void *buffer;
  long numElts;
  Scheme_Object *accessors[5];
  INTERVAL_FIELD_ACCESSOR fields[5];
  
  if (SQL_BUFFERP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-write-buffer","int",0,argc,argv);
  } 

  CDataType = SQL_BUFFER_CTYPE(argv[0]);
  buffer = SQL_BUFFER_VAL(argv[0]);
  numElts = SQL_BUFFER_NUMELTS(argv[0]);

  memset(buffer,'\0',numElts * sizeofCDataType(CDataType));

  // check that data to be written is of appropriate type, 
  // then call specialized write routine

  switch(CDataType) {
  case SQL_C_CHAR :

    if (SCHEME_STRINGP(argv[1]) == FALSE) {
      scheme_wrong_type("sql-write-buffer","string",1,argc,argv);
    }

    if (SCHEME_STRLEN_VAL(argv[1]) >= numElts) {
      scheme_signal_error("sql-write-buffer: string too long for buffer");
    }

    writeCharBuffer((char *)buffer,argv[1]);
    break;

  case SQL_C_SLONG :
  case SQL_C_LONG :

    checkIsPredList(argv[1],schemeExactIntegerP,numElts);

    writeLongBuffer((long *)buffer,argv[1]); 

    break;

  case SQL_C_ULONG :

    // SQL_C_BOOKMARK is the same

    checkIsPredList(argv[1],schemeExactIntegerP,numElts);
    writeULongBuffer((unsigned long *)buffer,argv[1]); 

    break;

  case SQL_C_SSHORT :
  case SQL_C_SHORT :

    checkIsPredList(argv[1],schemeIntP,numElts);
    writeShortBuffer((short *)buffer,argv[1]); 

    break;


  case SQL_C_USHORT :

    checkIsPredList(argv[1],schemeIntP,numElts);
    writeUShortBuffer((unsigned short *)buffer,argv[1]); 

    break;

  case SQL_C_STINYINT :
  case SQL_C_TINYINT :

    checkIsPredList(argv[1],schemeIntP,numElts);
    writeTinyBuffer((char *)buffer,argv[1]); 

  case SQL_C_UTINYINT :

    checkIsPredList(argv[1],schemeIntP,numElts);
    writeUTinyBuffer((unsigned char *)buffer,argv[1]); 

  case SQL_C_SBIGINT :

    checkIsPredList(argv[1],schemeExactIntegerP,numElts);
    writeBigIntBuffer((_int64 *)buffer,argv[1]); 

  case SQL_C_UBIGINT :

    checkIsPredList(argv[1],schemeExactIntegerP,numElts);
    writeUBigIntBuffer((unsigned _int64 *)buffer,argv[1]); 

  case SQL_C_FLOAT :

    checkIsPredList(argv[1],schemeFloatP,numElts);
    writeFloatBuffer((float *)buffer,argv[1]); 

    break;

  case SQL_C_DOUBLE :

    checkIsPredList(argv[1],schemeDoubleP,numElts);
    writeDoubleBuffer((double *)buffer,argv[1]); 

    break;

  case SQL_C_NUMERIC :

    checkIsPredList(argv[1],schemeNumericP,numElts);
    writeNumericBuffer((SQL_NUMERIC_STRUCT *)buffer,argv[1]); 

    break;

  case SQL_C_DATE :
  case SQL_C_TYPE_DATE :

    checkIsPredList(argv[1],schemeDateP,numElts);
    writeDateBuffer((SQL_DATE_STRUCT *)buffer,argv[1]); 

    break;

  case SQL_C_TIME :
  case SQL_C_TYPE_TIME :

    checkIsPredList(argv[1],schemeTimeP,numElts);
    writeTimeBuffer((SQL_TIME_STRUCT *)buffer,argv[1]); 

    break;

  case SQL_C_TIMESTAMP :
  case SQL_C_TYPE_TIMESTAMP :

    checkIsPredList(argv[1],schemeTimeP,numElts);
    writeTimeStampBuffer((SQL_TIMESTAMP_STRUCT *)buffer,argv[1]); 

    break;

  case SQL_C_GUID :

    checkIsPredList(argv[1],schemeGuidP,numElts);
    writeGuidBuffer((SQLGUID *)buffer,argv[1]); 

    break;

  case SQL_C_BIT :

    if (SCHEME_STRINGP(argv[1]) == FALSE) {
      scheme_wrong_type("sql-write-buffer","string",1,argc,argv);
    }

    writeBitBuffer((char *)buffer,argv[1]); 

    break;

  case SQL_C_BINARY :
    // SQL_C_VARBOOKMARK is the same

    if (SCHEME_STRINGP(argv[1]) == FALSE) {
      scheme_wrong_type("sql-write-buffer","string",1,argc,argv);
    }

    writeBinaryBuffer((char *)buffer,argv[1]); 

    break;

  case SQL_C_INTERVAL_YEAR :

    accessors[0] = YEAR_INTERVAL_YEAR;
    fields[0] = getIntervalYear;

    writeIntervalToBuff(buffer,argv[1],numElts,SQL_IS_YEAR,
			1,YEAR_INTERVAL_SIGN,
			accessors,fields);

    break;

  case SQL_C_INTERVAL_MONTH :

    accessors[0] = MONTH_INTERVAL_MONTH;
    fields[0] = getIntervalMonth;

    writeIntervalToBuff(buffer,argv[1],numElts,SQL_IS_MONTH,
			1,MONTH_INTERVAL_SIGN,
			accessors,fields);

    break;

  case SQL_C_INTERVAL_DAY :

    accessors[0] = DAY_INTERVAL_DAY;
    fields[0] = getIntervalDay;

    writeIntervalToBuff(buffer,argv[1],numElts,SQL_IS_DAY,
			1,DAY_INTERVAL_SIGN,
			accessors,fields);

    break;

  case SQL_C_INTERVAL_HOUR :

    accessors[0] = HOUR_INTERVAL_HOUR;
    fields[0] = getIntervalHour;

    writeIntervalToBuff(buffer,argv[1],numElts,SQL_IS_HOUR,
			1,HOUR_INTERVAL_SIGN,
			accessors,fields);

    break;

  case SQL_C_INTERVAL_MINUTE :

    accessors[0] = MINUTE_INTERVAL_MINUTE;
    fields[0] = getIntervalMinute;

    writeIntervalToBuff(buffer,argv[1],numElts,SQL_IS_MINUTE,
			1,MINUTE_INTERVAL_SIGN,
			accessors,fields);

    break;

  case SQL_C_INTERVAL_SECOND :

    accessors[0] = SECOND_INTERVAL_SECOND;
    fields[0] = getIntervalSecond;

    writeIntervalToBuff(buffer,argv[1],numElts,SQL_IS_SECOND,
			1,SECOND_INTERVAL_SIGN,
			accessors,fields);

    break;

  case SQL_C_INTERVAL_YEAR_TO_MONTH :

    accessors[0] = YEAR_TO_MONTH_INTERVAL_YEAR;
    accessors[1] = YEAR_TO_MONTH_INTERVAL_MONTH;

    fields[0] = getIntervalYear;
    fields[1] = getIntervalMonth;

    writeIntervalToBuff(buffer,argv[1],numElts,SQL_IS_YEAR_TO_MONTH,
			2,YEAR_TO_MONTH_INTERVAL_SIGN,
			accessors,fields);

    break;


  case SQL_C_INTERVAL_DAY_TO_HOUR :

    accessors[0] = DAY_TO_HOUR_INTERVAL_DAY;
    accessors[1] = DAY_TO_HOUR_INTERVAL_HOUR;

    fields[0] = getIntervalDay;
    fields[1] = getIntervalHour;

    writeIntervalToBuff(buffer,argv[1],numElts,SQL_IS_DAY_TO_HOUR,
			2,DAY_TO_HOUR_INTERVAL_SIGN,
			accessors,fields);

    break;


  case SQL_C_INTERVAL_DAY_TO_MINUTE :

    accessors[0] = DAY_TO_MINUTE_INTERVAL_DAY;
    accessors[1] = DAY_TO_MINUTE_INTERVAL_HOUR;
    accessors[2] = DAY_TO_MINUTE_INTERVAL_MINUTE;

    fields[0] = getIntervalDay;
    fields[1] = getIntervalHour;
    fields[2] = getIntervalMinute;

    writeIntervalToBuff(buffer,argv[1],numElts,SQL_IS_DAY_TO_MINUTE,
			3,DAY_TO_MINUTE_INTERVAL_SIGN,
			accessors,fields);
			
    break;

  case SQL_C_INTERVAL_DAY_TO_SECOND :

    accessors[0] = DAY_TO_SECOND_INTERVAL_DAY;
    accessors[1] = DAY_TO_SECOND_INTERVAL_HOUR;
    accessors[2] = DAY_TO_SECOND_INTERVAL_MINUTE;
    accessors[3] = DAY_TO_SECOND_INTERVAL_SECOND;

    fields[0] = getIntervalDay;
    fields[1] = getIntervalHour;
    fields[2] = getIntervalMinute;
    fields[3] = getIntervalSecond;

    writeIntervalToBuff(buffer,argv[1],numElts,SQL_IS_DAY_TO_SECOND,
			4,DAY_TO_SECOND_INTERVAL_SIGN,
			accessors,fields);

			
  case SQL_C_INTERVAL_HOUR_TO_MINUTE :

    accessors[0] = HOUR_TO_MINUTE_INTERVAL_HOUR;
    accessors[1] = HOUR_TO_MINUTE_INTERVAL_MINUTE;

    fields[0] = getIntervalHour;
    fields[1] = getIntervalMinute;

    writeIntervalToBuff(buffer,argv[1],numElts,SQL_IS_HOUR_TO_MINUTE,
			2,HOUR_TO_MINUTE_INTERVAL_SIGN,
			accessors,fields);

    break;


  case SQL_C_INTERVAL_HOUR_TO_SECOND :

    accessors[0] = HOUR_TO_SECOND_INTERVAL_HOUR;
    accessors[1] = HOUR_TO_SECOND_INTERVAL_MINUTE;
    accessors[2] = HOUR_TO_SECOND_INTERVAL_SECOND;

    fields[0] = getIntervalHour;
    fields[1] = getIntervalMinute;
    fields[2] = getIntervalSecond;

    writeIntervalToBuff(buffer,argv[1],numElts,SQL_IS_HOUR_TO_SECOND,
			3,HOUR_TO_SECOND_INTERVAL_SIGN,
			accessors,fields);

    break;

  case SQL_C_INTERVAL_MINUTE_TO_SECOND :

    accessors[0] = MINUTE_TO_SECOND_INTERVAL_MINUTE;
    accessors[1] = MINUTE_TO_SECOND_INTERVAL_SECOND;

    fields[0] = getIntervalMinute;
    fields[1] = getIntervalSecond;

    writeIntervalToBuff(buffer,argv[1],numElts,SQL_IS_MINUTE_TO_SECOND,
			2,MINUTE_TO_SECOND_INTERVAL_SIGN,
			accessors,fields);

    break;

  }

  return scheme_void;
}

Scheme_Object *srp_SQLLenBinaryAttr(int argc,Scheme_Object **argv) {
  long intVal;

  if (SCHEME_INTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-len-binary-attr","int",0,argc,argv);
  }

  if (scheme_get_int_val(argv[0],&intVal) == 0) {
    scheme_signal_error("sql-len-binary-attr: number too big");
  }

  // Scheme equivalent of SQL_LEN_BINARY_ATTR macro in SQLEXT.H

  return scheme_make_integer_value(-100L - intVal);
}

void scheme_add_prim_to_env(Scheme_Env *env,
			    Scheme_Object *(*f)(int,Scheme_Object **),
			    char *name,
			    short minArgs,short maxArgs) {
  Scheme_Object *pobj;

  pobj = scheme_make_prim_w_arity(f,name,minArgs,maxArgs);
    
  scheme_add_global(name,pobj,env);
}

char *nullableToString(SQLSMALLINT nullable) {
  
  switch(nullable) {
  case SQL_NO_NULLS :
    return "sql-no-nulls";
    
  case SQL_NULLABLE :
    return "sql-nullable";

  default :
    return "sql-nullable-unknown";
  }
}

int namedConstCmp(SRP_NAMED_CONSTANT *p1,SRP_NAMED_CONSTANT *p2) {
  return stricmp(p1->scheme_name,p2->scheme_name);
}

int namedTypedConstCmp(SRP_NAMED_TYPED_CONSTANT *p1,SRP_NAMED_TYPED_CONSTANT *p2) {
  return stricmp(p1->scheme_name,p2->scheme_name);
}

BOOL isSmallInt(Scheme_Object *s) {
  long val;
  short smallVal;

  if (SCHEME_INTP(s) == FALSE) {
    return FALSE;
  }

  val = SCHEME_INT_VAL(s);

  smallVal = (short)val;

  if (smallVal != val) {
    return FALSE;
  }
    
  return TRUE;
}

BOOL isUnsignedInt(Scheme_Object *obj) {
  unsigned long val;

  if (SCHEME_EXACT_INTEGERP(obj) == FALSE ||
      scheme_get_unsigned_int_val(obj,&val) == 0) {
    return FALSE;
  }

  return TRUE;
}

BOOL isUnsignedSmallInt(Scheme_Object *obj) {
  long val;
  unsigned short smallVal;

  if (SCHEME_INTP(obj) == FALSE) {
    return FALSE;
  }

  val = SCHEME_INT_VAL(obj);

  smallVal = (unsigned short)val;

  if (smallVal != val) {
    return FALSE;
  }
    
  return TRUE;
}

BOOL isCharInt(Scheme_Object *s) {
  long val;
  char charVal;

  if (SCHEME_INTP(s) == FALSE) {
    return FALSE;
  }

  val = SCHEME_INT_VAL(s);

  charVal = (char)val;

  if (charVal != val) {
    return FALSE;
  }
    
  return TRUE;
}

BOOL isUnsignedCharInt(Scheme_Object *s) {
  unsigned long val;
  unsigned char charVal;

  if (SCHEME_INTP(s) == FALSE) {
    return FALSE;
  }

  val = SCHEME_INT_VAL(s);

  charVal = (unsigned char)val;

  if (charVal != val) {
    return FALSE;
  }
    
  return TRUE;
}

void checkSQLReturn(SQLRETURN sr,char *f) {

  switch (sr) {

  case SQL_SUCCESS :
    return;

  case SQL_NO_DATA :
    scheme_signal_error("SQL_NO_DATA error in %s",f);

  case SQL_INVALID_HANDLE :
    scheme_signal_error("SQL_INVALID_HANDLE error in %s",f);

  case SQL_ERROR :
    scheme_signal_error("Unspecified error in %s",f);

    /*  case SQL_NEED_DATA :
    scheme_signal_error("SQL_NEED_DATA error in %s",f);
    */

  case SQL_SUCCESS_WITH_INFO :
    return;

  default :
    ;
  }

}

char *sqlReturnToString(SQLRETURN sr) {

 switch (sr) {

  case SQL_SUCCESS :
    return "sql-success";

  case SQL_NO_DATA :
    return "sql-no-data";

  case SQL_INVALID_HANDLE :
    return "sql-invalid-handle";

  case SQL_ERROR :
    return "sql-error";

 case SQL_NEED_DATA :
   return "sql-need-data";
 
 case SQL_SUCCESS_WITH_INFO :
   return "sql-success-with-info";

  default :
    return "?";
 }
}

// actual ODBC procedures

// Functions in SQL.H

Scheme_Object *srp_SQLAllocConnect(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHENV envHandle;
  SQLHDBC connectHandle;
  SRP_SQL_HDBC *retval;

  if (SQL_HENVP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-alloc-connect","sql_henv",0,argc,argv);
  }

  envHandle = SQL_HENV_VAL(argv[0]);

  sr = SQLAllocConnect(envHandle,&connectHandle);

  checkSQLReturn(sr,"sql-alloc-connect");

  retval = (SRP_SQL_HDBC *)scheme_malloc(sizeof(SRP_SQL_HDBC));
  retval->type = sql_hdbc_type;
  retval->hdbc = connectHandle;

  return (Scheme_Object *)retval;
}

Scheme_Object *srp_SQLAllocEnv(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHENV envHandle;
  SRP_SQL_HENV *retval;

  sr = SQLAllocEnv(&envHandle);

  checkSQLReturn(sr,"sql-alloc-env");

  retval = (SRP_SQL_HENV *)scheme_malloc(sizeof(SRP_SQL_HENV));
  retval->type = sql_henv_type;
  retval->henv = envHandle;

  return (Scheme_Object *)retval;
}

Scheme_Object *srp_SQLAllocHandle(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  char *handleTypeString;

  if (SCHEME_SYMBOLP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-alloc-handle","symbol",0,argc,argv);
  }

  handleTypeString = SCHEME_SYM_VAL(argv[0]);

  if (stricmp(handleTypeString,"sql-handle-env") == 0) {
    SQLHENV envHandle;
    SRP_SQL_HENV *retval;
    
    if (argc > 1) {
      scheme_signal_error("In sql-alloc-handle, when first argument is "
			  "'sql-handle-env, no other argument is allowed"); 
    }

    sr = SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&envHandle);

    checkSQLReturn(sr,"sql-alloc-handle");

    retval = (SRP_SQL_HENV *)scheme_malloc(sizeof(SRP_SQL_HENV));
    retval->type = sql_henv_type;
    retval->henv = envHandle;

    return (Scheme_Object *)retval;

  }

  if (argc < 2) {
    scheme_signal_error("sql-alloc-handle: unless first argument "
			"is 'sql-handle-env, second argument required"); 
  }

  if (stricmp(handleTypeString,"sql-handle-dbc") == 0) {
    SQLHDBC dbcHandle;
    SRP_SQL_HDBC *retval;
    
    if (SQL_HENVP(argv[1]) == FALSE) {
      scheme_wrong_type("sql-alloc-handle","sql-henv",1,argc,argv);
    }

    sr = SQLAllocHandle(SQL_HANDLE_DBC,SQL_HENV_VAL(argv[1]),&dbcHandle);

    checkSQLReturn(sr,"sql-alloc-handle");

    retval = (SRP_SQL_HDBC *)scheme_malloc(sizeof(SRP_SQL_HDBC));
    retval->type = sql_hdbc_type;
    retval->hdbc = dbcHandle;

    return (Scheme_Object *)retval;

  }

  if (stricmp(handleTypeString,"sql-handle-stmt") == 0) {
    SQLHSTMT stmtHandle;
    SRP_SQL_HSTMT *retval;
    
    if (SQL_HDBCP(argv[1]) == FALSE) {
      scheme_wrong_type("sql-alloc-handle","sql-hdbc",1,argc,argv);
    }

    sr = SQLAllocHandle(SQL_HANDLE_STMT,SQL_HDBC_VAL(argv[1]),&stmtHandle);

    checkSQLReturn(sr,"sql-alloc-handle");

    retval = (SRP_SQL_HSTMT *)scheme_malloc(sizeof(SRP_SQL_HSTMT));
    retval->type = sql_hstmt_type;
    retval->hstmt = stmtHandle;

    return (Scheme_Object *)retval;

  }

  if (stricmp(handleTypeString,"sql-handle-desc") == 0) {
    SQLHDESC descHandle;
    SRP_SQL_HDESC *retval;
    
    if (SQL_HDBCP(argv[1]) == FALSE) {
      scheme_wrong_type("sql-alloc-handle","sql-hdbc",1,argc,argv);
    }

    sr = SQLAllocHandle(SQL_HANDLE_DESC,SQL_HDBC_VAL(argv[1]),&descHandle);

    checkSQLReturn(sr,"sql-alloc-handle");

    retval = (SRP_SQL_HDESC *)scheme_malloc(sizeof(SRP_SQL_HDESC));
    retval->type = sql_hdesc_type;
    retval->descType = EXPLICIT;
    retval->hdesc = descHandle;

    return (Scheme_Object *)retval;

  }

  scheme_signal_error("Handle type must be one of "
		      "'sql-handle-env, " 
		      "'sql-handle-dbc, "
		      "'sql-handle-stmt, or "
		      "'sql-handle-desc");

  return scheme_void; // unreachable
}

Scheme_Object *srp_SQLAllocStmt(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHDBC connectHandle;
  SQLHSTMT stmtHandle;
  SRP_SQL_HSTMT *retval;

  if (SQL_HDBCP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-alloc-stmt","sql_hdbc",0,argc,argv);
  }

  connectHandle = SQL_HDBC_VAL(argv[0]);

  sr = SQLAllocStmt(connectHandle,&stmtHandle);

  checkSQLReturn(sr,"sql-alloc-stmt");

  retval = (SRP_SQL_HSTMT *)scheme_malloc(sizeof(SRP_SQL_HSTMT));
  retval->type = sql_hstmt_type;
  retval->hstmt = stmtHandle;

  return (Scheme_Object *)retval;
}

Scheme_Object *srp_SQLBindCol(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  int colNumber;
  void *buffer;
  long buflen;
  int buftype;
  SQLINTEGER *indicator;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-bind-col","sql_hstmt",0,argc,argv);
  }

  if (isUnsignedSmallInt(argv[1]) == FALSE) {
    scheme_wrong_type("sql-bind-col","unsigned-small-int",1,argc,argv);
  }
   
  if (SQL_BUFFERP(argv[2]) == FALSE) {
    scheme_wrong_type("sql-bind-col","sql-buffer",2,argc,argv);
  }

  if (SQL_INDICATORP(argv[3]) == FALSE) {
    scheme_wrong_type("sql-bind-col","sql-indicator",3,argc,argv);
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);

  colNumber = SCHEME_INT_VAL(argv[1]);

  buffer = SQL_BUFFER_VAL(argv[2]);
  buflen = SQL_BUFFER_LEN(argv[2]);
  buftype = SQL_BUFFER_CTYPE(argv[2]);

  indicator = &SQL_INDICATOR_VAL(argv[3]);

  sr = SQLBindCol(stmtHandle,colNumber,buftype,buffer,buflen,indicator);

  checkSQLReturn(sr,"sql-bind-col");

  return scheme_void;
}

Scheme_Object *srp_SQLBindParam(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SRP_NAMED_SMALL_CONSTANT *p;
  char *SQLTypeName;
  SQLSMALLINT CTypeVal,SQLTypeVal;
  short paramNum;
  short decimalDigits;
  unsigned long lengthPrecision;
  SQLPOINTER buffer;
  SQLINTEGER indicator;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-bind-param","sql-hstmt",0,argc,argv);
  }

  if (isSmallInt(argv[1]) == FALSE) {
    scheme_wrong_type("sql-bind-param","small-int",1,argc,argv);
  }    

  if (SCHEME_SYMBOLP(argv[2]) == FALSE) {
    scheme_wrong_type("sql-bind-param","symbol",2,argc,argv);
  }
   
  if (SCHEME_EXACT_INTEGERP(argv[3]) == FALSE) {
    scheme_wrong_type("sql-bind-param","int",3,argc,argv);
  }
   
  if (isSmallInt(argv[4]) == FALSE) {
    scheme_wrong_type("sql-bind-param","int",4,argc,argv);
  }
   
  if (SQL_BUFFERP(argv[5]) == FALSE) {
    scheme_wrong_type("sql-bind-param","sql-buffer",5,argc,argv);
  }
   
  if (SQL_INDICATORP(argv[6]) == FALSE) {
    scheme_wrong_type("sql-bind-param","sql-indicator",6,argc,argv);
  }
   
  stmtHandle = SQL_HSTMT_VAL(argv[0]);

  paramNum = (short)(SCHEME_INT_VAL(argv[1]));

  SQLTypeName = SCHEME_SYM_VAL(argv[2]);

  p = namedSmallConstSearch(SQLTypeName,SQLDataTypes);
 
  if (p == NULL) {
    scheme_signal_error("sql-bind-col: invalid SQL data type name %s",SQLTypeName);
  }

  SQLTypeVal = (SQLSMALLINT)(p->val);

  scheme_get_unsigned_int_val(argv[3],&lengthPrecision);

  decimalDigits = (short)SCHEME_INT_VAL(argv[4]);

  CTypeVal = SQL_BUFFER_CTYPE(argv[5]);

  buffer = SQL_BUFFER_VAL(argv[5]);

  indicator = SQL_INDICATOR_VAL(argv[6]);

  sr = SQLBindParam(stmtHandle,paramNum,CTypeVal,SQLTypeVal,
		    lengthPrecision,decimalDigits,buffer,
		    &indicator);

  checkSQLReturn(sr,"sql-bind-param");

  return scheme_void;
}

Scheme_Object *srp_SQLCancel(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-cancel","sql-hstmt",0,argc,argv);
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);

  sr = SQLCancel(stmtHandle);

  checkSQLReturn(sr,"sql-cancel");
  
  return scheme_void;
}

Scheme_Object *srp_SQLCloseCursor(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-close-cursor","sql-hstmt",0,argc,argv);
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);

  sr = SQLCloseCursor(stmtHandle);

  checkSQLReturn(sr,"sql-close-cancel");
  
  return scheme_void;
}

Scheme_Object *srp_SQLColAttribute(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLUSMALLINT colNumber;
  SQLUSMALLINT fieldId;
  char *fieldIdString;
  char buff[2048];
  SQLSMALLINT bufflen;
  SQLINTEGER numBuffer;
  SQLSMALLINT actualLen;
  SRP_NAMED_TYPED_CONSTANT *p;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-col-attribute","sql-hstmt",0,argc,argv);
  }

  if (isSmallInt(argv[1]) == FALSE) {
    scheme_wrong_type("sql-col-attribute","small-int",1,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[2]) == FALSE) {
    scheme_wrong_type("sql-col-attribute","symbol",2,argc,argv);
  }

  fieldIdString = SCHEME_SYM_VAL(argv[2]);

  p = namedTypedConstSearch(fieldIdString,colAttributes);

  if (p == NULL) {
    scheme_signal_error("Invalid column attribute: %s",fieldIdString);
  }
    
  fieldId = (SQLUSMALLINT)(p->val);
  stmtHandle = SQL_HSTMT_VAL(argv[0]);
  colNumber = (SQLSMALLINT)SCHEME_INT_VAL(argv[1]);

  switch(p->type) {

  case sqlbool :

    bufflen = SQL_IS_INTEGER;
    sr = SQLColAttribute(stmtHandle,colNumber,fieldId,
			 buff,bufflen,&actualLen,&numBuffer);
    checkSQLReturn(sr,"sql-col-attribute");		       
    return (numBuffer == SQL_FALSE) ? scheme_false : scheme_true;

  case sqlinteger :

    bufflen = SQL_IS_INTEGER;
    sr = SQLColAttribute(stmtHandle,colNumber,fieldId,
			 buff,bufflen,&actualLen,&numBuffer);
    checkSQLReturn(sr,"sql-col-attribute");		       
    return scheme_make_integer_value((long)numBuffer);

  case namedinteger :

    bufflen = SQL_IS_INTEGER;
    sr = SQLColAttribute(stmtHandle,colNumber,fieldId,
			 buff,bufflen,&actualLen,&numBuffer);
    checkSQLReturn(sr,"sql-col-attribute");		       

    return scheme_intern_symbol(findIntegerName(fieldIdString,numBuffer,
						namedColAttrIntegers,
						sizeray(namedColAttrIntegers)));

  case string :

    bufflen = sizeof(buff);
    sr = SQLColAttribute(stmtHandle,colNumber,fieldId,
			 buff,bufflen,&actualLen,&numBuffer);
    checkSQLReturn(sr,"sql-col-attribute");		       
    return scheme_make_sized_string(buff,actualLen,TRUE);
  }

  scheme_signal_error("sql-col-attribute: invalid attribute type");

  return scheme_void; // unreachable
}

Scheme_Object *srp_SQLColumns(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLCHAR *catalog;
  SQLSMALLINT catalogLen;
  SQLCHAR *schema;
  SQLSMALLINT schemaLen;
  SQLCHAR *table;
  SQLSMALLINT tableLen;
  SQLCHAR *column;
  SQLSMALLINT columnLen;
  int i;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-columns","sql-hstmt",0,argc,argv);
  }

  for (i = 1; i <= 4; i++) {
    if (SCHEME_STRINGP(argv[i]) == FALSE) {
      scheme_wrong_type("sql-columns","string",i,argc,argv);
    }
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);
  catalog = (SQLCHAR *)SCHEME_STR_VAL(argv[1]);
  catalogLen = SCHEME_STRLEN_VAL(argv[1]);
  schema = (SQLCHAR *)SCHEME_STR_VAL(argv[2]);
  schemaLen = SCHEME_STRLEN_VAL(argv[2]);
  table = (SQLCHAR *)SCHEME_STR_VAL(argv[3]);
  tableLen = SCHEME_STRLEN_VAL(argv[3]);
  column = (SQLCHAR *)SCHEME_STR_VAL(argv[4]);
  columnLen = SCHEME_STRLEN_VAL(argv[4]);

  sr = SQLColumns(stmtHandle,
		  catalog,catalogLen,
		  schema,schemaLen,
		  table,tableLen,
		  column,columnLen);

  checkSQLReturn(sr,"sql-columns");		       

  return scheme_void;
}

Scheme_Object *srp_SQLConnect(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHDBC dbcHandle;
  SQLCHAR *server;
  SQLSMALLINT serverLen;
  SQLCHAR *user;
  SQLSMALLINT userLen;
  SQLCHAR *password;
  SQLSMALLINT passwordLen;
  int i;

  if (SQL_HDBCP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-columns","sql-hdbc",0,argc,argv);
  }

  for (i = 1; i <= 3; i++) {
    if (SCHEME_STRINGP(argv[i]) == FALSE) {
      scheme_wrong_type("sql-columns","string",i,argc,argv);
    }
  }

  dbcHandle = SQL_HDBC_VAL(argv[0]);
  server = (SQLCHAR *)SCHEME_STR_VAL(argv[1]);
  serverLen = SCHEME_STRLEN_VAL(argv[1]);
  user = (SQLCHAR *)SCHEME_STR_VAL(argv[2]);
  userLen = SCHEME_STRLEN_VAL(argv[2]);
  password = (SQLCHAR *)SCHEME_STR_VAL(argv[3]);
  passwordLen = SCHEME_STRLEN_VAL(argv[3]);

  sr = SQLConnect(dbcHandle,
		  server,serverLen,
		  user,userLen,
		  password,passwordLen);
		  
  checkSQLReturn(sr,"sql-connect");		       

  return scheme_void;
}

Scheme_Object *srp_SQLCopyDesc(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHDESC srcDescHandle,targetDescHandle;
  int i;

  for (i = 0; i <= 1; i++) {
    if (SQL_HDESCP(argv[i]) == FALSE) {
      scheme_wrong_type("sql-copy-desc","sql-hdesc",i,argc,argv);
    }
  }

  srcDescHandle = SQL_HDESC_VAL(argv[0]);
  targetDescHandle = SQL_HDESC_VAL(argv[1]);

  sr = SQLCopyDesc(srcDescHandle,targetDescHandle);

  checkSQLReturn(sr,"sql-copy-desc");		       

  return scheme_void;
}

Scheme_Object *srp_SQLDataSources(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHENV envHandle;
  SQLUSMALLINT direction;
  char *directionString;
  SQLCHAR server[SQL_MAX_DSN_LENGTH + 1];
  SQLCHAR description[SQL_MAX_DSN_LENGTH + 1];
  SQLSMALLINT serverLen,descriptionLen;
  SRP_NAMED_SMALL_CONSTANT *p;
  Scheme_Object *retval;

  if (SQL_HENVP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-data-sources","sql-henv",0,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-data-sources","symbol",0,argc,argv);
  }

  directionString = SCHEME_SYM_VAL(argv[1]);

  p = namedSmallConstSearch(directionString,fetchDirections);

  if (p == NULL) {
    scheme_signal_error("sql-data-sources: invalid direction: %s",
			directionString);
  }

  direction = p->val;

  envHandle = SQL_HENV_VAL(argv[0]);

  sr = SQLDataSources(envHandle,direction,
		      server,sizeof(server),&serverLen,
		      description,sizeof(description),&descriptionLen);

  checkSQLReturn(sr,"sql-data-sources");		       

  retval = scheme_make_pair(scheme_make_sized_string((char *)description,
						     descriptionLen,TRUE),
			    scheme_null);
  retval = scheme_make_pair(scheme_make_sized_string((char *)server,
						     serverLen,TRUE),
			    retval);

  return retval;
			  
}

Scheme_Object *srp_SQLDescribeCol(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLSMALLINT colNumber;
  SQLCHAR columnName[256];
  SQLSMALLINT colNameLen;
  SQLSMALLINT dataType;
  SQLUINTEGER colSize;
  SQLSMALLINT decimalDigits;
  SQLSMALLINT nullable;
  char *nullableString;
  char *dataTypeString;
  int i;
  Scheme_Object *retval;
  
  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-describe-col","sql-hstmt",0,argc,argv);
  }

  if (isUnsignedSmallInt(argv[1]) == FALSE) {
    scheme_wrong_type("sql-describe-col","unsigned-small-int",0,argc,argv);
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);
  colNumber = (SQLUSMALLINT)SCHEME_INT_VAL(argv[1]);

  sr = SQLDescribeCol(stmtHandle,colNumber,
		      columnName,sizeof(columnName),&colNameLen,
		      &dataType,&colSize,&decimalDigits,
		      &nullable);

  checkSQLReturn(sr,"sql-describe-col");		       

  dataTypeString = "?";

  for (i = 0; i < sizeray(SQLDataTypes); i++) {
    if (dataType == SQLDataTypes[i].val) {
      dataTypeString = SQLDataTypes[i].scheme_name;
      break;
    }
  }

  nullableString = nullableToString(nullable);

  retval = scheme_null;
  retval = scheme_make_pair(scheme_intern_symbol(nullableString),retval);
  retval = scheme_make_pair(scheme_make_integer_value(decimalDigits),retval);
  retval = scheme_make_pair(scheme_make_integer_value_from_unsigned(colSize),retval);
  retval = scheme_make_pair(scheme_intern_symbol(dataTypeString),retval);
  retval = scheme_make_pair(scheme_make_sized_string((char *)columnName,
						     colNameLen,TRUE),retval);
  
  return retval;
}

Scheme_Object *srp_SQLDisconnect(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHDBC dbcHandle;

  if (SQL_HDBCP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-disconnect","sql-hdbc",0,argc,argv);
  }

  dbcHandle = SQL_HDBC_VAL(argv[0]);

  sr = SQLDisconnect(dbcHandle);

  checkSQLReturn(sr,"sql-disconnect");
  
  return scheme_void;
}

Scheme_Object *srp_SQLEndTran(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLSMALLINT actionType;
  char *action;

  if (SQL_HDBCP(argv[0]) == FALSE && SQL_HENVP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-end-tran","sql-hdbc> or <sql-henv",0,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-end-tran","symbol",1,argc,argv);
  }

  action = SCHEME_SYM_VAL(argv[1]);

  if (stricmp(action,"sql-commit") == 0) {
    actionType = SQL_COMMIT;
  }
  else if (stricmp(action,"sql-rollback") == 0) {
    actionType = SQL_ROLLBACK;
  }
  else {
    scheme_signal_error("sql-end-tran: invalid completion type: %s",
			action);
  }

  if (SQL_HDBCP(argv[0])) {
    sr = SQLEndTran(SQL_HANDLE_DBC,SQL_HDBC_VAL(argv[0]),actionType);
  }
  else if (SQL_HENVP(argv[0])) {
    sr = SQLEndTran(SQL_HANDLE_ENV,SQL_HENV_VAL(argv[0]),actionType);
  }

  checkSQLReturn(sr,"sql-end-tran");
  
  return scheme_void;
}

Scheme_Object *srp_SQLError(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHENV envHandle;
  SQLHDBC connectionHandle;
  SQLHSTMT stmtHandle;
  SQLCHAR state[6];
  SQLINTEGER nativeError;
  SQLCHAR text[2048];
  SQLSMALLINT textLen;
  Scheme_Object *retval;

  if (SQL_HENVP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-error","sql-henv",0,argc,argv);
  }

  if (SQL_HDBCP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-error","sql-hdbc",1,argc,argv);
  }

  if (SQL_HSTMTP(argv[2]) == FALSE) {
    scheme_wrong_type("sql-error","sql-hstmt",2,argc,argv);
  }

  envHandle = SQL_HENV_VAL(argv[0]);
  connectionHandle = SQL_HDBC_VAL(argv[1]);
  stmtHandle = SQL_HSTMT_VAL(argv[2]);

  sr = SQLError(envHandle,connectionHandle,stmtHandle,
		state,&nativeError,
		text,sizeof(text),&textLen);
		
  checkSQLReturn(sr,"sql-error");

  retval = scheme_null;
  retval = scheme_make_pair(scheme_make_sized_string((char *)text,textLen,TRUE),
			    retval);
  retval = scheme_make_pair(scheme_make_integer_value(nativeError),
			    retval);
  retval = scheme_make_pair(scheme_make_string((const char *)state),
			    retval);

  return retval;
}

Scheme_Object *srp_SQLExecDirect(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLCHAR *stmt;
  SQLINTEGER stmtLen;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-exec-direct","sql-hstmt",0,argc,argv);
  }

  if (SCHEME_STRINGP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-exec-direct","string",1,argc,argv);
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);
  stmt = (SQLCHAR *)SCHEME_STR_VAL(argv[1]);
  stmtLen = SCHEME_STRLEN_VAL(argv[1]);

  sr = SQLExecDirect(stmtHandle,stmt,stmtLen);

  checkSQLReturn(sr,"sql-exec-direct");  

  return scheme_void;
}

Scheme_Object *srp_SQLExecute(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-execute","sql-hstmt",0,argc,argv);
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);

  sr = SQLExecute(stmtHandle);

  checkSQLReturn(sr,"sql-execute");
  
  return scheme_void;
}

Scheme_Object *srp_SQLFetch(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-fetch","sql-hstmt",0,argc,argv);
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);

  sr = SQLFetch(stmtHandle);

  checkSQLReturn(sr,"sql-fetch");
  
  return scheme_void;
}

Scheme_Object *srp_SQLFetchScroll(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLINTEGER offset;
  char *orientationString;
  SRP_NAMED_SMALL_CONSTANT *p;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-fetch-scroll","sql-hstmt",0,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-fetch-scroll","symbol",1,argc,argv);
  }

  orientationString = SCHEME_SYM_VAL(argv[1]);

  p = namedSmallConstSearch(orientationString,fetchScrolls);

  if (p == NULL) {
    scheme_signal_error("sql-fetch-scroll: invalid orientation: %s",
			orientationString);
  } 

  if (p->val == SQL_FETCH_ABSOLUTE || p->val == SQL_FETCH_RELATIVE || 
      p->val == SQL_FETCH_BOOKMARK) {
    if (argc != 3) {
      scheme_signal_error("sql-fetch-scroll: given orientation %s "
			  "requires offset",
			  orientationString);
    }
    if (SCHEME_EXACT_INTEGERP(argv[2]) == FALSE) {
      scheme_wrong_type("sql-fetch-scroll","int",2,argc,argv);
      if (scheme_get_int_val(argv[2],&offset) == 0) {
	scheme_signal_error("sql-fetch-scroll: offset too large");
      }
    }
  }
  else {
    offset = 0;
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);
  offset = SCHEME_INT_VAL(argv[1]);

  sr = SQLFetchScroll(stmtHandle,p->val,offset);

  checkSQLReturn(sr,"sql-fetch-scroll");  

  return scheme_void;
}

Scheme_Object *srp_SQLFreeConnect(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHDBC connectionHandle;

  if (SQL_HDBCP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-free-connect","sql-hdbc",0,argc,argv);
  }

  connectionHandle = SQL_HDBC_VAL(argv[0]);

  sr = SQLFreeConnect(connectionHandle);

  checkSQLReturn(sr,"sql-free-connect");  

  return scheme_void;
}

Scheme_Object *srp_SQLFreeEnv(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHENV envHandle;

  if (SQL_HENVP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-free-env","sql-henv",0,argc,argv);
  }

  envHandle = SQL_HENV_VAL(argv[0]);

  sr = SQLFreeEnv(envHandle);

  checkSQLReturn(sr,"sql-free-env");  

  return scheme_void;
}

Scheme_Object *srp_SQLFreeHandle(int argc,Scheme_Object **argv) {
  SQLRETURN sr;

  if (SQL_HENVP(argv[0])) {
    sr = SQLFreeHandle(SQL_HANDLE_ENV,SQL_HENV_VAL(argv[0]));
  }
  else if (SQL_HDBCP(argv[0])) {
    sr = SQLFreeHandle(SQL_HANDLE_DBC,SQL_HDBC_VAL(argv[0]));
  }
  else if (SQL_HSTMTP(argv[0])) {
    sr = SQLFreeHandle(SQL_HANDLE_STMT,SQL_HSTMT_VAL(argv[0]));
  }
  else if (SQL_HDESCP(argv[0])) {
    sr = SQLFreeHandle(SQL_HANDLE_DESC,SQL_HDESC_VAL(argv[0]));
  }
  else {
    scheme_wrong_type("sql-free-handle",
		      "sql-henv> or <sql-hdbc> or <sql-hstmt> or <sql-hdesc",
		      0,argc,argv);
  }
    
  checkSQLReturn(sr,"sql-free-handle");  

  return scheme_void;
}

Scheme_Object *srp_SQLFreeStmt(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT hstmt;
  SQLUSMALLINT option;
  char *optionString;
  SRP_NAMED_SMALL_CONSTANT *p;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-free-stmt","sql-hstmt",0,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-free-stmt","symbol",1,argc,argv);
  }

  optionString = SCHEME_SYM_VAL(argv[1]);

  p = namedSmallConstSearch(optionString,stmtFreeOptions);

  if (p == NULL) {
    scheme_signal_error("sql-free-stmt: invalid option: %s",optionString);
  }

  option = p->val;

  hstmt = SQL_HSTMT_VAL(argv[0]);

  sr = SQLFreeStmt(hstmt,option);

  checkSQLReturn(sr,"sql-free-stmt");  

  return scheme_void;
}

Scheme_Object *srp_SQLGetConnectAttr(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  HDBC connectionHandle;
  SQLINTEGER attribute;
  char *attributeString;
  SRP_CONST_TYPE attributeType;
  SQLUINTEGER number;
  char buff[2048];
  SQLINTEGER actualLen;
  SRP_NAMED_TYPED_CONSTANT *p;

  if (SQL_HDBCP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-get-connect-attr","sql-hdbc",0,argc,argv);
  }
  
  if (SCHEME_SYMBOLP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-get-connect-attr","symbol",1,argc,argv);
  }

  attributeString = SCHEME_SYM_VAL(argv[1]);

  // need to check settable and read-only attributes

  p = namedTypedConstSearch(attributeString,settableConnectionAttributes);
  
  if (p == NULL) {

    p = namedTypedConstSearch(attributeString,readOnlyConnectionAttributes);

    if (p == NULL) {
      scheme_signal_error("sql-get-connect-attr: invalid attribute: %s",
			  attributeString);
    }
  }

  attribute = p->val;
  attributeType = p->type;

  connectionHandle = SQL_HDBC_VAL(argv[0]);

  switch(attributeType) {

  case sqlbool :

    sr = SQLGetConnectAttr(connectionHandle,attribute,&number,0,&actualLen);
    checkSQLReturn(sr,"sql-get-connect-attr");  			 

    return (number == SQL_FALSE) ? scheme_false : scheme_true;

  case string :

    sr = SQLGetConnectAttr(connectionHandle,attribute,buff,sizeray(buff),&actualLen);
    checkSQLReturn(sr,"sql-get-connect-attr");  			 

    return scheme_make_sized_string(buff,actualLen,TRUE);

  case sqluinteger :

    sr = SQLGetConnectAttr(connectionHandle,attribute,&number,0,&actualLen);
    checkSQLReturn(sr,"sql-get-connect-attr");  			 

    return scheme_make_integer_value_from_unsigned(number);

  case nameduinteger :

    sr = SQLGetConnectAttr(connectionHandle,attribute,&number,0,&actualLen);
    checkSQLReturn(sr,"sql-get-connect-attr");  			 

    return scheme_intern_symbol(findIntegerName(attributeString,number,
						namedConnectAttrIntegers,
						sizeray(namedConnectAttrIntegers)));

  }
  
  return scheme_void;
}

Scheme_Object *srp_SQLGetConnectOption(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  HDBC connectionHandle;
  SQLUSMALLINT option;
  char *optionString;
  SRP_CONST_TYPE optionType;
  SQLUINTEGER number;
  char buff[2048];
  SRP_NAMED_TYPED_CONSTANT *p;

  if (SQL_HDBCP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-get-connect-option","sql-hdbc",0,argc,argv);
  }
  
  if (SCHEME_SYMBOLP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-get-connect-option","symbol",1,argc,argv);
  }

  optionString = SCHEME_SYM_VAL(argv[1]);

  // need to check settable and read-only options

  p = namedTypedConstSearch(optionString,settableConnectionAttributes);
  
  if (p == NULL) {

    p = namedTypedConstSearch(optionString,readOnlyConnectionAttributes);

    if (p == NULL) {
      scheme_signal_error("sql-get-connect-option: invalid option: %s",
			  optionString);
    }
  }

  option = (SQLUSMALLINT)(p->val);
  optionType = p->type;

  connectionHandle = SQL_HDBC_VAL(argv[0]);

  switch(optionType) {

  case sqlbool :

    sr = SQLGetConnectOption(connectionHandle,option,&number);
    checkSQLReturn(sr,"sql-get-connect-option");  			 

    return (number == SQL_FALSE) ? scheme_false : scheme_true;

  case string :

    sr = SQLGetConnectOption(connectionHandle,option,buff);
    checkSQLReturn(sr,"sql-get-connect-option");  			 

    return scheme_make_string(buff);

  case sqluinteger :

    sr = SQLGetConnectOption(connectionHandle,option,&number);
    checkSQLReturn(sr,"sql-get-connect-option");  			 

    return scheme_make_integer_value_from_unsigned(number);

  case nameduinteger :

    sr = SQLGetConnectOption(connectionHandle,option,&number);
    checkSQLReturn(sr,"sql-get-connect-option");  			 

    return scheme_intern_symbol(findIntegerName(optionString,number,
						namedConnectAttrIntegers,
						sizeray(namedConnectAttrIntegers)));

  case bitmask :

    sr = SQLGetConnectOption(connectionHandle,option,&number);
    checkSQLReturn(sr,"sql-get-connect-option");  			 

    return bitsListFromBitMask(optionString,number);
  }
  
  return scheme_void;
}

Scheme_Object *srp_SQLGetCursorName(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLCHAR name[1024];
  SQLSMALLINT actualLen;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-get-cursor-name","sql-hstmt",0,argc,argv);
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);

  sr = SQLGetCursorName(stmtHandle,name,sizeray(name),&actualLen);

  checkSQLReturn(sr,"sql-get-cursor-name");

  return scheme_make_sized_string((char *)name,actualLen,TRUE);
}

Scheme_Object *srp_SQLGetData(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLUSMALLINT colNumber;
  SQLPOINTER buffer;
  SQLINTEGER bufferlen;
  SQLSMALLINT buffertype;
  SQLINTEGER *indicator;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-get-data","sql-hstmt",0,argc,argv);
  }

  if (isUnsignedSmallInt(argv[1]) == FALSE) {
    scheme_wrong_type("sql-get-data","unsigned-small-int",1,argc,argv);
  }

  if (SQL_BUFFERP(argv[2]) == FALSE) {
    scheme_wrong_type("sql-get-data","sql-buffer",2,argc,argv);
  }

  if (SQL_INDICATORP(argv[3]) == FALSE) {
    scheme_wrong_type("sql-get-data","sql-indicator",3,argc,argv);
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);
  colNumber = (SQLUSMALLINT)SCHEME_INT_VAL(argv[1]);
  buffer = SQL_BUFFER_VAL(argv[2]);
  bufferlen = SQL_BUFFER_LEN(argv[2]);
  buffertype = SQL_BUFFER_CTYPE(argv[2]);

  indicator = &SQL_INDICATOR_VAL(argv[3]);

  sr = SQLGetData(stmtHandle,colNumber,buffertype,buffer,bufferlen,indicator);

  checkSQLReturn(sr,"sql-get-data");

  return scheme_void;
}

Scheme_Object *srp_SQLGetDescField(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHDESC descHandle;
  SQLSMALLINT recNumber;
  SQLSMALLINT fieldId;
  SRP_CONST_TYPE fieldType;
  char *fieldIdString;
  SQLINTEGER actualLen;
  SRP_NAMED_TYPED_CONSTANT *p;

  if (SQL_HDESCP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-get-desc-field","sql-hdesc",0,argc,argv);
  }

  if (isSmallInt(argv[1]) == FALSE) {
    scheme_wrong_type("sql-get-desc-field","small-int",1,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[2]) == FALSE) {
    scheme_wrong_type("sql-get-desc-field","symbol",2,argc,argv);
  }

  fieldIdString = SCHEME_SYM_VAL(argv[2]);

  p = namedTypedConstSearch(fieldIdString,fieldDescriptors);

  if (p == NULL) {
    scheme_signal_error("sql-get-desc-field: invalid field identifier: %s",
			fieldIdString);
  }

  fieldId = (SQLSMALLINT)(p->val);
  fieldType = p->type;

  descHandle = SQL_HDESC_VAL(argv[0]);
  recNumber = (SQLSMALLINT)SCHEME_INT_VAL(argv[1]);

  switch(fieldType) {

  case sqlsmallint :

    SQLSMALLINT smallIntVal;

    sr = SQLGetDescField(descHandle,recNumber,fieldId,
			 &smallIntVal,sizeof(SQLSMALLINT),&actualLen);

    checkSQLReturn(sr,"sql-get-desc-field");
 
    return scheme_make_integer(smallIntVal);

  case sqlinteger :

    SQLINTEGER intVal;

    sr = SQLGetDescField(descHandle,recNumber,fieldId,
			 &intVal,sizeof(SQLINTEGER),&actualLen);

    checkSQLReturn(sr,"sql-get-desc-field");

    return scheme_make_integer_value(intVal);
    
  case sqluinteger :

    SQLUINTEGER uintVal;

    sr = SQLGetDescField(descHandle,recNumber,fieldId,
			 &uintVal,sizeof(SQLUINTEGER),&actualLen);

    checkSQLReturn(sr,"sql-get-desc-field");

    return scheme_make_integer_value_from_unsigned(uintVal);
    
  case string :

    char buff[2048];

    sr = SQLGetDescField(descHandle,recNumber,fieldId,
			 buff,sizeof(buff),&actualLen);

    checkSQLReturn(sr,"sql-get-desc-field");

    return scheme_make_string(buff);
    
  case sqlbool :

    SQLINTEGER boolVal;

    sr = SQLGetDescField(descHandle,recNumber,fieldId,
			 &boolVal,sizeof(SQLINTEGER),&actualLen);

    checkSQLReturn(sr,"sql-get-desc-field");

    return (boolVal == SQL_FALSE) ? scheme_false : scheme_true;

  case namedinteger :
    SQLINTEGER namedIntVal;

    sr = SQLGetDescField(descHandle,recNumber,fieldId,
			 &namedIntVal,sizeof(SQLINTEGER),&actualLen);

    checkSQLReturn(sr,"sql-get-desc-field");

    return scheme_intern_symbol(findIntegerName(fieldIdString,namedIntVal,
						namedFieldDescriptors,
						sizeray(namedFieldDescriptors)));

  case sqlbuffer :
    SQLPOINTER p;
    SRP_SQL_BUFFER *pBuffer;

    sr = SQLGetDescField(descHandle,recNumber,fieldId,
			 &p,sizeof(SQLPOINTER),&actualLen);

    checkSQLReturn(sr,"sql-get-desc-field");

    if (p == NULL) {
      scheme_signal_error("NULL data pointer");
    }

    pBuffer = lookupBufferFromAddress(p);

    if (pBuffer == NULL) {
      scheme_signal_error("Internal buffer, can't be made into Scheme value");
    }

    return (Scheme_Object *)pBuffer;

  case sqlindicator :
    SQLINTEGER *pIntVal;
    SRP_SQL_INDICATOR *pIndicator;

    sr = SQLGetDescField(descHandle,recNumber,fieldId,
			 &pIntVal,sizeof(SQLINTEGER *),&actualLen);

    checkSQLReturn(sr,"sql-get-desc-field");

    pIndicator = (SRP_SQL_INDICATOR *)scheme_malloc_eternal(sizeof(SRP_SQL_INDICATOR));
    pIndicator->type = sql_indicator_type;
    pIndicator->value = *pIntVal;

    return (Scheme_Object *)pIndicator;

  case arraystatus :
    SQLUSMALLINT *pSmallUintVal;
    SRP_SQL_ARRAY_STATUS *pArrayStatus;

    sr = SQLGetDescField(descHandle,recNumber,fieldId,
			 &pSmallUintVal,sizeof(SQLUSMALLINT *),&actualLen);

    checkSQLReturn(sr,"sql-get-desc-field");

    pArrayStatus = (SRP_SQL_ARRAY_STATUS *)scheme_malloc_eternal(sizeof(SRP_SQL_ARRAY_STATUS));
    pArrayStatus->type = sql_array_status_type;
    pArrayStatus->hdesc = descHandle;
    pArrayStatus->descType = SQL_HDESC_DESCTYPE(argv[0]);
    pArrayStatus->values = pSmallUintVal;

    return (Scheme_Object *)pArrayStatus;

  case bindingoffset :
    // SQLINTEGER *pIntVal;
    SRP_SQL_BINDING_OFFSET *pBindingOffset;

    sr = SQLGetDescField(descHandle,recNumber,fieldId,
			 &pIntVal,sizeof(SQLINTEGER *),&actualLen);

    checkSQLReturn(sr,"sql-get-desc-field");

    pBindingOffset = (SRP_SQL_BINDING_OFFSET *)scheme_malloc_eternal(sizeof(SRP_SQL_BINDING_OFFSET));
    pBindingOffset->type = sql_binding_offset_type;
    pBindingOffset->val = pIntVal;

    return (Scheme_Object *)pBindingOffset;

  case rowsprocessed :
    SQLUINTEGER *pUintVal;
    SRP_SQL_ROWS_PROCESSED *pRowsProcessed;

    sr = SQLGetDescField(descHandle,recNumber,fieldId,
			 &pUintVal,sizeof(SQLUINTEGER *),&actualLen);

    checkSQLReturn(sr,"sql-get-desc-field");

    pRowsProcessed = (SRP_SQL_ROWS_PROCESSED *)scheme_malloc_eternal(sizeof(SRP_SQL_ROWS_PROCESSED));
    pRowsProcessed->type = sql_rows_processed_type;
    pRowsProcessed->val = pUintVal;

    return (Scheme_Object *)pRowsProcessed;

  case octetlength :
    // SQLINTEGER *pIntVal;
    SRP_SQL_OCTET_LENGTH *pOctetLength;

    sr = SQLGetDescField(descHandle,recNumber,fieldId,
			 &pIntVal,sizeof(SQLINTEGER *),&actualLen);

    checkSQLReturn(sr,"sql-get-desc-field");

    pOctetLength = (SRP_SQL_OCTET_LENGTH *)scheme_malloc_eternal(sizeof(SRP_SQL_OCTET_LENGTH));
    pOctetLength->type = sql_octet_length_type;
    pOctetLength->val = pIntVal;

    return (Scheme_Object *)pOctetLength;

  default :
    scheme_signal_error("sql-get-desc-field: unknown field type %X",fieldType);
  }

  return scheme_void;
}

Scheme_Object *srp_SQLGetDescRec(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHDESC descHandle;
  SQLSMALLINT recNumber;
  SQLCHAR name[1024];
  SQLSMALLINT actualLen;
  SQLSMALLINT type;
  char *typeString;
  SQLSMALLINT subtype;
  char *subtypeString;
  SQLINTEGER length;
  SQLSMALLINT precision;
  SQLSMALLINT scale;
  SQLSMALLINT nullable; 
  char *nullableString;
  Scheme_Object *retval;
  int i;

  if (SQL_HDESCP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-get-desc-rec","sql-hdesc",0,argc,argv);
  }

  if (isSmallInt(argv[1]) == FALSE) {
    scheme_wrong_type("sql-get-desc-rec","small-int",1,argc,argv);
  }
  
  recNumber = (SQLSMALLINT)SCHEME_INT_VAL(argv[1]);

  descHandle = SQL_HDESC_VAL(argv[0]);

  sr = SQLGetDescRec(descHandle,recNumber,name,sizeray(name),&actualLen,
		     &type,&subtype,&length,&precision,
		     &scale,&nullable);

  checkSQLReturn(sr,"sql-get-desc-rec");

  typeString = "?";

  for (i = 0; i < sizeray(SQLDataTypes); i++) {
    if (SQLDataTypes[i].val == type) {
      typeString = SQLDataTypes[i].scheme_name;
      break;
    }
  }

  subtypeString = "?";

  for (i = 0; i < sizeray(datetimeIntervalCodes); i++) {
    if ((SQLINTEGER)(datetimeIntervalCodes[i].val) == subtype) {
      subtypeString = datetimeIntervalCodes[i].scheme_name;
      break;
    }
  }

  nullableString = nullableToString(nullable);

  retval = scheme_null;
  retval = scheme_make_pair(scheme_intern_symbol(nullableString),retval);
  retval = scheme_make_pair(scheme_make_integer_value(scale),retval);
  retval = scheme_make_pair(scheme_make_integer_value(precision),retval);
  retval = scheme_make_pair(scheme_make_integer_value(length),retval);
  retval = scheme_make_pair(scheme_intern_symbol(subtypeString),retval);
  retval = scheme_make_pair(scheme_intern_symbol(typeString),retval);
  retval = scheme_make_pair(scheme_make_sized_string((char *)name,
						     actualLen,TRUE),retval);
  
  return retval;
}

Scheme_Object *srp_SQLGetDiagField(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLSMALLINT handleType;
  SQLHANDLE handle;
  SQLSMALLINT recNumber;
  SQLSMALLINT diagId;
  char *diagIdString;
  SQLSMALLINT actualLen;
  SRP_NAMED_TYPED_CONSTANT *p;

  if (SQL_HENVP(argv[0]) == FALSE && 
      SQL_HDBCP(argv[0]) == FALSE &&
      SQL_HSTMTP(argv[0]) == FALSE &&
      SQL_HDESCP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-get-diag-field",
		      "sql-henv> or <sql-hdbc> or <sql-hstmt> or <sql-hdesc",
		      0,argc,argv);
  }

  if (isSmallInt(argv[1]) == FALSE) {
    scheme_wrong_type("sql-get-diag-field",
		      "small-int",1,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[2]) == FALSE) {
    scheme_wrong_type("sql-get-diag-field","symbol",2,argc,argv);
  }

  diagIdString = SCHEME_SYM_VAL(argv[2]);

  p = namedTypedConstSearch(diagIdString,diagFields);

  if (p == NULL) {
    scheme_signal_error("Invalid diagnostic: %s",diagIdString);
  }
    
  diagId = (SQLUSMALLINT)(p->val);

  SchemeObjectToHandle(argv[0],&handle,&handleType);

  recNumber = (SQLSMALLINT)SCHEME_INT_VAL(argv[1]);

  switch(p->type) {

  case sqlinteger :

    SQLINTEGER intVal;

    sr = SQLGetDiagField(handleType,handle,recNumber,diagId,
			 &intVal,0,&actualLen);
    
    checkSQLReturn(sr,"sql-get-diag-field");		       

    return scheme_make_integer_value(intVal);

  case possiblynamedinteger :

    // SQLINTEGER intVal;
    char *diagString;

    sr = SQLGetDiagField(handleType,handle,recNumber,diagId,
			 &intVal,0,&actualLen);
    
    checkSQLReturn(sr,"sql-get-diag-field");		       

    diagString = findIntegerName(diagIdString,intVal,
			       namedDiagFieldIntegers,
			       sizeray(namedDiagFieldIntegers));

    if (strcmp(diagString,NO_BIT_NAME) == 0) {
      return scheme_make_integer_value(intVal);
    }

    return scheme_intern_symbol(diagString);

  case string :

    char buffer[2048];

    sr = SQLGetDiagField(handleType,handle,recNumber,diagId,
			 buffer,sizeof(buffer),&actualLen);
    
    checkSQLReturn(sr,"sql-get-diag-field");		       

    return scheme_make_sized_string(buffer,actualLen,TRUE);

  case sqlreturn :

    SQLRETURN returnVal;

    sr = SQLGetDiagField(handleType,handle,recNumber,diagId,
			 &returnVal,sizeof(returnVal),&actualLen);

    checkSQLReturn(sr,"sql-get-diag-field");		       

    return scheme_intern_symbol(sqlReturnToString(returnVal));

  default :

    scheme_signal_error("Unknown diagnostic type: %X",(int)p->type);
  }

  return scheme_void;

}

Scheme_Object *srp_SQLGetDiagRec(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLSMALLINT handleType;
  SQLHANDLE handle;
  SQLSMALLINT recNumber;
  SQLCHAR sqlState[6];
  SQLINTEGER nativeError;
  SQLCHAR messageText[1024];
  SQLSMALLINT actualLen;
  Scheme_Object *retval;

  if (SQL_HENVP(argv[0]) == FALSE && 
      SQL_HDBCP(argv[0]) == FALSE &&
      SQL_HSTMTP(argv[0]) == FALSE &&
      SQL_HDESCP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-get-diag-rec",
		      "sql-henv> or <sql-hdbc> or <sql-hstmt> or <sql-hdesc",
		      0,argc,argv);
  }

  if (isSmallInt(argv[1]) == FALSE) {
    scheme_wrong_type("sql-get-diag-rec",
		      "small-int",1,argc,argv);
  }

  SchemeObjectToHandle(argv[0],&handle,&handleType);

  recNumber = (SQLSMALLINT)SCHEME_INT_VAL(argv[1]);

  sqlState[5] = '\0';

  sr = SQLGetDiagRec(handleType,handle,recNumber,
		     sqlState,&nativeError,
		     messageText,sizeray(messageText),
		     &actualLen);

  checkSQLReturn(sr,"sql-get-diag-rec");		       

  retval = scheme_null;
  retval = scheme_make_pair(scheme_make_sized_string((char *)messageText,
						     actualLen,TRUE),retval);
  retval = scheme_make_pair(scheme_make_integer(nativeError),retval);
  retval = scheme_make_pair(scheme_make_string((const char *)sqlState),retval);

  return retval;
}

Scheme_Object *srp_SQLGetEnvAttr(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHENV envHandle;
  SQLINTEGER attribute;
  SRP_CONST_TYPE attributeType;
  char *attributeString;
  SRP_NAMED_TYPED_CONSTANT *p;
  SQLINTEGER actualLen;

  if (SQL_HENVP(argv[0]) == FALSE) { 
    scheme_wrong_type("sql-get-env-attr","sql-henv",0,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[1]) == FALSE) { 
    scheme_wrong_type("sql-get-env-attr","symbol",1,argc,argv);
  }

  attributeString = SCHEME_SYM_VAL(argv[1]);

  p = namedTypedConstSearch(attributeString,envAttributes);

  if (p == NULL) {
    scheme_signal_error("sql-get-env-attr: invalid environment attribute: %s",
			attributeString);
  }

  attribute = p->val;
  attributeType = p->type;

  envHandle = SQL_HENV_VAL(argv[0]);

  // interpret attribute according to type

  switch(attributeType) {

  case nameduinteger :

    SQLUINTEGER number;

    sr = SQLGetEnvAttr(envHandle,attribute,&number,0,&actualLen);
    checkSQLReturn(sr,"sql-get-env-attr");		       

    return scheme_intern_symbol(findIntegerName(attributeString,number,
						namedEnvAttrIntegers,
						sizeray(namedEnvAttrIntegers)));

  case sqlbool :

    SQLUINTEGER boolval;

    sr = SQLGetEnvAttr(envHandle,attribute,&boolval,0,&actualLen);
    checkSQLReturn(sr,"sql-get-env-attr");		       

    return (boolval == SQL_FALSE) ? scheme_false : scheme_true;
  }

  scheme_signal_error("Unknown environment attribute type: %X",
		      (int)attributeType);

  return scheme_void; // unreachable
}

Scheme_Object *srp_SQLGetFunctions(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHDBC connectionHandle;
  SQLUSMALLINT function;
  char *functionString;
  SQLUSMALLINT supported[SQL_API_ODBC3_ALL_FUNCTIONS_SIZE];
  SRP_NAMED_SMALL_CONSTANT *p;  
  int i;

  if (SQL_HDBCP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-get-functions","sql-hdbc",0,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-get-functions","symbol",1,argc,argv);
  }

  functionString = SCHEME_SYM_VAL(argv[1]);

  p = namedSmallConstSearch(functionString,sqlFunctions);
  
  if (p == NULL) {
    scheme_signal_error("sql-get-functions: invalid function name: %s",
			functionString);
  }

  connectionHandle = SQL_HDBC_VAL(argv[0]);

  function = p->val;
  
  sr = SQLGetFunctions(connectionHandle,function,supported);

  checkSQLReturn(sr,"sql-get-functions");		       

  if (function == SQL_API_ALL_FUNCTIONS) {
    Scheme_Object *value;
    Scheme_Object *retval;
    int ndx;

    retval = scheme_null;

    for (i = 0; i < sizeray(sqlFunctions); i++) {
      ndx = sqlFunctions[i].val;
      if (ndx < 100) { // valid ODBC 2 function
	if (supported[ndx] == SQL_TRUE) {
	  value = scheme_true;
	}
	else {
	  value = scheme_false;
	}
	retval = scheme_make_pair(scheme_make_pair(scheme_intern_symbol(sqlFunctions[i].scheme_name),
						   value),
				  retval);
      }
    }

    return retval;
  }
  else if (function == SQL_API_ODBC3_ALL_FUNCTIONS) {
    Scheme_Object *value;
    Scheme_Object *retval;

    retval = scheme_null;

    for (i = 0; i < sizeray(sqlFunctions); i++) {
      if (SQL_FUNC_EXISTS(supported,sqlFunctions[i].val) == SQL_TRUE) {
	value = scheme_true;
      }
      else {
	value = scheme_false;
      }
      retval = scheme_make_pair(scheme_make_pair(scheme_intern_symbol(sqlFunctions[i].scheme_name),
						 value),
				retval);
    }

    return retval;

  }
  else if (*supported == SQL_TRUE) {
    return scheme_true;
  }
  
  return scheme_false;
}

Scheme_Object *srp_SQLGetInfo(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHDBC connectionHandle;
  SQLUSMALLINT infoName;
  char *infoNameString;
  SRP_CONST_TYPE infoType;
  char buffer[2048];
  SQLSMALLINT actualLen;
  SRP_NAMED_TYPED_CONSTANT *p;    
  SQLUSMALLINT usmallint_value;
  SQLUINTEGER uinteger_value;
  SQLHDBC retConnectHandle;
  SQLHENV retEnvHandle;
  SQLHDESC retDescHandle;
  SQLHSTMT retStmtHandle;

  if (SQL_HDBCP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-get-info","sql-hdbc",0,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-get-info","symbol",1,argc,argv);
  }

  infoNameString = SCHEME_SYM_VAL(argv[1]);

  p = namedTypedConstSearch(infoNameString,sqlInfo);

  if (p == NULL) {
    scheme_signal_error("sql-get-info: invalid info type: %s",
			infoNameString);
  }

  infoName = (SQLUSMALLINT)p->val;
  infoType = p->type;

  connectionHandle = SQL_HDBC_VAL(argv[0]);

  if (argc == 3) {
    if (strcmpi(infoNameString,"sql-driver-hdesc") == 0) {
      if (SQL_HDESCP(argv[2]) == FALSE) {
	scheme_wrong_type("sql-get-info","sql-hdesc",2,argc,argv);
      }
      
      retDescHandle = SQL_HDBC_VAL(argv[2]);

    }
    else if (strcmpi(infoNameString,"sql-driver-hstmt") == 0) {
      if (SQL_HSTMTP(argv[2]) == FALSE) {
	scheme_wrong_type("sql-get-info","sql-hstmt",2,argc,argv);
      }
      
      retDescHandle = SQL_HSTMT_VAL(argv[2]);

    }
    else {
      scheme_signal_error("sql-get-info: too many arguments "
			  "for information type %s",infoNameString);
    }
  }

  switch(infoType) {

  case sqlusmallint :

    sr = SQLGetInfo(connectionHandle,infoName,&usmallint_value,0,&actualLen);
    checkSQLReturn(sr,"sql-get-info");    

    return scheme_make_integer((long)usmallint_value);

  case sqluinteger :

    sr = SQLGetInfo(connectionHandle,infoName,&uinteger_value,0,&actualLen);
    checkSQLReturn(sr,"sql-get-info");    

    return scheme_make_integer_value_from_unsigned(uinteger_value);

  case namedusmallint :

    sr = SQLGetInfo(connectionHandle,infoName,&usmallint_value,0,&actualLen);
    checkSQLReturn(sr,"sql-get-info");    

    return scheme_intern_symbol(findSmallIntName(infoNameString,usmallint_value,
						 namedInfoSmallInts,
						 sizeray(namedInfoSmallInts)));

  case nameduinteger :

    sr = SQLGetInfo(connectionHandle,infoName,&uinteger_value,0,&actualLen);
    checkSQLReturn(sr,"sql-get-info");    

    return scheme_intern_symbol(findIntegerName(infoNameString,uinteger_value,
						namedInfoIntegers,
						sizeray(namedInfoIntegers)));

  case boolstring :

    sr = SQLGetInfo(connectionHandle,infoName,buffer,sizeray(buffer),&actualLen);
    checkSQLReturn(sr,"sql-get-info");    

    if (buffer[0] == 'Y' || buffer[0] == 'y') {
      return scheme_true;
    }
    else if (buffer[0] == 'N' || buffer[0] == 'n') {
      return scheme_false;
    }
    else {
      scheme_signal_error("sql-get-info: expected 'Y' or 'N', got %s",buffer);
    }

  case string :

    sr = SQLGetInfo(connectionHandle,infoName,buffer,sizeray(buffer),&actualLen);
    checkSQLReturn(sr,"sql-get-info");    

    return scheme_make_sized_string(buffer,actualLen,TRUE);

  case bitmask :

    sr = SQLGetInfo(connectionHandle,infoName,&uinteger_value,0,&actualLen);
    checkSQLReturn(sr,"sql-get-info");    

    return bitsListFromBitMask(infoNameString,uinteger_value);

  case henv :

    {
    
      SRP_SQL_HENV *retval;

      sr = SQLGetInfo(connectionHandle,infoName,&retEnvHandle,0,&actualLen);

      checkSQLReturn(sr,"sql-get-info");    

      retval = (SRP_SQL_HENV *)scheme_malloc(sizeof(SRP_SQL_HENV));
      retval->type = sql_henv_type;
      retval->henv = retEnvHandle;

      return (Scheme_Object *)retval;

    }

  case hdbc : 

    { 

      SRP_SQL_HDBC *retval;

      sr = SQLGetInfo(connectionHandle,infoName,&retConnectHandle,0,&actualLen);

      checkSQLReturn(sr,"sql-get-info");    

      retval = (SRP_SQL_HDBC *)scheme_malloc(sizeof(SRP_SQL_HDBC));
      retval->type = sql_hdbc_type;
      retval->hdbc = retConnectHandle;

      return (Scheme_Object *)retval;

    }

  case hstmt :

    {

      SRP_SQL_HSTMT *retval;

      sr = SQLGetInfo(connectionHandle,infoName,&retStmtHandle,0,&actualLen);

      checkSQLReturn(sr,"sql-get-info");    

      retval = (SRP_SQL_HSTMT *)scheme_malloc(sizeof(SRP_SQL_HSTMT));
      retval->type = sql_hstmt_type;
      retval->hstmt = retStmtHandle;
      
      return (Scheme_Object *)retval;

    }

  case hdesc :
    
    {

      SRP_SQL_HDESC *retval;

      sr = SQLGetInfo(connectionHandle,infoName,&retDescHandle,0,&actualLen);

      checkSQLReturn(sr,"sql-get-info");    

      retval = (SRP_SQL_HDESC *)scheme_malloc(sizeof(SRP_SQL_HDESC));
      retval->type = sql_hdesc_type;
      retval->hdesc = retDescHandle;

      return (Scheme_Object *)retval;

    }

  }

  return scheme_void;  // unreachable
}

Scheme_Object *srp_SQLGetStmtAttr(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLINTEGER actualLen;
  char *attributeString;
  SQLINTEGER attribute;
  SRP_CONST_TYPE attributeType;
  SQLUINTEGER number;
  SQLUINTEGER *numpointer;
  SQLUSMALLINT *smallnumpointer;
  SRP_NAMED_TYPED_CONSTANT *p;    

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-get-stmt-attr","sql-hstmt",0,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-get-stmt-attr","symbol",1,argc,argv);
  }

  attributeString = SCHEME_SYM_VAL(argv[1]);

  p = namedTypedConstSearch(attributeString,stmtAttributes);

  if (p == NULL) {
    scheme_signal_error("sql-get-stmt-attr: invalid attribute: %s",
			attributeString);
  }

  attribute = p->val;
  attributeType = p->type;

  stmtHandle = SQL_HSTMT_VAL(argv[0]);

  // interpret attribute according to type

  switch(attributeType) {

  case sqluinteger :

    sr = SQLGetStmtAttr(stmtHandle,attribute,&number,0,&actualLen);
    checkSQLReturn(sr,"sql-get-stmt-attr");

    return scheme_make_integer_value_from_unsigned(number);

  case sqlbool :

    sr = SQLGetStmtAttr(stmtHandle,attribute,&number,0,&actualLen);
    checkSQLReturn(sr,"sql-get-stmt-attr");
    return (number == SQL_FALSE) ? scheme_false : scheme_true;

  case nameduinteger :

    sr = SQLGetStmtAttr(stmtHandle,attribute,&number,0,&actualLen);
    checkSQLReturn(sr,"sql-get-stmt-attr");

    return scheme_intern_symbol(findIntegerName(attributeString,number,
						namedStmtAttributes,
						sizeray(namedStmtAttributes)));
    
  case possiblynameduinteger :

    char *attrName;

    sr = SQLGetStmtAttr(stmtHandle,attribute,&number,0,&actualLen);
    checkSQLReturn(sr,"sql-get-stmt-attr");

    attrName = findIntegerName(attributeString,number,
			       namedStmtAttributes,
			       sizeray(namedStmtAttributes));

    if (strcmp(attrName,NO_BIT_NAME) == 0) {
      return scheme_make_integer_value_from_unsigned(number);
    }

    return scheme_intern_symbol(attrName);

  case rowstatus :
    
    SRP_SQL_ROW_STATUS *rowStatus;

    sr = SQLGetStmtAttr(stmtHandle,attribute,&smallnumpointer,0,&actualLen);
    checkSQLReturn(sr,"sql-get-stmt-attr");

    // need to keep rowStatus around until cursor closed
    // conservatively, make it uncollectable

    rowStatus = (SRP_SQL_ROW_STATUS *)scheme_malloc_eternal(sizeof(SRP_SQL_ROW_STATUS));
    rowStatus->type = sql_row_status_type;
    rowStatus->numRows = 0;
    rowStatus->values = smallnumpointer;

    return (Scheme_Object *)rowStatus;

  case sqlboxeduint :

    { SRP_SQL_BOXED_UINT *retval;

      sr = SQLGetStmtAttr(stmtHandle,attribute,&numpointer,0,&actualLen);
      checkSQLReturn(sr,"sql-get-stmt-attr");

      retval = (SRP_SQL_BOXED_UINT *)scheme_malloc(sizeof(SRP_SQL_BOXED_UINT));
      retval->type = sql_boxed_uint_type;
      retval->pointer = numpointer;

      return (Scheme_Object *)retval;
    }

  case hdesc :

    { SRP_SQL_HDESC *retval;

      sr = SQLGetStmtAttr(stmtHandle,attribute,&number,0,&actualLen);
      checkSQLReturn(sr,"sql-get-stmt-attr");

      retval = (SRP_SQL_HDESC *)scheme_malloc(sizeof(SRP_SQL_HDESC));
      retval->type = sql_hdesc_type;

      switch(attribute) {

      case SQL_ATTR_APP_PARAM_DESC :
	retval->descType = APD;
	break;
      case SQL_ATTR_APP_ROW_DESC :
	retval->descType = ARD;
	break;
      case SQL_ATTR_IMP_PARAM_DESC :
	retval->descType = IPD;
	break;
      case SQL_ATTR_IMP_ROW_DESC :
	retval->descType = IRD;
	break;
      default :
	scheme_signal_error("sql-get-stmt-attribute: unknown hdesc type");
      }
      
      retval->hdesc = (SQLHDESC)number;

      return (Scheme_Object *)retval;
    }

  case opparms :  

    { SRP_SQL_OP_PARMS *retval;
      SQLUINTEGER paramSetSize;

      sr = SQLGetStmtAttr(stmtHandle,SQL_ATTR_PARAMSET_SIZE,&paramSetSize,0,&actualLen);
      checkSQLReturn(sr,"sql-get-stmt-attr");
      
      sr = SQLGetStmtAttr(stmtHandle,attribute,&smallnumpointer,0,&actualLen);
      checkSQLReturn(sr,"sql-get-stmt-attr");
    
      retval = (SRP_SQL_OP_PARMS *)scheme_malloc(sizeof(SRP_SQL_OP_PARMS));
      retval->type = sql_op_parms_type;
      retval->paramSetSize = paramSetSize;
      retval->values = smallnumpointer;

      return (Scheme_Object *)retval;
    }
  }

  scheme_signal_error("sql-get-stmt-attr: invalid attribute type: %X",
		      attributeType);

  return scheme_void; // unreachable
}

Scheme_Object *srp_SQLGetStmtOption(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  char *optionString;
  SQLUSMALLINT option;
  SRP_CONST_TYPE optionType;
  SQLUINTEGER number;
  SQLUINTEGER *numpointer;
  SQLUSMALLINT *smallnumpointer;
  SRP_NAMED_TYPED_CONSTANT *p;    

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-get-stmt-option","sql-hstmt",0,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-get-stmt-option","symbol",1,argc,argv);
  }

  optionString = SCHEME_SYM_VAL(argv[1]);

  p = namedTypedConstSearch(optionString,stmtAttributes);

  if (p == NULL) {
    scheme_signal_error("sql-get-stmt-option: invalid option: %s",
			optionString);
  }

  option = (SQLUSMALLINT)(p->val);
  optionType = p->type;

  stmtHandle = SQL_HSTMT_VAL(argv[0]);


  // interpret option according to type

  switch(optionType) {

  case sqluinteger :

    sr = SQLGetStmtOption(stmtHandle,option,&number);
    checkSQLReturn(sr,"sql-get-stmt-option");

    return scheme_make_integer_value_from_unsigned(number);

  case sqlbool :

    sr = SQLGetStmtOption(stmtHandle,option,&number);
    checkSQLReturn(sr,"sql-get-stmt-option");
    return (number == SQL_FALSE) ? scheme_false : scheme_true;

  case nameduinteger :

    sr = SQLGetStmtOption(stmtHandle,option,&number);
    checkSQLReturn(sr,"sql-get-stmt-option");

    return scheme_intern_symbol(findIntegerName(optionString,number,
						namedStmtAttributes,
						sizeray(namedStmtAttributes)));
    
  case possiblynameduinteger :

    char *attrName;

    sr = SQLGetStmtOption(stmtHandle,option,&number);
    checkSQLReturn(sr,"sql-get-stmt-option");

    attrName = findIntegerName(optionString,number,
			       namedStmtAttributes,
			       sizeray(namedStmtAttributes));

    if (strcmp(attrName,NO_BIT_NAME) == 0) {
      return scheme_make_integer_value_from_unsigned(number);
    }

    return scheme_intern_symbol(attrName);

  case rowstatus :
    
    SRP_SQL_ROW_STATUS *rowStatus;

    sr = SQLGetStmtOption(stmtHandle,option,&smallnumpointer);
    checkSQLReturn(sr,"sql-get-stmt-option");

    // need to keep rowStatus around until cursor closed
    // conservatively, make it uncollectable

    rowStatus = (SRP_SQL_ROW_STATUS *)scheme_malloc_eternal(sizeof(SRP_SQL_ROW_STATUS));
    rowStatus->type = sql_row_status_type;
    rowStatus->numRows = 0;
    rowStatus->values = smallnumpointer;

    return (Scheme_Object *)rowStatus;

  case sqlboxeduint :

    { SRP_SQL_BOXED_UINT *retval;

      sr = SQLGetStmtOption(stmtHandle,option,&numpointer);
      checkSQLReturn(sr,"sql-get-stmt-option");

      retval = (SRP_SQL_BOXED_UINT *)scheme_malloc(sizeof(SRP_SQL_BOXED_UINT));
      retval->type = sql_boxed_uint_type;
      retval->pointer = numpointer;

      return (Scheme_Object *)retval;
    }

  case hdesc :

    { SRP_SQL_HDESC *retval;

      sr = SQLGetStmtOption(stmtHandle,option,&number);
      checkSQLReturn(sr,"sql-get-stmt-option");

      retval = (SRP_SQL_HDESC *)scheme_malloc(sizeof(SRP_SQL_HDESC));
      retval->type = sql_hdesc_type;

      switch(option) {

      case SQL_ATTR_APP_PARAM_DESC :
	retval->descType = APD;
	break;
      case SQL_ATTR_APP_ROW_DESC :
	retval->descType = ARD;
	break;
      case SQL_ATTR_IMP_PARAM_DESC :
	retval->descType = IPD;
	break;
      case SQL_ATTR_IMP_ROW_DESC :
	retval->descType = IRD;
	break;
      default :
	scheme_signal_error("sql-get-stmt-option: unknown hdesc type");
      }
      
      retval->hdesc = (SQLHDESC)number;

      return (Scheme_Object *)retval;
    }

  case opparms :  

    { SRP_SQL_OP_PARMS *retval;
      SQLUINTEGER paramSetSize;

      sr = SQLGetStmtOption(stmtHandle,SQL_ATTR_PARAMSET_SIZE,&paramSetSize);
      checkSQLReturn(sr,"sql-get-stmt-option");
      
      sr = SQLGetStmtOption(stmtHandle,option,&smallnumpointer);
      checkSQLReturn(sr,"sql-get-stmt-option");
    
      retval = (SRP_SQL_OP_PARMS *)scheme_malloc(sizeof(SRP_SQL_OP_PARMS));
      retval->type = sql_op_parms_type;
      retval->paramSetSize = paramSetSize;
      retval->values = smallnumpointer;

      return (Scheme_Object *)retval;
    }
  }

  scheme_signal_error("sql-get-stmt-attr: invalid option type: %X",
		      optionType);

  return scheme_void; // unreachable

}

Scheme_Object *srp_SQLGetTypeInfo(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLSMALLINT dataType;
  char *dataTypeString;
  SRP_NAMED_SMALL_CONSTANT *p;    

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-get-type-info","sql-hstmt",0,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-get-type-info","symbol",1,argc,argv);
  }

  dataTypeString = SCHEME_SYM_VAL(argv[1]);

  p = namedSmallConstSearch(dataTypeString,SQLDataTypes);

  if (p == NULL) {
    scheme_signal_error("sql-get-type-info: invalid data type: %s",
			dataTypeString);
  }

  dataType = p->val;

  stmtHandle = SQL_HSTMT_VAL(argv[0]);

  sr = SQLGetTypeInfo(stmtHandle,dataType);

  checkSQLReturn(sr,"sql-get-type-info");

  return scheme_void;
}

Scheme_Object *srp_SQLNumResultCols(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLSMALLINT colCount;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-num-result-cols","sql-hstmt",0,argc,argv);
  }
  
  stmtHandle = SQL_HSTMT_VAL(argv[0]);

  sr = SQLNumResultCols(stmtHandle,&colCount);

  checkSQLReturn(sr,"sql-num-result-cols");

  return scheme_make_integer(colCount);
}

Scheme_Object *srp_SQLParamData(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLPOINTER buffer;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-param-data","sql-hstmt",0,argc,argv);
  }
  
  stmtHandle = SQL_HSTMT_VAL(argv[0]);

  sr = SQLParamData(stmtHandle,&buffer);

  checkSQLReturn(sr,"sql-param-data");

  return (Scheme_Object *)(lookupBufferFromAddress(buffer));
}

Scheme_Object *srp_SQLPrepare(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLCHAR *text;
  SQLINTEGER textLen;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-prepare","sql-hstmt",0,argc,argv);
  }
  
  if (SCHEME_STRINGP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-prepare","string",1,argc,argv);
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);
  text = (SQLCHAR *)SCHEME_STR_VAL(argv[1]);
  textLen = SCHEME_STRLEN_VAL(argv[1]);

  sr = SQLPrepare(stmtHandle,text,textLen);

  checkSQLReturn(sr,"sql-prepare");

  return scheme_void;
}

Scheme_Object *srp_SQLPutData(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLPOINTER buffer;
  SQLINTEGER bufferLen;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-put-data","sql-hstmt",0,argc,argv);
  }
  
  if (SQL_BUFFERP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-put-data","sql-buffer",1,argc,argv);
  }
  
  stmtHandle = SQL_HSTMT_VAL(argv[0]);
  buffer = SQL_BUFFER_VAL(argv[1]);
  bufferLen = SQL_BUFFER_LEN(argv[1]);
  
  sr = SQLPutData(stmtHandle,buffer,bufferLen);

  checkSQLReturn(sr,"sql-put-data");

  return scheme_void;
}

Scheme_Object *srp_SQLRowCount(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLINTEGER rowCount;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-row-count","sql-hstmt",0,argc,argv);
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);
  
  sr = SQLRowCount(stmtHandle,&rowCount);

  checkSQLReturn(sr,"sql-row-count");

  if (rowCount >= 0) {
    return scheme_make_integer_value((long)rowCount);
  }

  return scheme_intern_symbol("sql-row-count-unavailable");
}

Scheme_Object *srp_SQLSetConnectAttr(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHDBC connectionHandle;
  SQLINTEGER attribute;
  char *attributeString;
  SRP_CONST_TYPE attributeType;
  SQLUINTEGER val;
  char *attributeValString;
  SRP_NAMED_TYPED_CONSTANT *p;    
  
  if (SQL_HDBCP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-set-connect-attr","sql-hdbc",0,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[1]) == FALSE && SCHEME_INTP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-set-connect-attr","sym or int",1,argc,argv);
  }

  attributeString = SCHEME_SYM_VAL(argv[1]);

  p = namedTypedConstSearch(attributeString,settableConnectionAttributes);

  if (p == NULL) {
    scheme_signal_error("sql-set-connect-attr: invalid connection attribute: %s",
			attributeString);
  }

  attribute = p->val;
  attributeType = p->type;

  connectionHandle = SQL_HDBC_VAL(argv[0]);

  switch(attributeType) {

  case sqlbool :

    SQLUINTEGER boolVal;

    // treat non-#f as true

    boolVal = (argv[2] == scheme_false) ? SQL_FALSE : SQL_TRUE;

    sr = SQLSetConnectAttr(connectionHandle,attribute,(SQLPOINTER)boolVal,0);
    checkSQLReturn(sr,"sql-set-connect-attr");

    return scheme_void;

  case sqluinteger :

    SQLUINTEGER number;

    if (SCHEME_EXACT_INTEGERP(argv[2]) == FALSE) { 
      scheme_wrong_type("sql-set-connect-attr","int",2,argc,argv);
    }

    if (scheme_get_unsigned_int_val(argv[2],&number) == 0) {
      scheme_signal_error("sql-set-connect-attr: numeric value too big");
    }

    sr = SQLSetConnectAttr(connectionHandle,attribute,(SQLPOINTER)number,0);
    checkSQLReturn(sr,"sql-set-connect-attr");

    return scheme_void;

  case nameduinteger :

    if (SCHEME_SYMBOLP(argv[2]) == FALSE) { 
      scheme_wrong_type("sql-set-connect-attr","symbol",2,argc,argv);
    }

    attributeValString = SCHEME_SYM_VAL(argv[2]);

    val = findNamedInteger(attributeString,attributeValString,
			   namedConnectAttrIntegers,
			   sizeray(namedConnectAttrIntegers));

    if (val == -1) {
      scheme_signal_error("sql-set-connect-attr: unknown attribute value: %s",
			  attributeValString);
    }

    sr = SQLSetConnectAttr(connectionHandle,attribute,(SQLPOINTER)val,0);
    checkSQLReturn(sr,"sql-set-connect-attr");

    return scheme_void;

  case string :

    SQLCHAR *s;
    SQLINTEGER len;

    if (SCHEME_STRINGP(argv[2]) == FALSE) { 
      scheme_wrong_type("sql-set-connect-attr","string",2,argc,argv);
    }

    s = (SQLCHAR *)SCHEME_STR_VAL(argv[2]);
    len = SCHEME_STRLEN_VAL(argv[2]);

    sr = SQLSetConnectAttr(connectionHandle,attribute,s,len);
    checkSQLReturn(sr,"sql-set-connect-attr");

    return scheme_void;
  }

  scheme_signal_error("sql-set-connect-attr: unknown attribute type: %X",
		      attributeType);

  return scheme_void; // unreachable
}

Scheme_Object *srp_SQLSetConnectOption(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHDBC connectionHandle;
  SQLINTEGER option;
  char *optionString;
  SRP_CONST_TYPE optionType;
  SQLUINTEGER val;
  char *optionValString;
  SRP_NAMED_TYPED_CONSTANT *p;    
  
  if (SQL_HDBCP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-set-connect-option","sql-hdbc",0,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-set-connect-option","symbol",1,argc,argv);
  }

  optionString = SCHEME_SYM_VAL(argv[1]);

  p = namedTypedConstSearch(optionString,settableConnectionAttributes);

  if (p == NULL) {
    scheme_signal_error("sql-set-connect-option: invalid connection option: %s",
			optionString);
  }

  option = p->val;
  optionType = p->type;

  connectionHandle = SQL_HDBC_VAL(argv[0]);

  switch(optionType) {

  case sqlbool :

    SQLUINTEGER boolVal;

    // treat non-#f as true

    boolVal = (argv[2] == scheme_false) ? SQL_FALSE : SQL_TRUE;

    sr = SQLSetConnectAttr(connectionHandle,option,(SQLPOINTER)boolVal,0);
    checkSQLReturn(sr,"sql-set-connect-option");

    return scheme_void;

  case sqluinteger :

    SQLUINTEGER number;

    if (SCHEME_EXACT_INTEGERP(argv[2]) == FALSE) { 
      scheme_wrong_type("sql-set-connect-option","int",2,argc,argv);
    }

    if (scheme_get_unsigned_int_val(argv[2],&number) == 0) {
      scheme_signal_error("sql-set-connect-option: numeric value too big");
    }

    sr = SQLSetConnectAttr(connectionHandle,option,(SQLPOINTER)number,0);
    checkSQLReturn(sr,"sql-set-connect-option");

    return scheme_void;

  case nameduinteger :

    if (SCHEME_SYMBOLP(argv[2]) == FALSE) { 
      scheme_wrong_type("sql-set-connect-option","symbol",2,argc,argv);
    }

    optionValString = SCHEME_SYM_VAL(argv[2]);

    val = findNamedInteger(optionString,optionValString,
			   namedConnectAttrIntegers,
			   sizeray(namedConnectAttrIntegers));

    if (val == -1) {
      scheme_signal_error("sql-set-connect-option: unknown option value: %s",
			  optionValString);
    }

    sr = SQLSetConnectAttr(connectionHandle,option,(SQLPOINTER)val,0);
    checkSQLReturn(sr,"sql-set-connect-option");

    return scheme_void;

  case string :

    SQLCHAR *s;
    SQLINTEGER len;

    if (SCHEME_STRINGP(argv[2]) == FALSE) { 
      scheme_wrong_type("sql-set-connect-option","string",2,argc,argv);
    }

    s = (SQLCHAR *)SCHEME_STR_VAL(argv[2]);
    len = SCHEME_STRLEN_VAL(argv[2]);

    sr = SQLSetConnectAttr(connectionHandle,option,s,len);
    checkSQLReturn(sr,"sql-set-connect-option");

    return scheme_void;
  }

  scheme_signal_error("sql-set-connect-option: unknown option type: %X",
		      optionType);

  return scheme_void; // unreachable
}

Scheme_Object *srp_SQLSetCursorName(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLCHAR *name;
  SQLSMALLINT nameLen;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-set-cursor-name","sql-hstmt",0,argc,argv);
  }

  if (SCHEME_STRINGP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-set-cursor-name","string",1,argc,argv);
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);
  name = (SQLCHAR *)SCHEME_STR_VAL(argv[1]);
  nameLen = SCHEME_STRLEN_VAL(argv[1]);

  sr = SQLSetCursorName(stmtHandle,name,nameLen);

  checkSQLReturn(sr,"sql-set-cursor-name");

  return scheme_void;
}

Scheme_Object *srp_SQLSetDescField(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHDESC descHandle;
  SQLSMALLINT recNumber;
  SQLSMALLINT fieldId;
  SQLINTEGER intVal;
  SQLUINTEGER uintVal;
  SRP_CONST_TYPE fieldType;
  char *fieldIdString;
  SRP_NAMED_TYPED_CONSTANT *p;    

  if (SQL_HDESCP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-set-desc-field","sql-hdesc",0,argc,argv);    
  }

  if (isSmallInt(argv[1]) == FALSE) {
    scheme_wrong_type("sql-set-desc-field","small-int",1,argc,argv);    
  }

  if (SCHEME_SYMBOLP(argv[2]) == FALSE) {
    scheme_wrong_type("sql-set-desc-field","symbol",2,argc,argv);    
  }

  fieldIdString = SCHEME_SYM_VAL(argv[2]);

  p = namedTypedConstSearch(fieldIdString,fieldDescriptors);

  if (p == NULL) {
      scheme_signal_error("sql-set-desc-field: invalid field id: %s",
			  fieldIdString);

  }

  fieldId = (SQLSMALLINT)(p->val);
  fieldType = p->type;

  descHandle = SQL_HDESC_VAL(argv[0]);
  recNumber = (SQLSMALLINT)SCHEME_INT_VAL(argv[1]);

  switch(fieldType) {

  case sqlsmallint :

    if (isSmallInt(argv[3]) == FALSE) {
      scheme_wrong_type("sql-set-desc-field","small-int",3,argc,argv);    
    }

    sr = SQLSetDescField(descHandle,recNumber,fieldId,
			 (SQLPOINTER)SCHEME_INT_VAL(argv[3]),0);

    break;

  case sqlinteger :

    if (SCHEME_EXACT_INTEGERP(argv[3]) == FALSE) {
      scheme_wrong_type("sql-set-desc-field","int",3,argc,argv);    
    }

    if (scheme_get_int_val(argv[3],&intVal) == 0) {
      scheme_signal_error("sql-set-desc-field: exact integer value too large");
    }

    sr = SQLSetDescField(descHandle,recNumber,fieldId,
			 (SQLPOINTER)intVal,0);

    break;
 
  case sqluinteger :

    if (SCHEME_EXACT_INTEGERP(argv[3]) == FALSE) {
      scheme_wrong_type("sql-set-desc-field","unsigned int",3,argc,argv);    
    }

    if (scheme_get_unsigned_int_val(argv[3],&uintVal) == 0) {
      scheme_signal_error("sql-set-desc-field: exact integer value too large");
    }

    sr = SQLSetDescField(descHandle,recNumber,fieldId,(SQLPOINTER)uintVal,0);
    
    break;

  case string :

    if (SCHEME_STRINGP(argv[3]) == FALSE) {
      scheme_wrong_type("sql-set-desc-field","string",3,argc,argv);    
    }

    sr = SQLSetDescField(descHandle,recNumber,fieldId,
			 (SQLPOINTER)SCHEME_STR_VAL(argv[3]),
			 SCHEME_STRLEN_VAL(argv[3]));
    
    break;

  case sqlbool :

    intVal = (argv[3] == scheme_false) ? SQL_FALSE : SQL_TRUE;

    sr = SQLSetDescField(descHandle,recNumber,fieldId,(SQLPOINTER)intVal,0);

    break;

  case namedinteger :

    if (SCHEME_SYMBOLP(argv[3]) == FALSE) {
      scheme_wrong_type("sql-set-desc-field","symbol",3,argc,argv);    
    }

    uintVal = findNamedInteger(fieldIdString,SCHEME_SYM_VAL(argv[3]),
			       namedFieldDescriptors,sizeray(namedFieldDescriptors));

    sr = SQLSetDescField(descHandle,recNumber,fieldId,
			 (SQLPOINTER)uintVal,0);

    break;

  case sqlbuffer :

    if (SQL_BUFFERP(argv[3]) == FALSE) {
      scheme_wrong_type("sql-set-desc-field","sql-buffer",3,argc,argv);    
    }

    sr = SQLSetDescField(descHandle,recNumber,fieldId,
			 (SQLPOINTER)SQL_BUFFER_VAL(argv[3]),sizeof(SQLPOINTER));

    break;

  case sqlindicator :

    if (SQL_INDICATORP(argv[3]) == FALSE) {
      scheme_wrong_type("sql-set-desc-field","sql-indicator",3,argc,argv);    
    }

    sr = SQLSetDescField(descHandle,recNumber,fieldId,
			 (SQLPOINTER)&SQL_INDICATOR_VAL(argv[3]),
			 sizeof(SQLPOINTER));

    break;

  case arraystatus :

    if (SQL_ARRAY_STATUSP(argv[3]) == FALSE) {
      scheme_wrong_type("sql-set-desc-field","sql-array-status",3,argc,argv);
    }

    sr = SQLSetDescField(descHandle,recNumber,fieldId,
			 (SQLPOINTER)SQL_ARRAY_STATUS_VAL(argv[3]),
			 sizeof(SQLPOINTER));


  case bindingoffset :

    if (SQL_BINDING_OFFSETP(argv[3]) == FALSE) {
      scheme_wrong_type("sql-set-desc-field","sql-binding-offset",3,argc,argv);
    }

    sr = SQLSetDescField(descHandle,recNumber,fieldId,
			 (SQLPOINTER)SQL_BINDING_OFFSET_VAL(argv[3]),
			 sizeof(SQLPOINTER));

    break;

  case rowsprocessed :

    if (SQL_ROWS_PROCESSEDP(argv[3]) == FALSE) {
      scheme_wrong_type("sql-set-desc-field","sql-rows-processed",3,argc,argv);
    }

    sr = SQLSetDescField(descHandle,recNumber,fieldId,
			 (SQLPOINTER)SQL_ROWS_PROCESSED_VAL(argv[3]),
			 sizeof(SQLPOINTER));

    break;

  case octetlength :

    if (SQL_OCTET_LENGTHP(argv[3]) == FALSE) {
      scheme_wrong_type("sql-set-desc-field","sql-octet-length",3,argc,argv);
    }

    sr = SQLSetDescField(descHandle,recNumber,fieldId,
			 (SQLPOINTER)SQL_OCTET_LENGTH_VAL(argv[3]),
			 sizeof(SQLPOINTER));

  default :
    scheme_signal_error("sql-set-desc-field: unknown field type %X",fieldType);
  }

  checkSQLReturn(sr,"sql-set-desc-field");

  return scheme_void;
}

Scheme_Object *srp_SQLSetDescRec(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHDESC descHandle;
  SQLSMALLINT recNumber;
  SQLSMALLINT type;
  char *typeString;
  SQLSMALLINT subType;
  char *subTypeString;
  SQLINTEGER octetLen;
  SQLSMALLINT precision;
  SQLSMALLINT scale;
  SQLPOINTER buffer;
  SRP_NAMED_SMALL_CONSTANT *p;    
  SQLINTEGER *length;
  SQLINTEGER indicator;

  if (SQL_HDESCP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-set-desc-rec","sql-hdesc",0,argc,argv);    
  }

  if (isSmallInt(argv[1]) == FALSE) {
    scheme_wrong_type("sql-set-desc-rec","small-int",1,argc,argv);    
  }
  
  if (SCHEME_SYMBOLP(argv[2]) == FALSE) {
    scheme_wrong_type("sql-set-desc-rec","symbol",1,argc,argv);    
  }

  if (isSmallInt(argv[3]) == FALSE) {
    scheme_wrong_type("sql-set-desc-rec","small-int",3,argc,argv);    
  }

  if (isSmallInt(argv[4]) == FALSE) {
    scheme_wrong_type("sql-set-desc-rec","small-int",4,argc,argv);    
  }

  if (SQL_BUFFERP(argv[5]) == FALSE) {
    scheme_wrong_type("sql-set-desc-rec","sql-buffer",5,argc,argv);    
  }

  if (SQL_LENGTHP(argv[6]) == FALSE) {
    scheme_wrong_type("sql-set-desc-rec","sql-length",6,argc,argv);    
  }

  if (SQL_INDICATORP(argv[7]) == FALSE) {
    scheme_wrong_type("sql-set-desc-rec","int",7,argc,argv);    
  }

  typeString = SCHEME_SYM_VAL(argv[2]);

  p = namedSmallConstSearch(typeString,SQLDataTypes);

  if (p == NULL) {
      scheme_signal_error("sql-set-desc-rec: invalid data type: %s",
			  typeString);
  }

  type = p->val;

  if (type == SQL_DATETIME || type == SQL_INTERVAL) {

    if (SCHEME_INTP(argv[8]) == FALSE) {
      scheme_wrong_type("sql-set-desc-rec","int",3,argc,argv);    
    }

    subTypeString = SCHEME_SYM_VAL(argv[8]);

    p = namedSmallConstSearch(subTypeString,datetimeIntervalCodes);
 
    if (p == NULL) {
      scheme_signal_error("sql-set-desc-rec: invalid date/time interval code: %s",
			  subTypeString);
      
    }

    subType = p->val;
  }
  else {
    subType = 0;
  }

  descHandle = SQL_HDESC_VAL(argv[0]);
  recNumber = (SQLSMALLINT)SCHEME_INT_VAL(argv[1]);
  precision = (SQLSMALLINT)SCHEME_INT_VAL(argv[3]);
  scale = (SQLSMALLINT)SCHEME_INT_VAL(argv[4]);
  octetLen = SQL_BUFFER_LEN(argv[5]);
  buffer = SQL_BUFFER_VAL(argv[5]);
  length = &SQL_LENGTH_VAL(argv[6]);
  indicator = SQL_INDICATOR_VAL(argv[7]);
  
  sr = SQLSetDescRec(descHandle,recNumber,type,subType,
		     octetLen,precision,scale,buffer,
		     length,&indicator);
  
  checkSQLReturn(sr,"sql-set-desc-rec");

  return scheme_void;
}

Scheme_Object *srp_SQLSetEnvAttr(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHENV envHandle;
  SQLINTEGER attribute;
  SRP_CONST_TYPE attributeType;
  char *attributeString;
  SRP_NAMED_TYPED_CONSTANT *p;

  if (SQL_HENVP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-set-env-attr","sql-henv",0,argc,argv);    
  }

  if (SCHEME_SYMBOLP(argv[1]) == FALSE) { 
    scheme_wrong_type("sql-set-env-attr","symbol",1,argc,argv);
  }

  attributeString = SCHEME_SYM_VAL(argv[1]);

  p = namedTypedConstSearch(attributeString,envAttributes);

  if (p == NULL) {
    scheme_signal_error("sql-get-env-attr: invalid environment attribute: %s",
			attributeString);
  }

  attribute = p->val;
  attributeType = p->type;

  envHandle = SQL_HENV_VAL(argv[0]);

  switch(attributeType) {

  case nameduinteger :

    SQLUINTEGER val;
    char *attributeValString;

    if (SCHEME_SYMBOLP(argv[2]) == FALSE) { 
      scheme_wrong_type("sql-set-env-attr","symbol",2,argc,argv);
    }

    attributeValString = SCHEME_SYM_VAL(argv[2]);

    val = findNamedInteger(attributeString,attributeValString,
			   namedEnvAttrIntegers,
			   sizeray(namedEnvAttrIntegers));

    if (val == -1) {
      scheme_signal_error("sql-set-env-attr: unknown attribute value: %s",
			  attributeValString);
    }

    sr = SQLSetEnvAttr(envHandle,attribute,(SQLPOINTER)val,0);
    checkSQLReturn(sr,"sql-set-env-attr");

    return scheme_void;

  case sqlbool :

    SQLUINTEGER boolVal;

    // treat non-#f as true

    boolVal = (argv[2] == scheme_false) ? SQL_FALSE : SQL_TRUE;

    sr = SQLSetEnvAttr(envHandle,attribute,(SQLPOINTER)boolVal,0);
    checkSQLReturn(sr,"sql-set-env-attr");
    
    return scheme_void;
  }

  scheme_signal_error("sql-set-env-attr: unknown attribute type: %X",
		      attributeType);

  return scheme_void; // unreachable
}

Scheme_Object *srp_SQLSetParam(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLUSMALLINT paramNumber;
  SQLSMALLINT CDataType;
  SQLSMALLINT paramType;
  char *paramTypeString;
  SQLUINTEGER precision;
  SQLSMALLINT scale;
  SQLPOINTER buffer;
  SQLINTEGER *indicator;
  SRP_NAMED_SMALL_CONSTANT *p;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-set-param","sql-hstmt",0,argc,argv);
  } 

  if (SCHEME_INTP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-set-param","int",1,argc,argv);
  } 

  if (SCHEME_SYMBOLP(argv[2]) == FALSE) {
    scheme_wrong_type("sql-set-param","symbol",2,argc,argv);
  } 

  if (SQL_BUFFERP(argv[3]) == FALSE) {
    scheme_wrong_type("sql-set-param","sql-buffer",3,argc,argv);
  }

  if (SQL_INDICATORP(argv[4]) == FALSE) {
    scheme_wrong_type("sql-set-param","sql-indicator",4,argc,argv);
  }

  paramTypeString = SCHEME_SYM_VAL(argv[2]);

  p = namedSmallConstSearch(paramTypeString,SQLDataTypes);

  if (p == NULL) {
      scheme_signal_error("sql-set-param: invalid parameter type: %s",
			  paramTypeString);
  }

  paramType = p->val;

  CDataType = SQL_BUFFER_CTYPE(argv[3]);

  switch(paramType) {

  case SQL_DECIMAL:
  case SQL_NUMERIC:
  case SQL_TIMESTAMP:

    if (argc != 7) {
      scheme_wrong_count("sql-set-param",6,6,argc,argv);
    }

    if (isUnsignedInt(argv[5]) == FALSE) {
      scheme_wrong_type("sql-set-param","int",5,argc,argv);
    }

    if (isSmallInt(argv[6]) == FALSE) {
      scheme_wrong_type("sql-set-param","small-int",6,argc,argv);
    }

    scheme_get_unsigned_int_val(argv[5],&precision);

    if (paramType == SQL_TIMESTAMP && (precision < 16 || precision > 26)) {
      scheme_signal_error("Timestamp precision must be between 16 and 26");
    }

    scale = (SQLSMALLINT)SCHEME_INT_VAL(argv[6]);

    break;

  default :
    
    precision = SQL_BUFFER_LEN(argv[3]);
    scale = 0L;

    break;
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);
  paramNumber = (SQLUSMALLINT)SCHEME_INT_VAL(argv[1]);
  buffer = SQL_BUFFER_VAL(argv[3]);
  indicator = &SQL_INDICATOR_VAL(argv[4]);

  sr = SQLSetParam(stmtHandle,paramNumber,CDataType,paramType,
		   precision,scale,buffer,indicator);

  checkSQLReturn(sr,"sql-set-param");

  return scheme_void;
}

Scheme_Object *srp_SQLSetStmtAttr(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLINTEGER attribute;
  char *attributeString;
  SQLINTEGER actualLen;
  SQLUINTEGER val;
  char *attributeValString;
  SRP_NAMED_TYPED_CONSTANT *p;    
  SRP_CONST_TYPE attributeType;
  SQLUINTEGER number;
  SQLUINTEGER boolVal;
  SQLUINTEGER paramSetSize;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-set-stmt-attr","sql-hstmt",0,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-set-stmt-attr","symbol",1,argc,argv);
  }

  attributeString = SCHEME_SYM_VAL(argv[1]);

  p = namedTypedConstSearch(attributeString,stmtAttributes);

  if (p == NULL) {
    scheme_signal_error("sql-set-stmt-attr: invalid statement attribute: %s",
			attributeString);
  }

  attribute = p->val;
  attributeType = p->type;

  stmtHandle = SQL_HSTMT_VAL(argv[0]);

  switch(attributeType) {

  case sqluinteger :

    if (SCHEME_EXACT_INTEGERP(argv[2]) == FALSE) {
      scheme_wrong_type("sql-set-stmt-attr","int",2,argc,argv);
    }

    if (scheme_get_unsigned_int_val(argv[2],&number) == 0) {
      scheme_signal_error("Numeric argument too large");
    }

    sr = SQLSetStmtAttr(stmtHandle,attribute,(SQLPOINTER)number,0);
    checkSQLReturn(sr,"sql-set-stmt-attr");

    return scheme_void;

  case sqlbool :

    if (argv[2] == scheme_false) {
      boolVal = SQL_FALSE;
    }
    else {
      boolVal = SQL_TRUE;
    }

    sr = SQLSetStmtAttr(stmtHandle,attribute,(SQLPOINTER)boolVal,0);
    checkSQLReturn(sr,"sql-set-stmt-attr");

    return scheme_void;

  case nameduinteger :

    if (SCHEME_SYMBOLP(argv[2]) == FALSE) {
      scheme_wrong_type("sql-set-stmt-attr","symbol",2,argc,argv);
    }

    attributeValString = SCHEME_SYM_VAL(argv[2]);

    val = findNamedInteger(attributeString,attributeValString,
			   namedStmtAttributes,sizeray(namedStmtAttributes));

    if (val == -1) {
      scheme_signal_error("sql-set-stmt-attr: unknown attribute value: %s",
			  attributeValString);
    }

    sr = SQLSetStmtAttr(stmtHandle,attribute,(SQLPOINTER)val,0);
    checkSQLReturn(sr,"sql-set-stmt-attr");

    return scheme_void;

  case possiblynameduinteger :

    if (SCHEME_SYMBOLP(argv[2])) {

      attributeValString = SCHEME_SYM_VAL(argv[2]);

      val = findNamedInteger(attributeString,attributeValString,
			     namedStmtAttributes,sizeray(namedStmtAttributes));

      if (val == -1) {
	scheme_signal_error("sql-set-stmt-attr: unknown attribute value: %s",
			    attributeValString);
      }

      sr = SQLSetStmtAttr(stmtHandle,attribute,(SQLPOINTER)val,0);
    }
    else if (SCHEME_EXACT_INTEGERP(argv[2])) {
      if (scheme_get_unsigned_int_val(argv[2],&number) == 0) {
	scheme_signal_error("Numeric argument too large");
      }

      sr = SQLSetStmtAttr(stmtHandle,attribute,(SQLPOINTER)number,0);
    }
    else {
      scheme_wrong_type("sql-set-stmt-attr","int or symbol",2,argc,argv);
    }

    checkSQLReturn(sr,"sql-set-stmt-attr");

    return scheme_void;

  case rowstatus :
    
    if (SQL_ROW_STATUSP(argv[2]) == FALSE) {
      scheme_wrong_type("sql-set-stmt-attr","sql-row-status",2,argc,argv);
    }

    sr = SQLSetStmtAttr(stmtHandle,attribute,SQL_ROW_STATUS_VAL(argv[2]),0);

    checkSQLReturn(sr,"sql-set-stmt-attr");

    return scheme_void;

  case sqlboxeduint :

    if (SQL_BOXED_UINTP(argv[2]) == FALSE) {
      scheme_wrong_type("sql-set-stmt-attr","sql-boxed-uint",2,argc,argv);
    }

    sr = SQLSetStmtAttr(stmtHandle,attribute,SQL_BOXED_UINT_VAL(argv[2]),0);
    
    checkSQLReturn(sr,"sql-get-stmt-attr");

    return scheme_void;

  case hdesc :

    if (SQL_HDESCP(argv[2]) == FALSE) {
      scheme_wrong_type("sql-set-stmt-attr","sql-hdesc",2,argc,argv);
    }

    sr = SQLSetStmtAttr(stmtHandle,attribute,SQL_HDESC_VAL(argv[2]),0);
    checkSQLReturn(sr,"sql-get-stmt-attr");

    return scheme_void;

  case opparms :  

    if (SQL_OP_PARMSP(argv[2]) == FALSE) {
      scheme_wrong_type("sql-set-stmt-attr","sql-op-parms",2,argc,argv);
    }

    sr = SQLGetStmtAttr(stmtHandle,SQL_ATTR_PARAMSET_SIZE,&paramSetSize,
			0,&actualLen);
    checkSQLReturn(sr,"sql-get-stmt-attr");

    if (paramSetSize != SQL_OP_PARMS_LEN(argv[2])) {
      scheme_signal_error("Lengnth of operational parameters does not "
			  "match current number of parameters");

    } 
      
    sr = SQLSetStmtAttr(stmtHandle,attribute,SQL_OP_PARMS_VAL(argv[2]),0);

    checkSQLReturn(sr,"sql-set-stmt-attr");

    return scheme_void;

  }

  scheme_signal_error("sql-set-stmt-attr: invalid attribute type: %X",
		      attributeType);

  return scheme_void;
}

Scheme_Object *srp_SQLSetStmtOption(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLUSMALLINT option;
  char *optionString;
  SQLUINTEGER val;
  char *optionValString;
  SRP_NAMED_TYPED_CONSTANT *p;    
  SRP_CONST_TYPE optionType;
  SQLUINTEGER number;
  SQLUINTEGER boolVal;
  SQLUINTEGER paramSetSize;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-set-stmt-option","sql-hstmt",0,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-set-stmt-option","symbol",1,argc,argv);
  }

  optionString = SCHEME_SYM_VAL(argv[1]);

  p = namedTypedConstSearch(optionString,stmtAttributes);

  if (p == NULL) {
    scheme_signal_error("sql-set-stmt-option: invalid statement option: %s",
			optionString);
  }

  option = (SQLUSMALLINT)(p->val);
  optionType = p->type;

  stmtHandle = SQL_HSTMT_VAL(argv[0]);

  switch(optionType) {

  case sqluinteger :

    if (SCHEME_EXACT_INTEGERP(argv[2]) == FALSE) {
      scheme_wrong_type("sql-set-stmt-option","int",2,argc,argv);
    }

    if (scheme_get_unsigned_int_val(argv[2],&number) == 0) {
      scheme_signal_error("Numeric argument too large");
    }

    sr = SQLSetStmtOption(stmtHandle,option,number);
    checkSQLReturn(sr,"sql-set-stmt-option");

    return scheme_void;

  case sqlbool :

    if (argv[2] == scheme_false) {
      boolVal = SQL_FALSE;
    }
    else {
      boolVal = SQL_TRUE;
    }

    sr = SQLSetStmtOption(stmtHandle,option,boolVal);
    checkSQLReturn(sr,"sql-set-stmt-option");

    return scheme_void;

  case nameduinteger :

    if (SCHEME_SYMBOLP(argv[2]) == FALSE) {
      scheme_wrong_type("sql-set-stmt-option","symbol",2,argc,argv);
    }

    optionValString = SCHEME_SYM_VAL(argv[2]);

    val = findNamedInteger(optionString,optionValString,
			   namedStmtAttributes,sizeray(namedStmtAttributes));

    if (val == -1) {
      scheme_signal_error("sql-set-stmt-option: unknown option value: %s",
			  optionValString);
    }

    sr = SQLSetStmtOption(stmtHandle,option,val);
    checkSQLReturn(sr,"sql-set-stmt-option");

    return scheme_void;

  case possiblynameduinteger :

    if (SCHEME_SYMBOLP(argv[2])) {

      optionValString = SCHEME_SYM_VAL(argv[2]);

      val = findNamedInteger(optionString,optionValString,
			     namedStmtAttributes,sizeray(namedStmtAttributes));

      if (val == -1) {
	scheme_signal_error("sql-set-stmt-option: unknown option value: %s",
			    optionValString);
      }

      sr = SQLSetStmtOption(stmtHandle,option,val);
    }
    else if (SCHEME_EXACT_INTEGERP(argv[2])) {
      if (scheme_get_unsigned_int_val(argv[2],&number) == 0) {
	scheme_signal_error("Numeric argument too large");
      }

      sr = SQLSetStmtOption(stmtHandle,option,number);
    }
    else {
      scheme_wrong_type("sql-set-stmt-option","int or symbol",2,argc,argv);
    }

    checkSQLReturn(sr,"sql-set-stmt-option");

    return scheme_void;

  case rowstatus :
    
    if (SQL_ROW_STATUSP(argv[2]) == FALSE) {
      scheme_wrong_type("sql-set-stmt-option","sql-row-status",2,argc,argv);
    }

    sr = SQLSetStmtOption(stmtHandle,option,(SQLUINTEGER)SQL_ROW_STATUS_VAL(argv[2]));

    checkSQLReturn(sr,"sql-set-stmt-option");

    return scheme_void;

  case sqlboxeduint :

    if (SQL_BOXED_UINTP(argv[2]) == FALSE) {
      scheme_wrong_type("sql-set-stmt-option","sql-boxed-uint",2,argc,argv);
    }

    sr = SQLSetStmtOption(stmtHandle,option,(SQLUINTEGER)SQL_BOXED_UINT_VAL(argv[2]));
    
    checkSQLReturn(sr,"sql-get-stmt-option");

    return scheme_void;

  case hdesc :

    if (SQL_HDESCP(argv[2]) == FALSE) {
      scheme_wrong_type("sql-set-stmt-option","sql-hdesc",2,argc,argv);
    }

    sr = SQLSetStmtAttr(stmtHandle,option,SQL_HDESC_VAL(argv[2]),0);
    checkSQLReturn(sr,"sql-get-stmt-option");

    return scheme_void;

  case opparms :  

    if (SQL_OP_PARMSP(argv[2]) == FALSE) {
      scheme_wrong_type("sql-set-stmt-option","sql-op-parms",2,argc,argv);
    }

    sr = SQLGetStmtOption(stmtHandle,SQL_ATTR_PARAMSET_SIZE,&paramSetSize);
    checkSQLReturn(sr,"sql-get-stmt-option");

    if (paramSetSize != SQL_OP_PARMS_LEN(argv[2])) {
      scheme_signal_error("Lengnth of operational parameters does not "
			  "match current number of parameters");

    } 
      
    sr = SQLSetStmtOption(stmtHandle,option,(SQLUINTEGER)SQL_OP_PARMS_VAL(argv[2]));

    checkSQLReturn(sr,"sql-set-stmt-option");

    return scheme_void;

  }

  scheme_signal_error("sql-set-stmt-option: invalid option type: %X",
		      optionType);

  return scheme_void;
}

Scheme_Object *srp_SQLSpecialColumns(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLSMALLINT idType;
  char *idTypeString;
  SQLCHAR *catalogName;
  SQLSMALLINT catalogNameLen;
  SQLCHAR *schemaName;
  SQLSMALLINT schemaNameLen;
  SQLCHAR *tableName;
  SQLSMALLINT tableNameLen;
  SQLSMALLINT scope;
  char *scopeString;
  SQLSMALLINT nullable;
  char *nullableString;
  int i;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-special-columns","sql-hstmt",0,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-special-columns","symbol",1,argc,argv);
  }

  for (i = 2; i <= 4; i++) {
    if (SCHEME_STRINGP(argv[i]) == FALSE) {
      scheme_wrong_type("sql-special-columns","string",i,argc,argv);
    }
  }

  for (i = 5; i <= 6; i++) {
    if (SCHEME_SYMBOLP(argv[i]) == FALSE) {
      scheme_wrong_type("sql-special-columns","symbol",i,argc,argv);
    }
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);

  idTypeString = SCHEME_SYM_VAL(argv[1]);

  if (stricmp(idTypeString,"sql-best-rowid") == 0) {
    idType = SQL_BEST_ROWID;
  }
  else if (stricmp(idTypeString,"sql-rowver") == 0) {
    idType = SQL_ROWVER;
  }
  else {
    scheme_signal_error("sql-special-columns: invalid identifier type: %s",
			idTypeString);
  }

  catalogName = (SQLCHAR *)SCHEME_STR_VAL(argv[2]);
  catalogNameLen = SCHEME_STRLEN_VAL(argv[2]);

  schemaName = (SQLCHAR *)SCHEME_STR_VAL(argv[3]);
  schemaNameLen = SCHEME_STRLEN_VAL(argv[3]);

  tableName = (SQLCHAR *)SCHEME_STR_VAL(argv[4]);
  tableNameLen = SCHEME_STRLEN_VAL(argv[4]);

  scopeString = SCHEME_SYM_VAL(argv[5]);

  if (stricmp(scopeString,"sql-scope-currow") == 0) {
    scope = SQL_SCOPE_CURROW;
  }
  else if (stricmp(scopeString,"sql-scope-transaction") == 0) {
    scope = SQL_SCOPE_TRANSACTION;
  }
  else if (stricmp(scopeString,"sql-scope-session") == 0) {
    scope = SQL_SCOPE_SESSION;
  }
  else {
    scheme_signal_error("sql-special-columns: invalid scope: %s",
			scopeString);
  }

  nullableString = SCHEME_SYM_VAL(argv[6]);

  if (stricmp(nullableString,"sql-no-nulls") == 0) {
    nullable = SQL_NO_NULLS;
  }
  else if (stricmp(nullableString,"sql-nullable") == 0) {
    nullable = SQL_NULLABLE;
  }
  else {
    scheme_signal_error("sql-special-columns: invalid nullable: %s",
			nullableString);
  }

  sr = SQLSpecialColumns(stmtHandle,idType,
			 catalogName,catalogNameLen,
			 schemaName,schemaNameLen,
			 tableName,tableNameLen,
			 scope,nullable);

  checkSQLReturn(sr,"sql-special-columns");

  return scheme_void;
}

Scheme_Object *srp_SQLStatistics(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLCHAR *catalogName;
  SQLSMALLINT catalogNameLen;
  SQLCHAR *schemaName;
  SQLSMALLINT schemaNameLen;
  SQLCHAR *tableName;
  SQLSMALLINT tableNameLen;
  SQLSMALLINT unique;
  char *uniqueString;
  SQLSMALLINT reserved;
  char *reservedString;
  int i;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-statistics","sql-hstmt",0,argc,argv);
  }

  for (i = 1; i <= 3; i++) {
    if (SCHEME_STRINGP(argv[i]) == FALSE) {
      scheme_wrong_type("sql-statistics","string",i,argc,argv);
    }
  }

  for (i = 4; i <= 5; i++) {
    if (SCHEME_SYMBOLP(argv[i]) == FALSE) {
      scheme_wrong_type("sql-statistics","symbol",i,argc,argv);
    }
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);

  catalogName = (SQLCHAR *)SCHEME_STR_VAL(argv[1]);
  catalogNameLen = SCHEME_STRLEN_VAL(argv[1]);

  schemaName = (SQLCHAR *)SCHEME_STR_VAL(argv[2]);
  schemaNameLen = SCHEME_STRLEN_VAL(argv[2]);

  tableName = (SQLCHAR *)SCHEME_STR_VAL(argv[3]);
  tableNameLen = SCHEME_STRLEN_VAL(argv[3]);

  uniqueString = SCHEME_SYM_VAL(argv[4]);

  if (stricmp(uniqueString,"sql-index-unique") == 0) {
    unique = SQL_INDEX_UNIQUE;
  }
  else if (stricmp(uniqueString,"sql-index-all") == 0) {
    unique = SQL_INDEX_ALL;
  }
  else {
    scheme_signal_error("sql-statistics: invalid uniqueness specification: %s",
			uniqueString);
  }

  reservedString = SCHEME_SYM_VAL(argv[5]);

  if (stricmp(reservedString,"sql-ensure") == 0) {
    reserved = SQL_ENSURE;
  }
  else if (stricmp(reservedString,"sql-quick") == 0) {
    reserved = SQL_QUICK;
  }
  else {
    scheme_signal_error("sql-statistics: invalid reserved specification: %s",
			reservedString);
  }

  sr = SQLStatistics(stmtHandle,
		     catalogName,catalogNameLen,
		     schemaName,schemaNameLen,
		     tableName,tableNameLen,
		     unique,reserved);

  checkSQLReturn(sr,"sql-statistics");

  return scheme_void;
}

Scheme_Object *srp_SQLTables(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLCHAR *catalogName;
  SQLSMALLINT catalogNameLen;
  SQLCHAR *schemaName;
  SQLSMALLINT schemaNameLen;
  SQLCHAR *tableName;
  SQLSMALLINT tableNameLen;
  SQLCHAR *tableType;
  SQLSMALLINT tableTypeLen;
  int i;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-tables","sql-hstmt",0,argc,argv);
  }

  for (i = 1; i <= 2; i++) {
    if (SCHEME_STRINGP(argv[i]) == FALSE && 
	SCHEME_SYMBOLP(argv[i]) == FALSE) {
      scheme_wrong_type("sql-tables","string or symbol",i,argc,argv);
    }
  }

  if (SCHEME_STRINGP(argv[3]) == FALSE) {
    scheme_wrong_type("sql-tables","string or symbol",i,argc,argv);
  }

  if (SCHEME_STRINGP(argv[4]) == FALSE && 
      SCHEME_SYMBOLP(argv[4]) == FALSE) {
    scheme_wrong_type("sql-tables","string or symbol",4,argc,argv);
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);

  if (SCHEME_SYMBOLP(argv[1])) {
    char *catalogNameString;

    catalogNameString = SCHEME_SYM_VAL(argv[1]);
    if (stricmp(catalogNameString,"sql-all-catalogs") == 0) {
      catalogName = (SQLCHAR *)SQL_ALL_CATALOGS;
      catalogNameLen = strlen((const char *)catalogName);
    }
    else {
      scheme_signal_error("sql-tables: invalid catalog name: %s",catalogNameString);
    }
  }
  else {
    catalogName = (SQLCHAR *)SCHEME_STR_VAL(argv[1]);
    catalogNameLen = SCHEME_STRLEN_VAL(argv[1]);
  }

  if (SCHEME_SYMBOLP(argv[2])) {
    char *schemaNameString;

    schemaNameString = SCHEME_SYM_VAL(argv[2]);
    if (stricmp(schemaNameString,"sql-all-schemas") == 0) {
      schemaName = (SQLCHAR *)SQL_ALL_SCHEMAS;
      schemaNameLen = strlen((const char *)schemaName);
    }
    else {
      scheme_signal_error("sql-tables: invalid schema name: %s",schemaNameString);
    }
  }
  else {
    schemaName = (SQLCHAR *)SCHEME_STR_VAL(argv[2]);
    schemaNameLen = SCHEME_STRLEN_VAL(argv[2]);
  }

  tableName = (SQLCHAR *)SCHEME_STR_VAL(argv[3]);
  tableNameLen = SCHEME_STRLEN_VAL(argv[3]);

  if (SCHEME_SYMBOLP(argv[4])) {
    char *tableTypeString;

    tableTypeString = SCHEME_SYM_VAL(argv[4]);
    if (stricmp(tableTypeString,"sql-all-table-types") == 0) {
      tableType = (SQLCHAR *)SQL_ALL_TABLE_TYPES;
      tableTypeLen = strlen((const char *)tableType);
    }
    else {
      scheme_signal_error("sql-tables: invalid schema name: %s",tableTypeString);
    }
  }
  else {
    tableType = (SQLCHAR *)SCHEME_STR_VAL(argv[4]);
    tableTypeLen = SCHEME_STRLEN_VAL(argv[4]);
  }

  sr = SQLTables(stmtHandle,
		 catalogName,catalogNameLen,
		 schemaName,schemaNameLen,
		 tableName,tableNameLen,
		 tableType,tableTypeLen);

  checkSQLReturn(sr,"sql-tables");

  return scheme_void;

}

Scheme_Object *srp_SQLTransact(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHENV envHandle;
  SQLHDBC connectionHandle;
  SQLUSMALLINT action;
  char *actionString;

  if (SQL_HENVP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-transact","sql-henv",0,argc,argv);
  }

  if (SQL_HDBCP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-transact","sql-hdbc",1,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[2]) == FALSE) {
    scheme_wrong_type("sql-transact","symbol",2,argc,argv);
  }

  actionString = SCHEME_SYM_VAL(argv[2]);

  if (stricmp(actionString,"sql-commit") == 0) {
    action = SQL_COMMIT;
  }
  else if (stricmp(actionString,"sql-rollback") == 0) {
    action = SQL_ROLLBACK;
  }
  else {
    scheme_signal_error("sql-transact: invalid completion type: %s",
			actionString);
  }

  envHandle = SQL_HENV_VAL(argv[0]);
  connectionHandle = SQL_HDBC_VAL(argv[1]);

  sr = SQLTransact(envHandle,connectionHandle,action);

  checkSQLReturn(sr,"sql-transaction");

  return scheme_void;
}

// Functions in SQLEXT.H

Scheme_Object *srp_SQLDriverConnect(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHDBC connectionHandle;
  SQLCHAR *inConnectString;
  SQLSMALLINT inConnectStringLen;
  SQLCHAR outConnectString[2048];
  SQLSMALLINT actualLen;
  char *completionString;
  SQLUSMALLINT completion; 

  if (SQL_HDBCP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-browse-connect","sql-hdbc",0,argc,argv);
  }
  
  if (SCHEME_STRINGP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-browse-connect","string",1,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[2]) == FALSE) {
    scheme_wrong_type("sql-browse-connect","symbol",2,argc,argv);
  }

  completionString = SCHEME_SYM_VAL(argv[2]);

  if (stricmp(completionString,"sql-driver-prompt") == 0) {
    completion = SQL_DRIVER_PROMPT;
  }
  else if (stricmp(completionString,"sql-driver-complete") == 0) {
    completion = SQL_DRIVER_COMPLETE;
  }
  else if (stricmp(completionString,"sql-driver-complete-required") == 0) {
    completion = SQL_DRIVER_COMPLETE_REQUIRED;
  }
  else if (stricmp(completionString,"sql-driver-no-prompt") == 0) {
    completion = SQL_DRIVER_NOPROMPT;
  }
  else {
    scheme_signal_error("sql-driver-connect: invalid completion: %s",
			completionString);
  }

  connectionHandle = SQL_HDBC_VAL(argv[0]);
  inConnectString = (SQLCHAR *)SCHEME_STR_VAL(argv[1]);
  inConnectStringLen = SCHEME_STRLEN_VAL(argv[1]);
  
  sr = SQLDriverConnect(connectionHandle,
			GetDesktopWindow(),
			inConnectString,inConnectStringLen,
			outConnectString,sizeray(outConnectString),
			&actualLen,completion);

  checkSQLReturn(sr,"sql-driver-connect");

  return scheme_make_sized_string((char *)outConnectString,actualLen,TRUE);
}

Scheme_Object *srp_SQLBrowseConnect(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHDBC connectionHandle;
  SQLCHAR *inConnectString;
  SQLSMALLINT inConnectStringLen;
  SQLCHAR outConnectString[2048];
  SQLSMALLINT actualLen;

  if (SQL_HDBCP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-browse-connect","sql-hdbc",0,argc,argv);
  }
  
  if (SCHEME_STRINGP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-browse-connect","string",1,argc,argv);
  }
  
  connectionHandle = SQL_HDBC_VAL(argv[0]);
  inConnectString = (SQLCHAR *)SCHEME_STR_VAL(argv[1]);
  inConnectStringLen = SCHEME_STRLEN_VAL(argv[1]);
  
  sr = SQLBrowseConnect(connectionHandle,
			inConnectString,inConnectStringLen,
			outConnectString,sizeray(outConnectString),
			&actualLen);

  checkSQLReturn(sr,"sql-browse-connect");

  return scheme_make_sized_string((char *)outConnectString,actualLen,TRUE);
}

Scheme_Object *srp_SQLBulkOperations(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLSMALLINT operation;
  char *operationString;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-bulk-operations","sql-hstmt",0,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-bulk-operations","symbol",1,argc,argv);
  }

  operationString = SCHEME_SYM_VAL(argv[1]);
  
  if (stricmp(operationString,"sql-add") == 0) {
    operation = SQL_ADD;
  }
  else if (stricmp(operationString,"sql-update-by-bookmark") == 0) {
    operation = SQL_UPDATE_BY_BOOKMARK;
  }
  else if (stricmp(operationString,"sql-delete-by-bookmark") == 0) {
    operation = SQL_DELETE_BY_BOOKMARK;
  }
  else if (stricmp(operationString,"sql-fetch-by-bookmark") == 0) {
    operation = SQL_FETCH_BY_BOOKMARK;
  }
  else {
    scheme_signal_error("sql-bulk-operations: invalid operation: %s",
			operationString);
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);

  sr = SQLBulkOperations(stmtHandle,operation);

  checkSQLReturn(sr,"sql-bulk-operations");

  return scheme_void;
}

Scheme_Object *srp_SQLColAttributes(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLUSMALLINT colNumber;
  SQLUSMALLINT fieldId;
  char *fieldIdString;
  char buff[2048];
  SQLSMALLINT bufflen;
  SQLINTEGER numBuffer;
  SQLSMALLINT actualLen;
  SRP_NAMED_TYPED_CONSTANT *p;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-col-attributes","sql-hstmt",0,argc,argv);
  }

  if (isSmallInt(argv[1]) == FALSE) {
    scheme_wrong_type("sql-col-attributes","small-int",1,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[2]) == FALSE) {
    scheme_wrong_type("sql-col-attributes","symbol",2,argc,argv);
  }

  fieldIdString = SCHEME_SYM_VAL(argv[2]);

  p = namedTypedConstSearch(fieldIdString,colAttributesOld);

  if (p == NULL) {
    scheme_signal_error("Invalid column attribute: %s",fieldIdString);
  }
    
  fieldId = (SQLUSMALLINT)(p->val);
  stmtHandle = SQL_HSTMT_VAL(argv[0]);
  colNumber = (SQLSMALLINT)SCHEME_INT_VAL(argv[1]);

  switch(p->type) {

  case sqlbool :

    bufflen = SQL_IS_INTEGER;
    sr = SQLColAttribute(stmtHandle,colNumber,fieldId,
			 buff,bufflen,&actualLen,&numBuffer);
    checkSQLReturn(sr,"sql-col-attributes");		       
    return (numBuffer == SQL_FALSE) ? scheme_false : scheme_true;

  case sqlinteger :

    bufflen = SQL_IS_INTEGER;
    sr = SQLColAttribute(stmtHandle,colNumber,fieldId,
			 buff,bufflen,&actualLen,&numBuffer);
    checkSQLReturn(sr,"sql-col-attributes");		       
    return scheme_make_integer_value((long)numBuffer);

  case namedinteger :

    bufflen = SQL_IS_INTEGER;
    sr = SQLColAttribute(stmtHandle,colNumber,fieldId,
			 buff,bufflen,&actualLen,&numBuffer);
    checkSQLReturn(sr,"sql-col-attributes");		       

    return scheme_intern_symbol(findIntegerName(fieldIdString,numBuffer,
						namedColAttrsIntegers,
						sizeray(namedColAttrsIntegers)));

  case string :

    bufflen = sizeof(buff);
    sr = SQLColAttribute(stmtHandle,colNumber,fieldId,
			 buff,bufflen,&actualLen,&numBuffer);
    checkSQLReturn(sr,"sql-col-attributes");		       
    return scheme_make_sized_string(buff,actualLen,TRUE);
  }

  scheme_signal_error("sql-col-attributes: invalid attribute type");

  return scheme_void; // unreachable
} 

Scheme_Object *srp_SQLColumnPrivileges(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLCHAR *catalogName;
  SQLSMALLINT catalogNameLen;
  SQLCHAR *schemaName;
  SQLSMALLINT schemaNameLen;
  SQLCHAR *tableName;
  SQLSMALLINT tableNameLen;
  SQLCHAR *columnName;
  SQLSMALLINT columnNameLen;
  int i;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-column-privileges","sql-hstmt",0,argc,argv);
  }

  for (i = 1; i <= 4; i++) {
    if (SCHEME_STRINGP(argv[i]) == FALSE) {
      scheme_wrong_type("sql-column-privileges","string",i,argc,argv);
    }
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);
  catalogName = (SQLCHAR *)SCHEME_STR_VAL(argv[1]);
  catalogNameLen = SCHEME_STRLEN_VAL(argv[1]);
  schemaName = (SQLCHAR *)SCHEME_STR_VAL(argv[2]);
  schemaNameLen = SCHEME_STRLEN_VAL(argv[2]);
  tableName = (SQLCHAR *)SCHEME_STR_VAL(argv[3]);
  tableNameLen = SCHEME_STRLEN_VAL(argv[3]);
  columnName = (SQLCHAR *)SCHEME_STR_VAL(argv[4]);
  columnNameLen = SCHEME_STRLEN_VAL(argv[4]);

  sr = SQLColumnPrivileges(stmtHandle,
			   catalogName,catalogNameLen,
			   schemaName,schemaNameLen,
			   tableName,tableNameLen,
			   columnName,columnNameLen);

  checkSQLReturn(sr,"sql-column-privileges");

  return scheme_void;
}

Scheme_Object *srp_SQLDescribeParam(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLSMALLINT paramNumber;
  SQLSMALLINT dataType;
  SQLUINTEGER paramSize;
  SQLSMALLINT decimalDigits;
  SQLSMALLINT nullable;
  char *nullableString;
  char *dataTypeString;
  int i;
  Scheme_Object *retval;
  
  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-describe-param","sql-hstmt",0,argc,argv);
  }

  if (isUnsignedSmallInt(argv[1]) == FALSE) {
    scheme_wrong_type("sql-describe-param","sql-hstmt",0,argc,argv);
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);
  paramNumber = (SQLUSMALLINT)SCHEME_INT_VAL(argv[1]);

  sr = SQLDescribeParam(stmtHandle,paramNumber,
			&dataType,&paramSize,&decimalDigits,
			&nullable);

  checkSQLReturn(sr,"sql-describe-param");		       

  dataTypeString = "?";

  for (i = 0; i < sizeray(SQLDataTypes); i++) {
    if (dataType == SQLDataTypes[i].val) {
      dataTypeString = SQLDataTypes[i].scheme_name;
      break;
    }
  }

  nullableString = nullableToString(nullable);

  retval = scheme_null;
  retval = scheme_make_pair(scheme_intern_symbol(nullableString),retval);
  retval = scheme_make_pair(scheme_make_integer_value(decimalDigits),retval);
  retval = scheme_make_pair(scheme_make_integer_value_from_unsigned(paramSize),retval);
  retval = scheme_make_pair(scheme_intern_symbol(dataTypeString),retval);
  
  return retval;
}

Scheme_Object *srp_SQLExtendedFetch(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLUSMALLINT fetchType;
  char *fetchTypeString;
  SQLINTEGER rowNumber;
  SRP_SQL_ROW_STATUS *rowStatus;
  SRP_NAMED_SMALL_CONSTANT *p;      
  SQLINTEGER actualLen;
  SQLINTEGER maxRows;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-extended-fetch","sql-hstmt",0,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-extended-fetch","symbol",1,argc,argv);
  }
  
  if (SCHEME_INTP(argv[2]) == FALSE) {
    scheme_wrong_type("sql-extended-fetch","int",2,argc,argv);
  }
  
  fetchTypeString = SCHEME_SYM_VAL(argv[1]);

  p = namedSmallConstSearch(fetchTypeString,fetchOrientation);

  if (p == NULL) {
    scheme_signal_error("sql-extended-fetch: invalid fetch orientation: %s",
			fetchTypeString);
  }

  fetchType = p->val;

  stmtHandle = SQL_HSTMT_VAL(argv[0]);

  sr = SQLGetStmtAttr(stmtHandle,SQL_ATTR_MAX_ROWS,&maxRows,0,&actualLen);

  checkSQLReturn(sr,"sql-get-stmt-attr");

  // need to keep rowStatus around until cursor closed
  // conservatively, make it uncollectable

  rowStatus = (SRP_SQL_ROW_STATUS *)scheme_malloc_eternal(sizeof(SRP_SQL_ROW_STATUS));
  rowStatus->type = sql_row_status_type;
  rowStatus->numRows = 0;
  rowStatus->values = (SQLUSMALLINT *)scheme_malloc_eternal(maxRows * sizeof(SQLUSMALLINT));

  rowNumber = SCHEME_INT_VAL(argv[1]);

  sr = SQLExtendedFetch(stmtHandle,fetchType,rowNumber,
			&rowStatus->numRows,rowStatus->values);

  checkSQLReturn(sr,"sql-extended-fetch");

  return (Scheme_Object *)rowStatus;
}

Scheme_Object *srp_SQLForeignKeys(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLCHAR *catalogName;
  SQLSMALLINT catalogNameLen;
  SQLCHAR *schemaName;
  SQLSMALLINT schemaNameLen;
  SQLCHAR *tableName;
  SQLSMALLINT tableNameLen;
  SQLCHAR *foreignCatalogName;
  SQLSMALLINT foreignCatalogNameLen;
  SQLCHAR *foreignSchemaName;
  SQLSMALLINT foreignSchemaNameLen;
  SQLCHAR *foreignTableName;
  SQLSMALLINT foreignTableNameLen;
  int i;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-foreign-keys","sql-hstmt",0,argc,argv);
  }

  for (i = 1; i <= 6; i++) {
    if (SCHEME_STRINGP(argv[i]) == FALSE) {
      scheme_wrong_type("sql-foreign-keys","string",i,argc,argv);
    }
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);

  catalogName = (SQLCHAR *)SCHEME_STR_VAL(argv[1]);
  catalogNameLen = SCHEME_STRLEN_VAL(argv[1]);
  
  schemaName = (SQLCHAR *)SCHEME_STR_VAL(argv[2]);
  schemaNameLen = SCHEME_STRLEN_VAL(argv[2]);
  
  tableName = (SQLCHAR *)SCHEME_STR_VAL(argv[3]);
  tableNameLen = SCHEME_STRLEN_VAL(argv[3]);

  foreignCatalogName = (SQLCHAR *)SCHEME_STR_VAL(argv[4]);
  foreignCatalogNameLen = SCHEME_STRLEN_VAL(argv[4]);
  
  foreignSchemaName = (SQLCHAR *)SCHEME_STR_VAL(argv[5]);
  foreignSchemaNameLen = SCHEME_STRLEN_VAL(argv[5]);
  
  foreignTableName = (SQLCHAR *)SCHEME_STR_VAL(argv[6]);
  foreignTableNameLen = SCHEME_STRLEN_VAL(argv[6]);

  sr = SQLForeignKeys(stmtHandle,
		      catalogName,catalogNameLen,
		      schemaName,schemaNameLen,
		      tableName,tableNameLen,
		      foreignCatalogName,foreignCatalogNameLen,
		      foreignSchemaName,foreignSchemaNameLen,
		      foreignTableName,foreignTableNameLen);

  checkSQLReturn(sr,"sql-foreign-keys");
  
  return scheme_void;
}

Scheme_Object *srp_SQLMoreResults(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-more-results","sql-hstmt",0,argc,argv);
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);

  sr = SQLMoreResults(stmtHandle);

  checkSQLReturn(sr,"sql-more-results");  

  return scheme_void;
}

Scheme_Object *srp_SQLNativeSql(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHDBC connectionHandle;
  SQLCHAR *inSql;
  SQLINTEGER inSqlLen;
  SQLCHAR *outSql;
  SQLINTEGER outSqlLen;
  SQLINTEGER actualLen;

  if (SQL_HDBCP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-native-sql","sql-hdbc",0,argc,argv);
  }

  if (SCHEME_STRINGP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-native-sql","string",1,argc,argv);
  }

  connectionHandle = SQL_HDBC_VAL(argv[0]);

  inSql = (SQLCHAR *)SCHEME_STR_VAL(argv[1]);
  inSqlLen = SCHEME_STRLEN_VAL(argv[1]);

  outSqlLen = 2 * inSqlLen;
  outSql = (SQLCHAR *)scheme_malloc(outSqlLen * sizeof(*outSql));

  sr = SQLNativeSql(connectionHandle,
		    inSql,inSqlLen,
		    outSql,outSqlLen,
		    &actualLen);

  checkSQLReturn(sr,"sql-native-sql");    

  return scheme_make_sized_string((char *)outSql,actualLen,TRUE);
}

Scheme_Object *srp_SQLNumParams(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLSMALLINT numParams;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-num-params","sql-hstmt",0,argc,argv);
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);  

  sr = SQLNumParams(stmtHandle,&numParams);

  checkSQLReturn(sr,"sql-num-params");    

  return scheme_make_integer(numParams);
}

Scheme_Object *srp_SQLParamOptions(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLUINTEGER cRow;
  SQLUINTEGER piRow;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-param-options","sql-hstmt",0,argc,argv);
  }

  if (isUnsignedInt(argv[1]) == FALSE) {
    scheme_wrong_type("sql-param-options","unsigned-int",1,argc,argv);
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);
  scheme_get_unsigned_int_val(argv[1],&cRow);

  sr = SQLParamOptions(stmtHandle,cRow,&piRow);
 
  checkSQLReturn(sr,"sql-param-options");    

  return scheme_make_integer_value_from_unsigned(piRow);
}

Scheme_Object *srp_SQLPrimaryKeys(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLCHAR *catalogName;
  SQLSMALLINT catalogNameLen;
  SQLCHAR *schemaName;
  SQLSMALLINT schemaNameLen;
  SQLCHAR *tableName;
  SQLSMALLINT tableNameLen;
  int i;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-primary-keys","sql-hstmt",0,argc,argv);
  }

  for (i = 1; i <= 3; i++) {
    if (SCHEME_STRINGP(argv[i]) == FALSE) {
      scheme_wrong_type("sql-primary-keys","string",i,argc,argv);
    }
  } 
  
  stmtHandle = SQL_HSTMT_VAL(argv[0]);
  
  catalogName = (SQLCHAR *)SCHEME_STR_VAL(argv[1]);
  catalogNameLen = SCHEME_STRLEN_VAL(argv[1]);

  schemaName = (SQLCHAR *)SCHEME_STR_VAL(argv[2]);
  schemaNameLen = SCHEME_STRLEN_VAL(argv[2]);

  tableName = (SQLCHAR *)SCHEME_STR_VAL(argv[3]);
  tableNameLen = SCHEME_STRLEN_VAL(argv[3]);

  sr = SQLPrimaryKeys(stmtHandle,
		      catalogName,catalogNameLen,
		      schemaName,schemaNameLen,
		      tableName,tableNameLen);

  checkSQLReturn(sr,"sql-primary-keys");    

  return scheme_void;
}

Scheme_Object *srp_SQLProcedureColumns(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLCHAR *catalogName;
  SQLSMALLINT catalogNameLen;
  SQLCHAR *schemaName;
  SQLSMALLINT schemaNameLen;
  SQLCHAR *procName;
  SQLSMALLINT procNameLen;
  SQLCHAR *columnName;
  SQLSMALLINT columnNameLen;
  int i;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-procedure-columns","sql-hstmt",0,argc,argv);
  }

  for (i = 1; i <= 4; i++) {
    if (SCHEME_STRINGP(argv[i]) == FALSE) {
      scheme_wrong_type("sql-procedure-columns","string",i,argc,argv);
    }
  } 
  
  stmtHandle = SQL_HSTMT_VAL(argv[0]);
  
  catalogName = (SQLCHAR *)SCHEME_STR_VAL(argv[1]);
  catalogNameLen = SCHEME_STRLEN_VAL(argv[1]);

  schemaName = (SQLCHAR *)SCHEME_STR_VAL(argv[2]);
  schemaNameLen = SCHEME_STRLEN_VAL(argv[2]);

  procName = (SQLCHAR *)SCHEME_STR_VAL(argv[3]);
  procNameLen = SCHEME_STRLEN_VAL(argv[3]);

  columnName = (SQLCHAR *)SCHEME_STR_VAL(argv[4]);
  columnNameLen = SCHEME_STRLEN_VAL(argv[4]);

  sr = SQLProcedureColumns(stmtHandle,
			   catalogName,catalogNameLen,
			   schemaName,schemaNameLen,
			   procName,procNameLen,
			   columnName,columnNameLen);

  checkSQLReturn(sr,"sql-procedure-columns");    

  return scheme_void;

}

Scheme_Object *srp_SQLProcedures(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLCHAR *catalogName;
  SQLSMALLINT catalogNameLen;
  SQLCHAR *schemaName;
  SQLSMALLINT schemaNameLen;
  SQLCHAR *procName;
  SQLSMALLINT procNameLen;
  int i;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-procedures","sql-hstmt",0,argc,argv);
  }

  for (i = 1; i <= 3; i++) {
    if (SCHEME_STRINGP(argv[i]) == FALSE) {
      scheme_wrong_type("sql-procedures","string",i,argc,argv);
    }
  } 

  stmtHandle = SQL_HSTMT_VAL(argv[0]);
  
  catalogName = (SQLCHAR *)SCHEME_STR_VAL(argv[1]);
  catalogNameLen = SCHEME_STRLEN_VAL(argv[1]);

  schemaName = (SQLCHAR *)SCHEME_STR_VAL(argv[2]);
  schemaNameLen = SCHEME_STRLEN_VAL(argv[2]);

  procName = (SQLCHAR *)SCHEME_STR_VAL(argv[3]);
  procNameLen = SCHEME_STRLEN_VAL(argv[3]);

  sr = SQLProcedures(stmtHandle,
		     catalogName,catalogNameLen,
		     schemaName,schemaNameLen,
		     procName,procNameLen);

  checkSQLReturn(sr,"sql-procedures");    

  return scheme_void;
}

Scheme_Object *srp_SQLSetPos(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLUSMALLINT rowNumber;
  SQLUSMALLINT operation;
  char *operationString;
  SQLUSMALLINT lock;
  char *lockString;
  SRP_NAMED_SMALL_CONSTANT *p;
  int i;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-set-pos","sql-hstmt",0,argc,argv);
  }

  if (isUnsignedSmallInt(argv[1]) == FALSE) {
    scheme_wrong_type("sql-set-pos","unsigned-small-int",1,argc,argv);
  }

  for (i = 2; i <= 3; i++) {
    if (SCHEME_SYMBOLP(argv[i]) == FALSE) {
      scheme_wrong_type("sql-set-pos","symbol",i,argc,argv);
    }
  }

  operationString = SCHEME_SYM_VAL(argv[2]);
  lockString = SCHEME_SYM_VAL(argv[3]);

  p = namedSmallConstSearch(operationString,posOperations);

  if (p == NULL) {
    scheme_signal_error("sql-set-pos: invalid operation: %s",operationString);
  }

  operation = p->val;
  
  p = namedSmallConstSearch(lockString,lockTypes);

  if (p == NULL) {
    scheme_signal_error("sql-set-pos: invalid lock type: %s",lockString);
  }

  lock = p->val;

  stmtHandle = SQL_HSTMT_VAL(argv[0]);
  rowNumber = (SQLUSMALLINT)SCHEME_INT_VAL(argv[1]);
  
  sr = SQLSetPos(stmtHandle,rowNumber,operation,lock);

  checkSQLReturn(sr,"sql-set-pos");    

  return scheme_void;
}

Scheme_Object *srp_SQLTablePrivileges(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLCHAR *catalogName;
  SQLSMALLINT catalogNameLen;
  SQLCHAR *schemaName;
  SQLSMALLINT schemaNameLen;
  SQLCHAR *tableName;
  SQLSMALLINT tableNameLen;
  int i;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-table-privileges","sql-hstmt",0,argc,argv);
  }

  for (i = 1; i <= 3; i++) {
    if (SCHEME_STRINGP(argv[i]) == FALSE) {
      scheme_wrong_type("sql-table-privileges","string",i,argc,argv);
    }
  } 
  
  stmtHandle = SQL_HSTMT_VAL(argv[0]);
  
  catalogName = (SQLCHAR *)SCHEME_STR_VAL(argv[1]);
  catalogNameLen = SCHEME_STRLEN_VAL(argv[1]);

  schemaName = (SQLCHAR *)SCHEME_STR_VAL(argv[2]);
  schemaNameLen = SCHEME_STRLEN_VAL(argv[2]);

  tableName = (SQLCHAR *)SCHEME_STR_VAL(argv[3]);
  tableNameLen = SCHEME_STRLEN_VAL(argv[3]);

  sr = SQLTablePrivileges(stmtHandle,
			  catalogName,catalogNameLen,
			  schemaName,schemaNameLen,
			  tableName,tableNameLen);

  checkSQLReturn(sr,"sql-table-privileges");    

  return scheme_void;
}

Scheme_Object *srp_SQLDrivers(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHENV envHandle;
  SQLUSMALLINT selection;
  char *selectionString;
  SQLCHAR description[2048];
  SQLCHAR attributes[2048];
  SQLSMALLINT actualLen1,actualLen2;
  Scheme_Object *retval;

  if (SQL_HENVP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-drivers","sql-henv",0,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-drivers","symbol",1,argc,argv);
  }

  selectionString = SCHEME_SYM_VAL(argv[1]);

  if (stricmp(selectionString,"sql-fetch-first") == 0) {
    selection = SQL_FETCH_FIRST;
  }
  else if (stricmp(selectionString,"sql-fetch-next") == 0) {
    selection = SQL_FETCH_NEXT;
  }
  else {
    scheme_signal_error("sql-drivers: invalid selection: %s",
			selectionString);
  }

  envHandle = SQL_HENV_VAL(argv[0]);
  
  sr = SQLDrivers(envHandle,selection,
		  description,sizeray(description),&actualLen1,
		  attributes,sizeray(attributes),&actualLen2);

  checkSQLReturn(sr,"sql-drivers");    

  retval = scheme_null;
  retval = scheme_make_pair(scheme_make_sized_string((char *)attributes,
						     actualLen2,TRUE),
			    retval);
  retval = scheme_make_pair(scheme_make_sized_string((char *)description,
						     actualLen1,TRUE),
			    retval);
					       
  return retval;
}

Scheme_Object *srp_SQLBindParameter(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  SQLHSTMT stmtHandle;
  SQLUSMALLINT paramNumber;
  SQLSMALLINT ioType;
  char *ioTypeString;
  SQLSMALLINT valueType;
  SQLSMALLINT paramType;
  char *paramTypeString;
  SQLUINTEGER valueSize;
  SQLSMALLINT decimalDigits;
  SQLPOINTER buffer;
  SQLINTEGER bufferLen;
  SQLINTEGER indicator;
  SRP_NAMED_SMALL_CONSTANT *p;      
  int i;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-bind-parameter","sql-hstmt",0,argc,argv);
  }

  if (isUnsignedSmallInt(argv[1]) == FALSE) {
    scheme_wrong_type("sql-bind-parameter","unsigned-small-int",1,argc,argv);
  }

  for (i = 2; i <= 3; i++) {
    if (SCHEME_SYMBOLP(argv[i]) == FALSE) {
      scheme_wrong_type("sql-bind-parameter","symbol",i,argc,argv);
    }
  }

  if (isSmallInt(argv[4]) == FALSE) {
    scheme_wrong_type("sql-bind-parameter","small-int",4,argc,argv);
  }
  
  if (SQL_BUFFERP(argv[5]) == FALSE) {
    scheme_wrong_type("sql-bind-parameter","sql-buffer",5,argc,argv);
  }

  if (SQL_INDICATORP(argv[6]) == FALSE) {
    scheme_wrong_type("sql-bind-parameter","sql-indicator",6,argc,argv);
  }

  ioTypeString = SCHEME_SYM_VAL(argv[2]);

  if (stricmp(ioTypeString,"sql-param-input") == 0) {
    ioType = SQL_PARAM_INPUT;
  }
  else if (stricmp(ioTypeString,"sql-param-input-output") == 0) {
    ioType = SQL_PARAM_INPUT_OUTPUT;
  }
  else if (stricmp(ioTypeString,"sql-param-output") == 0) {
    ioType = SQL_PARAM_OUTPUT;
  }
  else {
    scheme_signal_error("sql-bind-parameter: invalid I/O type: %s",
			ioTypeString);
  }

  paramTypeString = SCHEME_SYM_VAL(argv[3]);

  p = namedSmallConstSearch(paramTypeString,SQLDataTypes);

  if (p == NULL) {
    scheme_signal_error("sql-bind-parameter: invalid parameter type: %s",
			paramTypeString);
  }

  paramType = p->val;

  switch(paramType) {

  case SQL_DECIMAL :
  case SQL_NUMERIC :
  case SQL_TIME :
  case SQL_TIMESTAMP :
  case SQL_TYPE_TIME :
  case SQL_INTERVAL_SECOND :
  case SQL_INTERVAL_DAY_TO_SECOND :
  case SQL_INTERVAL_HOUR_TO_SECOND :
  case SQL_INTERVAL_MINUTE_TO_SECOND :

      // need Decimals

      if (argc != 8) {
	scheme_wrong_count("sql-bind-parameter",8,8,argc,argv);
      }

      if (isSmallInt(argv[7]) == FALSE) {
	scheme_wrong_type("sql-bind-parameter","small-int",7,argc,argv);
      }

      decimalDigits = (SQLSMALLINT)SCHEME_INT_VAL(argv[6]);

      break;


  default :

    decimalDigits = 0;
    break;
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);
  paramNumber = (SQLUSMALLINT)SCHEME_INT_VAL(argv[1]);
  buffer = SQL_BUFFER_VAL(argv[5]);
  valueType = SQL_BUFFER_CTYPE(argv[5]);
  bufferLen = SQL_BUFFER_LEN(argv[5]);
  scheme_get_unsigned_int_val(argv[4],&valueSize);
  indicator = SQL_INDICATOR_VAL(argv[6]);

  sr = SQLBindParameter(stmtHandle,paramNumber,ioType,
			valueType,paramType,
			valueSize,decimalDigits,
			buffer,bufferLen,&indicator);

  checkSQLReturn(sr,"sql-bind-param");    

  return scheme_void;			
}

Scheme_Object *srp_SQLSetScrollOptions(int argc,Scheme_Object **argv) {
  SQLRETURN sr;
  HSTMT stmtHandle;
  SQLUSMALLINT concur;
  char *concurString;
  SQLINTEGER keyset;
  SQLUSMALLINT rowset;
  SRP_NAMED_SMALL_CONSTANT *p;

  if (SQL_HSTMTP(argv[0]) == FALSE) {
    scheme_wrong_type("sql-set-scroll-options","sql-hstmt",0,argc,argv);
  }

  if (SCHEME_SYMBOLP(argv[1]) == FALSE) {
    scheme_wrong_type("sql-set-scroll-options","symbol",1,argc,argv);
  }

  // deal with argv[2] below

  if (isUnsignedSmallInt(argv[3]) == FALSE) {
    scheme_wrong_type("sql-set-scroll-options","unsigned-small-int",3,argc,argv);
  }

  concurString = SCHEME_SYM_VAL(argv[1]);

  p = namedSmallConstSearch(concurString,scrollConcurrency);

  if (p == NULL) {
    scheme_signal_error("sql-set-scroll-options: invalid concurrency: %s",
			concurString);
  }

  concur = p->val;

  rowset = (SQLUSMALLINT)SCHEME_INT_VAL(argv[3]);  

  if (SCHEME_SYMBOLP(argv[2])) {
    char *keysetString;
    SRP_NAMED_CONSTANT *q;

    keysetString = SCHEME_SYM_VAL(argv[2]);
    
    q = namedConstSearch(keysetString,scrollCursor);

    if (q == NULL) {
      scheme_signal_error("sql-set-scroll-options: invalid keyset: %s",
			  keysetString);
    }

    keyset = q->val;
  }
  else if (SCHEME_EXACT_INTEGERP(argv[2])) {
    if (scheme_get_int_val(argv[1],&keyset) == 0) {
      scheme_signal_error("sql-set-scroll-options: keyset value too large");
    }

    if (keyset < rowset) {
      scheme_signal_error("sql-set-scroll-options: keyset smaller than rowset");
    }
  }
  else {
    scheme_wrong_type("sql-set-scroll-options","symbol or int",2,argc,argv);
  }

  stmtHandle = SQL_HSTMT_VAL(argv[0]);

  sr = SQLSetScrollOptions(stmtHandle,concur,keyset,rowset);

  checkSQLReturn(sr,"sql-set-scroll-options");    

  return scheme_void;
}

Scheme_Object *stringArrayToSchemeSymbolList(char **strs,int count) {
  Scheme_Object *retval;
  int i;

  retval = scheme_null;

  for (i = count-1; i >= 0; i--) {
    retval = scheme_make_pair(scheme_intern_symbol(strs[i]),retval);
  }

  return retval;
}

char **schemeSymbolListToStringArray(Scheme_Object **objs,int n) {
  char **strs;
  int i;

  strs = (char **)scheme_malloc(n * sizeof(char *));

  for (i = 0; i < n; i++) {
    strs[i] = SCHEME_SYM_VAL(objs[i]);
  }

  return strs;
}

Scheme_Object *srp_unit_init(Scheme_Object **boxes,Scheme_Object **anchors,
			     Scheme_Unit *m,void *debug_request) {
  int nameCount;
  int i,j,k;

  for (i = 0; i < sizeray(srpPrims); i++) {
    SCHEME_ENVBOX_VAL(boxes[i]) = scheme_make_prim_w_arity(srpPrims[i].c_fun,
							   srpPrims[i].name,
							   srpPrims[i].minargs,
							   srpPrims[i].maxargs);
    anchors[i] = boxes[i];
  }

  for (i = 0,k = sizeray(srpPrims); i < sizeray(srpStructs); i++) {
    nameCount = srpStructs[i].nameCount;
    for (j = 0; j < nameCount; j++,k++) {
      SCHEME_ENVBOX_VAL(boxes[k]) = (*(srpStructs[i].pStructFuns))[j];
      anchors[k] = boxes[k];
    }
  }
  
  return scheme_void;
}

Scheme_Object *scheme_initialize(Scheme_Env *env) {
  Scheme_Object *structType,*structNameSymbol,**structNames,**structValues;
  int nameCount;
  int structNameCount;
  int i,j,k;

  sql_date_type = scheme_make_type("<sql-date>");
  sql_decimal_type = scheme_make_type("<sql-decimal>");
  sql_pointer_type = scheme_make_type("<sql-pointer>");
  sql_time_type = scheme_make_type("<sql-time>");
  sql_timestamp_type = scheme_make_type("<sql-timestamp>");
  sql_return_type = scheme_make_type("<sql-return>");
  sql_henv_type = scheme_make_type("<sql-henv>");
  sql_hdbc_type = scheme_make_type("<sql-hdbc>");
  sql_hstmt_type = scheme_make_type("<sql-hstmt>");
  sql_hdesc_type = scheme_make_type("<sql-hdesc>");
  sql_boxed_uint_type = scheme_make_type("<sql-boxed-uint>");
  sql_buffer_type = scheme_make_type("<sql-buffer>");
  sql_length_type = scheme_make_type("<sql-length>");
  sql_indicator_type = scheme_make_type("<sql-indicator>");
  sql_row_status_type = scheme_make_type("<sql-row-status>");
  sql_array_status_type = scheme_make_type("<sql-array-status>");
  sql_binding_offset_type = scheme_make_type("<sql-binding-offset>");
  sql_rows_processed_type = scheme_make_type("<sql-rows-processed>");
  sql_octet_length_type = scheme_make_type("<sql-octet-length>");
  sql_op_parms_type = scheme_make_type("<sql-op-parms>");
  sql_guid_type = scheme_make_type("<sql-guid>");
  sql_paramlength_type = scheme_make_type("<sql-paramlength>");

  namedBitsDictSort(namedInfoSmallInts);
  namedBitsDictSort(namedInfoIntegers);
  namedBitsDictSort(namedColAttrIntegers);
  namedBitsDictSort(namedColAttrsIntegers);
  namedBitsDictSort(namedConnectAttrIntegers);
  namedBitsDictSort(namedEnvAttrIntegers);
  namedBitsDictSort(bitMaskTable);
  namedBitsDictSort(namedFieldDescriptors);
  namedBitsDictSort(namedDiagFieldIntegers);

  namedTypedConstSort(sqlInfo);
  namedTypedConstSort(colAttributes);
  namedTypedConstSort(colAttributesOld);
  namedTypedConstSort(fieldDescriptors);
  namedTypedConstSort(settableConnectionAttributes);
  namedTypedConstSort(readOnlyConnectionAttributes);
  namedTypedConstSort(envAttributes);

  namedConstSort(sqlFunctions);
  namedConstSort(diagFields);
  namedConstSort(fetchOrientation);
  namedConstSort(fetchScrolls);
  namedConstSort(scrollConcurrency);
  namedConstSort(scrollCursor);
  namedConstSort(fetchDirections);
  namedConstSort(buffLenTypes);
  namedConstSort(posOperations);
  namedConstSort(lockTypes);
  namedConstSort(stmtFreeOptions);
  namedConstSort(CDataTypes);
  namedConstSort(SQLDataTypes);
  namedConstSort(descDataTypes);
  namedConstSort(SQLDescUnnamed);
  namedConstSort(SQLDescSearchable);
  namedConstSort(SQLDescParameterTypes);
  namedConstSort(datetimeIntervalCodes);
  namedConstSort(SQLColNumbers);
  namedConstSort(SQLRowNumbers);

  // structures

  structNameCount = 0;

  for (i = 0; i < sizeray(srpStructs); i++) {
    structNameSymbol = scheme_intern_symbol(srpStructs[i].name);
    structType = scheme_make_struct_type(structNameSymbol,NULL,srpStructs[i].numFields);
    structNames = scheme_make_struct_names(structNameSymbol,
					   stringArrayToSchemeSymbolList(srpStructs[i].fields,srpStructs[i].numFields),
					   0,&nameCount);
    srpStructs[i].names = schemeSymbolListToStringArray(structNames,nameCount);
    structValues = scheme_make_struct_values(structType,structNames,nameCount,0);    
    *(srpStructs[i].pStructFuns) = structValues;
    srpStructs[i].nameCount = nameCount;
    structNameCount += nameCount;
  }

  srp_unit = (Scheme_Unit *)scheme_malloc(sizeof(Scheme_Unit));
  srp_unit->type = scheme_unit_type;
  srp_unit->num_imports = 0;
  srp_unit->num_exports = sizeray(srpPrims) + structNameCount;
  srp_unit->exports = (Scheme_Object **)
    scheme_malloc((sizeray(srpPrims) + structNameCount) * sizeof(Scheme_Object *));
  srp_unit->export_debug_names = NULL;
  srp_unit->init_func = srp_unit_init;
  
  for (i = 0; i < sizeray(srpPrims); i++) {
    srp_unit->exports[i] = scheme_intern_symbol(srpPrims[i].name);
  }

  for (i = 0,k = sizeray(srpPrims); i < sizeray(srpStructs); i++) {
    nameCount = srpStructs[i].nameCount;
    for (j = 0; j < nameCount; j++,k++) {
      srp_unit->exports[k] = scheme_intern_symbol(srpStructs[i].names[j]);
    }
  }

  puts("SisterPersist extension for MzScheme, "
       "Copyright (c) 1999 Rice PLT (Paul Steckler)");

  return (Scheme_Object *)srp_unit;
}

Scheme_Object *scheme_reload(Scheme_Env *env) {
  return (Scheme_Object *)srp_unit;
}
