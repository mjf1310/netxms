/* 
** nxap - command line tool used to manage agent policies
** Copyright (C) 2010 Victor Kirhenshtein
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
** File: nxap.cpp
**
**/

#include <nms_common.h>
#include <nms_agent.h>
#include <nms_util.h>
#include <nxclapi.h>
#include <nxsrvapi.h>

#ifndef _WIN32
#include <netdb.h>
#endif


//
// Get policy list
//

static int GetPolicyInventory(AgentConnection &conn)
{
	AgentPolicyInfo *ap;
	DWORD rcc = conn.getPolicyInventory(&ap);
	if (rcc == ERR_SUCCESS)
	{
		char buffer[64];
		uuid_t guid;

		printf("GUID                                 Type Server\n"
		       "----------------------------------------------------------\n");
		for(int i = 0; i < ap->getSize(); i++)
		{
			ap->getGuid(i, guid);
			printf("%-16s %-4d %s\n", uuid_to_string(guid, buffer), ap->getType(i), ap->getServer(i));
		}
		delete ap;
	}
	else
	{
      printf("%d: %s\n", rcc, AgentErrorCodeToText(rcc));
	}
	return (rcc == ERR_SUCCESS) ? 0 : 1;
}


//
// Uninstall policy
//

static int UninstallPolicy(AgentConnection &conn, uuid_t guid)
{
	DWORD rcc = conn.uninstallPolicy(guid);
	if (rcc == ERR_SUCCESS)
	{
		printf("Policy successfully uninstalled from agent\n");
	}
	else
	{
      printf("%d: %s\n", rcc, AgentErrorCodeToText(rcc));
	}
	return (rcc == ERR_SUCCESS) ? 0 : 1;
}


//
// Startup
//

int main(int argc, char *argv[])
{
   char *eptr;
   BOOL bStart = TRUE, bVerbose = TRUE;
   int i, ch, iExitCode = 3, action = -1;
   int iAuthMethod = AUTH_NONE;
#ifdef _WITH_ENCRYPTION
   int iEncryptionPolicy = ENCRYPTION_ALLOWED;
#else
   int iEncryptionPolicy = ENCRYPTION_DISABLED;
#endif
   WORD wPort = AGENT_LISTEN_PORT;
   DWORD dwAddr, dwTimeout = 5000, dwConnTimeout = 30000, dwError;
   char szSecret[MAX_SECRET_LENGTH] = "";
   char szKeyFile[MAX_PATH] = DEFAULT_DATA_DIR DFILE_KEYS;
   RSA *pServerKey = NULL;
	uuid_t guid;

   InitThreadLibrary();

   // Parse command line
   opterr = 1;
	while((ch = getopt(argc, argv, "a:e:hK:lp:qs:u:vw:W:")) != -1)
   {
      switch(ch)
      {
         case 'h':   // Display help and exit
            printf("Usage: nxap [<options>] -l <host>\n"
				       "   or: nxap [<options>] -u <guid> <host>\n"
                   "Valid options are:\n"
                   "   -a <auth>    : Authentication method. Valid methods are \"none\",\n"
                   "                  \"plain\", \"md5\" and \"sha1\". Default is \"none\".\n"
#ifdef _WITH_ENCRYPTION
                   "   -e <policy>  : Set encryption policy. Possible values are:\n"
                   "                    0 = Encryption disabled;\n"
                   "                    1 = Encrypt connection only if agent requires encryption;\n"
                   "                    2 = Encrypt connection if agent supports encryption;\n"
                   "                    3 = Force encrypted connection;\n"
                   "                  Default value is 1.\n"
#endif
                   "   -h           : Display help and exit.\n"
#ifdef _WITH_ENCRYPTION
                   "   -K <file>    : Specify server's key file\n"
                   "                  (default is " DEFAULT_DATA_DIR DFILE_KEYS ").\n"
#endif
                   "   -p <port>    : Specify agent's port number. Default is %d.\n"
                   "   -q           : Quiet mode.\n"
                   "   -s <secret>  : Specify shared secret for authentication.\n"
                   "   -v           : Display version and exit.\n"
                   "   -w <seconds> : Set command timeout (default is 5 seconds)\n"
                   "   -W <seconds> : Set connection timeout (default is 30 seconds)\n"
                   "\n", wPort);
				bStart = FALSE;
            break;
         case 'a':   // Auth method
            if (!strcmp(optarg, "none"))
               iAuthMethod = AUTH_NONE;
            else if (!strcmp(optarg, "plain"))
               iAuthMethod = AUTH_PLAINTEXT;
            else if (!strcmp(optarg, "md5"))
               iAuthMethod = AUTH_MD5_HASH;
            else if (!strcmp(optarg, "sha1"))
               iAuthMethod = AUTH_SHA1_HASH;
            else
            {
               printf("Invalid authentication method \"%s\"\n", optarg);
               bStart = FALSE;
            }
            break;
         case 'p':   // Port number
            i = strtol(optarg, &eptr, 0);
            if ((*eptr != 0) || (i < 0) || (i > 65535))
            {
               printf("Invalid port number \"%s\"\n", optarg);
               bStart = FALSE;
            }
            else
            {
               wPort = (WORD)i;
            }
            break;
         case 'q':   // Quiet mode
            bVerbose = FALSE;
            break;
         case 's':   // Shared secret
            nx_strncpy(szSecret, optarg, MAX_SECRET_LENGTH - 1);
            break;
         case 'v':   // Print version and exit
            printf("NetXMS Agent Policy Management Tool Version " NETXMS_VERSION_STRING "\n");
            bStart = FALSE;
            break;
         case 'w':   // Command timeout
            i = strtol(optarg, &eptr, 0);
            if ((*eptr != 0) || (i < 1) || (i > 120))
            {
               printf("Invalid timeout \"%s\"\n", optarg);
               bStart = FALSE;
            }
            else
            {
               dwTimeout = (DWORD)i * 1000;  // Convert to milliseconds
            }
            break;
         case 'W':   // Connect timeout
            i = strtol(optarg, &eptr, 0);
            if ((*eptr != 0) || (i < 1) || (i > 120))
            {
               printf("Invalid timeout \"%s\"\n", optarg);
               bStart = FALSE;
            }
            else
            {
               dwConnTimeout = (DWORD)i * 1000;  // Convert to milliseconds
            }
            break;
#ifdef _WITH_ENCRYPTION
         case 'e':
            iEncryptionPolicy = atoi(optarg);
            if ((iEncryptionPolicy < 0) ||
                (iEncryptionPolicy > 3))
            {
               printf("Invalid encryption policy %d\n", iEncryptionPolicy);
               bStart = FALSE;
            }
            break;
         case 'K':
            nx_strncpy(szKeyFile, optarg, MAX_PATH);
            break;
#else
         case 'e':
         case 'K':
            printf("ERROR: This tool was compiled without encryption support\n");
            bStart = FALSE;
            break;
#endif
			case 'l':	// List policies on agent
				action = 0;
				break;
			case 'u':	// Uninstall policy from agent
				action = 1;
				uuid_parse(optarg, guid);
				break;
         case '?':
            bStart = FALSE;
            break;
         default:
            break;
      }
   }

   // Check parameter correctness
	if (action == -1)
	{
		printf("ERROR: You must specify either -l or -u option\n");
		bStart = FALSE;
	}

   if (bStart)
   {
      if (argc - optind < 1)
      {
         printf("Required argument(s) missing.\nUse nxap -h to get complete command line syntax.\n");
         bStart = FALSE;
      }
      else if ((iAuthMethod != AUTH_NONE) && (szSecret[0] == 0))
      {
         printf("Shared secret not specified or empty\n");
         bStart = FALSE;
      }

      // Load server key if requested
#ifdef _WITH_ENCRYPTION
      if ((iEncryptionPolicy != ENCRYPTION_DISABLED) && bStart)
      {
         if (InitCryptoLib(0xFFFF))
         {
            pServerKey = LoadRSAKeys(szKeyFile);
            if (pServerKey == NULL)
            {
               printf("Error loading RSA keys from \"%s\"\n", szKeyFile);
               if (iEncryptionPolicy == ENCRYPTION_REQUIRED)
                  bStart = FALSE;
            }
         }
         else
         {
            printf("Error initializing cryptografy module\n");
            if (iEncryptionPolicy == ENCRYPTION_REQUIRED)
               bStart = FALSE;
         }
      }
#endif

      // If everything is ok, start communications
      if (bStart)
      {
         struct hostent *hs;

         // Initialize WinSock
#ifdef _WIN32
         WSADATA wsaData;
         WSAStartup(2, &wsaData);
#endif

         // Resolve hostname
         hs = gethostbyname(argv[optind]);
         if (hs != NULL)
         {
            memcpy(&dwAddr, hs->h_addr, sizeof(DWORD));
         }
         else
         {
            dwAddr = inet_addr(argv[optind]);
         }
         if ((dwAddr == 0) || (dwAddr == INADDR_NONE))
         {
            fprintf(stderr, "Invalid host name or address specified\n");
         }
         else
         {
            AgentConnection conn(dwAddr, wPort, iAuthMethod, szSecret);

				conn.setConnectionTimeout(dwConnTimeout);
            conn.setCommandTimeout(dwTimeout);
            conn.setEncryptionPolicy(iEncryptionPolicy);
            if (conn.connect(pServerKey, bVerbose, &dwError))
            {
					if (action == 0)
					{
						iExitCode = GetPolicyInventory(conn);
					}
					else
					{
						iExitCode = UninstallPolicy(conn, guid);
					}
               conn.disconnect();
            }
            else
            {
               printf("%d: %s\n", dwError, AgentErrorCodeToText(dwError));
               iExitCode = 2;
            }
         }
      }
   }

   return iExitCode;
}
