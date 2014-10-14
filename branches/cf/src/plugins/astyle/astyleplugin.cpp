/***************************************************************
 * Name:      astyle.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Yiannis Mandravellos<mandrav@codeblocks.org>
 * Created:   05/25/04 10:06:40
 * Copyright: (c) Yiannis Mandravellos
 * License:   GPL
 **************************************************************/

#include <sdk.h>

#include "astyleplugin.h"
#include <cbexception.h>
#include "astyleconfigdlg.h"
#include <sstream>
#include <string>
#include "formattersettings.h"
#include <manager.h>
#include <editormanager.h>
#include <configmanager.h>
#include <cbeditor.h>
#include <wx/msgdlg.h>
#include <wx/xrc/xmlres.h>
#include <wx/fs_zip.h>
#include <wx/strconv.h>
#include "asstreamiterator.h"

using std::istringstream;
using std::string;

// this auto-registers the plugin
namespace
{
    PluginRegistrant<AStylePlugin> reg(_T("AStylePlugin"));
}

AStylePlugin::AStylePlugin()
{
	//ctor

    if(!Manager::LoadResource(_T("astyle.zip")))
    {
        NotifyMissingFile(_T("astyle.zip"));
    }
}

AStylePlugin::~AStylePlugin()
{
	//dtor
}

void AStylePlugin::OnAttach()
{
}

void AStylePlugin::OnRelease(bool appShutDown)
{
	// do de-initialization for your plugin
	// NOTE: after this function, the inherited member variable
	// m_IsAttached will be FALSE...
}

int AStylePlugin::Configure()
{
//  AstyleConfigDlg dlg(Manager::Get()->GetAppWindow());
//  dlg.ShowModal();

  return 0;
}

cbConfigurationPanel* AStylePlugin::GetConfigurationPanel(wxWindow* parent)
{
    AstyleConfigDlg* dlg = new AstyleConfigDlg(parent);
    // deleted by the caller

    return dlg;
}

int AStylePlugin::Execute()
{
  if (!IsAttached())
  {
    return -1;
  }

  cbEditor *ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();

  if (!ed)
  {
    return 0;
  }

  wxString edText(ed->GetControl()->GetText());
  wxString formattedText;

  astyle::ASFormatter formatter;

  // load settings
  FormatterSettings settings;
  settings.ApplyTo(formatter);

  wxString eolChars;

  switch (ed->GetControl()->GetEOLMode())
  {
    case wxSCI_EOL_CRLF: eolChars = _T("\r\n"); break;
    case wxSCI_EOL_CR: eolChars = _T("\r"); break;
    case wxSCI_EOL_LF: eolChars = _T("\n"); break;
  }

  if (edText.size() && edText.Last() != _T('\r') && edText.Last() != _T('\n'))
  {
    edText += eolChars;
  }

  formatter.init(new ASStreamIterator(edText, eolChars));

  wxSetCursor(*wxHOURGLASS_CURSOR);

  while (formatter.hasMoreLines())
  {
    formattedText << cbC2U(formatter.nextLine().c_str());

    if (formatter.hasMoreLines())
    {
      formattedText << eolChars;
    }
  }

  int pos = ed->GetControl()->GetCurrentPos();

  ed->GetControl()->BeginUndoAction();
  ed->GetControl()->SetText(formattedText);
  ed->GetControl()->EndUndoAction();
  ed->GetControl()->GotoPos(pos);
  ed->SetModified(true);

  wxSetCursor(*wxSTANDARD_CURSOR);

  return 0;
}