#ifndef MENUITEMSMANAGER_H
#define MENUITEMSMANAGER_H

#include "settings.h"
#include <wx/menu.h>

WX_DEFINE_ARRAY(wxMenuItem*, MenuItemsList);

/**
  * @brief Manager for wxMenuItem pointers.
  *
  * This class manages an array of wxMenuItem pointers. Usually used by
  * classes that need to create menu items in the app and, at some point,
  * remove them *without* messing with other menu items, created by other
  * classes. Useful for plugins.\n
  * To use it, add a MenuItemsManager variable in your class and then
  * use MenuItemsManager::Add() to add menu items to a menu (instead of
  * wxMenu::Append). When you no longer want those menu items, call
  * MenuItemsManager::Clear(). That's it.
  *
  * @author Yiannis Mandravellos
  */
class DLLIMPORT MenuItemsManager
{
	public:
		MenuItemsManager();
		virtual ~MenuItemsManager();
		
		virtual void Add(wxMenu* parent, int id, const wxString& caption, const wxString& helptext);
		virtual void Insert(wxMenu* parent, int index, int id, const wxString& caption, const wxString& helptext);
		virtual void Clear(wxMenu* menu);
	protected:
		MenuItemsList m_MenuItems; ///< The managed array of wxMenuItem pointers
		wxMenu* m_Menu; ///< Holds the menu to append items (used only in dtor)
	private:
};

#endif // MENUITEMSMANAGER_H

