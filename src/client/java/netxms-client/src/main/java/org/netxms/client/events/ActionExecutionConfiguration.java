/**
 * NetXMS - open source network management system
 * Copyright (C) 2013-2018 Victor Kirhenshtein
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
package org.netxms.client.events;

import org.netxms.base.NXCPMessage;

/**
 * Action execution data
 */
public class ActionExecutionConfiguration
{
   private long actionId;
   private int timerDelay;
   private String timerKey;
   private String blockingTimerKey;
   
   /**
    * Create new action execution configuration without timer delay and key.
    * 
    * @param actionId action ID
    */
   ActionExecutionConfiguration(long actionId)
   {
      this.actionId = actionId;
      this.timerDelay = 0;
      this.timerKey = null;
      this.blockingTimerKey = null;
   }
   
   /**
    * Create new action execution configuration
    * 
    * @param actionId action ID
    * @param timerDelay timer delay in seconds
    * @param timerKey timer key
    * @param blockingTimerKey timer key for blocking the action
    */
   public ActionExecutionConfiguration(long actionId, int timerDelay, String timerKey, String blockingTimerKey)
   {
      this.actionId = actionId;
      this.timerDelay = timerDelay;
      this.timerKey = timerKey;
      this.blockingTimerKey = blockingTimerKey;
   }

   /**
    * Create action execution configuration from NXCP message 
    * 
    * @param msg NXCP message
    * @param baseId base field ID
    */
   protected ActionExecutionConfiguration(NXCPMessage msg, long baseId)
   {
      actionId = msg.getFieldAsInt64(baseId);
      timerDelay = msg.getFieldAsInt32(baseId + 1);
      timerKey = msg.getFieldAsString(baseId + 2);
      blockingTimerKey = msg.getFieldAsString(baseId + 3);
   }

   /**
    * Copy constructor
    * 
    * @param src source object
    */
   public ActionExecutionConfiguration(ActionExecutionConfiguration src)
   {
      this.actionId = src.actionId;
      this.timerDelay = src.timerDelay;
      this.timerKey = src.timerKey;
      this.blockingTimerKey = src.blockingTimerKey;
   }

   /**
    * Fill NXCP message
    * 
    * @param msg NXCP message
    * @param baseId base field ID
    */
   public void fillMessage(NXCPMessage msg, long baseId)
   {
      msg.setFieldInt32(baseId, (int)actionId);
      msg.setFieldInt32(baseId + 1, timerDelay);
      msg.setField(baseId + 2, timerKey);
      msg.setField(baseId + 3, blockingTimerKey);
   }
   
   /**
    * Get timer delay.
    * 
    * @return timer delay in seconds
    */
   public int getTimerDelay()
   {
      return timerDelay;
   }

   /**
    * Set timer delay.
    * 
    * @param timerDelay timer delay in seconds
    */
   public void setTimerDelay(int timerDelay)
   {
      this.timerDelay = timerDelay;
   }

   /**
    * Get timer key
    * 
    * @return current timer key
    */
   public String getTimerKey()
   {
      return (timerKey != null) ? timerKey : "";
   }

   /**
    * Set timer key
    * 
    * @param timerKey new timer key
    */
   public void setTimerKey(String timerKey)
   {
      this.timerKey = timerKey;
   }

   /**
    * Get action ID
    * 
    * @return action ID
    */
   public long getActionId()
   {
      return actionId;
   }

   /**
    * Get blocking timer key.
    *
    * @return blocking timer key or empty string if not set
    */
   public String getBlockingTimerKey()
   {
      return (blockingTimerKey != null) ? blockingTimerKey : "";
   }

   /**
    * Set blocking timer key
    *
    * @param blockingTimerKey blocking timer key
    */
   public void setBlockingTimerKey(String blockingTimerKey)
   {
      this.blockingTimerKey = blockingTimerKey;
   }
}
