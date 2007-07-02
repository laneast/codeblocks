#ifndef WXSLISTCTRL_H
#define WXSLISTCTRL_H

#include "../wxswidget.h"

/** \brief Class for wxsListCtrl widget */
class wxsListCtrl: public wxsWidget
{
    public:

        wxsListCtrl(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);
};

#endif
