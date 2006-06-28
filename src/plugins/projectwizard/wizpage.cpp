#include <sdk.h>
#ifndef CB_PRECOMP
    #include <manager.h>
    #include <messagemanager.h>
    #include <configmanager.h>
    #include <projectmanager.h>
    #include <scriptingmanager.h>
    #include <compilerfactory.h>
    #include <compiler.h>
#endif
#include <sqplus.h>

#include "wizpage.h"
#include "intropanel.h"
#include "projectpathpanel.h"
#include "compilerpanel.h"
#include "languagepanel.h"

// utility function to append a path separator to the
// string parameter, if needed.
wxString AppendPathSepIfNeeded(const wxString& path)
{
    if (path.Last() == _T('/') || path.Last() == _T('\\'))
        return path;
    return path + wxFILE_SEP_PATH;
}

////////////////////////////////////////////////////////////////////////////////
// WizPage
////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(WizPage, wxWizardPageSimple)
    EVT_BUTTON(-1, WizPage::OnButton)
    EVT_WIZARD_PAGE_CHANGING(-1, WizPage::OnPageChanging)
    EVT_WIZARD_PAGE_CHANGED(-1, WizPage::OnPageChanged)
END_EVENT_TABLE()

WizPage::WizPage(const wxString& panelName, wxWizard* parent, const wxBitmap& bitmap)
    : wxWizardPageSimple(parent, 0, 0, bitmap),
    m_PanelName(panelName)
{
    wxXmlResource::Get()->LoadPanel(this, m_PanelName);
}

//------------------------------------------------------------------------------
WizPage::~WizPage()
{
}

//------------------------------------------------------------------------------
void WizPage::OnButton(wxCommandEvent& event)
{
    wxWindow* win = FindWindowById(event.GetId(), this);
    if (!win)
    {
        LOGSTREAM << _T("Can't locate window with id ") << event.GetId() << _T("!\n");
        return;
    }
    try
    {
        wxString sig = _T("OnClick_") + win->GetName();
        SqPlus::SquirrelFunction<void>(cbU2C(sig))();
    }
    catch (SquirrelError& e)
    {
        cbMessageBox(cbC2U(e.desc), _("Script error"), wxICON_ERROR);
    }
}

//------------------------------------------------------------------------------
void WizPage::OnPageChanging(wxWizardEvent& event)
{
    try
    {
        wxString sig = _T("OnLeave_") + m_PanelName;
        bool allow = SqPlus::SquirrelFunction<bool>(cbU2C(sig))(event.GetDirection() != 0); // !=0 forward, ==0 backward
        if (!allow)
            event.Veto();
    }
    catch (SquirrelError& e)
    {
        cbMessageBox(cbC2U(e.desc), _("Script error"), wxICON_ERROR);
    }
}

//------------------------------------------------------------------------------
void WizPage::OnPageChanged(wxWizardEvent& event)
{
    try
    {
        wxString sig = _T("OnEnter_") + m_PanelName;
        SqPlus::SquirrelFunction<void>(cbU2C(sig))(event.GetDirection() != 0); // !=0 forward, ==0 backward
    }
    catch (SquirrelError& e)
    {
        cbMessageBox(cbC2U(e.desc), _("Script error"), wxICON_ERROR);
    }
}

////////////////////////////////////////////////////////////////////////////////
// WizIntroPanel
////////////////////////////////////////////////////////////////////////////////

WizIntroPanel::WizIntroPanel(const wxString& intro_msg, wxWizard* parent, const wxBitmap& bitmap)
    : wxWizardPageSimple(parent, 0, 0, bitmap)
{
    IntroPanel* pnl = new IntroPanel(this);
    pnl->SetIntroText(intro_msg);
}

//------------------------------------------------------------------------------
WizIntroPanel::~WizIntroPanel()
{
}

////////////////////////////////////////////////////////////////////////////////
// WizProjectPathPanel
////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(WizProjectPathPanel, wxWizardPageSimple)
    EVT_BUTTON(-1, WizProjectPathPanel::OnButton)
    EVT_WIZARD_PAGE_CHANGING(-1, WizProjectPathPanel::OnPageChanging)
    EVT_WIZARD_PAGE_CHANGED(-1, WizProjectPathPanel::OnPageChanged)
END_EVENT_TABLE()

WizProjectPathPanel::WizProjectPathPanel(wxWizard* parent, const wxBitmap& bitmap)
    : wxWizardPageSimple(parent, 0, 0, bitmap)
{
    m_pProjectPathPanel = new ProjectPathPanel(this);
}

//------------------------------------------------------------------------------
WizProjectPathPanel::~WizProjectPathPanel()
{
}

//------------------------------------------------------------------------------
wxString WizProjectPathPanel::GetPath()
{
    return AppendPathSepIfNeeded(m_pProjectPathPanel->GetPath());
}

//------------------------------------------------------------------------------
wxString WizProjectPathPanel::GetName()
{
    return m_pProjectPathPanel->GetName();
}

//------------------------------------------------------------------------------
wxString WizProjectPathPanel::GetFullFileName()
{
    return m_pProjectPathPanel->GetFullFileName();
}

//------------------------------------------------------------------------------
wxString WizProjectPathPanel::GetTitle()
{
    return m_pProjectPathPanel->GetTitle();
}

//------------------------------------------------------------------------------
void WizProjectPathPanel::OnButton(wxCommandEvent& event)
{
    wxString dir = m_pProjectPathPanel->GetPath();
    dir = ChooseDirectory(0, _("Please select the folder to create your project in"), dir, wxEmptyString, false, true);
    if (!dir.IsEmpty() && wxDirExists(dir))
        m_pProjectPathPanel->SetPath(dir);
}

//------------------------------------------------------------------------------
void WizProjectPathPanel::OnPageChanging(wxWizardEvent& event)
{
    if (event.GetDirection() != 0) // !=0 forward, ==0 backward
	{
	    wxString dir = m_pProjectPathPanel->GetPath();
	    wxString name = m_pProjectPathPanel->GetName();
	    wxString fullname = m_pProjectPathPanel->GetFullFileName();
	    wxString title = m_pProjectPathPanel->GetTitle();
//		if (!wxDirExists(dir))
//		{
//            cbMessageBox(_("Please select a valid path to create your project..."), _("Error"), wxICON_ERROR);
//            event.Veto();
//            return;
//		}
		if (title.IsEmpty())
		{
            cbMessageBox(_("Please select a title for your project..."), _("Error"), wxICON_ERROR);
            event.Veto();
            return;
		}
		if (name.IsEmpty())
		{
            cbMessageBox(_("Please select a name for your project..."), _("Error"), wxICON_ERROR);
            event.Veto();
            return;
		}
		if (wxFileExists(fullname))
		{
            if (cbMessageBox(_("A project with the same name already exists in the project folder.\n"
                        "Are you sure you want to use this directory (files may be OVERWRITTEN)?"),
                        _("Confirmation"),
                        wxICON_QUESTION | wxYES_NO) != wxID_YES)
            {
//                cbMessageBox(_("A project with the same name already exists in the project folder.\n"
//                            "Please select a different project name..."), _("Warning"), wxICON_WARNING);
                event.Veto();
                return;
            }
		}
        Manager::Get()->GetProjectManager()->SetDefaultPath(dir);
	}
}

//------------------------------------------------------------------------------
void WizProjectPathPanel::OnPageChanged(wxWizardEvent& event)
{
    if (event.GetDirection() != 0) // !=0 forward, ==0 backward
    {
        wxString dir = Manager::Get()->GetProjectManager()->GetDefaultPath();
        m_pProjectPathPanel->SetPath(dir);
    }
}

////////////////////////////////////////////////////////////////////////////////
// WizCompilerPanel
////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(WizCompilerPanel, wxWizardPageSimple)
    EVT_WIZARD_PAGE_CHANGING(-1, WizCompilerPanel::OnPageChanging)
END_EVENT_TABLE()

WizCompilerPanel::WizCompilerPanel(const wxString& compilerID, const wxString& validCompilerIDs, wxWizard* parent, const wxBitmap& bitmap,
                                    bool allowCompilerChange, bool allowConfigChange)
    : wxWizardPageSimple(parent, 0, 0, bitmap)
{
    m_pCompilerPanel = new CompilerPanel(this);

    wxArrayString valids = GetArrayFromString(validCompilerIDs, _T(";"), true);
    wxString def = compilerID;
    if (def.IsEmpty())
        def = CompilerFactory::GetDefaultCompiler()->GetName();
    int id = 0;
    wxComboBox* cmb = m_pCompilerPanel->GetCompilerCombo();
    cmb->Clear();
    for (size_t i = 0; i < CompilerFactory::GetCompilersCount(); ++i)
    {
        for (size_t n = 0; n < valids.GetCount(); ++n)
        {
            if (CompilerFactory::GetCompiler(i)->GetID().Matches(valids[n]))
            {
                cmb->Append(CompilerFactory::GetCompiler(i)->GetName());
                if (CompilerFactory::GetCompiler(i)->GetID().IsSameAs(def))
                    id = cmb->GetCount();
                break;
            }
        }
    }
    cmb->SetSelection(id);
    cmb->Enable(allowCompilerChange);
    m_pCompilerPanel->EnableConfigurationTargets(allowConfigChange);

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("scripts"));

    m_pCompilerPanel->SetWantDebug(cfg->ReadBool(_T("/generic_wizard/want_debug"), true));
    m_pCompilerPanel->SetDebugName(cfg->Read(_T("/generic_wizard/debug_name"), _T("Debug")));
    m_pCompilerPanel->SetDebugOutputDir(cfg->Read(_T("/generic_wizard/debug_output"), _T("bin") + wxString(wxFILE_SEP_PATH) + _T("Debug")));
    m_pCompilerPanel->SetDebugObjectOutputDir(cfg->Read(_T("/generic_wizard/debug_objects_output"), _T("obj") + wxString(wxFILE_SEP_PATH) + _T("Debug")));

    m_pCompilerPanel->SetWantRelease(cfg->ReadBool(_T("/generic_wizard/want_release"), true));
    m_pCompilerPanel->SetReleaseName(cfg->Read(_T("/generic_wizard/release_name"), _T("Release")));
    m_pCompilerPanel->SetReleaseOutputDir(cfg->Read(_T("/generic_wizard/release_output"), _T("bin") + wxString(wxFILE_SEP_PATH) + _T("Release")));
    m_pCompilerPanel->SetReleaseObjectOutputDir(cfg->Read(_T("/generic_wizard/release_objects_output"), _T("obj") + wxString(wxFILE_SEP_PATH) + _T("Release")));
}

//------------------------------------------------------------------------------
WizCompilerPanel::~WizCompilerPanel()
{
}

//------------------------------------------------------------------------------
wxString WizCompilerPanel::GetCompilerID()
{
    Compiler* compiler = CompilerFactory::GetCompilerByName(m_pCompilerPanel->GetCompilerCombo()->GetStringSelection());
    if (compiler)
        return compiler->GetID();
    return wxEmptyString;
}

//------------------------------------------------------------------------------
bool WizCompilerPanel::GetWantDebug()
{
    return m_pCompilerPanel->GetWantDebug();
}

//------------------------------------------------------------------------------
wxString WizCompilerPanel::GetDebugName()
{
    return m_pCompilerPanel->GetDebugName();
}

//------------------------------------------------------------------------------
wxString WizCompilerPanel::GetDebugOutputDir()
{
    return AppendPathSepIfNeeded(m_pCompilerPanel->GetDebugOutputDir());
}

//------------------------------------------------------------------------------
wxString WizCompilerPanel::GetDebugObjectOutputDir()
{
    return AppendPathSepIfNeeded(m_pCompilerPanel->GetDebugObjectOutputDir());
}

//------------------------------------------------------------------------------
bool WizCompilerPanel::GetWantRelease()
{
    return m_pCompilerPanel->GetWantRelease();
}

//------------------------------------------------------------------------------
wxString WizCompilerPanel::GetReleaseName()
{
    return m_pCompilerPanel->GetReleaseName();
}

//------------------------------------------------------------------------------
wxString WizCompilerPanel::GetReleaseOutputDir()
{
    return AppendPathSepIfNeeded(m_pCompilerPanel->GetReleaseOutputDir());
}

//------------------------------------------------------------------------------
wxString WizCompilerPanel::GetReleaseObjectOutputDir()
{
    return AppendPathSepIfNeeded(m_pCompilerPanel->GetReleaseObjectOutputDir());
}

//------------------------------------------------------------------------------
void WizCompilerPanel::OnPageChanging(wxWizardEvent& event)
{
    if (event.GetDirection() != 0) // !=0 forward, ==0 backward
	{
        if (GetCompilerID().IsEmpty())
        {
            wxMessageBox(_("You must select a compiler for your project..."), _("Error"), wxICON_ERROR);
            event.Veto();
            return;
        }
        if (!GetWantDebug() && !GetWantRelease())
        {
            wxMessageBox(_("You must select at least one configuration..."), _("Error"), wxICON_ERROR);
            event.Veto();
            return;
        }

        ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("scripts"));

        cfg->Write(_T("/generic_wizard/want_debug"), (bool)GetWantDebug());
        cfg->Write(_T("/generic_wizard/debug_name"), GetDebugName());
        cfg->Write(_T("/generic_wizard/debug_output"), GetDebugOutputDir());
        cfg->Write(_T("/generic_wizard/debug_objects_output"), GetDebugObjectOutputDir());

        cfg->Write(_T("/generic_wizard/want_release"), (bool)GetWantRelease());
        cfg->Write(_T("/generic_wizard/release_name"), GetReleaseName());
        cfg->Write(_T("/generic_wizard/release_output"), GetReleaseOutputDir());
        cfg->Write(_T("/generic_wizard/release_objects_output"), GetReleaseObjectOutputDir());
	}
}

////////////////////////////////////////////////////////////////////////////////
// WizLanguagePanel
////////////////////////////////////////////////////////////////////////////////

WizLanguagePanel::WizLanguagePanel(const wxArrayString& langs, int defLang, wxWizard* parent, const wxBitmap& bitmap)
    : wxWizardPageSimple(parent, 0, 0, bitmap)
{
    m_pLanguagePanel = new LanguagePanel(this);
    m_pLanguagePanel->SetChoices(langs, defLang);
}

//------------------------------------------------------------------------------
WizLanguagePanel::~WizLanguagePanel()
{
}

int WizLanguagePanel::GetLanguage()
{
    return m_pLanguagePanel->GetLanguage();
}

void WizLanguagePanel::SetLanguage(int lang)
{
    m_pLanguagePanel->SetLanguage(lang);
}
