#ifndef DEVPLUGIN_H
#define DEVPLUGIN_H

#include <wx/dynarray.h>
#include <wx/event.h>

#include "settings.h" // build settings
#include "globals.h"

#ifdef __WXMSW__
	#ifndef PLUGIN_EXPORT
		#ifdef EXPORT_LIB
			#define PLUGIN_EXPORT __declspec (dllexport)
		#else // !EXPORT_LIB
			#if BUILDING_PLUGIN
				#define PLUGIN_EXPORT __declspec (dllexport)
			#else // !BUILDING_PLUGIN
				#define PLUGIN_EXPORT __declspec (dllimport)
			#endif // BUILDING_PLUGIN
		#endif // EXPORT_LIB
	#endif // PLUGIN_EXPORT
#else
	#define PLUGIN_EXPORT
#endif

// class decls
class ProjectBuildTarget;
class wxMenuBar;
class wxMenu;
class wxToolBar;
class cbProject;

/** Information about the plugin */
struct PluginInfo
{
    wxString name;
    wxString title;
    wxString version;
    wxString description;
    wxString author;
    wxString authorEmail;
    wxString authorWebsite;
    wxString thanksTo;
    wxString license;
	bool hasConfigure;
};

/** @brief Base class for plugins
  * This is the most basic class a plugin must descend
  * from.
  * cbPlugin descends from wxEvtHandler, so it provides its methods as well...
  * \n \n
  * If a plugin has PluginInfo::hasConfigure set to true, this means that
  * the plugin provides a configuration dialog of some sort. In this case,
  * Code::Blocks creates a menu entry for the plugin, under the Settings
  * menu, to configure it. See Configure() for more information.
  */
class cbPlugin : public wxEvtHandler
{
    public:
		/** In default cbPlugin's constructor the associated PluginInfo structure
		  * is filled with default values. If you inherit from cbPlugin, you
		  * should fill the m_PluginInfo members with the appropriate values.
		  */
        cbPlugin();
		/** cbPlugin destructor. */
        virtual ~cbPlugin();
		/** Attach is <u>not</u> a virtual function, so you can't override it.
		  * The default implementation hooks the plugin to Code::Block's
		  * event handling system, so that the plugin can receive (and process)
		  * events from Code::Blocks core library. Use OnAttach() for any
		  * initialization specific tasks.
		  * @see OnAttach()
		  */
        void Attach();
		/** Release is <u>not</u> a virtual function, so you can't override it.
		  * The default implementation un-hooks the plugin from Code::Blocks's
		  * event handling system. Use OnRelease() for any clean-up specific
		  * tasks.
		  * @param appShutDown If true, the application is shutting down. In this
		  *         case *don't* use Manager::Get()->Get...() functions or the
		  *         behaviour is undefined...
		  * @see OnRelease()
		  */
        void Release(bool appShutDown);
		/** The plugin must return its type on request. */
        virtual PluginType GetType() const { return m_Type; }

		/** The plugin must return its info on request. */
        virtual PluginInfo const* GetInfo() const { return &m_PluginInfo; }
		/** This is a pure virtual method that should be overriden by all
		  * plugins. If a plugin provides some sort of configuration dialog,
		  * this is the place to invoke it. If it does not support/allow
		  * configuration, just return 0.
		  */
        virtual int Configure() = 0;
		/** This method is called by Code::Blocks and is used by the plugin
		  * to add any menu items it needs on Code::Blocks's menu bar.\n
		  * It is a pure virtual method that needs to be implemented by all
		  * plugins. If the plugin does not need to add items on the menu,
		  * just do nothing ;)
		  * @param menuBar the wxMenuBar to create items in
		  */
        virtual void BuildMenu(wxMenuBar* menuBar) = 0;
		/** This method is called by Code::Blocks core modules (EditorManager,
		  * ProjectManager etc) and is used by the plugin to add any menu
		  * items it needs in the module's popup menu. For example, when
		  * the user right-clicks on a project file in the project tree,
		  * ProjectManager prepares a popup menu to display with context
		  * sensitive options for that file. Before it displays this popup
		  * menu, it asks all attached plugins (by asking PluginManager to call
		  * this method), if they need to add any entries
		  * in that menu. This method is called.\n
		  * If the plugin does not need to add items in the menu,
		  * just do nothing ;)
		  * @param type the module that's preparing a popup menu
		  * @param menu pointer to the popup menu
		  * @param arg a wxString argument. In the example above, it would contain the selected project file
		  */
        virtual void BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg) = 0;
		/** This method is called by Code::Blocks and is used by the plugin
		  * to add any toolbar items it needs on Code::Blocks's toolbar.\n
		  * It is a pure virtual method that needs to be implemented by all
		  * plugins. If the plugin does not need to add items on the toolbar,
		  * just do nothing ;)
		  * @param toolBar the wxToolBar to create items on
		  * @return The plugin should return true if it needed the toolbar, false if not
		  */
        virtual bool BuildToolBar(wxToolBar* toolBar) = 0;
        /** See whether this plugin is attached or not. A plugin should not perform
		  * any of its tasks, if not attached...
		  */
		bool IsAttached() const { return m_IsAttached; }
    protected:
		/** Any descendent plugin should override this virtual method and
		  * perform any necessary initialization. This method is called by
		  * Code::Blocks (PluginManager actually) when the plugin has been
		  * loaded and should attach in Code::Blocks. When Code::Blocks
		  * starts up, it finds and <em>loads</em> all plugins but <em>does
		  * not</em> activate (attaches) them. It then activates all plugins
		  * that the user has selected to be activated on start-up.\n
		  * This means that a plugin might be loaded but <b>not</b> activated...\n
		  * Think of this method as the actual constructor...
		  */
        virtual void OnAttach(){}

		/** Any descendent plugin should override this virtual method and
		  * perform any necessary de-initialization. This method is called by
		  * Code::Blocks (PluginManager actually) when the plugin has been
		  * loaded, attached and should de-attach from Code::Blocks.\n
		  * Think of this method as the actual destructor...
		  * @param appShutDown If true, the application is shutting down. In this
		  *         case *don't* use Manager::Get()->Get...() functions or the
		  *         behaviour is undefined...
		  */
        virtual void OnRelease(bool appShutDown){}

		/** This method logs a "Not implemented" message and is provided for
		  * convenience only. For example, if the plugin *will* provide a
		  * configuration dialog, but it's not implemented yet, the author
		  * should create the Configure() method and put in there something
		  * like: NotImplemented("myCustomPlugin::OnConfigure"). This would
		  * log this message: "myCustomPlugin::OnConfigure : Not implemented"
		  * in the Code::Blocks debug log.
		  */
        virtual void NotImplemented(const wxString& log);
		/** Holds the PluginInfo structure that describes the plugin. */
        PluginInfo m_PluginInfo;
		/** Holds the plugin's type. Set in the default constructor. You shouldn't change it. */
        PluginType m_Type;
		/** Holds the "attached" state. */
        bool m_IsAttached;
	private:
};

/** @brief Base class for compiler plugins
  *
  * This plugin type must offer some pre-defined build facilities, on top
  * of the generic plugin's.
  */
class cbCompilerPlugin: public cbPlugin
{
	public:
		cbCompilerPlugin();
		/** @brief Run the project/target.
		  *
		  * Running a project means executing its build output. Of course
		  * this depends on the selected build target and its type.
		  *
		  * @param target The specific build target to "run". If NULL, the plugin
		  * should ask the user which target to "run" (except maybe if there is
		  * only one build target in the project).
		  */
        virtual int Run(ProjectBuildTarget* target = 0L) = 0;
		/** @brief Clean the project/target.
		  *
		  * Cleaning a project means deleting any files created by building it.
		  * This includes any object files, the binary output file, etc.
		  *
		  * @param target The specific build target to "clean". If NULL, it
		  * cleans all the build targets of the current project.
		  */
        virtual int Clean(ProjectBuildTarget* target = 0L) = 0;
		/** @brief Compile the project/target.
		  *
		  * @param target The specific build target to compile. If NULL, it
		  * compiles all the build targets of the current project.
		  */
        virtual int Compile(ProjectBuildTarget* target = 0L) = 0;
		/** @brief Rebuild the project/target.
		  *
		  * Rebuilding a project is equal to calling Clean() and then Compile().
		  * This makes sure that all compilable files in the project will be
		  * compiled again.
		  *
		  * @param target The specific build target to rebuild. If NULL, it
		  * rebuilds all the build targets of the current project.
		  */
        virtual int Rebuild(ProjectBuildTarget* target = 0L) = 0;
		/** @brief Compile all open projects, i.e. all targets in all projects. */
        virtual int CompileAll() = 0;
		/** @brief Rebuild all open projects, i.e. all targets in all projects. */
        virtual int RebuildAll() = 0;
        /** @brief Compile a specific file.
          *
          * @param file The file to compile (must be a project file!)
          */
        virtual int CompileFile(const wxString& file) = 0;
        /** @brief Abort the current build process. */
        virtual int KillProcess() = 0;
        /** @brief Is the plugin currently compiling? */
		virtual bool IsRunning() const = 0;
        /** @brief Get the exit code of the last build process. */
		virtual int GetExitCode() const = 0;
        /** @brief Display configuration dialog.
          *
          * The default implementation calls Configure(cbProject*,ProjectBuildTarget*).
          *
          * @see Configure(cbProject*,ProjectBuildTarget*)
          */
		virtual int Configure(){ return Configure(0L, 0L); }
        /** @brief Display configuration dialog.
          *
          * @param project The selected project (can be NULL).
          * @param target The selected target (can be NULL).
          */
		virtual int Configure(cbProject* project, ProjectBuildTarget* target = 0L) = 0;
	private:
};

/** @brief Base class for debugger plugins
  *
  * This plugin type must offer some pre-defined debug facilities, on top
  * of the generic plugin's.
  */
class cbDebuggerPlugin: public cbPlugin
{
	public:
		cbDebuggerPlugin();
		/** @brief Start a new debugging process. */
		virtual int Debug() = 0;
		/** @brief Continue running the debugged program. */
		virtual void CmdContinue() = 0;
		/** @brief Execute the next instruction and return control to the debugger. */
		virtual void CmdNext() = 0;
		/** @brief Execute the next instruction, stepping into function calls if needed, and return control to the debugger. */
		virtual void CmdStep() = 0;
		/** @brief Stop the debugging process. */
		virtual void CmdStop() = 0;
        /** @brief Is the plugin currently debugging? */
		virtual bool IsRunning() const = 0;
        /** @brief Get the exit code of the last debug process. */
		virtual int GetExitCode() const = 0;
};

/** @brief Base class for tool plugins
  *
  * This plugin is automatically managed by Code::Blocks, so the inherited
  * functions to build menus/toolbars are hidden.
  *
  * Tool plugins are automatically added under the "Plugins" menu. If they
  * provide a configuration dialog, they 're also added under the
  * "Settings/Configure plugins" menu.
  */
class cbToolPlugin : public cbPlugin
{
    public:
        cbToolPlugin();
        /** @brief Execute the plugin.
          *
          * This is the only function needed by a cbToolPlugin (and Configure() if needed).
          * This will be called when the user selects the plugin from the "Plugins"
          * menu.
          */
        virtual int Execute() = 0;
    private:
        // "Hide" some virtual members, that are not needed in cbToolPlugin
        void BuildMenu(wxMenuBar* menuBar){}
        void RemoveMenu(wxMenuBar* menuBar){}
        void BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg){}
        bool BuildToolBar(wxToolBar* toolBar){ return false; }
        void RemoveToolBar(wxToolBar* toolBar){}
};

/** @brief Base class for mime plugins
  *
  * Mime plugins are called by Code::Blocks to operate on files that Code::Blocks
  * wouldn't know how to handle on itself.
  */
class cbMimePlugin : public cbPlugin
{
    public:
        cbMimePlugin();
        /** @brief Can a file be handled by this plugin?
          *
          * @param filename The file in question.
          * @return The plugin should return true if it can handle this file,
          * false if not.
          */
        virtual bool CanHandleFile(const wxString& filename) const = 0;
        /** @brief Open the file.
          *
          * @param filename The file to open.
          * @return The plugin should return zero on success, other value on error.
          */
        virtual int OpenFile(const wxString& filename) = 0;
        /** @brief Is this a default handler?
          *
          * This is a flag notifying the main app that this plugin can handle
          * every file passed to it. Usually you 'll want to return false in
          * this function, because you usually create specialized handler
          * plugins (for specific MIME types)...
          *
          * @return True if this plugin can handle every possible MIME type,
          * false if not.
          */
        virtual bool HandlesEverything() const = 0;
    private:
        // "Hide" some virtual members, that are not needed in cbMimePlugin
        void BuildMenu(wxMenuBar* menuBar){}
        void RemoveMenu(wxMenuBar* menuBar){}
        void BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg){}
        bool BuildToolBar(wxToolBar* toolBar){ return false; }
        void RemoveToolBar(wxToolBar* toolBar){}
};

/** @brief Base class for code-completion plugins
  *
  * This interface is subject to change, so not much info here...
  */
class cbCodeCompletionPlugin : public cbPlugin
{
    public:
        cbCodeCompletionPlugin();
		virtual wxArrayString GetCallTips() = 0;
		virtual int CodeComplete() = 0;
		virtual void ShowCallTip() = 0;
};

/** @brief Base class for wizard plugins
  *
  * Wizard plugins are called by Code::Blocks when the user selects
  * "New project" on a template provided by the plugin.
  */
class cbProjectWizardPlugin : public cbPlugin
{
    public:
        cbProjectWizardPlugin();

        /// @return the template's title
        virtual const wxString& GetTitle() const = 0;
        /// @return the template's description
        virtual const wxString& GetDescription() const = 0;
        /// @return the template's category (GUI, Console, etc; free-form text)
        virtual const wxString& GetCategory() const = 0;
        /// @return the template's bitmap
        virtual const wxBitmap& GetBitmap() const = 0;
        /// When this is called, the wizard must get to work ;)
        virtual int Launch() = 0; // do your work ;)
    private:
        // "Hide" some virtual members, that are not needed in cbCreateWizardPlugin
        void BuildMenu(wxMenuBar* menuBar){}
        void RemoveMenu(wxMenuBar* menuBar){}
        void BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg){}
        bool BuildToolBar(wxToolBar* toolBar){ return false; }
        void RemoveToolBar(wxToolBar* toolBar){}
};

typedef cbPlugin*(*GetPluginProc)(void);
typedef int(*GetSDKVersionMajorProc)(void);
typedef int(*GetSDKVersionMinorProc)(void);

// this is the plugins SDK version number
// it will change when the plugins interface breaks
#define PLUGIN_SDK_VERSION_MAJOR 1
#define PLUGIN_SDK_VERSION_MINOR 2

/** This is used to declare the plugin's hooks.
  */
#define CB_DECLARE_PLUGIN() \
    extern "C" \
    { \
        PLUGIN_EXPORT cbPlugin* GetPlugin(); \
        PLUGIN_EXPORT int GetSDKVersionMajor(); \
        PLUGIN_EXPORT int GetSDKVersionMinor(); \
    }

/** This is used to actually implement the plugin's hooks.
  * @param name The plugin's name (class).
  */
#define CB_IMPLEMENT_PLUGIN(name) \
    cbPlugin* GetPlugin() { return new name; } \
    int GetSDKVersionMajor() { return PLUGIN_SDK_VERSION_MAJOR; } \
    int GetSDKVersionMinor() { return PLUGIN_SDK_VERSION_MINOR; } \

#endif // DEVPLUGIN_H
