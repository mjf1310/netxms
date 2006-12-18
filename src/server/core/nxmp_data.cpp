/* $Id: nxmp_data.cpp,v 1.3 2006-12-18 00:21:10 victor Exp $ */
/* 
** NetXMS - Network Management System
** Copyright (C) 2003, 2004, 2005, 2006 Victor Kirhenshtein
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
** File: nxmp_data.cpp
**
**/

#include "nxcore.h"
#include "nxmp_parser.h"


//
// Get string valus as unsigned int
//

static BOOL GetValueAsUInt(char *pszStr, DWORD *pdwValue)
{
   char *eptr;
   int nLen;
   
   nLen = strlen(pszStr);
   if (nLen == 0)
      return FALSE;

   if (pszStr[nLen - 1] == 'U')
   {
      pszStr[nLen - 1] = 0;
      *pdwValue = strtoul(pszStr, &eptr, 0);
   }
   else
   {
      LONG nValue;

      nValue = strtol(pszStr, &eptr, 0);
      memcpy(pdwValue, &nValue, sizeof(DWORD));
   }
   return (*eptr == 0);
}


//
// Constructor
//

NXMP_Data::NXMP_Data(NXMP_Lexer *pLexer, NXMP_Parser *pParser)
{
   m_pEventList = NULL;
   m_dwNumEvents = 0;
   m_pCurrEvent = NULL;
   m_nContext = CTX_NONE;
   m_pLexer = pLexer;
   m_pParser = pParser;
}


//
// Destructor
//

NXMP_Data::~NXMP_Data()
{
   DWORD i;

   for(i = 0; i < m_dwNumEvents; i++)
   {
      safe_free(m_pEventList[i].pszMessageTemplate);
      safe_free(m_pEventList[i].pszDescription);
   }
   safe_free(m_pEventList);
}


//
// Report error
//

void NXMP_Data::Error(const char *pszMsg, ...)
{
   va_list args;
   char szBuffer[1024];

   va_start(args, pszMsg);
   vsnprintf(szBuffer, 1024, pszMsg, args);
   va_end(args);
   m_pParser->Error(szBuffer);
   m_pLexer->SetErrorState();
}


//
// Parse variable
//

BOOL NXMP_Data::ParseVariable(char *pszName, char *pszValue)
{
   BOOL bRet = FALSE;
   DWORD dwValue;

   switch(m_nContext)
   {
      case CTX_EVENT:
         if (!stricmp(pszName, "CODE"))
         {
            if (GetValueAsUInt(pszValue, &dwValue))
            {
               SetEventCode(dwValue);
            }
            else
            {
               Error("Event code must be a numeric value");
            }
         }
         else if (!stricmp(pszName, "SEVERITY"))
         {
            if (GetValueAsUInt(pszValue, &dwValue))
            {
               SetEventSeverity(dwValue);
            }
            else
            {
               Error("Event severity must be a numeric value");
            }
         }
         else if (!stricmp(pszName, "FLAGS"))
         {
            if (GetValueAsUInt(pszValue, &dwValue))
            {
               SetEventFlags(dwValue);
            }
            else
            {
               Error("Event flags must be a numeric value");
            }
         }
         else if (!stricmp(pszName, "TEXT"))
         {
            SetEventText(pszValue);
         }
         else if (!stricmp(pszName, "DESCRIPTION"))
         {
            SetEventDescription(pszValue);
         }
         else
         {
            Error("Unknown event attribute %s", pszName);
         }
         break;
      default:
         break;
   }
   return bRet;
}


//
// Create new event
//

void NXMP_Data::NewEvent(char *pszName)
{
   m_dwNumEvents++;
   m_pEventList = (EVENT_TEMPLATE *)realloc(m_pEventList, sizeof(EVENT_TEMPLATE) * m_dwNumEvents);
   m_pCurrEvent = &m_pEventList[m_dwNumEvents - 1];
   memset(m_pCurrEvent, 0, sizeof(EVENT_TEMPLATE));
#ifdef UNICODE
   MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszName, -1, m_pCurrEvent->szName, MAX_EVENT_NAME - 1);
#else
   nx_strncpy(m_pCurrEvent->szName, pszName, MAX_EVENT_NAME);
#endif
   m_nContext = CTX_EVENT;
}


//
// Set event's text
//

void NXMP_Data::SetEventText(char *pszText)
{
   int nLen;

   if (m_pCurrEvent == NULL)
      return;

   nLen = (int)strlen(pszText) + 1;
   m_pCurrEvent->pszMessageTemplate = (TCHAR *)realloc(m_pCurrEvent->pszMessageTemplate, nLen * sizeof(TCHAR));
#ifdef UNICODE
   MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszText, -1, m_pCurrEvent->pszMessageTemplate, nLen);
#else
   strcpy(m_pCurrEvent->pszMessageTemplate, pszText);
#endif
}


//
// Set event's description
//

void NXMP_Data::SetEventDescription(char *pszText)
{
   int nLen;

   if (m_pCurrEvent == NULL)
      return;

   nLen = (int)strlen(pszText) + 1;
   m_pCurrEvent->pszDescription = (TCHAR *)realloc(m_pCurrEvent->pszDescription, nLen * sizeof(TCHAR));
#ifdef UNICODE
   MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszText, -1, m_pCurrEvent->pszDescription, nLen);
#else
   strcpy(m_pCurrEvent->pszDescription, pszText);
#endif
}
