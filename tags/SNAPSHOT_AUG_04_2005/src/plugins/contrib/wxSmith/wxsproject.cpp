#include "wxsproject.h"

#include "widget.h"
#include "defwidgets/wxsstdmanager.h"
#include "defwidgets/wxsdialog.h"
#include "resources/wxsdialogres.h"
#include "wxswidgetfactory.h"
#include "wxsmith.h"
#include <wx/string.h>

#define XML_DIALOG_STR   "dialog"
#define XML_FRAME_STR    "frame"
#define XML_PANEL_STR    "panel"
#define XML_FNAME_STR    "xrc_file"
#define XML_CNAME_STR    "class"
#define XML_SFILE_STR    "src_file"
#define XML_HFILE_STR    "header_file"


wxsProject::wxsProject():  Integration(NotBinded), Project(NULL), DuringClear(false)
{}

wxsProject::~wxsProject()
{
    Clear();
}

wxsProject::IntegrationState wxsProject::BindProject(cbProject* Proj)
{

    Clear();

    /* creating new node in resource tree */
    
    wxTreeCtrl* ResTree = wxSmith::Get()->GetResourceTree();
    
    TreeItem = ResTree->AppendItem(ResTree->GetRootItem(),Proj->GetTitle());
    
    /* Binding project object */
    if ( Proj && !Proj->IsLoaded() )
    {
        Proj = NULL;
    }
    Project = Proj;
    if ( !Proj )
    {
        return Integration = NotBinded;
    }

    /* Checkign association of C::B project with wxS project */

    ProjectPath.Assign(Proj->GetFilename());
    WorkingPath = ProjectPath;
    WorkingPath.AppendDir(wxSmithSubDirectory);
    WorkingPath.SetName(wxSmithMainConfigFile);
    WorkingPath.SetExt(wxT(""));
    WorkingPath.Assign(WorkingPath.GetFullPath());  // Reparsing path

    if ( ! WorkingPath.FileExists() )
    {
        return Integration = NotWxsProject;
    }

    /* Trying to read configuration data */

    TiXmlDocument Doc(WorkingPath.GetFullPath());

    if ( !Doc.LoadFile() )
    {
        return Integration = NotWxsProject;
    }

    TiXmlNode* MainNode = Doc.FirstChild("wxsmith");

    if ( MainNode == NULL || ! LoadFromXml(MainNode) )
    {
        return Integration = NotWxsProject;
    }

    BuildTree(ResTree,TreeItem);
    
    return Integration = Integrated;
}

inline void wxsProject::Clear()
{
    DuringClear = true;
    
    if ( Project ) wxSmith::Get()->GetResourceTree()->Delete(TreeItem);
    
    Integration = NotBinded;
    Project = NULL;
    WorkingPath.Clear();
    ProjectPath.Clear();

    for ( DialogListI i = Dialogs.begin(); i!=Dialogs.end(); ++i )
    {
        if ( *i )
        {
            delete *i;
            *i = NULL;
        }
    }

    for ( FrameListI i = Frames.begin(); i!=Frames.end(); ++i )
    {
        // TODO (SpOoN#1#): Uncommend when frames done
        //delete *i;
    }

    for ( PanelListI i = Panels.begin(); i!=Panels.end(); ++i )
    {
        // TODO (SpOoN#1#): Uncomment when panel done
        //delete *i;
    }

    Dialogs.clear();
    Frames.clear();
    Panels.clear();
    wxSmith::Get()->GetResourceTree()->Refresh();
    DuringClear = false;
}

void wxsProject::BuildTree(wxTreeCtrl* Tree,wxTreeItemId WhereToAdd)
{
    DialogId = Tree->AppendItem(WhereToAdd,"Dialog resources");
    FrameId  = Tree->AppendItem(WhereToAdd,"Frame resources");
    PanelId  = Tree->AppendItem(WhereToAdd,"Panel resources");

    for ( DialogListI i = Dialogs.begin(); i!=Dialogs.end(); ++i )
    {
        (*i)->GetDialog().BuildTree(
            Tree,
            Tree->AppendItem(
                DialogId,
                (*i)->GetClassName(),
                -1, -1,
                new wxsResourceTreeData(*i) ) );
    }

    Tree->Refresh();
}

void wxsProject::DumpXml(const TiXmlNode* Elem,wxTreeCtrl* Tree,wxTreeItemId id)
{
    while ( Elem )
    {
        wxTreeItemId NewId = Tree->AppendItem(id,Elem->Value());
        // Dumping attributes
        TiXmlElement* ToElem = Elem->ToElement();
        TiXmlAttribute* Attr = ToElem ? ToElem->FirstAttribute() : NULL;
        if ( Attr )
        {
            while ( Attr )
            {
                wxString Str = Attr->Name() + wxString(wxT(" = ")) + Attr->Value();
                Tree->AppendItem(NewId,Str);
                Attr = Attr->Next();
            }
            Tree->AppendItem(NewId,wxT("-----------------------"));
        }
        DumpXml(Elem->FirstChildElement(),Tree,NewId);
        Elem = Elem->NextSiblingElement();
    }
}

bool wxsProject::LoadFromXml(TiXmlNode* MainNode)
{
    TiXmlElement* Elem;

    // Loading dialog resources

    for ( Elem = MainNode->FirstChildElement(XML_DIALOG_STR);
            Elem;
            Elem = Elem->NextSiblingElement(XML_DIALOG_STR) )
    {
        AddDialogResource(
            Elem->Attribute(XML_FNAME_STR),
            Elem->Attribute(XML_CNAME_STR),
            Elem->Attribute(XML_SFILE_STR),
            Elem->Attribute(XML_HFILE_STR) );
    }

    // Loading frame resources

    for ( Elem = MainNode->FirstChildElement(XML_FRAME_STR);
            Elem;
            Elem = Elem->NextSiblingElement(XML_FRAME_STR) )
    {
        AddFrameResource(
            Elem->Attribute(XML_FNAME_STR),
            Elem->Attribute(XML_CNAME_STR),
            Elem->Attribute(XML_SFILE_STR),
            Elem->Attribute(XML_HFILE_STR) );
    }

    // Loading panel resources

    for ( Elem = MainNode->FirstChildElement(XML_PANEL_STR);
            Elem;
            Elem = Elem->NextSiblingElement(XML_PANEL_STR) )
    {
        AddPanelResource(
            Elem->Attribute(XML_FNAME_STR),
            Elem->Attribute(XML_CNAME_STR),
            Elem->Attribute(XML_SFILE_STR),
            Elem->Attribute(XML_HFILE_STR) );
    }
    
    return true;
}

void wxsProject::AddDialogResource(
    const char* FileName,
    const char* ClassName,
    const char* SourceName,
    const char* HeaderName)
{
    if ( !FileName   || !*FileName   || !ClassName  || !*ClassName ||
         !SourceName || !*SourceName || !HeaderName || !*HeaderName )
        return;
        
    if ( !CheckProjFileExists(SourceName) )
    {
        Manager::Get()->GetMessageManager()->Log("Couldn't find source file '%s'",SourceName);
        Manager::Get()->GetMessageManager()->Log("Not all resources will be loaded");
        return;
    }
    
    if ( !CheckProjFileExists(HeaderName) )
    {
        Manager::Get()->GetMessageManager()->Log("Couldn't find header file '%s'",HeaderName);
        Manager::Get()->GetMessageManager()->Log("Not all resources will be loaded");
        return;
    }

    /* Opening xrc data */

    wxFileName Name;
    Name.Assign(WorkingPath.GetPath(),wxString(FileName));
    
    TiXmlDocument Doc(Name.GetFullPath());
    TiXmlElement* Resource;
    
    if ( !  Doc.LoadFile() ||
         ! (Resource = Doc.FirstChildElement("resource")) )
    {
        Manager::Get()->GetMessageManager()->Log("Couldn't load xrc data");
        return;
    }
    
    /* Finding dialog object */
    
    TiXmlElement* Dialog = Resource->FirstChildElement("object");
    while ( Dialog )
    {
        if ( !strcmp(Dialog->Attribute("class"),"wxDialog") &&
             !strcmp(Dialog->Attribute("name"),ClassName) )
        {
            break;
        }
        
        Dialog = Dialog->NextSiblingElement("object");
    }
    
    if ( !Dialog ) return;
    
    /* Creating dialog */

    wxsDialogRes* Res = new wxsDialogRes(this,ClassName,FileName,SourceName,HeaderName);

    
    if ( ! (Res->GetDialog().XmlLoad(Dialog))  )
    {
        Manager::Get()->GetMessageManager()->Log("Couldn't load xrc data");
        delete Res;
        return;
    }
    
    Dialogs.push_back(Res);
}

void wxsProject::AddFrameResource( const char* FileName, const char* ClassName, const char* SourceName, const char* HeaderName)
{
    if ( !FileName   || !*FileName   || !ClassName  || !*ClassName ||
         !SourceName || !*SourceName || !HeaderName || !*HeaderName )
        return;

// TODO (SpOoN#1#): Implement frames
    assert(!"Not implemented yet - this is not an official version\n");
}

void wxsProject::AddPanelResource(const char* FileName, const char* ClassName, const char* SourceName, const char* HeaderName)
{
    if ( !FileName   || !*FileName   || !ClassName  || !*ClassName ||
            !SourceName || !*SourceName || !HeaderName || !*HeaderName )
        return;
        
// TODO (SpOoN#1#): Implement panels
    assert(!"Not implemented yet - this is not an official version\n");
}

bool wxsProject::CheckProjFileExists(const char* FileName)
{
    if ( !Project ) return false;
    return Project->GetFileByFilename(FileName) != NULL;
}

TiXmlDocument* wxsProject::GenerateXml()
{
    if ( !Project ) return NULL;
    if ( Integration != Integrated ) return NULL;
    
    TiXmlDocument* Doc = new TiXmlDocument();
    TiXmlNode* Elem = Doc->InsertEndChild(TiXmlElement("wxsmith"));
    
    for ( DialogListI i = Dialogs.begin(); i!=Dialogs.end(); ++i )
    {
        TiXmlElement Dlg(XML_DIALOG_STR);
        wxsDialogRes* Sett = *i;
        Dlg.SetAttribute(XML_FNAME_STR,Sett->GetXrcFile());
        Dlg.SetAttribute(XML_CNAME_STR,Sett->GetClassName());
        Dlg.SetAttribute(XML_SFILE_STR,Sett->GetSourceFile());
        Dlg.SetAttribute(XML_HFILE_STR,Sett->GetHeaderFile());
        Elem->InsertEndChild(Dlg);
    }
    
    return Doc;
}

void wxsProject::SaveProject()
{
    
    if ( Integration != Integrated ) return;
    
    WorkingPath.SetName(wxSmithMainConfigFile);
    WorkingPath.SetExt(wxT(""));
    WorkingPath.Assign(WorkingPath.GetFullPath());  // Reparsing path

    TiXmlDocument* Doc = GenerateXml();

    if ( Doc )
    {
        Doc->SaveFile(WorkingPath.GetFullPath());
        delete Doc;
    }

    for ( DialogListI i = Dialogs.begin(); i!=Dialogs.end(); ++i )
    {
        (*i)->Save();
    }
}

void wxsProject::DeleteDialog(wxsDialogRes* Resource)
{
    if ( DuringClear ) return;
    
    DialogListI i;
    for ( i=Dialogs.begin(); i!=Dialogs.end(); ++i ) if ( *i == Resource ) break;
    
    if ( i == Dialogs.end() ) return;
    
    Dialogs.erase(i);
}

wxString wxsProject::GetInternalFileName(const wxString& FileName)
{
	wxFileName Path = WorkingPath;
    Path.SetName(FileName);
    Path.SetExt(wxT(""));
    Path.Assign(Path.GetFullPath());  // Reparsing path
    return Path.GetFullPath();
}

wxString wxsProject::GetProjectFileName(const wxString& FileName)
{
	wxFileName Path = ProjectPath;
    Path.SetName(FileName);
    Path.SetExt(wxT(""));
    Path.Assign(Path.GetFullPath());
    return Path.GetFullPath();
}

bool wxsProject::AddSmithConfig()
{
    if ( GetIntegration() != NotWxsProject ) return false;
    
    if ( ! wxFileName::Mkdir(WorkingPath.GetPath(wxPATH_GET_VOLUME),0744,wxPATH_MKDIR_FULL) )
    {
        wxMessageBox(wxT("Couldn't create wxsmith directory in main projet's path"),wxT("Error"),wxOK|wxICON_ERROR);
        return false;
    }
    
    Integration = Integrated;
    
    SaveProject();
    
    BuildTree(wxSmith::Get()->GetResourceTree(),TreeItem);
    
    return true;
}

void wxsProject::AddDialog(wxsDialogRes* Dialog)
{
    if ( !Dialog ) return;
    Dialogs.push_back(Dialog);
    wxTreeCtrl* Tree = wxSmith::Get()->GetResourceTree();
    BuildTree(Tree, Tree->AppendItem( DialogId, Dialog->GetClassName(), -1, -1, new wxsResourceTreeData(Dialog) ) );
}

wxsResource* wxsProject::FindResource(const wxString& Name)
{
    for ( DialogListI i = Dialogs.begin(); i!=Dialogs.end(); ++i )
    {
        if ( (*i)->GetResourceName() == Name ) return *i;
    }

    for ( FrameListI i = Frames.begin(); i!=Frames.end(); ++i )
    {
        // TODO (SpOoN#1#): Uncommend when frames done
        //delete *i;
    }

    for ( PanelListI i = Panels.begin(); i!=Panels.end(); ++i )
    {
        // TODO (SpOoN#1#): Uncomment when panel done
        //delete *i;
    }
    
    return NULL;
    
}