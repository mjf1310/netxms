/**
 * NetXMS - open source network management system
 * Copyright (C) 2003-2019 Raden Solutions
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
package org.netxms.ui.eclipse.objectmanager.dialogs;

import java.util.Date;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.netxms.ui.eclipse.objectmanager.Messages;
import org.netxms.ui.eclipse.tools.MessageDialogHelper;
import org.netxms.ui.eclipse.tools.WidgetHelper;
import org.netxms.ui.eclipse.widgets.DateTimeSelector;
import org.netxms.ui.eclipse.widgets.LabeledText;

/**
 * Dialog for entering maintenance schedule
 */
public class MaintanenceScheduleDialog extends Dialog
{
   private Date startDate;
   private Date endDate;
   private String comments;
   private DateTimeSelector startDateSelector;
   private DateTimeSelector endDateSelector;
   private Label labelStartDate;
   private Label labelEndDate;
   private LabeledText commentsEditor;

   /**
    * @param parentShell
    */
   public MaintanenceScheduleDialog(Shell parentShell)
   {
      super(parentShell);
   }
   
   /* (non-Javadoc)
    * @see org.eclipse.jface.window.Window#configureShell(org.eclipse.swt.widgets.Shell)
    */
   @Override
   protected void configureShell(Shell newShell)
   {
      super.configureShell(newShell);
      newShell.setText(Messages.get().MaintanenceScheduleDialog_Title);
   }
   
   /* (non-Javadoc)
    * @see org.eclipse.jface.dialogs.Dialog#createDialogArea(org.eclipse.swt.widgets.Composite)
    */
   @Override
   protected Control createDialogArea(Composite parent)
   {
      Composite dialogArea = (Composite)super.createDialogArea(parent);
      
      GridLayout layout = new GridLayout();
      layout.verticalSpacing = WidgetHelper.OUTER_SPACING;
      layout.marginHeight = WidgetHelper.DIALOG_HEIGHT_MARGIN;
      layout.marginWidth = WidgetHelper.DIALOG_WIDTH_MARGIN;
      layout.numColumns = 2;
      dialogArea.setLayout(layout);
      
      labelStartDate = new Label(dialogArea, SWT.NONE);
      labelStartDate.setText(Messages.get().MaintanenceScheduleDialog_StartDate);
            
      startDateSelector = new DateTimeSelector(dialogArea, SWT.NONE);
      startDateSelector.setValue(new Date());
      startDateSelector.setToolTipText(Messages.get().MaintanenceScheduleDialog_StartDate);
      
      labelEndDate = new Label(dialogArea, SWT.NONE);
      labelEndDate.setText(Messages.get().MaintanenceScheduleDialog_EndDate);
      
      endDateSelector = new DateTimeSelector(dialogArea, SWT.NONE);
      endDateSelector.setValue(new Date());
      endDateSelector.setToolTipText(Messages.get().MaintanenceScheduleDialog_EndDate);
      
      commentsEditor = new LabeledText(dialogArea, SWT.NONE);
      commentsEditor.setLabel("Comments");
      commentsEditor.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
      
      return dialogArea;
   }
   
   /* (non-Javadoc)
    * @see org.eclipse.jface.dialogs.Dialog#okPressed()
    */
   @Override
   protected void okPressed()
   {
      startDate = startDateSelector.getValue();
      endDate = endDateSelector.getValue();
      comments = commentsEditor.getText();
      if (startDate.after(endDate))
      {
         MessageDialogHelper.openWarning(getShell(), Messages.get().MaintanenceScheduleDialog_Warning, Messages.get().MaintanenceScheduleDialog_WarningText);
         return;
      }
      
      super.okPressed();
   }

   /**
    * Get start date
    * 
    * @return start date
    */
   public Date getStartDate()
   {
      return startDate;
   }

   /**
    * Get end date
    * 
    * @return end date
    */
   public Date getEndDate()
   {
      return endDate;
   }

   /**
    * Get comments
    * 
    * @return comments
    */
   public String getComments()
   {
      return comments;
   }
}
