/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef EXAMINEMEMORYDLG_H
#define EXAMINEMEMORYDLG_H

#include <wx/panel.h>

class wxTextCtrl;

class cbExamineMemoryDlg : public wxPanel
{
    public:
        cbExamineMemoryDlg(wxWindow* parent);

        // used for Freeze()/Thaw() calls
        void Begin();
        void End();

        void Clear();
        wxString GetBaseAddress();
        int GetBytes();
        void AddError(const wxString& err);
        void AddHexByte(const wxString& addr, const wxString& hexbyte);
    protected:
        void OnGo(wxCommandEvent& event);

    private:
        wxTextCtrl* m_pText;
        size_t m_ByteCounter;
        wxChar m_LineText[67]; // 16*3 "7F " + 3 "   " + 16 "."
        unsigned long m_LastRowStartingAddress;
    private:
        DECLARE_EVENT_TABLE()
};

#endif // EXAMINEMEMORYDLG_H
