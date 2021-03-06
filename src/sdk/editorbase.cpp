#include "sdk_precomp.h"
#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/notebook.h>
#include <wx/menu.h>
#include "manager.h"
#include "editorbase.h"
#include "editormanager.h"
#include "pluginmanager.h"

// needed for initialization of variables
int editorbase_RegisterId(int id)
{
    wxRegisterId(id);
    return id;
}

// The following lines reserve 255 consecutive id's
const int EditorMaxSwitchTo = 255;
const int idSwitchFile1 = wxNewId();
const int idSwitchFileMax = editorbase_RegisterId(idSwitchFile1 + EditorMaxSwitchTo -1);

const int idCloseMe = wxNewId();
const int idCloseAll = wxNewId();
const int idCloseAllOthers = wxNewId();
const int idSaveMe = wxNewId();
const int idSaveAll = wxNewId();
const int idSwitchTo = wxNewId();

BEGIN_EVENT_TABLE(EditorBase, wxPanel)
	EVT_MENU_RANGE(idSwitchFile1, idSwitchFileMax,EditorBase::OnContextMenuEntry)
	EVT_MENU(idCloseMe, EditorBase::OnContextMenuEntry)
	EVT_MENU(idCloseAll, EditorBase::OnContextMenuEntry)
	EVT_MENU(idCloseAllOthers, EditorBase::OnContextMenuEntry)
    EVT_MENU(idSaveMe, EditorBase::OnContextMenuEntry)
    EVT_MENU(idSaveAll, EditorBase::OnContextMenuEntry)
END_EVENT_TABLE()

void EditorBase::InitFilename(const wxString& filename)
{
    if (filename.IsEmpty())
    	m_Filename = wxGetCwd() + wxFileName::GetPathSeparator() + CreateUniqueFilename();
    else
    	m_Filename = filename;

    wxFileName fname;
    fname.Assign(m_Filename);
    m_Shortname = fname.GetFullName();
//    Manager::Get()->GetMessageManager()->DebugLog("ctor: Filename=%s\nShort=%s", m_Filename.c_str(), m_Shortname.c_str());
}

wxString EditorBase::CreateUniqueFilename()
{
    const wxString prefix = _("Untitled");
    wxString tmp;
    int iter = 1;
    while (true)
    {
        tmp.Clear();
        tmp << prefix << iter;
        if (!Manager::Get()->GetEditorManager()->GetEditor(tmp))
            return tmp;
        ++iter;
    }
}

EditorBase::EditorBase(wxWindow* parent, const wxString& filename)
    : wxPanel(parent, -1),
    m_IsBuiltinEditor(false),
    m_Shortname(_T("")),
    m_Filename(_T("")),
    m_WinTitle(filename)
{
    Manager::Get()->GetEditorManager()->AddCustomEditor(this);
    InitFilename(filename);
    SetTitle(filename);
}

EditorBase::~EditorBase()
{
    if (Manager::Get()->GetEditorManager()) // sanity check
        Manager::Get()->GetEditorManager()->RemoveCustomEditor(this);
}

const wxString& EditorBase::GetTitle()
{
    return m_WinTitle;
}

void EditorBase::SetTitle(const wxString& newTitle)
{
    m_WinTitle = newTitle;
    int mypage = Manager::Get()->GetEditorManager()->FindPageFromEditor(this);
    if (mypage != -1)
        Manager::Get()->GetEditorManager()->GetNotebook()->SetPageText(mypage, newTitle);
}

void EditorBase::Activate()
{
    Manager::Get()->GetEditorManager()->SetActiveEditor(this);
}

bool EditorBase::Close()
{
    Destroy();
    return true;
}

bool EditorBase::IsBuiltinEditor()
{
    return m_IsBuiltinEditor;
}

bool EditorBase::ThereAreOthers()
{
    bool hasOthers = false;
    hasOthers = Manager::Get()->GetEditorManager()->GetEditorsCount() > 1;
//    for(int i = 0; i < Manager::Get()->GetEditorManager()->GetEditorsCount(); ++i)
//    {
//        EditorBase* other = Manager::Get()->GetEditorManager()->GetEditor(i);
//        if (!other || other == (EditorBase*)this)
//            continue;
//        hasOthers = true;
//        break;
//    }
    return hasOthers;
}

wxMenu* EditorBase::CreateContextSubMenu(int id) // For context menus
{
    wxMenu* menu = 0;

    if(id == idSwitchTo)
    {
        menu = new wxMenu;
        m_SwitchTo.clear();
        for (int i = 0; i < EditorMaxSwitchTo && i < Manager::Get()->GetEditorManager()->GetEditorsCount(); ++i)
        {
            EditorBase* other = Manager::Get()->GetEditorManager()->GetEditor(i);
            if (!other || other == this)
                continue;
            int id = idSwitchFile1+i;
            m_SwitchTo[id] = other;
            menu->Append(id, other->GetShortName());
        }
        if(!menu->GetMenuItemCount())
        {
            delete menu;
            menu = 0;
        }
    }
    return menu;
}

void EditorBase::BasicAddToContextMenu(wxMenu* popup,bool noeditor)
{
	popup->Append(idCloseMe, _("Close"));
    popup->Append(idCloseAll, _("Close all"));
    popup->Append(idCloseAllOthers, _("Close all others"));
	popup->AppendSeparator();
    popup->Append(idSaveMe, _("Save"));
    popup->Append(idSaveAll, _("Save all"));
	popup->AppendSeparator();
	// enable/disable some items, based on state
    popup->Enable(idSaveMe, GetModified());

    bool hasOthers = ThereAreOthers();
	popup->Enable(idCloseAll, hasOthers);
	popup->Enable(idCloseAllOthers, hasOthers);

    if(!noeditor)
    {
        wxMenu* switchto = CreateContextSubMenu(idSwitchTo);
        if(switchto)
            popup->Append(idSwitchTo, _("Switch to..."), switchto);
    }
}

void EditorBase::DisplayContextMenu(const wxPoint& position,bool noeditor)
{
	// inform the editors we 're just about to create a context menu
	if (!OnBeforeBuildContextMenu(noeditor))
        return;

	// noeditor:
	// True if context menu belongs to open files tree;
	// False if belongs to cbEditor

	wxMenu* popup = new wxMenu;

	// build menu

	// Basic functions
	BasicAddToContextMenu(popup,noeditor);

	// Extended functions, part 1 (virtual)
	AddToContextMenu(popup,noeditor,false);

	// ask other editors / plugins if they need to add any entries in this menu...
    Manager::Get()->GetPluginManager()->AskPluginsForModuleMenu(mtEditorManager, popup, m_Filename);

	popup->AppendSeparator();
	// Extended functions, part 2 (virtual)
    AddToContextMenu(popup,noeditor,true);

	// inform the editors we 're done creating a context menu (just about to show it)
	OnAfterBuildContextMenu(noeditor);

	// display menu
	wxPoint pos = ScreenToClient(position);
	PopupMenu(popup, pos.x, pos.y);

 	delete popup;
}

void EditorBase::OnContextMenuEntry(wxCommandEvent& event)
{
	// we have a single event handler for all popup menu entries
	// This was ported from cbEditor and used for the basic operations:
	// Switch to, close, save, etc.

	const int id = event.GetId();

	if (id == idCloseMe)
		Manager::Get()->GetEditorManager()->Close(this);
	else if (id == idCloseAll)
		Manager::Get()->GetEditorManager()->CloseAll();
	else if (id == idCloseAllOthers)
		Manager::Get()->GetEditorManager()->CloseAllExcept(this);
	else if (id == idSaveMe)
        Save();
    else if (id == idSaveAll)
        Manager::Get()->GetEditorManager()->SaveAll();
    else if (id >= idSwitchFile1 && id <= idSwitchFileMax)
    {
        // "Switch to..." item
        EditorBase* ed = m_SwitchTo[id];
        if (ed)
            Manager::Get()->GetEditorManager()->SetActiveEditor(ed);
        m_SwitchTo.clear();
    }
    else
        event.Skip();
}
