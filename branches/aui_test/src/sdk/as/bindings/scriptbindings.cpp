#include <sdk_precomp.h>
#include <settings.h>
#include <manager.h>
#include <messagemanager.h>
#include <configmanager.h>
#include <editormanager.h>
#include <projectmanager.h>
#include <macrosmanager.h>
#include <cbproject.h>
#include <cbeditor.h>
#include "scriptbindings.h"
#include "sc_wxstring.h"
#include "sc_wxarraystring.h"
#include "sc_io.h"

#ifdef offsetof
    #undef offsetof
#endif
#define offsetof(T, M) ( reinterpret_cast <size_t> ( & reinterpret_cast <const volatile char &>(reinterpret_cast<T *> (1000)->M) ) - 1000u)

// In Code::Blocks nothing is refcounted.
// In order to use objects as handles (aka pointers), we must provide
// these two dummy functions.
#define ADD_DUMMY_REFCOUNT(engine,api) \
    engine->RegisterObjectBehaviour(#api, asBEHAVE_ADDREF, "void f()", asFUNCTION(DummyAddRef), asCALL_CDECL_OBJLAST); \
    engine->RegisterObjectBehaviour(#api, asBEHAVE_RELEASE, "void f()", asFUNCTION(DummyRelease), asCALL_CDECL_OBJLAST)

void DummyAddRef(cbProject& p){}
void DummyRelease(cbProject& p){}

//------------------------------------------------------------------------------
// Forwards
//------------------------------------------------------------------------------
void Register_ConfigManager(asIScriptEngine* engine);
void Register_Editor(asIScriptEngine* engine);
void Register_EditorManager(asIScriptEngine* engine);
void Register_ProjectFile(asIScriptEngine* engine);
void Register_ProjectBuildTarget(asIScriptEngine* engine);
void Register_Project(asIScriptEngine* engine);
void Register_ProjectManager(asIScriptEngine* engine);

//------------------------------------------------------------------------------
// Globals
//------------------------------------------------------------------------------
void gShowMessage(const wxString& msg){ wxMessageBox(msg, _("Script message")); }
void gShowMessageWarn(const wxString& msg){ wxMessageBox(msg, _("Script message"), wxICON_WARNING); }
void gShowMessageError(const wxString& msg){ wxMessageBox(msg, _("Script message"), wxICON_ERROR); }
void gShowMessageInfo(const wxString& msg){ wxMessageBox(msg, _("Script message"), wxICON_INFORMATION); }
void gDebugLog(const wxString& msg){ DBGLOG(msg); }
wxString gReplaceMacros(const wxString& buffer, bool envVarsToo)
{
    return Manager::Get()->GetMacrosManager()->ReplaceMacros(buffer, envVarsToo);
}

//------------------------------------------------------------------------------
// Actual registration
//------------------------------------------------------------------------------
void RegisterBindings(asIScriptEngine* engine)
{
    // register wx types in script
    Register_wxString(engine);
    Register_wxArrayString(engine);
    Register_IO(engine);

    // register types
    engine->RegisterObjectType("Editor", 0, asOBJ_CLASS);
    ADD_DUMMY_REFCOUNT(engine, Editor);
    engine->RegisterObjectType("ProjectFile", 0, asOBJ_CLASS);
    ADD_DUMMY_REFCOUNT(engine, ProjectFile);
    engine->RegisterObjectType("BuildTarget", 0, asOBJ_CLASS);
    ADD_DUMMY_REFCOUNT(engine, BuildTarget);
    engine->RegisterObjectType("Project", 0, asOBJ_CLASS);
    ADD_DUMMY_REFCOUNT(engine, Project);
    engine->RegisterObjectType("ProjectManagerClass", 0, asOBJ_CLASS);
    engine->RegisterObjectType("EditorManagerClass", 0, asOBJ_CLASS);
    engine->RegisterObjectType("ConfigManagerClass", 0, asOBJ_CLASS);

    // register member functions
    Register_ConfigManager(engine);
    Register_Editor(engine);
    Register_EditorManager(engine);
    Register_ProjectFile(engine);
    Register_ProjectBuildTarget(engine);
    Register_Project(engine);
    Register_ProjectManager(engine);

    // register global functions
    engine->RegisterGlobalFunction("void ShowMessage(const wxString& in)", asFUNCTION(gShowMessage), asCALL_CDECL);
    engine->RegisterGlobalFunction("void ShowWarning(const wxString& in)", asFUNCTION(gShowMessageWarn), asCALL_CDECL);
    engine->RegisterGlobalFunction("void ShowError(const wxString& in)", asFUNCTION(gShowMessageError), asCALL_CDECL);
    engine->RegisterGlobalFunction("void ShowInfo(const wxString& in)", asFUNCTION(gShowMessageInfo), asCALL_CDECL);
    engine->RegisterGlobalFunction("void Log(const wxString& in)", asFUNCTION(gDebugLog), asCALL_CDECL);
    engine->RegisterGlobalFunction("wxString ReplaceMacros(const wxString& in, bool)", asFUNCTION(gReplaceMacros), asCALL_CDECL);
}

//------------------------------------------------------------------------------
// ConfigManager
//------------------------------------------------------------------------------
void Register_ConfigManager(asIScriptEngine* engine)
{
    engine->RegisterObjectMethod("ConfigManagerClass", "wxString Read(const wxString& in,const wxString& in)", asMETHODPR(ConfigManager, Read, (const wxString&,const wxString&), wxString), asCALL_THISCALL);
    engine->RegisterObjectMethod("ConfigManagerClass", "int Read(const wxString& in,int)", asMETHODPR(ConfigManager, ReadInt, (const wxString&,int), int), asCALL_THISCALL);
    engine->RegisterObjectMethod("ConfigManagerClass", "bool Read(const wxString& in,bool)", asMETHODPR(ConfigManager, ReadBool, (const wxString&,bool), bool), asCALL_THISCALL);
    engine->RegisterObjectMethod("ConfigManagerClass", "double Read(const wxString& in,double)", asMETHODPR(ConfigManager, ReadDouble, (const wxString&,double), double), asCALL_THISCALL);
    engine->RegisterObjectMethod("ConfigManagerClass", "void Write(const wxString& in,int)", asMETHODPR(ConfigManager, Write, (const wxString&,int), void), asCALL_THISCALL);
    engine->RegisterObjectMethod("ConfigManagerClass", "void Write(const wxString& in,double)", asMETHODPR(ConfigManager, Write, (const wxString&,double), void), asCALL_THISCALL);
    engine->RegisterObjectMethod("ConfigManagerClass", "void Write(const wxString& in,bool)", asMETHODPR(ConfigManager, Write, (const wxString&,bool), void), asCALL_THISCALL);
    engine->RegisterObjectMethod("ConfigManagerClass", "void Write(const wxString& in,const wxString& in,bool)", asMETHODPR(ConfigManager, Write, (const wxString&,const wxString&,bool), void), asCALL_THISCALL);

    // actually bind EditorManager's instance
    engine->RegisterGlobalProperty("ConfigManagerClass ConfigManager", Manager::Get()->GetConfigManager(_T("volatile:scripting")));
}

//------------------------------------------------------------------------------
// EditorBase
//------------------------------------------------------------------------------
template <class T> void Register_EditorBase(asIScriptEngine* engine, const wxString& classname)
{
    engine->RegisterObjectMethod(_C(classname), "wxString& GetFilename()", asMETHOD(T, GetFilename), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetFilename(const wxString& in)", asMETHOD(T, SetFilename), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "wxString& GetShortName()", asMETHOD(T, GetShortName), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "bool GetModified()", asMETHOD(T, GetModified), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetModified(bool)", asMETHOD(T, SetModified), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "wxString& GetTitle()", asMETHOD(T, GetTitle), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetTitle(const wxString& in)", asMETHOD(T, SetTitle), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void Activate()", asMETHOD(T, Activate), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "bool Close()", asMETHOD(T, Close), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "bool Save()", asMETHOD(T, Save), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "bool IsBuiltinEditor()", asMETHOD(T, IsBuiltinEditor), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "bool ThereAreOthers()", asMETHOD(T, ThereAreOthers), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "bool Close()", asMETHOD(T, Close), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "bool Close()", asMETHOD(T, Close), asCALL_THISCALL);
}

//------------------------------------------------------------------------------
// cbEditor
//------------------------------------------------------------------------------
void Register_Editor(asIScriptEngine* engine)
{
    // add CompileTargetBase methods/properties
    Register_EditorBase<cbEditor>(engine, _T("Editor"));

    engine->RegisterObjectMethod("Editor", "void SetEditorTitle(const wxString& in)", asMETHOD(cbEditor, SetEditorTitle), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "ProjectFile@ GetProjectFile()", asMETHOD(cbEditor, GetProjectFile), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "bool Save()", asMETHOD(cbEditor, Save), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "bool SaveAs()", asMETHOD(cbEditor, SaveAs), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "void FoldAll()", asMETHOD(cbEditor, FoldAll), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "void UnfoldAll()", asMETHOD(cbEditor, UnfoldAll), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "void ToggleAllFolds()", asMETHOD(cbEditor, ToggleAllFolds), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "void FoldBlockFromLine(int)", asMETHOD(cbEditor, FoldBlockFromLine), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "void UnfoldBlockFromLine(int)", asMETHOD(cbEditor, UnfoldBlockFromLine), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "void ToggleFoldBlockFromLine(int)", asMETHOD(cbEditor, ToggleFoldBlockFromLine), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "wxString GetLineIndentString(int)", asMETHOD(cbEditor, GetLineIndentString), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "void Touch()", asMETHOD(cbEditor, Touch), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "bool Reload()", asMETHOD(cbEditor, Reload), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "void Print(bool,int)", asMETHOD(cbEditor, Print), asCALL_THISCALL);
}

//------------------------------------------------------------------------------
// EditorManager
//------------------------------------------------------------------------------
void Register_EditorManager(asIScriptEngine* engine)
{
    engine->RegisterObjectMethod("EditorManagerClass", "void Configure()", asMETHOD(EditorManager, Configure), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "Editor@ New()", asMETHOD(EditorManager, New), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "Editor@ Open(const wxString& in,int,ProjectFile@)", asMETHOD(EditorManager, Open), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "Editor@ IsBuiltinOpen(const wxString& in)", asMETHOD(EditorManager, IsBuiltinOpen), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "Editor@ GetBuiltinEditor(const wxString& in)", asMETHODPR(EditorManager, GetBuiltinEditor, (const wxString&), cbEditor*), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "Editor@ GetBuiltinEditor(int)", asMETHODPR(EditorManager, GetBuiltinEditor, (int), cbEditor*), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "Editor@ GetBuiltinActiveEditor()", asMETHOD(EditorManager, GetBuiltinActiveEditor), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "void ActivateNext()", asMETHOD(EditorManager, ActivateNext), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "void ActivatePrevious()", asMETHOD(EditorManager, ActivatePrevious), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "bool SwapActiveHeaderSource()", asMETHOD(EditorManager, SwapActiveHeaderSource), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "bool CloseActive()", asMETHOD(EditorManager, CloseActive), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "bool Close(const wxString& in)", asMETHODPR(EditorManager, Close, (const wxString&,bool), bool), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "bool Close(int)", asMETHODPR(EditorManager, Close, (int,bool), bool), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "bool CloseAll()", asMETHOD(EditorManager, CloseAll), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "bool Save(const wxString& in)", asMETHODPR(EditorManager, Save, (const wxString&), bool), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "bool Save(int)", asMETHODPR(EditorManager, Save, (int), bool), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "bool SaveActive()", asMETHOD(EditorManager, SaveActive), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "bool SaveAs(int)", asMETHOD(EditorManager, SaveAs), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "bool SaveActiveAs()", asMETHOD(EditorManager, SaveActiveAs), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "bool SaveAll()", asMETHOD(EditorManager, SaveAll), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "int ShowFindDialog(bool)", asMETHOD(EditorManager, ShowFindDialog), asCALL_THISCALL);

    // actually bind EditorManager's instance
    engine->RegisterGlobalProperty("EditorManagerClass EditorManager", Manager::Get()->GetEditorManager());
}

//------------------------------------------------------------------------------
// CompileOptionsBase
//------------------------------------------------------------------------------
template <class T> void Register_CompileOptionsBase(asIScriptEngine* engine, const wxString& classname)
{
    engine->RegisterObjectMethod(_C(classname), "void SetBuildConfiguration(int)", asMETHOD(T, SetBuildConfiguration), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetLinkerOptions(const wxArrayString& in)", asMETHOD(T, SetLinkerOptions), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetLinkLibs(const wxArrayString& in)", asMETHOD(T, SetLinkLibs), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetCompilerOptions(const wxArrayString& in)", asMETHOD(T, SetCompilerOptions), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetIncludeDirs(const wxArrayString& in)", asMETHOD(T, SetIncludeDirs), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetResourceIncludeDirs(const wxArrayString& in)", asMETHOD(T, SetResourceIncludeDirs), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetLibDirs(const wxArrayString& in)", asMETHOD(T, SetLibDirs), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetCommandsBeforeBuild(const wxArrayString& in)", asMETHOD(T, SetCommandsBeforeBuild), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetCommandsAfterBuild(const wxArrayString& in)", asMETHOD(T, SetCommandsAfterBuild), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "int& GetBuildConfiguration()", asMETHOD(T, GetBuildConfiguration), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "wxArrayString& GetLinkerOptions()", asMETHOD(T, GetLinkerOptions), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "wxArrayString& GetLinkLibs()", asMETHOD(T, GetLinkLibs), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "wxArrayString& GetCompilerOptions()", asMETHOD(T, GetCompilerOptions), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "wxArrayString& GetIncludeDirs()", asMETHOD(T, GetIncludeDirs), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "wxArrayString& GetResourceIncludeDirs()", asMETHOD(T, GetResourceIncludeDirs), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "wxArrayString& GetLibDirs()", asMETHOD(T, GetLibDirs), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "wxArrayString& GetCommandsBeforeBuild()", asMETHOD(T, GetCommandsBeforeBuild), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "wxArrayString& GetCommandsAfterBuild()", asMETHOD(T, GetCommandsAfterBuild), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "bool GetModified()", asMETHOD(T, GetModified), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetModified(bool)", asMETHOD(T, SetModified), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void AddLinkerOption(const wxString& in)", asMETHOD(T, AddLinkerOption), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void AddLinkLib(const wxString& in)", asMETHOD(T, AddLinkLib), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void AddCompilerOption(const wxString& in)", asMETHOD(T, AddCompilerOption), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void AddIncludeDir(const wxString& in)", asMETHOD(T, AddIncludeDir), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void AddResourceIncludeDir(const wxString& in)", asMETHOD(T, AddResourceIncludeDir), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void AddLibDir(const wxString& in)", asMETHOD(T, AddLibDir), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void AddCommandsBeforeBuild(const wxString& in)", asMETHOD(T, AddCommandsBeforeBuild), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void AddCommandsAfterBuild(const wxString& in)", asMETHOD(T, AddCommandsAfterBuild), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "bool GetAlwaysRunPreBuildSteps()", asMETHOD(T, GetAlwaysRunPreBuildSteps), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "bool GetAlwaysRunPostBuildSteps()", asMETHOD(T, GetAlwaysRunPostBuildSteps), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetAlwaysRunPreBuildSteps(bool)", asMETHOD(T, SetAlwaysRunPreBuildSteps), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetAlwaysRunPostBuildSteps(bool)", asMETHOD(T, SetAlwaysRunPostBuildSteps), asCALL_THISCALL);
}

//------------------------------------------------------------------------------
// CompileTargetBase
//------------------------------------------------------------------------------
template <class T> void Register_CompileTargetBase(asIScriptEngine* engine, const wxString& classname)
{
    // add CompileOptionsBase methods/properties
    Register_CompileOptionsBase<T>(engine, classname);

    engine->RegisterObjectMethod(_C(classname), "wxString& GetFilename()", asMETHOD(T, GetFilename), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "wxString& GetTitle()", asMETHOD(T, GetTitle), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetTitle(const wxString& in)", asMETHOD(T, SetTitle), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetOutputFilename(const wxString& in)", asMETHOD(T, SetOutputFilename), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetWorkingDir(const wxString& in)", asMETHOD(T, SetWorkingDir), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetObjectOutput(const wxString& in)", asMETHOD(T, SetObjectOutput), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetDepsOutput(const wxString& in)", asMETHOD(T, SetDepsOutput), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "int GetOptionRelation(int)", asMETHOD(T, GetOptionRelation), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetOptionRelation(int,int)", asMETHOD(T, SetOptionRelation), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "wxString GetWorkingDir()", asMETHOD(T, GetWorkingDir), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "wxString GetObjectOutput()", asMETHOD(T, GetObjectOutput), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "wxString GetDepsOutput()", asMETHOD(T, GetDepsOutput), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "wxString GetOutputFilename()", asMETHOD(T, GetOutputFilename), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "wxString SuggestOutputFilename()", asMETHOD(T, SuggestOutputFilename), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "wxString GetExecutableFilename()", asMETHOD(T, GetExecutableFilename), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "wxString GetDynamicLibFilename()", asMETHOD(T, GetDynamicLibFilename), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "wxString GetStaticLibFilename()", asMETHOD(T, GetStaticLibFilename), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "wxString GetBasePath()", asMETHOD(T, GetBasePath), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetTargetType(const int& in)", asMETHOD(T, SetTargetType), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "int& GetTargetType()", asMETHOD(T, GetTargetType), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "wxString& GetExecutionParameters()", asMETHOD(T, GetExecutionParameters), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetExecutionParameters(const wxString& in)", asMETHOD(T, SetExecutionParameters), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "wxString& GetHostApplication()", asMETHOD(T, GetHostApplication), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetHostApplication(const wxString& in)", asMETHOD(T, SetHostApplication), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "void SetCompilerIndex(int)", asMETHOD(T, SetCompilerIndex), asCALL_THISCALL);
    engine->RegisterObjectMethod(_C(classname), "int GetCompilerIndex()", asMETHOD(T, GetCompilerIndex), asCALL_THISCALL);
}

//------------------------------------------------------------------------------
// ProjectFile
//------------------------------------------------------------------------------
void Register_ProjectFile(asIScriptEngine* engine)
{
    engine->RegisterObjectMethod("ProjectFile", "void AddBuildTarget(const wxString& in)", asMETHOD(ProjectFile, AddBuildTarget), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectFile", "void RenameBuildTarget(const wxString& in, const wxString& in)", asMETHOD(ProjectFile, RenameBuildTarget), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectFile", "void RemoveBuildTarget(const wxString& in)", asMETHOD(ProjectFile, RemoveBuildTarget), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectFile", "wxString GetBaseName()", asMETHOD(ProjectFile, GetBaseName), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectFile", "wxString& GetObjName()", asMETHOD(ProjectFile, GetObjName), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectFile", "void SetObjName(const wxString& in)", asMETHOD(ProjectFile, SetObjName), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectFile", "Project@ GetParentProject()", asMETHOD(ProjectFile, GetParentProject), asCALL_THISCALL);

    engine->RegisterObjectProperty("ProjectFile", "wxString relativeFilename", offsetof(ProjectFile, relativeFilename));
    engine->RegisterObjectProperty("ProjectFile", "wxString relativeToCommonTopLevelPath", offsetof(ProjectFile, relativeToCommonTopLevelPath));
    engine->RegisterObjectProperty("ProjectFile", "bool compile", offsetof(ProjectFile, compile));
    engine->RegisterObjectProperty("ProjectFile", "bool link", offsetof(ProjectFile, link));
    engine->RegisterObjectProperty("ProjectFile", "uint16 weight", offsetof(ProjectFile, weight));
    engine->RegisterObjectProperty("ProjectFile", "wxString buildCommand", offsetof(ProjectFile, buildCommand));
    engine->RegisterObjectProperty("ProjectFile", "bool useCustomBuildCommand", offsetof(ProjectFile, useCustomBuildCommand));
    engine->RegisterObjectProperty("ProjectFile", "bool autoDeps", offsetof(ProjectFile, autoDeps));
    engine->RegisterObjectProperty("ProjectFile", "wxString customDeps", offsetof(ProjectFile, customDeps));
    engine->RegisterObjectProperty("ProjectFile", "wxString compilerVar", offsetof(ProjectFile, compilerVar));
}

//------------------------------------------------------------------------------
// ProjectBuildTarget
//------------------------------------------------------------------------------
void Register_ProjectBuildTarget(asIScriptEngine* engine)
{
    // add CompileTargetBase methods/properties
    Register_CompileTargetBase<ProjectBuildTarget>(engine, _T("BuildTarget"));

    engine->RegisterObjectMethod("BuildTarget", "Project@ GetParentProject()", asMETHOD(ProjectBuildTarget, GetParentProject), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "wxString GetFullTitle()", asMETHOD(ProjectBuildTarget, GetFullTitle), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "wxString& GetExternalDeps()", asMETHOD(ProjectBuildTarget, GetExternalDeps), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "void SetExternalDeps(const wxString& in)", asMETHOD(ProjectBuildTarget, SetExternalDeps), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "void SetAdditionalOutputFiles(const wxString& in)", asMETHOD(ProjectBuildTarget, SetAdditionalOutputFiles), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "wxString& GetAdditionalOutputFiles()", asMETHOD(ProjectBuildTarget, GetAdditionalOutputFiles), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "bool GetIncludeInTargetAll()", asMETHOD(ProjectBuildTarget, GetIncludeInTargetAll), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "void SetIncludeInTargetAll(bool)", asMETHOD(ProjectBuildTarget, SetIncludeInTargetAll), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "bool GetCreateDefFile()", asMETHOD(ProjectBuildTarget, GetCreateDefFile), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "void SetCreateDefFile(bool)", asMETHOD(ProjectBuildTarget, SetCreateDefFile), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "bool GetCreateStaticLib()", asMETHOD(ProjectBuildTarget, GetCreateStaticLib), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "void SetCreateStaticLib(bool)", asMETHOD(ProjectBuildTarget, SetCreateStaticLib), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "bool GetUseConsoleRunner()", asMETHOD(ProjectBuildTarget, GetUseConsoleRunner), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "void SetUseConsoleRunner(bool)", asMETHOD(ProjectBuildTarget, SetUseConsoleRunner), asCALL_THISCALL);
}

//------------------------------------------------------------------------------
// Project
//------------------------------------------------------------------------------
void Register_Project(asIScriptEngine* engine)
{
    // add CompileTargetBase methods/properties
    Register_CompileTargetBase<cbProject>(engine, _T("Project"));

    engine->RegisterObjectMethod("Project", "bool GetModified()", asMETHOD(cbProject, GetModified), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "void SetModified(bool)", asMETHOD(cbProject, SetModified), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "void SetMakefile(const wxString& in)", asMETHOD(cbProject, SetMakefile), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "wxString& GetMakefile()", asMETHOD(cbProject, GetMakefile), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "void SetMakefileCustom(bool)", asMETHOD(cbProject, SetMakefileCustom), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool IsMakefileCustom()", asMETHOD(cbProject, IsMakefileCustom), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool CloseAllFiles()", asMETHOD(cbProject, CloseAllFiles), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool SaveAllFiles()", asMETHOD(cbProject, SaveAllFiles), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool Save()", asMETHOD(cbProject, Save), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool SaveAs()", asMETHOD(cbProject, SaveAs), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool SaveLayout()", asMETHOD(cbProject, SaveLayout), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool LoadLayout()", asMETHOD(cbProject, LoadLayout), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool ShowOptions()", asMETHOD(cbProject, ShowOptions), asCALL_THISCALL);

    engine->RegisterObjectMethod("Project", "ProjectFile@ GetFile(int)", asMETHOD(cbProject, GetFile), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool RemoveFile(int)", asMETHOD(cbProject, RemoveFile), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "ProjectFile@ AddFile(int,const wxString& in,bool,bool,uint16)", asMETHODPR(cbProject, AddFile, (int,const wxString&,bool,bool,unsigned short int), ProjectFile*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "ProjectFile@ AddFile(const wxString& in,const wxString& in,bool,bool,uint16)", asMETHODPR(cbProject, AddFile, (const wxString&,const wxString&,bool,bool,unsigned short int), ProjectFile*), asCALL_THISCALL);

    engine->RegisterObjectMethod("Project", "int GetBuildTargetsCount()", asMETHOD(cbProject, GetBuildTargetsCount), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "BuildTarget@ GetBuildTarget(int)", asMETHODPR(cbProject, GetBuildTarget, (int), ProjectBuildTarget*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "BuildTarget@ GetBuildTarget(const wxString& in)", asMETHODPR(cbProject, GetBuildTarget, (const wxString&), ProjectBuildTarget*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "BuildTarget@ AddBuildTarget(const wxString& in)", asMETHOD(cbProject, AddBuildTarget), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool RenameBuildTarget(int,const wxString& in)", asMETHODPR(cbProject, RenameBuildTarget, (int, const wxString&), bool), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool RenameBuildTarget(const wxString& in,const wxString& in)", asMETHODPR(cbProject, RenameBuildTarget, (const wxString&, const wxString&), bool), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool RemoveBuildTarget(int)", asMETHODPR(cbProject, RemoveBuildTarget, (int), bool), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool RemoveBuildTarget(const wxString& in)", asMETHODPR(cbProject, RemoveBuildTarget, (const wxString&), bool), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool SetActiveBuildTarget(int)", asMETHOD(cbProject, SetActiveBuildTarget), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "int GetActiveBuildTarget()", asMETHOD(cbProject, GetActiveBuildTarget), asCALL_THISCALL);
}

//------------------------------------------------------------------------------
// ProjectManager
//------------------------------------------------------------------------------
void Register_ProjectManager(asIScriptEngine* engine)
{
    engine->RegisterObjectMethod("ProjectManagerClass", "Project@ GetActiveProject()", asMETHOD(ProjectManager, GetActiveProject), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "void SetProject(Project@, bool)", asMETHOD(ProjectManager, SetProject), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool LoadWorkspace(const wxString& in)", asMETHOD(ProjectManager, LoadWorkspace), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool SaveWorkspace()", asMETHOD(ProjectManager, SaveWorkspace), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool SaveWorkspaceAs(const wxString& in)", asMETHOD(ProjectManager, SaveWorkspaceAs), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool CloseWorkspaceAs()", asMETHOD(ProjectManager, CloseWorkspace), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "Project@ LoadProject(const wxString& in)", asMETHOD(ProjectManager, LoadProject), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool SaveProject(Project@)", asMETHOD(ProjectManager, SaveProject), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool SaveProjectAs(Project@)", asMETHOD(ProjectManager, SaveProjectAs), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool SaveActiveProject()", asMETHOD(ProjectManager, SaveActiveProject), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool SaveActiveProjectAs()", asMETHOD(ProjectManager, SaveActiveProjectAs), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool SaveAllProjects()", asMETHOD(ProjectManager, SaveAllProjects), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool CloseProject(Project@)", asMETHOD(ProjectManager, CloseProject), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool CloseActiveProject()", asMETHOD(ProjectManager, CloseActiveProject), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool CloseAllProjects()", asMETHOD(ProjectManager, CloseAllProjects), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "Project@ NewProject(const wxString& in)", asMETHOD(ProjectManager, NewProject), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "int AddFileToProject(const wxString& in, Project@, int)", asMETHODPR(ProjectManager, AddFileToProject, (const wxString&,cbProject*,int), int), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "int AskForBuildTargetIndex(Project@)", asMETHOD(ProjectManager, AskForBuildTargetIndex), asCALL_THISCALL);

    // actually bind ProjectManager's instance
    engine->RegisterGlobalProperty("ProjectManagerClass ProjectManager", Manager::Get()->GetProjectManager());
}