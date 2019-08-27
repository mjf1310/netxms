#include "nxuseragent.h"

#define MENU_ITEM_CLASS_NAME  _T("NetXMS_UA_MenuItem")

/**
 * Top level menu items
 */
static MenuItem s_topLevelMenu;

/**
 * Current open submenu
 */
static MenuItem *s_currentSubmenu = &s_topLevelMenu;

/**
 * Current cursor position
 */
int s_cursorPos = -1;

/**
 * Menu item constructor for top level
 */
MenuItem::MenuItem()
{
   m_name = _tcsdup(_T("[ROOT]"));
   m_displayName = _tcsdup(m_name);
   m_path = _tcsdup(_T(""));
   m_command = NULL;
   m_parent = NULL;
   m_subItems = new ObjectArray<MenuItem>(16, 16, true);
   m_hWnd = NULL;
   m_selected = false;
   m_highlighted = false;
}

/**
 * Menu item constructor
 */
MenuItem::MenuItem(MenuItem *parent)
{
   m_name = MemCopyString(_T("\x21A9"));
   m_displayName = MemCopyString(m_name);
   m_path = _tcsdup(parent->getPath());
   m_command = MemCopyString(_T("\x21A9"));
   m_parent = parent;
   m_subItems = NULL;
   m_hWnd = NULL;
   m_selected = false;
   m_highlighted = false;
}

/**
 * Menu item constructor
 */
MenuItem::MenuItem(MenuItem *parent, ConfigEntry *config, const TCHAR *rootPath)
{
   m_name = MemCopyString(config->getSubEntryValue(_T("name"), 0, _T("")));
   m_displayName = MemCopyString(config->getSubEntryValue(_T("displayName"), 0, m_name));

   String path = rootPath;
   if (!path.isEmpty())
      path.append(_T(" / "));
   path.append(m_name);
   m_path = _tcsdup(path);

   ConfigEntry *subItems = config->findEntry(_T("items"));
   if (subItems != NULL)
   {
      m_command = NULL;
      m_subItems = new ObjectArray<MenuItem>(16, 16, true);
      loadSubItems(subItems);
   }
   else
   {
      m_command = MemCopyString(config->getSubEntryValue(_T("command"), 0, _T("")));
      m_subItems = NULL;
   }

   m_parent = parent;
   m_hWnd = NULL;
   m_selected = false;
   m_highlighted = false;
}

/**
 * Menu item destructor
 */
MenuItem::~MenuItem()
{
   MemFree(m_name);
   MemFree(m_displayName);
   MemFree(m_path);
   MemFree(m_command);
   delete m_subItems;
}

/**
* Load sub-items
*/
void MenuItem::loadSubItems(ConfigEntry *config)
{
   if (*m_path != 0)
      m_subItems->add(new MenuItem(this));   // Add "back" item

   ObjectArray<ConfigEntry> *items = config->getSubEntries(_T("item"));
   for (int i = 0; i < items->size(); i++)
   {
      MenuItem *item = new MenuItem(this, items->get(i), m_path);
      if (!item->isEmptySubMenu())
      {
         nxlog_debug(2, _T("Added menu item %s"), item->getPath());
         m_subItems->add(item);
      }
      else
      {
         nxlog_debug(2, _T("Empty menu item %s"), item->getPath());
         delete item;
      }
   }
   delete items;
}

/**
 * Execute item
 */
void MenuItem::execute()
{
   if (isSubMenu())
   {
      ActivateMenuItem(GetParent(m_hWnd), this);
   }
   else if (!_tcscmp(m_command, _T("\x21A9")))
   {
      ActivateMenuItem(GetParent(m_hWnd), m_parent->m_parent, m_parent);
   }
   else
   {
      TCHAR command[8192];
      ExpandEnvironmentStrings(m_command, command, 8192);
      nxlog_debug(3, _T("Executing command \"%s\""), command);
      nxlog_debug(4, _T("Command was expanded from \"%s\""), m_command);
      ShellExecute(GetParent(m_hWnd), _T("open"), command, NULL, NULL, SW_SHOWDEFAULT);
      CloseApplicationWindow();
   }
   m_selected = false;
   m_highlighted = false;
}

/**
 * Calculate expanded size of this menu item
 */
POINT MenuItem::calculateSize(HDC hdc) const
{
   RECT rect = { 0, 0, 0, 0 };
   DrawText(hdc, m_displayName, (int)_tcslen(m_displayName), &rect, DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE);
   rect.bottom += MENU_VERTICAL_SPACING;
   rect.right += MARGIN_WIDTH * 2;
   if (isSubMenu())
   {
      // Add space for right arrow symbol
      RECT srect = { 0, 0, 0, 0 };
      DrawText(hdc, _T(" M"), 2, &srect, DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE);
      rect.right += srect.right;

      int textHeight = rect.bottom;
      int maxSubMenuHeight = 0;
      for (int i = 0; i < m_subItems->size(); i++)
      {
         MenuItem *item = m_subItems->get(i);
         POINT size = item->calculateSize(hdc);
         if (i > 0)
            rect.bottom += textHeight;
         if (size.x > rect.right)
            rect.right = size.x;
         if (item->isSubMenu() && (size.y > maxSubMenuHeight))
            maxSubMenuHeight = size.y;
      }
      if (maxSubMenuHeight > rect.bottom)
         rect.bottom = maxSubMenuHeight;
   }
   return { rect.right, rect.bottom };
}

/**
 * Draw menu elements
 */
void MenuItem::draw(HDC hdc) const
{
   RECT rect;
   GetClientRect(m_hWnd, &rect);
   SetBkColor(hdc, GetApplicationColor(m_selected ? APP_COLOR_MENU_SELECTED : 
      (m_highlighted ? APP_COLOR_MENU_HIGHLIGHTED : APP_COLOR_MENU_BACKGROUND)));
   SetTextColor(hdc, GetApplicationColor(APP_COLOR_MENU_FOREGROUND));
   SelectObject(hdc, g_menuFont);

   HBRUSH brush = CreateSolidBrush(GetBkColor(hdc));
   SelectObject(hdc, brush);
   SelectObject(hdc, GetStockObject(NULL_PEN));
   Rectangle(hdc, rect.left, rect.top, rect.right + 1, rect.bottom + 1);
   SelectObject(hdc, GetStockObject(NULL_BRUSH));
   DeleteObject(brush);

   InflateRect(&rect, -(MARGIN_WIDTH - MENU_HIGHLIGHT_MARGIN), -MENU_VERTICAL_SPACING / 2);
   DrawText(hdc, m_displayName, (int)_tcslen(m_displayName), &rect, DT_NOPREFIX | DT_LEFT | DT_VCENTER);

   if (isSubMenu())
   {
      SelectObject(hdc, g_symbolFont);
      DrawText(hdc, _T("\xE017"), 1, &rect, DT_NOPREFIX | DT_RIGHT | DT_VCENTER);
   }
}

/**
 * Set "selected" state
 */
void MenuItem::setSelected(bool selected)
{
   m_highlighted = false;
   m_selected = selected;
   InvalidateRect(m_hWnd, NULL, FALSE);
}

/**
 * Activate menu
 */
void MenuItem::activate(HWND hParentWnd)
{
   if (!isSubMenu())
      return;

   RECT parentSize;
   GetClientRect(hParentWnd, &parentSize);
   InflateRect(&parentSize, -BORDER_WIDTH - MENU_HIGHLIGHT_MARGIN, 0);

   HDC hdc = GetDC(hParentWnd);
   SelectObject(hdc, g_menuFont);
   RECT textRect = { 0, 0, 0, 0 };
   DrawText(hdc, _T("X"), 1, &textRect, DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE);
   textRect.bottom += MENU_VERTICAL_SPACING;
   ReleaseDC(hParentWnd, hdc);

   POINT p = GetMenuPosition();
   for (int i = 0; i < m_subItems->size(); i++)
   {
      MenuItem *item = m_subItems->get(i);
      HWND hWnd = CreateWindow(MENU_ITEM_CLASS_NAME, item->getDisplayName(),
         WS_CHILD, p.x, p.y, parentSize.right - parentSize.left, textRect.bottom,
         hParentWnd, NULL, g_hInstance, NULL);
      SetWindowLongPtr(hWnd, 0, (LONG_PTR)item);
      item->setWindowHandle(hWnd);
      ShowWindow(hWnd, SW_SHOW);
      p.y += textRect.bottom;
   }
}

/**
 * Deactivate current menu item
 */
void MenuItem::deactivate()
{
   if (!isSubMenu())
      return;

   for (int i = 0; i < m_subItems->size(); i++)
      DestroyWindow(m_subItems->get(i)->m_hWnd);
}

/**
 * Enable mouse tracking
 */
void MenuItem::trackMouseEvent()
{
   TRACKMOUSEEVENT t;
   t.cbSize = sizeof(TRACKMOUSEEVENT);
   t.dwFlags = TME_HOVER | TME_LEAVE;
   t.dwHoverTime = HOVER_DEFAULT;
   t.hwndTrack = m_hWnd;
   TrackMouseEvent(&t);
}

/**
 * Process Windows messages
 */
LRESULT MenuItem::processMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(msg)
   {
      case WM_PAINT:
         {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(m_hWnd, &ps);
            draw(hdc);
            EndPaint(m_hWnd, &ps);
         }
         break;
      case WM_PRINTCLIENT:
         draw((HDC)wParam);
         break;
      case WM_CREATE:
         trackMouseEvent();
         break;
      case WM_DESTROY:
         m_selected = false;
         m_highlighted = false;
         break;
      case WM_LBUTTONDOWN:
         m_highlighted = false;
         m_selected = true;
         InvalidateRect(m_hWnd, NULL, FALSE);
         trackMouseEvent();
         break;
      case WM_LBUTTONUP:
         if (m_selected)
         {
            execute();
         }
         break;
      case WM_MOUSEHOVER:
         trackMouseEvent();
         if (!m_highlighted && !m_selected)
         {
            m_highlighted = true;
            InvalidateRect(m_hWnd, NULL, FALSE);
         }
         break;
      case WM_MOUSEMOVE:
         if (!m_highlighted && !m_selected)
         {
            m_highlighted = true;
            InvalidateRect(m_hWnd, NULL, FALSE);
            trackMouseEvent();
         }
         break;
      case WM_MOUSELEAVE:
         if (m_selected || m_highlighted)
         {
            m_selected = false;
            m_highlighted = false;
            InvalidateRect(m_hWnd, NULL, FALSE);
         }
         break;
      default:
         return DefWindowProc(m_hWnd, msg, wParam, lParam);
   }
   return 0;
}

/**
 * Window procedure for menu item
 */
static LRESULT CALLBACK MenuItemWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   MenuItem *item = (MenuItem*)GetWindowLongPtr(hWnd, 0);
   return (item != NULL) ? item->processMessage(uMsg, wParam, lParam) : DefWindowProc(hWnd, uMsg, wParam, lParam);;
}

/**
 * Initialize menu
 */
bool InitMenu()
{
   WNDCLASS wc;
   memset(&wc, 0, sizeof(WNDCLASS));
   wc.lpfnWndProc = MenuItemWndProc;
   wc.hInstance = g_hInstance;
   wc.cbWndExtra = sizeof(MenuItem*);
   wc.lpszClassName = MENU_ITEM_CLASS_NAME;
   wc.hbrBackground = CreateSolidBrush(GetApplicationColor(APP_COLOR_MENU_BACKGROUND));
   wc.hCursor = LoadCursor(NULL, IDC_ARROW);
   if (RegisterClass(&wc) == 0)
   {
      TCHAR buffer[1024];
      nxlog_debug(1, _T("RegisterClass(%s) failed (%s)"), MENU_ITEM_CLASS_NAME, GetSystemErrorText(GetLastError(), buffer, 1024));
      return false;
   }
   return true;
}

/**
 * Load menu items
 */
void LoadMenuItems(Config *config)
{
   ConfigEntry *items = config->getEntry(_T("/menu/items"));
   if (items != NULL)
   {
      s_topLevelMenu.loadRootMenu(items);
   }
   else
   {
      nxlog_debug(3, _T("Cannot find top level menu entry in configuration file"));
   }
}

/**
 * Calculate menu size
 */
POINT CalculateMenuSize(HWND hWnd)
{
   HDC hdc = GetDC(hWnd);
   HGDIOBJ oldFont = SelectObject(hdc, g_menuFont);
   POINT size = s_topLevelMenu.calculateSize(hdc);
   SelectObject(hdc, oldFont);
   ReleaseDC(hWnd, hdc);
   return size;
}

/**
 * Activate application menu
 */
void ActivateMenuItem(HWND hParentWnd, MenuItem *item, MenuItem *selection)
{
   if (s_currentSubmenu != NULL)
      s_currentSubmenu->deactivate();
   s_currentSubmenu = (item != NULL) ? item : &s_topLevelMenu;
   s_currentSubmenu->activate(hParentWnd);
   if (s_cursorPos != -1)
   {
      if (selection != NULL)
      {
         s_cursorPos = s_currentSubmenu->getItemPos(selection);
         selection->setSelected(true);
      }
      else
      {
         s_cursorPos = 0;
         MenuItem *item = s_currentSubmenu->getItemAtPos(s_cursorPos);
         if (item != NULL)
            item->setSelected(true);
      }
   }
}

/**
 * Move menu cursor
 */
void MoveMenuCursor(bool backward)
{
   if (s_cursorPos != -1)
   {
      MenuItem *item = s_currentSubmenu->getItemAtPos(s_cursorPos);
      if (item != NULL)
         item->setSelected(false);
   }
   else if (backward)
   {
      s_cursorPos = s_currentSubmenu->getItemCount();
   }

   if (backward)
   {
      s_cursorPos--;
      if (s_cursorPos < 0)
         s_cursorPos = s_currentSubmenu->getItemCount() - 1;
   }
   else
   {
      s_cursorPos++;
      if (s_cursorPos == s_currentSubmenu->getItemCount())
         s_cursorPos = 0;
   }

   MenuItem *item = s_currentSubmenu->getItemAtPos(s_cursorPos);
   if (item != NULL)
      item->setSelected(true);
}

/**
 * Reset menu cursor
 */
void ResetMenuCursor()
{
   s_cursorPos = -1;
}

/**
 * Execute currently selected menu item
 */
void ExecuteCurrentMenuItem()
{
   if (s_cursorPos != -1)
   {
      MenuItem *item = s_currentSubmenu->getItemAtPos(s_cursorPos);
      if (item != NULL)
         item->execute();
   }
}

/**
 * Execute "back" menu item
 */
void ExecuteReturnToParentMenu()
{
   if (!s_currentSubmenu->isTopLevelMenu())
   {
      MenuItem *item = s_currentSubmenu->getItemAtPos(0);
      if (item != NULL)
         item->execute();
   }
}