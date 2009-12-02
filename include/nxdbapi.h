/* 
** NetXMS - Network Management System
** Server Library
** Copyright (C) 2003-2009 Victor Kirhenshtein
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** File: nxdbapi.h
**
**/

#ifndef _nxdbapi_h_
#define _nxdbapi_h_

#ifdef _WIN32
#ifdef LIBNXDB_EXPORTS
#define LIBNXDB_EXPORTABLE __declspec(dllexport)
#else
#define LIBNXDB_EXPORTABLE __declspec(dllimport)
#endif
#else    /* _WIN32 */
#define LIBNXDB_EXPORTABLE
#endif

#include <dbdrv.h>
#include <uuid.h>


//
// DB-related constants
//

#define MAX_DB_LOGIN          64
#define MAX_DB_PASSWORD       64
#define MAX_DB_NAME           256


//
// DB events
//

#define DBEVENT_CONNECTION_LOST        0
#define DBEVENT_CONNECTION_RESTORED    1
#define DBEVENT_QUERY_FAILED           2


//
// Database syntax codes
//

#define DB_SYNTAX_MYSQL    0
#define DB_SYNTAX_PGSQL    1
#define DB_SYNTAX_MSSQL    2
#define DB_SYNTAX_ORACLE   3
#define DB_SYNTAX_SQLITE   4
#define DB_SYNTAX_UNKNOWN	-1


//
// Database connection structure
//

struct db_handle_t
{
   DB_CONNECTION hConn;
   MUTEX mutexTransLock;      // Transaction lock
   int nTransactionLevel;
   TCHAR *pszServer;
   TCHAR *pszLogin;
   TCHAR *pszPassword;
   TCHAR *pszDBName;
};
typedef db_handle_t * DB_HANDLE;


//
// Functions
//

BOOL LIBNXDB_EXPORTABLE DBInit(DWORD logMsgCode, DWORD sqlErrorMsgCode, BOOL bDumpSQL,
                                void (* fpEventHandler)(DWORD, const TCHAR *, const TCHAR *));
DB_HANDLE LIBNXDB_EXPORTABLE DBConnect(void);
DB_HANDLE LIBNXDB_EXPORTABLE DBConnectEx(const TCHAR *pszServer, const TCHAR *pszDBName,
                                          const TCHAR *pszLogin, const TCHAR *pszPassword);
void LIBNXDB_EXPORTABLE DBDisconnect(DB_HANDLE hConn);
BOOL LIBNXDB_EXPORTABLE DBQuery(DB_HANDLE hConn, const TCHAR *szQuery);
BOOL LIBNXDB_EXPORTABLE DBQueryEx(DB_HANDLE hConn, const TCHAR *szQuery, TCHAR *errorText);
DB_RESULT LIBNXDB_EXPORTABLE DBSelect(DB_HANDLE hConn, const TCHAR *szQuery);
DB_RESULT LIBNXDB_EXPORTABLE DBSelectEx(DB_HANDLE hConn, const TCHAR *szQuery, TCHAR *errorText);
DB_ASYNC_RESULT LIBNXDB_EXPORTABLE DBAsyncSelect(DB_HANDLE hConn, const TCHAR *szQuery);
DB_ASYNC_RESULT LIBNXDB_EXPORTABLE DBAsyncSelectEx(DB_HANDLE hConn, const TCHAR *szQuery, TCHAR *errorText);
BOOL LIBNXDB_EXPORTABLE DBFetch(DB_ASYNC_RESULT hResult);
int LIBNXDB_EXPORTABLE DBGetColumnCount(DB_RESULT hResult);
BOOL LIBNXDB_EXPORTABLE DBGetColumnName(DB_RESULT hResult, int column, TCHAR *buffer, int bufSize);
int LIBNXDB_EXPORTABLE DBGetColumnCountAsync(DB_ASYNC_RESULT hResult);
BOOL LIBNXDB_EXPORTABLE DBGetColumnNameAsync(DB_ASYNC_RESULT hResult, int column, TCHAR *buffer, int bufSize);
TCHAR LIBNXDB_EXPORTABLE *DBGetField(DB_RESULT hResult, int iRow, int iColumn,
                                      TCHAR *pszBuffer, int nBufLen);
LONG LIBNXDB_EXPORTABLE DBGetFieldLong(DB_RESULT hResult, int iRow, int iColumn);
DWORD LIBNXDB_EXPORTABLE DBGetFieldULong(DB_RESULT hResult, int iRow, int iColumn);
INT64 LIBNXDB_EXPORTABLE DBGetFieldInt64(DB_RESULT hResult, int iRow, int iColumn);
QWORD LIBNXDB_EXPORTABLE DBGetFieldUInt64(DB_RESULT hResult, int iRow, int iColumn);
double LIBNXDB_EXPORTABLE DBGetFieldDouble(DB_RESULT hResult, int iRow, int iColumn);
DWORD LIBNXDB_EXPORTABLE DBGetFieldIPAddr(DB_RESULT hResult, int iRow, int iColumn);
BOOL LIBNXDB_EXPORTABLE DBGetFieldByteArray(DB_RESULT hResult, int iRow, int iColumn,
                                             int *pnArray, int nSize, int nDefault);
BOOL LIBNXDB_EXPORTABLE DBGetFieldGUID(DB_RESULT hResult, int iRow,
                                        int iColumn, uuid_t guid);
TCHAR LIBNXDB_EXPORTABLE *DBGetFieldAsync(DB_ASYNC_RESULT hResult, int iColumn, TCHAR *pBuffer, int iBufSize);
LONG LIBNXDB_EXPORTABLE DBGetFieldAsyncLong(DB_RESULT hResult, int iColumn);
DWORD LIBNXDB_EXPORTABLE DBGetFieldAsyncULong(DB_ASYNC_RESULT hResult, int iColumn);
INT64 LIBNXDB_EXPORTABLE DBGetFieldAsyncInt64(DB_RESULT hResult, int iColumn);
QWORD LIBNXDB_EXPORTABLE DBGetFieldAsyncUInt64(DB_ASYNC_RESULT hResult, int iColumn);
double LIBNXDB_EXPORTABLE DBGetFieldAsyncDouble(DB_RESULT hResult, int iColumn);
DWORD LIBNXDB_EXPORTABLE DBGetFieldAsyncIPAddr(DB_RESULT hResult, int iColumn);
int LIBNXDB_EXPORTABLE DBGetNumRows(DB_RESULT hResult);
void LIBNXDB_EXPORTABLE DBFreeResult(DB_RESULT hResult);
void LIBNXDB_EXPORTABLE DBFreeAsyncResult(DB_HANDLE hConn, DB_ASYNC_RESULT hResult);
BOOL LIBNXDB_EXPORTABLE DBBegin(DB_HANDLE hConn);
BOOL LIBNXDB_EXPORTABLE DBCommit(DB_HANDLE hConn);
BOOL LIBNXDB_EXPORTABLE DBRollback(DB_HANDLE hConn);
void LIBNXDB_EXPORTABLE DBUnloadDriver(void);

int LIBNXDB_EXPORTABLE DBGetSchemaVersion(DB_HANDLE conn);
int LIBNXDB_EXPORTABLE DBGetSyntax(DB_HANDLE conn);

String LIBNXDB_EXPORTABLE DBPrepareString(const TCHAR *str, int maxSize = -1);
TCHAR LIBNXDB_EXPORTABLE *EncodeSQLString(const TCHAR *pszIn);
void LIBNXDB_EXPORTABLE DecodeSQLString(TCHAR *pszStr);

bool LIBNXDB_EXPORTABLE DBConnectionPoolStartup(int basePoolSize, int maxPoolSize, int cooldownTime);
void LIBNXDB_EXPORTABLE DBConnectionPoolShutdown();
DB_HANDLE LIBNXDB_EXPORTABLE DBConnectionPoolAcquireConnection();
void LIBNXDB_EXPORTABLE DBConnectionPoolReleaseConnection(DB_HANDLE connection);

void LIBNXDB_EXPORTABLE DBSetDebugPrintCallback(void (*cb)(int, const TCHAR *, va_list));


//
// Variables
//

extern TCHAR LIBNXDB_EXPORTABLE g_szDbDriver[];
extern TCHAR LIBNXDB_EXPORTABLE g_szDbDrvParams[];
extern TCHAR LIBNXDB_EXPORTABLE g_szDbServer[];
extern TCHAR LIBNXDB_EXPORTABLE g_szDbLogin[];
extern TCHAR LIBNXDB_EXPORTABLE g_szDbPassword[];
extern TCHAR LIBNXDB_EXPORTABLE g_szDbName[];

#endif   /* _nxsrvapi_h_ */
