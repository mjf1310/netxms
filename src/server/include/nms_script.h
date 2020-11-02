/*
** NetXMS - Network Management System
** Copyright (C) 2003-2016 Victor Kirhenshtein
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
** File: nms_script.h
**
**/

#ifndef _nms_script_h_
#define _nms_script_h_

/**
 * NXSL "NetObj" class
 */
class NXSL_NetObjClass : public NXSL_Class
{
public:
   NXSL_NetObjClass();

   virtual void onObjectDelete(NXSL_Object *object) override;

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
};

/**
 * NXSL "Subnet" class
 */
class NXSL_SubnetClass : public NXSL_NetObjClass
{
public:
   NXSL_SubnetClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
};

/**
 * NXSL "DataCollectionTarget" class
 */
class NXSL_DCTargetClass : public NXSL_NetObjClass
{
public:
   NXSL_DCTargetClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
};

/**
 * NXSL "Node" class
 */
class NXSL_NodeClass : public NXSL_DCTargetClass
{
public:
   NXSL_NodeClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
};

/**
 * NXSL "Interface" class
 */
class NXSL_InterfaceClass : public NXSL_NetObjClass
{
public:
   NXSL_InterfaceClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
};

/**
 * NXSL "AccessPoint" class
 */
class NXSL_AccessPointClass : public NXSL_DCTargetClass
{
public:
   NXSL_AccessPointClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
};

/**
 * NXSL "MobileDevice" class
 */
class NXSL_MobileDeviceClass : public NXSL_DCTargetClass
{
public:
   NXSL_MobileDeviceClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
};

/**
 * NXSL "Chassis" class
 */
class NXSL_ChassisClass : public NXSL_DCTargetClass
{
public:
   NXSL_ChassisClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
};

/**
 * NXSL "Cluster" class
 */
class NXSL_ClusterClass : public NXSL_DCTargetClass
{
public:
   NXSL_ClusterClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
};

/**
 * NXSL "Container" class
 */
class NXSL_ContainerClass : public NXSL_NetObjClass
{
public:
   NXSL_ContainerClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
};

/**
 * NXSL "Template" class
 */
class NXSL_TemplateClass : public NXSL_NetObjClass
{
public:
   NXSL_TemplateClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
};

/**
 * NXSL "Zone" class
 */
class NXSL_ZoneClass : public NXSL_NetObjClass
{
public:
   NXSL_ZoneClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
};

/**
 * NXSL "Sensor" class
 */
class NXSL_SensorClass : public NXSL_DCTargetClass
{
public:
   NXSL_SensorClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
};

/**
 * NXSL "Event" class
 */
class NXSL_EventClass : public NXSL_Class
{
public:
   NXSL_EventClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
   virtual bool setAttr(NXSL_Object *object, const char *attr, NXSL_Value *value) override;
   virtual void onObjectDelete(NXSL_Object *object) override;
};

/**
 * NXSL "Alarm" class
 */
class NXSL_AlarmClass : public NXSL_Class
{
public:
   NXSL_AlarmClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
   virtual void onObjectDelete(NXSL_Object *object) override;
};

/**
 * NXSL "AlarmComment" class
 */
class NXSL_AlarmCommentClass : public NXSL_Class
{
public:
   NXSL_AlarmCommentClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
   virtual void onObjectDelete(NXSL_Object *object) override;
};

/**
 * NXSL "DCI" class
 */
class NXSL_DciClass : public NXSL_Class
{
public:
   NXSL_DciClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
   virtual void onObjectDelete(NXSL_Object *object) override;
};

/**
 * NXSL "SNMP_Transport" class
 */
class NXSL_SNMPTransportClass : public NXSL_Class
{
public:
   NXSL_SNMPTransportClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
   virtual void onObjectDelete(NXSL_Object *object) override;
};

/**
 * "SNMP_VarBind" class
 */
class NXSL_SNMPVarBindClass : public NXSL_Class
{
public:
   NXSL_SNMPVarBindClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
   virtual void onObjectDelete(NXSL_Object *object) override;
};

/**
 * NXSL "NodeDependency" class
 */
class NXSL_NodeDependencyClass : public NXSL_Class
{
public:
   NXSL_NodeDependencyClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
   virtual void onObjectDelete(NXSL_Object *object) override;
};

/**
 * Server's default script environment
 */
class NXSL_ServerEnv : public NXSL_Environment
{
protected:
   CONSOLE_CTX m_console;

public:
   NXSL_ServerEnv();

   virtual void print(NXSL_Value *value) override;
   virtual void trace(int level, const TCHAR *text) override;

   virtual void configureVM(NXSL_VM *vm) override;
   virtual NXSL_Value *getConstantValue(const NXSL_Identifier& name, NXSL_ValueManager *vm) override;

   void setConsole(CONSOLE_CTX console) { m_console = console; }
};

/**
 * Script environment with output redirected to client session
 */
class NXSL_ClientSessionEnv : public NXSL_ServerEnv
{
protected:
   ClientSession *m_session;
   NXCPMessage *m_response;

public:
   NXSL_ClientSessionEnv(ClientSession *session, NXCPMessage *response);

   virtual void print(NXSL_Value *value) override;
   virtual void trace(int level, const TCHAR *text) override;
};

/**
 * NXSL "UserDBObject" class
 */
class NXSL_UserDBObjectClass : public NXSL_Class
{
public:
   NXSL_UserDBObjectClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
};

/**
 * NXSL "User" class
 */
class NXSL_UserClass : public NXSL_UserDBObjectClass
{
public:
   NXSL_UserClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
   virtual void onObjectDelete(NXSL_Object *object) override;
};

/**
 * NXSL "UserGroup" class
 */
class NXSL_UserGroupClass : public NXSL_UserDBObjectClass
{
public:
   NXSL_UserGroupClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
   virtual void onObjectDelete(NXSL_Object *object) override;
};

/**
 * NXSL "VLAN" class
 */
class NXSL_VlanClass : public NXSL_Class
{
public:
   NXSL_VlanClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
   virtual void onObjectDelete(NXSL_Object *object) override;
};

/**
 * NXSL class with tunnel information
 */
class NXSL_TunnelClass : public NXSL_Class
{
public:
   NXSL_TunnelClass();

   virtual NXSL_Value *getAttr(NXSL_Object *object, const char *attr) override;
   virtual void onObjectCreate(NXSL_Object *object) override;
   virtual void onObjectDelete(NXSL_Object *object) override;
};

class ScheduleParameters;

/**
 * Script VM creation failure reason
 */
enum class ScriptVMFailureReason
{
   SUCCESS,
   SCRIPT_NOT_FOUND,
   SCRIPT_IS_EMPTY,
   SCRIPT_LOAD_ERROR
};

/**
 * Script VM handle
 */
class NXCORE_EXPORTABLE ScriptVMHandle
{
private:
   NXSL_VM *m_vm;
   ScriptVMFailureReason m_failureReason;

public:
   ScriptVMHandle(NXSL_VM *vm) { m_vm = vm; m_failureReason = ScriptVMFailureReason::SUCCESS; }
   ScriptVMHandle(ScriptVMFailureReason failureReason) { m_vm = nullptr; m_failureReason = failureReason; }

   operator NXSL_VM *() { return m_vm; }
   NXSL_VM *operator->() { return m_vm; }
   NXSL_VM *vm() const { return m_vm; }
   ScriptVMFailureReason failureReason() const { return m_failureReason; }
   bool isValid() const { return m_vm != nullptr; }

   void destroy() { delete m_vm; }
};

/**
 * Get server script library
 */
NXSL_Library NXCORE_EXPORTABLE *GetServerScriptLibrary();

/**
 * Setup server script VM. Returns pointer to same VM for convenience.
 */
NXSL_VM NXCORE_EXPORTABLE *SetupServerScriptVM(NXSL_VM *vm, const shared_ptr<NetObj>& object, const shared_ptr<DCObjectInfo>& dciInfo);

/**
 * Create NXSL VM from library script
 */
ScriptVMHandle NXCORE_EXPORTABLE CreateServerScriptVM(const TCHAR *name, const shared_ptr<NetObj>& object, const shared_ptr<DCObjectInfo>& dciInfo = shared_ptr<DCObjectInfo>());

/**
 * Create NXSL VM from compiled script
 */
ScriptVMHandle NXCORE_EXPORTABLE CreateServerScriptVM(const NXSL_Program *script, const shared_ptr<NetObj>& object, const shared_ptr<DCObjectInfo>& dciInfo = shared_ptr<DCObjectInfo>());

/**
 * Functions
 */
void LoadScripts();
void ReloadScript(uint32_t scriptId);
bool IsValidScriptId(uint32_t id);
uint32_t ResolveScriptName(const TCHAR *name);
bool GetScriptName(uint32_t scriptId, TCHAR *buffer, size_t size);
void CreateScriptExportRecord(StringBuffer &xml, uint32_t id);
void ImportScript(ConfigEntry *config, bool overwrite);
NXSL_VM *FindHookScript(const TCHAR *hookName, shared_ptr<NetObj> object);
bool ParseValueList(NXSL_VM *vm, TCHAR **start, ObjectRefArray<NXSL_Value> &args, bool hasBrackets);

/**
 * Global variables
 */
extern NXSL_AccessPointClass g_nxslAccessPointClass;
extern NXSL_AlarmClass g_nxslAlarmClass;
extern NXSL_AlarmCommentClass g_nxslAlarmCommentClass;
extern NXSL_ChassisClass g_nxslChassisClass;
extern NXSL_ClusterClass g_nxslClusterClass;
extern NXSL_ContainerClass g_nxslContainerClass;
extern NXSL_DciClass g_nxslDciClass;
extern NXSL_EventClass g_nxslEventClass;
extern NXSL_InterfaceClass g_nxslInterfaceClass;
extern NXSL_MobileDeviceClass g_nxslMobileDeviceClass;
extern NXSL_NetObjClass g_nxslNetObjClass;
extern NXSL_NodeClass g_nxslNodeClass;
extern NXSL_NodeDependencyClass g_nxslNodeDependencyClass;
extern NXSL_SensorClass g_nxslSensorClass;
extern NXSL_SNMPTransportClass g_nxslSnmpTransportClass;
extern NXSL_SNMPVarBindClass g_nxslSnmpVarBindClass;
extern NXSL_SubnetClass g_nxslSubnetClass;
extern NXSL_TemplateClass g_nxslTemplateClass;
extern NXSL_TunnelClass g_nxslTunnelClass;
extern NXSL_UserDBObjectClass g_nxslUserDBObjectClass;
extern NXSL_UserClass g_nxslUserClass;
extern NXSL_UserGroupClass g_nxslUserGroupClass;
extern NXSL_VlanClass g_nxslVlanClass;
extern NXSL_ZoneClass g_nxslZoneClass;

#endif
