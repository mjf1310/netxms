/* 
** NetXMS - Network Management System
** Copyright (C) 2003, 2004, 2005 Victor Kirhenshtein
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
** $module: correlate.cpp
**
**/

#include "nxcore.h"


//
// Correlate SYS_SERVICE_DOWN event to possible SYS_NODE_DOWN
//

static void C_SysServiceDown(Event *pEvent)
{
   NetObj *pObject;

   pObject = FindObjectById(pEvent->SourceId());
   if ((pObject != NULL) && (pObject->Type() == OBJECT_NODE))
   {
      if (((Node *)pObject)->RuntimeFlags() & NDF_UNREACHEABLE)
      {
         pEvent->SetRootId(((Node *)pObject)->LastEventId(LAST_EVENT_NODE_DOWN));
      }
   }
}


//
// Correlate event
//

void CorrelateEvent(Event *pEvent)
{
   switch(pEvent->Code())
   {
      case EVENT_SERVICE_DOWN:
         C_SysServiceDown(pEvent);
         break;
      default:
         break;
   }
}
