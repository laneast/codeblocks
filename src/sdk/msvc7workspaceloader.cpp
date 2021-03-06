#include "sdk_precomp.h"
#include <wx/stream.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/dynarray.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>

#include "msvc7workspaceloader.h"

#include "globals.h"
#include "importers_globals.h"
#include "manager.h"
#include "messagemanager.h"
#include "projectmanager.h"
#include "compilerfactory.h"
#include "compiler.h"

MSVC7WorkspaceLoader::MSVC7WorkspaceLoader()
{
	//ctor
}

MSVC7WorkspaceLoader::~MSVC7WorkspaceLoader()
{
	//dtor
}

bool MSVC7WorkspaceLoader::Open(const wxString& filename)
{
    bool askForCompiler = false;
    bool askForTargets = false;
    switch (wxMessageBox(_("Do you want the imported projects to use the default compiler?\n"
                        "(If you answer No, you will be asked for each and every project"
                        " which compiler to use...)"), _("Question"), wxICON_QUESTION | wxYES_NO | wxCANCEL))
    {
        case wxYES: askForCompiler = false; break;
        case wxNO: askForCompiler = true; break;
        case wxCANCEL: return false;
    }
    switch (wxMessageBox(_("Do you want to import all configurations (e.g. Debug/Release) from the "
                        "imported projects?\n"
                        "(If you answer No, you will be asked for each and every project"
                        " which configurations to import...)"), _("Question"), wxICON_QUESTION | wxYES_NO | wxCANCEL))
    {
        case wxYES: askForTargets = false; break;
        case wxNO: askForTargets = true; break;
        case wxCANCEL: return false;
    }

    wxFileInputStream file(filename);
    if (!file.Ok())
        return false; // error opening file???

    wxArrayString comps;
    wxTextInputStream input(file);

    // read "header"
    if (!file.Eof())
    {
        wxString line = input.ReadLine();
        if (line.IsEmpty())
        {
            Manager::Get()->GetMessageManager()->DebugLog(_("Unsupported format."));
            return false;
        }
        comps = GetArrayFromString(line, _T(","));
        line = comps[0];
        line.Trim(true);
        line.Trim(false);
        if (line != _T("Microsoft Visual Studio Solution File"))
        {
            Manager::Get()->GetMessageManager()->DebugLog(_("Unsupported format."));
            return false;
        }
        line = comps.GetCount() > 1 ? comps[1] : wxString(wxEmptyString);
        line.Trim(true);
        line.Trim(false);
        _version = line.AfterLast(' '); // want the version number
        if ((_version != _T("7.00")) && (_version != _T("8.00")))
            Manager::Get()->GetMessageManager()->DebugLog(_T("Version not recognized. Will try to parse though..."));
    }

    ImportersGlobals::UseDefaultCompiler = !askForCompiler;
    ImportersGlobals::ImportAllTargets = !askForTargets;

    int count = 0;
    wxArrayString keyvalue;
    cbProject* project = 0;
    wxString uuid;
    bool depSection = false;  // ProjectDependencies section?
    bool slnConfSection = false; // SolutionConfiguration section?
    bool projConfSection = false; // ProjectConfiguration section?
    bool global = false;  // global section or project section?
    wxFileName wfname = filename;
    wfname.Normalize();
    Manager::Get()->GetMessageManager()->DebugLog(_("Workspace dir: %s"), wfname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR).c_str());
    while (!file.Eof())
    {
        wxString line = input.ReadLine();
        line.Trim(true);
        line.Trim(false);

        if (line.StartsWith(_T("Project("))) {
            // example wanted line:
            //Project("{UUID of the solution}") = "project name to display", "project filename", "project UUID".
            // UUID type 4 for projects (i.e. random based), UUID type 1 for solutions (i.e. time+host based)
            keyvalue = GetArrayFromString(line, _T("="));
            if (keyvalue.GetCount() != 2) continue;
            // ignore keyvalue[0], i.e. solution UUID/GUID

            // the second part contains the project title and filename
            comps = GetArrayFromString(keyvalue[1], _T(","));
            if (comps.GetCount() < 3) continue;

            // read project title and trim quotes
            wxString prjTitle = comps[0];
            prjTitle.Trim(true);
            prjTitle.Trim(false);
            if (prjTitle.IsEmpty()) continue;
            if (prjTitle.GetChar(0) == _T('\"'))
            {
                prjTitle.Truncate(prjTitle.Length() - 1);
                prjTitle.Remove(0, 1);
            }

            // read project filename and trim quotes
            wxString prjFile = comps[1];
            prjFile.Trim(true);
            prjFile.Trim(false);
            if (prjFile.IsEmpty()) continue;
            if (prjFile.GetChar(0) == _T('\"'))
            {
                prjFile.Truncate(prjFile.Length() - 1);
                prjFile.Remove(0, 1);
            }

            // read project UUID, i.e. "{35AFBABB-DF05-43DE-91A7-BB828A874015}"
            uuid = comps[2];
            uuid.Replace(_T("\""), _T("")); // remove quotes

            ++count;
            wxFileName fname(UnixFilename(prjFile));
            fname.Normalize(wxPATH_NORM_ALL, wfname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR), wxPATH_NATIVE);
            Manager::Get()->GetMessageManager()->DebugLog(_("Found project '%s' in '%s'"), prjTitle.c_str(), fname.GetFullPath().c_str());
            project = Manager::Get()->GetProjectManager()->LoadProject(fname.GetFullPath());
            if (project) registerProject(uuid, project);
        }
        else if (line.StartsWith(_T("GlobalSection(ProjectDependencies)"))) {
        	depSection = true;
        	global = true;
        }
        else if (line.StartsWith(_T("ProjectSection(ProjectDependencies)"))) {
        	depSection = true;
        	global = false;
        }
        else if (line.StartsWith(_T("GlobalSection(ProjectConfiguration)"))) {
            projConfSection = true;
        }
        else if (line.StartsWith(_T("GlobalSection(SolutionConfiguration)"))) {
            slnConfSection = true;
        }
        else if (line.StartsWith(_T("EndGlobalSection")) || line.StartsWith(_T("EndProjectSection"))) {
        	depSection = false;
        	projConfSection = false;
        	slnConfSection = false;
        }
        else if (depSection) { // start reading a dependency
        	keyvalue = GetArrayFromString(line, _T("="));
        	if (keyvalue.GetCount() != 2) continue;
        	if (global) {
        	    // {31635C8-67BF-4808-A918-0FBF822771BD}.0 = {658BFA12-8417-49E5-872A-33F0973544DC}
              // i.e. project on the left of '=' depend on the project on the right
              keyvalue[0]= keyvalue[0].BeforeFirst(_T('.'));
              addDependency(keyvalue[0], keyvalue[1]);
          }
          else {
              // {F87429BF-4583-4A67-BD6F-6CA8AA27702A} = {F87429BF-4583-4A67-BD6F-6CA8AA27702A}
              // i.e. both uuid are the dependency
              addDependency(uuid, keyvalue[1]);
          }
        }
        else if (slnConfSection) {
            // either "Debug = Debug" in V8 or "ConfigName.0 = Debug" in V7
            // ignore every on the left of equal sign
            line = line.AfterLast('=');
            line.Trim(true);
            line.Trim(false);
            addWorkspaceConfiguration(line);
        }
        else if (projConfSection && line.StartsWith(_T("{"))) {
            // {X}.Debug TA.ActiveCfg = Debug TA|Win32     ---> match solution configuration to project configuration or just say what is the active config?
            // {X}.Debug TA.Build.0 = Debug TA|Win32       ---> we have to build (others are not build)
            keyvalue = GetArrayFromString(line, _T("="));
            wxArrayString key = GetArrayFromString(keyvalue[0], _T("."));
            wxArrayString value = GetArrayFromString(keyvalue[1], _T("|"));
            if (key[2] == _T("Build")) addConfigurationMatching(key[0], key[1], value[0]);
        }
    }

    updateProjects();
    ImportersGlobals::ResetDefaults();

    m_Title = wxFileName(filename).GetName() + _(" workspace");
    return count != 0;
}

bool MSVC7WorkspaceLoader::Save(const wxString& title, const wxString& filename)
{
    // no support for saving solution files (.sln) yet
    return false;
}

