/* srptypes.c */

#include "stdafx.h"

#ifdef WIN32
#include <windows.h>
#else
#define FALSE (0)
#define TRUE  (1)
typedef bool BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
/* dummy typedefs -- only used in trace API, not ODBC as such */
typedef void VOID; 
typedef int CHAR; 
typedef void *LPWSTR; 
typedef int WCHAR; 
typedef int GUID; 
#endif

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#include "escheme.h"

#include "srptypes.h"
#include "srpersist.h"

Scheme_Type sql_date_type;
Scheme_Type sql_decimal_type;
Scheme_Type sql_pointer_type;
Scheme_Type sql_time_type;
Scheme_Type sql_timestamp_type;
Scheme_Type sql_return_type;
Scheme_Type sql_handle_type;
Scheme_Type sql_henv_type;
Scheme_Type sql_hdbc_type;
Scheme_Type sql_hstmt_type;
Scheme_Type sql_hdesc_type;
Scheme_Type sql_boxed_uint_type;
Scheme_Type sql_buffer_type;
Scheme_Type sql_length_type;
Scheme_Type sql_indicator_type;
Scheme_Type sql_row_status_type;
Scheme_Type sql_array_status_type;
Scheme_Type sql_binding_offset_type;
Scheme_Type sql_rows_processed_type;
Scheme_Type sql_octet_length_type;
Scheme_Type sql_op_parms_type;
Scheme_Type sql_guid_type;
Scheme_Type sql_paramlength_type;


