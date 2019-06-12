/*
** NetXMS - Network Management System
** Copyright (C) 2003-2019 Raden Solutions
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published
** by the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** File: user_notification.cpp
**
**/

#include "libnxagent.h"

/**
 * User agent message constructor
 */
UserNotification::UserNotification(UINT64 serverId, const NXCPMessage *msg, UINT32 baseId) : m_id(serverId, msg->getFieldAsUInt32(baseId))
{
   m_message = msg->getFieldAsString(baseId + 1);
   m_startTime = msg->getFieldAsTime(baseId + 2);
   m_endTime = msg->getFieldAsTime(baseId + 3);
}

/**
 * User agent message constructor
 */
UserNotification::UserNotification(const NXCPMessage *msg, UINT32 baseId) : m_id(msg->getFieldAsUInt64(baseId + 9), msg->getFieldAsUInt32(baseId))
{
   m_message = msg->getFieldAsString(baseId + 1);
   m_startTime = msg->getFieldAsTime(baseId + 2);
   m_endTime = msg->getFieldAsTime(baseId + 3);
}

/**
 * User agent message constructor
 */
UserNotification::UserNotification(UINT64 serverId, UINT32 messageId, TCHAR *message, time_t start, time_t end) : m_id(serverId, messageId)
{
   m_message = message;
   m_startTime = start;
   m_endTime = end;
}

/**
 * User agent message destructor
 */
UserNotification::~UserNotification()
{
   MemFree(m_message);
}

/**
 * Prepare NXCP message with user agent message info
 */
void UserNotification::fillMessage(NXCPMessage *msg, UINT32 baseId)
{
   msg->setField(baseId, m_id.objectId);
   msg->setField(baseId + 2, m_message);
   msg->setFieldFromTime(baseId + 3, m_startTime);
   msg->setFieldFromTime(baseId + 4, m_endTime);
   msg->setField(baseId + 9, m_id.serverId);
}

void UserNotification::saveToDatabase(DB_HANDLE db)
{
   TCHAR query[2048];
   _sntprintf(query, 2048, _T("INSERT INTO user_agent_messages (server_id,message_id,message,start_time,end_time)")
         _T("VALUES (") INT64_FMT _T(",%d,%s,%d,%d)"), m_id.serverId, m_id.objectId, (const TCHAR *)DBPrepareString(db, m_message,1023),
         m_startTime, m_endTime);
   DBQuery(db, query);
}