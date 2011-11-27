/* 
** NetXMS multiplatform core agent
** Copyright (C) 2003-2011 Victor Kirhenshtein
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
** File: master.cpp
**
**/

#include "nxagentd.h"


/**
 * Handler for CMD_GET_PARAMETER command
 */
static void H_GetParameter(CSCPMessage *pRequest, CSCPMessage *pMsg)
{
   TCHAR szParameter[MAX_PARAM_NAME], szValue[MAX_RESULT_LENGTH];
   DWORD dwErrorCode;

   pRequest->GetVariableStr(VID_PARAMETER, szParameter, MAX_PARAM_NAME);
   dwErrorCode = GetParameterValue(0, szParameter, szValue);
   pMsg->SetVariable(VID_RCC, dwErrorCode);
   if (dwErrorCode == ERR_SUCCESS)
      pMsg->SetVariable(VID_VALUE, szValue);
}

/**
 * Listener thread for master agent commands
 */
#ifdef _WIN32

THREAD_RESULT THREAD_CALL MasterAgentListener(void *arg)
{
	TCHAR pipeName[MAX_PATH], buffer[256];
	_sntprintf(pipeName, MAX_PATH, _T("\\\\.\\pipe\\nxagentd.subagent.%s"), g_masterAgent);

   while(!(g_dwFlags & AF_SHUTDOWN))
	{
		HANDLE hPipe = CreateFile(pipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hPipe != INVALID_HANDLE_VALUE)
		{
			DWORD pipeMode = PIPE_READMODE_MESSAGE;
			SetNamedPipeHandleState(hPipe, &pipeMode, NULL, NULL);
			AgentWriteDebugLog(1, _T("Connected to master agent"));

			while(!(g_dwFlags & AF_SHUTDOWN))
			{
				CSCPMessage *msg = ReadMessageFromPipe(hPipe, NULL);
				if ((msg == NULL) || (g_dwFlags & AF_SHUTDOWN))
					break;
				AgentWriteDebugLog(6, _T("Received message %s from master agent"), NXCPMessageCodeName(msg->GetCode(), buffer));
				
				CSCPMessage response;
				response.SetCode(CMD_REQUEST_COMPLETED);
				response.SetId(msg->GetId());
				switch(msg->GetCode())
				{
					case CMD_GET_PARAMETER:
						H_GetParameter(msg, &response);
						break;
					case CMD_GET_PARAMETER_LIST:
						response.SetVariable(VID_RCC, ERR_SUCCESS);
						GetParameterList(&response);
						break;
					default:
						response.SetVariable(VID_RCC, ERR_UNKNOWN_COMMAND);
						break;
				}
				delete msg;

				// Send response to pipe
				CSCP_MESSAGE *rawMsg = response.CreateMessage();
				DWORD bytes;
				if (!WriteFile(hPipe, rawMsg, ntohl(rawMsg->dwSize), &bytes, NULL))
					break;
				if (bytes != ntohl(rawMsg->dwSize))
					break;
			}
			CloseHandle(hPipe);
			AgentWriteDebugLog(1, _T("Disconnected from master agent"));
		}
		else
		{
			AgentWriteDebugLog(1, _T("Cannot connect to master agent, will retry in 5 seconds"));
			ThreadSleep(5);
		}
	}
	AgentWriteDebugLog(1, _T("Master agent listener stopped"));
	return THREAD_OK;
}

#else

THREAD_RESULT THREAD_CALL MasterAgentListener(void *arg)
{
	return THREAD_OK;
}

#endif
