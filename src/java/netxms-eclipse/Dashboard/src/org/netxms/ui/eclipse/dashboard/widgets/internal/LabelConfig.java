/**
 * NetXMS - open source network management system
 * Copyright (C) 2003-2011 Victor Kirhenshtein
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
package org.netxms.ui.eclipse.dashboard.widgets.internal;

import java.io.StringWriter;
import java.io.Writer;
import org.simpleframework.xml.Element;
import org.simpleframework.xml.Root;
import org.simpleframework.xml.Serializer;
import org.simpleframework.xml.core.Persister;

/**
 * Configuration for label
 */
@Root(name="element")
public class LabelConfig
{
	@Element(required=true)
	private String title = "";

	@Element(required=false)
	private String foreground = "0x000000";

	@Element(required=false)
	private String background = "0xFFFFFF";

	/**
	 * Create line chart settings object from XML document
	 * 
	 * @param xml XML document
	 * @return deserialized object
	 * @throws Exception if the object cannot be fully deserialized
	 */
	public static LabelConfig createFromXml(final String xml) throws Exception
	{
		Serializer serializer = new Persister();
		return serializer.read(LabelConfig.class, xml);
	}
	
	/**
	 * Create XML document from object
	 * 
	 * @return XML document
	 * @throws Exception if the schema for the object is not valid
	 */
	public String createXml() throws Exception
	{
		Serializer serializer = new Persister();
		Writer writer = new StringWriter();
		serializer.write(this, writer);
		return writer.toString();
	}

	/**
	 * @return the title
	 */
	public String getTitle()
	{
		return title;
	}

	/**
	 * @param title the title to set
	 */
	public void setTitle(String title)
	{
		this.title = title;
	}

	/**
	 * @return the foreground
	 */
	public String getForeground()
	{
		return foreground;
	}

	/**
	 * @param foreground the foreground to set
	 */
	public void setForeground(String foreground)
	{
		this.foreground = foreground;
	}

	/**
	 * @return the background
	 */
	public String getBackground()
	{
		return background;
	}

	/**
	 * @param background the background to set
	 */
	public void setBackground(String background)
	{
		this.background = background;
	}
}
