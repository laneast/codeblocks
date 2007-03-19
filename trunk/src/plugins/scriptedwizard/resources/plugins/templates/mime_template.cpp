#include <sdk.h> // Code::Blocks SDK
#include <configurationpanel.h>
#include "[HEADER_FILENAME]"

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<[PLUGIN_NAME]> reg(_T("[PLUGIN_NAME]"));
}

// constructor
[PLUGIN_NAME]::[PLUGIN_NAME]()
{
    // Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
    if(!Manager::LoadResource(_T("[PLUGIN_NAME].zip")))
    {
        NotifyMissingFile(_T("[PLUGIN_NAME].zip"));
    }
}

// destructor
[PLUGIN_NAME]::~[PLUGIN_NAME]()
{
}

void [PLUGIN_NAME]::OnAttach()
{
	// do whatever initialization you need for your plugin
	// NOTE: after this function, the inherited member variable
	// m_IsAttached will be TRUE...
	// You should check for it in other functions, because if it
	// is FALSE, it means that the application did *not* "load"
	// (see: does not need) this plugin...
}

void [PLUGIN_NAME]::OnRelease(bool appShutDown)
{
	// do de-initialization for your plugin
	// if appShutDown is true, the plugin is unloaded because Code::Blocks is being shut down,
	// which means you must not use any of the SDK Managers
	// NOTE: after this function, the inherited member variable
	// m_IsAttached will be FALSE...
}
[IF HAS_CONFIGURE]
int [PLUGIN_NAME]::Configure()
{
	//create and display the configuration dialog for your plugin
	cbConfigurationDialog dlg(Manager::Get()->GetAppWindow(), wxID_ANY, _("Your dialog title"));
	cbConfigurationPanel* panel = GetConfigurationPanel(&dlg);
	if (panel)
	{
		dlg.AttachConfigurationPanel(panel);
		PlaceWindow(&dlg);
		return dlg.ShowModal() == wxID_OK ? 0 : -1;
	}
	return -1;
}
[ENDIF HAS_CONFIGURE]
bool [PLUGIN_NAME]::CanHandleFile(const wxString& filename) const
{
	NotImplemented(_T("[PLUGIN_NAME]::CanHandleFile()"));
	return false;
}

int [PLUGIN_NAME]::OpenFile(const wxString& filename)
{
	NotImplemented(_T("[PLUGIN_NAME]::OpenFile()"));
	return -1;
}

bool [PLUGIN_NAME]::HandlesEverything() const
{
	NotImplemented(_T("[PLUGIN_NAME]::HandlesEverything()"));
	return false;
}

