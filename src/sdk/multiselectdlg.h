#ifndef MULTISELECTDLG_H
#define MULTISELECTDLG_H

#include <wx/intl.h>
#include <wx/string.h>
#include <wx/dynarray.h>
#include <wx/dialog.h>

class MultiSelectDlg : public wxDialog
{
	public:
		MultiSelectDlg(wxWindow* parent,
                        const wxArrayString& items,
                        bool selectall = false,
                        const wxString& label = _("Select items:"),
                        const wxString& title = _("Multiple selection"));
		virtual ~MultiSelectDlg();
		
		wxArrayString GetSelectedStrings();
		wxArrayInt GetSelectedIndices();
	protected:
	private:
};

#endif // MULTISELECTDLG_H
