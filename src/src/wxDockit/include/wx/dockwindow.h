///////////////////////////////////////////////////////////////////////////////
// Name:        wx/dockwindow.h
// Purpose:     wxDockWindowBase class
// Author:      Mark McCormack
// Modified by:
// Created:     23/02/04
// RCS-ID:
// Copyright:
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DOCKWINDOW_H_BASE_
#define _WX_DOCKWINDOW_H_BASE_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/defs.h>
#include <wx/dockit_defs.h>
#include <wx/minifram.h>
#include <wx/layoutmanager.h>

class wxDockPanel;
class wxBoxSizer;
class wxPanel;

// ----------------------------------------------------------------------------
// DockRect
// ----------------------------------------------------------------------------

struct DockRect {
    DockRect() {
        Reset();
    }
    int operator =(wxRect r) {
        rect = r;
        valid = true;
        return 0;   // XXX: this can't be OK - can it?
    }
    void Reset() {
        valid = false;
        rect.SetPosition( wxPoint( 0, 0 ) );
        rect.SetSize( wxSize( 0, 0 ) );
    }
    bool valid;
    wxRect rect;
};

// ----------------------------------------------------------------------------
// wxDockWindow interface is defined by the class wxDockWindowBase
// ----------------------------------------------------------------------------

#define wxDWC_NO_CONTROLS   0x0001
#define wxDWC_DEFAULT       0x0000

class WXDOCKIT_DECLSPEC wxDockWindowBase : public wxMiniFrame
{
    DECLARE_CLASS( wxDockWindowBase )
    DECLARE_EVENT_TABLE()

public:
    wxDockWindowBase() {
        Init();
    }
    ~wxDockWindowBase() {
    }
    void Init();

    wxDockWindowBase( wxWindow * parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, const wxString& name = wxT("dockwindow"), unsigned int flags = wxDWC_DEFAULT ) {
        Init();
        Create( parent, id, title, pos, size, name, flags );
    }

    // basic interface
    bool Create( wxWindow * parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, const wxString& name = wxT("dockwindow"), unsigned int flags = wxDWC_DEFAULT );

    void Appear();
    void Remove();

    void SetDockingManager( wxLayoutManager * pLayoutManager );
    wxLayoutManager * GetLayoutManager();

    void SetClient( wxWindow * pClient, bool autoPane = false );
    wxWindow * GetClient();
    wxWindow * RemoveClient( wxWindow * pNewParent = NULL );

    void AutoFitSingleChild();

    // access
    wxDockPanel * GetDockPanel();

    void SetDocked( bool state );
    bool IsDocked();

    void SetDockingInfo( HostInfo &hi );
    void ClearDockingInfo();
    HostInfo & GetDockingInfo();

    bool ActualShow( bool show = true );
    void DisableShowOverride();

    void RepeatLastMouseEvent();

    // overrides
    virtual bool Show( bool show = true );

    // platform
    virtual void StartDragging( int x, int y, bool needMouseCapture = true );
    virtual void StopDragging( bool needMouseRelease = true );
    virtual bool BlockDocking();

    // events
    void OnMouseMove( wxMouseEvent &e );
    void OnClose( wxCloseEvent &e );

protected:
    // misc
    void createClient();
    bool applyLastDock( bool noShowOperation = false );

protected:
    unsigned int flags_;
    bool disableShowOverride_;

    bool dragging_;
    bool haveMoved_;
    wxMouseEvent lastMouseEvent_;

    DockRect startRect_;    // starting rectangle
    DockRect prevRect_;     // erase rectangle
    DockRect dragRect_;     // draw rectangle

    wxPoint startPoint_;    // starting mouse x & y

    wxLayoutManager * pLayoutManager_;    // our layout manager

    wxDockPanel * pClientPanel_;      // the contents
    wxBoxSizer * pClientSizer_;       // the contents' sizer

    HostInfo newHost_;          // host upon dragging finish
    HostInfo prevHost_;         // last applied host

    bool docked_;   // are we docked
};

// ----------------------------------------------------------------------------
// include the platform-specific class declaration
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
    #include "wx/msw/dockwindow.h"
#elif defined(__WXGTK__)
	#include "wx/gtk/dockwindow.h"
#else
    #error "Your platform does not currently support wxDockWindow"
#endif

#endif
    // _WX_DOCKWINDOW_H_BASE_
