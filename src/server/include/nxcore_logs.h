/* 
** NetXMS - Network Management System
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
** File: nxcore_logs.h
**
**/

#ifndef _nxcore_logs_h_
#define _nxcore_logs_h_


//
// Column types
//

#define LC_TEXT            0
#define LC_SEVERITY        1
#define LC_OBJECT_ID       2
#define LC_USER_ID         3
#define LC_EVENT_CODE      4
#define LC_TIMESTAMP       5


//
// Log definition structure
//

struct LOG_COLUMN
{
	const TCHAR *name;
	const TCHAR *description;
	int type;
};

struct NXCORE_LOG
{
	const TCHAR *name;
	const TCHAR *table;
	DWORD requiredAccess;
	LOG_COLUMN columns[32];
};


//
// Log filter
//

class ColumnFilter
{
private:
	int m_type;

public:
	ColumnFilter();
	~ColumnFilter();
};

class LogFilter
{
private:
	int m_numColumnFilters;
	ColumnFilter *m_columnFilters;

public:
	LogFilter(CSCPMessage *msg);
	~LogFilter();
};


//
// Log handle - object used to access log
//

class LogHandle
{
private:
	NXCORE_LOG *m_log;
	bool m_isDataReady;
	TCHAR m_tempTable[64];
	INT64 m_rowCount;
	LogFilter *m_filter;
	MUTEX m_lock;

	void deleteQueryResults();

public:
	LogHandle(NXCORE_LOG *log);
	~LogHandle();

	void lock() { MutexLock(m_lock, INFINITE); }
	void unlock() { MutexUnlock(m_lock); }

	bool query(LogFilter *filter);
	Table *getData(INT64 startRow, INT64 numRows);
};


//
// API functions
//

void InitLogAccess();
int OpenLog(const TCHAR *name, ClientSession *session, DWORD *rcc);
DWORD CloseLog(ClientSession *session, int logHandle);
LogHandle *AcquireLogHandleObject(ClientSession *session, int logHandle);


#endif
