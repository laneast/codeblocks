#include "wxsgridsizer.h"

#include "wxsstdmanager.h"
#include "../wxspropertiesman.h"

#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/dcclient.h>

class wxsGridSizerPreview: public wxPanel
{
    public:
        wxsGridSizerPreview(wxWindow* Parent,wxsGridSizer* wxsSizer):
            wxPanel(Parent,-1,wxDefaultPosition,wxDefaultSize, wxTAB_TRAVERSAL),
            Sizer(new wxGridSizer(1)),
            sSizer(wxsSizer)
        {}
        
        
        wxGridSizer* Sizer;
        wxsGridSizer* sSizer;
        
        void UpdatePreview()
        {
            assert ( sSizer != NULL );
            wxGridSizer* NewSizer = new wxGridSizer(sSizer->Rows,sSizer->Cols,sSizer->VGap,sSizer->HGap);
            
            int Cnt = sSizer->GetChildCount();
            for ( int i=0; i<Cnt; i++ )
            {
                wxsWidget* Child = sSizer->GetChild(i);
                if ( !Child ) continue;
                
                const wxsWidgetBaseParams& Par = Child->GetBaseParams();

                int Flags = 0;
                
                if ( Par.BorderFlags & Par.Top )    Flags |= wxTOP;
                if ( Par.BorderFlags & Par.Bottom ) Flags |= wxBOTTOM;
                if ( Par.BorderFlags & Par.Left )   Flags |= wxLEFT;
                if ( Par.BorderFlags & Par.Right )  Flags |= wxRIGHT;
                
                switch ( Par.Placement )
                {
                    case wxsWidgetBaseParams::LeftTop:      Flags |= wxALIGN_LEFT | wxALIGN_TOP;  break;
                    case wxsWidgetBaseParams::LeftCenter:   Flags |= wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL; break;
                    case wxsWidgetBaseParams::LeftBottom:   Flags |= wxALIGN_LEFT | wxALIGN_BOTTOM; break;
                    case wxsWidgetBaseParams::CenterTop:    Flags |= wxALIGN_CENTER_HORIZONTAL | wxTOP; break;
                    case wxsWidgetBaseParams::Center:       Flags |= wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL; break;
                    case wxsWidgetBaseParams::CenterBottom: Flags |= wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM; break;
                    case wxsWidgetBaseParams::RightTop:     Flags |= wxALIGN_RIGHT | wxALIGN_TOP;  break;
                    case wxsWidgetBaseParams::RightCenter:  Flags |= wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL; break;
                    case wxsWidgetBaseParams::RightBottom:  Flags |= wxALIGN_RIGHT | wxALIGN_BOTTOM; break;
                }
                
                if ( Par.Expand ) Flags |= wxEXPAND;
                if ( Par.Shaped ) Flags |= wxSHAPED;

                NewSizer->Add(
                    Child->GetPreview(),
                    Par.Proportion,
                    Flags,
                    Par.Border
                );
            }
            SetSizer(NewSizer);
            NewSizer->SetSizeHints(this);
            Layout();
            SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
        }
        
//        virtual bool HasTransparentBackground() const { return true; }
    private:
    
        void OnPaint(wxPaintEvent& event)
        {
            wxPaintDC DC(this);
            int W, H;
            GetSize(&W,&H);
            DC.SetBrush(*wxTRANSPARENT_BRUSH);
            DC.SetPen(*wxRED_PEN);
            DC.DrawRectangle(0,0,W,H);
        }
        
        void OnClick(wxMouseEvent& event)
        {
            wxsPropertiesMan::Get()->SetActiveWidget(sSizer);
        }
        
        void OnEraseBack(wxEraseEvent& event)
        { }
        
        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxsGridSizerPreview,wxPanel)
    EVT_PAINT(wxsGridSizerPreview::OnPaint)
//    EVT_ERASE_BACKGROUND(wxsGridSizerPreview::OnEraseBack)
    EVT_LEFT_DOWN(wxsGridSizerPreview::OnClick)
END_EVENT_TABLE()


wxsGridSizer::wxsGridSizer(wxsWidgetManager* Man,wxsWindowRes* Res):
    wxsContainer(Man,Res,false,-1,propSizer),
    Cols(0),
    Rows(0),
    VGap(0),
    HGap(0)
{
}

wxsGridSizer::~wxsGridSizer()
{
}

const wxsWidgetInfo& wxsGridSizer::GetInfo()
{
    return *wxsStdManager.GetWidgetInfo(wxsGridSizerId);
}

wxString wxsGridSizer::GetProducingCode(wxsCodeParams& Params)
{
    static wxString Str;
    
    Str = wxString::Format(_T("%s = new wxGridSizer(1);"),
        BaseParams.VarName.c_str());
        
    return Str;
}

wxString wxsGridSizer::GetFinalizingCode(wxsCodeParams& Params)
{
    static wxString Str;

    if ( Params.IsDirectParent )
    {
        Str = wxString::Format(_T("%s->SetSizer(%s);"),
            Params.ParentName.c_str(),
            BaseParams.VarName.c_str());
        return Str;
    }
    
    return _T("");
}

/** This function should create preview window for widget */
wxWindow* wxsGridSizer::MyCreatePreview(wxWindow* Parent)
{
    return new wxsGridSizerPreview(Parent,this);
}

void wxsGridSizer::MyFinalUpdatePreview(wxWindow* Window)
{
    dynamic_cast<wxsGridSizerPreview*> (Window) -> UpdatePreview();
}

bool wxsGridSizer::MyXmlLoad()
{
    Rows = XmlGetInteger(_T("rows"));
    Cols = XmlGetInteger(_T("cols"));
    VGap = XmlGetInteger(_T("vgap"));
    HGap = XmlGetInteger(_T("hgap"));
    return true;
}

bool wxsGridSizer::MyXmlSave()
{
    XmlSetInteger(_T("rows"),Rows);
    XmlSetInteger(_T("cols"),Cols);
    XmlSetInteger(_T("vgap"),VGap);
    XmlSetInteger(_T("hgap"),HGap);
    return true;
}


void wxsGridSizer::CreateObjectProperties()
{
    wxsWidget::CreateObjectProperties();
    PropertiesObject.Add2IProperty(_("Cols x rows:"),Cols,Rows,0);
    PropertiesObject.Add2IProperty(_("VGap x HGap:"),VGap,HGap,1);
}

wxString wxsGridSizer::GetDeclarationCode(wxsCodeParams& Params)
{
	static wxString Temp;
	Temp.Printf(_T("wxGridSizer* %s"),GetBaseParams().VarName.c_str());
	return Temp;
}
