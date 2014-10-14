#ifndef CODEBLOCKS_APP_H
#define CODEBLOCKS_APP_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/laywin.h>
    #include <wx/image.h>
    #include <wx/filename.h>
    #include <wx/hashmap.h>
    #include <wx/docview.h> // recent files history
#endif

#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
    #include "resources/icons/app.xpm"
#endif

#ifdef __WXMSW__
	#include <wx/ipc.h>
#endif

#include <wx/stc/stc.h>
#include <manager.h>
#include "main.h"

class CodeBlocksApp : public wxApp
{
    public:
        virtual bool OnInit();
        virtual int OnExit();
        wxString GetAppPath() const;
        int ParseCmdLine(MainFrame* handlerFrame);
		void OnAppActivate(wxActivateEvent& event);
#ifdef __WXMSW__
		void SetAssociations();
		void DoSetAssociation(const wxString& ext, const wxString& descr, const wxString& exe, const wxString& icoNum);
#endif
    private:
        bool CheckResource(const wxString& res) const;
		bool m_NoAssocs; // no associations check
		bool m_NoSplash; // no splash screen
		bool m_HasDebugLog; // display debug log
        DECLARE_EVENT_TABLE()
};
DECLARE_APP(CodeBlocksApp);

#ifdef __WXMSW__
class DDEServer : public wxServer
{
	public:
		DDEServer(MainFrame* frame) : m_Frame(frame) {}
		wxConnectionBase *OnAcceptConnection(const wxString& topic);
		MainFrame* GetFrame(){ return m_Frame; }
		void SetFrame(MainFrame* frame){ m_Frame = frame; }
	private:
		MainFrame* m_Frame;
};

class DDEConnection : public wxConnection
{
	public:
		DDEConnection(MainFrame* frame) : m_Frame(frame) {}
		bool OnExecute(const wxString& topic, wxChar *data, int size, wxIPCFormat format);
	private:
		MainFrame* m_Frame;
};
#endif


#endif // CODEBLOCKS_APP_H