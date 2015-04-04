/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
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
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Id$
* $Date$
*/

#include "sdk_precomp.h"
#include <wx/xrc/xmlres.h>
#include <wx/fs_zip.h>
#include <wx/menu.h>
#include <wx/toolbar.h>

#include "manager.h" // class's header file
#include "cbexception.h"
#include "projectmanager.h"
#include "editormanager.h"
#include "messagemanager.h"
#include "pluginmanager.h"
#include "toolsmanager.h"
#include "macrosmanager.h"
#include "configmanager.h"
#include "scriptingmanager.h"
#include "templatemanager.h"
#include "personalitymanager.h"
#include "uservarmanager.h"
#include "xtra_classes.h" // Our custom set of wxWidgets classes
#include "xtra_res.h" // our new ToolBarAddOn handler


Manager::Manager() : m_pAppWindow(0)
{
}

Manager::~Manager()
{
    Shutdown();
    CfgMgrBldr::Free(); // only terminate config at the very last moment
}


Manager* Manager::Get(wxFrame *appWindow)
{
	static Manager instance;

    if(appWindow)
    {
        if(instance.m_pAppWindow)
        {
            cbThrow(_T("Illegal argument to Manager::Get()"));
        }
        else
        {
            instance.m_pAppWindow = appWindow;
            Initxrc(true);
            Loadxrc(_T("/manager_resources.zip#zip:*.xrc"));
            Get()->GetMessageManager()->Log(_("Manager initialized"));
        }
    }
    return &instance;
}

void Manager::BlockYields(bool block)
{
    blockYields = block;
}

void Manager::ProcessPendingEvents()
{
    if (!blockYields && !appShuttingDown)
        wxTheApp->ProcessPendingEvents();
}

void Manager::Yield()
{
    if (!blockYields && !appShuttingDown)
        wxTheApp->Yield(true);
}

void Manager::Shutdown()
{
    appShuttingDown = true;

    ToolsManager::Free();
	TemplateManager::Free();
	PluginManager::Free();
	ScriptingManager::Free();
	ProjectManager::Free();
	EditorManager::Free();
	PersonalityManager::Free();
	MacrosManager::Free();
	UserVariableManager::Free();
	MessageManager::Free();
}

bool Manager::SendEventTo(wxEvtHandler* handler, CodeBlocksEvent& event)
{
    return handler && handler->ProcessEvent(event);
}

bool Manager::ProcessEvent(CodeBlocksEvent& event)
{
//    if (SendEventTo(GetMessageManager(), event) ||
//        SendEventTo(GetEditorManager(), event) ||
//        SendEventTo(GetProjectManager(), event) ||
//        SendEventTo(GetToolsManager(), event))
//    {
//        return true;
//    }

    // send it to plugins
    if (GetPluginManager())
    {
        GetPluginManager()->NotifyPlugins(event);
    }
    return false;
}


bool Manager::isappShuttingDown()
{
    return appShuttingDown;
}


void Manager::Initxrc(bool force)
{
    static bool xrcok = false;
    if(!xrcok || force)
    {
        wxFileSystem::AddHandler(new wxZipFSHandler);
        wxXmlResource::Get()->InsertHandler(new wxToolBarAddOnXmlHandler);
        wxXmlResource::Get()->InitAllHandlers();

        xrcok=true;
    }
}

void Manager::Loadxrc(wxString relpath)
{
    Manager::Initxrc();
    wxString resPath = ConfigManager::GetDataFolder();
    wxXmlResource::Get()->Load(resPath + relpath);
}

wxMenuBar *Manager::LoadMenuBar(wxString resid,bool createonfailure)
{
    wxMenuBar *m = wxXmlResource::Get()->LoadMenuBar(resid);
    if(!m && createonfailure) m=new wxMenuBar();
    return m;
}

wxMenu *Manager::LoadMenu(wxString menu_id,bool createonfailure)
{
    wxMenu *m = wxXmlResource::Get()->LoadMenu(menu_id);
    if(!m && createonfailure) m=new wxMenu(_T(""));
    return m;
}

wxToolBar *Manager::LoadToolBar(wxFrame *parent,wxString resid,bool defaultsmall)
{
    if(!parent)
        return 0L;
    wxToolBar *tb = wxXmlResource::Get()->LoadToolBar(parent,resid);
    if(!tb)
    {
        int flags = wxTB_HORIZONTAL;
        int major;
        int minor;
        // version==wxWINDOWS_NT && major==5 && minor==1 => windowsXP
        bool isXP = wxGetOsVersion(&major, &minor) == wxWINDOWS_NT && major == 5 && minor == 1;
        if (!isXP)
            flags |= wxTB_FLAT;
        tb = parent->CreateToolBar(flags, wxID_ANY);
        tb->SetToolBitmapSize(defaultsmall ? wxSize(16, 16) : wxSize(22, 22));
    }

    return tb;
}

void Manager::AddonToolBar(wxToolBar* toolBar,wxString resid)
{
    if(!toolBar)
        return;
    wxXmlResource::Get()->LoadObject(toolBar,NULL,resid,_T("wxToolBarAddOn"));
}

bool Manager::isToolBar16x16(wxToolBar* toolBar)
{
    if(!toolBar) return true; // Small by default
    wxSize mysize=toolBar->GetToolBitmapSize();
    return (mysize.GetWidth()<=16 && mysize.GetHeight()<=16);
}

wxFrame* Manager::GetAppWindow() const
{
    return m_pAppWindow;
}

ProjectManager* Manager::GetProjectManager() const
{
    return ProjectManager::Get();
}

EditorManager* Manager::GetEditorManager() const
{
    return EditorManager::Get();
}

MessageManager* Manager::GetMessageManager() const
{
    return MessageManager::Get();
}

PluginManager* Manager::GetPluginManager() const
{
    return PluginManager::Get();
}

ToolsManager* Manager::GetToolsManager() const
{
    return ToolsManager::Get();
}

MacrosManager* Manager::GetMacrosManager() const
{
    return MacrosManager::Get();
}

PersonalityManager* Manager::GetPersonalityManager() const
{
    return PersonalityManager::Get();
}

UserVariableManager* Manager::GetUserVariableManager() const
{
    return UserVariableManager::Get();
}

ScriptingManager* Manager::GetScriptingManager() const
{
    return ScriptingManager::Get();
}

ConfigManager* Manager::GetConfigManager(const wxString& name_space) const
{
    return CfgMgrBldr::GetConfigManager(name_space);
}


bool Manager::appShuttingDown = false;
bool Manager::blockYields = false;