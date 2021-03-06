#ifndef WXSSTDMANAGER_H
#define WXSSTDMANAGER_H

#include "../widget.h"


enum std_widgets
{
    wxsNoneId = 0,
    
    /* Sizers */
    wxsGridSizerId,
    wxsBoxSizerId,
    wxsStaticBoxSizerId,
    wxsFlexGridSizerId,
    
    /* Spacer */
    wxsSpacerId,
    
    /* Controls */
    wxsButtonId,
    wxsToggleButtonId,      /* Warning - not compatible with XRC 2.4 */
    wxsCheckBoxId,
    wxsStaticTextId,
    wxsComboBoxId,
    wxsListBoxId,
    wxsPanelId,
    wxsTextCtrlId,
    wxsGaugeId,
    wxsRadioButtonId,
    wxsScrollBarId,
    wxsSpinButtonId,
    wxsSpinCtrlId,
    wxsTreeCtrlId,
    wxsRadioBoxId,
    wxsDatePickerCtrlId,
    wxsStaticLineId,
    wxsSplitterWindowId,
    wxsNotebookId,
    wxsListbookId,
    
    
    /* Windows */
    wxsDialogId,
    wxsFrameId,
    wxsPanelrId,
    
    /* Count */
    wxsStdIdCount
};

class wxsStdManagerT : public wxsWidgetManager
{
	public:
		wxsStdManagerT();
		virtual ~wxsStdManagerT();
		
		/** Initializing manager */
		virtual bool Initialize();
		
		/** Getting number of handled widgets */
        virtual int GetCount(); 
        
        /** Getting widget's info */
        virtual const wxsWidgetInfo* GetWidgetInfo(int Number);
        
        /** Getting new widget */
        virtual wxsWidget* ProduceWidget(int Id,wxsWindowRes* Res);
        
        /** Killing widget */
        virtual void KillWidget(wxsWidget* Widget);
};

extern wxsStdManagerT wxsStdManager;

#endif // WXSSTDMANAGER_H
