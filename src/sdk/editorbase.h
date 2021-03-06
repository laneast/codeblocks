#ifndef EDITORBASE_H
#define EDITORBASE_H

#include <wx/hashmap.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include "settings.h"

class wxMenu;
class EditorBase;

WX_DECLARE_HASH_MAP(int, EditorBase*, wxIntegerHash, wxIntegerEqual, SwitchToMap);

/** @brief Base class that all "editors" should inherit from. */
class DLLIMPORT EditorBase : public wxPanel
{
    DECLARE_EVENT_TABLE()
    public:
        EditorBase(wxWindow* parent, const wxString& filename);
        virtual ~EditorBase();

		virtual const wxString& GetFilename(){ return m_Filename; }
		/** Sets the editor's filename */
		virtual void SetFilename(const wxString& x){ m_Filename = x; }

		/** Returns the editor's short name. It is the name displayed on the
		  * editor's tab...
		  */
		virtual const wxString& GetShortName(){ return m_Shortname; }
		/** Returns true if editor is modified, false otherwise */
		virtual bool GetModified() { return false; }
		/** Set the editor's modification state to \c modified. */
		virtual void SetModified(bool modified = true) {}

		virtual const wxString& GetTitle();
		virtual void SetTitle(const wxString& newTitle);

		virtual void Activate();

		/** @return true if this editor can be closed */
		virtual bool QueryClose(){ return true; }
		/** default implementation, calls Destroy() and returns true
		  *  @return true if editor closed succesfully
		  */
		virtual bool Close();

		/** Save editor contents. Returns true on success, false otherwise. */
		virtual bool Save() { return true; }

        /** deliberately non-virtual */
        virtual bool IsBuiltinEditor();
        /** Are there other editors besides this? */
        virtual bool ThereAreOthers();

		/** Displays the editor's context menu (usually invoked by the user right-clicking in the editor) */
		virtual void DisplayContextMenu(const wxPoint& position,bool noeditor = false);

        /** Should this kind of editor be visible from the open files tree? */
        virtual bool VisibleToTree() { return true; }

    protected:
        /** Initializes filename data */
        virtual void InitFilename(const wxString& filename);
        /** Creates context submenus. See cbEditor code for details */
        virtual wxMenu* CreateContextSubMenu(int id); // For context menus

        /** Creates context menu items, both before and after creating plugins menu items */
        virtual void AddToContextMenu(wxMenu* popup,bool noeditor,bool pluginsdone) {}
        /** Creates unique filename when asking to save the file */
        virtual wxString CreateUniqueFilename();
        /** Informs the editor we 're just about to create a context menu.
          * Default implementation, just returns true.
          * @param noeditor If true, this is an "OpenFilesList" popup menu.
          * @return If the editor returns false, the context menu creation is aborted.
          */
        virtual bool OnBeforeBuildContextMenu(bool noeditor){ return true; }
        /** Informs the editor we 're done creating the context menu (just about to display it).
          * Default implementation does nothing.
          * @param noeditor If true, this is an "OpenFilesList" popup menu.
          */
        virtual void OnAfterBuildContextMenu(bool noeditor){}

        bool m_IsBuiltinEditor; // do not mess with it!
		wxString m_Shortname;
		wxString m_Filename;
    private:

        /** one event handler for all popup menu entries */
        void OnContextMenuEntry(wxCommandEvent& event);
        void BasicAddToContextMenu(wxMenu* popup,bool noeditor);
        SwitchToMap m_SwitchTo;
        wxString m_WinTitle;
};

#endif // EDITORBASE_H
