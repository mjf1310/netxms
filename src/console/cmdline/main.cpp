/* 
** NetXMS - Network Management System
** Command line client
** Copyright (C) 2004 Victor Kirhenshtein
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
** $module: main.cpp
**
**/

#include "nxcmd.h"


//
// Global variables
//

DWORD g_dwState = STATE_DISCONNECTED;
CONDITION g_hCondOperationComplete;


//
// Callback function for debug printing
//

static void DebugCallback(char *pMsg)
{
   printf("*debug* %s\n", pMsg);
}


//
// Event handler
//

static void EventHandler(DWORD dwEvent, DWORD dwCode, void *pArg)
{
   switch(dwEvent)
   {
      case NXC_EVENT_STATE_CHANGED:
         g_dwState = dwCode;
         switch(dwCode)
         {
            case STATE_IDLE:
            case STATE_DISCONNECTED:
               ConditionSet(g_hCondOperationComplete);
               break;
            default:
               break;
         }
         break;
      default:
         printf("Event %d [Code = %d, Arg = 0x%08X]\n", dwEvent, dwCode, pArg);
         break;
   }
}


//
// main()
//

int main(int argc, char *argv[])
{
   DWORD dwVersion;
   char szServer[256], szLogin[256], szPassword[256];

#ifdef _WIN32
   WSADATA wsaData;

   WSAStartup(0x0002, &wsaData);
#endif

   g_hCondOperationComplete = ConditionCreate();

   dwVersion = NXCGetVersion();
   printf("Using NetXMS Client Library version %d.%d.%d\n", dwVersion >> 24,
          (dwVersion >> 16) & 255, dwVersion & 0xFFFF);
   NXCInitialize();
   NXCSetEventHandler(EventHandler);
   NXCSetDebugCallback(DebugCallback);

strcpy(szServer,"eagle");
strcpy(szLogin,"admin");
szPassword[0]=0;

   printf("Connecting to server %s as user %s ...\n", szServer, szLogin);
   NXCConnect(szServer, szLogin, szPassword);
   ConditionWait(g_hCondOperationComplete, INFINITE);
   if (g_dwState == STATE_DISCONNECTED)
   {
      printf("Unable to establish connection with the server.\n");
      return 1;
   }
   printf("Connection established.\n");

   // Load oblects from server
   NXCSyncObjects();
   ConditionWait(g_hCondOperationComplete, INFINITE);
   printf("Objects synchronized.\n");

   return 0;
}
