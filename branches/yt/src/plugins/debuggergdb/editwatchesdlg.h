#ifndef EDITWATCHESDLG_H
#define EDITWATCHESDLG_H

#include <wx/dialog.h>
#include "debugger_defs.h"

class EditWatchesDlg : public wxDialog
{
    public:
        EditWatchesDlg(WatchesArray& arr, wxWindow* parent = 0);
        virtual ~EditWatchesDlg();
    protected:
        void FillWatches();
        void FillRecord(int sel);
        void EndModal(int retCode);

        void OnAdd(wxCommandEvent& event);
        void OnRemove(wxCommandEvent& event);
        void OnListboxClick(wxCommandEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);

        int m_LastSel;
        WatchesArray& m_Watches;
    private:
        DECLARE_EVENT_TABLE()
};

#endif // EDITWATCHESDLG_H