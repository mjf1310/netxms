/*
** nxdbmgr - NetXMS database manager
** Copyright (C) 2020 Raden Solutions
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
** File: upgrade_v36.cpp
**
**/

#include "nxdbmgr.h"
#include <nxevent.h>

/**
 * Upgrade from 36.11 to 36.12
 */
static bool H_UpgradeFromV11()
{
   static const TCHAR *batch =
         _T("UPDATE event_cfg SET event_name='SYS_SNMP_TRAP_FLOOD_DETECTED' WHERE event_code=109\n")
         _T("UPDATE event_cfg SET event_name='SYS_SNMP_TRAP_FLOOD_ENDED' WHERE event_code=110\n")
         _T("<END>");
   CHK_EXEC(SQLBatch(batch));

   CHK_EXEC(CreateEventTemplate(EVENT_GEOLOCATION_CHANGED, _T("SYS_GEOLOCATION_CHANGED"),
            SEVERITY_NORMAL, EF_LOG, _T("2c2f40d4-91d3-441e-92d4-62a32e98a341"),
            _T("Device geolocation changed to %3 %4 (was %7 %8)"),
            _T("Generated when device geolocation change is detected.\r\n")
            _T("Parameters:\r\n")
            _T("   1) Latitude of new location in degrees\r\n")
            _T("   2) Longitude of new location in degrees\r\n")
            _T("   3) Latitude of new location in textual form\r\n")
            _T("   4) Longitude of new location in textual form\r\n")
            _T("   5) Latitude of previous location in degrees\r\n")
            _T("   6) Longitude of previous location in degrees\r\n")
            _T("   7) Latitude of previous location in textual form\r\n")
            _T("   8) Longitude of previous location in textual form")
            ));

   CHK_EXEC(CreateEventTemplate(EVENT_GEOLOCATION_INSIDE_RESTRICTED_AREA, _T("SYS_GEOLOCATION_INSIDE_RESTRICTED_AREA"),
            SEVERITY_WARNING, EF_LOG, _T("94d7a94b-a289-4cbe-8a83-a9c52b36c650"),
            _T("Device geolocation %3 %4 is within restricted area %5"),
            _T("Generated when new device geolocation is within restricted area.\r\n")
            _T("Parameters:\r\n")
            _T("   1) Latitude of new location in degrees\r\n")
            _T("   2) Longitude of new location in degrees\r\n")
            _T("   3) Latitude of new location in textual form\r\n")
            _T("   4) Longitude of new location in textual form\r\n")
            _T("   5) Area name\r\n")
            _T("   6) Area ID")
            ));

   CHK_EXEC(CreateEventTemplate(EVENT_GEOLOCATION_OUTSIDE_ALLOWED_AREA, _T("SYS_GEOLOCATION_OUTSIDE_ALLOWED_AREA"),
            SEVERITY_WARNING, EF_LOG, _T("24dcb315-a5cc-41f5-a813-b0a5f8c31e7d"),
            _T("Device geolocation %3 %4 is outside allowed area"),
            _T("Generated when new device geolocation is within restricted area.\r\n")
            _T("Parameters:\r\n")
            _T("   1) Latitude of new location in degrees\r\n")
            _T("   2) Longitude of new location in degrees\r\n")
            _T("   3) Latitude of new location in textual form\r\n")
            _T("   4) Longitude of new location in textual form")
            ));

   CHK_EXEC(CreateTable(
         _T("CREATE TABLE geo_areas (")
         _T("   id integer not null,")
         _T("   name varchar(127) not null,")
         _T("   comments $SQL:TEXT null,")
         _T("   configuration $SQL:TEXT null,")
         _T("   PRIMARY KEY(id))")));

   CHK_EXEC(SetMinorSchemaVersion(12));
   return true;
}

/**
 * Upgrade from 36.10 to 36.11
 */
static bool H_UpgradeFromV10()
{
   CHK_EXEC(SQLQuery(_T("ALTER TABLE nodes ADD snmp_engine_id varchar(255)")));
   CHK_EXEC(SetSchemaLevelForMajorVersion(36, 11));
   CHK_EXEC(SetMinorSchemaVersion(11));
   return true;
}

/**
 * Upgrade from 36.9 to 36.10
 */
static bool H_UpgradeFromV9()
{
   static const TCHAR *batch =
         _T("ALTER TABLE object_properties ADD category integer\n")
         _T("UPDATE object_properties SET category=0\n")
         _T("<END>");
   CHK_EXEC(SQLBatch(batch));
   CHK_EXEC(DBSetNotNullConstraint(g_dbHandle, _T("object_properties"), _T("category")));
   CHK_EXEC(DBRenameColumn(g_dbHandle, _T("object_properties"), _T("image"), _T("map_image")));

   CHK_EXEC(CreateTable(
         _T("CREATE TABLE object_categories (")
         _T("   id integer not null,")
         _T("   name varchar(63) not null,")
         _T("   icon varchar(36) null,")
         _T("   map_image varchar(36) null,")
         _T("   PRIMARY KEY(id))")));

   // Update access rights for predefined "Admins" group
   DB_RESULT hResult = SQLSelect(_T("SELECT system_access FROM user_groups WHERE id=1073741825"));
   if (hResult != nullptr)
   {
      if (DBGetNumRows(hResult) > 0)
      {
         uint64_t access = DBGetFieldUInt64(hResult, 0, 0);
         access |= SYSTEM_ACCESS_OBJECT_CATEGORIES;
         TCHAR query[256];
         _sntprintf(query, 256, _T("UPDATE user_groups SET system_access=") UINT64_FMT _T(" WHERE id=1073741825"), access);
         CHK_EXEC(SQLQuery(query));
      }
      DBFreeResult(hResult);
   }
   else
   {
      if (!g_ignoreErrors)
         return false;
   }

   CHK_EXEC(SetMinorSchemaVersion(10));
   return true;
}

/**
 * Upgrade from 36.8 to 36.9
 */
static bool H_UpgradeFromV8()
{
   static const TCHAR *batch =
         _T("UPDATE config SET var_name='Syslog.IgnoreMessageTimestamp' WHERE var_name='SyslogIgnoreMessageTimestamp'\n")
         _T("UPDATE config SET var_name='Syslog.ListenPort' WHERE var_name='SyslogListenPort'\n")
         _T("UPDATE config_values SET var_name='Syslog.ListenPort' WHERE var_name='SyslogListenPort'\n")
         _T("UPDATE config SET var_name='Syslog.NodeMatchingPolicy' WHERE var_name='SyslogNodeMatchingPolicy'\n")
         _T("UPDATE config_values SET var_name='Syslog.NodeMatchingPolicy' WHERE var_name='SyslogNodeMatchingPolicy'\n")
         _T("UPDATE config SET var_name='Syslog.RetentionTime' WHERE var_name='SyslogRetentionTime'\n")
         _T("UPDATE config SET var_name='Syslog.EnableListener' WHERE var_name='EnableSyslogReceiver'\n")
         _T("UPDATE config SET description='Retention time in days for stored syslog messages. All messages older than specified will be deleted by housekeeping process.' WHERE var_name='Syslog.RetentionTime'\n")
         _T("<END>");
   CHK_EXEC(SQLBatch(batch));
   CHK_EXEC(CreateConfigParam(_T("Syslog.EnableStorage"), _T("1"),
            _T("Enable/disable local storage of received syslog messages in NetXMS database."),
            nullptr, 'B', true, false, false, false));
   CHK_EXEC(SetMinorSchemaVersion(9));
   return true;
}

/**
 * Upgrade from 36.7 to 36.8
 */
static bool H_UpgradeFromV7()
{
   static const TCHAR *batch =
         _T("ALTER TABLE mobile_devices ADD speed varchar(20)\n")
         _T("ALTER TABLE mobile_devices ADD direction integer\n")
         _T("ALTER TABLE mobile_devices ADD altitude integer\n")
         _T("UPDATE mobile_devices SET speed='-1',direction=-1,altitude=0\n")
         _T("<END>");
   CHK_EXEC(SQLBatch(batch));
   CHK_EXEC(DBSetNotNullConstraint(g_dbHandle, _T("mobile_devices"), _T("speed")));
   CHK_EXEC(DBSetNotNullConstraint(g_dbHandle, _T("mobile_devices"), _T("direction")));
   CHK_EXEC(DBSetNotNullConstraint(g_dbHandle, _T("mobile_devices"), _T("altitude")));
   CHK_EXEC(SetMinorSchemaVersion(8));
   return true;
}

/**
 * Upgrade from 36.6 to 36.7
 */
static bool H_UpgradeFromV6()
{
   CHK_EXEC(SQLQuery(_T("ALTER TABLE mobile_devices ADD comm_protocol varchar(31)")));
   CHK_EXEC(SetMinorSchemaVersion(7));
   return true;
}

/**
 * Upgrade from 36.5 to 36.6
 */
static bool H_UpgradeFromV5()
{
   CHK_EXEC(CreateConfigParam(_T("WinEventLogRetentionTime"), _T("90"), _T("Retention time in days for records in windows event log. All records older than specified will be deleted by housekeeping process."), _T("days"), 'I', true, false, false, false));

   if (g_dbSyntax == DB_SYNTAX_TSDB)
   {
      CHK_EXEC(CreateTable(
            _T("CREATE TABLE win_event_log (")
            _T("  id $SQL:INT64 not null,")
            _T("  event_timestamp timestamptz not null,")
            _T("  node_id integer not null,")
            _T("  zone_uin integer not null,")
            _T("  origin_timestamp integer not null,")
            _T("  log_name varchar(63) null,")
            _T("  event_source varchar(127) null,")
            _T("  event_severity integer not null,")
            _T("  event_code integer not null,")
            _T("  message varchar(2000) null,")
            _T("  raw_data $SQL:TEXT null,")
            _T("PRIMARY KEY(id,event_timestamp))")));
      CHK_EXEC(SQLQuery(_T("SELECT create_hypertable('win_event_log', 'event_timestamp', chunk_time_interval => interval '86400 seconds')")));
   }
   else
   {
      CHK_EXEC(CreateTable(
            _T("CREATE TABLE win_event_log (")
            _T("  id $SQL:INT64 not null,")
            _T("  event_timestamp integer not null,")
            _T("  node_id integer not null,")
            _T("  zone_uin integer not null,")
            _T("  origin_timestamp integer not null,")
            _T("  log_name varchar(63) null,")
            _T("  event_source varchar(127) null,")
            _T("  event_severity integer not null,")
            _T("  event_code integer not null,")
            _T("  message varchar(2000) null,")
            _T("  raw_data $SQL:TEXT null,")
            _T("PRIMARY KEY(id))")));
   }
   CHK_EXEC(SQLQuery(_T("CREATE INDEX idx_win_event_log_timestamp ON win_event_log(event_timestamp)")));
   CHK_EXEC(SQLQuery(_T("CREATE INDEX idx_win_event_log_node ON win_event_log(node_id)")));

   CHK_EXEC(SetMinorSchemaVersion(6));
   return true;
}

/**
 * Upgrade from 36.4 to 36.5
 */
static bool H_UpgradeFromV4()
{
   if (GetSchemaLevelForMajorVersion(35) < 17)
   {
      CHK_EXEC(SQLQuery(_T("UPDATE config SET var_name='SNMP.Traps.LogAll',need_server_restart=0,description='Log all SNMP traps (even those received from addresses not belonging to any known node).' WHERE var_name='LogAllSNMPTraps'")));
      CHK_EXEC(SQLQuery(_T("UPDATE config SET var_name='SNMP.Traps.AllowVarbindsConversion',need_server_restart=0 WHERE var_name='AllowTrapVarbindsConversion'")));
      CHK_EXEC(SQLQuery(_T("UPDATE config SET need_server_restart=0 WHERE var_name='SNMP.Traps.ProcessUnmanagedNodes'")));
      CHK_EXEC(SetSchemaLevelForMajorVersion(35, 17));
   }
   CHK_EXEC(SetMinorSchemaVersion(5));
   return true;
}

/**
 * Upgrade from 36.3 to 36.4
 */
static bool H_UpgradeFromV3()
{
   CHK_EXEC(SQLQuery(_T("ALTER TABLE nodes ADD agent_cert_mapping_method char(1)")));
   CHK_EXEC(SQLQuery(_T("ALTER TABLE nodes ADD agent_cert_mapping_data varchar(500)")));
   CHK_EXEC(SetMinorSchemaVersion(4));
   return true;
}

/**
 * Upgrade from 36.2 to 36.3
 */
static bool H_UpgradeFromV2()
{
   CHK_EXEC(SQLQuery(_T("DROP TABLE certificates")));
   CHK_EXEC(SetMinorSchemaVersion(3));
   return true;
}

/**
 * Upgrade from 36.1 to 36.2
 */
static bool H_UpgradeFromV1()
{
   CHK_EXEC(CreateConfigParam(_T("AgentTunnels.TLS.MinVersion"), _T("2"), _T("Minimal version of TLS protocol used on agent tunnel connection."), nullptr, 'C', true, false, false, false));

   static const TCHAR *batch =
         _T("INSERT INTO config_values (var_name,var_value,var_description) VALUES ('AgentTunnels.TLS.MinVersion','0','1.0')\n")
         _T("INSERT INTO config_values (var_name,var_value,var_description) VALUES ('AgentTunnels.TLS.MinVersion','1','1.1')\n")
         _T("INSERT INTO config_values (var_name,var_value,var_description) VALUES ('AgentTunnels.TLS.MinVersion','2','1.2')\n")
         _T("INSERT INTO config_values (var_name,var_value,var_description) VALUES ('AgentTunnels.TLS.MinVersion','3','1.3')\n")
         _T("<END>");
   CHK_EXEC(SQLBatch(batch));

   CHK_EXEC(SetMinorSchemaVersion(2));
   return true;
}

/**
 * Upgrade from 36.0 to 36.1
 */
static bool H_UpgradeFromV0()
{
   CHK_EXEC(SQLQuery(_T("ALTER TABLE object_properties ADD name_on_map varchar(63)")));
   CHK_EXEC(SetMinorSchemaVersion(1));
   return true;
}

/**
 * Upgrade map
 */
static struct
{
   int version;
   int nextMajor;
   int nextMinor;
   bool (*upgradeProc)();
} s_dbUpgradeMap[] =
{
   { 11, 36, 12, H_UpgradeFromV11 },
   { 10, 36, 11, H_UpgradeFromV10 },
   { 9,  36, 10, H_UpgradeFromV9  },
   { 8,  36, 9,  H_UpgradeFromV8  },
   { 7,  36, 8,  H_UpgradeFromV7  },
   { 6,  36, 7,  H_UpgradeFromV6  },
   { 5,  36, 6,  H_UpgradeFromV5  },
   { 4,  36, 5,  H_UpgradeFromV4  },
   { 3,  36, 4,  H_UpgradeFromV3  },
   { 2,  36, 3,  H_UpgradeFromV2  },
   { 1,  36, 2,  H_UpgradeFromV1  },
   { 0,  36, 1,  H_UpgradeFromV0  },
   { 0,  0,  0,  nullptr          }
};

/**
 * Upgrade database to new version
 */
bool MajorSchemaUpgrade_V36()
{
   int32_t major, minor;
   if (!DBGetSchemaVersion(g_dbHandle, &major, &minor))
      return false;

   while((major == 36) && (minor < DB_SCHEMA_VERSION_V36_MINOR))
   {
      // Find upgrade procedure
      int i;
      for(i = 0; s_dbUpgradeMap[i].upgradeProc != nullptr; i++)
         if (s_dbUpgradeMap[i].version == minor)
            break;
      if (s_dbUpgradeMap[i].upgradeProc == nullptr)
      {
         _tprintf(_T("Unable to find upgrade procedure for version 36.%d\n"), minor);
         return false;
      }
      _tprintf(_T("Upgrading from version 36.%d to %d.%d\n"), minor, s_dbUpgradeMap[i].nextMajor, s_dbUpgradeMap[i].nextMinor);
      DBBegin(g_dbHandle);
      if (s_dbUpgradeMap[i].upgradeProc())
      {
         DBCommit(g_dbHandle);
         if (!DBGetSchemaVersion(g_dbHandle, &major, &minor))
            return false;
      }
      else
      {
         _tprintf(_T("Rolling back last stage due to upgrade errors...\n"));
         DBRollback(g_dbHandle);
         return false;
      }
   }
   return true;
}
