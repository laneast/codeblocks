/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef MANAGER_H
#define MANAGER_H

#include <map>
#include <vector>

#ifndef WX_PRECOMP
#   ifdef __WXMSW__
#       include <wx/msw/wrapwin.h>  // Needed to prevent Yield define bug.
#   endif
#endif
#include <wx/event.h>
#include <wx/cmdline.h>

#include "settings.h"
#include "sdk_events.h"
#include "cbfunctor.h"

// forward decls
#ifndef CB_FOR_CONSOLE
class wxFrame;
class wxWindow;
#endif // #ifndef CB_FOR_CONSOLE
class ProjectManager;
#ifndef CB_FOR_CONSOLE
class EditorManager;
class DebuggerManager;
#endif // #ifndef CB_FOR_CONSOLE
class LogManager;
class PluginManager;
#ifndef CB_FOR_CONSOLE
class ToolsManager;
#endif // #ifndef CB_FOR_CONSOLE
class MacrosManager;
class PersonalityManager;
#ifndef CB_FOR_CONSOLE
class wxMenu;
class wxMenuBar;
class wxToolBar;
#endif // #ifndef CB_FOR_CONSOLE
class UserVariableManager;
class ScriptingManager;
class ConfigManager;
class FileManager;


class DLLIMPORT Manager
{
    wxFrame*               m_pAppWindow;
    static bool            m_AppShuttingDown;
    static bool            m_AppStartedUp;
    static bool            m_BlockYields;
    static bool            m_IsBatch;
    static wxCmdLineParser m_CmdLineParser;

     Manager();
    ~Manager();

    void OnMenu(wxCommandEvent& event);

public:
    static void SetAppStartedUp(bool app_started_up);
    static void SetAppShuttingDown(bool app_shutting_down);
    static void SetBatchBuild(bool is_batch);
    static bool IsBatchBuild() { return m_IsBatch; }
    /// Blocks/unblocks Manager::Yield(). Be careful when using it. Actually, do *not* use it ;)
    static void BlockYields(bool block);
    /// Whenever you need to call wxYield(), call Manager::Yield(). It's safer.
    static void Yield();
    static void ProcessPendingEvents();
    static void Shutdown();

    bool ProcessEvent(CodeBlocksEvent&       event);
    bool ProcessEvent(CodeBlocksDockEvent&   event);
    bool ProcessEvent(CodeBlocksLayoutEvent& event);
    bool ProcessEvent(CodeBlocksLogEvent&    event);


    /** Use Manager::Get() to get a pointer to its instance
     * Manager::Get() is guaranteed to never return an invalid pointer.
     */
    static Manager* Get();

    /** Never, EVER, call this function! It is the last function called on shutdown.... */
    static void Free();

#ifndef CB_FOR_CONSOLE
    wxFrame*  GetAppFrame()  const;
    wxWindow* GetAppWindow() const;
#endif // #ifndef CB_FOR_CONSOLE

    static bool IsAppShuttingDown();
    static bool IsAppStartedUp();

    /** Functions returning pointers to the respective sub-manager instances.
     * During application startup as well as during runtime, these functions will always return a valid pointer.
     * During application shutdown, these functions will continue to return a valid pointer until the requested manager shuts down.
     * From that point, the below functions will return null. If there is any chance that your code might execute
     * during application shutdown, you MUST check for a null pointer.
     * The one notable exception to this rule is ConfigManager, which has the same lifetime as Manager itself.
     *
     * The order of destruction is:
     * ----------------------------
     *   ToolsManager,       TemplateManager, PluginManager,
     *   ScriptingManager,   ProjectManager,  EditorManager,
     *   PersonalityManager, MacrosManager,   UserVariableManager,
     *   LogManager
     *   The ConfigManager is destroyed immediately before the application terminates, so it can be
     *   considered being omnipresent.
     *
     * For plugin developers, this means that most managers (except for the ones you probably don't use anyway)
     * will be available throughout the entire lifetime of your plugins.
     */

    ProjectManager*      GetProjectManager()                          const;
#ifndef CB_FOR_CONSOLE
    EditorManager*       GetEditorManager()                           const;
#endif // #ifndef CB_FOR_CONSOLE
    LogManager*          GetLogManager()                              const;
    PluginManager*       GetPluginManager()                           const;
#ifndef CB_FOR_CONSOLE
    ToolsManager*        GetToolsManager()                            const;
#endif // #ifndef CB_FOR_CONSOLE
    MacrosManager*       GetMacrosManager()                           const;
    PersonalityManager*  GetPersonalityManager()                      const;
    UserVariableManager* GetUserVariableManager()                     const;
    ScriptingManager*    GetScriptingManager()                        const;
    ConfigManager*       GetConfigManager(const wxString& name_space) const;
    FileManager*         GetFileManager()                             const;
#ifndef CB_FOR_CONSOLE
    DebuggerManager*     GetDebuggerManager()                         const;
#endif // #ifndef CB_FOR_CONSOLE



    /////// XML Resource functions ///////
    /// Inits XML Resource system
    static void InitXRC(bool force=false);
    /// Loads XRC file(s) using data_path
    static void LoadXRC(wxString relpath);
    static bool LoadResource(const wxString& file);

#ifndef CB_FOR_CONSOLE
    /// Loads Menubar from XRC
    static wxMenuBar* LoadMenuBar(wxString resid, bool createonfailure = false);
    /// Loads Menu from XRC
    static wxMenu*    LoadMenu(wxString menu_id, bool createonfailure = false);
    /// Loads ToolBar from XRC
    static wxToolBar* LoadToolBar(wxFrame *parent, wxString resid, bool defaultsmall = true);

    // Do not use this, use Get()
    static Manager* Get(wxFrame* appWindow);

    wxToolBar* CreateEmptyToolbar();
    static void AddonToolBar(wxToolBar* toolBar,wxString resid);
    static bool isToolBar16x16(wxToolBar* toolBar);
#endif // #ifndef CB_FOR_CONSOLE

    static wxCmdLineParser* GetCmdLineParser();

    // event sinks
    void RegisterEventSink(wxEventType eventType, IEventFunctorBase<CodeBlocksEvent>*       functor);
    void RegisterEventSink(wxEventType eventType, IEventFunctorBase<CodeBlocksDockEvent>*   functor);
    void RegisterEventSink(wxEventType eventType, IEventFunctorBase<CodeBlocksLayoutEvent>* functor);
    void RegisterEventSink(wxEventType eventType, IEventFunctorBase<CodeBlocksLogEvent>*    functor);
    void RemoveAllEventSinksFor(void* owner);

private:
    // event sinks
    typedef std::vector< IEventFunctorBase<CodeBlocksEvent>* >       EventSinksArray;
    typedef std::map< wxEventType, EventSinksArray >                 EventSinksMap;
    typedef std::vector< IEventFunctorBase<CodeBlocksDockEvent>* >   DockEventSinksArray;
    typedef std::map< wxEventType, DockEventSinksArray >             DockEventSinksMap;
    typedef std::vector< IEventFunctorBase<CodeBlocksLayoutEvent>* > LayoutEventSinksArray;
    typedef std::map< wxEventType, LayoutEventSinksArray >           LayoutEventSinksMap;
    typedef std::vector< IEventFunctorBase<CodeBlocksLogEvent>* >    LogEventSinksArray;
    typedef std::map< wxEventType, LogEventSinksArray >              LogEventSinksMap;

    EventSinksMap       m_EventSinks;
    DockEventSinksMap   m_DockEventSinks;
    LayoutEventSinksMap m_LayoutEventSinks;
    LogEventSinksMap    m_LogEventSinks;
};

template <class MgrT> class Mgr
{
    static MgrT *instance;
    static bool isShutdown;
    explicit Mgr(const Mgr<MgrT>&)         { ; };
    Mgr<MgrT>& operator=(Mgr<MgrT> const&) { ; };

protected:

    Mgr()          { assert(Mgr<MgrT>::instance == 0); }
    virtual ~Mgr() { Mgr<MgrT>::instance = 0; }

public:

    static inline bool Valid() { return instance; }

    static inline MgrT* Get()
    {
        if (instance == 0 && isShutdown == false)
            instance = new MgrT();

        return instance;
    }

    static void Free()
    {
        isShutdown = true;
        delete instance;
        instance = 0;
    }
};

#endif // MANAGER_H