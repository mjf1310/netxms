/* 
** NetXMS - Network Management System
** Windows Console
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
** $module: comm.cpp
** Background communication functions
**
**/

#include "stdafx.h"
#include "nxcon.h"


//
// Constants
//

#define UI_THREAD_WAIT_TIME   300


//
// Set status text in wait window
//

inline void SetInfoText(HWND hWnd, char *pszText)
{
   SendMessage(hWnd, WM_SET_INFO_TEXT, 0, (LPARAM)pszText);
}


//
// Check if MIB file is exist and up-to-date
//

static DWORD CheckMIBFile(char *pszName, BYTE *pHash)
{
   char szFileName[MAX_PATH];
   BYTE currHash[MD5_DIGEST_SIZE];
   BOOL bNeedUpdate = TRUE;
   DWORD dwResult = RCC_SUCCESS;

   // Build full file name
   strcpy(szFileName, g_szWorkDir);
   strcat(szFileName, WORKDIR_MIBCACHE);
   strcat(szFileName, "\\");
   strcat(szFileName, pszName);

   // Check file hash
   if (CalculateFileMD5Hash(szFileName, currHash))
      bNeedUpdate = memcmp(currHash, pHash, MD5_DIGEST_SIZE);

   // Download file from server if needed
   if (bNeedUpdate)
   {
      strcpy(szFileName, g_szWorkDir);
      strcat(szFileName, WORKDIR_MIBCACHE);
      dwResult = NXCDownloadMIBFile(g_hSession, pszName, szFileName);
   }
   return dwResult;
}


//
// Wrapper for client library event handler
//

static void ClientEventHandler(NXC_SESSION hSession, DWORD dwEvent, DWORD dwCode, void *pArg)
{
   theApp.EventHandler(dwEvent, dwCode, pArg);
}


//
// Login thread
//

static DWORD WINAPI LoginThread(void *pArg)
{
   HWND hWnd = *((HWND *)pArg);    // Handle to status window
   DWORD dwResult;

   dwResult = NXCConnect(g_szServer, g_szLogin, g_szPassword, &g_hSession);

   // If successful, load container objects' categories
   if (dwResult == RCC_SUCCESS)
   {
      NXCSetEventHandler(g_hSession, ClientEventHandler);

      SetInfoText(hWnd, "Loading container categories...");
      dwResult = NXCLoadCCList(g_hSession, &g_pCCList);
   }

   // Synchronize objects
   if (dwResult == RCC_SUCCESS)
   {
      SetInfoText(hWnd, "Synchronizing objects...");
      dwResult = NXCSyncObjects(g_hSession);
   }

   if (dwResult == RCC_SUCCESS)
   {
      SetInfoText(hWnd, "Loading user database...");
      dwResult = NXCLoadUserDB(g_hSession);
   }

   if (dwResult == RCC_SUCCESS)
   {
      SetInfoText(hWnd, "Loading action configuration...");
      dwResult = NXCLoadActions(g_hSession, &g_dwNumActions, &g_pActionList);
      if (dwResult == RCC_ACCESS_DENIED)
         dwResult = RCC_SUCCESS;    // User may not have rights to see actions, it's ok here
   }

   if (dwResult == RCC_SUCCESS)
   {
      NXC_MIB_LIST *pMibList;
      DWORD i;

      SetInfoText(hWnd, "Loading and initializing MIB files...");
      dwResult = NXCGetMIBList(g_hSession, &pMibList);
      if (dwResult == RCC_SUCCESS)
      {
         for(i = 0; i < pMibList->dwNumFiles; i++)
            if ((dwResult = CheckMIBFile(pMibList->ppszName[i], pMibList->ppHash[i])) != RCC_SUCCESS)
               break;
         NXCDestroyMIBList(pMibList);
         if (dwResult == RCC_SUCCESS)
            CreateMIBTree();
      }
   }

   if (dwResult == RCC_SUCCESS)
   {
      SetInfoText(hWnd, "Loading event information...");
      dwResult = NXCLoadEventDB(g_hSession);
   }

   // Synchronize images
   if (dwResult == RCC_SUCCESS)
   {
      char szCacheDir[MAX_PATH];

      SetInfoText(hWnd, "Synchronizing images...");
      strcpy(szCacheDir, g_szWorkDir);
      strcat(szCacheDir, WORKDIR_IMAGECACHE);
      dwResult = NXCSyncImages(g_hSession, &g_pSrvImageList, szCacheDir, IMAGE_FORMAT_ICO);
      if (dwResult == RCC_SUCCESS)
         CreateObjectImageList();
   }

   // Load default image list
   if (dwResult == RCC_SUCCESS)
   {
      DWORD i, *pdwClassId, *pdwImageId;

      SetInfoText(hWnd, "Loading default image list...");
      dwResult = NXCLoadDefaultImageList(g_hSession, &g_dwDefImgListSize, &pdwClassId, &pdwImageId);
      if (dwResult == RCC_SUCCESS)
      {
         g_pDefImgList = (DEF_IMG *)realloc(g_pDefImgList, sizeof(DEF_IMG) * g_dwDefImgListSize);
         for(i = 0; i < g_dwDefImgListSize; i++)
         {
            g_pDefImgList[i].dwObjectClass = pdwClassId[i];
            g_pDefImgList[i].dwImageId = pdwImageId[i];
            g_pDefImgList[i].iImageIndex = ImageIdToIndex(pdwImageId[i]);
         }
         safe_free(pdwClassId);
         safe_free(pdwImageId);
      }
   }

   // Disconnect if some of post-login operations was failed
   if (dwResult != RCC_SUCCESS)
   {
      NXCDisconnect(g_hSession);
      g_hSession = NULL;
   }

   PostMessage(hWnd, WM_REQUEST_COMPLETED, 0, dwResult);
   return dwResult;
}


//
// Perform login
//

DWORD DoLogin(void)
{
   HANDLE hThread;
   HWND hWnd = NULL;
   DWORD dwThreadId, dwResult;

   hThread = CreateThread(NULL, 0, LoginThread, &hWnd, CREATE_SUSPENDED, &dwThreadId);
   if (hThread != NULL)
   {
      CRequestProcessingDlg wndWaitDlg;

      wndWaitDlg.m_phWnd = &hWnd;
      wndWaitDlg.m_hThread = hThread;
      wndWaitDlg.m_strInfoText = "Connecting to server...";
      dwResult = (DWORD)wndWaitDlg.DoModal();
      CloseHandle(hThread);
   }
   else
   {
      dwResult = RCC_SYSTEM_FAILURE;
   }

   return dwResult;
}


//
// Request processing thread
//

static DWORD WINAPI RequestThread(void *pArg)
{
   RqData *pData = (RqData *)pArg;
   DWORD dwResult;

   switch(pData->dwNumParams)
   {
      case 0:
         dwResult = pData->pFunc();
         break;
      case 1:
         dwResult = pData->pFunc(pData->pArg1);
         break;
      case 2:
         dwResult = pData->pFunc(pData->pArg1, pData->pArg2);
         break;
      case 3:
         dwResult = pData->pFunc(pData->pArg1, pData->pArg2, pData->pArg3);
         break;
      case 4:
         dwResult = pData->pFunc(pData->pArg1, pData->pArg2, pData->pArg3, pData->pArg4);
         break;
      case 5:
         dwResult = pData->pFunc(pData->pArg1, pData->pArg2, pData->pArg3, 
                                 pData->pArg4, pData->pArg5);
         break;
      case 6:
         dwResult = pData->pFunc(pData->pArg1, pData->pArg2, pData->pArg3, 
                                 pData->pArg4, pData->pArg5, pData->pArg6);
         break;
      case 7:
         dwResult = pData->pFunc(pData->pArg1, pData->pArg2, pData->pArg3, 
                                 pData->pArg4, pData->pArg5, pData->pArg6,
                                 pData->pArg7);
         break;
   }
   if (pData->hWnd != NULL)
      PostMessage(pData->hWnd, WM_REQUEST_COMPLETED, 0, dwResult);
   return dwResult;
}


//
// Perform request (common code)
//

static DWORD ExecuteRequest(RqData *pData, char *pszInfoText)
{
   HANDLE hThread;
   DWORD dwThreadId, dwResult;

   hThread = CreateThread(NULL, 0, RequestThread, pData, 0, &dwThreadId);
   if (hThread != NULL)
   {
      CRequestProcessingDlg wndWaitDlg;

      // Wait for request completion
      if (WaitForSingleObject(hThread, UI_THREAD_WAIT_TIME) == WAIT_TIMEOUT)
      {
         // Thread still not finished, open status window
         SuspendThread(hThread);
         wndWaitDlg.m_phWnd = &pData->hWnd;
         wndWaitDlg.m_hThread = hThread;
         wndWaitDlg.m_strInfoText = pszInfoText;
         dwResult = (DWORD)wndWaitDlg.DoModal();
      }
      else
      {
         // Thread is finished, get it's exit code
         if (!GetExitCodeThread(hThread, &dwResult))
            dwResult = RCC_SYSTEM_FAILURE;
      }
      CloseHandle(hThread);
   }
   else
   {
      dwResult = RCC_SYSTEM_FAILURE;
   }

   return dwResult;
}


//
// Perform generic request without parameters
//

DWORD DoRequest(DWORD (* pFunc)(void), char *pszInfoText)
{
   RqData rqData;

   rqData.hWnd = NULL;
   rqData.dwNumParams = 0;
   rqData.pFunc = (DWORD (*)(...))pFunc;
   return ExecuteRequest(&rqData, pszInfoText);
}


//
// Perform request with 1 parameter
//

DWORD DoRequestArg1(void *pFunc, void *pArg1, char *pszInfoText)
{
   RqData rqData;

   rqData.hWnd = NULL;
   rqData.dwNumParams = 1;
   rqData.pArg1 = pArg1;
   rqData.pFunc = (DWORD (*)(...))pFunc;
   return ExecuteRequest(&rqData, pszInfoText);
}


//
// Perform request with 2 parameters
//

DWORD DoRequestArg2(void *pFunc, void *pArg1, void *pArg2, char *pszInfoText)
{
   RqData rqData;

   rqData.hWnd = NULL;
   rqData.dwNumParams = 2;
   rqData.pArg1 = pArg1;
   rqData.pArg2 = pArg2;
   rqData.pFunc = (DWORD (*)(...))pFunc;
   return ExecuteRequest(&rqData, pszInfoText);
}


//
// Perform request with 3 parameter
//

DWORD DoRequestArg3(void *pFunc, void *pArg1, void *pArg2, void *pArg3, char *pszInfoText)
{
   RqData rqData;

   rqData.hWnd = NULL;
   rqData.dwNumParams = 3;
   rqData.pArg1 = pArg1;
   rqData.pArg2 = pArg2;
   rqData.pArg3 = pArg3;
   rqData.pFunc = (DWORD (*)(...))pFunc;
   return ExecuteRequest(&rqData, pszInfoText);
}


//
// Perform request with 4 parameter
//

DWORD DoRequestArg4(void *pFunc, void *pArg1, void *pArg2, void *pArg3, 
                    void *pArg4, char *pszInfoText)
{
   RqData rqData;

   rqData.hWnd = NULL;
   rqData.dwNumParams = 4;
   rqData.pArg1 = pArg1;
   rqData.pArg2 = pArg2;
   rqData.pArg3 = pArg3;
   rqData.pArg4 = pArg4;
   rqData.pFunc = (DWORD (*)(...))pFunc;
   return ExecuteRequest(&rqData, pszInfoText);
}


//
// Perform request with 5 parameter
//

DWORD DoRequestArg5(void *pFunc, void *pArg1, void *pArg2, void *pArg3, void *pArg4, 
                    void *pArg5, char *pszInfoText)
{
   RqData rqData;

   rqData.hWnd = NULL;
   rqData.dwNumParams = 5;
   rqData.pArg1 = pArg1;
   rqData.pArg2 = pArg2;
   rqData.pArg3 = pArg3;
   rqData.pArg4 = pArg4;
   rqData.pArg5 = pArg5;
   rqData.pFunc = (DWORD (*)(...))pFunc;
   return ExecuteRequest(&rqData, pszInfoText);
}


//
// Perform request with 6 parameter
//

DWORD DoRequestArg6(void *pFunc, void *pArg1, void *pArg2, void *pArg3, void *pArg4, 
                    void *pArg5, void *pArg6, char *pszInfoText)
{
   RqData rqData;

   rqData.hWnd = NULL;
   rqData.dwNumParams = 6;
   rqData.pArg1 = pArg1;
   rqData.pArg2 = pArg2;
   rqData.pArg3 = pArg3;
   rqData.pArg4 = pArg4;
   rqData.pArg5 = pArg5;
   rqData.pArg6 = pArg6;
   rqData.pFunc = (DWORD (*)(...))pFunc;
   return ExecuteRequest(&rqData, pszInfoText);
}


//
// Perform request with 7 parameter
//

DWORD DoRequestArg7(void *pFunc, void *pArg1, void *pArg2, void *pArg3, void *pArg4, 
                    void *pArg5, void *pArg6, void *pArg7, char *pszInfoText)
{
   RqData rqData;

   rqData.hWnd = NULL;
   rqData.dwNumParams = 7;
   rqData.pArg1 = pArg1;
   rqData.pArg2 = pArg2;
   rqData.pArg3 = pArg3;
   rqData.pArg4 = pArg4;
   rqData.pArg5 = pArg5;
   rqData.pArg6 = pArg6;
   rqData.pArg7 = pArg7;
   rqData.pFunc = (DWORD (*)(...))pFunc;
   return ExecuteRequest(&rqData, pszInfoText);
}


//
// Callback function for node poller
//

static void PollerCallback(TCHAR *pszMsg, void *pArg)
{
   if (((RqData *)pArg)->hWnd != NULL)
      PostMessage(((RqData *)pArg)->hWnd, WM_POLLER_MESSAGE, 0, (LPARAM)_tcsdup(pszMsg));
}


//
// Poller thread
//

DWORD WINAPI PollerThread(void *pArg)
{
   RqData *pData = (RqData *)pArg;
   DWORD dwResult;

   dwResult = NXCPollNode(g_hSession, (DWORD)pData->pArg1, (int)pData->pArg2, 
                          PollerCallback, pArg);
   if (pData->hWnd != NULL)
      PostMessage(pData->hWnd, WM_REQUEST_COMPLETED, 0, dwResult);
   return dwResult;
}
