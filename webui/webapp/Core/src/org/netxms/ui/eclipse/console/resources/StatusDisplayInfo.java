/**
 * NetXMS - open source network management system
 * Copyright (C) 2003-2014 Victor Kirhenshtein
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
package org.netxms.ui.eclipse.console.resources;

import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.preference.PreferenceConverter;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Display;
import org.netxms.client.constants.ObjectStatus;
import org.netxms.client.constants.Severity;
import org.netxms.ui.eclipse.console.Activator;
import org.netxms.ui.eclipse.console.Messages;
import org.netxms.ui.eclipse.shared.ConsoleSharedData;
import org.netxms.ui.eclipse.tools.ColorCache;

/**
 * Status display information
 */
public final class StatusDisplayInfo
{
   private String[] statusText = new String[9];
   private ImageDescriptor[] statusImageDesc = new ImageDescriptor[9];
   private Image[] statusImage = new Image[9];
   private ColorCache colorCache;
   private Color statusColor[] = new Color[9]; 
   
   /**
    * Get status display instance for current display
    * 
    * @return
    */
   private static StatusDisplayInfo getInstance()
   {
      StatusDisplayInfo instance = (StatusDisplayInfo)ConsoleSharedData.getProperty("StatusDisplayInfo");
      if (instance == null)
      {
         instance = new StatusDisplayInfo(Display.getCurrent());
         ConsoleSharedData.setProperty("StatusDisplayInfo", instance);
      }
      return instance;
   }
   
	/**
	 * Private constructor to create per-display instance
	 * 
	 * @param display
	 */
	private StatusDisplayInfo(Display display)
	{
		statusText[ObjectStatus.NORMAL.getValue()] = Messages.get(display).StatusDisplayInfo_Normal;
		statusText[ObjectStatus.WARNING.getValue()] = Messages.get(display).StatusDisplayInfo_Warning;
		statusText[ObjectStatus.MINOR.getValue()] = Messages.get(display).StatusDisplayInfo_Minor;
		statusText[ObjectStatus.MAJOR.getValue()] = Messages.get(display).StatusDisplayInfo_Major;
		statusText[ObjectStatus.CRITICAL.getValue()] = Messages.get(display).StatusDisplayInfo_Critical;
		statusText[ObjectStatus.UNKNOWN.getValue()] = Messages.get(display).StatusDisplayInfo_Unknown;
		statusText[ObjectStatus.UNMANAGED.getValue()] = Messages.get(display).StatusDisplayInfo_Unmanaged;
		statusText[ObjectStatus.DISABLED.getValue()] = Messages.get(display).StatusDisplayInfo_Disabled;
		statusText[ObjectStatus.TESTING.getValue()] = Messages.get(display).StatusDisplayInfo_Testing;

		statusImageDesc[ObjectStatus.NORMAL.getValue()] = Activator.getImageDescriptor("icons/status/normal.png"); //$NON-NLS-1$
		statusImageDesc[ObjectStatus.WARNING.getValue()] = Activator.getImageDescriptor("icons/status/warning.png"); //$NON-NLS-1$
		statusImageDesc[ObjectStatus.MINOR.getValue()] = Activator.getImageDescriptor("icons/status/minor.png"); //$NON-NLS-1$
		statusImageDesc[ObjectStatus.MAJOR.getValue()] = Activator.getImageDescriptor("icons/status/major.png"); //$NON-NLS-1$
		statusImageDesc[ObjectStatus.CRITICAL.getValue()] = Activator.getImageDescriptor("icons/status/critical.png"); //$NON-NLS-1$
		statusImageDesc[ObjectStatus.UNKNOWN.getValue()] = Activator.getImageDescriptor("icons/status/unknown.png"); //$NON-NLS-1$
		statusImageDesc[ObjectStatus.UNMANAGED.getValue()] = Activator.getImageDescriptor("icons/status/unmanaged.png"); //$NON-NLS-1$
		statusImageDesc[ObjectStatus.DISABLED.getValue()] = Activator.getImageDescriptor("icons/status/disabled.png"); //$NON-NLS-1$
		statusImageDesc[ObjectStatus.TESTING.getValue()] = Activator.getImageDescriptor("icons/status/testing.png"); //$NON-NLS-1$
		
		for(int i = 0; i < statusImageDesc.length; i++)
			statusImage[i] = statusImageDesc[i].createImage(display);

      colorCache = new ColorCache(display);
      updateStatusColorsInternal();
		
		display.disposeExec(new Runnable() {
         @Override
         public void run()
         {
            for(int i = 0; i < statusImageDesc.length; i++)
               statusImage[i].dispose();
            colorCache.dispose();
         }
      });
	}

   /**
    * Update status colors
    */
   public static void updateStatusColors()
   {
      getInstance().updateStatusColorsInternal();
   }
   
   /**
    * Update status colors
    */
   private void updateStatusColorsInternal()
   {
      final IPreferenceStore ps = Activator.getDefault().getPreferenceStore();
      statusColor[0] = colorCache.create(PreferenceConverter.getColor(ps, "Status.Colors.Normal"));
      statusColor[1] = colorCache.create(PreferenceConverter.getColor(ps, "Status.Colors.Warning"));
      statusColor[2] = colorCache.create(PreferenceConverter.getColor(ps, "Status.Colors.Minor"));
      statusColor[3] = colorCache.create(PreferenceConverter.getColor(ps, "Status.Colors.Major"));
      statusColor[4] = colorCache.create(PreferenceConverter.getColor(ps, "Status.Colors.Critical"));
      statusColor[5] = colorCache.create(PreferenceConverter.getColor(ps, "Status.Colors.Unknown"));
      statusColor[6] = colorCache.create(PreferenceConverter.getColor(ps, "Status.Colors.Unmanaged"));
      statusColor[7] = colorCache.create(PreferenceConverter.getColor(ps, "Status.Colors.Disabled"));
      statusColor[8] = colorCache.create(PreferenceConverter.getColor(ps, "Status.Colors.Testing"));
   }
	
   /**
    * Get text for given status/severity code.
    * 
    * @param status Status code
    * @return Text for given code
    */
   public static String getStatusText(ObjectStatus status)
   {
      return getInstance().statusText[status.getValue()];
   }
   
   /**
    * Get text for given status/severity code.
    * 
    * @param severity Severity code
    * @return Text for given code
    */
   public static String getStatusText(Severity severity)
   {
      return getInstance().statusText[severity.getValue()];
   }
   
   /**
    * Get text for given status/severity code.
    * 
    * @param code Status or severity code
    * @return Text for given code
    */
   public static String getStatusText(int code)
   {
      return getStatusText(ObjectStatus.getByValue(code));
   }
   
   /**
    * Get image descriptor for given status/severity code.
    * 
    * @param status Status code
    * @return Image descriptor for given code
    */
   public static ImageDescriptor getStatusImageDescriptor(ObjectStatus status)
   {
      return getInstance().statusImageDesc[status.getValue()];
   }
   
   /**
    * Get image descriptor for given status/severity code.
    * 
    * @param severity Severity code
    * @return Image descriptor for given code
    */
   public static ImageDescriptor getStatusImageDescriptor(Severity severity)
   {
      return getInstance().statusImageDesc[severity.getValue()];
   }
   
   /**
    * Get image descriptor for given status/severity code.
    * 
    * @param code Status or severity code
    * @return Image descriptor for given code
    */
   public static ImageDescriptor getStatusImageDescriptor(int code)
   {
      return getStatusImageDescriptor(ObjectStatus.getByValue(code));
   }
   
   /**
    * Get image for given status/severity code. Image is owned by library
    * and should not be disposed by caller.
    * 
    * @param status Status code
    * @return Image descriptor for given code
    */
   public static Image getStatusImage(ObjectStatus status)
   {
      return getInstance().statusImage[status.getValue()];
   }
   
   /**
    * Get image for given status/severity code. Image is owned by library
    * and should not be disposed by caller.
    * 
    * @param code Status or severity code
    * @return Image descriptor for given code
    */
   public static Image getStatusImage(int code)
   {
      return getStatusImage(ObjectStatus.getByValue(code));
   }
   
   /**
    * Get image for given status/severity code. Image is owned by library
    * and should not be disposed by caller.
    * 
    * @param severity Severity code
    * @return Image descriptor for given code
    */
   public static Image getStatusImage(Severity severity)
   {
      return getInstance().statusImage[severity.getValue()];
   }
   
	/**
	 * Get color for given status/severity code.
	 * 
	 * @param status Status code
	 * @return Color for given code
	 */
	public static Color getStatusColor(ObjectStatus status)
	{
	   
		return getInstance().statusColor[status.getValue()];
	}
   
   /**
    * Get color for given status/severity code.
    * 
    * @param severity Severity code
    * @return Color for given code
    */
   public static Color getStatusColor(Severity severity)
   {
      return getInstance().statusColor[severity.getValue()];
   }
   
   /**
    * Get color for given status/severity code.
    * 
    * @param code Status or severity code
    * @return Color for given code
    */
   public static Color getStatusColor(int code)
   {
      return getStatusColor(ObjectStatus.getByValue(code));
   }
}
