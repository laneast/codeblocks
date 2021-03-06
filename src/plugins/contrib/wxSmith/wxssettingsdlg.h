#ifndef WXSSETTINGSDLG_H
#define WXSSETTINGSDLG_H

//(*Headers(wxsSettingsDlg)
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <wx/xrc/xmlres.h>
//*)

class wxsSettingsDlg: public wxDialog
{
    public:

        wxsSettingsDlg(wxWindow* parent,wxWindowID id = -1);
        virtual ~wxsSettingsDlg();

        //(*Identifiers(wxsSettingsDlg)
        //*)

    protected:

        //(*Handlers(wxsSettingsDlg)
        void OnBtnOkClick(wxCommandEvent& event);
        void OnBtnCancelClick(wxCommandEvent& event);
        void OnDragTargetColClick(wxCommandEvent& event);
        void OnDragParentColClick(wxCommandEvent& event);
        void OnDragAssistTypeSelect(wxCommandEvent& event);
        //*)

        //(*Declarations(wxsSettingsDlg)
        wxComboBox* DragAssistType;
        wxButton* DragTargetCol;
        wxButton* DragParentCol;
        wxSpinCtrl* PrevFetchDelay;
        wxButton* BtnCancel;
        //*)

    private:

        DECLARE_EVENT_TABLE()
};

#endif
