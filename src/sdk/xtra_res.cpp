#include "sdk_precomp.h"
#include "xtra_res.h"
#include <wx/wx.h>

/////////////////////////////////////////////////////////////////////////////
// Name:        xh_toolb.cpp
// Purpose:     XRC resource for wxBoxSizer
// Author:      Vaclav Slavik
// Created:     2000/08/11
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
// Modified by Ricardo Garcia for Code::Blocks
// Comment: Things would've been much easier if field m_isInside had been
//          protected instead of private! >:(
/////////////////////////////////////////////////////////////////////////////

wxToolBarAddOnXmlHandler::wxToolBarAddOnXmlHandler()
: wxXmlResourceHandler(), m_isInside(FALSE), m_isAddon(false), m_toolbar(NULL)
{
    XRC_ADD_STYLE(wxTB_FLAT);
    XRC_ADD_STYLE(wxTB_DOCKABLE);
    XRC_ADD_STYLE(wxTB_VERTICAL);
    XRC_ADD_STYLE(wxTB_HORIZONTAL);
    XRC_ADD_STYLE(wxTB_3DBUTTONS);
    XRC_ADD_STYLE(wxTB_TEXT);
    XRC_ADD_STYLE(wxTB_NOICONS);
    XRC_ADD_STYLE(wxTB_NODIVIDER);
    XRC_ADD_STYLE(wxTB_NOALIGN);
}

wxObject *wxToolBarAddOnXmlHandler::DoCreateResource()
{
    wxToolBar* toolbar=NULL;
    if (m_class == _T("tool"))
    {
        wxCHECK_MSG(m_toolbar, NULL, _("Incorrect syntax of XRC resource: tool not within a toolbar!"));

        if (GetPosition() != wxDefaultPosition)
        {
            m_toolbar->AddTool(GetID(),
                               GetBitmap(_T("bitmap"), wxART_TOOLBAR),
                               GetBitmap(_T("bitmap2"), wxART_TOOLBAR),
                               GetBool(_T("toggle")),
                               GetPosition().x,
                               GetPosition().y,
                               NULL,
                               GetText(_T("tooltip")),
                               GetText(_T("longhelp")));
           if (GetBool(_T("disabled")))
           {
               m_toolbar->Realize();
               m_toolbar->EnableTool(GetID(),false);
           }
        }
        else
        {
            wxItemKind kind = wxITEM_NORMAL;
            if (GetBool(_T("radio")))
                kind = wxITEM_RADIO;
            if (GetBool(_T("toggle")))
            {
                wxASSERT_MSG( kind == wxITEM_NORMAL,
                              _("can't have both toggleable and radion button at once") );
                kind = wxITEM_CHECK;
            }
            m_toolbar->AddTool(GetID(),
                               GetText(_T("label")),
                               GetBitmap(_T("bitmap"), wxART_TOOLBAR),
                               GetBitmap(_T("bitmap2"), wxART_TOOLBAR),
                               kind,
                               GetText(_T("tooltip")),
                               GetText(_T("longhelp")));
           if (GetBool(_T("disabled")))
           {
               m_toolbar->Realize();
               m_toolbar->EnableTool(GetID(),false);
           }
        }
        return m_toolbar; // must return non-NULL
    }

    else if (m_class == _T("separator"))
    {
        wxCHECK_MSG(m_toolbar, NULL, _("Incorrect syntax of XRC resource: separator not within a toolbar!"));
        m_toolbar->AddSeparator();
        return m_toolbar; // must return non-NULL
    }

    else /*<object class="wxToolBar">*/
    {
        m_isAddon=(m_class == _T("wxToolBarAddOn"));
        if(m_isAddon)
        { // special case: Only add items to toolbar
          toolbar=(wxToolBar*)m_instance;
          // XRC_MAKE_INSTANCE(toolbar, wxToolBar);
        }
        else
        {
            int style = GetStyle(_T("style"), wxNO_BORDER | wxTB_HORIZONTAL);
            #ifdef __WXMSW__
            if (!(style & wxNO_BORDER)) style |= wxNO_BORDER;
            #endif

            XRC_MAKE_INSTANCE(toolbar, wxToolBar)

            toolbar->Create(m_parentAsWindow,
                             GetID(),
                             GetPosition(),
                             GetSize(),
                             style,
                             GetName());
            wxSize bmpsize = GetSize(_T("bitmapsize"));
            if (!(bmpsize == wxDefaultSize))
                toolbar->SetToolBitmapSize(bmpsize);
            wxSize margins = GetSize(_T("margins"));
            if (!(margins == wxDefaultSize))
                toolbar->SetMargins(margins.x, margins.y);
            long packing = GetLong(_T("packing"), -1);
            if (packing != -1)
                toolbar->SetToolPacking(packing);
            long separation = GetLong(_T("separation"), -1);
            if (separation != -1)
                toolbar->SetToolSeparation(separation);
        }

        wxXmlNode *children_node = GetParamNode(_T("object"));
        if (!children_node)
           children_node = GetParamNode(_T("object_ref"));

        if (children_node == NULL) return toolbar;

        m_isInside = TRUE;
        m_toolbar = toolbar;

        wxXmlNode *n = children_node;

        while (n)
        {
            if ((n->GetType() == wxXML_ELEMENT_NODE) &&
                (n->GetName() == _T("object") || n->GetName() == _T("object_ref")))
            {
                wxObject *created = CreateResFromNode(n, toolbar, NULL);
                wxControl *control = wxDynamicCast(created, wxControl);
                if (!IsOfClass(n, _T("tool")) &&
                    !IsOfClass(n, _T("separator")) &&
                    control != NULL)
                    toolbar->AddControl(control);
            }
            n = n->GetNext();
        }

        toolbar->Realize();

        m_isInside = FALSE;
        m_toolbar = NULL;

        if(!m_isAddon)
        {
            if (m_parentAsWindow && !GetBool(_T("dontattachtoframe")))
            {
                wxFrame *parentFrame = wxDynamicCast(m_parent, wxFrame);
                if (parentFrame)
                    parentFrame->SetToolBar(toolbar);
            }
        }
        m_isAddon=false;
        return toolbar;
    }
}

bool wxToolBarAddOnXmlHandler::CanHandle(wxXmlNode *node)
{
    return ((!m_isInside && IsOfClass(node, _T("wxToolBarAddOn"))) ||
            (m_isInside && IsOfClass(node, _T("tool"))) ||
            (m_isInside && IsOfClass(node, _T("separator"))));
}
