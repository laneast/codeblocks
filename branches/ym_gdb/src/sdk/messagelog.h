#ifndef MESSAGELOG_H
#define MESSAGELOG_H

#include "settings.h"
#include <wx/panel.h>
#include <wx/notebook.h>

/*
 * No description
 */
class DLLIMPORT MessageLog : public wxPanel
{
	public:
		// class destructor
		virtual ~MessageLog();

		virtual void AddLog(const wxString& msg, bool addNewLine = true) = 0;
		virtual int GetPageId() const { return m_PageId; }
    protected:
		// class constructor
		MessageLog();

    private:
        friend class MessageManager; // allow MessageManager to tamper with m_PageId
        int m_PageId;
};

#endif // MESSAGELOG_H