; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
#include "setup.iss"
OutputBaseFilename=nxagent-1.2.17

[Files]
Source: "..\..\..\Release\libnetxms.dll"; DestDir: "{app}\bin"; BeforeInstall: StopService; Flags: ignoreversion
Source: "..\..\..\Release\libnxlp.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\libnxdb.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\appagent.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\nxagentd.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\nxsagent.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\db2.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\dbquery.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\ecs.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\filemgr.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\informix.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\logwatch.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\netsvc.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\odbcquery.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\oracle.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\ping.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\portcheck.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\sms.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\ups.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\winnt.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\winperf.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\wmi.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\db2.ddr"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\informix.ddr"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\mssql.ddr"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\mysql.ddr"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\oracle.ddr"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\pgsql.ddr"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\contrib\nxagentd.conf-dist"; DestDir: "{app}\etc"; Flags: ignoreversion
Source: "..\..\..\release\libexpat.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\release\libtre.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\install\files\windows\x86\libeay32.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\install\files\windows\x86\ssleay32.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\install\files\windows\x86\libcurl.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\install\files\windows\x86\Microsoft.VC80.CRT\*"; DestDir: "{app}\bin\Microsoft.VC80.CRT"; Flags: ignoreversion
; Command-line tools
Source: "..\..\..\Release\nxappget.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\nxapush.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\nxevent.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\nxpush.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\libnxmap.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\..\..\Release\libnxcl.dll"; DestDir: "{app}\bin"; Flags: ignoreversion

#include "common.iss"

Function GetCustomCmdLine(Param: String): String;
Begin
  Result := '';
End;
