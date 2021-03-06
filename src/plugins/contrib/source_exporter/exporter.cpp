/***************************************************************
 * Name:      exporter.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Ceniza<ceniza@gda.utp.edu.co>
 * Copyright: (c) Ceniza
 * License:   GPL
 **************************************************************/

#if defined(__GNUG__) && !defined(__APPLE__)
 #pragma implementation "exporter.h"
#endif

#include "exporter.h"
#include <configmanager.h>
#include <manager.h>
#include <cbeditor.h>
#include <editormanager.h>
#include <editorcolorset.h>
#include <messagemanager.h>
#include <cbexception.h>
#include <licenses.h> // defines some common licenses (like the GPL)
#include "HTMLExporter.h"
#include "RTFExporter.h"
#include "ODTExporter.h"

static int idFileExportHTML = wxNewId();
static int idFileExportRTF = wxNewId();
static int idFileExportODT = wxNewId();

// Implement the plugin's hooks
CB_IMPLEMENT_PLUGIN(Exporter);

BEGIN_EVENT_TABLE(Exporter, cbPlugin)
  EVT_MENU(idFileExportHTML, Exporter::OnExportHTML)
  EVT_MENU(idFileExportRTF, Exporter::OnExportRTF)
  EVT_MENU(idFileExportODT, Exporter::OnExportODT)
  EVT_UPDATE_UI(idFileExportHTML, Exporter::OnUpdateUI)
  EVT_UPDATE_UI(idFileExportRTF, Exporter::OnUpdateUI)
  EVT_UPDATE_UI(idFileExportODT, Exporter::OnUpdateUI)
END_EVENT_TABLE()

Exporter::Exporter()
{
  //ctor
  m_PluginInfo.name = _T("Source HTML, RTF and ODT Exporter");
  m_PluginInfo.title = _("Source HTML, RTF and ODT exporter");
  m_PluginInfo.version = _T("0.4");
  m_PluginInfo.description = _("Plugin to export syntax highlighted source files to HTML, RTF or ODT.");
  m_PluginInfo.author = _T("Ceniza");
  m_PluginInfo.authorEmail = _T("ceniza@gda.utp.edu.co");
  m_PluginInfo.authorWebsite = _T("");
  m_PluginInfo.thanksTo = _("Code::Blocks Development Team");
  m_PluginInfo.license = LICENSE_GPL;
  m_PluginInfo.hasConfigure = false;

  ConfigManager::AddConfiguration(m_PluginInfo.title, _T("/exporter"));
}

Exporter::~Exporter()
{
  //dtor
}

void Exporter::OnAttach()
{
  // do whatever initialization you need for your plugin
  // NOTE: after this function, the inherited member variable
  // m_IsAttached will be TRUE...
  // You should check for it in other functions, because if it
  // is FALSE, it means that the application did *not* "load"
  // (see: does not need) this plugin...
}

void Exporter::OnRelease(bool appShutDown)
{
  // do de-initialization for your plugin
  // if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down,
  // which means you must not use any of the SDK Managers
  // NOTE: after this function, the inherited member variable
  // m_IsAttached will be FALSE...
}

void Exporter::BuildMenu(wxMenuBar *menuBar)
{
  // find "File" menu position
  int fileMenuPos = menuBar->FindMenu(_("File"));

  if (fileMenuPos == -1)
  {
    cbThrow("Can't find \"File\" menu position?!?");
  }

  // find actual "File" menu
  wxMenu *file = menuBar->GetMenu(fileMenuPos);

  if (!file)
  {
    cbThrow("Can't find \"File\" menu?!?");
  }

  // decide where to insert in "File" menu
  size_t printPos = file->GetMenuItemCount() - 4; // the default location
  int printID = file->FindItem(_("Print..."));

  if (printID != wxNOT_FOUND)
  {
    file->FindChildItem(printID, &printPos);
    ++printPos; // after "Print"
  }

  // insert menu items
  file->Insert(printPos, idFileExportODT, _("Export to ODT"), _("Exports the current file to ODT"));
  file->Insert(printPos, idFileExportRTF, _("Export to RTF"), _("Exports the current file to RTF"));
  file->Insert(printPos, idFileExportHTML, _("Export to HTML"), _("Exports the current file to HTML"));
}

void Exporter::RemoveMenu(wxMenuBar *menuBar)
{
  wxMenu *menu = 0;
  wxMenuItem *itemHTML = menuBar->FindItem(idFileExportHTML, &menu);

  if (menu && itemHTML)
  {
    menu->Remove(itemHTML);
  }

  wxMenuItem *itemRTF = menuBar->FindItem(idFileExportRTF, &menu);

  if (menu && itemRTF)
  {
    menu->Remove(itemRTF);
  }

  wxMenuItem *itemODT = menuBar->FindItem(idFileExportODT, &menu);

  if (menu && itemODT)
  {
    menu->Remove(itemODT);
  }
}

void Exporter::OnUpdateUI(wxUpdateUIEvent &event)
{
  if (Manager::isappShuttingDown())
  {
    event.Skip();
    return;
  }

  wxMenuBar *mbar = Manager::Get()->GetAppWindow()->GetMenuBar();

  if (mbar)
  {
    EditorManager *em = EDMAN();

    // Enabled if there's a source file opened (be sure it isn't the "Start here" page)
    bool disable = !em || !em->GetActiveEditor() || !em->GetBuiltinActiveEditor();
    mbar->Enable(idFileExportHTML, !disable);
    mbar->Enable(idFileExportRTF, !disable);
    mbar->Enable(idFileExportODT, !disable);
  }

  event.Skip();
}

void Exporter::OnExportHTML(wxCommandEvent &event)
{
  HTMLExporter exp;
  ExportFile(&exp, _T("html"), _("HTML files|*.html;*.htm"));
}

void Exporter::OnExportRTF(wxCommandEvent &event)
{
  RTFExporter exp;
  ExportFile(&exp, _T("rtf"), _("RTF files|*.rtf"));
}


void Exporter::OnExportODT(wxCommandEvent &event)
{
  ODTExporter exp;
  ExportFile(&exp, _T("odt"), _("ODT files|*.odt"));
}

void Exporter::ExportFile(BaseExporter *exp, const wxString &default_extension, const wxString &wildcard)
{
  if (!m_IsAttached)
  {
    return;
  }

  EditorManager *em = EDMAN();
  cbEditor *cb = em->GetBuiltinActiveEditor();
  wxString filename = wxFileSelector(_("Choose the filename"), _T(""), _T(""), default_extension, wildcard, wxSAVE | wxOVERWRITE_PROMPT);

  if (filename.IsEmpty())
  {
    return;
  }

  wxScintilla *ed = cb->GetControl();
  wxMemoryBuffer mb = ed->GetStyledText(0, ed->GetLength() - 1);
  EditorColorSet *ecs = cb->GetColorSet();

  exp->Export(filename, cb->GetFilename(), mb, ecs);
}
